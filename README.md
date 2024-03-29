# OCR Android 离线中国身份证识别 Demo 使用指南
# Offline OCR Android Chinese ID card identification Demo Usage guide

## 简介 Intro
Android用离线身份证OCR识别. 包含了CV图像预处理, 以及OCR文字识别.

界面包含一个身份证拍摄框, 支持打开闪光灯. 会实时识别, 如果所需内容已完全识别, 就会跳转新的activity包含身份证信息.

**注意: 在第一次运行会要权限, 取得权限后可能会闪退, 属正常现象. 以后打开就可以正常使用.**

安装包地址:
[Release页面](https://github.com/Blackmesa-Canteen/android_ocr_demo/releases/tag/v0.2.0-alpha)

Android Chinese ID card identification with offline OCR. It includes CV image preprocessing and OCR text recognition.

The screen contains an ID card photo box and supports the flash. It will be recognized in real time, and if the required content is fully recognized, it will jump to a new activity containing ID information.

**Note: It is normal that the app asks for permission on the first run. After obtaining permission, it may blink back. You can open it later and use it normally.**
Installation package address:[Release page](https://github.com/Blackmesa-Canteen/android_ocr_demo/releases/tag/v0.2.0-alpha)

## 源码部署 Run the src
是可以正常运行的, 如果您这边出了问题, 大概率是缺失了paddleLite预测依赖库和openCV依赖库, 请参考以下开源项目的安装说明配置好依赖库:
[Paddle-Lite-Demo](https://github.com/PaddlePaddle/Paddle-Lite-Demo)

It should work. if not, it could be missing paddleLite predictive dependency library and openCV dependency library, please refer to the following open source project installation instructions to configure the dependency library: [Paddle-Lite-Demo](https://github.com/PaddlePaddle/Paddle-Lite-Demo)
 
 ## 升级预测模型 Upgrade models
 **注意：**
 如果预测模型有版本升级，建议同步更新 OPT 优化后的模型。例如，预测库升级至 2.10—rc 版本，需要做以下操作：
If the version of the prediction model has been upgraded, it is recommended to update the OPT optimized model simultaneously. For example, to upgrade the predictive library to version 2.10-RC, you need to do the following:

 ```shell
 # 下载 PaddleOCR V2.0 版本的中英文 inference 模型
 wget  https://paddleocr.bj.bcebos.com/dygraph_v2.0/slim/ch_ppocr_mobile_v2.0_det_slim_infer.tar && tar xf  ch_ppocr_mobile_v2.0_det_slim_infer.tar
 wget  https://paddleocr.bj.bcebos.com/dygraph_v2.0/slim/ch_ppocr_mobile_v2.0_rec_slim_infer.tar && tar xf  ch_ppocr_mobile_v2.0_rec_slim_infer.tar
 wget  https://paddleocr.bj.bcebos.com/dygraph_v2.0/slim/ch_ppocr_mobile_v2.0_cls_slim_infer.tar && tar xf  ch_ppocr_mobile_v2.0_cls_slim_infer.tar
 # 获取 2.10 版本的 MAC 系统的 OPT 工具
 wget https://github.com/PaddlePaddle/Paddle-Lite/releases/download/v2.10-rc/opt_mac
 # 转换 V2.0 检测模型
 ./opt --model_file=./ch_ppocr_mobile_v2.0_det_slim_infer/inference.pdmodel  --param_file=./ch_ppocr_mobile_v2.0_det_slim_infer/inference.pdiparams  --optimize_out=./ch_ppocr_mobile_v2.0_det_slim_opt --valid_targets=arm  --optimize_out_type=naive_buffer
 # 转换 V2.0 识别模型
 ./opt --model_file=./ch_ppocr_mobile_v2.0_rec_slim_infer/inference.pdmodel  --param_file=./ch_ppocr_mobile_v2.0_rec_slim_infer/inference.pdiparams  --optimize_out=./ch_ppocr_mobile_v2.0_rec_slim_opt --valid_targets=arm  --optimize_out_type=naive_buffer
 # 转换 V2.0 方向分类器模型
 ./opt --model_file=./ch_ppocr_mobile_v2.0_cls_slim_infer/inference.pdmodel  --param_file=./ch_ppocr_mobile_v2.0_cls_slim_infer/inference.pdiparams  --optimize_out=./ch_ppocr_mobile_v2.0_cls_slim_opt --valid_targets=arm  --optimize_out_type=naive_buffer
 ```

 ## Demo 代码介绍

 先整体介绍下OCR 文字识别 Demo 的代码结构，然后再从 Java 和 C++ 两部分简要的介绍 Demo 每部分功能.

 <p align="center"><img width="300" height="500" src="https://paddlelite-demo.bj.bcebos.com/demo/ocr/docs_img/android/ppocr_android_app.jpg"/></p>

 1. `PaddleLite/` : 存放 PaddleLite 预测库

 **备注：**
   如需更新预测库，例如更新 Adnroid v8 动态库 `so`，则将新的动态库 `so` 更新到 `PaddleLite/cxx/libs/armv64-v8a` 目录

 2. `OpenCV/` : 存放 OpenCV 预测库

 3. `assets/` : 存放 OCR demo 的模型、测试图片、标签文件及 config 文件

 **备注：**

  - `./assets/ppocr_keys_v1.txt` 是中文字典文件，如果使用的模型是英文数字或其他语言的模型，则需要更换为对应语言的字典.
  - 其他语言的字典文件，可从 PaddleOCR 仓库下载：https://github.com/PaddlePaddle/PaddleOCR/tree/release/2.3/ppocr/utils

 4. `./src/main/cpp` : 存放 C++ 预测代码
     - `cls_process.cc` : 方向分类器的推理全流程，包含预处理、预测和后处理三部分
     - `rec_process.cc` : 识别模型 CRNN 的推理全流程，包含预处理、预测和后处理三部分
     - `det_process.cc` : 检测模型 CRNN 的推理全流程，包含预处理、预测和后处理三部分
     - `det_post_process` : 检测模型 DB 的后处理文件
     - `pipeline.cc` : OCR 文字识别 Demo 推理全流程代码
     - `Native.cc` : 用于 C++ 和 Java 间传递信息桥梁
     - `CMakeLists.txt` : 预测代码的 MakeFile 文件

### Java 端

 * common Java 包
   在 `app/src/java/com/baidu/paddle/lite/demo/common` 目录下，实现摄像头和框架的公共处理，一般不用修改。其中，Utils.java 用于存放一些公用的且与 Java 基类无关的功能，例如模型拷贝、字符串类型转换等
 
 * util Java 包
   在`app/src/main/java/com/baidu/paddle/lite/demo/util`, 里面封装了对身份证信息处理的逻辑.
 * ppocr_demo Java 包
   在 `app/src/java/com/baidu/paddle/lite/demo/ppocr_demo` 目录下，实现 APP 界面消息事件和 Java/C++ 端代码互传的桥梁功能
   * MainActivity
     实现 APP 的创建、运行、释放功能
     重点关注 `checkRun` 函数，实现 APP 界面值向 C++ 端值互传
     
      ```
     public void checkRun() {
                 try {
                 Utils.copyAssets(this, labelPath);
                 String labelRealDir = new File(
                         this.getExternalFilesDir(null),
                         labelPath).getAbsolutePath();
     
                 Utils.copyAssets(this, configPath);
                 String configRealDir = new File(
                         this.getExternalFilesDir(null),
                         configPath).getAbsolutePath();
     
                 Utils.copyAssets(this, detModelPath);
                 String detRealModelDir = new File(
                         this.getExternalFilesDir(null),
                         detModelPath).getAbsolutePath();
     
                 Utils.copyAssets(this, clsModelPath);
                 String clsRealModelDir = new File(
                         this.getExternalFilesDir(null),
                         clsModelPath).getAbsolutePath();
     
                 Utils.copyAssets(this, recModelPath);
                 String recRealModelDir = new File(
                         this.getExternalFilesDir(null),
                         recModelPath).getAbsolutePath();
     
                     predictor.init(
                             this,
                             detRealModelDir,
                             clsRealModelDir,
                             recRealModelDir,
                             configRealDir,
                             labelRealDir,
                             cpuThreadNum,
                             cpuPowerMode);
                 } catch (Throwable e) {
                     e.printStackTrace();
                 }
             }
         ```java
   
   * Native
      实现 Java 与 C++ 端代码互传的桥梁功能
      **备注：**
          Java 的 native 方法和 C++ 的 native 方法要一一对应
   
### C++ 端

* Native
   实现 Java 与 C++ 端代码互传的桥梁功能，将 Java 数值转换为 c++ 数值，调用 c++ 端的完成 OCR 文字识别功能
   **注意：**
   Native 文件生成方法：
   ```
   cd app/src/java/com/baidu/paddle/lite/demo/ppocr_demo
   # 在当前目录会生成包含 Native 方法的头文件，用户可以将其内容拷贝至 `cpp/Native.cc` 中
   javac -classpath D:\dev\android-sdk\platforms\android-29\android.jar -encoding utf8 -h . Native.java 
   ```

 * Pipeline
   实现输入预处理、推理执行和输出后处理的流水线处理，支持多个模型的串行处理

 * Utils
   实现其他辅助功能，如 `NHWC` 格式转 `NCHW` 格式、字符串处理等
   
 * 其他 CC 文件
   * `cls_process.cc` : 方向分类器的推理全流程，包含预处理、预测和后处理三部分
   * `rec_process.cc` : 识别模型 CRNN 的推理全流程，包含预处理、预测和后处理三部分
   * `det_process.cc` : 检测模型 CRNN 的推理全流程，包含预处理、预测和后处理三部分
   * `det_post_process.cc` : 检测模型 DB 的后处理文件


 ## 代码讲解 （使用 Paddle Lite `C++ API` 执行预测）

 该示例基于 C++ API 开发，调用 Paddle Lite `C++s API` 包括以下五步。
 更详细的 `API` 描述参考：[Paddle Lite C++ API ](https://paddle-lite.readthedocs.io/zh/latest/api_reference/c++_api_doc.html)。

 ```c++
 #include <iostream>
 // 引入 C++ API
 #include "include/paddle_api.h"
 #include "include/paddle_use_ops.h"
 #include "include/paddle_use_kernels.h"

 // 1. 设置 MobileConfig
 MobileConfig config;
 config.set_model_from_file(modelPath); // 设置 NaiveBuffer 格式模型路径
 config.set_power_mode(LITE_POWER_NO_BIND); // 设置 CPU 运行模式
 config.set_threads(4); // 设置工作线程数

 // 2. 创建 PaddlePredictor
 std::shared_ptr<PaddlePredictor> predictor = CreatePaddlePredictor<MobileConfig>(config);

 // 3. 设置输入数据
 std::unique_ptr<Tensor> input_tensor(std::move(predictor->GetInput(0)));
 input_tensor->Resize({1, 3, 224, 224});
 auto* data = input_tensor->mutable_data<float>();
 for (int i = 0; i < ShapeProduction(input_tensor->shape()); ++i) {
   data[i] = 1;
 }
 // 如果输入是图片，则可在第三步时将预处理后的图像数据赋值给输入 Tensor

 // 4. 执行预测
 predictor->run();

 // 5. 获取输出数据
 std::unique_ptr<const Tensor> output_tensor(std::move(predictor->GetOutput(0)));
 std::cout << "Output shape " << output_tensor->shape()[1] << std::endl;
 for (int i = 0; i < ShapeProduction(output_tensor->shape()); i += 100) {
   std::cout << "Output[" << i << "]: " << output_tensor->data<float>()[i]
             << std::endl;
 }
 ```

 ## 如何更新模型、输入/输出预处理

 ### 更新模型

 1. 将优化后的新模型存放到目录 `./assets/` 下；
 2. 如果模型名字跟工程中模型名字一模一样，则不需更新代码；否则话，需要修改 `./src/main/java/com/baidu/paddle/lite/demo/ppocr_demo/MainActivity.java` 中代码；
 

 以将检测 det 模型更新为例，则先将优化后的模型存放到 `./assetss/ssd_mv3.nb` 下，然后更新 `MainActivity.java` 中代码

 ```shell
 # 代码文件 `./src/main/java/com/baidu/paddle/lite/demo/ppocr_demo/MainActivity.java`
 Utils.copyAssets(this, detModelPath);
            String detRealModelDir = new File(
                    this.getExternalFilesDir(null),
                    detModelPath).getAbsolutePath();
 # updata
 Utils.copyAssets(this, "ssd_mv3.nb");
             String detRealModelDir = new File(
                     this.getExternalFilesDir(null),
                     detModelPath).getAbsolutePath();
 ```


 **注意：**

 - 如果更新模型中的输入 Tensor、Shape、和 Dtype 发生更新:

   - 更新文字方向分类器模型，则需要更新 `ppocr_demo/src/main/cpp/cls_process.cc` 中 `ClsPredictor::Preprocss` 函数
   - 更新检测模型，则需要更新 `ppocr_demo/src/main/cpp/det_process.cc` 中 `DetPredictor::Preprocss` 函数
   - 更新识别器模型，则需要更新 `ppocr_demo/src/main/cpp/rec_process.cc` 中 `RecPredictor::Preprocss` 函数

 - 如果更新模型中的输出 Tensor 和 Dtype 发生更新:

   - 更新文字方向分类器模型，则需要更新 `ppocr_demo/src/main/cpp/cls_process.cc` 中 `ClsPredictor::Postprocss` 函数
   - 更新检测模型，则需要更新 `ppocr_demo/src/main/cpp/det_process.cc` 中 `DetPredictor::Postprocss` 函数
   - 更新识别器模型，则需要更新 `ppocr_demo/src/main/cpp/rec_process.cc` 中 `RecPredictor::Postprocss` 函数


 - 如果需要更新 `ppocr_keys_v1.txt` 标签文件，则需要将新的标签文件存放在目录 `./assets/` 下，并更新 `./src/main/java/com/baidu/paddle/lite/demo/ppocr_demo/MainActivity.java` 中代码；

```shell
 # 代码文件 `./src/main/java/com/baidu/paddle/lite/demo/ppocr_demo/MainActivity.java`
 Utils.copyAssets(this, labelPath);
             String labelRealDir = new File(
                     this.getExternalFilesDir(null),
                     labelPath).getAbsolutePath();
 # updata
 Utils.copyAssets(this, "new_label.txt");
             String labelRealDir = new File(
                     this.getExternalFilesDir(null),
                     labelPath).getAbsolutePath();
 ```

 ### 更新输入/输出预处理

 1. 更新输入预处理
   - 更新文字方向分类器模型，则需要更新 `ppocr_demo/src/main/cpp/cls_process.cc` 中 `ClsPredictor::Preprocss` 函数
   - 更新检测模型，则需要更新 `ppocr_demo/src/main/cpp/det_process.cc` 中 `DetPredictor::Preprocss` 函数
   - 更新识别器模型，则需要更新 `ppocr_demo/src/main/cpp/rec_process.cc` 中 `RecPredictor::Preprocss` 函数

 2. 更新输出预处理

   - 更新文字方向分类器模型，则需要更新 `ppocr_demo/src/main/cpp/cls_process.cc` 中 `ClsPredictor::Postprocss` 函数
   - 更新检测模型，则需要更新 `ppocr_demo/src/main/cpp/det_process.cc` 中 `DetPredictor::Postprocss` 函数
   - 更新识别器模型，则需要更新 `ppocr_demo/src/main/cpp/rec_process.cc` 中 `RecPredictor::Postprocss` 函数

 ## OCR 文字识别 Demo 工程详解

 OCR 文字识别 Demo 由三个模型一起完成 OCR 文字识别功能，对输入图片先通过 `ch_ppocr_mobile_v2.0_det_slim_opt.nb` 模型做检测处理，然后通过 `ch_ppocr_mobile_v2.0_cls_slim_opt.nb` 模型做文字方向分类处理，最后通过 `ch_ppocr_mobile_v2.0_rec_slim_opt.nb` 模型完成文字识别处理。

 1. `pipeline.cc` : OCR 文字识别 Demo 预测全流程代码
   该文件完成了三个模型串行推理的全流程控制处理，包含整个处理过程的调度处理。

   - `Pipeline::Pipeline(...)` 方法完成调用三个模型类构造函数，完成模型加载和线程数、绑核处理及 predictor 创建处理
   - `Pipeline::Process(...)` 方法用于完成这三个模型串行推理的全流程控制处理

 2. `cls_process.cc` 方向分类器的预测文件
   该文件完成了方向分类器的预处理、预测和后处理过程

   - `ClsPredictor::ClsPredictor()`  方法用于完成模型加载和线程数、绑核处理及 predictor 创建处理
   - `ClsPredictor::Preprocess()` 方法用于模型的预处理
   - `ClsPredictor::Postprocess()` 方法用于模型的后处理

 3. `rec_process.cc` 识别模型 CRNN 的预测文件
   该文件完成了识别模型 CRNN 的预处理、预测和后处理过程

   - `RecPredictor::RecPredictor()`  方法用于完成模型加载和线程数、绑核处理及 predictor 创建处理
   - `RecPredictor::Preprocess()` 方法用于模型的预处理
   - `RecPredictor::Postprocess()` 方法用于模型的后处理

 4. `det_process.cc` 检测模型 DB 的预测文件
   该文件完成了检测模型 DB 的预处理、预测和后处理过程

   - `DetPredictor::DetPredictor()`  方法用于完成模型加载和线程数、绑核处理及 predictor 创建处理
   - `DetPredictor::Preprocess()` 方法用于模型的预处理
   - `DetPredictor::Postprocess()` 方法用于模型的后处理

 5. `db_post_process` 检测模型 DB 的后处理函数，包含 clipper 库的调用
   该文件完成了检测模型 DB 的第三方库调用和其他后处理方法实现

   - `std::vector<std::vector<std::vector<int>>> BoxesFromBitmap(...)` 方法从 Bitmap 图中获取检测框
   - `std::vector<std::vector<std::vector<int>>> FilterTagDetRes(...)` 方法根据识别结果获取目标框位置
