#ifndef MUTEX_H
#define MUTEX_H

class ConditionSignal;

#include <pthread.h>

class Mutex {
public:
  Mutex();
  ~Mutex();
  void lock();
  void unlock();
private:
  friend class ConditionSignal;
  pthread_mutex_t *getInternalMutex();
  pthread_mutex_t mutex;
  pthread_t lockOwner;
  bool locked;
};

#endif
