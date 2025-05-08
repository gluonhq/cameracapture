#include <windows.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <iostream>
#include <vector>
#include <comdef.h>
#include "../cameracapture.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mf.lib")

static int active = 0;
static GotFrameCallback got_frame_callback = nullptr;

void CheckHR(HRESULT hr, const char* msg) {
    if (FAILED(hr)) {
        std::cerr << msg << " (HRESULT=0x" << std::hex << hr << ")\n";
    }
}

void PrintGUID(GUID guid, UINT32 w, UINT32 h) {
    if (guid == MFVideoFormat_RGB8) std::cout << "MFVideoFormat_RGB8 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_RGB555) std::cout << "MFVideoFormat_RGB555 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_RGB565) std::cout << "MFVideoFormat_RGB565 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_RGB24) std::cout << "MFVideoFormat_RGB24 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_RGB32) std::cout << "MFVideoFormat_RGB32 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_ARGB32) std::cout << "MFVideoFormat_ARGB32 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_A2R10G10B10) std::cout << "MFVideoFormat_A2R10G10B10 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_A16B16G16R16F) std::cout << "MFVideoFormat_A16B16G16R16F [" << w << ", " << h << "]\n";

    if (guid == MFVideoFormat_AI44) std::cout << "MFVideoFormat_AI44 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_AYUV) std::cout << "MFVideoFormat_AYUV [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_I420) std::cout << "MFVideoFormat_I420 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_IYUV) std::cout << "MFVideoFormat_IYUV [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_NV11) std::cout << "MFVideoFormat_NV11 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_NV12) std::cout << "MFVideoFormat_NV12 [" << w << ", " << h << "]\n";
    // if (guid == MFVideoFormat_NV21) std::cout << "MFVideoFormat_NV21 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_UYVY) std::cout << "MFVideoFormat_UYVY [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y41P) std::cout << "MFVideoFormat_Y41P [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y41T) std::cout << "MFVideoFormat_Y41T [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y42T) std::cout << "MFVideoFormat_Y42T [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_YUY2) std::cout << "MFVideoFormat_YUY2 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_YVU9) std::cout << "MFVideoFormat_YVU9 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_YV12) std::cout << "MFVideoFormat_YV12 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_YVYU) std::cout << "MFVideoFormat_YVYU [" << w << ", " << h << "]\n";

    if (guid == MFVideoFormat_P010) std::cout << "MFVideoFormat_P010 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_P016) std::cout << "MFVideoFormat_P016 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_P210) std::cout << "MFVideoFormat_P210 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_P216) std::cout << "MFVideoFormat_P216 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_v210) std::cout << "MFVideoFormat_v210 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_v216) std::cout << "MFVideoFormat_v216 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_v410) std::cout << "MFVideoFormat_v410 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y210) std::cout << "MFVideoFormat_Y210 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y216) std::cout << "MFVideoFormat_Y216 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y410) std::cout << "MFVideoFormat_Y410 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_Y416) std::cout << "MFVideoFormat_Y416 [" << w << ", " << h << "]\n";

    if (guid == MFVideoFormat_DV25) std::cout << "MFVideoFormat_DV25 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_DV50) std::cout << "MFVideoFormat_DV50 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_DVC) std::cout << "MFVideoFormat_DVC [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_DVH1) std::cout << "MFVideoFormat_DVH1 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_DVHD) std::cout << "MFVideoFormat_DVHD [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_DVSD) std::cout << "MFVideoFormat_DVSD [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_DVSL) std::cout << "MFVideoFormat_DVSL [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_H263) std::cout << "MFVideoFormat_H263 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_H264) std::cout << "MFVideoFormat_H264 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_H265) std::cout << "MFVideoFormat_H265 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_H264_ES) std::cout << "MFVideoFormat_H264_ES [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_HEVC) std::cout << "MFVideoFormat_HEVC [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_HEVC_ES) std::cout << "MFVideoFormat_HEVC_ES [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_HEVC_HDCP) std::cout << "MFVideoFormat_HEVC_HDCP [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_M4S2) std::cout << "MFVideoFormat_M4S2 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MJPG) std::cout << "MFVideoFormat_MJPG [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MP43) std::cout << "MFVideoFormat_MP43 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MP4S) std::cout << "MFVideoFormat_MP4S [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MP4V) std::cout << "MFVideoFormat_MP4V [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MPEG2) std::cout << "MFVideoFormat_MPEG2 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_VP80) std::cout << "MFVideoFormat_VP80 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_VP90) std::cout << "MFVideoFormat_VP90 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MPG1) std::cout << "MFVideoFormat_MPG1 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MSS1) std::cout << "MFVideoFormat_MSS1 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_MSS2) std::cout << "MFVideoFormat_MSS2 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_WMV1) std::cout << "MFVideoFormat_WMV1 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_WMV2) std::cout << "MFVideoFormat_WMV2 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_WMV3) std::cout << "MFVideoFormat_WMV3 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_WVC1) std::cout << "MFVideoFormat_WVC1 [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_420O) std::cout << "MFVideoFormat_420O [" << w << ", " << h << "]\n";
    if (guid == MFVideoFormat_AV1) std::cout << "MFVideoFormat_AV1 [" << w << ", " << h << "]\n";

    std::flush(std::cout);
}

