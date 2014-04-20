#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include "BaseCamera.h"

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
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
};

#endif

