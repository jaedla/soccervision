#ifndef ANDROIDBINDERTHREAD_H
#define ANDROIDBINDERTHREAD_H

#include "Thread.h"

class AndroidBinderThread : public Thread {
public:
  AndroidBinderThread();
  virtual ~AndroidBinderThread();
  virtual void run();
};

#endif // ANDROIDBINDERTHREAD_H
