#ifndef THREAD_H
#define THREAD_H

#include "Mutex.h"
#include <pthread.h>
#include "SharedFlag.h"
#include <string>
#include <vector>

class PerfDebug;

class Thread {

public:
  Thread(std::string name);
  virtual ~Thread();

  static void setPerfDebugger(PerfDebug *newPerfDebugger);
  static std::vector<Thread *> getAllThreads();

  std::string getName();
  int start();
  int join();
  int detach();
  void requestStop() { stopRequested.set(true); }
  bool isStopRequested() { return stopRequested.get(); }
  pthread_t self();
  void debugPerf(std::string msg);

  virtual void run() = 0;

private:
  static std::vector<Thread *> allThreads;
  static Mutex allThreadsMutex;
  static PerfDebug *perfDebugger;
  static void registerThread(Thread *thread);
  pthread_t handle;
  bool running;
  bool detached;
  SharedFlag stopRequested;
  std::string name;
};

#include "PerfDebug.h"

#endif // THREAD_H
