package com.king.activemqtest.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.king.activemqtest.Api;
import com.king.activemqtest.MqttManager;
import com.king.activemqtest.R;
import com.king.activemqtest.bean.EventBusMsg;
import com.king.activemqtest.util.SpUtils;
import com.tencent.mmkv.MMKV;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

/**
 * 注册
 */
public class RegisterActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);//关联activity_register.xml
        EventBus.getDefault().register(this);
        // 关联用户名、密码、确认密码、邮箱和注册、返回登录按钮
        EditText userName = findViewById(R.id.UserNameEdit);
        EditText passWord = findViewById(R.id.PassWordEdit);
        EditText passWordAgain = findViewById(R.id.PassWordAgainEdit);
        Button signUpButton = findViewById(R.id.SignUpButton);
        Button backLoginButton = findViewById(R.id.BackLoginButton);

        // 注册按钮监听器
        signUpButton.setOnClickListener(v -> {
            String strUserName = userName.getText().toString().trim();
            String strPassWord = passWord.getText().toString().trim();
            String strPassWordAgain = passWordAgain.getText().toString().trim();
            //注册格式粗检
            if(TextUtils.isEmpty(strUserName)||TextUtils.isEmpty(strPassWord)||TextUtils.isEmpty(strPassWordAgain)){
                Toast.makeText(RegisterActivity.this, "注册信息不能为空", Toast.LENGTH_SHORT).show();
            }else if (!strPassWord.equals(strPassWordAgain)) {
                Toast.makeText(RegisterActivity.this, "两次密码输入不一致！", Toast.LENGTH_SHORT).show();
            } else {
                String s="{\"Command\":\"Register\",\"Userid\":\""+strUserName+"\",\"Password\":\""+strPassWord+"\"}";
                MqttManager.getInstance().sendMsg(Api.APP_REQUEST,s);
//                SpUtils.encode(strUserName,strPassWord);
//                Toast.makeText(RegisterActivity.this, "注册成功！", Toast.LENGTH_SHORT).show();
//                Intent intent = new Intent(RegisterActivity.this, LoginActivity.class);
//                startActivity(intent);
            }
        });
        // 注册按钮监听器
        backLoginButton.setOnClickListener(v -> {
            Intent intent = new Intent(RegisterActivity.this, LoginActivity.class);
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
        if(TextUtils.equals(event.getCommand(),Api.REGISTER)){
                Toast.makeText(RegisterActivity.this, "注册成功！", Toast.LENGTH_SHORT).show();
                Intent intent = new Intent(RegisterActivity.this, LoginActivity.class);
                startActivity(intent);
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }
}
