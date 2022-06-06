package com.baidu.paddle.lite.demo.util;

import java.util.LinkedList;
import java.util.List;
import java.util.regex.Pattern;

public class IdCardUtil {

    public static List<String> preprocessResStringArray(String[] textArray) {
        List<String> res = new LinkedList<>();

        // remove label text
        for (String str : textArray) {
            boolean isStrLabel = "姓名".equals(str) || "性别".equals(str)
                    || "民族".equals(str) || "出生".equals(str)
                    || "住址".equals(str) || "公民身份号码".equals(str);

            if (isStrLabel) {
                continue;
            }

            res.add(str);
        }

        return res;

    }

    public static boolean isTextName(String text) {
        String regex = "^[\u4e00-\u9fa5.·\u36c3\u4DAE]{0,}$";
        return text.matches(regex);
    }

    public static boolean isTextGender(String text) {
        boolean isStrGender = "男".equals(text) || "女".equals(text)
                || "性别男".equals(text) || "性别女".equals(text)
                || "别男".equals(text) || "别女".equals(text);

        return isStrGender;
    }
}
