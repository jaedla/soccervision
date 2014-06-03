#include "Check.h"
#include "Mutex.h"

Mutex::Mutex() : locked(false) {
  Check(pthread_mutex_init(&mutex, NULL) == 0, "pthread_mutex_init failed");
}

Mutex::~Mutex() {
  Check(pthread_mutex_destroy(&mutex) == 0, "pthread_mutex_destroy failed");
}

void Mutex::lock() {
  Check(pthread_mutex_lock(&mutex) == 0, "pthread_mutex_lock failed");
  locked = true;
  lockOwner = pthread_self();
}

void Mutex::unlock() {
  locked = false;
  Check(pthread_mutex_unlock(&mutex) == 0, "pthread_mutex_unlock failed");
}

pthread_mutex_t *Mutex::getInternalMutex() {
  return &mutex;
}

