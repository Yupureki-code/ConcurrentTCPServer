#include "http_test_common.hpp"

int main()
{
    try
    {
        constexpr int thread_count = 20;
        std::vector<std::thread> workers;
        std::vector<std::string> failures;
        std::mutex failure_mutex;

        for(int i = 0; i < thread_count; ++i)
        {
            workers.emplace_back([&]() {
                try
                {
                    http_test::Client client;
                    auto response = client.request(http_test::basic_get("/", "close"));
                    http_test::require(response.status == 200, "parallel request status != 200");
                    http_test::require(!response.body.empty(), "parallel request body empty");
                }
                catch(const std::exception& ex)
                {
                    std::lock_guard<std::mutex> lock(failure_mutex);
                    failures.emplace_back(ex.what());
                }
            });
        }

        for(auto& worker : workers)
            worker.join();

        if(!failures.empty())
            http_test::fail(failures.front());
        std::cout << "[PASS] parallel requests" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] parallel requests: " << ex.what() << std::endl;
        return 1;
    }
}