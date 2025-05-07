cmake_minimum_required(VERSION 3.10)

project(cameracapture VERSION 0.0.1)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library(cameracapture SHARED src/main/c/windows/capture.cpp)
    target_link_libraries(cameracapture mfplat mfreadwrite mf)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_library(cameracapture SHARED src/main/c/linux/capture.c)
    target_link_libraries(cameracapture v4l2 udev)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    enable_language(OBJCXX)

    add_library(cameracapture SHARED src/main/c/darwin/capture.m)
    target_link_libraries(cameracapture "-framework Foundation" "-framework AVFoundation" "-framework CoreMedia" "-framework CoreVideo" "-framework AppKit")
endif()
