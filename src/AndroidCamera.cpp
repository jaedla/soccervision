#include <stdio.h>
#include <string.h>
#include "AndroidCamera.h"
#include "device3/Camera3Device.h"
#include "hardware/hardware.h"
#include "utils/Errors.h"
#include "utils/Vector.h"

using namespace android;

static void Check(bool condition, const char *msg) {
  if (!condition) {
    printf("Check failed: %s\n", msg);
    exit(1);
  }
}

void AndroidCamera::FrameListener::onFrameAvailable() {
  //printf("frame available\n");
}

AndroidCamera::AndroidCamera() {
}

AndroidCamera::~AndroidCamera() {
  close();
}

Frame *AndroidCamera::getFrame() {
  LockedBuffer *imageBuffer = getNewestFrame();
  Frame *frame = new AndroidCameraFrame();
  frame->data = new uint8_t[frameSize];
  frame->size = frameSize;
  frame->width = width;
  frame->height = height;
  frame->fresh = true;
  convertFrameToYuyv(*imageBuffer, frame->data);
  releaseFrame(imageBuffer);
  //printf("took frame\n");
  return frame;
}

void AndroidCamera::getModule() {
  int res = hw_get_module(CAMERA_HARDWARE_MODULE_ID, (const hw_module_t **)&cameraModule);
  Check(res == 0, "Failed to get camera module");
}

void AndroidCamera::findBackCamera() {
  int numberOfCameras = (*cameraModule->get_number_of_cameras)();
  for (int cameraId = 0; cameraId < numberOfCameras; cameraId++) {
    camera_info_t cameraInfo;
    int res = (*cameraModule->get_camera_info)(cameraId, &cameraInfo);
    Check(res == 0, "get_camera_info failed");
    if (cameraInfo.facing == CAMERA_FACING_BACK) {
      this->cameraId = cameraId;
      return;
    }
  }
  Check(false, "Didn't find a back camera");
}

void AndroidCamera::getDevice() {
  device = new Camera3Device(cameraId);
  status_t res = device->initialize(cameraModule);
  Check(res == OK, "Failed to initialize camera device");
}

void AndroidCamera::createParameters() {
  parameters.reset(new camera2::Parameters(cameraId, CAMERA_FACING_BACK));
  parameters->initialize(&(device->info()));
  width = parameters->previewWidth;
  height = parameters->previewHeight;
  frameSize = width * height * 2;
  Check(!(width & 1), "Image width from camera not multiple of 2");
  printf("Camera resolution %ux%u\n", width, height);
}

void AndroidCamera::createStream() {
  bufferQueue = new BufferQueue();
  bufferQueueConsumer = new CpuConsumer(bufferQueue, 6);
  frameListener = new FrameListener(this);
  bufferQueueConsumer->setFrameAvailableListener(frameListener);
  surface = new Surface(bufferQueue);
  status_t res = device->createStream(
    surface,
    width,
    height,
    HAL_PIXEL_FORMAT_YCbCr_420_888,
    0,
    &streamId);
  Check(res == OK, "Failed to create Android camera stream");
}

void AndroidCamera::createRequest() {
  status_t res = device->createDefaultRequest(CAMERA3_TEMPLATE_PREVIEW, &request);
  Check(res == OK, "Failed to create camera default request");
  res = parameters->updateRequest(&request);
  Check(res == OK, "Failed to configure camera request");
  Vector<int32_t> outputStreams;
  outputStreams.push(streamId);
  res = request.update(ANDROID_REQUEST_OUTPUT_STREAMS, outputStreams);
  Check(res == OK, "Failed to set camera request output streams");
  res = request.sort();
  Check(res == OK, "Failed to sort camera request");
}

void AndroidCamera::startStream() {
  status_t res = device->setStreamingRequest(request);
  Check(res == OK, "Failed to start camera stream");
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

void AndroidCamera::convertFrameToYuyv(LockedBuffer& frame, uint8_t *yuyv) {
  uint8_t *yuyvDst1 = yuyv;
  uint8_t *yuyvDst2 = yuyv + width * 2;
  uint8_t yuyvGap = width * 2;
  uint8_t *ySrc1 = frame.data;
  uint8_t *ySrc2 = frame.data + frame.stride;
  int32_t yGap = 2 * frame.stride - width;
  uint8_t *uSrc = frame.dataCb;
  uint8_t *vSrc = frame.dataCr;
  int32_t uvStep = frame.chromaStep;
  int32_t uvGap = frame.chromaStride - (width / 2) * uvStep;
  for (uint32_t row = 0; row < height; row += 2) {
    for (uint32_t column = 0; column < width; column += 2) {
      uint8_t u = *uSrc;
      uint8_t v = *vSrc;
      uSrc += uvStep;
      vSrc += uvStep;
      *yuyvDst1++ = *ySrc1++;
      *yuyvDst1++ = u;
      *yuyvDst1++ = *ySrc1++;
      *yuyvDst1++ = v;
      *yuyvDst2++ = *ySrc2++;
      *yuyvDst2++ = u;
      *yuyvDst2++ = *ySrc2++;
      *yuyvDst2++ = v;
    }
    yuyvDst1 += yuyvGap;
    yuyvDst2 += yuyvGap;
    ySrc1 += yGap;
    ySrc2 += yGap;
    uSrc += uvGap;
    vSrc += uvGap;
  }
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
  if (device != 0) {
    printf("Closing camera\n");
    device->disconnect();
    device.clear();
  }
}

