#ifndef SCOPEDMUTEX_H
#define SCOPEDMUTEX_H

#include "Mutex.h"

class ScopedMutex {
public:
  ScopedMutex(Mutex *mutex);
  ~ScopedMutex();
private:
  Mutex *mutex;
};

#endif
