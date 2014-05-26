#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include "api1/client2/Parameters.h"
#include "BaseCamera.h"
#include "common/CameraDeviceBase.h"
#include "gui/BufferQueue.h"
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
  void getModule();
  void findBackCamera();
  void getDevice();
  void createParameters();
  void createStream();
  struct camera_module *cameraModule;
  int cameraId;
  android::sp<android::CameraDeviceBase> device;
  UniquePtr<android::camera2::Parameters> parameters;
  android::sp<android::BufferQueue> bufferQueue;
  android::sp<ANativeWindow> surface;
  int streamId;
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
};

#endif

