#include "AndroidCamera.h"
#include "AndroidFrame.h"
#include "Check.h"
#include "device3/Camera3Device.h"
#include "hardware/hardware.h"
#include "ScopedMutex.h"
#include "sp.h"
#include <string.h>
#include "utils/Errors.h"
#include "utils/Vector.h"

AndroidCamera::AndroidCamera() {
}

AndroidCamera::~AndroidCamera() {
  close();
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

void AndroidCamera::createDevice() {
  device = new android::Camera3Device(cameraId);
  android::status_t res = device->initialize(cameraModule);
  Check(res == android::OK, "Failed to initialize camera device");
}

void AndroidCamera::createParameters() {
  parameters = make_sp<android::camera2::Parameters>(cameraId, CAMERA_FACING_BACK);
  parameters->initialize(&(device->info()));
  width = parameters->previewWidth;
  height = parameters->previewHeight;
  Check(!(width & 1), "Image width from camera not multiple of 2");
  printf("Camera resolution %ux%u\n", width, height);
}

uint32_t AndroidCamera::getMaxJpegSize() {
  camera_metadata_ro_entry_t maxJpegSize = parameters->staticInfo(ANDROID_JPEG_MAX_SIZE);
  Check(maxJpegSize.count != 0, "Failed to get max jpeg size");
  return maxJpegSize.data.i32[0];
}

AndroidCameraStream *AndroidCamera::createStream(uint32_t width, uint32_t height, uint32_t size, int format) {
  AndroidCameraStream *stream = new AndroidCameraStream(this);
  stream->create(width, height, size, format);
  return stream;
}

void AndroidCamera::createStreams() {
  stream = createStream(width, height, 0, HAL_PIXEL_FORMAT_YCbCr_420_888);
}

void AndroidCamera::createRequest() {
  android::status_t res = device->createDefaultRequest(CAMERA3_TEMPLATE_PREVIEW, &request);
  Check(res == android::OK, "Failed to create camera default request");
  res = parameters->updateRequest(&request);
  Check(res == android::OK, "Failed to configure camera request");
  android::Vector<int32_t> outputStreams;
  outputStreams.push(stream->getId());
  res = request.update(ANDROID_REQUEST_OUTPUT_STREAMS, outputStreams);
  Check(res == android::OK, "Failed to set camera request output streams");
  res = request.sort();
  Check(res == android::OK, "Failed to sort camera request");
}

void AndroidCamera::startStream() {
  android::status_t res = device->setStreamingRequest(request);
  Check(res == android::OK, "Failed to start camera stream");
}

bool AndroidCamera::open(int serial) {
  getModule();
  findBackCamera();
  createDevice();
  createParameters();
  createStreams();
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

void AndroidCamera::gotNewFrame(sp<Frame> frame) {
  newWorkProduced(frame);
}

