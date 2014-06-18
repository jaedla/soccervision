#ifndef WAITABLEFLAG_H
#define WAITABLEFLAG_H

#include "ConditionSignal.h"
#include <pthread.h>

class WaitableFlag {
public:
  WaitableFlag();
  ~WaitableFlag();
  void set(bool newFlag);
  void waitUntil(bool untilFlag);
  bool waitUntil(bool untilFlag, uint32_t timeoutMs);
private:
  ConditionSignal signal;
  bool flag;
};

#endif
