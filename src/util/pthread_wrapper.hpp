#ifndef UTIL_PTHREAD_WRAPPER_HPP_
#define UTIL_PTHREAD_WRAPPER_HPP_

#include <memory>

std::shared_ptr<pthread_t> run_pthread(void* (*func)(void*), void *arg = NULL);


#endif /* UTIL_PTHREAD_WRAPPER_HPP_ */
