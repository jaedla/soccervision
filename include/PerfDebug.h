#ifndef PERFDEBUG_H
#define PERFDEBUG_H

#include "Thread.h"
#include <map>
#include "Mutex.h"
#include <string>
#include <vector>

class PerfDebug : public Thread {
public:
  PerfDebug();
  ~PerfDebug();
  virtual void run();
  void addDebuggedThread(Thread *debuggedThread);
  void debugMessage(Thread *debuggedThread, std::string message);
private:
  void getAllThreads(std::vector<Thread *> &threads);
  void printInfo();
  Mutex mutex;
  std::map<Thread *, std::string> debugMessages;
};

#endif
