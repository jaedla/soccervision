#ifndef ANDROIDCAMERA_H
#define ANDROIDCAMERA_H

#include "api1/client2/Parameters.h"
#include "BaseCamera.h"
#include "camera/CameraMetadata.h"
#include "common/CameraDeviceBase.h"
#include "gui/BufferQueue.h"
#include "gui/CpuConsumer.h"
#include "gui/Surface.h"
#include "hardware/camera3.h"
#include "utils/RefBase.h"
#include "utils/UniquePtr.h"

class AndroidCamera : public BaseCamera {
public:
  AndroidCamera();
  ~AndroidCamera();
  virtual Frame *getFrame();
  virtual int getSerial() {
    return -1;
  }
  virtual bool open(int serial = 0);
  virtual bool isOpened();
  virtual bool isAcquisitioning();
  virtual void startAcquisition();
  virtual void stopAcquisition();
  virtual void close();
private:
  class FrameListener : public android::CpuConsumer::FrameAvailableListener {
  public:
    FrameListener(AndroidCamera *androidCamera) : androidCamera(androidCamera) {
    }
    virtual void onFrameAvailable();
  private:
    AndroidCamera *androidCamera;
  };
  typedef android::CpuConsumer::LockedBuffer LockedBuffer;
  void getModule();
  void findBackCamera();
  void getDevice();
  void createParameters();
  void createStream();
  void createRequest();
  void startStream();
  LockedBuffer *getNewestFrame();
  void releaseFrame(LockedBuffer *frame);
  struct camera_module *cameraModule;
  int cameraId;
  android::sp<android::CameraDeviceBase> device;
  UniquePtr<android::camera2::Parameters> parameters;
  android::sp<android::BufferQueue> bufferQueue;
  android::sp<android::CpuConsumer> bufferQueueConsumer;
  android::sp<FrameListener> frameListener;
  android::sp<ANativeWindow> surface;
  int streamId;
  android::CameraMetadata request;
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
};

#endif

