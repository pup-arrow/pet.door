package com.king.activemqtest;

import android.app.Application;

import com.king.activemqtest.util.SpUtils;
import com.tencent.mmkv.MMKV;

/**
 * app相关配置
 */
public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        //数据存储初始化
        MMKV.initialize(this);
        SpUtils.getInstance();
        //初始化自定义WiseMqtt模块的配置,并开启长连接
        MqttManager.getInstance()
                .init(this)
                .setServerIp(Api.IP_MQTT)                            //Ip
                .setServerPort(Api.PORT_MQTT)                        //port
                .connect();
    }
}
