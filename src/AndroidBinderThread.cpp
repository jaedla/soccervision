#include "AndroidBinderThread.h"
#include "binder/IPCThreadState.h"

AndroidBinderThread::AndroidBinderThread() : Thread() {
  setName("AndroidBinder");
}

AndroidBinderThread::~AndroidBinderThread() {
}

void AndroidBinderThread::run() {
  //sp<IBinder> b = ProcessState::self()->getContextObject(NULL);
  android::IPCThreadState::self()->joinThreadPool();
}

