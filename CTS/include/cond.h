#pragma once

#include <pthread.h>

class mycond
{
public:
    mycond();
    void signal();
    void wait(pthread_mutex_t* lock);
    void broadcast();
    pthread_cond_t* get_cond();
    ~mycond();

private:
    pthread_cond_t _cond;
};