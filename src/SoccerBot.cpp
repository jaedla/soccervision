#include "AndroidCamera.h"
#include "SoccerBot.h"
#include "VirtualCamera.h"
#include "CameraTranslator.h"
#include "Vision.h"
#include "DebugRenderer.h"
#include "Communication.h"
#include "ProcessThread.h"
#include "FpsCounter.h"
#include "SignalHandler.h"
#include "Config.h"
#include "Util.h"
#include "Robot.h"
#include "Dribbler.h"
#include "Wheel.h"
#include "ManualController.h"
#include "TestController.h"
#include "OffensiveAI.h"
#include "ImageProcessor.h"

#include <iostream>
#include <algorithm>

SoccerBot::SoccerBot() :
    Thread(),
    androidBinderThread(NULL),
    frontCamera(NULL),
    androidCamera(NULL),
    virtualFrontCamera(NULL),
    frontBlobber(NULL),
    frontVision(NULL),
    frontProcessor(NULL),
    frontCameraTranslator(NULL),
    fpsCounter(NULL), visionResults(NULL), robot(NULL), activeController(NULL), server(NULL), com(NULL),
    controllerRequested(false), stateRequested(false), frameRequested(false), useScreenshot(false),
    dt(0.01666f), totalTime(0.0f),
    jpegBuffer(NULL), screenshotBufferFront(NULL) {
  setName("SoccerBot");
}

SoccerBot::~SoccerBot() {
  std::cout << "! Releasing all resources" << std::endl;

  for (std::map<std::string, Controller *>::iterator it = controllers.begin(); it != controllers.end(); it++)
    delete it->second;

  controllers.clear();
  activeController = NULL;

  if (robot != NULL) delete robot;
  robot = NULL;
  if (androidCamera != NULL) delete androidCamera;
  androidCamera = NULL;
  if (virtualFrontCamera != NULL) delete virtualFrontCamera;
  virtualFrontCamera = NULL;
  if (frontCameraTranslator != NULL) delete frontCameraTranslator;
  frontCameraTranslator = NULL;
  if (fpsCounter != NULL) delete fpsCounter;
  fpsCounter = NULL;
  if (frontProcessor != NULL) frontBlobber->saveOptions(Config::blobberConfigFilename);
  if (visionResults != NULL) delete visionResults;
  visionResults = NULL;
  if (frontVision != NULL) delete frontVision;
  frontVision = NULL;
  if (frontBlobber != NULL) delete frontBlobber;
  frontBlobber = NULL;
  if (com != NULL) delete com;
  com = NULL;
  if (jpegBuffer != NULL) delete[] jpegBuffer;
  jpegBuffer = NULL;
  frontCamera = NULL;
  std::cout << "! Resources freed" << std::endl;
}

void SoccerBot::setup() {
  perfDebug = new PerfDebug();
  perfDebug->start();
  setupAndroid();
  setupCommunication();
  setupVision();
  setupFpsCounter();
  setupRobot();
  setupControllers();
  setupSignalHandler();
  setupServer();
}

void SoccerBot::run() {
  std::cout << "! Starting main loop" << std::endl;

  com->start();
  server->start();
  com->send("reset");

  setController(Config::defaultController);

  if (frontCamera->isOpened())
    frontCamera->startAcquisition();

  if (!frontCamera->isOpened()) {
    std::cout << "! Camera wasn't opened, running in test mode" << std::endl;
    while (!SignalHandler::exitRequested) {
      sleep(100);
    }
    return;
  }

  double lastTime = 0;

  while (!SignalHandler::exitRequested) {
    double time = Util::millitime();
    dt = lastTime ? (time - lastTime) : (1.0f / 60.0f);
    totalTime += dt;
    lastTime = time;

    fpsCounter->step();
    // get frame here

    if (frameRequested) {
      //renderDebugObjects();
      //broadcastFrame();
      frameRequested = false;
    }

    handleServerMessages();
    handleCommunicationMessages();

    /*
    if (activeController != NULL)
      activeController->step(dt, visionResults);
    robot->step(dt, visionResults);
    */

    if (server != NULL && stateRequested) {
      server->broadcast(Util::json("state", getStateJSON()));
      stateRequested = false;
    }
  }

  perfDebug->requestStop();
  perfDebug->join();
  com->send("reset");
  std::cout << "! Main loop ended" << std::endl;
}

