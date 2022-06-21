package com.baidu.paddle.lite.demo.util;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.regex.Pattern;

/**
 * @author shaotienlee
 */
public class IdCardUtil {

    public static boolean isResultMapSatisfied(Map<String, String> resultMap) {
        String unknownVal = Constants.UNKNOWN;
        return !Objects.equals(resultMap.get(Constants.ID), unknownVal) &&
                !Objects.equals(resultMap.get(Constants.NAME), unknownVal) &&
                !Objects.equals(resultMap.get(Constants.BIRTH_YEAR), unknownVal) &&
                !Objects.equals(resultMap.get(Constants.NATION), unknownVal);
    }


    public static Map<String, String> preprocessResStringMap(String[] textArray) {
        Map<String, String> resultMap = new HashMap<>(8);

        // prefill result set
        resultMap.put(Constants.NAME, Constants.UNKNOWN);
        resultMap.put(Constants.GENDER, Constants.UNKNOWN);
        resultMap.put(Constants.NATION, Constants.UNKNOWN);
        resultMap.put(Constants.BIRTH_YEAR, Constants.UNKNOWN);
        resultMap.put(Constants.BIRTH_MONTH, Constants.UNKNOWN);
        resultMap.put(Constants.BIRTH_DAY, Constants.UNKNOWN);
        resultMap.put(Constants.ADDRESS, Constants.UNKNOWN);
        resultMap.put(Constants.ID, Constants.UNKNOWN);


        for (int i = 0; i < textArray.length; i++) {

            String str = textArray[i];
            // handle name
            boolean isNameNotEdited = Objects.equals(resultMap.get(Constants.NAME), Constants.UNKNOWN);
            boolean isNameWithLabel = i < 3 && (str.contains("姓名") || str.contains("名")) && str.length() > 2;
            boolean isNameWithoutLabel = i < 3 && str.length() >= 2 && isTextName(str);
            if (isNameNotEdited && isNameWithLabel) {
                String nameString = str.replace("姓名", "");
                if (nameString.length() == 0) {
                    // 如果去掉'姓名'后全没了, 姓名在下一个框里
                    continue;
                }

                if (nameString.charAt(0) == '名') {
                    if (nameString.length() == 1) {
                        // 如果只是一个'名'字
                        continue;
                    }
                    nameString = nameString.substring(1);
                }
                resultMap.put(Constants.NAME, nameString);
                continue;
            } else if (isNameNotEdited && isNameWithoutLabel) {
                resultMap.put(Constants.NAME, str);
                continue;
            }

            // handle gender
            boolean isGenderNotEdited = Objects.equals(resultMap.get(Constants.GENDER), Constants.UNKNOWN);
            boolean isGenderWithLabel = i > 0 && (str.contains("性别") || str.contains("别"));
            boolean isGenderWithoutLabel = i > 0 && str.length() == 1 && isTextChinese(str);
            if (isGenderNotEdited && isGenderWithLabel) {
                String genderString = str.replace("性别", "");
                if (genderString.length() == 0) {
                    // 如果去掉'性别'后全没了, 性别在下一个框里
                    continue;
                }

                if (genderString.charAt(0) == '别') {
                    if (genderString.length() == 1) {
                        // 如果只是一个'别'字
                        continue;
                    }
                    genderString = genderString.substring(1);

                    boolean isMale = "男".equals(genderString) || "光".equals(genderString) || "另".equals(genderString);
                    boolean isFemale = "女".equals(genderString);
                    if (isMale) {
                        resultMap.put(Constants.GENDER, "男");
                    } else if (isFemale) {
                        resultMap.put(Constants.GENDER, "女");
                    }
                    continue;
                }
            } else if (isGenderNotEdited && isGenderWithoutLabel) {
                boolean isMale = "男".equals(str) || "光".equals(str) || "另".equals(str);
                boolean isFemale = "女".equals(str);
                if (isMale) {
                    resultMap.put(Constants.GENDER, "男");
                } else if (isFemale) {
                    resultMap.put(Constants.GENDER, "女");
                }
                continue;
            }

            // handle nation
            boolean isNationNotEdited = Objects.equals(resultMap.get(Constants.NATION), Constants.UNKNOWN);
            boolean isNationWithLabel = i > 1 && (str.contains("民族") || str.contains("族"));
            boolean isNationWithoutLabel = i > 1 && str.length() == 1 && isTextChinese(str);
            if (isNationNotEdited && isNationWithLabel) {
                String nationString = str.replace("民族", "");
                if (nationString.length() == 0) {
                    // 如果去掉label后全没了, 性别在下一个框里
                    continue;
                }

                if (nationString.charAt(0) == '族') {
                    if (nationString.length() == 1) {
                        // 如果只是一个'族'字
                        continue;
                    }
                    nationString = nationString.substring(1);
                    resultMap.put(Constants.NATION, nationString);
                    continue;
                }
            } else if (isNationNotEdited && isNationWithoutLabel) {
                resultMap.put(Constants.NATION, str);
                continue;
            }

            // handle birth
            boolean isBirthYearNotEdited = Objects.equals(resultMap.get(Constants.BIRTH_YEAR), Constants.UNKNOWN);
            boolean isBirthMonthNotEdited = Objects.equals(resultMap.get(Constants.BIRTH_MONTH), Constants.UNKNOWN);
            boolean isBirthDayNotEdited = Objects.equals(resultMap.get(Constants.BIRTH_DAY), Constants.UNKNOWN);

            if (isBirthYearNotEdited && i > 2 && isTextNumeric(str) && str.length() == 4) {
                int yearNumber = Integer.parseInt(str);
                if (yearNumber > Constants.START_YEAR && yearNumber < Constants.END_YEAR) {
                    resultMap.put(Constants.BIRTH_YEAR, str);
                    continue;
                }
            } else if (isBirthMonthNotEdited && i > 3 && isTextNumeric(str) && str.length() < 3) {
                int monthNumber = Integer.parseInt(str);
                if (monthNumber > 0 && monthNumber < 13) {
                    resultMap.put(Constants.BIRTH_MONTH, str);
                    continue;
                }

            } else if (isBirthDayNotEdited && i > 4 && isTextNumeric(str) && str.length() < 3) {
                int dayNumber = Integer.parseInt(str);
                if (dayNumber > 0 && dayNumber < 32) {
                    resultMap.put(Constants.BIRTH_DAY, str);
                    continue;
                }
            }

            boolean isAddressNotEdited = Objects.equals(resultMap.get(Constants.ADDRESS), Constants.UNKNOWN);
            if (isAddressNotEdited && i > 5 && !isTextNumeric(str) && !isTextIdNumber(str)) {
                StringBuilder resAddressSb = new StringBuilder();
                resAddressSb.append(str);
                if (i + 1 < textArray.length) {
                    String str2 = textArray[i + 1];
                    if (!isTextIdNumber(str2)) {
                        resAddressSb.append(str2);
                    }
                }

                resultMap.put(Constants.ADDRESS, resAddressSb.toString());
                continue;
            }

            if (isTextIdNumber(str)) {
                resultMap.put(Constants.ID, str);
            }
        }

        return resultMap;
    }

