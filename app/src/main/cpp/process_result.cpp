//
// Created by Shaotien Lee on 2022/6/5.
//
#include "process_result.h"

ProcessResult::ProcessResult() {
    isSuccessful = true;
}

ProcessResult::ProcessResult(
        bool isSuccessful_,
        std::vector<std::string> resTextVector_,
        std::vector<float> resTextTrustVector_) {

    isSuccessful = isSuccessful_;
    resTextVector = resTextVector_;
    resTextTrustVector = resTextTrustVector_;
}

ProcessResult::~ProcessResult() {
    std::vector<std::string>().swap(resTextVector);
    std::vector<float>().swap(resTextTrustVector);
}

bool ProcessResult::getIsSuccessful() const {
    return isSuccessful;
}

std::vector<std::string> ProcessResult::getResTextVector() const {
    return resTextVector;
}

std::vector<float> ProcessResult::getResTextTrustVector() const {
    return resTextTrustVector;
}



