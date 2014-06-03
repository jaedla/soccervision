#include "Check.h"
#include "ScopedMutex.h"
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

