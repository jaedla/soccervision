#ifndef CONDITIONSIGNAL_H
#define CONDITIONSIGNAL_H

#include "Mutex.h"

class ConditionSignal {
public:
  ConditionSignal();
  ~ConditionSignal();
  void lock();
  void unlock();
  void signal();
  void wait();
  void wait(uint32_t timeoutMs);
  Mutex *mutex();
private:
  void waitInternal(bool withTimeout, uint32_t timeoutMs);
  Mutex internalMutex;
  pthread_cond_t internalCond;
};

#endif
