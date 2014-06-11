#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include "BaseCamera.h"
#include "Config.h"
#include <string>
#include "Thread.h"
#include "Vision.h"
#include "WaitableFlag.h"

class BaseCamera;
class Blobber;

class ProcessThread : public Thread {

public:
  ProcessThread(std::string name, BaseCamera *camera, Blobber *blobber, Vision *vision);
  ~ProcessThread();

  bool isDone() {
    return done;
  };

  void stopThread();
  void startProcessing();
  void waitUntilProcessed();

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
  void run();
  bool fetchFrame();
  void process();
  bool stopRequested;
  Frame *frame;
  WaitableFlag processing;
  bool done;
};

#endif // PROCESSTHREAD_H
