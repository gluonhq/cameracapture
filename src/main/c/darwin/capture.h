#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#import <Accelerate/Accelerate.h>

@interface CaptureVideo : NSViewController <AVCaptureVideoDataOutputSampleBufferDelegate> {}
    - (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection *)connection;
    - (void) startSession;
    - (void) stopSession;
@end

void sendPicturesResult(int width, int height, uint8_t* data, size_t len);