void SoccerBot::renderDebugObjects() {
  Object *closestBall = visionResults->getClosestBall();
  Object *largestBlueGoal = visionResults->getLargestGoal(Side::BLUE);
  Object *largestYellowGoal = visionResults->getLargestGoal(Side::YELLOW);
  if (closestBall != NULL)
    DebugRenderer::renderObjectHighlight(frontProcessor->getRgb(), closestBall, 255, 0, 0);
  if (largestBlueGoal != NULL)
    DebugRenderer::renderObjectHighlight(frontProcessor->getRgb(), largestBlueGoal, 0, 0, 255);
  if (largestYellowGoal != NULL)
    DebugRenderer::renderObjectHighlight(frontProcessor->getRgb(), largestYellowGoal, 255, 255, 0);
}

void SoccerBot::broadcastFrame() {
  printf("broadcastFrame\n");
  uint8_t *rgb = frontProcessor->getRgb();
  uint8_t *classification = frontProcessor->getClassification();
  int jpegBufferSize = Config::jpegBufferSize;

  if (jpegBuffer == NULL) {
    std::cout << "! Creating frame JPEG buffer of " << jpegBufferSize << " bytes.. ";
    jpegBuffer = new unsigned char[Config::jpegBufferSize];
    std::cout << "done!" << std::endl;
  }

  printf("to jpeg\n");
  if (!ImageProcessor::rgbToJpeg(rgb, jpegBuffer, jpegBufferSize, Config::cameraWidth, Config::cameraHeight)) {
    std::cout << "- Converting RGB image to JPEG failed, probably need to increase buffer size" << std::endl;
    return;
  }

  printf("to base64 %d bytes\n", jpegBufferSize);
  std::string base64Rgb = Util::base64Encode(jpegBuffer, jpegBufferSize);

  /*
  jpegBufferSize = Config::jpegBufferSize;

  printf("to jpeg\n");
  if (!ImageProcessor::rgbToJpeg(classification, jpegBuffer, jpegBufferSize, Config::cameraWidth, Config::cameraHeight)) {
    std::cout << "- Converting classification image to JPEG failed, probably need to increase buffer size" << std::endl;
    return;
  }

  printf("to base64 %d bytes\n", jpegBufferSize);
  std::string base64Classification = Util::base64Encode(jpegBuffer, jpegBufferSize);
  */

  std::string base64Classification;

  std::string frameResponse = Util::json("frame", "{\"rgb\": \"" + base64Rgb + "\",\"classification\": \"" + base64Classification + "\",\"activeStream\":\"" + activeStreamName + "\",\"cameraK\":" + Util::toString(Util::cameraCorrectionK) + ",\"cameraZoom\":" + Util::toString(Util::cameraCorrectionZoom) + "}");

  printf("server->broadcast\n");
  server->broadcast(frameResponse);
  printf("broadcastFrame done\n");
}

void SoccerBot::broadcastScreenshots() {
  std::vector<std::string> screenshotFiles = Util::getFilesInDir(Config::screenshotsDirectory);
  std::vector<std::string> screenshotNames;
  std::string filename;
  std::string screenshotName;
  int dashPos;

  std::cout << "! Screenshots:" << std::endl;

  for (std::vector<std::string>::const_iterator it = screenshotFiles.begin(); it != screenshotFiles.end(); it++) {
    filename = *it;

    //std::cout << "  > " << filename << std::endl;

    dashPos = Util::strpos(filename, "-");

    if (dashPos != -1) {
      screenshotName = filename.substr(0, dashPos);

      if (std::find(screenshotNames.begin(), screenshotNames.end(), screenshotName) == screenshotNames.end()) {
        screenshotNames.push_back(screenshotName);

        std::cout << "  > " << screenshotName << std::endl;
      }
    }
  }

  std::string response = Util::json("screenshots", Util::toString(screenshotNames));

  server->broadcast(response);
}

