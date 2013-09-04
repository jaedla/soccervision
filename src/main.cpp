#include "Gui.h"
#include "XimeaCamera.h"
#include "ProcessThread.h"
#include "FpsCounter.h"
#include "Util.h"
#include "Config.h"

#include <iostream>

void setupCamera(XimeaCamera* camera) {
	camera->setExposure(16000);
	camera->setFormat(XI_RAW8);
	camera->setAutoWhiteBalance(false);
	camera->setAutoExposureGain(false);
	camera->setQueueSize(12);

	std::cout << "Camera info:" << std::endl;
	std::cout << "  > Name: " << camera->getName() << std::endl;
	std::cout << "  > Type: " << camera->getDeviceType() << std::endl;
	std::cout << "  > API version: " << camera->getApiVersion() << std::endl;
	std::cout << "  > Driver version: " << camera->getDriverVersion() << std::endl;
	std::cout << "  > Serial number: " << camera->getSerialNumber() << std::endl;
	std::cout << "  > Color: " << (camera->supportsColor() ? "yes" : "no") << std::endl;
	std::cout << "  > Framerate: " << camera->getFramerate() << std::endl;
	std::cout << "  > Available bandwidth: " << camera->getAvailableBandwidth() << std::endl;

	camera->startAcquisition();
}

/*void processCamera(XimeaCamera& camera, Blobber* blobber) {

}*/

int main(int argc, char* argv[]) {
	HWND consoleWindow = GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(0);

    std::cout << "-- Starting Up --" << std::endl;

	// config
	int width = Config::cameraWidth;
	int height = Config::cameraHeight;
	bool debug = true;

	Gui* gui = new Gui(instance);
	FpsCounter* fpsCounter = new FpsCounter();
	DisplayWindow* winRGB1 = gui->createWindow(1280, 1024, "Camera 1 RGB");
	DisplayWindow* winRGB2 = gui->createWindow(1280, 1024, "Camera 2 RGB");
	DisplayWindow* winClassification1 = gui->createWindow(1280, 1024, "Camera 1 Classification");
	DisplayWindow* winClassification2 = gui->createWindow(1280, 1024, "Camera 2 Classification");

	XimeaCamera* camera1 = new XimeaCamera();
	XimeaCamera* camera2 = new XimeaCamera();

	ProcessThread* processor1 = new ProcessThread(width, height);
	ProcessThread* processor2 = new ProcessThread(width, height);

	int cameraSerial1 = 857735761;
	int cameraSerial2 = 857769553;

	if (!camera1->open(cameraSerial2) && !camera1->open(cameraSerial1)) {
		std::cout << "- Opening camera 1 failed" << std::endl;
	}

	if (!camera2->open(cameraSerial1) && !camera2->open(cameraSerial2)) {
		std::cout << "- Opening camera 2 failed" << std::endl;
	}

	if (!camera1->isOpened() && !camera2->isOpened()) {
		std::cout << "- Opening both cameras failed, giving up" << std::endl;

		return 1;
	}

	setupCamera(camera1);
	//setupCamera(camera2);

	std::cout << "! Capturing frames" << std::endl;

	const BaseCamera::Frame* frame = NULL;

	bool running = true;
	bool gotFrame1, gotFrame2;

	//for (int i = 0; i < 60 * 10; i++) {
	while (running) {
		gotFrame1 = false;
		gotFrame2 = false;

		processor1->classify = processor2->classify = debug;
		processor1->convertRGB = processor2->convertRGB = debug;
		processor1->renderBlobs = processor2->renderBlobs = debug;

		//__int64 startTime = Util::timerStart();

		// camera1
		if (camera1->isAcquisitioning()) {

			// get the frame
			//Util::timerStart();
			frame = camera1->getFrame();
			//double getFrameTime = Util::timerEnd();

			// check if there's a new frame
			if (frame != NULL && frame->fresh) {
				gotFrame1 = true;
				processor1->setFrame(frame->data);
			} else {
				std::cout << "- Got empty/old frame from camera 1" << std::endl;
			}
		}

		if (camera2->isAcquisitioning()) {
			frame = camera2->getFrame();

			if (frame != NULL && frame->fresh) {
				gotFrame2 = true;

				processor2->setFrame(frame->data);
			} else {
				std::cout << "- Got empty/old frame from camera 2" << std::endl;
			}
		}

		processor1->start();
		processor2->start();

		processor1->join();
		processor2->join();

		if (debug) {
			if (gotFrame1) {
				winRGB1->setImage(processor1->rgb, false);
				winClassification1->setImage(processor1->classification, false);
			}

			if (gotFrame2) {
				winRGB2->setImage(processor2->rgb, false);
				winClassification2->setImage(processor2->classification, false);
			}

			gui->update();
		}

		//std::cout << "! Total time: " << Util::timerEnd(startTime) << ", " << fpsCounter->getFps() << "FPS" << std::endl << std::endl;
		std::cout << "! " << fpsCounter->getFps() << "FPS" << std::endl;


		fpsCounter->step();
	}

	delete camera1;
	delete camera2;
	delete winRGB1;
	delete winRGB2;
	delete fpsCounter;
	delete gui;

	//camera.open(857735761);

    std::cout << "-- Properly Terminated --" << std::endl;

    return 0;
}
