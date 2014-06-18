#include "Check.h"
#include "ScopedMutex.h"
#include "Timer.h"
#include "WaitableFlag.h"

WaitableFlag::WaitableFlag() : flag(false) {
}

WaitableFlag::~WaitableFlag() {
}

void WaitableFlag::set(bool newFlag) {
  ScopedMutex lock(signal.mutex());
  flag = newFlag;
  signal.signal();
}

void WaitableFlag::waitUntil(bool untilFlag) {
  ScopedMutex lock(signal.mutex());
  while (flag != untilFlag)
    signal.wait();
}

bool WaitableFlag::waitUntil(bool untilFlag, uint32_t timeoutMs) {
  Timer timer;
  timer.setTarget(timeoutMs);
  ScopedMutex lock(signal.mutex());
  while (flag != untilFlag && !timer.isExpired())
    signal.wait(timer.remaining());
  return flag == untilFlag;
}

