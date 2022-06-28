// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pipeline.h"
#include <iostream>

cv::Mat GetRotateCropImage(cv::Mat srcimage,
                           std::vector<std::vector<int>> box) {
    cv::Mat image;
    srcimage.copyTo(image);
    std::vector<std::vector<int>> points = box;

    int x_collect[4] = {box[0][0], box[1][0], box[2][0], box[3][0]};
    int y_collect[4] = {box[0][1], box[1][1], box[2][1], box[3][1]};
    int left = int(*std::min_element(x_collect, x_collect + 4));
    int right = int(*std::max_element(x_collect, x_collect + 4));
    int top = int(*std::min_element(y_collect, y_collect + 4));
    int bottom = int(*std::max_element(y_collect, y_collect + 4));

    cv::Mat img_crop;
    image(cv::Rect(left, top, right - left, bottom - top)).copyTo(img_crop);

    for (int i = 0; i < points.size(); i++) {
        points[i][0] -= left;
        points[i][1] -= top;
    }

    int img_crop_width =
            static_cast<int>(sqrt(pow(points[0][0] - points[1][0], 2) +
                                  pow(points[0][1] - points[1][1], 2)));
    int img_crop_height =
            static_cast<int>(sqrt(pow(points[0][0] - points[3][0], 2) +
                                  pow(points[0][1] - points[3][1], 2)));

    cv::Point2f pts_std[4];
    pts_std[0] = cv::Point2f(0., 0.);
    pts_std[1] = cv::Point2f(img_crop_width, 0.);
    pts_std[2] = cv::Point2f(img_crop_width, img_crop_height);
    pts_std[3] = cv::Point2f(0.f, img_crop_height);

    cv::Point2f pointsf[4];
    pointsf[0] = cv::Point2f(points[0][0], points[0][1]);
    pointsf[1] = cv::Point2f(points[1][0], points[1][1]);
    pointsf[2] = cv::Point2f(points[2][0], points[2][1]);
    pointsf[3] = cv::Point2f(points[3][0], points[3][1]);

    cv::Mat M = cv::getPerspectiveTransform(pointsf, pts_std);

    cv::Mat dst_img;
    cv::warpPerspective(img_crop, dst_img, M,
                        cv::Size(img_crop_width, img_crop_height),
                        cv::BORDER_REPLICATE);

    const float ratio = 1.5;
    if (static_cast<float>(dst_img.rows) >=
        static_cast<float>(dst_img.cols) * ratio) {
        cv::Mat srcCopy = cv::Mat(dst_img.rows, dst_img.cols, dst_img.depth());
        cv::transpose(dst_img, srcCopy);
        cv::flip(srcCopy, srcCopy, 0);
        return srcCopy;
    } else {
        return dst_img;
    }
}

std::vector<std::string> ReadDict(std::string path) {
    std::ifstream in(path);
    std::string filename;
    std::string line;
    std::vector<std::string> m_vec;
    if (in) {
        while (getline(in, line)) {
            m_vec.push_back(line);
        }
    } else {
        std::cout << "no such file" << std::endl;
    }
    return m_vec;
}

std::vector<std::string> split(const std::string &str,
                               const std::string &delim) {
    std::vector<std::string> res;
    if ("" == str)
        return res;
    char *strs = new char[str.length() + 1];
    std::strcpy(strs, str.c_str());

    char *d = new char[delim.length() + 1];
    std::strcpy(d, delim.c_str());

    char *p = std::strtok(strs, d);
    while (p) {
        std::string s = p;
        res.push_back(s);
        p = std::strtok(NULL, d);
    }

    return res;
}

std::map<std::string, double> LoadConfigTxt(std::string config_path) {
    auto config = ReadDict(config_path);

    std::map<std::string, double> dict;
    for (int i = 0; i < config.size(); i++) {
        std::vector<std::string> res = split(config[i], " ");
        dict[res[0]] = stod(res[1]);
    }
    return dict;
}

cv::Mat Visualization(cv::Mat srcimg,
                      std::vector<std::vector<std::vector<int>>> boxes,
                      std::string output_image_path) {
    cv::Point rook_points[boxes.size()][4];
    for (int n = 0; n < boxes.size(); n++) {
        for (int m = 0; m < boxes[0].size(); m++) {
            rook_points[n][m] = cv::Point(static_cast<int>(boxes[n][m][0]),
                                          static_cast<int>(boxes[n][m][1]));
        }
    }
    cv::Mat img_vis;
    srcimg.copyTo(img_vis);
    for (int n = 0; n < boxes.size(); n++) {
        const cv::Point *ppt[1] = {rook_points[n]};
        int npt[] = {4};
        cv::polylines(img_vis, ppt, npt, 1, 1, CV_RGB(0, 255, 0), 2, 8, 0);
    }
    cv::Mat img_vis_bgr;
    cv::cvtColor(img_vis, img_vis_bgr, cv::COLOR_RGBA2BGR);
    cv::imwrite(output_image_path, img_vis_bgr);
    return img_vis;
}

