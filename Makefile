SRC_SOURCES = main.cpp SoccerBot.cpp Robot.cpp Blobber.cpp Vision.cpp Communication.cpp CameraTranslator.cpp FpsCounter.cpp ProcessThread.cpp ManualController.cpp TestController.cpp OffensiveAI.cpp SignalHandler.cpp Server.cpp Thread.cpp DebugRenderer.cpp Util.cpp ImageProcessor.cpp Dribbler.cpp Command.cpp VirtualCamera.cpp Maths.cpp Coilgun.cpp BallLocalizer.cpp ParticleFilterLocalizer.cpp OdometerLocalizer.cpp Wheel.cpp Odometer.cpp Tasks.cpp Object.cpp Canvas.cpp BaseAI.cpp DebouncedButton.cpp WebSocketServer.cpp
JPEG_SOURCES = lib/jpeg/jpge.cpp
OUT = out
SOURCES = $(SRC_SOURCES:%.cpp=src/%.cpp)
SOURCES += $(JPEG_SOURCES)
OBJECTS = $(SOURCES:%.cpp=$(OUT)/%.o)

$(OUT)/Soccerbot: $(OBJECTS)
	clang++ -fcolor-diagnostics -o $(OUT)/soccervision $(OBJECTS) lib/libyuv/trunk/out/Debug/libyuv.a -lboost_system -lpthread

$(OBJECTS): $(OUT)/%.o : %.cpp
	@mkdir -p $(dir $@)
	clang++ -fcolor-diagnostics -std=c++11 -c -Iinclude -Idependencies/websocket -Ilib/armadillo/include -Ilib/libyuv/trunk/include -Ilib/jpeg $< -o $@

clean:
	rm -rf $(OUT)

