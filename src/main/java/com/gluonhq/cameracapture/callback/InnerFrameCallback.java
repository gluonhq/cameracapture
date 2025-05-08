package com.gluonhq.cameracapture.callback;

public interface InnerFrameCallback {

    void gotFrame(int width, int height, int format, byte[] data);

}
