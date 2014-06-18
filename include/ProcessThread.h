#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include "BaseCamera.h"
#include "Config.h"
#include "Frame.h"
#include "SharedFlag.h"
#include <string>
#include "Thread.h"
#include "Vision.h"
#include "Worker.h"

class BaseCamera;
class Blobber;

class ProcessThread : public Worker<Frame> {

public:
  ProcessThread(std::string name, BaseCamera *camera, Blobber *blobber, Vision *vision);
  ~ProcessThread();
  void stopThread();
  void setCamera(BaseCamera *newCamera) { camera = newCamera; }
  uint8_t *getYuyv() { return yuyv; }
  uint8_t *getRgb() { return rgb; }
  uint8_t *getClassification() { return classification; }
  Vision::Result *getVisionResult() { return visionResult; }

private:
  virtual void doWork(sp<Frame> frame);
  void nextFrame();
  BaseCamera *camera;
  Blobber *blobber;
  Vision *vision;
  Vision::Result *visionResult;
  SharedFlag stopRequested;
  int width;
  int height;
  uint8_t *yuyv;
  uint8_t *argb;
  uint8_t *rgb;
  uint8_t *classification;
};

#endif // PROCESSTHREAD_H
