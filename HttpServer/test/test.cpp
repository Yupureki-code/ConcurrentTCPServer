#include "../src/httpserver.hpp"

#include <chrono>
#include <thread>

const std::string HTML_PATH = "../src/wwwroot";

int main()
{
    HttpServer server(8080, 8, 3);
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