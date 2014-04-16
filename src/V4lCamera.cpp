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
}

V4lCamera::~V4lCamera() {
  close();
}

V4lCamera:: Frame* V4lCamera::getFrame() {
  return NULL;
}

bool V4lCamera::open(int serial) {
  fd = ::open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
  if (fd == -1)
    printf("Failed to open v4l device");
  else
    init();
  /*
  capture = new cv::VideoCapture(0);
  if (capture->isOpened()) {
    capture->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    capture->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    cv::Mat frame;
    *capture >> frame;
    printf("dims=%d %dx%d rowsize=%lu elemsize=%lu\n", frame.dims, frame.size[1], frame.size[0], frame.step[0], frame.step[1]);
  }
  return capture->isOpened();
  */
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
  printf("Image size: %u\n", fmt.fmt.pix.sizeimage);
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

