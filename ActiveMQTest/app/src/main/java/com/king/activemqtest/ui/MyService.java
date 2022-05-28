package com.king.activemqtest.ui;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.reflect.TypeToken;
import com.king.activemqtest.Api;
import com.king.activemqtest.MqttManager;
import com.king.activemqtest.bean.EventBean;
import com.king.activemqtest.bean.EventBusMsg;
import com.king.activemqtest.bean.PersonBean;
import com.king.activemqtest.bean.StatisticsBean;
import com.king.activemqtest.interfaces.OnMqttAndroidConnectListener;
import com.king.activemqtest.util.InfoUtils;
import com.king.activemqtest.util.JsUtil;
import com.king.activemqtest.util.SpUtils;

import org.greenrobot.eventbus.EventBus;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * 接收MQtt消息服务
 */
public class MyService extends Service {

    private List<StatisticsBean> eventList;

    @Override
    public void onCreate() {
        super.onCreate();
        MqttManager.getInstance().regeisterServerMsg(new OnMqttAndroidConnectListener() {
            @Override
            public void onDataReceive(String topic,String message) {
                Log.e("onDataReceive",message);
                if(TextUtils.equals(message,"SystemStart")){
                    return;
                }
                if(TextUtils.equals(topic,Api.OUT_TOPIC[0])){
                    Gson gson=new Gson();
                    EventBean bean=gson.fromJson(message,EventBean.class);
                    if(!TextUtils.isEmpty(bean.getAlarm())){
                        EventBus.getDefault().postSticky(new EventBusMsg(1,Api.MAIN));
                    }else if(!TextUtils.isEmpty(bean.getDoorStatus())){
                        if(TextUtils.equals(bean.getDoorStatus(),"0")) {
                            EventBus.getDefault().postSticky(new EventBusMsg(2, Api.MAIN));
                        }else {
                            EventBus.getDefault().postSticky(new EventBusMsg(3,Api.MAIN));
                        }
                    }else if(TextUtils.isEmpty(bean.getSystemStart())){
                        EventBus.getDefault().postSticky(new EventBusMsg(3,Api.MAIN));
                    }
                }else if(TextUtils.equals(topic,Api.OUT_TOPIC[1])) {
                    try {
                       Map<String,Object> map= JsUtil.jsonToMap(message, true);
                       String level=(String) map.get("Level");
                       String command=(String) map.get("RetCommand");
                    if (TextUtils.equals(command, Api.REGISTER)) {
                        if (TextUtils.equals(level, "1")) {
                            EventBus.getDefault().postSticky(new EventBusMsg(Api.REGISTER));
                        }else {
                            Toast.makeText(MyService.this,"注册失败",Toast.LENGTH_SHORT).show();
                        }
                    }else if(TextUtils.equals(command, Api.SIGN)){
                        if (TextUtils.equals(level, "1")) {
                            EventBus.getDefault().postSticky(new EventBusMsg(Api.SIGN));
                        }else {
                            Toast.makeText(MyService.this,"登录失败",Toast.LENGTH_SHORT).show();
                        }
                    }else if(TextUtils.equals(command, Api.RFID_ADD)){
                        if (TextUtils.equals(level, "1")) {
                            EventBus.getDefault().postSticky(new EventBusMsg(Api.RFID_ADD));
                        }else {
                            Toast.makeText(MyService.this,"添加失败",Toast.LENGTH_SHORT).show();
                        }
                    }else if(TextUtils.equals(command, Api.RFID_DELETE)){
                        if (TextUtils.equals(level, "1")) {
                            EventBus.getDefault().postSticky(new EventBusMsg(Api.RFID_DELETE));
                        }else {
                            Toast.makeText(MyService.this,"删除失败",Toast.LENGTH_SHORT).show();
                        }
                    }else if(TextUtils.equals(command, Api.RFID_LOAD)){
                        if (TextUtils.equals(level, "1")) {
                            List<PersonBean> list=jsonObject2List((JSONObject) map.get("Result"));
                            SpUtils.encode(Api.PERSON_KEY, InfoUtils.List2Str(list));
                            EventBus.getDefault().postSticky(new EventBusMsg(Api.RFID_LOAD));
                        }else {
                            Toast.makeText(MyService.this,"加载失败",Toast.LENGTH_SHORT).show();
                        }
                    }else if(TextUtils.equals(command, Api.NOTIFICATION_LOAD)){
                        if (TextUtils.equals(level, "1")) {
                            List<StatisticsBean> list=jsonObject2StatisticsList((JSONObject) map.get("Result"));
                            SpUtils.encode(Api.EVENT_KEY, InfoUtils.List2Str(list));
                            EventBus.getDefault().postSticky(new EventBusMsg(Api.NOTIFICATION_LOAD));
                        }else {
                            Toast.makeText(MyService.this,"加载失败",Toast.LENGTH_SHORT).show();
                        }
                    }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
//                if(!SpUtils.containsKey(Api.EVENT_KEY)){
//                    eventList =new ArrayList<>();
//                }else {
//                    eventList =InfoUtils.Str2List(SpUtils.decodeString(Api.EVENT_KEY),StatisticsBean.class);
//                }


//                OutTopicBean bean=gson.fromJson(message, OutTopicBean.class);
//                StatisticsBean statisticsBean=new StatisticsBean();
//                if(!TextUtils.isEmpty(bean.getAlarm())){
//                    //警告
//                    statisticsBean.setEvent("door status warning");
//                }else if(!TextUtils.isEmpty(bean.getDoorStatus())){
//                    //状态改变
//                    statisticsBean.setEvent("Alert motion sensed");
//                }else {
//                    List<PersonBean> personBeanList;
//                    if(SpUtils.containsKey(Api.PERSON_KEY)){
//                        personBeanList= InfoUtils.Str2List(SpUtils.decodeString(Api.PERSON_KEY),PersonBean.class);
//                    }else {
//                        personBeanList = new ArrayList<>();
//                    }
//                    for (PersonBean person:personBeanList) {
//                        if(TextUtils.equals(person.getId(),bean.getRFID())){
//                            statisticsBean.setEvent("Door open by"+person.getName());
//                        }else {
//                            statisticsBean.setEvent("Door open by");
//                        }
//                    }
//                }
////                statisticsBean.setTime(calendar.get(Calendar.HOUR_OF_DAY)+":"+ calendar.get(Calendar.MINUTE));
//                eventList.add(statisticsBean);
//                String s= InfoUtils.List2Str(eventList);
//                SpUtils.encode(Api.EVENT_KEY,s);
//                if(!TextUtils.isEmpty(bean.getAlarm())){
//                    EventBus.getDefault().postSticky(new EventBusMsg(1));
//                }else if(!TextUtils.isEmpty(bean.getDoorStatus())){
//                    EventBus.getDefault().postSticky(new EventBusMsg(bean.getDoorStatus()));
//                }else {
//                    EventBus.getDefault().postSticky(new EventBusMsg(bean.getDoorS()));
//                }

            }
        });
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    private List<PersonBean> jsonObject2List(JSONObject jso) {
        List<PersonBean> personBeanList = new ArrayList<>();
        try {
            for (Iterator<String> keyStr = jso.keys(); keyStr.hasNext(); ) {
                PersonBean personBean=new PersonBean();
                String key = keyStr.next().trim();
                personBean.setRfid(key);
                personBean.setName(jso.getString(key));
                personBeanList.add(personBean);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return personBeanList;
    }
    private List<StatisticsBean> jsonObject2StatisticsList(JSONObject jso) {
        List<StatisticsBean> statisticsBeanList = new ArrayList<>();
        try {
            StatisticsBean statisticsBean=new StatisticsBean();
            for (Iterator<String> keyStr = jso.keys(); keyStr.hasNext(); ) {

                String key = keyStr.next().trim();
                if(TextUtils.isEmpty(statisticsBean.getTime())){
                    statisticsBean.setTime(jso.getString(key));
                }else {
                    statisticsBean.setEvent(jso.getString(key));
                }
                if(!TextUtils.isEmpty(statisticsBean.getTime())&&!TextUtils.isEmpty(statisticsBean.getEvent())){
                    statisticsBeanList.add(statisticsBean);
                    statisticsBean=new StatisticsBean();
                }

            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return statisticsBeanList;
    }

}
