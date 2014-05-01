#include "AndroidBinderThread.h"
#include "binder/IPCThreadState.h"

AndroidBinderThread::AndroidBinderThread() {
}

AndroidBinderThread::~AndroidBinderThread() {
}

void *AndroidBinderThread::run() {
  //sp<IBinder> b = ProcessState::self()->getContextObject(NULL);
  android::IPCThreadState::self()->joinThreadPool();
  return NULL;
}