void yuvToRgb(BYTE* pYuvData, BYTE* pRgbData, UINT32 width, UINT32 height) {
    int u, v, y, y2;
    for (UINT32 i = 0; i < width * height / 2; i++) {
        y = pYuvData[i * 4];
        u = pYuvData[i * 4 + 1];
        y2 = pYuvData[i * 4 + 2];
        v = pYuvData[i * 4 + 3];

        int r1 = y + (1.370705 * (v-128));
        int g1 = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
        int b1 = y + (1.732446 * (u-128));
        if (r1 > 255) r1 = 255;
        if (g1 > 255) g1 = 255;
        if (b1 > 255) b1 = 255;
        if (r1 < 0) r1 = 0;
        if (g1 < 0) g1 = 0;
        if (b1 < 0) b1 = 0;
        pRgbData[i * 6] = r1;
        pRgbData[i * 6 + 1] = g1;
        pRgbData[i * 6 + 2] = b1;

        int r2 = y2 + (1.370705 * (v-128));
        int g2 = y2 - (0.698001 * (v-128)) - (0.337633 * (u-128));
        int b2 = y2 + (1.732446 * (u-128));
        if (r2 > 255) r2 = 255;
        if (g2 > 255) g2 = 255;
        if (b2 > 255) b2 = 255;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        pRgbData[i * 6 + 3] = r2;
        pRgbData[i * 6 + 4] = g2;
        pRgbData[i * 6 + 5] = b2;
    }
}

