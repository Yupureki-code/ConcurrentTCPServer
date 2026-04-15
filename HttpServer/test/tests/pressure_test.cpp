#include "http_test_common.hpp"

int main()
{
    try
    {
        constexpr int concurrency = 20;
        constexpr int total_requests = 200;

        std::atomic<int> completed{0};
        std::atomic<int> failed{0};
        std::vector<std::thread> workers;
        workers.reserve(concurrency);

        auto per_thread = total_requests / concurrency;
        auto remainder = total_requests % concurrency;

        for(int i = 0; i < concurrency; ++i)
        {
            int requests = per_thread + (i < remainder ? 1 : 0);
            workers.emplace_back([requests, &completed, &failed]() {
                for(int j = 0; j < requests; ++j)
                {
                    try
                    {
                        http_test::Client client;
                        auto response = client.request(http_test::basic_get("/", "close"));
                        if(response.status == 200 && !response.body.empty())
                            ++completed;
                        else
                            ++failed;
                    }
                    catch(...)
                    {
                        ++failed;
                    }
                }
            });
        }

        for(auto& worker : workers)
            worker.join();

        http_test::require(failed.load() == 0, "pressure test had failures");
        http_test::require(completed.load() == total_requests, "pressure test request count mismatch");
        std::cout << "[PASS] pressure" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[FAIL] pressure: " << ex.what() << std::endl;
        return 1;
    }
}