#include "BaseCamera.h"
#include "Blobber.h"
#include "Canvas.h"
#include "Config.h"
#include "DebugRenderer.h"
#include "ImageProcessor.h"
#include <iostream>
#include "ProcessThread.h"
#include "Util.h"

ProcessThread::ProcessThread(std::string name, BaseCamera *camera, Blobber *blobber, Vision *vision) :
    camera(camera),
    blobber(blobber),
    vision(vision),
    stopRequested(false) {
  setName(name);
  width = blobber->getWidth();
  height = blobber->getHeight();
  classification = new unsigned char[width * height * 3];
  argb = new unsigned char[width * height * 4];
  rgb = new unsigned char[width * height * 3];
  visionResult = new Vision::Result();
  visionResult->vision = vision;
}

ProcessThread::~ProcessThread() {
  if (visionResult != NULL) {
    delete visionResult;
    visionResult = NULL;
  }
  delete[] classification;
  delete[] argb;
  delete[] rgb;
}

void ProcessThread::stopThread() {
  stopRequested.set(true);
}

void ProcessThread::nextFrame() {
  // dequeue camera frame
  // frame = camera->getFrame();
  /*
  if (visionResult != NULL) {
    delete visionResult;
    visionResult = NULL;
  }
  visionResult = vision->process();
  */
  // post visionResult

  /*
  if (debug) {
    vision->setDebugImage(rgb, width, height);
  } else {
    vision->setDebugImage(NULL, 0, 0);
  }
  */

  /*
  if (debug) {
    DebugRenderer::renderGrid(rgb, vision);
    DebugRenderer::renderBlobs(classification, blobber);
    DebugRenderer::renderBalls(rgb, vision, visionResult->balls);
    DebugRenderer::renderGoals(rgb, visionResult->goals);
    // DebugRenderer::renderObstructions(rgb, visionResult->obstructionSide);
    DebugRenderer::renderObstructions(rgb, visionResult->goalPathObstruction);
    // TODO Show whether a ball is in the way
  }
  */
}

void ProcessThread::doWork(sp<Frame> frame) {
  printf("ProcessThread got frame\n");
}

