package com.king.activemqtest.ui;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.bigkoo.pickerview.builder.TimePickerBuilder;
import com.bigkoo.pickerview.listener.OnTimeSelectListener;
import com.bigkoo.pickerview.view.TimePickerView;
import com.google.android.material.switchmaterial.SwitchMaterial;
import com.king.activemqtest.Api;
import com.king.activemqtest.MqttManager;
import com.king.activemqtest.R;
import com.king.activemqtest.bean.EventBusMsg;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * 首页
 */
public class MainFragment extends Fragment {


    private Context context;
    private TextView startText;
    private TextView endText;
    private static final int TAG=123;
    private boolean isDestroy=false;


    private long startTime;
    private long endTime;

    private boolean start;
    private boolean end;

    private TextView alarmTextView;
    private TextView doorTextView;
    private Button timeOnButton;
    private Button timeOffButton;
    private SwitchMaterial controlSwitch;



    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == TAG) {
                timeAfterTime();
                removeMessages(TAG);
                if (!isDestroy) {
                    sendEmptyMessageDelayed(TAG, 1000*15);//这里15s刷新一次
                }
            }
        }
    };

    private void timeAfterTime() {
        if(System.currentTimeMillis()>=endTime){
            //发送一次重连命令
            if(end){
                end=false;
                String msg="{\"TimerStatus\":\""+0+"\"}";
                MqttManager.getInstance().sendMsg(Api.IN_TOPIC,msg);
            }
        }else if(System.currentTimeMillis()>=startTime){
            //发送一次断开命令
            if(start){
                start=false;
                String msg="{\"TimerStatus\":\""+1+"\"}";
                MqttManager.getInstance().sendMsg(Api.IN_TOPIC,msg);
            }
        }
    }



    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view=inflater.inflate(R.layout.fragment_main, container, false);
        initView(view);
        return view;
    }

    private void initView(View view) {
        context=getActivity();
        String load="{\"Command\":\"Rfidload\"}";
        MqttManager.getInstance().sendMsg(Api.APP_REQUEST,load);
        startText=view.findViewById(R.id.startText);
        endText=view.findViewById(R.id.endText);
        Button openButton = view.findViewById(R.id.openButton);
        Button cleanButton=view.findViewById(R.id.cleanButton);
        alarmTextView=view.findViewById(R.id.alarmTextView);
        alarmTextView.setVisibility(View.GONE);
        doorTextView=view.findViewById(R.id.doorTextView);
        controlSwitch=view.findViewById(R.id.controlSwitch);
        timeOnButton=view.findViewById(R.id.timeOnButton);
        timeOffButton=view.findViewById(R.id.timeOffButton);
        startText.setOnClickListener(view1 -> {
            if(!controlSwitch.isChecked()){
                return;
            }
            showTimePickerDialog(startText);

        });
        endText.setOnClickListener(view12 -> {
            if(!controlSwitch.isChecked()){
                return;
            }
            showTimePickerDialog(endText);
        });
        openButton.setOnClickListener(view13 -> {
            String s="{\"OpenDoor\":\""+1+"\"}";
            MqttManager.getInstance().sendMsg(Api.IN_TOPIC,s);
        });
        cleanButton.setOnClickListener(view1 -> {
            start=false;
            end=false;
            startText.setText("off");
            endText.setText("off");
            controlSwitch.setChecked(false);
            String msg="{\"TimerStatus\":\""+0+"\"}";
            MqttManager.getInstance().sendMsg(Api.IN_TOPIC,msg);
        });

        timeOnButton.setOnClickListener(view1 -> {
            controlSwitch.setChecked(true);
        });
        timeOffButton.setOnClickListener(view1 -> {
            controlSwitch.setChecked(false);
            //发送一次定时器不启用命令
            String msg="{\"TimerStatus\":\""+0+"\"}";
            MqttManager.getInstance().sendMsg(Api.IN_TOPIC,msg);
        });
    }




    /**
     * 时间选择
     */
    private void showTimePickerDialog(TextView tv) {

        TimePickerView pvTime = new TimePickerBuilder(context, new OnTimeSelectListener() {
            @Override
            public void onTimeSelect(Date date,View v) {//选中事件回调
                if(tv.getId()==startText.getId()) {
                    startTime = date.getTime();
                    start=true;
                }else {
                    endTime=date.getTime();
                    end=true;
                }
                tv.setText(new SimpleDateFormat("HH:mm").format(date));
                if(startTime!=0&&endTime!=0){
                    if(startTime>endTime){
                        endTime+=1000*60*60*24;
                    }
                    mHandler.sendEmptyMessage(TAG);
                }
            }
        })
                .setType(new boolean[]{false, false, false, true, true, false})// 默认全部显示
                .setCancelText("Cancel")//取消按钮文字
                .setSubmitText("Sure")//确认按钮文字
                .setTitleSize(20)//标题文字大小
                .setTitleText("Time Select")//标题文字
                .setOutSideCancelable(true)//点击屏幕，点在控件外部范围时，是否取消显示
                .isCyclic(true)//是否循环滚动
                .setItemVisibleCount(5)
                .setTitleColor(Color.BLACK)//标题文字颜色
                .setSubmitColor(Color.BLUE)//确定按钮文字颜色
                .setCancelColor(Color.BLUE)//取消按钮文字颜色
                .setLabel("年","月","日","H","m","秒")//默认设置为年月日时分秒
                .isCenterLabel(false) //是否只显示中间选中项的label文字，false则每项item全部都带有label。
                .isDialog(false)//是否显示为对话框样式
                .build();
        pvTime.show();
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
        if(TextUtils.equals(event.getCommand(),Api.MAIN)) {
            if (event.getAlarm() == 1) {
                alarmTextView.setVisibility(View.VISIBLE);
            } else if (event.getAlarm()==2) {
                alarmTextView.setVisibility(View.GONE);
                doorTextView.setText("close");
            }else if(event.getAlarm()==3){
                alarmTextView.setVisibility(View.GONE);
                doorTextView.setText("open");
            }
            String s="{\"Command\":\"Notiload\"}";
            MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
        }

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        isDestroy=true;
        mHandler.removeCallbacksAndMessages(null);
        EventBus.getDefault().unregister(this);
    }
}