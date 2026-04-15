#include "../include/lock.h"

#include <cstdlib>

mylock::mylock()
{
    int n = pthread_mutex_init(&_lock, nullptr);
    if (n != 0)
        exit(1);
}

int mylock::lock()
{
    return pthread_mutex_lock(&_lock);
}

int mylock::unlock()
{
    return pthread_mutex_unlock(&_lock);
}

pthread_mutex_t* mylock::get_lock()
{
    return &_lock;
}

mylock::~mylock()
{
    pthread_mutex_destroy(&_lock);
}