package com.king.activemqtest.ui;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.widget.RadioGroup;

import com.king.activemqtest.R;


/**
 * 主页
 */
public class MainActivity extends AppCompatActivity {


    private FragmentManager fragmentManager;
    private int iShowFragment=0;//定义一个显示的Fragment的下标
    private Fragment[] fragments;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();

    }

    @SuppressLint("NonConstantResourceId")
    private void initView() {


        RadioGroup mainRadioGroup=findViewById(R.id.mainRadioGroup);
        fragmentManager=getSupportFragmentManager();
        MainFragment mainFragment = new MainFragment();
        StatisticsFragment statisticsFragment = new StatisticsFragment();
        PersonFragment personFragment = new PersonFragment();
        fragments=new Fragment[]{mainFragment, statisticsFragment, personFragment};
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(R.id.mainFragment, mainFragment).show(mainFragment).commit();
        mainRadioGroup.setOnCheckedChangeListener((radioGroup, i) -> {
            switch (i){
                case R.id.mainRadioButton:
                    if (iShowFragment != 0) {
                        switchFragment(iShowFragment, 0);
                        iShowFragment = 0;
                    }
                    break;
                case R.id.statisticsRadioButton:
                    if (iShowFragment != 1) {
                        switchFragment(iShowFragment, 1);
                        iShowFragment = 1;
                    }
                    break;
                case R.id.personRadioButton:
                    if (iShowFragment != 2) {
                        switchFragment(iShowFragment, 2);
                        iShowFragment = 2;
                    }
                    break;
                default:
                    break;
            }
        });
    }

    /**
     * 切换fragment
     * @param lastIndex 上一个显示的下标
     * @param index 本次要显示的下标
     */
    private void switchFragment(int lastIndex, int index) {
        FragmentTransaction fragmentTransaction=fragmentManager.beginTransaction();
        fragmentTransaction.hide(fragments[lastIndex]);
        if (!fragments[index].isAdded()) {
            fragmentTransaction.add(R.id.mainFragment, fragments[index]);
        }
        fragmentTransaction.show(fragments[index]).commit();
    }
}