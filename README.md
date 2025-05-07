# camera capture

A Java interface for capturing camera frames natively. The interface
from Java to the native code is generated with [jextract](https://github.com/openjdk/jextract).

## Compiling native code

Native compilation is done in a CMake project. The following steps
will generate a shared native library that can then be used from Java.

```
cmake -B build
cmake --build build --config Release
```

### Linux

We use [libv4l2](https://www.kernel.org/doc/html/v4.8/media/uapi/v4l/v4l2.html) to detect
the camera and capture frames. Make sure you have the libv4l2 developer package installed
for your linux distribution.

### macOS

The implementation for macOS relies on the `AVFoundation` framework.

### Windows

The Windows implementation uses Windows Media Framework. Running `cmake`
should be done from within a `Developer Command Prompt for Visual Studio`.

## Compiling the java library

The java library can be built with maven. It requires the following environment
variables to be set:

  - JAVA_HOME: path to a JDK 24 installation folder
  - JEXTRACT_BIN: full path to the jextract script located in the bin folder of jextract
