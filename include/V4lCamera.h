#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include "BaseCamera.h"
#include <opencv2/opencv.hpp>

class V4lCamera : public BaseCamera {
public:
  V4lCamera();
  ~V4lCamera();
	virtual Frame* getFrame();
	virtual int getSerial() { return -1; }
	virtual bool open(int serial = 0);
	virtual bool isOpened();
	virtual bool isAcquisitioning();
	virtual void startAcquisition();
	virtual void stopAcquisition();
	virtual void close();
private:
  void init();
  void initFormat();
  void requestMmapBuffers();
  void mapBuffers();
  void queueBuffers();
  void queueBuffer(uint8_t index);
  int fd;
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
  uint8_t *frameBuffers[4];
  uint8_t numFrames;
  Frame frame;
};

#endif
