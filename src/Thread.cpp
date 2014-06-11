#include "Thread.h"
#include "ScopedMutex.h"
#include <stdio.h>
#include <string>

std::vector<Thread *> Thread::allThreads;
Mutex Thread::allThreadsMutex;
PerfDebug *Thread::perfDebugger = NULL;

static void *runThread(void *arg) {
  ((Thread *)arg)->run();
  return NULL;
}

void Thread::registerThread(Thread *thread) {
  ScopedMutex lock(&allThreadsMutex);
  allThreads.push_back(thread);
}

std::vector<Thread *> Thread::getAllThreads() {
  ScopedMutex lock(&allThreadsMutex);
  return allThreads;
}

void Thread::setPerfDebugger(PerfDebug *newPerfDebugger) {
  perfDebugger = newPerfDebugger;
}

Thread::Thread(std::string name) :
    running(0),
    detached(0),
    name(name) {
  registerThread(this);
}

Thread::~Thread() {
  if (running) {
    if (!detached)
      pthread_detach(handle);
    printf("Warning: pthread_cancel not implemented\n");
    //pthread_cancel(handle);
  }
}

std::string Thread::getName() {
  return name;
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

void Thread::debugPerf(std::string msg) {
  if (perfDebugger)
    perfDebugger->debugMessage(this, msg);
}
