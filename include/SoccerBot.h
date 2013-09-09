#ifndef SOCCERBOT_H
#define SOCCERBOT_H

#include "Vision.h"
#include <string>

class XimeaCamera;
class Blobber;
class ProcessThread;
class Gui;
class FpsCounter;

class SoccerBot {

public:
	SoccerBot();
	~SoccerBot();

	void setup();
	void run();

	void setupVision();
	void setupProcessors();
	void setupFpsCounter();
	void setupCameras();
	void setupGui();

	bool debugVision;
	bool showGui;

private:
	void setupCamera(std::string name, XimeaCamera* camera);
	bool fetchFrame(XimeaCamera* camera, ProcessThread* processor);

	XimeaCamera* frontCamera;
	XimeaCamera* rearCamera;
	Blobber* frontBlobber;
	Blobber* rearBlobber;
	Vision* frontVision;
	Vision* rearVision;
	ProcessThread* frontProcessor;
	ProcessThread* rearProcessor;
	Gui* gui;
	FpsCounter* fpsCounter;
	Vision::Results* visionResults;

	bool running;
};

#endif // SOCCERBOT_H