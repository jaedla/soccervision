#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include "BaseCamera.h"

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
};

#endif
