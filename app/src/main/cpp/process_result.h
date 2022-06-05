//
// Created by Shaotien Lee on 2022/6/5.
//
#include <string>
#include <vector>

#ifndef PPOCR_DEMO_PROCESS_RESULT_H
#define PPOCR_DEMO_PROCESS_RESULT_H
class ProcessResult
{
private:
    bool isSuccessful;
    std::vector<std::string> resTextVector;
    std::vector<float> resTextTrustVector;

public:
    ProcessResult();
    ProcessResult(
            bool isSuccessful_,
            std::vector<std::string> resTextVector_,
            std::vector<float> resTextTrustVector_
            );
    ~ProcessResult();
public:
    bool getIsSuccessful() const;
    std::vector<std::string> getResTextVector() const;
    std::vector<float> getResTextTrustVector() const;


};
#endif //PPOCR_DEMO_PROCESS_RESULT_H
