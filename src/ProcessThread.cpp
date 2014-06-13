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
    Thread(name),
    camera(camera),
    blobber(blobber),
    vision(vision),
    debug(false),
    stopRequested(false) {
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

void ProcessThread::run() {
  while (true) {
    processing.waitUntil(true);
    if (stopRequested)
      break;
    process();
    processing.set(false);
  }
}

void ProcessThread::stopThread() {
  stopRequested = true;
  processing.set(true);
}

void ProcessThread::startProcessing() {
  processing.set(true);
}

void ProcessThread::waitUntilProcessed() {
  processing.waitUntil(false);
}

void ProcessThread::process() {
  if (!fetchFrame())
    return;

  if (visionResult != NULL) {
    delete visionResult;
    visionResult = NULL;
  }

  //blobber->processFrame((Blobber::Pixel *)yuyv);

  if (debug) {
    printf("to rgb\n");
    //blobber->classify((Blobber::Rgb *)classification, (Blobber::Pixel *)yuyv);
    ImageProcessor::YUYVToARGB(yuyv, argb, width, height);
    ImageProcessor::ARGBToRGB(argb, rgb, width, height);
    printf("to rgb done\n");
    //vision->setDebugImage(rgb, width, height);
  } else {
    //vision->setDebugImage(NULL, 0, 0);
  }

  //visionResult = vision->process();

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

  if (frame != NULL) {
    delete frame;
    frame = NULL;
  }
}

bool ProcessThread::fetchFrame() {
  if (camera->isAcquisitioning()) {
    frame = camera->getFrame();
    if (frame != NULL && frame->fresh) {
      yuyv = frame->data;
      return true;
    }
  }
  return false;
}

