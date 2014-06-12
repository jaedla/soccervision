#include <algorithm>
#include "Check.h"
#include "PerfDebug.h"
#include "ScopedMutex.h"
#include "Util.h"

PerfDebug::PerfDebug() : Thread("PerfDebug") {
  setPerfDebugger(this);
}

PerfDebug::~PerfDebug() {
}

void PerfDebug::run() {
  while (!isStopRequested()) {
    //printInfo();
    printf(".\n");
    Util::sleep(10);
  }
}

void PerfDebug::debugMessage(Thread *debuggedThread, std::string message) {
  ScopedMutex lock(&mutex);
  debugMessages[debuggedThread] = message;
}

void PerfDebug::getAllThreads(std::vector<Thread *> &threads) {
  threads = Thread::getAllThreads();
  auto it = std::find(threads.begin(), threads.end(), this);
  if (it != threads.end())
    threads.erase(it);
}

void PerfDebug::printInfo() {
  ScopedMutex lock(&mutex);
  std::vector<Thread *> allThreads;
  getAllThreads(allThreads);
  unsigned spacePerThread = 23;
  char *line = new char[spacePerThread * allThreads.size() + 1];
  char *printfSpot = line;
  for (size_t i = 0; i < allThreads.size(); ++i) {
    Thread *thread = allThreads[i];
    const char *msg = debugMessages[thread].c_str();
    if (msg[0]) {
      snprintf(printfSpot, spacePerThread, "%s:%s", thread->getName().c_str(), msg);
    } else {
      sprintf(printfSpot, ".");
    }
    unsigned written = strlen(printfSpot);
    Check(written < spacePerThread, "written < spacePerThread failed");
    memset(printfSpot + written, ' ', spacePerThread - written);
    printfSpot += spacePerThread;
  }
  printfSpot[0] = '\0';
  printf("%s\n", line);
  delete line;
  debugMessages.clear();
}

