#include "http_test_common.hpp"

int main()
{
    try
    {
        http_test::Client client;
        client.set_timeout(12);
        std::this_thread::sleep_for(std::chrono::seconds(7));
        char buffer[1];
        int n = client.recv_one(buffer, sizeof(buffer));
        http_test::require(n == 0 || n < 0, "connection was not closed by idle timeout");
        std::cout << "[PASS] timeout connection" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] timeout connection: " << ex.what() << std::endl;
        return 1;
    }
}