void Pipeline::VisualizeResults(std::vector<std::string> rec_text,
                                std::vector<float> rec_text_score,
                                cv::Mat *rgbaImage,
                                double *visualizeResultsTime) {
    auto t = GetCurrentTime();
    char text[255];
    cv::Scalar color = cv::Scalar(255, 255, 255);
    int font_face = cv::FONT_HERSHEY_PLAIN;
    double font_scale = 1.f;
    float thickness = 1;
    sprintf(text, "OCR results");
    cv::Size text_size =
            cv::getTextSize(text, font_face, font_scale, thickness, nullptr);
    text_size.height *= 1.25f;
    cv::Point2d offset(10, text_size.height + 15);
    cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                thickness);

    for (int i = 0; i < rec_text.size(); i++) {
        LOGD("debug=== line %d %s, %f", i, rec_text[i].c_str(), rec_text_score[i]);
        sprintf(text, "line: %d %s  %f", i, rec_text[i].c_str(), rec_text_score[i]);
        offset.y += text_size.height;
        cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                    thickness);
    }
    *visualizeResultsTime = GetElapsedTime(t);
    LOGD("VisualizeResults costs %f ms", *visualizeResultsTime);
}

void Pipeline::VisualizeStatus(double readGLFBOTime, double writeGLTextureTime,
                               double predictTime,
                               std::vector<std::string> rec_text,
                               std::vector<float> rec_text_score,
                               double visualizeResultsTime,
                               cv::Mat *rgbaImage) {
    char text[255];
    cv::Scalar color = cv::Scalar(255, 255, 255);
    int font_face = cv::FONT_HERSHEY_PLAIN;
    double font_scale = 1.f;
    float thickness = 1;
    sprintf(text, "Read GLFBO time: %.1f ms", readGLFBOTime);
    cv::Size text_size =
            cv::getTextSize(text, font_face, font_scale, thickness, nullptr);
    text_size.height *= 1.25f;
    cv::Point2d offset(10, text_size.height + 15);
    cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                thickness);
    sprintf(text, "Write GLTexture time: %.1f ms", writeGLTextureTime);
    offset.y += text_size.height;
    cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                thickness);
    // predict time
    sprintf(text, "OCR all process time: %.1f ms", predictTime);
    offset.y += text_size.height;
    cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                thickness);
    // Visualize results
    sprintf(text, "Visualize results time: %.1f ms", visualizeResultsTime);
    offset.y += text_size.height;
    cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                thickness);

    // Visualize text results
    sprintf(text, "text results size: %ld", rec_text.size());
    offset.y += text_size.height;
    cv::putText(*rgbaImage, text, offset, font_face, font_scale, color,
                thickness);
}

Pipeline::Pipeline(const std::string &detModelDir,
                   const std::string &clsModelDir,
                   const std::string &recModelDir,
                   const std::string &cPUPowerMode, const int cPUThreadNum,
                   const std::string &config_path,
                   const std::string &dict_path) {
    clsPredictor_.reset(
            new ClsPredictor(clsModelDir, cPUThreadNum, cPUPowerMode));
    detPredictor_.reset(
            new DetPredictor(detModelDir, cPUThreadNum, cPUPowerMode));
    recPredictor_.reset(
            new RecPredictor(recModelDir, cPUThreadNum, cPUPowerMode));
    Config_ = LoadConfigTxt(config_path);
    charactor_dict_ = ReadDict(dict_path);
    charactor_dict_.insert(charactor_dict_.begin(), "#"); // blank char for ctc
    charactor_dict_.push_back(" ");
}


