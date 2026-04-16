#include "http_test_common.hpp"

int main()
{
    try
    {
        http_test::Client client;
        client.set_timeout(8);
        client.send_all("POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 1000\r\nConnection: keep-alive\r\n\r\nhelloworldhello");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto response = client.read_response();
        http_test::require(response.status == 408, "incomplete body did not return 408");
        http_test::require(response.headers.count("Connection") != 0, "timeout response missing Connection header");
        http_test::require(response.headers["Connection"] == "close", "timeout response did not close connection");
        http_test::require(!response.body.empty(), "timeout response body empty");
        std::cout << "[PASS] request timeout" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] request timeout: " << ex.what() << std::endl;
        return 1;
    }
}