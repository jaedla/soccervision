#ifndef SOCCERBOT_H
#define SOCCERBOT_H

#include "AndroidBinderThread.h"
#include "AndroidCamera.h"
#include "Command.h"
#include "Controller.h"
#include "FrameSender.h"
#include "PerfDebug.h"
#include "Server.h"
#include "sp.h"
#include <string>
#include "Thread.h"
#include "Vision.h"

class BaseCamera;
class VirtualCamera;
class Blobber;
class ProcessThread;
class FpsCounter;
class Robot;
class Communication;
class CameraTranslator;

class SoccerBot : public Thread {

public:
  SoccerBot();
  ~SoccerBot();

  void setup();
  void run();

  void setupAndroid();
  void setupVision();
  void setupProcessors();
  void setupFpsCounter();
  void setupCameras();
  void setupRobot();
  void setupControllers();
  void setupSignalHandler();
  void setupServer();
  void setupCommunication();
  void setupPerfDebug();

  void renderDebugObjects();
  void addController(std::string name, Controller *controller);
  Controller *getController(std::string name);
  bool setController(std::string name);
  std::string getActiveControllerName();

  void handleServerMessages();
  void handleServerMessage(Server::Message *message);
  void handleGetControllerCommand(Server::Message *message);
  void handleSetControllerCommand(Command::Parameters parameters, Server::Message *message);
  void handleGetStateCommand();
  void handleGetFrameCommand();
  void handleStreamChoiceCommand(Command::Parameters parameters);
  void handleCameraChoiceCommand(Command::Parameters parameters);
  void handleCameraAdjustCommand(Command::Parameters parameters);
  void handleBlobberThresholdCommand(Command::Parameters parameters);
  void handleBlobberClearCommand(Command::Parameters parameters);
  void handleScreenshotCommand(Command::Parameters parameters);
  void handleListScreenshotsCommand(Server::Message *message);

  void handleCommunicationMessages();
  void handleCommunicationMessage(std::string message);

  std::string getStateJSON();

private:
  //bool fetchFrame(BaseCamera* camera, ProcessThread* processor);
  void broadcastFrame();
  void broadcastScreenshots();

  AndroidBinderThread *androidBinderThread;
  BaseCamera *frontCamera;
  PerfDebug *perfDebug;
  AndroidCamera *androidCamera;
  VirtualCamera *virtualFrontCamera;
  Blobber *frontBlobber;
  Vision *frontVision;
  sp<ProcessThread> frontProcessor;
  sp<FrameSender> frameSender;
  CameraTranslator *frontCameraTranslator;
  FpsCounter *fpsCounter;
  Vision::Results *visionResults;
  Robot *robot;
  Controller *activeController;
  sp<Server> server;
  Communication *com;
  ControllerMap controllers;
  std::string activeControllerName;
  std::string activeStreamName;

  bool running;
  bool controllerRequested;
  bool stateRequested;
  bool frameRequested;
  bool useScreenshot;
  float dt;
  float totalTime;

  unsigned char *jpegBuffer;
  unsigned char *screenshotBufferFront;
};

#endif // SOCCERBOT_H
