#include <stdio.h>
#include <string.h>
#include "AndroidCamera.h"
#include "modules/androidcamera/camera_wrapper/camera_wrapper.h"

static void check(bool condition, const char *msg) {
  if (!condition) {
    printf("Check failed: %s\n", msg);
    exit(1);
  }
}

AndroidCamera::AndroidCamera() {
}

AndroidCamera::~AndroidCamera() {
  close();
}

Frame *AndroidCamera::getFrame() {
  return NULL;
  Frame *frame = new Frame();
  frame->data = NULL;
  frame->size = frameSize;
  frame->width = width;
  frame->height = height;
  frame->fresh = true;
  return frame;
}

bool AndroidCamera::open(int serial) {
  return true;
}

bool AndroidCamera::isOpened() {
  return true;
}

bool AndroidCamera::isAcquisitioning() {
  return acquisitioning;
}

void AndroidCamera::startAcquisition() {
  acquisitioning = true;
}

void AndroidCamera::stopAcquisition() {
  acquisitioning = false;
}

void AndroidCamera::close() {
}

