#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include "BaseCamera.h"
#include "Config.h"
#include <string>
#include "Thread.h"
#include "Vision.h"

class BaseCamera;
class Blobber;

class ProcessThread : public Thread {

public:
  ProcessThread(std::string name, BaseCamera *camera, Blobber *blobber, Vision *vision);
  ~ProcessThread();

  bool isDone() {
    return done;
  };

  int width;
  int height;
  Dir dir;

  BaseCamera *camera;
  Blobber *blobber;
  Vision *vision;
  Vision::Result *visionResult;
  bool debug;

  bool gotFrame;
  bool faulty;
  unsigned char *frameData;
  unsigned char *dataYUYV;
  unsigned char *dataY;
  unsigned char *dataU;
  unsigned char *dataV;
  unsigned char *classification;
  unsigned char *argb;
  unsigned char *rgb;

private:
  Frame *frame;
  void *run();
  bool fetchFrame();

  bool done;
};

#endif // PROCESSTHREAD_H
