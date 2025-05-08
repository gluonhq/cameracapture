package com.gluonhq.cameracapture;

/**
 * Holds the information of a single captured frame of the camera,
 * including the width and height, the format of the data and the
 * pixels themselves.
 */
public record Frame(int width, int height, int format, byte[] data) {

    public static final int FORMAT_RGB = 101;
    public static final int FORMAT_RGBA = 102;
    public static final int FORMAT_JPEG = 201;

}
