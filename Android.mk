LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := soccervision

LOCAL_SRC_FILES := \
    src/main.cpp \
    src/AndroidBinderThread.cpp \
    src/AndroidCamera.cpp \
    src/SoccerBot.cpp \
    src/Robot.cpp \
    src/Blobber.cpp \
    src/Vision.cpp \
    src/Communication.cpp \
    src/CameraTranslator.cpp \
    src/FpsCounter.cpp \
    src/ProcessThread.cpp \
    src/ManualController.cpp \
    src/TestController.cpp \
    src/OffensiveAI.cpp \
    src/SignalHandler.cpp \
    src/Server.cpp \
    src/Thread.cpp \
    src/DebugRenderer.cpp \
    src/Util.cpp \
    src/ImageProcessor.cpp \
    src/Dribbler.cpp \
    src/Command.cpp \
    src/VirtualCamera.cpp \
    src/Maths.cpp \
    src/Coilgun.cpp \
    src/BallLocalizer.cpp \
    src/ParticleFilterLocalizer.cpp \
    src/OdometerLocalizer.cpp \
    src/Wheel.cpp \
    src/Odometer.cpp \
    src/Tasks.cpp \
    src/Object.cpp \
    src/Canvas.cpp \
    src/BaseAI.cpp \
    src/DebouncedButton.cpp \
    src/WebSocketServer.cpp \
    src/V4lCamera.cpp \
    lib/boost/error_code.cpp \
    lib/jpeg/jpge.cpp \

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/lib/jpeg \
    $(src)/boost_1_55_0 \
    $(src)/websocketpp \
    $(src)/libyuv/trunk/include \
    frameworks/av/services/camera/libcameraservice \
    system/media/camera/include \
    $(TARGET_PROJECT_INCLUDES) \

LOCAL_CPPFLAGS += \
    -std=c++11 \
    -fexceptions \
    -Wno-unused-parameter \
    -Wno-overloaded-virtual \
    -DHAVE_WIDE_CHAR_TYPES \
    -D_WEBSOCKETPP_NOEXCEPT_ \
    -DBOOST_ALL_NO_LIB=1 \

LOCAL_SHARED_LIBRARIES += \
    libbinder \
    libcameraservice \
    libhardware \
    libutils \

LOCAL_LDFLAGS += \
    -L$(src)/libyuv/trunk/out/Debug/ \
    -lyuv \
    -lyuv_neon \
    -Wl,--no-fatal-warnings \

LOCAL_SDK_VERSION := 18
LOCAL_NDK_STL_VARIANT := gnustl_static
LOCAL_CLANG := true

include $(BUILD_EXECUTABLE)

