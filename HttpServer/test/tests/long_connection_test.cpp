#include "http_test_common.hpp"

int main()
{
    try
    {
        http_test::Client client;
        auto first = client.request(http_test::basic_get("/", "keep-alive"));
        http_test::require(first.status == 200, "first response status != 200");
        http_test::require(http_test::trim(first.headers["Connection"]) == "keep-alive", "first response not keep-alive");
        http_test::require(!first.body.empty(), "first response body empty");

        auto second = client.request(http_test::basic_get("/", "keep-alive"));
        http_test::require(second.status == 200, "second response status != 200");
        http_test::require(http_test::trim(second.headers["Connection"]) == "keep-alive", "second response not keep-alive");
        http_test::require(!second.body.empty(), "second response body empty");

        std::cout << "[PASS] long connection" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] long connection: " << ex.what() << std::endl;
        return 1;
    }
}