void SoccerBot::setupVision() {
  std::cout << "! Setting up vision.. " << std::endl;

  frontBlobber = new Blobber();
  frontBlobber->initialize(Config::cameraWidth, Config::cameraHeight);
  frontBlobber->loadOptions(Config::blobberConfigFilename);
  frontCameraTranslator = new CameraTranslator();

  /*
  std::cout << "  > loading front camera undistorion mappings.. ";
  fflush(stdout);

  frontCameraTranslator->loadUndistortionMapping(
    Config::undistortMappingFilenameFrontX,
    Config::undistortMappingFilenameFrontY
  );

  std::cout << "done!" << std::endl;
  */

  // TODO Add to config or load from file
  frontCameraTranslator->setConstants(
    //-67.5f, 256000.0f, 189.5f,
    -49.274208f, 238068.968750f, 173.179504f,
    -2.0971206246161431e-001f, 1.3804267164342868e-001f, -1.2815466847742417e-002f,
    -16.678467f, 1.2402373257077263e+003f,
    Config::cameraWidth, Config::cameraHeight
  );

  frontVision = new Vision(frontBlobber, frontCameraTranslator, Dir::FRONT, Config::cameraWidth, Config::cameraHeight);
  visionResults = new Vision::Results();

  setupCameras();
  setupProcessors();
  frameSender = make_sp<FrameSender>();

  androidCamera->addWorker(frontProcessor);
  androidCamera->addWorker(frameSender);
}

void SoccerBot::setupProcessors() {
  std::cout << "! Setting up processor thread.. ";
  frontProcessor = make_sp<ProcessThread>("FrontProcessor", frontCamera, frontBlobber, frontVision);
  std::cout << "done!" << std::endl;
}

void SoccerBot::setupFpsCounter() {
  std::cout << "! Setting up fps counter.. ";
  fpsCounter = new FpsCounter();
  std::cout << "done!" << std::endl;
}

void SoccerBot::setupCameras() {
  std::cout << "! Setting up cameras" << std::endl;
  virtualFrontCamera = new VirtualCamera();
  androidCamera = new AndroidCamera();
  androidCamera->open();
  frontCamera = androidCamera;
}

void SoccerBot::setupRobot() {
  std::cout << "! Setting up the robot.. ";
  robot = new Robot(com);
  robot->setup();
  std::cout << "done!" << std::endl;
}

void SoccerBot::setupControllers() {
  std::cout << "! Setting up controllers.. ";

  addController("manual", new ManualController(robot, com));
  addController("test", new TestController(robot, com));
  addController("offensive-ai", new OffensiveAI(robot, com));

  std::cout << "done!" << std::endl;
}

void SoccerBot::setupSignalHandler() {
  SignalHandler::setup();
}

void SoccerBot::setupServer() {
  server = make_sp<Server>();
  frameSender->setServer(server);
}

void SoccerBot::setupAndroid() {
  std::cout << "Starting android binder thread" << std::endl;
  androidBinderThread = new AndroidBinderThread();
  androidBinderThread->start();
}

void SoccerBot::setupCommunication() {
  com = new Communication(Config::communicationHost, Config::communicationPort);
}

void SoccerBot::addController(std::string name, Controller *controller) {
  controllers[name] = controller;
}

Controller *SoccerBot::getController(std::string name) {
  std::map<std::string, Controller *>::iterator result = controllers.find(name);
  if (result == controllers.end())
    return NULL;
  return result->second;
}

bool SoccerBot::setController(std::string name) {
  if (name == "") {
    if (activeController != NULL)
      activeController->onExit();
    activeController = NULL;
    activeControllerName = "";
    controllerRequested = true;
    return true;
  } else {
    std::map<std::string, Controller *>::iterator result = controllers.find(name);
    if (result != controllers.end()) {
      if (activeController != NULL)
        activeController->onExit();
      activeController = result->second;
      activeControllerName = name;
      activeController->onEnter();
      controllerRequested = true;
      return true;
    }
    return false;
  }
}

std::string SoccerBot::getActiveControllerName() {
  return activeControllerName;
}

