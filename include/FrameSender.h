#ifndef FRAMESENDER_H
#define FRAMESENDER_H

#include "Frame.h"
#include "Mutex.h"
#include "Server.h"
#include "sp.h"
#include "Worker.h"

class FrameSender : public Worker<Frame> {
public:
  FrameSender();
  ~FrameSender();
  void setServer(sp<Server> server);
private:
  void doWork(sp<Frame> frame);
  sp<Server> getServer();
  std::string rgbToJpeg(uint8_t *rgb, uint32_t width, uint32_t height);
  sp<Server> server;
  Mutex serverMutex;
};

#endif
