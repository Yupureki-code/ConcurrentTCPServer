#include "http_test_common.hpp"

int main()
{
    try
    {
        http_test::Client client;
        auto response = client.request(http_test::bad_request());
        http_test::require(response.status == 400, "bad request did not return 400");
        http_test::require(!response.body.empty(), "bad request body empty");
        std::cout << "[PASS] bad request" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] bad request: " << ex.what() << std::endl;
        return 1;
    }
}