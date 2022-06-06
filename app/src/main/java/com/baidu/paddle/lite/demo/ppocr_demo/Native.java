package com.baidu.paddle.lite.demo.ppocr_demo;

import android.content.Context;
import android.util.Log;

import com.baidu.paddle.lite.demo.common.SDKExceptions;
import com.baidu.paddle.lite.demo.common.Utils;
import com.baidu.paddle.lite.demo.dto.ProcessResult;

public class Native {
    static {
        System.loadLibrary("Native");
    }

    private long ctx = 0;
    private boolean run_status = false;

    public boolean init(Context mContext,
                        String detModelPath,
                        String clsModelPath,
                        String recModelPath,
                        String configPath,
                        String labelPath,
                        int cputThreadNum,
                        String cpuPowerMode) {
        ctx = nativeInit(
                detModelPath,
                clsModelPath,
                recModelPath,
                configPath,
                labelPath,
                cputThreadNum,
                cpuPowerMode);
        return ctx == 0;
    }

    public boolean release() {
        if (ctx == 0) {
            return false;
        }
        return nativeRelease(ctx);
    }

    public ProcessResult process(
            int inTextureId,
            int outTextureId,
            int textureWidth,
            int textureHeight,
            String savedImagePath) {
        if (ctx == 0) {
            ProcessResult res = new ProcessResult();
            res.setSuccessful(false);
            return res;
        }
        ProcessResult res = nativeProcess(
                ctx,
                inTextureId,
                outTextureId,
                textureWidth,
                textureHeight,
                savedImagePath
        );

        Log.d("DEBUG", "text result length: " + res.getResTextArray().length);
        Log.d("DEBUG", "text score length: " + res.getResTextTrustArray().length);

        if (res.getResTextArray().length != 0) {
            Log.d("DEBUG", "text[0] result: " + res.getResTextArray()[0]);
        }

        return res;
    }


    public static native long nativeInit(String detModelPath,
                                         String clsModelPath,
                                         String recModelPath,
                                         String configPath,
                                         String labelPath,
                                         int cputThreadNum,
                                         String cpuPowerMode);

    public static native boolean nativeRelease(long ctx);

    public static native ProcessResult nativeProcess(long ctx,
                                                     int inTextureId,
                                                     int outTextureId,
                                                     int textureWidth,
                                                     int textureHeight,
                                                     String savedImagePath);
}
