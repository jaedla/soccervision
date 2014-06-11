#ifndef SHAREDFLAG_H
#define SHAREDFLAG_H

#include "Mutex.h"

class SharedFlag {
public:
  SharedFlag();
  ~SharedFlag();
  bool get();
  void set(bool newValue);
private:
  bool flag;
  Mutex mutex;
};

#endif