extern "C" {

    void got_frame(GotFrameCallback cb) {
        got_frame_callback = cb;
    }

    void init(void) {
    }

    int start(void) {
        IMFMediaSource* pVideoSource = nullptr;
        IMFSourceReader* pReader = nullptr;

        // Initialize Media Foundation
        HRESULT hr = MFStartup(MF_VERSION);
        CheckHR(hr, "MFStartup failed");
        if (FAILED(hr)) {
            return hr;
        }

        // Activate default video device
        IMFAttributes* pConfig = nullptr;
        hr = MFCreateAttributes(&pConfig, 1);
        CheckHR(hr, "Failed to create attributes");
        if (FAILED(hr)) {
            return hr;
        }
        hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        CheckHR(hr, "Failed to set attribute");
        if (FAILED(hr)) {
            return hr;
        }

        IMFActivate** ppDevices = nullptr;
        UINT32 count = 0;
        hr = MFEnumDeviceSources(pConfig, &ppDevices, &count);
        CheckHR(hr, "Device enumeration failed");
        if (FAILED(hr)) {
            return hr;
        }

        if (count == 0) {
            std::cerr << "No video devices found.\n";
            return -1;
        }

        // Create media source
        hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(&pVideoSource));
        CheckHR(hr, "Failed to activate video source");
        if (FAILED(hr)) {
            return hr;
        }

        // Create reader
        hr = MFCreateSourceReaderFromMediaSource(pVideoSource, nullptr, &pReader);
        CheckHR(hr, "Failed to create source reader");
        if (FAILED(hr)) {
            return hr;
        }

        HRESULT hrMediaType = S_OK;
        DWORD dwMediaTypeIndex = 0;
        IMFMediaType* pType = nullptr;

        while (SUCCEEDED(hrMediaType)) {
            hrMediaType = pReader->GetNativeMediaType(0, dwMediaTypeIndex, &pType);
            if (hr == MF_E_NO_MORE_TYPES)
            {
                hr = S_OK;
                break;
            }
            else if (SUCCEEDED(hrMediaType))
            {
                GUID s;
                hr = pType->GetGUID(MF_MT_SUBTYPE, &s);
                CheckHR(hr, "Failed to get sub media type");

                UINT32 w = 0;
                UINT32 h = 0;
                hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &w, &h);
                CheckHR(hr, "Failed to get media type attribute size");

                // PrintGUID(s, w, h);
        
                if (s == MFVideoFormat_YUY2 || s == MFVideoFormat_MJPG) {
                    break;
                }

                pType->Release();
            }

            dwMediaTypeIndex++;
        }

        if (pType == nullptr) {
            hr = pReader->GetNativeMediaType(0, 0, &pType);
            CheckHR(hr, "Failed to set media type");
            if (FAILED(hr)) {
                return hr;
            }
        }

        GUID mediaSubType;
        hr = pType->GetGUID(MF_MT_SUBTYPE, &mediaSubType);
        CheckHR(hr, "Failed to get sub media type");
        if (FAILED(hr)) {
            return hr;
        }

        UINT32 width = 0;
        UINT32 height = 0;
        hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
        CheckHR(hr, "Failed to get attribute size");
        if (FAILED(hr)) {
            return hr;
        }

        hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pType);
        CheckHR(hr, "Failed to set media type");
        if (FAILED(hr)) {
            return hr;
        }

        std::cout << "Capturing frames...\n";
        active = 1;

        // Start reading
        while (active > 0) {
            DWORD streamIndex, flags;
            LONGLONG timestamp;
            IMFSample* pSample = nullptr;

            hr = pReader->ReadSample(
                (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                0,
                &streamIndex,
                &flags,
                &timestamp,
                &pSample
            );

            CheckHR(hr, "Failed to read sample");
            if (FAILED(hr)) {
                break;
            }

            if (pSample) {
                IMFMediaBuffer* pBuffer = nullptr;
                BYTE* pData = nullptr;
                DWORD maxLen = 0, curLen = 0;

                hr = pSample->ConvertToContiguousBuffer(&pBuffer);
                CheckHR(hr, "Failed to get buffer");
                if (FAILED(hr)) {
                    break;
                }
                hr = pBuffer->Lock(&pData, &maxLen, &curLen);
                CheckHR(hr, "Failed to lock buffer");
                if (FAILED(hr)) {
                    break;
                }


                if (got_frame_callback != nullptr) {
                    if (mediaSubType == MFVideoFormat_YUY2) {
                        PBYTE pRgbData = new BYTE[width * height * 3];
                        yuvToRgb(pData, pRgbData, width, height);
    
                        got_frame_callback(width, height, 101, pRgbData, width * height * 3);

                        delete [] pRgbData;
                    } else if (mediaSubType == MFVideoFormat_MJPG) {
                        got_frame_callback(width, height, 201, pData, curLen);
                    }
                }

                pBuffer->Unlock();
                pBuffer->Release();
                pSample->Release();
            }

            Sleep(30);  // Simple throttling
        }

        std::cout << "Cleaning up...\n";

        // Cleanup
        pType->Release();
        for (UINT32 i = 0; i < count; ++i) ppDevices[i]->Release();
        CoTaskMemFree(ppDevices);
        pConfig->Release();
        pReader->Release();
        pVideoSource->Release();
        MFShutdown();

        if (FAILED(hr)) {
            std::cerr << "Failed to read sample\n";
            return hr;
        }

        return 0;
    }

    void stop(void) {
        active = 0;
    }
}
