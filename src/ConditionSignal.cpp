#include "Check.h"
#include "ConditionSignal.h"

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
  // must hold the lock
  Check(pthread_cond_broadcast(&internalCond) == 0, "pthread_cond_broadcast failed");
}

void ConditionSignal::wait() {
  // must hold the lock
  Check(pthread_cond_wait(&internalCond, internalMutex.getInternalMutex()) == 0, "pthread_cond_wait failed");
}

