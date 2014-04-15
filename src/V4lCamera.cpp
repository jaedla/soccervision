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

