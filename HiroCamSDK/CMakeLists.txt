# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_definitions(-DLINUX -D_USE_RECORD)


link_directories(
    "src/main/cpp/dependence/lib/android/PPCS"
    "libs/armeabi"
    "src/main/cpp/dependence/lib/android/ffmpeg/armv7a"
    "src/main/cpp/dependence/lib/android/faac"
    )

include_directories(
    src/main/cpp/
    src/main/cpp/dependence/include/faac
    src/main/cpp/dependence/include/ffmpeg
    src/main/cpp/dependence/include/PPCS
    src/main/cpp/src/platform/BaseDataType
    src/main/cpp/src/platform/Thread
    src/main/cpp/src/platform/Time
    src/main/cpp/src/common/controller
    src/main/cpp/src/common/protocol/common
    src/main/cpp/src/common/component/AudioCodec
    src/main/cpp/src/common/component/P2P
    src/main/cpp/src/common/component/P2P/SYUN/
    src/main/cpp/src/common/component/Record
    src/main/cpp/src/common/component/StreamBuffer
    src/main/cpp/src/common/component/VideoCodec
    src/main/cpp/src/common/component/Record/ps
    src/main/cpp/src/common/component/Record/mp4
    src/main/cpp/src/common/component/Record/avi
)

add_library( # Sets the name of the library.
             HiroCamSDK

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp
             src/main/cpp/HiroCamSDK.cpp
             src/main/cpp/src/common/component/AudioCodec/adpcm.cpp
             src/main/cpp/src/common/component/AudioCodec/AudioCodec.cpp
             src/main/cpp/src/common/component/AudioCodec/g72x.c
             src/main/cpp/src/common/component/AudioCodec/g726.c
             src/main/cpp/src/common/component/AudioCodec/g726_16.c
             src/main/cpp/src/common/component/P2P/SYUN/SYSock.cpp
             src/main/cpp/src/common/component/Record/avi/AVIEncoder.cpp
             src/main/cpp/src/common/component/Record/avi/gmavi.cpp
             src/main/cpp/src/common/component/Record/mp4/MP4Encoder.cpp
             src/main/cpp/src/common/component/Record/ps/psHeader.cpp
             src/main/cpp/src/common/component/Record/ps/PSPacket.cpp
             src/main/cpp/src/common/component/Record/Record.cpp
             src/main/cpp/src/common/component/Record/FilePS.cpp
             src/main/cpp/src/common/component/Record/AVIRecord.cpp
             src/main/cpp/src/common/component/Record/MP4Record.cpp
             src/main/cpp/src/common/component/Record/RecordFactory.cpp
             src/main/cpp/src/common/component/StreamBuffer/CStreamBuffer.cpp
             src/main/cpp/src/common/component/VideoCodec/DecoderManager.cpp
             src/main/cpp/src/common/controller/ApiImpl.cpp
             src/main/cpp/src/common/controller/HandleManager.cpp
             src/main/cpp/src/common/protocol/HiroCam/HiroCamParse.cpp
             src/main/cpp/src/common/protocol/common/DataProc.cpp
             src/main/cpp/src/platform/BaseDataType/BaseFunc.cpp
             src/main/cpp/src/platform/Thread/AutoLock.cpp
             src/main/cpp/src/platform/Thread/MsgQue.hpp
             src/main/cpp/src/platform/Thread/MsgThread.hpp
             src/main/cpp/src/platform/Thread/Thread.cpp
             src/main/cpp/src/platform/Time/HRTime.cpp
             src/main/cpp/src/KeepAlive/KeepAlive.cpp
             src/main/cpp/src/Push/PushSDK.cpp
             src/main/cpp/src/Push/WiPN_API.c
             src/main/cpp/src/Push/WiPN_StringEncDec.c
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       HiroCamSDK jnigraphics

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       libPPCS_API.a
                       libavcodec.a
                       libavformat.a
                       libavutil.a
                       libswresample.a
                       libswscale.a
                       libfaac.a
                       libNDT_API_PPCS.so
                       libDPS_API_PPCS.so
                       libmp4v2.so
                       )