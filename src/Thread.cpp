#include "Thread.h"
#include <stdio.h>
#include <string>

static void *runThread(void *arg) {
  return ((Thread *)arg)->run();
}

Thread::Thread(std::string name) : running(0), detached(0), name(name) {}

Thread::~Thread() {
  if (running) {
    if (!detached)
      pthread_detach(handle);
    printf("Warning: pthread_cancel not implemented\n");
    //pthread_cancel(handle);
  }
}

int Thread::start() {
  int result = pthread_create(&handle, NULL, runThread, this);

  if (result == 0) {
    pthread_setname_np(handle, name.c_str());
    running = true;
  }

  return result;
}

int Thread::join() {
  int result = -1;

  if (running) {
    result = pthread_join(handle, NULL);

    if (result == 0)
      detached = false;
  }

  return result;
}

int Thread::detach() {
  int result = -1;

  if (running && !detached) {
    result = pthread_detach(handle);

    if (result == 0)
      detached = 1;
  }

  return result;
}

pthread_t Thread::self() {
  return handle;
}
