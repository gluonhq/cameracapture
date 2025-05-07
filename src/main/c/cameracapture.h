#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#if defined(_MSC_VER)
    //  Microsoft 
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
#else
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GotFrameCallback)(int width, int height, int format, unsigned char* data, int length);

EXPORT void init(void);
EXPORT int start(void);
EXPORT void stop(void);
EXPORT void got_frame(GotFrameCallback callback);

#ifdef __cplusplus
}
#endif

#endif
