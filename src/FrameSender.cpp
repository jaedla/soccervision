#include <algorithm>
#include "Check.h"
#include "Config.h"
#include <cstdint>
#include "FrameSender.h"
#include "ImageProcessor.h"
#include <memory>
#include "ScopedMutex.h"
#include <string>
#include "Util.h"

FrameSender::FrameSender() {
  setSkipToLatest(true);
}

FrameSender::~FrameSender() {
}

void FrameSender::setServer(sp<Server> server) {
  ScopedMutex lock(&serverMutex);
  this->server = server;
}

void FrameSender::doWork(sp<Frame> frame) {
  printf("FrameSender got frame, queueSize=%u\n", queueSize());
  sp<Server> server = getServer();
  if (!server)
    return;
  uint32_t wholeWidth = frame->getWidth();
  uint32_t wholeHeight = frame->getHeight();
  uint32_t width = std::min(200u, wholeWidth);
  uint32_t height = (width * wholeHeight) / wholeWidth;
  uint8_t *rgbPreview = frame->getRgbPreview(width, height);
  std::string base64Jpeg = rgbToJpeg(rgbPreview, width, height);
  std::string json = Util::json("frame", "{\"rgb\": \"" + base64Jpeg + "\"}");
  server->broadcast(json);
  printf("broadcast done\n");
}

sp<Server> FrameSender::getServer() {
  ScopedMutex lock(&serverMutex);
  return server;
}

std::string FrameSender::rgbToJpeg(uint8_t *rgb, uint32_t width, uint32_t height) {
  int jpegSize = Config::jpegBufferSize;
  std::unique_ptr<uint8_t[]> jpeg(new uint8_t[jpegSize]);
  printf("to jpeg\n");
  bool ret = ImageProcessor::rgbToJpeg(rgb, &jpeg[0], jpegSize, width, height);
  Check(ret, "Converting RGB image to JPEG failed, probably need to increase buffer size");
  printf("jpeg done\n");
  std::string base64Jpeg = Util::base64Encode(&jpeg[0], jpegSize);
  printf("base64 done\n");
  return base64Jpeg;
}

