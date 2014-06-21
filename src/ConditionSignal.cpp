#include "Check.h"
#include "ConditionSignal.h"
#include "errno.h"
#include <time.h>

ConditionSignal::ConditionSignal() {
  Check(pthread_cond_init(&internalCond, NULL) == 0, "pthread_cond_init_failed");
}

ConditionSignal::~ConditionSignal() {
  Check(pthread_cond_destroy(&internalCond) == 0, "pthread_cond_destroy failed");
}

void ConditionSignal::lock() {
  internalMutex.lock();
}

void ConditionSignal::unlock() {
  internalMutex.unlock();
}

Mutex *ConditionSignal::mutex() {
  return &internalMutex;
}

void ConditionSignal::signal() {
  Check(pthread_cond_broadcast(&internalCond) == 0, "pthread_cond_broadcast failed");
}

void ConditionSignal::wait() {
  Check(pthread_cond_wait(&internalCond, internalMutex.getInternalMutex()) == 0, "pthread_cond_wait failed");
}

void ConditionSignal::wait(uint32_t timeoutMs) {
  // slightly ridiculous
  struct timeval current;
  struct timeval timeout;
  struct timeval timeoutAbsolute;
  struct timespec timespec;
  gettimeofday(&current, NULL);
  timeout.tv_sec = timeoutMs / 1000;
  timeout.tv_usec = (timeoutMs % 1000) * 1000;
  timeradd(&current, &timeout, &timeoutAbsolute);
  timespec.tv_sec = timeoutAbsolute.tv_sec;
  timespec.tv_nsec = timeoutAbsolute.tv_usec * 1000;

  int ret = pthread_cond_timedwait(&internalCond, internalMutex.getInternalMutex(), &timespec);
  Check(!ret || ret == ETIMEDOUT, "pthread_cond_wait failed");
}

