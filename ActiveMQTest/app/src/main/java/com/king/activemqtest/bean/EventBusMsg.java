package com.king.activemqtest.bean;

/**
 * <p>类描述：一句话描述下这个类的作用
 * <p>创建人：wh
 * <p>创建时间：2022/5/21
 */
public class EventBusMsg {

    private int alarm;
    private String command;

    public EventBusMsg(int alarm) {
        this.alarm = alarm;
    }

    public EventBusMsg(String command) {
        this.command = command;
    }

    public EventBusMsg(int alarm, String command) {
        this.alarm = alarm;
        this.command = command;
    }

    public String getCommand() {
        return command;
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public int getAlarm() {
        return alarm;
    }

    public void setAlarm(int alarm) {
        this.alarm = alarm;
    }
}
