#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> 
#include "V4lCamera.h"

V4lCamera::V4lCamera() : fd(-1) {
  memset(&frame, 0x00, sizeof(frame));
}

V4lCamera::~V4lCamera() {
  close();
}

V4lCamera::Frame* V4lCamera::getFrame() {
  printf("read(%d, %016lx, %u)\n", fd, (uintptr_t)frameBuffer, frameSize);
  int ret = ::read(fd, frameBuffer, frameSize);
  if (ret != frameSize) {
    printf("Failed to read v4l frame, %d != %u, errno=%d", ret, frameSize, errno);
    return NULL;
  }
  frame.data = frameBuffer;
  frame.size = frameSize;
  frame.width = 1280;
  frame.height = 720;
  frame.fresh = true;
  for (uint32_t i = 0; i < frameSize; i++)
    printf("%02x ", frameBuffer[0]);
  printf("\n");
  return &frame;
}

bool V4lCamera::open(int serial) {
  fd = ::open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
  if (fd != -1)
    init();
  else
    printf("Failed to open v4l device");
  return fd != -1;
}

static void xioctl(int fh, int request, void *arg) {
  int r;
  do {
    r = ioctl(fh, request, arg);
  } while (-1 == r && EINTR == errno);
  if (r == -1)
    printf("xioctl request %d failed\n", request);
}

void V4lCamera::init() {
  struct v4l2_format fmt;
  memset(&fmt, 0x00, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 1280;
  fmt.fmt.pix.height = 720;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  xioctl(fd, VIDIOC_S_FMT, (void *)&fmt);
  frameSize = fmt.fmt.pix.sizeimage;
  frameBuffer = new uint8_t[frameSize];
}

bool V4lCamera::isOpened() {
  return fd != -1;
}

bool V4lCamera::isAcquisitioning() {
  return acquisitioning;
}

void V4lCamera::startAcquisition() {
  acquisitioning = true;
}

void V4lCamera::stopAcquisition() {
  acquisitioning = false;
}

void V4lCamera::close() {
  if (fd != -1) {
    delete frameBuffer;
    ::close(fd);
    fd = -1;
  }
}

