#include <stdio.h>
#include "V4lCamera.h"

V4lCamera::V4lCamera() {
}

V4lCamera::~V4lCamera() {
  close();
}

V4lCamera:: Frame* V4lCamera::getFrame() {
  return NULL;
}

bool V4lCamera::open(int serial) {
  capture = new cv::VideoCapture(0);
  printf("isOpened: %d\n", capture->isOpened());
  if (capture->isOpened()) {
    capture->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    capture->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    cv::Mat frame;
    *capture >> frame;
    printf("dims=%d %dx%d rowsize=%lu elemsize=%lu\n", frame.dims, frame.size[1], frame.size[0], frame.step[0], frame.step[1]);
  }
  return capture->isOpened();
}

bool V4lCamera::isOpened() {
  return capture && capture->isOpened();
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
  if (capture) {
    delete capture;
    capture = NULL;
  }
}

