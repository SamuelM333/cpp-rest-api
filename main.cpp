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

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::type;

// Print document parts to standard output.
void print_document(const bsoncxx::document::view& doc) {
    // Extract _id element as a string.
    bsoncxx::document::element id_ele = doc["_id"];
    if (id_ele.type() == type::k_oid) {
        std::string oid = id_ele.get_oid().value.to_string();
        std::cout << "OID: " << oid << std::endl;
    } else {
        std::cout << "Error: _id was not an object ID." << std::endl;
    }

}

namespace Generic {

    void handleReady(const Rest::Request &, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

}

class StatsEndpoint {
public:
    StatsEndpoint(Address addr) : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

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
    void setupRoutes() {
        using namespace Rest;

        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/db", Routes::bind(&StatsEndpoint::doDB, this));

    }

    void doDB(const Rest::Request &request, Http::ResponseWriter response) {

        bsoncxx::builder::stream::document document{};

        auto collection = mongo_conn["metal_api"]["artists"];
        auto cursor = collection.find({});

        for (auto &&doc : cursor) {

            bsoncxx::document::element name = doc["name"];
//            cout << to_string(name.type()) << endl;
            cout << name.get_utf8().value << endl;

//            bsoncxx::document::element id_ele = doc["_id"];
//            if (id_ele.type() == type::k_oid) {
//                std::string oid = id_ele.get_oid().value.to_string();
//                std::cout << "OID: " << oid << std::endl;
//            } else {
//                std::cout << "Error: _id was not an object ID." << std::endl;
//            }
        }

        const char *json = "{\"results\": []}";
        Document json_response;
        json_response.Parse(json);

        Value &results = json_response["results"];

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        json_response.Accept(writer);

        std::cout << buffer.GetString() << std::endl;

        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        response.send(Http::Code::Ok, buffer.GetString());
    }

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
    mongocxx::client mongo_conn;

};

int main(int argc, char *argv[]) {
    Port port(9080);

    int thr = 2;

    if (argc >= 2) {
        port = std::stol(argv[1]);

        if (argc == 3) thr = std::stol(argv[2]);
    }

    Address addr(Ipv4::any(), port);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;
    cout << "Starting server in localhost:" << port << endl;

    StatsEndpoint stats(addr);

    stats.init(thr);
    stats.start();

    stats.shutdown();
}