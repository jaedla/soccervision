#ifndef SHAREDFLAG_H
#define SHAREDFLAG_H

#include "Mutex.h"

class SharedFlag {
public:
  SharedFlag();
  SharedFlag(bool value);
  ~SharedFlag();
  bool get();
  void set(bool newValue);
  bool is(bool value);
private:
  bool flag;
  Mutex mutex;
};

#endif
