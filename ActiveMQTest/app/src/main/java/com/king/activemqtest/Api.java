package com.king.activemqtest;


/**
 * mqtt配置参数
 */

public class Api {
    public static String IP_MQTT = "124.71.19.61"; //Mqtt的Ip
    public static int PORT_MQTT = 1883;
    public static String IN_TOPIC = "inTopic";
    public static String APP_REQUEST="apprequest";
    public static String[] OUT_TOPIC=new String[]{"outTopic","appresponse"};

    public static String PERSON_KEY="personList";
    public static String EVENT_KEY ="eventList";

    public static String REGISTER="Register";
    public static String SIGN="Sign";
    public static String RFID_ADD="Rfidadd";
    public static String RFID_LOAD="Rfidload";
    public static String RFID_DELETE="Rfiddele";
    public static String NOTIFICATION_LOAD="Notiload";
    public static String MAIN="main";


}
