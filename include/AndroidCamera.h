#ifndef ANDROIDCAMERA_H
#define ANDROIDCAMERA_H

#include "api1/client2/Parameters.h"
#include "BaseCamera.h"
#include "camera/CameraMetadata.h"
#include "common/CameraDeviceBase.h"
#include "gui/BufferQueue.h"
#include "gui/CpuConsumer.h"
#include "gui/Surface.h"
#include "hardware/camera3.h"
#include "utils/RefBase.h"
#include "utils/UniquePtr.h"
#include <string>

class AndroidCamera : public BaseCamera {
public:
  AndroidCamera();
  ~AndroidCamera();
  virtual Frame *getFrame();
  virtual int getSerial() {
    return -1;
  }
  virtual bool open(int serial = 0);
  virtual bool isOpened();
  virtual bool isAcquisitioning();
  virtual void startAcquisition();
  virtual void stopAcquisition();
  virtual void close();

private:
  class AndroidCameraFrame : public Frame {
    virtual ~AndroidCameraFrame() {
      delete data;
    }
  };

  typedef android::CpuConsumer::LockedBuffer LockedBuffer;

  class Stream : public android::CpuConsumer::FrameAvailableListener {
  public:
    Stream(std::string name, AndroidCamera *androidCamera);
    void create(uint32_t width, uint32_t height, uint32_t size, int format);
    virtual void onFrameAvailable();
    int getId() { return streamId; }
    LockedBuffer *getNewestFrame();
    void releaseFrame(LockedBuffer *frame);
  private:
    std::string name;
    AndroidCamera *androidCamera;
    android::sp<android::BufferQueue> bufferQueue;
    android::sp<android::CpuConsumer> bufferQueueConsumer;
    android::sp<ANativeWindow> surface;
    int streamId;
  };

  void getModule();
  void findBackCamera();
  void getDevice();
  void createParameters();
  Stream *createStream(std::string name, uint32_t width, uint32_t height, uint32_t size, int format);
  uint32_t getMaxJpegSize();
  void createStreams();
  void createRequest();
  void startStream();
  LockedBuffer *getNewestFrame();
  void releaseFrame(LockedBuffer *frame);
  void convertFrameToYuyv(LockedBuffer& frame, uint8_t *yuyv);

  struct camera_module *cameraModule;
  int cameraId;
  android::sp<android::CameraDeviceBase> device;
  UniquePtr<android::camera2::Parameters> parameters;
  android::sp<Stream> stream;
  android::CameraMetadata request;
  bool acquisitioning;
  uint32_t width;
  uint32_t height;
  uint32_t frameSize;
};

#endif

