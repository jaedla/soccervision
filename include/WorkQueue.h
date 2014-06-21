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
    return getWork();
  }

  sp<T> waitForWork(uint32_t timeoutMs) {
    Timer timer;
    timer.setTarget(timeoutMs);
    ScopedMutex lock(signal.mutex());
    while (queue.empty() && !timer.isExpired())
      signal.wait(timer.remaining());
    return queue.empty() ? NULL : getWork();
  }

  uint32_t queueSize() {
    ScopedMutex lock(signal.mutex());
    return queue.size(); 
  }

  void setSkipToLatest(bool value) {
    skipToLatest.set(value);
  }
private:
  sp<T> getWork() {
    if (skipToLatest.is(true)) {
      while (queue.size() > 1)
        queue.pop();
    }
    sp<T> work = queue.front();
    queue.pop();
    return work;
  }

  ConditionSignal signal;
  std::queue<sp<T>> queue;
  SharedFlag skipToLatest;
};

#endif
