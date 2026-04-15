#pragma once

#include <pthread.h>

class mylock
{
public:
	mylock();
	int lock();
	int unlock();
	pthread_mutex_t* get_lock();
	~mylock();

private:
	pthread_mutex_t _lock;
};
