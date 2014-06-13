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
  void stopThread();
  void startProcessing();
  void waitUntilProcessed();
  void setDebug(bool newDebug) { debug = newDebug; }
  void setCamera(BaseCamera *newCamera) { camera = newCamera; }
  uint8_t *getYuyv() { return yuyv; }
  uint8_t *getRgb() { return rgb; }
  uint8_t *getClassification() { return classification; }
  Vision::Result *getVisionResult() { return visionResult; }

private:
  void run();
  bool fetchFrame();
  void process();

  BaseCamera *camera;
  Blobber *blobber;
  Vision *vision;
  Vision::Result *visionResult;
  WaitableFlag processing;
  bool debug;
  bool stopRequested;
  int width;
  int height;
  Dir dir;
  Frame *frame;
  uint8_t *yuyv;
  uint8_t *argb;
  uint8_t *rgb;
  uint8_t *classification;
};

#endif // PROCESSTHREAD_H
