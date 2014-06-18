#ifndef WORKER_H
#define WORKER_H

#include "sp.h"
#include "Thread.h"
#include "WorkQueue.h"

template<typename T>
class Worker : public Thread {
public:
  virtual ~Worker() {
  }

  void addWork(sp<T> work) {
    workQueue.addWork(work);
  }

  virtual void run() {
    while (!isStopRequested()) {
      sp<T> work = workQueue.waitForWork(100);
      if (work)
        doWork(work);
    }
  }
private:
  virtual void doWork(sp<T> work) = 0;
  WorkQueue<T> workQueue;
};

#endif
