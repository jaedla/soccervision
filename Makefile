out = out
soccervision_sources_list = main.cpp SoccerBot.cpp Robot.cpp Blobber.cpp Vision.cpp Communication.cpp CameraTranslator.cpp FpsCounter.cpp ProcessThread.cpp ManualController.cpp TestController.cpp OffensiveAI.cpp SignalHandler.cpp Server.cpp Thread.cpp DebugRenderer.cpp Util.cpp ImageProcessor.cpp Dribbler.cpp Command.cpp VirtualCamera.cpp Maths.cpp Coilgun.cpp BallLocalizer.cpp ParticleFilterLocalizer.cpp OdometerLocalizer.cpp Wheel.cpp Odometer.cpp Tasks.cpp Object.cpp Canvas.cpp BaseAI.cpp DebouncedButton.cpp WebSocketServer.cpp V4lCamera.cpp
soccervision_sources = $(soccervision_sources_list:%.cpp=src/%.cpp)
soccervision_objects = $(soccervision_sources:%.cpp=$(out)/%.o)
libjpeg_sources = lib/jpeg/jpge.cpp
libjpeg_objects = $(libjpeg_sources:%.cpp=$(out)/%.o)
boost_sources = lib/boost/error_code.cpp
boost_objects = $(boost_sources:%.cpp=$(out)/%.o)
objects = $(soccervision_objects) $(libjpeg_objects) $(boost_objects)

debug_flags = -g -fno-inline-functions -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls
includes = include $(src)/boost_1_55_0 $(src)/websocketpp lib/armadillo/include $(src)/libyuv/trunk/include lib/jpeg
include_flags = $(includes:%=-I%)

$(OUT)/Soccerbot: $(objects)
	./android-compiler link -o $(out)/soccervision $(objects) $(src)/libyuv/trunk/out/Debug/libyuv.a $(src)/libyuv/trunk/out/Debug/libyuv_neon.a -lgcc -lc -lm

$(soccervision_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler compile $(debug_flags) $(include_flags) -std=c++11 -c $< -o $@

$(libjpeg_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler compile $(debug_flags) -Ilib/jpeg -std=c++11 -c $< -o $@

$(boost_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler compile -c -x c++ -O3 -finline-functions -Wno-inline -Wall -pthread -fPIC -DBOOST_ALL_NO_LIB=1 -BOOST_SYSTEM_DYN_LINK=1 -DNDEBUG -I$(src)/boost_1_55_0 $< -o $@

clean:
	rm -rf $(out)

