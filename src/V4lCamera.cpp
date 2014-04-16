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

static int ioctlHelper(int fd, int request, void *arg, bool allowEagain, int lineNumber) {
  int r;
  do {
    r = ioctl(fd, request, arg);
  } while (-1 == r && EINTR == errno);
  if (r == -1 && !(errno == EAGAIN && allowEagain)) {
    printf("ioctl request %d failed at V4lCamera.cpp:%d, errno=%d\n", request, lineNumber, errno);
    exit(1);
  }
  return r;
}

static void ioctlNoFail(int fd, int request, void *arg, int lineNumber) {
  ioctlHelper(fd, request, arg, false, lineNumber);
}

static void ioctlHandleEagain(int fd, int request, void *arg, int lineNumber) {
  int ret;
  do {
    ret = ioctlHelper(fd, request, arg, true, lineNumber);
  } while (ret == -1);
}

V4lCamera::V4lCamera() : fd(-1) {
  memset(&frame, 0x00, sizeof(frame));
}

V4lCamera::~V4lCamera() {
  close();
}

V4lCamera::Frame* V4lCamera::getFrame() {
  struct v4l2_buffer buf;
  memset(&buf, 0x00, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  ioctlHandleEagain(fd, VIDIOC_DQBUF, &buf, __LINE__);
  frame.data = frameBuffers[buf.index];
  frame.size = frameSize;
  frame.width = width;
  frame.height = height;
  frame.fresh = true;
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

void V4lCamera::initFormat() {
  struct v4l2_format fmt;
  memset(&fmt, 0x00, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  width = 1280;
  height = 720;
  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  ioctlNoFail(fd, VIDIOC_S_FMT, (void *)&fmt, __LINE__);
  frameSize = fmt.fmt.pix.sizeimage;
  check(frameSize == width * height * 2, "Bad fmt.fmt.pix.sizeimage");
}

void V4lCamera::requestMmapBuffers() {
  struct v4l2_requestbuffers req;
  memset(&req, 0x00, sizeof(req));
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  ioctlNoFail(fd, VIDIOC_REQBUFS, &req, __LINE__);
  numFrames = req.count;
  check(numFrames >= 2, "Too few mmap buffers");
}

void V4lCamera::mapBuffers() {
  for (uint8_t i = 0; i < numFrames; i++) {
    struct v4l2_buffer buf;
    memset(&buf, 0x00, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    ioctlNoFail(fd, VIDIOC_QUERYBUF, &buf, __LINE__);
    check(buf.length == frameSize, "Weird buf.length");
    void *map =
        mmap(NULL, frameSize,
             PROT_READ | PROT_WRITE, MAP_SHARED,
             fd, buf.m.offset);
    check(map != MAP_FAILED, "mmap failed");
    frameBuffers[i] = (uint8_t *)map;
  }
}

void V4lCamera::queueBuffer(uint8_t index) {
  struct v4l2_buffer buf;
  memset(&buf, 0x00, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = index;
  ioctlNoFail(fd, VIDIOC_QBUF, &buf, __LINE__);
}

void V4lCamera::queueBuffers() {
  for (uint8_t i = 0; i < numFrames; i++)
    queueBuffer(i);
}

void V4lCamera::init() {
  initFormat();
  requestMmapBuffers();
  mapBuffers();
}

bool V4lCamera::isOpened() {
  return fd != -1;
}

bool V4lCamera::isAcquisitioning() {
  return acquisitioning;
}

void V4lCamera::startAcquisition() {
  queueBuffers();
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ioctlNoFail(fd, VIDIOC_STREAMON, &type, __LINE__);
  acquisitioning = true;
}

void V4lCamera::stopAcquisition() {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ioctlNoFail(fd, VIDIOC_STREAMOFF, &type, __LINE__);
  acquisitioning = false;
}

void V4lCamera::close() {
  if (fd != -1) {
    ::close(fd);
    fd = -1;
  }
}

