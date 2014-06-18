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
  Check(pthread_cond_broadcast(&internalCond) == 0, "pthread_cond_broadcast failed");
}

void ConditionSignal::wait() {
  waitInternal(false, 0);
}

void ConditionSignal::wait(uint32_t timeoutMs) {
  waitInternal(true, timeoutMs);
}

void ConditionSignal::waitInternal(bool withTimeout, uint32_t timeoutMs) {
  Check(pthread_cond_wait(&internalCond, internalMutex.getInternalMutex()) == 0, "pthread_cond_wait failed");
}

