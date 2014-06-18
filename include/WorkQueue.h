#ifndef WORKQUEUE_H
#define WORKQUEUE_H

#include "ConditionSignal.h"
#include <queue>
#include "ScopedMutex.h"
#include "sp.h"
#include "Timer.h"

template<typename T>
class WorkQueue {
public:
  void addWork(sp<T> work) {
    ScopedMutex lock(signal.mutex());
    queue.push(work);
    signal.signal();
  }

  sp<T> waitForWork() {
    ScopedMutex lock(signal.mutex());
    while (queue.empty())
      signal.wait();
    return getNextWork();
  }

  sp<T> waitForWork(uint32_t timeoutMs) {
    Timer timer;
    timer.setTarget(timeoutMs);
    ScopedMutex lock(signal.mutex());
    while (queue.empty() && !timer.isExpired())
      signal.wait(timer.remaining());
    return queue.empty() ? NULL : getNextWork();
  }
private:
  sp<T> getNextWork() {
    sp<T> work = queue.front();
    queue.pop();
    return work;
  }

  ConditionSignal signal;
  std::queue<sp<T>> queue;
};

#endif
