#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> 
#include "V4lCamera.h"

void check(bool condition, const char *msg) {
  if (!condition) {
    printf("Check failed: %s\n", msg);
    exit(1);
  }
}

V4lCamera::V4lCamera() : fd(-1) {
  memset(&frame, 0x00, sizeof(frame));
}

V4lCamera::~V4lCamera() {
  close();
}

V4lCamera::Frame* V4lCamera::getFrame() {
  /*
  printf("read(%d, %016lx, %u)\n", fd, (uintptr_t)frameBuffer, frameSize);
  int ret = ::read(fd, frameBuffer, frameSize);
  if (ret != frameSize) {
    printf("Failed to read v4l frame, %d != %u, errno=%d", ret, frameSize, errno);
    return NULL;
  }
  frame.data = frameBuffer;
  frame.size = frameSize;
  frame.width = width;
  frame.height = height;
  frame.fresh = true;
  for (uint32_t i = 0; i < frameSize; i++)
    printf("%02x ", frameBuffer[0]);
  printf("\n");
  */
  return &frame;
}

bool V4lCamera::open(int serial) {
  fd = ::open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
  if (fd != -1) {
    init();
    printf("Camera initialized\n");
  } else {
    printf("Failed to open v4l device");
  }
  return fd != -1;
}

static void xioctl(int fd, int request, void *arg) {
  int r;
  do {
    r = ioctl(fd, request, arg);
  } while (-1 == r && EINTR == errno);
  if (r == -1) {
    printf("xioctl request %d failed\n", request);
    exit(1);
  }
}

void V4lCamera::initFormat() {
  struct v4l2_format fmt;
  memset(&fmt, 0x00, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  width = 1280;
  height = 720;
  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  xioctl(fd, VIDIOC_S_FMT, (void *)&fmt);
  frameSize = fmt.fmt.pix.sizeimage;
}

void V4lCamera::initMmap() {
  struct v4l2_requestbuffers req;
  memset(&req, 0x00, sizeof(req));
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  xioctl(fd, VIDIOC_REQBUFS, &req);
  numFrames = req.count;

  for (uint8_t i = 0; i < numFrames; i++) {
    struct v4l2_buffer buf;
    memset(&buf, 0x00, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    xioctl(fd, VIDIOC_QUERYBUF, &buf);
    check(buf.length == frameSize, "Weird buf.length");
    void *map =
        mmap(NULL, frameSize,
             PROT_READ | PROT_WRITE, MAP_SHARED,
             fd, buf.m.offset);
    check(map != MAP_FAILED, "mmap failed");
    frameBuffers[i] = (uint8_t *)map;
  }
}

void V4lCamera::init() {
  initFormat();
  initMmap();
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
    ::close(fd);
    fd = -1;
  }
}

