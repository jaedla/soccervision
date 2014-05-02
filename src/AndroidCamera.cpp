#include <stdio.h>
#include <string.h>
#include "AndroidCamera.h"
#include "device3/Camera3Device.h"
#include "hardware/camera3.h"
#include "hardware/hardware.h"

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

void AndroidCamera::getModule() {
  int res = hw_get_module(CAMERA_HARDWARE_MODULE_ID, (const hw_module_t **)&module);
  check(res == 0, "Failed to get camera module");
}

bool AndroidCamera::open(int serial) {
  getModule();
  printf("Success\n");
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

