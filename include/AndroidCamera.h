#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include "BaseCamera.h"

namespace android {
namespace camera3 {
class Camera3Device;
}
}

struct camera_module;

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
  const struct camera_module *module;
  android::camera3::Camera3Device *device;
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
};

#endif

