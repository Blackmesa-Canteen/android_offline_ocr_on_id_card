// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
// Edit by Xiaotian Li on Jun 5, 2022
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

#include "Native.h"
#include "pipeline.h"
#include "process_result.h"
#include <string>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_baidu_paddle_lite_demo_ocr_db_crnn_Native
 * Method:    nativeInit
 * Signature:
 * (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL
Java_com_baidu_paddle_lite_demo_ppocr_1demo_Native_nativeInit(
    JNIEnv *env, jclass thiz, jstring jDetModelPath, jstring jClsModelPath,
    jstring jRecModelPath, jstring jConfigPath, jstring jLabelPath,
    jint cpuThreadNum, jstring jCPUPowerMode) {
  std::string detModelPath = jstring_to_cpp_string(env, jDetModelPath);
  std::string clsModelPath = jstring_to_cpp_string(env, jClsModelPath);
  std::string recModelPath = jstring_to_cpp_string(env, jRecModelPath);
  std::string configPath = jstring_to_cpp_string(env, jConfigPath);
  std::string labelPath = jstring_to_cpp_string(env, jLabelPath);
  std::string cpuPowerMode = jstring_to_cpp_string(env, jCPUPowerMode);

  return reinterpret_cast<jlong>(
      new Pipeline(detModelPath, clsModelPath, recModelPath, cpuPowerMode,
                   cpuThreadNum, configPath, labelPath));
}

/*
 * Class:     com_baidu_paddle_lite_demo_ocr_db_crnn_Native
 * Method:    nativeRelease
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_baidu_paddle_lite_demo_ppocr_1demo_Native_nativeRelease(JNIEnv *env,
                                                                 jclass thiz,
                                                                 jlong ctx) {
  if (ctx == 0) {
    return JNI_FALSE;
  }
  Pipeline *pipeline = reinterpret_cast<Pipeline *>(ctx);
  delete pipeline;
  return JNI_TRUE;
}

/*
 * Class:     com_baidu_paddle_lite_demo_ocr_db_crnn_Native
 * Method:    nativeProcess
 * Signature: (JIIIILjava/lang/String;)Z
 */
JNIEXPORT jobject JNICALL
Java_com_baidu_paddle_lite_demo_ppocr_1demo_Native_nativeProcess(
    JNIEnv *env, jclass thiz, jlong ctx, jint inTextureId, jint outTextureId,
    jint textureWidth, jint textureHeight, jstring jsavedImagePath) {
  if (ctx == 0) {
    return JNI_FALSE;
  }
  std::string savedImagePath = jstring_to_cpp_string(env, jsavedImagePath);
  Pipeline *pipeline = reinterpret_cast<Pipeline *>(ctx);


// TODO 内存泄漏
  ProcessResult processResultBean = pipeline->Process_val(
          inTextureId,
          outTextureId,
          textureWidth,
          textureHeight,
          savedImagePath
  );

  bool isSuccessful = processResultBean.getIsSuccessful();
  std::vector<std::string> resTextVector = processResultBean.getResTextVector();
  std::vector<float> resTextTrustVector = processResultBean.getResTextTrustVector();
  // wrap result bean
  // TODO 内存泄漏?
//  std::string* resTextArray;
//  resTextArray = new std::string[resTextVector.size()];
//  for (int i = 0; i < resTextVector.size(); i++) {
//    resTextArray[i] = resTextVector[i];
//  }
//
  float* resTextTrustArray;
  resTextTrustArray = new float[resTextTrustVector.size()];
  for (int i = 0; i < resTextTrustVector.size(); i++) {
    resTextTrustArray[i] = resTextTrustVector[i];
  }

  // get bean class from java
  jclass  cls = env -> FindClass("com/baidu/paddle/lite/demo/dto/ProcessResult");

  // get Constructor from java bean
  jmethodID constructorId = env->GetMethodID(
          cls,
          "<init>",
          "()V"
  );

  jobject processResult = env->NewObject(
          cls,
          constructorId
  );

  jfieldID isSuccessfulField = env->GetFieldID(
          cls,
          "isSuccessful",
          "Z");

  jfieldID resTextArrayField = env->GetFieldID(
          cls,
          "resTextArray",
          "[Ljava/lang/String;"
          );

  jfieldID resTextTrustArrayField = env->GetFieldID(
          cls,
          "resTextTrustArray",
          "[F"
          );

  // bolean field
  env->SetBooleanField(processResult, isSuccessfulField, isSuccessful);

  // string[] field
  jobjectArray stringArrayForSaving =
          (jobjectArray)env->NewObjectArray(
                  resTextVector.size(),
                  env->FindClass("java/lang/String"),
                  env->NewStringUTF(""));
  for (int i = 0; i < resTextVector.size(); i++) {
    env->SetObjectArrayElement(
            stringArrayForSaving,
            i,
            env->NewStringUTF(resTextVector[i].c_str())
            );
  }
  env->SetObjectField(processResult, resTextArrayField, stringArrayForSaving);

  // float[] field
  jfloatArray floatArrayForSaving = env->NewFloatArray(resTextTrustVector.size());
  for (int i = 0; i < resTextTrustVector.size(); i++) {
    env->SetFloatArrayRegion(
          floatArrayForSaving,
          0,
          resTextTrustVector.size(),
          resTextTrustArray
          );
  }
  env->SetObjectField(processResult, resTextTrustArrayField, floatArrayForSaving);

  // release memory
  delete resTextTrustArray;
//  delete &processResultBean;

  return processResult;
}

#ifdef __cplusplus
}
#endif
