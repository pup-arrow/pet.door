package com.king.activemqtest.util;


import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;

public class JsUtil {
    private static JsUtil Instance = null;

    //单例模式
    public static JsUtil getInstance() {
        if (Instance == null) {
            Instance = new JsUtil();
        }
        return Instance;
    }

    //todo 将json对象转换成Map
    //jsonstr 是传过来需要解析的字符串   hashmap是接收的HashMap
    // isShowLog(这个就是方便查看解析到哪里失败了就可以)是控制是否打印的boolean   tue为打印   false为不打印
    //目前能解析int和double还有String 如果需要自己可以在后面添加
    public static HashMap jsonToMap(String jsonstr, boolean isShowLog) throws JSONException {
        //todo 拿到json后丢进来转换成为object对象
        JSONObject jsonObject = new JSONObject(jsonstr);
        //todo 接受返回值对象，封装最后返回的Map对象
        HashMap<String, Object> result = new HashMap<>();
        //todo 获取所有的KEY
        Iterator<String> iterator = jsonObject.keys();
        String key = null;
        //todo 因为value可能是任何类型，所以用所有类型的超类object接收
        Object value = null;
        //todo 此时iterator为所有的key，hasNext()如果有值的话
        while (iterator.hasNext()) {
            //todo 拿到一个值
            key = (String) iterator.next();
            //todo 拿到key所对应的value是一个object类型
            value = jsonObject.get(key);
            if (value instanceof String) {
                if (isShowLog) {
                    Log.e(JsUtil.class.getSimpleName(), key + "——" + jsonObject.optString(key, ""));
                }
                result.put(key, jsonObject.optString(key, ""));
            } else if (value instanceof Integer) {
                if (isShowLog) {
                    Log.e(JsUtil.class.getSimpleName(), key + "——" + jsonObject.optInt(key, 0));
                }
                result.put(key, "" + jsonObject.optInt(key, 0));
            } else if (value instanceof Double) {
                if (isShowLog) {
                    Log.e(JsUtil.class.getSimpleName(), key + "——" + jsonObject.optDouble(key, 0));
                }
                result.put(key, "" + jsonObject.optDouble(key, 0));
            } else if (value instanceof JSONObject) {
                if (isShowLog) {
                    Log.e(JsUtil.class.getSimpleName(), key + "——" + jsonObject.get(key));
                }
                result.put(key, value);
            }else {
                if (isShowLog) {
                    Log.e(JsUtil.class.getSimpleName(), key + "——" + jsonObject.get(key));
                }
                result.put(key, (String)value);
            }
        }
        return result;
    }

}

