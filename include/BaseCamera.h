#ifndef CAMERA_H
#define CAMERA_H

#include <stdlib.h>

class Frame {
public:
  Frame() : data(NULL), size(0), width(0), height(0), number(0), fresh(false), timestamp(0.0) {}
  virtual ~Frame() {};
  uint8_t *data;
  int size;
  int width;
  int height;
  int number;
  bool fresh;
  double timestamp;
};

class BaseCamera {
public:
  virtual ~BaseCamera() {}
  virtual Frame *getFrame() = 0;
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
