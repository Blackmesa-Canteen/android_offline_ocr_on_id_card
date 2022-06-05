package com.baidu.paddle.lite.demo.dto;

/**
 *
 * result object of process result of native process method
 *
 * @author xiaotian li
 */

public class ProcessResult {
    private boolean isSuccessful = false;
    private String[] resTextArray;
    private float[] resTextTrustArray;

    public ProcessResult() {
    }

    public ProcessResult(boolean isSuccessful, String[] resTextArray, float[] resTextTrustArray) {
        this.isSuccessful = isSuccessful;
        this.resTextArray = resTextArray;
        this.resTextTrustArray = resTextTrustArray;
    }

    public boolean isSuccessful() {
        return isSuccessful;
    }

    public void setSuccessful(boolean successful) {
        isSuccessful = successful;
    }

    public String[] getResTextArray() {
        return resTextArray;
    }

    public void setResTextArray(String[] resTextArray) {
        this.resTextArray = resTextArray;
    }

    public float[] getResTextTrustArray() {
        return resTextTrustArray;
    }

    public void setResTextTrustArray(float[] resTextTrustArray) {
        this.resTextTrustArray = resTextTrustArray;
    }
}
