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


class MetalAPI {
public:
    MetalAPI(Address addr) : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
                .threads(thr)
                .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();

        const char *uri_string = "mongodb://root:example@localhost:27017";
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
        Routes::Get(router, "/db", Routes::bind(&MetalAPI::doDB, this));

    }

    void handleReady(const Rest::Request &, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

    void doDB(const Rest::Request &request, Http::ResponseWriter response) {
        Document json_response;
        json_response.SetObject();
        Value results(kArrayType);
        Document::AllocatorType& allocator = json_response.GetAllocator();
        auto collection = mongo_conn["metal_api"]["artists"];
        auto order = document{} << "name" << 1 << finalize;
        auto opts = mongocxx::options::find{};
        opts.sort(order.view());

        auto cursor = collection.find({}, opts);

        for (auto &&doc : cursor) {
            string band_name;
            Document band;
            bsoncxx::document::element name = doc["name"];

            band_name = name.get_utf8().value.to_string();
            band.SetObject();
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