void SoccerBot::handleServerMessages() {
  Server::Message *message;
  while ((message = server->dequeueMessage()) != NULL) {
    handleServerMessage(message);
    delete message;
  }
}

void SoccerBot::handleServerMessage(Server::Message *message) {
  //std::cout << "! Request from " << message->client->id << ": " << message->content << std::endl;

  if (Command::isValid(message->content)) {
    Command command = Command::parse(message->content);

    if (
      activeController == NULL
      || (!activeController->handleCommand(command) && !activeController->handleRequest(message->content))
    ) {
      if (command.name == "get-controller")
        handleGetControllerCommand(message);
      else if (command.name == "set-controller" && command.parameters.size() == 1)
        handleSetControllerCommand(command.parameters, message);
      else if (command.name == "get-state")
        handleGetStateCommand();
      else if (command.name == "get-frame")
        handleGetFrameCommand();
      else if (command.name == "camera-choice" && command.parameters.size() == 1)
        handleCameraChoiceCommand(command.parameters);
      else if (command.name == "camera-adjust" && command.parameters.size() == 2)
        handleCameraAdjustCommand(command.parameters);
      else if (command.name == "stream-choice" && command.parameters.size() == 1)
        handleStreamChoiceCommand(command.parameters);
      else if (command.name == "blobber-threshold" && command.parameters.size() == 6)
        handleBlobberThresholdCommand(command.parameters);
      else if (command.name == "blobber-clear" && (command.parameters.size() == 0 || command.parameters.size() == 1))
        handleBlobberClearCommand(command.parameters);
      else if (command.name == "screenshot" && command.parameters.size() == 1)
        handleScreenshotCommand(command.parameters);
      else if (command.name == "list-screenshots")
        handleListScreenshotsCommand(message);
      else
        std::cout << "- Unsupported command: " << command.name << " " << Util::toString(command.parameters) << std::endl;
    }
  } else {
    std::cout << "- Message '" << message->content << "' is not a valid command" << std::endl;
  }
}

void SoccerBot::handleGetControllerCommand(Server::Message *message) {
  std::cout << "! Client #" << message->client->id << " requested controller, sending: " << activeControllerName << std::endl;
  message->respond(Util::json("controller", activeControllerName));
}

void SoccerBot::handleSetControllerCommand(Command::Parameters parameters, Server::Message *message) {
  std::string name = parameters[0];
  if (setController(name))
    std::cout << "+ Changed controller to: '" << name << "'" << std::endl;
  else
    std::cout << "- Failed setting controller to '" << name << "'" << std::endl;
  message->respond(Util::json("controller", activeControllerName));
}

void SoccerBot::handleGetStateCommand() {
  stateRequested = true;
}

void SoccerBot::handleGetFrameCommand() {
  frameRequested = true;
}

void SoccerBot::handleCameraChoiceCommand(Command::Parameters parameters) {
}

void SoccerBot::handleCameraAdjustCommand(Command::Parameters parameters) {
  Util::cameraCorrectionK = Util::toFloat(parameters[0]);
  Util::cameraCorrectionZoom = Util::toFloat(parameters[1]);
  std::cout << "! Adjust camera correction k: " << Util::cameraCorrectionK << ", zoom: " << Util::cameraCorrectionZoom << std::endl;
}

void SoccerBot::handleStreamChoiceCommand(Command::Parameters parameters) {
  std::string requestedStream = parameters[0];

  if (requestedStream == "") {
    std::cout << "! Switching to live stream" << std::endl;
    frontProcessor->setCamera(androidCamera);
    frontCamera = androidCamera;
    activeStreamName = requestedStream;
  } else {
    try {
      bool frontSuccess = virtualFrontCamera->loadImage(Config::screenshotsDirectory + "/" + requestedStream + "-front.scr", Config::cameraWidth * Config::cameraHeight * 2);

      if (!frontSuccess) {
        std::cout << "- Loading screenshot '" << requestedStream << "' failed" << std::endl;
        return;
      }

      std::cout << "! Switching to screenshot stream: " << requestedStream << std::endl;
      frontProcessor->setCamera(virtualFrontCamera);
      frontCamera = virtualFrontCamera;
      activeStreamName = requestedStream;
    } catch (std::exception &e) {
      std::cout << "- Failed to load screenshot: " << requestedStream << " (" << e.what() << ")" << std::endl;
    } catch (...) {
      std::cout << "- Failed to load screenshot: " << requestedStream << std::endl;
    }
  }
}

