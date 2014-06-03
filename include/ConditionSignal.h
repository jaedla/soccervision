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
  Mutex *mutex();
private:
  Mutex internalMutex;
  pthread_cond_t internalCond;
};

#endif
