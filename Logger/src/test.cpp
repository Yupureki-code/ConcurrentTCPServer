#include "../logstrategy.h"

int main()
{
    ns_log::Logger::GetInstance().DisableLogLevel(ns_log::INFO);
    logger(ns_log::INFO)<<"test";
    return 0;
}