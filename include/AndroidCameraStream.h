#ifndef ANDROIDCAMERASTREAM_H
#define ANDROIDCAMERASTREAM_H

#include "AndroidCamera.h"
#include "ConditionSignal.h"
#include <cstdint>
#include "gui/BufferQueue.h"
#include "gui/CpuConsumer.h"
#include "gui/Surface.h"
#include "sp.h"
#include "Thread.h"
#include "utils/RefBase.h"
#include "WaitableFlag.h"

class AndroidCamera;

class AndroidCameraStream : public android::CpuConsumer::FrameAvailableListener, public Thread {
private:
  typedef android::CpuConsumer::LockedBuffer LockedBuffer;
public:
  AndroidCameraStream(AndroidCamera *androidCamera);
  ~AndroidCameraStream();
  void create(uint32_t width, uint32_t height, uint32_t size, int format);
  virtual void onFrameAvailable();
  int getId() { return streamId; }
  void releaseBuffer(sp<LockedBuffer> buffer);
private:
  sp<LockedBuffer> getNextBuffer();
  void handleBuffer(sp<LockedBuffer> buffer);
  void handleAllBuffers();
  void waitUntilCreated();
  void frameLoop();
  virtual void run();

  WaitableFlag streamCreated;
  ConditionSignal signal;
  AndroidCamera *androidCamera;
  android::sp<android::BufferQueue> bufferQueue;
  android::sp<android::CpuConsumer> bufferQueueConsumer;
  android::sp<ANativeWindow> surface;
  int streamId;
};

#endif
