package com.king.activemqtest.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.king.activemqtest.Api;
import com.king.activemqtest.MqttManager;
import com.king.activemqtest.R;
import com.king.activemqtest.bean.EventBusMsg;
import com.king.activemqtest.util.SpUtils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

/**
 * 登录
 */
public class LoginActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);//关联activity.xml
        EventBus.getDefault().register(this);
        // 关联用户名、密码和登录、注册按钮
        EditText userName = findViewById(R.id.UserNameEdit);
        EditText passWord = findViewById(R.id.PassWordEdit);
        Button loginButton = findViewById(R.id.LoginButton);
        Button signUpButton = findViewById(R.id.SignUpButton);
        startService(new Intent(this,MyService.class));
        // 登录按钮监听器
        loginButton.setOnClickListener(v -> {
            String strUserName = userName.getText().toString().trim();
            String strPassWord = passWord.getText().toString().trim();
            String s="{\"Command\":\"Sign\",\"Userid\":\""+strUserName+"\",\"Password\":\""+strPassWord+"\"}";
            MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
//            if (SpUtils.containsKey(strUserName)&& TextUtils.equals(SpUtils.decodeString(strUserName),strPassWord)) {
//                Intent intent=new Intent(LoginActivity.this,MainActivity.class);
//                startActivity(intent);
//            } else {
//                Toast.makeText(LoginActivity.this, "请输入正确的用户名或密码！", Toast.LENGTH_SHORT).show();
//            }
        });
        // 注册按钮监听器
        signUpButton.setOnClickListener(v -> {
            Intent intent = new Intent(LoginActivity.this, RegisterActivity.class);
            startActivity(intent);
        });


    }

    /**
     * tag @Subscribe 注解必须要写，线程需要指定
     * 方法名可随意
     * @param event 接收数据
     */
    @Subscribe(threadMode = ThreadMode.MAIN,sticky = true)
    public void onMessageEvent(EventBusMsg event) {
        if(TextUtils.equals(event.getCommand(),Api.SIGN)){
                Intent intent=new Intent(LoginActivity.this,MainActivity.class);
                startActivity(intent);
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }
}
