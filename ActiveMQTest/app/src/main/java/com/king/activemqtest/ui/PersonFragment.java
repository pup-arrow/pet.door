package com.king.activemqtest.ui;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.king.activemqtest.Api;
import com.king.activemqtest.CustomDialog;
import com.king.activemqtest.MqttManager;
import com.king.activemqtest.R;
import com.king.activemqtest.adapter.PersonAdapter;
import com.king.activemqtest.bean.EventBusMsg;
import com.king.activemqtest.bean.PersonBean;
import com.king.activemqtest.util.InfoUtils;
import com.king.activemqtest.util.SpUtils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.List;

/**
 * 人员页
 */
public class PersonFragment extends Fragment {
    private Context context;
    private List<PersonBean> list;
    private PersonAdapter adapter;

    private CustomDialog mDialog;
    private RecyclerView personRecyclerView;



    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view =inflater.inflate(R.layout.fragment_person, container, false);
        context=getActivity();
        String s="{\"Command\":\"Rfidload\"}";
        MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
        list=new ArrayList<>();
//        if(SpUtils.containsKey(Api.PERSON_KEY)){
//            list=InfoUtils.Str2List(SpUtils.decodeString(Api.PERSON_KEY),PersonBean.class);
//        }else {
//            list = new ArrayList<>();
//        }
        personRecyclerView=view.findViewById(R.id.personRecyclerView);
        personRecyclerView.setLayoutManager(new LinearLayoutManager(context));
        adapter=new PersonAdapter(list,context);
        personRecyclerView.setAdapter(adapter);

        Button addButton = view.findViewById(R.id.addButton);
        addButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showAddDialog();
            }
        });
        return view;
    }

    protected void showAddDialog() {

        LayoutInflater factory = LayoutInflater.from(context);
        View textEntryView = factory.inflate(R.layout.dialog_add, null);
        Button sureButton=textEntryView.findViewById(R.id.sureButton);
        Button cancelButton=textEntryView.findViewById(R.id.cancelButton);
        EditText nameEditText=textEntryView.findViewById(R.id.nameEditText);
        EditText idEditText=textEntryView.findViewById(R.id.idEditText);
        AlertDialog.Builder ad = new AlertDialog.Builder(context);

        ad.setTitle("add pet:");
        ad.setIcon(android.R.drawable.ic_dialog_info);
        ad.setView(textEntryView);
        AlertDialog dialog=ad.show();// 显示对话框
        dialog.setCanceledOnTouchOutside(true);
        dialog.setCancelable(true);

        sureButton.setOnClickListener(view -> {
            String name=nameEditText.getText().toString();
            String id=idEditText.getText().toString();
            if(id.length()!=24){
                Toast.makeText(context,"请输入24位id"+id.length(),Toast.LENGTH_SHORT).show();
            }else {
                dialog.dismiss();
                PersonBean personBean=new PersonBean();
                personBean.setRfid(id);
                personBean.setName(name);
                list.add(personBean);
                adapter.notifyDataSetChanged();
                String msg="{\"RFIDADD\":\""+id+"\"}";
                String add="{\"Command\":\"Rfidadd\",\"Rfid\":\""+id+"\",\"Name\":\""+name+"\"}";
                String s=InfoUtils.List2Str(list);
                SpUtils.encode(Api.PERSON_KEY,s);
                MqttManager.getInstance().sendMsg(Api.IN_TOPIC,msg);
                MqttManager.getInstance().sendMsg(Api.APP_REQUEST,add);
            }

        });
        cancelButton.setOnClickListener(view -> {
            dialog.dismiss();
        });
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
        if(TextUtils.equals(event.getCommand(),Api.RFID_ADD)){
            //添加
            String s="{\"Command\":\"Rfidload\"}";
            MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
        }else if(TextUtils.equals(event.getCommand(),Api.RFID_DELETE)){
            //删除
            String s="{\"Command\":\"Rfidload\"}";
            MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
        }if(TextUtils.equals(event.getCommand(),Api.RFID_LOAD)){
            if(SpUtils.containsKey(Api.PERSON_KEY)){
                list=InfoUtils.Str2List(SpUtils.decodeString(Api.PERSON_KEY),PersonBean.class);
                adapter=new PersonAdapter(list,context);
                personRecyclerView.setAdapter(adapter);
                adapter.setOnItemLongClick((view1, position) -> {
                    mDialog=new CustomDialog(context, "Tips", "Are you sure you want to delete?", "Yes",new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            mDialog.dismiss();
                            String msg="{\"RFIDDELE\":\""+list.get(position).getRfid()+"\"}";
                            String delete="{\"Command\":\"Rfiddele\",\"Rfid\":\""+list.get(position).getRfid()+"\",\"Name\":\""+list.get(position).getName()+"\"}";
                            list.remove(position);
                            adapter.notifyDataSetChanged();
                            String s=InfoUtils.List2Str(list);
                            SpUtils.encode(Api.PERSON_KEY,s);
                            MqttManager.getInstance().sendMsg(Api.IN_TOPIC,msg);
                            MqttManager.getInstance().sendMsg(Api.APP_REQUEST,delete);
                        }
                    },"No");
                    mDialog.show();

                });
            }
        }

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }
}