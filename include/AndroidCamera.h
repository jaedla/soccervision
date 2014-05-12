#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include "BaseCamera.h"
#include "common/CameraDeviceBase.h"
#include "gui/BufferQueue.h"
#include "hardware/camera3.h"
#include "utils/RefBase.h"

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
  void createStream();
  struct camera_module *cameraModule;
  android::sp<android::CameraDeviceBase> device;
  android::sp<android::BufferQueue> bufferQueue;
  int cameraId;
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
};

#endif

