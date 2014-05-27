#include <stdio.h>
#include <string.h>
#include "AndroidCamera.h"
#include "device3/Camera3Device.h"
#include "hardware/hardware.h"
#include "utils/Errors.h"
#include "utils/Vector.h"

using namespace android;

static void check(bool condition, const char *msg) {
  if (!condition) {
    printf("Check failed: %s\n", msg);
    exit(1);
  }
}

void AndroidCamera::FrameListener::onFrameAvailable() {
  printf("onFrameAvailable\n");
}

AndroidCamera::AndroidCamera() {
}

AndroidCamera::~AndroidCamera() {
  close();
}

Frame *AndroidCamera::getFrame() {
  LockedBuffer *imageBuffer = getNewestFrame();
  releaseFrame(imageBuffer);
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
  parameters.reset(new camera2::Parameters(cameraId, CAMERA_FACING_BACK));
  parameters->initialize(&(device->info()));
}

void AndroidCamera::createStream() {
  bufferQueue = new BufferQueue();
  bufferQueueConsumer = new CpuConsumer(bufferQueue, 6);
  frameListener = new FrameListener(this);
  bufferQueueConsumer->setFrameAvailableListener(frameListener);
  surface = new Surface(bufferQueue);
  printf("Creating camera stream %ux%u\n", parameters->previewWidth, parameters->previewHeight);
  status_t res = device->createStream(
    surface,
    parameters->previewWidth,
    parameters->previewHeight,
    HAL_PIXEL_FORMAT_YCbCr_420_888,
    0,
    &streamId);
  check(res == OK, "Failed to create Android camera stream");
}

void AndroidCamera::createRequest() {
  status_t res = device->createDefaultRequest(CAMERA3_TEMPLATE_PREVIEW, &request);
  check(res == OK, "Failed to create camera default request");
  res = parameters->updateRequest(&request);
  check(res == OK, "Failed to configure camera request");
  Vector<int32_t> outputStreams;
  outputStreams.push(streamId);
  res = request.update(ANDROID_REQUEST_OUTPUT_STREAMS, outputStreams);
  check(res == OK, "Failed to set camera request output streams");
  res = request.sort();
  check(res == OK, "Failed to sort camera request");
}

void AndroidCamera::startStream() {
  status_t res = device->setStreamingRequest(request);
  check(res == OK, "Failed to start camera stream");
}

AndroidCamera::LockedBuffer *AndroidCamera::getNewestFrame() {
  UniquePtr<LockedBuffer> frame;
  do {
    status_t res;
    do {
      UniquePtr<LockedBuffer> nextFrame(new CpuConsumer::LockedBuffer());
      res = bufferQueueConsumer->lockNextBuffer(nextFrame.get());
      if (res == OK) {
        if (frame.get())
          bufferQueueConsumer->unlockBuffer(*frame);
        frame.reset(nextFrame.release());
      }
    } while (res == OK);
  } while (!frame.get());
  return frame.release();
}

void AndroidCamera::releaseFrame(LockedBuffer *frame) {
  bufferQueueConsumer->unlockBuffer(*frame);
  delete frame;
}

bool AndroidCamera::open(int serial) {
  getModule();
  findBackCamera();
  getDevice();
  createParameters();
  createStream();
  createRequest();
  startStream();
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

