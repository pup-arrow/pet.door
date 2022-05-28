package com.king.activemqtest.ui;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.google.gson.Gson;
import com.king.activemqtest.Api;
import com.king.activemqtest.MqttManager;
import com.king.activemqtest.R;
import com.king.activemqtest.adapter.EventAdapter;
import com.king.activemqtest.bean.EventBean;
import com.king.activemqtest.bean.EventBusMsg;
import com.king.activemqtest.bean.PersonBean;
import com.king.activemqtest.bean.StatisticsBean;
import com.king.activemqtest.util.InfoUtils;
import com.king.activemqtest.util.SpUtils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.List;

/**
 * 统计页
 */
public class StatisticsFragment extends Fragment {

    private Context context;
    private List<StatisticsBean> eventList;
    private List<StatisticsBean> list;
    private List<PersonBean> personBeanList;
    private RecyclerView eventRecycler;




    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view=inflater.inflate(R.layout.fragment_statistics, container, false);
        initView(view);
        return view;
    }



    private void initView(View view) {
        context=getActivity();
        eventList =new ArrayList<>();
        list=new ArrayList<>();
        personBeanList=new ArrayList<>();
//        if(!SpUtils.containsKey(Api.EVENT_KEY)){
//            eventList =new ArrayList<>();
//        }else {
//            eventList = InfoUtils.Str2List(SpUtils.decodeString(Api.EVENT_KEY),StatisticsBean.class);
//        }
        String s="{\"Command\":\"Notiload\"}";
        MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
        eventRecycler =view.findViewById(R.id.eventRecyclerView);
        LinearLayoutManager rfidLayoutManager=new LinearLayoutManager(context,RecyclerView.VERTICAL,true);
        eventRecycler.setLayoutManager(rfidLayoutManager);
        EventAdapter adapter = new EventAdapter(eventList, context);
        eventRecycler.setAdapter(adapter);


    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EventBus.getDefault().register(this);
    }

    /**
     * tag @Subscribe 注解必须要写，线程需要指定
     * 方法名可随意
     * @param event 接收数据
     */
    @Subscribe(threadMode = ThreadMode.MAIN,sticky = true)
    public void onMessageEvent(EventBusMsg event) {
        if(TextUtils.equals(event.getCommand(),Api.NOTIFICATION_LOAD)){
            Gson gson=new Gson();
            eventList=InfoUtils.Str2List(SpUtils.decodeString(Api.EVENT_KEY), StatisticsBean.class);
            personBeanList=InfoUtils.Str2List(SpUtils.decodeString(Api.PERSON_KEY),PersonBean.class);
            for (int i = 0; i < eventList.size(); i++) {
                StatisticsBean statisticsBean=new StatisticsBean();
                statisticsBean.setTime(eventList.get(i).getTime());
                String s=eventList.get(i).getEvent();
                EventBean bean=gson.fromJson(s,EventBean.class);
                if(!TextUtils.isEmpty(bean.getAlarm())){
                    statisticsBean.setEvent("Alarm");
                }else if(!TextUtils.isEmpty(bean.getSystemStart())) {
                    statisticsBean.setEvent("System start");
                }else if(!TextUtils.isEmpty(bean.getDoorStatus())){
                    if(TextUtils.equals(bean.getDoorStatus(),"0")){
                        statisticsBean.setEvent("Door close");
                    }else {
                        statisticsBean.setEvent("Door open");
                    }

                }else {
                    statisticsBean.setEvent("Door open by name");
                    for (int j = 0; j < personBeanList.size(); j++) {
                        if(TextUtils.equals(bean.getRFID(),personBeanList.get(j).getRfid())){
                            statisticsBean.setEvent("Door open by "+personBeanList.get(j).getName());
                        }
                    }

                }
                list.add(statisticsBean);
            }
            EventAdapter adapter = new EventAdapter(list, context);
            eventRecycler.setAdapter(adapter);
            eventRecycler.scrollToPosition(adapter.getItemCount()-1);
        }

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }
}