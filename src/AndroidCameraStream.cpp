#include "AndroidCameraStream.h"
#include "AndroidFrame.h"
#include "Check.h"
#include "ScopedMutex.h"

AndroidCameraStream::AndroidCameraStream(AndroidCamera *androidCamera) :
    androidCamera(androidCamera),
    streamId(0) {
  setName("AndroidCameraStream");
  start();
}

AndroidCameraStream::~AndroidCameraStream() {
  stop();
}

void AndroidCameraStream::onFrameAvailable() {
  printf("frame available\n");
  ScopedMutex lock(signal.mutex());
  signal.signal();
}

void AndroidCameraStream::create(uint32_t width, uint32_t height, uint32_t size, int format) {
  bufferQueue = new android::BufferQueue();
  bufferQueueConsumer = new android::CpuConsumer(bufferQueue, 6);
  bufferQueueConsumer->setFrameAvailableListener(this);
  surface = new android::Surface(bufferQueue);
  android::status_t res = androidCamera->getDevice()->createStream(
    surface,
    width,
    height,
    format,
    size,
    &streamId);
  Check(res == android::OK, "Failed to create Android camera stream");
  streamCreated.set(true);
}

sp<AndroidCameraStream::LockedBuffer> AndroidCameraStream::getNextBuffer() {
  sp<LockedBuffer> buffer = make_sp<LockedBuffer>();
  android::status_t res = bufferQueueConsumer->lockNextBuffer(buffer.get());
  if (res != android::OK)
    return NULL;
  return buffer;
}

void AndroidCameraStream::handleBuffer(sp<LockedBuffer> buffer) {
  sp<Frame> frame = make_sp<AndroidFrame>(this, buffer);
  androidCamera->gotNewFrame(frame);
}

void AndroidCameraStream::handleAllBuffers() {
  sp<LockedBuffer> buffer;
  do {
    sp<LockedBuffer> buffer = getNextBuffer();
    if (buffer)
      handleBuffer(buffer);
  } while (buffer);
}

void AndroidCameraStream::releaseBuffer(sp<LockedBuffer> buffer) {
  bufferQueueConsumer->unlockBuffer(*buffer.get());
}

void AndroidCameraStream::waitUntilCreated() {
  while (!isStopRequested()) {
    if (streamCreated.waitUntil(true, 100))
      break;
  }
}

void AndroidCameraStream::frameLoop() {
  sp<LockedBuffer> buffer;
  while (!isStopRequested()) {
    {
      ScopedMutex lock(signal.mutex());
      buffer = getNextBuffer();
      if (!buffer)
        signal.wait(100);
    }
    if (buffer) {
      handleBuffer(buffer);
      buffer.reset();
    }
    handleAllBuffers();
  }
}

void AndroidCameraStream::run() {
  waitUntilCreated();
  frameLoop();
}

