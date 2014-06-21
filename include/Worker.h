#ifndef WORKER_H
#define WORKER_H

#include "sp.h"
#include "Thread.h"
#include "WorkQueue.h"

template<typename T>
class Worker : public Thread {
public:
  Worker() {
    start();
  }

  virtual ~Worker() {
    stop();
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

  uint32_t queueSize() {
    return workQueue.queueSize();
  }

  void setSkipToLatest(bool value) {
    workQueue.setSkipToLatest(value);
  }
private:
  virtual void doWork(sp<T> work) = 0;
  WorkQueue<T> workQueue;
};

#endif
