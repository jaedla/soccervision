#ifndef ANDROIDCAMERA_H
#define ANDROIDCAMERA_H

#include "AndroidCameraStream.h"
#include "api1/client2/Parameters.h"
#include "BaseCamera.h"
#include "camera/CameraMetadata.h"
#include "common/CameraDeviceBase.h"
#include "hardware/camera3.h"
#include "sp.h"
#include <string>
#include "utils/RefBase.h"

class AndroidCameraStream;

class AndroidCamera : public BaseCamera {
public:
  AndroidCamera();
  ~AndroidCamera();
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
  friend AndroidCameraStream;
  void getModule();
  void findBackCamera();
  void createDevice();
  void createParameters();
  AndroidCameraStream *createStream(uint32_t width, uint32_t height, uint32_t size, int format);
  uint32_t getMaxJpegSize();
  void createStreams();
  void createRequest();
  void startStream();
  android::sp<android::CameraDeviceBase> getDevice() { return device; }
  void gotNewFrame(sp<Frame> frame);

  struct camera_module *cameraModule;
  int cameraId;
  android::sp<android::CameraDeviceBase> device;
  sp<android::camera2::Parameters> parameters;
  android::sp<AndroidCameraStream> stream;
  android::CameraMetadata request;
  uint32_t width;
  uint32_t height;
  bool acquisitioning;
};

#endif

