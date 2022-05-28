package com.king.activemqtest.io;

import android.util.Log;

import com.king.activemqtest.MqttManager;
import com.king.activemqtest.bean.MqttListenerData;
import com.king.activemqtest.interfaces.OnMqttAndroidConnectListener;

import java.util.ArrayList;


/**
 * 所有长连接的base
 */

public abstract class BaseConnect extends Thread {
    public String FORMAT_CLIENT_ID = "MQTT_%s_Android_%s";
    protected String TAG = "BaseConnect";
    private ArrayList<MqttListenerData> mDataLists = new ArrayList<>();   //界面注册的数据和listener


    protected abstract void startConnect(); //开始连接

    protected abstract boolean isConnected();   //判断是否可连接

    protected abstract void disConnect();   //断开连接

    protected abstract void publish(String topic, String json) throws Exception;  //发送消息

    protected abstract void subscribeTopic();   //订阅主题

    public BaseConnect() {

    }

    @Override
    public void run() {
        super.run();
        startConnect();
    }

    //发送无须在本次回应中回调的消息
    public void sendMsg(String topic, String body) {
        try {
            publish(topic, body);
        } catch (Exception e) {
            Log.e(TAG, "send msg Exception = " + e.toString());
        }
    }


    public void regeisterServerMsg(OnMqttAndroidConnectListener listener) {
        MqttListenerData data = new MqttListenerData();
        data.setListener(listener);
        mDataLists.add(data);
    }

    public void unRegeisterServerMsg(OnMqttAndroidConnectListener listener) {
        for (int i = 0; i < mDataLists.size(); i++) {
            MqttListenerData data = mDataLists.get(i);
            if (data.getListener() == listener) {
                mDataLists.remove(i--);
            }
        }
    }

    /**
     * 对相应注册回调模块进行消息回调
     */
    protected void onDataReceiveCallBack(String topic,final String message) {
        for (int i = 0; i < mDataLists.size(); i++) {
            final MqttListenerData data = mDataLists.get(i);

            MqttManager.mHandler.post(new Runnable() {
                @Override
                public void run() {
                    data.getListener().onDataReceive(topic,message);
                }
            });
        }
    }

    /**
     * 断开连接的回调
     */
    protected void disConnectCallBack() {
        ArrayList<OnMqttAndroidConnectListener> totalLists = getAvailListener();
        for (int i = 0; i < totalLists.size(); i++) {
            final OnMqttAndroidConnectListener onMqttAndroidConnectListener = totalLists.get(i);
            MqttManager.mHandler.post(new Runnable() {
                @Override
                public void run() {
                    onMqttAndroidConnectListener.disConnect();
                }
            });
        }
    }

    /**
     * 连接错误会对所有listener进行回调
     */
    protected void connectFailCallBack(final String exception) {
        ArrayList<OnMqttAndroidConnectListener> totalLists = getAvailListener();
        for (int i = 0; i < totalLists.size(); i++) {
            final OnMqttAndroidConnectListener onWebSocketConnectListener = totalLists.get(i);
            MqttManager.mHandler.post(new Runnable() {
                @Override
                public void run() {
                    onWebSocketConnectListener.onConnectFail(exception);
                }
            });
        }
    }

    /**
     * 连接成功会对所有listener进行回调
     */
    protected void connectSuccessCallBack(boolean reconnect) {
        if (!reconnect) {
            subscribeTopic();
        }
        ArrayList<OnMqttAndroidConnectListener> totalLists = getAvailListener();
        for (int i = 0; i < totalLists.size(); i++) {
            final OnMqttAndroidConnectListener onWebSocketConnectListener = totalLists.get(i);
            MqttManager.mHandler.post(new Runnable() {
                @Override
                public void run() {
                    onWebSocketConnectListener.connect();
                }
            });
        }
    }

    private ArrayList<OnMqttAndroidConnectListener> getAvailListener() {
        ArrayList<OnMqttAndroidConnectListener> list = new ArrayList<>();
        for (int i = 0; i < mDataLists.size(); i++) {   //去掉重复的回调
            MqttListenerData data = mDataLists.get(i);
            if (data.getListener() == null) {
                continue;
            }
            if (list.contains(data.getListener())) {
                continue;
            }
            list.add(data.getListener());
        }
        return list;
    }


}
