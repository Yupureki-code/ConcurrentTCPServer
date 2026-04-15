#include "http_test_common.hpp"

int main()
{
    try
    {
        http_test::create_large_fixture();
        struct Cleanup
        {
            ~Cleanup()
            {
                http_test::remove_large_fixture();
            }
        } cleanup;

        http_test::Client client;
        auto response = client.request(http_test::basic_get("/large.bin", "close"));
        http_test::require(response.status == 200, "large file status != 200");
        http_test::require(response.body.size() == 2 * 1024 * 1024, "large file size mismatch");
        http_test::require(response.body.find_first_not_of('A') == std::string::npos, "large file contents corrupted");
        std::cout << "[PASS] large file" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] large file: " << ex.what() << std::endl;
        return 1;
    }
}