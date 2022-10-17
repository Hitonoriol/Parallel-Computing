#include "pthread_wrapper.hpp"

#include <iostream>

std::shared_ptr<pthread_t> run_pthread(void* (*func)(void*), void *arg)
{
	auto thread = std::make_shared<pthread_t>();
	int rc = pthread_create(thread.get(), NULL, func, arg);
	if (rc) {
		std::cerr << "ERROR: pthread_create() returned " << rc << std::endl;
		exit(-1);
	}
	return thread;
}
