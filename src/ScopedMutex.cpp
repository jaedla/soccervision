#include "ScopedMutex.h"

ScopedMutex::ScopedMutex(Mutex *mutex) : mutex(mutex) {
  mutex->lock();
}

ScopedMutex::~ScopedMutex() {
  mutex->unlock();
}

