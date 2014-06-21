#ifndef ANDROIDFRAME_H
#define ANDROIDFRAME_H

#include "AndroidCameraStream.h"
#include "Check.h"
#include <cstdint>
#include "Frame.h"
#include "gui/CpuConsumer.h"
#include "ImageProcessor.h"
#include <memory>
#include "sp.h"
#include "utils/RefBase.h"

class AndroidFrame : public Frame {
private:
  typedef android::CpuConsumer::LockedBuffer LockedBuffer;
public:
  AndroidFrame(android::sp<AndroidCameraStream> stream, sp<LockedBuffer> buffer);
  virtual ~AndroidFrame();
  virtual uint32_t getWidth();
  virtual uint32_t getHeight();
  virtual double getTimestamp();
  virtual uint32_t getFrameNumber();
  virtual uint32_t *classify(Blobber *blobber);
  virtual uint8_t *getRgbPreview(uint32_t previewWidth, uint32_t previewHeight);
private:
  void createPreviewYuyv(uint8_t *previewYuyv, uint32_t width, uint32_t height);
  void convertFrameToYuyv();

  android::sp<AndroidCameraStream> stream;
  sp<LockedBuffer> buffer;
  uint32_t width;
  uint32_t height;
  uint32_t rawWidth;
  uint32_t rawHeight;
  std::unique_ptr<uint8_t[]> yuyv;
  std::unique_ptr<uint8_t[]> preview;
};

#endif
