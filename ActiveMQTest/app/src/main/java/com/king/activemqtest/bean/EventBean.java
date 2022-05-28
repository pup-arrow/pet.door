package com.king.activemqtest.bean;

/**
 * 操作通知Bean
 */
public class EventBean {

    private String SystemStart;
    private String Alarm;
    private String RFID;
    private String DoorS;
    private String DoorStatus;

    public String getSystemStart() {
        return SystemStart;
    }

    public void setSystemStart(String systemStart) {
        SystemStart = systemStart;
    }

    public String getAlarm() {
        return Alarm;
    }

    public void setAlarm(String alarm) {
        Alarm = alarm;
    }

    public String getRFID() {
        return RFID;
    }

    public void setRFID(String RFID) {
        this.RFID = RFID;
    }

    public String getDoorS() {
        return DoorS;
    }

    public void setDoorS(String doorS) {
        DoorS = doorS;
    }

    public String getDoorStatus() {
        return DoorStatus;
    }

    public void setDoorStatus(String doorStatus) {
        DoorStatus = doorStatus;
    }
}
