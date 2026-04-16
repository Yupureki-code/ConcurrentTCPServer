#include "../src/httpserver.hpp"

#include <chrono>
#include <thread>

#ifndef HTTP_SERVER_TEST_PORT
#define HTTP_SERVER_TEST_PORT 18080
#endif

#ifndef HTTP_SERVER_WWWROOT
#define HTTP_SERVER_WWWROOT "../src/wwwroot"
#endif

const std::string HTML_PATH = HTTP_SERVER_WWWROOT;

int main()
{
    HttpServer server(HTTP_SERVER_TEST_PORT, 8, 3);
    server.EnableInactiveRelease(5);
    server.SetBaseDir(HTML_PATH);
    server.Get("/", [](const HttpRequest& req,HttpResponse& rep){
        std::ifstream in( HTML_PATH +  std::string("/index.html"));
        std::string html((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();
        rep.SetHeader("Content-Type", "text/html;charset=utf-8");
        rep._body = html;
    });
    server.Get("/slow", [](const HttpRequest&,HttpResponse& rep){
        std::this_thread::sleep_for(std::chrono::seconds(4));
        rep.SetHeader("Content-Type", "text/plain;charset=utf-8");
        rep._body = "slow response";
    });
    server.Listen();
    return 0;
}