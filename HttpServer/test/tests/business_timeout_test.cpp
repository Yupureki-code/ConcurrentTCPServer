#include "http_test_common.hpp"

int main()
{
    try
    {
        http_test::Client client;
        auto start = std::chrono::steady_clock::now();
        auto response = client.request(http_test::basic_get("/slow", "close"));
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();
        http_test::require(response.status == 504, "slow route did not return 504");
        http_test::require(elapsed >= 3 && elapsed < 8, "slow route latency out of range");
        http_test::require(!response.body.empty(), "slow route body empty");
        std::cout << "[PASS] business timeout" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] business timeout: " << ex.what() << std::endl;
        return 1;
    }
}