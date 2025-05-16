/*
 * Copyright (c) 2022, Gluon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLUON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "capture.h"
#include "../cameracapture.h"

static int captureInited = 0;
static GotFrameCallback got_frame_callback = NULL;

// Capture Video
CaptureVideo *_captureVideo;

void got_frame(GotFrameCallback cb) {
    got_frame_callback = cb;
}

void init(void) {
    fprintf(stderr, "initCaptureVideo\n");
    if (captureInited)
    {
        return;
    }
    captureInited = 1;
}

int start(void) {
    if (!captureInited) {
        fprintf(stderr, "Warning: Capture Video not initialized yet\n");
        return -1;
    }
    fprintf(stderr, "Capture video start\n");
    _captureVideo = [[CaptureVideo alloc] init];
    [_captureVideo startSession];
    return 0;
}

void stop(void) {
    if (!captureInited) {
        fprintf(stderr, "Warning: Capture Video not initialized yet\n");
        return;
    }
    fprintf(stderr, "Capture video stop\n");
    if (_captureVideo) {
        [_captureVideo stopSession];
    }
}

@implementation CaptureVideo

- (void) loadView {
    // prevents runtime error: "could not load the nibName:"
}

AVCaptureSession *_session;
AVCaptureDevice *_device;
AVCaptureDeviceInput *_input;
AVCaptureVideoDataOutput *_output;

 - (void) startSession
 {
     _session = [[AVCaptureSession alloc] init];
     _session.sessionPreset = AVCaptureSessionPreset320x240;
     _device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
     NSError *error = nil;

     _input = [AVCaptureDeviceInput deviceInputWithDevice:_device error:&error];
     if (_input) {
        fprintf(stderr, "Add _input: %p", _input);
         [_session addInput:_input];
     } else {
         fprintf(stderr, "Error _input: %p", error);
     }

     _output = [[AVCaptureVideoDataOutput alloc] init];
     if (_output) {
        NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
        NSNumber* val = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
//         NSNumber* val = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange];
        NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:val forKey:key];
        _output.videoSettings = videoSettings;
        _output.alwaysDiscardsLateVideoFrames = true;
        dispatch_queue_t queue = dispatch_queue_create("myQueue", NULL);
        [_output setSampleBufferDelegate:self queue:queue];
        dispatch_release(queue);
        fprintf(stderr, "Add _output: %p", _output);
        [_session addOutput:_output];
     } else {
        fprintf(stderr, "Error _output: %p", error);
     }

     fprintf(stderr, "startRunning: %p", _session);
     [_session startRunning];
 }

- (void) stopSession
{
    fprintf(stderr, "stopSession: %p", _session);
    if([_session isRunning])
    {
        [_session stopRunning];
    }
    [_session removeInput:_input];
    [_session removeOutput:_output];
    _session = nil;
}

- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    {
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
        if (imageBuffer != NULL) {
            CVPixelBufferLockBaseAddress(imageBuffer, 0);
            uint8_t* data = (uint8_t*) CVPixelBufferGetBaseAddress(imageBuffer);
            size_t width = CVPixelBufferGetWidth(imageBuffer);
            size_t height = CVPixelBufferGetHeight(imageBuffer);
            size_t length = CVPixelBufferGetDataSize(imageBuffer);
            size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);

            // need to copy data since unlocking image buffer makes data pointer invalid
            uint8_t* copiedData = (uint8_t*) malloc(length);
            memcpy(copiedData, data, length);
            // swap to RGBA
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    uint8_t* pixel = copiedData + y * bytesPerRow + x * 4;
                    uint8_t temp = pixel[0];      // B
                    pixel[0] = pixel[2];          // R
                    pixel[2] = temp;              // B
                    // G and A remain unchanged
                }
            }

            CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
            dispatch_async(dispatch_get_main_queue(), ^{
                if (got_frame_callback != NULL) {
                    got_frame_callback(width, height, 2, copiedData, length);
                }
                free(copiedData);
            });
       }
    }
    [pool drain];
    pool=nil;
}

@end 
