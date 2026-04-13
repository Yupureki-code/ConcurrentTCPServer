#pragma once
#include <iostream>
#include <pthread.h>

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)
    
class mycond{
public:
    mycond()
    {
        int n = pthread_cond_init(&_cond,nullptr);
        if(n != 0)
        {
            ERR_EXIT("cond init");
            return;
        }
        //std::cout<<"cond init success!"<<std::endl;
    }
    void signal()
    {
        pthread_cond_signal(&_cond);
    }
    void wait(pthread_mutex_t* lock)
    {
        pthread_cond_wait(&_cond,lock);
    }
    void broadcast()
    {
        pthread_cond_broadcast(&_cond);
    }
    pthread_cond_t* get_cond()
    {
        return &_cond;
    }
    ~mycond()
    {
        pthread_cond_destroy(&_cond);
    }
private:
    pthread_cond_t _cond;
};