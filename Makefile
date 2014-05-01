out = out

soccervision_sources_list = main.cpp SoccerBot.cpp Robot.cpp Blobber.cpp Vision.cpp Communication.cpp CameraTranslator.cpp FpsCounter.cpp ProcessThread.cpp ManualController.cpp TestController.cpp OffensiveAI.cpp SignalHandler.cpp Server.cpp Thread.cpp DebugRenderer.cpp Util.cpp ImageProcessor.cpp Dribbler.cpp Command.cpp VirtualCamera.cpp Maths.cpp Coilgun.cpp BallLocalizer.cpp ParticleFilterLocalizer.cpp OdometerLocalizer.cpp Wheel.cpp Odometer.cpp Tasks.cpp Object.cpp Canvas.cpp BaseAI.cpp DebouncedButton.cpp WebSocketServer.cpp V4lCamera.cpp

soccervision_sources = $(soccervision_sources_list:%.cpp=src/%.cpp)
soccervision_objects = $(soccervision_sources:%.cpp=$(out)/%.o)
libjpeg_sources = lib/jpeg/jpge.cpp
libjpeg_objects = $(libjpeg_sources:%.cpp=$(out)/%.o)
boost_sources = lib/boost/error_code.cpp
boost_objects = $(boost_sources:%.cpp=$(out)/%.o)
android_sources = src/AndroidBinderThread.cpp src/AndroidCamera.cpp
android_objects = $(android_sources:%.cpp=$(out)/%.o)

objects = $(soccervision_objects) $(libjpeg_objects) $(boost_objects) $(android_objects)
include_deps = $(objects:%.o=%.d)

debug_flags = -g -fno-inline-functions -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls
includes = include $(src)/boost_1_55_0 $(src)/websocketpp lib/armadillo/include $(src)/libyuv/trunk/include lib/jpeg $(src)/opencv/ $(android_source)/system/core/include $(android_source)/frameworks/native/include
include_flags = $(includes:%=-I%)
android_compile_flags = -fexceptions -fno-rtti -Wno-psabi -fpic -funwind-tables -finline-limit=64 -fsigned-char -no-canonical-prefixes -fdata-sections -ffunction-sections -Wa,--noexecstack -fsigned-char -fdiagnostics-show-option -fomit-frame-pointer -fvisibility=hidden -fvisibility-inlines-hidden -fomit-frame-pointer -Os -fno-strict-aliasing -finline-limit=64 -fuse-cxa-atexit -fPIC -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wno-narrowing -Wno-delete-non-virtual-dtor 
android_libs_dir = $(android_source)/out/target/product/hammerhead/system/lib
android_libs = binder
android_linker_so_list = $(android_libs:%=$(android_libs_dir)/lib%.so)

$(out)/soccervision: $(objects)
	./android-compiler-clang link -o $(out)/soccervision $(objects) $(android_linker_so_list) $(src)/libyuv/trunk/out/Debug/libyuv.a $(src)/libyuv/trunk/out/Debug/libyuv_neon.a -lgcc -lc -lm

$(soccervision_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler-clang compile $(debug_flags) $(include_flags) -std=c++11 -MMD -c $< -o $@

$(libjpeg_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler-clang compile $(debug_flags) -Ilib/jpeg -std=c++11 -MMD -c $< -o $@

$(boost_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler-clang compile $(debug_flags) -I$(src)/boost_1_55_0 -O3 -finline-functions -Wno-inline -Wall -pthread -DBOOST_ALL_NO_LIB=1 -BOOST_SYSTEM_DYN_LINK=1 -DNDEBUG -MMD -c $< -o $@

$(android_objects): $(out)/%.o : %.cpp
	@mkdir -p $(dir $@)
	./android-compiler-gcc compile $(android_compile_flags) -Iinclude -MMD -c $< -o $@

-include $(include_deps)

clean:
	rm -rf $(out)

