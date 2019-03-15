#include <algorithm>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;
using namespace Pistache;
using namespace rapidjson;
using namespace bsoncxx::builder::stream;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::type;

// TODO Split class in a separated file
// TODO Make singleton
class MetalAPI {
public:
    MetalAPI(Address addr) : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
                .threads(thr)
                .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();

        // TODO Get user and pass from env
        cout << "Connecting to mongo server" << endl;
        const char *uri_string = "mongodb://root:example@mongo:27017";
        mongocxx::instance inst{};
        mongo_conn = {mongocxx::uri{uri_string}};
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() {
        httpEndpoint->shutdown();
    }

private:
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
    mongocxx::client mongo_conn;

    void setupRoutes() {
        using namespace Rest;

        Routes::Get(router, "/ready", Routes::bind(&MetalAPI::handleReady, this));
        Routes::Get(router, "/artist", Routes::bind(&MetalAPI::retrieve_artists, this));
        Routes::Post(router, "/artist", Routes::bind(&MetalAPI::create_artist, this));
    }

    void handleReady(const Rest::Request &, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

    void retrieve_artists(const Rest::Request &request, Http::ResponseWriter response) {
        Document json_response;
        json_response.SetObject();
        Value results(kArrayType);
        cout << "Querying metal_api.artists" << endl;
        Document::AllocatorType &allocator = json_response.GetAllocator();
        auto collection = mongo_conn["metal_api"]["artists"];
        auto order = document{} << "name" << 1 << finalize;
        auto opts = mongocxx::options::find{};
        opts.sort(order.view());

        auto cursor = collection.find({}, opts);

        for (auto &&doc : cursor) {
            Document band;
            bsoncxx::document::element _id_element = doc["_id"];
            bsoncxx::document::element name_element = doc["name"];

            const string _id = _id_element.get_oid().value.to_string();
            const string band_name = name_element.get_utf8().value.to_string();
            band.SetObject();
            band.AddMember("_id", _id, allocator);
            band.AddMember("name", band_name, allocator);
            results.PushBack(band, allocator);
        }

        json_response.AddMember("results", results, allocator);

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        json_response.Accept(writer);

        cout << buffer.GetString() << endl;

        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        response.send(Http::Code::Ok, buffer.GetString());
    }

    void create_artist(const Rest::Request &request, Http::ResponseWriter response) {
        Document payload, json_response;
        json_response.SetObject();
        payload.Parse(request.body());

        Document::AllocatorType &allocator = json_response.GetAllocator();

        Value &name = payload["name"];
        cout << name.GetString() << endl;
        mongocxx::collection artists = mongo_conn["metal_api"]["artists"];
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value doc_value = builder
                << "name" << name.GetString()
                << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view view = doc_value.view();
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = artists.insert_one(view);
        const bsoncxx::types::value& result_id = result->inserted_id();
        const auto _id = result_id.get_oid().value.to_string();

        json_response.AddMember("_id", _id, allocator);
        json_response.AddMember("name", (string) name.GetString(), allocator);

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        json_response.Accept(writer);

        cout << buffer.GetString() << endl;
        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        response.send(Http::Code::Ok, buffer.GetString());
    }

};

int main(int argc, char *argv[]) {
    Port port(9080);
    unsigned int const threads = 2;
    Address addr(Ipv4::any(), port);
    MetalAPI stats(addr);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << threads << " threads" << endl;
    cout << "Starting server in localhost:" << port << endl;

    stats.init(threads);
    stats.start();
    stats.shutdown();
}