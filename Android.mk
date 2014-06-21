LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := soccervision

LOCAL_SRC_FILES := \
    lib/boost/error_code.cpp \
    lib/jpeg/jpge.cpp \
    src/AndroidBinderThread.cpp \
    src/AndroidCamera.cpp \
    src/AndroidCameraStream.cpp \
    src/AndroidFrame.cpp \
    src/BallLocalizer.cpp \
    src/BaseAI.cpp \
    src/Blobber.cpp \
    src/CameraTranslator.cpp \
    src/Canvas.cpp \
    src/Check.cpp \
    src/Coilgun.cpp \
    src/Command.cpp \
    src/Communication.cpp \
    src/ConditionSignal.cpp \
    src/DebouncedButton.cpp \
    src/DebugRenderer.cpp \
    src/Dribbler.cpp \
    src/FpsCounter.cpp \
    src/FrameSender.cpp \
    src/ImageProcessor.cpp \
    src/main.cpp \
    src/ManualController.cpp \
    src/Maths.cpp \
    src/Mutex.cpp \
    src/Object.cpp \
    src/Odometer.cpp \
    src/OdometerLocalizer.cpp \
    src/OffensiveAI.cpp \
    src/ParticleFilterLocalizer.cpp \
    src/PerfDebug.cpp \
    src/ProcessThread.cpp \
    src/Robot.cpp \
    src/ScopedMutex.cpp \
    src/Server.cpp \
    src/SharedFlag.cpp \
    src/SignalHandler.cpp \
    src/SoccerBot.cpp \
    src/Tasks.cpp \
    src/TestController.cpp \
    src/Thread.cpp \
    src/Timer.cpp \
    src/Util.cpp \
    src/VirtualCamera.cpp \
    src/Vision.cpp \
    src/WaitableFlag.cpp \
    src/WebSocketServer.cpp \
    src/Wheel.cpp \

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
    libcamera_client \
    libcameraservice \
    libgui \
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
LOCAL_ADDRESS_SANITIZER := true

include $(BUILD_EXECUTABLE)

