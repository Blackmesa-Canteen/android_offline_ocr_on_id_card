package com.baidu.paddle.lite.demo.dto;


/**
 *
 * a object holds camera rectangle zone
 *
 * @author xiaotian li
 */
public class CameraRectangleZone {

    private int topLeft_x;
    private int topLeft_y;
    private int bottomRight_x;
    private int bottomRight_y;

    public CameraRectangleZone() {
    }

    public CameraRectangleZone(int topLeft_x, int topLeft_y, int bottomRight_x, int bottomRight_y) {
        this.topLeft_x = topLeft_x;
        this.topLeft_y = topLeft_y;
        this.bottomRight_x = bottomRight_x;
        this.bottomRight_y = bottomRight_y;
    }

    public int getTopLeft_x() {
        return topLeft_x;
    }

    public void setTopLeft_x(int topLeft_x) {
        this.topLeft_x = topLeft_x;
    }

    public int getTopLeft_y() {
        return topLeft_y;
    }

    public void setTopLeft_y(int topLeft_y) {
        this.topLeft_y = topLeft_y;
    }

    public int getBottomRight_x() {
        return bottomRight_x;
    }

    public void setBottomRight_x(int bottomRight_x) {
        this.bottomRight_x = bottomRight_x;
    }

    public int getBottomRight_y() {
        return bottomRight_y;
    }

    public void setBottomRight_y(int bottomRight_y) {
        this.bottomRight_y = bottomRight_y;
    }
}
