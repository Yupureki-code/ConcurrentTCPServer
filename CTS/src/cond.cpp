#include "../include/cond.h"

#include <cstdlib>
#include <iostream>

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

mycond::mycond()
{
    int n = pthread_cond_init(&_cond, nullptr);
    if (n != 0)
    {
        ERR_EXIT("cond init");
    }
}

void mycond::signal()
{
    pthread_cond_signal(&_cond);
}

void mycond::wait(pthread_mutex_t* lock)
{
    pthread_cond_wait(&_cond, lock);
}

void mycond::broadcast()
{
    pthread_cond_broadcast(&_cond);
}

pthread_cond_t* mycond::get_cond()
{
    return &_cond;
}

mycond::~mycond()
{
    pthread_cond_destroy(&_cond);
}