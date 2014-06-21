#include "AndroidFrame.h"

/*   ____h____
 *  |         |
 * w|         |
 *  |_________|
 */

AndroidFrame::AndroidFrame(android::sp<AndroidCameraStream> stream, sp<LockedBuffer> buffer) :
    stream(stream),
    buffer(buffer) {
  rawHeight = buffer->height;
  rawWidth = buffer->width;
  width = rawHeight;
  height = rawWidth;
  //yuyv.reset(new uint8_t[width * height * 2]);
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

uint8_t *AndroidFrame::getRgbPreview(uint32_t width, uint32_t height) {
  if (!preview) {
    uint32_t resolution = width * height;
    std::unique_ptr<uint8_t[]> yuyv(new uint8_t[resolution * 2]);
    std::unique_ptr<uint8_t[]> argb(new uint8_t[resolution * 4]);
    preview.reset(new uint8_t[resolution * 3]);
    createPreviewYuyv(&yuyv[0], width, height);
    ImageProcessor::YUYVToARGB(&yuyv[0], &argb[0], width, height);
    ImageProcessor::ARGBToRGB(&argb[0], &preview[0], width, height);
  }
  return &preview[0];
}

void AndroidFrame::createPreviewYuyv(
    uint8_t *previewYuyv,
    uint32_t previewWidth,
    uint32_t previewHeight) {
  Check(!(previewWidth & 1), "Preview width must be divisible by 2");
  uint32_t stepY = rawHeight / previewWidth;
  uint32_t stepX = rawWidth / previewHeight;
  uint32_t halfStepY = (stepY + 1) / 2;
  uint32_t halfStepX = stepX / 2;
  uint32_t x = halfStepX;
  uint32_t yRowStep = stepY * buffer->stride;
  uint8_t *yColSrc = buffer->data + (rawHeight - halfStepY) * buffer->stride + halfStepX;
  uint32_t chromaStride = buffer->chromaStride;
  uint32_t chromaStep = buffer->chromaStep;
  for (uint32_t previewY = 0; previewY < previewHeight; ++previewY) {
    uint32_t y = rawHeight - halfStepY;
    uint8_t *ySrc = yColSrc;
    uint8_t *uSrc = buffer->dataCb + (x / 2) * chromaStep;
    uint8_t *vSrc = buffer->dataCr + (x / 2) * chromaStep;
    for (uint32_t previewX = 0; previewX < previewWidth; previewX += 2) {
      uint32_t y1 = *ySrc;
      uint32_t u1 = *(uSrc + (y / 2) * chromaStride);
      uint32_t v1 = *(vSrc + (y / 2) * chromaStride);
      ySrc -= yRowStep;
      y -= stepY;
      uint32_t y2 = *ySrc;
      uint32_t u2 = *(uSrc + (y / 2) * chromaStride);
      uint32_t v2 = *(vSrc + (y / 2) * chromaStride);
      ySrc -= yRowStep;
      y -= stepY;
      *previewYuyv++ = y1;
      *previewYuyv++ = (u1 + u2 + 1) / 2;
      *previewYuyv++ = y2;
      *previewYuyv++ = (v1 + v2 + 1) / 2;
    }
    x += stepX;
    yColSrc += stepX;
  }
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

