#include <stdio.h>
#include <string.h>
#include "AndroidCamera.h"
#include "device3/Camera3Device.h"
#include "hardware/hardware.h"
#include "utils/Errors.h"

using namespace android;

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
  int res = hw_get_module(CAMERA_HARDWARE_MODULE_ID, (const hw_module_t **)&cameraModule);
  check(res == 0, "Failed to get camera module");
}

void AndroidCamera::findBackCamera() {
  int numberOfCameras = (*cameraModule->get_number_of_cameras)();
  for (int cameraId = 0; cameraId < numberOfCameras; cameraId++) {
    camera_info_t cameraInfo;
    int res = (*cameraModule->get_camera_info)(cameraId, &cameraInfo);
    check(res == 0, "get_camera_info failed");
    if (cameraInfo.facing == CAMERA_FACING_BACK) {
      this->cameraId = cameraId;
      return;
    }
  }
  check(false, "Didn't find a back camera");
}

void AndroidCamera::getDevice() {
  device = new Camera3Device(cameraId);
  status_t res = device->initialize(cameraModule);
  check(res == OK, "Failed to initialize camera device");
}

void AndroidCamera::createParameters() {
  parameters.reset(new android::camera2::Parameters(cameraId, CAMERA_FACING_BACK));
  parameters->initialize(&(device->info()));
}

void AndroidCamera::createStream() {
  bufferQueue = new BufferQueue();
  surface = new Surface(bufferQueue);
  //device->createStream(surface
}

bool AndroidCamera::open(int serial) {
  getModule();
  findBackCamera();
  getDevice();
  createParameters();
  createStream();
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

