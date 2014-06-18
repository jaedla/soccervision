#include "AndroidFrame.h"

AndroidFrame::AndroidFrame(android::sp<AndroidCameraStream> stream, sp<LockedBuffer> buffer) :
    stream(stream),
    buffer(buffer) {
  width = buffer->width;
  height = buffer->height;
  frameSize = width * height * 2;
  yuyv.reset(new uint8_t[frameSize]);
  convertFrameToYuyv();
}

AndroidFrame::~AndroidFrame() {
  stream->releaseBuffer(buffer);
}

uint32_t AndroidFrame::getWidth() {
  return width;
}

uint32_t AndroidFrame::getHeight() {
  return height;
}

double AndroidFrame::getTimestamp() {
  return 0;
}

uint32_t AndroidFrame::getFrameNumber() {
  return 0;
}

uint32_t *AndroidFrame::classify(Blobber *blobber) {
  //blobber->processFrame((Blobber::Pixel *)yuyv);
  //blobber->classify((Blobber::Rgb *)classification, (Blobber::Pixel *)yuyv);
  return NULL;
}

uint8_t *AndroidFrame::getPreview() {
  //ImageProcessor::YUYVToARGB(yuyv, argb, width, height);
  //ImageProcessor::ARGBToRGB(argb, rgb, width, height);
  return NULL;
}

void AndroidFrame::convertFrameToYuyv() {
  uint8_t *yuyvDst1 = &yuyv[0];
  uint8_t *yuyvDst2 = &yuyv[0] + width * 2;
  uint8_t yuyvGap = width * 2;
  uint8_t *ySrc1 = buffer->data;
  uint8_t *ySrc2 = buffer->data + buffer->stride;
  int32_t yGap = 2 * buffer->stride - width;
  uint8_t *uSrc = buffer->dataCb;
  uint8_t *vSrc = buffer->dataCr;
  int32_t uvStep = buffer->chromaStep;
  int32_t uvGap = buffer->chromaStride - (width / 2) * uvStep;
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

