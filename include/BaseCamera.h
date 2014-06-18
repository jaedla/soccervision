#ifndef CAMERA_H
#define CAMERA_H

#include "Frame.h"
#include <stdlib.h>
#include "WorkProducer.h"

class BaseCamera : public WorkProducer<Frame> {
public:
  virtual ~BaseCamera() {}
  virtual int getSerial() {
    return -1;
  }
  virtual bool open(int serial = 0) {
    return true;
  }
  virtual bool isOpened() {
    return true;
  }
  virtual bool isAcquisitioning() {
    return true;
  }
  virtual void startAcquisition() {}
  virtual void stopAcquisition() {}
  virtual void close() {}
};

#endif // CAMERA_H
