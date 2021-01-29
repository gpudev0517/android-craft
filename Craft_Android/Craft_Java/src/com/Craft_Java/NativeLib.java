package com.Craft_Java;

import android.content.res.AssetManager;

// Wrapper for native library

public class NativeLib {
    /**
     * @param width the current view width
     * @param height the current view height
     */
    public static native void init(AssetManager mgr, String path);
    public static native void terminate();
    public static native void surfaceChanged(int width, int height);
    public static native void draw();

    public static native void onTouch(float x, float y, int action);
    public static native void onButtonClick(int buttonId);
    public static native void onArrow(int arrowId);
}