void SoccerBot::handleBlobberThresholdCommand(Command::Parameters parameters) {
  std::string selectedColorName = parameters[0];
  int centerX = Util::toInt(parameters[1]);
  int centerY = Util::toInt(parameters[2]);
  int mode = Util::toInt(parameters[3]);
  int brushRadius = Util::toInt(parameters[4]);
  float stdDev = Util::toFloat(parameters[5]);

  unsigned char *yuyv = frontProcessor->getYuyv();

  ImageProcessor::YUYVRange yuyvRange = ImageProcessor::extractColorRange(yuyv, Config::cameraWidth, Config::cameraHeight, centerX, centerY, brushRadius, stdDev);

  frontBlobber->getColor(selectedColorName)->addThreshold(
    yuyvRange.minY, yuyvRange.maxY,
    yuyvRange.minU, yuyvRange.maxU,
    yuyvRange.minV, yuyvRange.maxV
  );
}

void SoccerBot::handleBlobberClearCommand(Command::Parameters parameters) {
  if (parameters.size() == 1) {
    std::string color = parameters[0];
    frontBlobber->clearColor(color);
  } else {
    frontBlobber->clearColors();
  }
}

void SoccerBot::handleScreenshotCommand(Command::Parameters parameters) {
  std::string name = parameters[0];

  std::cout << "! Storing screenshot: " << name << std::endl;

  ImageProcessor::saveBitmap(
      frontProcessor->getYuyv(),
      Config::screenshotsDirectory + "/" + name + "-front.scr",
      Config::cameraWidth * Config::cameraHeight * 2);

  ImageProcessor::saveJPEG(
      frontProcessor->getRgb(),
      Config::screenshotsDirectory + "/" + name + "-rgb-front.jpeg",
      Config::cameraWidth,
      Config::cameraHeight,
      3);

  ImageProcessor::saveJPEG(
      frontProcessor->getClassification(),
      Config::screenshotsDirectory + "/" + name + "-classification-front.jpeg",
      Config::cameraWidth,
      Config::cameraHeight,
      3);

  broadcastScreenshots();
}

void SoccerBot::handleListScreenshotsCommand(Server::Message *message) {
  broadcastScreenshots();
}

void SoccerBot::handleCommunicationMessages() {
  std::string message;
  while ((message = com->dequeueMessage()) != "")
    handleCommunicationMessage(message);
}

void SoccerBot::handleCommunicationMessage(std::string message) {
  robot->handleCommunicationMessage(message);
  if (activeController != NULL)
    activeController->handleCommunicationMessage(message);
}

std::string SoccerBot::getStateJSON() {
  std::stringstream stream;

  Math::Position pos = robot->getPosition();

  stream << "{";

  stream << "\"robot\":{" << robot->getJSON() << "},";
  stream << "\"dt\":" << dt << ",";
  stream << "\"totalTime\":" << totalTime << ",";
  stream << "\"gotBall\":" << (robot->dribbler->gotBall() ? "true" : "false") << ",";

  if (activeController != NULL) {
    stream << "\"controllerName\": \"" + activeControllerName + "\",";
    std::string controllerInfo = activeController->getJSON();

    if (controllerInfo.length() > 0)
      stream << "\"controllerState\": " << controllerInfo << ",";
    else
      stream << "\"controllerState\": null,";

    stream << "\"targetSide\":" << activeController->getTargetSide() << ",";
    stream << "\"playing\":" << (activeController->isPlaying() ? "true" : "false") << ",";
  } else {
    stream << "\"controllerName\": null,";
    stream << "\"controllerState\": null,";
    stream << "\"targetSide\":" << Side::UNKNOWN << ",";
    stream << "\"playing\":false,";
  }

  stream << "\"fps\":" << fpsCounter->getFps();
  stream << "}";
  return stream.str();
}