ProcessResult Pipeline::Process_val(int inTextureId, int outTextureId, int textureWidth,
                                    int textureHeight, std::string savedImagePath) {
    double readGLFBOTime = 0, writeGLTextureTime = 0;
    double visualizeResultsTime = 0, predictTime = 0;
    // 448
    int height = 420;
    int width = 420;
    cv::Mat rgbaImage;
    CreateRGBAImageFromGLFBOTexture(textureWidth, textureHeight, &rgbaImage,
                                    &readGLFBOTime);
    // change to 3-channel
    cv::Mat bgrImage;
    cv::cvtColor(rgbaImage, bgrImage, cv::COLOR_RGBA2BGR);
    cv::Mat bgrImage_resize;
    cv::resize(bgrImage, bgrImage_resize, cv::Size(width, height));

    // colorful image for user viewing
    cv::Mat outImageForUserView = bgrImage_resize;

    // 直方图增强对比度
//    cv::equalizeHist(bgrImage_resize, bgrImage_resize);

    /* threshold */
    // Get green channel, which is suitable for ID card
    std::vector<cv::Mat> channels(3);
    cv::split(bgrImage_resize, channels);
    cv::Mat grayImage;
    grayImage = channels[1];

    // threshold
    cv::Mat binImageTotal;
    cv::threshold(grayImage, binImageTotal, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    /* find word zone */
    // medianBlur
    cv::medianBlur(grayImage, grayImage, 1);

    // threshold
    cv::Mat binImage;
    cv::threshold(grayImage, binImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    cv::Mat blurredImage;
    cv::medianBlur(binImage, blurredImage, 3);

    // open: 先腐蚀后膨胀
    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::Mat openedImage;
    cv::morphologyEx(blurredImage, openedImage, cv::MORPH_OPEN, element);

    // emphasize text zone
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat dilatedTextZoneImage;
    cv::dilate(openedImage, dilatedTextZoneImage, element, cv::Point(-1, -1), 6);

    // 把文字区域和原二值图做按位与, 精准提取文字区
    cv::Mat reverse_bin_img;
    cv::bitwise_not(binImageTotal, reverse_bin_img);
    cv::Mat bin_and_res;
    cv::bitwise_and(reverse_bin_img, dilatedTextZoneImage, bin_and_res);

    // 白底黑字识别率更高
    cv::bitwise_not(bin_and_res, bin_and_res);

    cv::Mat out;

    // 分解原先彩图
    cv::bitwise_and(grayImage, grayImage, out, dilatedTextZoneImage);

    // 重新二值化
//    cv::cvtColor(out, out, cv::COLOR_BGR2GRAY);
    cv::threshold(out, out, 100, 255, cv::THRESH_BINARY);
    cv::Mat negativeDialatedTextZoneImage;
    cv::bitwise_not(dilatedTextZoneImage, negativeDialatedTextZoneImage);

    // 生成更精准的白底黑子文字区域
    cv::bitwise_and(out, dilatedTextZoneImage, out);
    cv::bitwise_xor(out,negativeDialatedTextZoneImage, out);

    cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);

    int use_direction_classify = int(Config_["use_direction_classify"]);
    cv::Mat srcimg;
    out.copyTo(srcimg);
    // Stage1: rec
    auto t = GetCurrentTime();
    // det predict
    auto boxes =
            detPredictor_->Predict(srcimg, Config_, nullptr, nullptr, nullptr);

    std::vector<float> mean = {0.5f, 0.5f, 0.5f};
    std::vector<float> scale = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};

    cv::Mat img;
    out.copyTo(img);
    cv::Mat crop_img;

    std::vector<std::string> rec_text;
    std::vector<float> rec_text_score;
    LOGD("debug===boxes: %d", boxes.size());
    for (int i = boxes.size() - 1; i >= 0; i--) {
        crop_img = GetRotateCropImage(img, boxes[i]);
        if (use_direction_classify >= 1) {
            crop_img =
                    clsPredictor_->Predict(crop_img, nullptr, nullptr, nullptr, 0.9);
        }
        auto res = recPredictor_->Predict(crop_img, nullptr, nullptr, nullptr,
                                          charactor_dict_);
        rec_text.push_back(res.first);
        rec_text_score.push_back(res.second);
    }
    predictTime = GetElapsedTime(t);
    // visualization
    auto img_res = Visualization(outImageForUserView, boxes, savedImagePath);
    cv::Mat img_vis;
    cv::resize(img_res, img_vis, cv::Size(textureWidth, textureHeight));
    cv::cvtColor(img_vis, img_vis, cv::COLOR_BGR2RGBA);
    // show ocr results on image
    //  VisualizeResults(rec_text, rec_text_score, &img_vis,
    //  &visualizeResultsTime);
    VisualizeStatus(readGLFBOTime, writeGLTextureTime, predictTime, rec_text,
                    rec_text_score, visualizeResultsTime, &img_vis);

    WriteRGBAImageBackToGLTexture(img_vis, outTextureId, &writeGLTextureTime);

    ProcessResult resultBean = ProcessResult(
            true,
            rec_text,
            rec_text_score
    );

    return resultBean;
}