    public static boolean isTextChinese(String text) {
        String reg = "[\\u4e00-\\u9fa5]+";
        return text.matches(reg);
    }

    public static boolean isTextNumeric(String str) {
        Pattern pattern = Pattern.compile("[0-9]+");
        return pattern.matcher(str).matches();
    }

    public static boolean isTextName(String text) {
        String regex = "^[\u4e00-\u9fa5.·\u36c3\u4DAE]{0,}$";
        return text.matches(regex);
    }

    public static boolean isTextGender(String text) {

        return "男".equals(text) || "女".equals(text)
                || "性别男".equals(text) || "性别女".equals(text)
                || "别男".equals(text) || "别女".equals(text);
    }

    public static boolean isTextIdNumber(String IdNumber) {
        if (IdNumber == null || "".equals(IdNumber)) {
            return false;
        }
        // 定义判别用户身份证号的正则表达式（15位或者18位，最后一位可以为字母）
        String regularExpression = "(^[1-9]\\d{5}(18|19|20)\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$)|" +
                "(^[1-9]\\d{5}\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}$)";
        //假设18位身份证号码:41000119910101123X  410001 19910101 123X
        //^开头
        //[1-9] 第一位1-9中的一个      4
        //\\d{5} 五位数字           10001（前六位省市县地区）
        //(18|19|20)                19（现阶段可能取值范围18xx-20xx年）
        //\\d{2}                    91（年份）
        //((0[1-9])|(10|11|12))     01（月份）
        //(([0-2][1-9])|10|20|30|31)01（日期）
        //\\d{3} 三位数字            123（第十七位奇数代表男，偶数代表女）
        //[0-9Xx] 0123456789Xx其中的一个 X（第十八位为校验值）
        //$结尾

        //假设15位身份证号码:410001910101123  410001 910101 123
        //^开头
        //[1-9] 第一位1-9中的一个      4
        //\\d{5} 五位数字           10001（前六位省市县地区）
        //\\d{2}                    91（年份）
        //((0[1-9])|(10|11|12))     01（月份）
        //(([0-2][1-9])|10|20|30|31)01（日期）
        //\\d{3} 三位数字            123（第十五位奇数代表男，偶数代表女），15位身份证不含X
        //$结尾


        boolean matches = IdNumber.matches(regularExpression);

        //判断第18位校验值
        if (matches) {

            if (IdNumber.length() == 18) {
                try {
                    char[] charArray = IdNumber.toCharArray();
                    //前十七位加权因子
                    int[] idCardWi = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
                    //这是除以11后，可能产生的11位余数对应的验证码
                    String[] idCardY = {"1", "0", "X", "9", "8", "7", "6", "5", "4", "3", "2"};
                    int sum = 0;
                    for (int i = 0; i < idCardWi.length; i++) {
                        int current = Integer.parseInt(String.valueOf(charArray[i]));
                        int count = current * idCardWi[i];
                        sum += count;
                    }
                    char idCardLast = charArray[17];
                    int idCardMod = sum % 11;
                    return idCardY[idCardMod].equalsIgnoreCase(String.valueOf(idCardLast));

                } catch (Exception e) {
                    e.printStackTrace();
                    return false;
                }
            }

        }
        return matches;
    }
}
