#ifndef FRAME_H
#define FRAME_H

#include "Blobber.h"

class Frame {
public:
  Frame() {}
  virtual ~Frame() {}
  virtual uint32_t getWidth();
  virtual uint32_t getHeight();
  virtual double getTimestamp();
  virtual uint32_t getFrameNumber();
  virtual uint32_t *classify(Blobber *blobber);
  virtual uint8_t *getRgbPreview(uint32_t previewWidth, uint32_t previewHeight);
private:
};

#endif
