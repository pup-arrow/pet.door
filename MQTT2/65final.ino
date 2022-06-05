
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//#define RFPin 26
//#define NormolClose 13
#define Infrare1Pin 4
#define Infrare2Pin 15
#define RFCHPin 27
#define Indoor 26
//#define DoorPin 0
//#define AlarmPin 2
#define REDLED 5
#define WHITELED 14
#define GREENLED 12

#define IN4 0
#define IN3 2
//#include <SPI.h>
//#include <MFRC522.h>
//
//#define RST_PIN   22  
//#define SS_PIN    21  
//#define MISO_PIN  19 
//#define MOSI_PIN  23 
//#define SCK_PIN   18 
//
//MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
//#define DoorStatusPin 10
// Update these with values suitable for your network.
//char a[10]="123456789";\0
//mosquitto linux
const char* ssid = "Pineapple";
const char* password = "Beijing4415";
const char* mqtt_server = "124.71.19.61";


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (100)
char msg[MSG_BUFFER_SIZE];
int value = 0;
//Date Define

String DeviceName[10]={"RFID","DoorStatus","Infrare1","Infrare2","Alarm"};
String DeviceStatus[10]={"0","0","0","0","0"};
String RFIDList[100];
String ReCntrl[10]={"OpenDoor","TimerStatus","RFIDADD","RFIDDELE"};
int ReOpenDoor=0;
int ReTimerStatus=1;
int RFIDCount=0;
const char *RECERFID = NULL;  
bool door_open_flag; //开门动作标志位
HardwareSerial mySerial(1);
unsigned char incomingByte;
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void parmPass(JsonVariant parm)
{
//        int IndoorLight=doc["params"]["IndoorLight"];
//        Serial.println(IndoorLight);
//    const char *myC = parm["myChar"];
//    int myN0 = parm["myArray"][0];
//    int myN1 = parm["myArray"][1];
    if (parm.containsKey(ReCntrl[0]))
    {
        ReOpenDoor=parm[ReCntrl[0]];
        if(ReOpenDoor)
        {
//          digitalWrite(DoorPin,HIGH);
//          delay(1500);
            digitalWrite(IN4,LOW);
            digitalWrite(IN3,HIGH);
            delay(10000);
            digitalWrite(IN4,HIGH);
            digitalWrite(IN3,LOW);
            delay(10000);
            digitalWrite(IN4,LOW);
            digitalWrite(IN3,LOW);
          }
      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }
    else  if (parm.containsKey(ReCntrl[1]))
    {
//        int a=parm[ReCntrl[1]];
//          Serial.println(a);
      ReTimerStatus=parm[ReCntrl[1]];
      Serial.println("ReTimerStatus");
      Serial.println(ReTimerStatus);
//      RFIDList[RFIDCount++]=
      }
    else  if (parm.containsKey(ReCntrl[2]))
    {
       RECERFID=parm[ReCntrl[2]];
       RFIDList[RFIDCount++]=RECERFID;
       
//       Serial.println(RECERFID);
//        Serial.println(RFIDList[1]);
//      ReValue[2]=parm[ReCntrl[2]];
//      String TempRFID=String(ReValue[2]);
//      RFIDList[RFIDCount++]=TempRFID;
      Serial.println("RFIDList:");
      for(int i=0;i<RFIDCount;i++)
      Serial.println(RFIDList[i]);
      }
    else  if (parm.containsKey(ReCntrl[3]))
    {
       RECERFID=parm[ReCntrl[3]];
       RFIDCount--;
      for(int i=0;i<100;i++)
      {
        if(RFIDList[i]==RECERFID)
        {
          if(i==99)
            RFIDList[i]="-1";
          for(int j=i;j<100-1;j++)
            RFIDList[j]=RFIDList[j+1];
        }
        }
//      RFIDList[RFIDCount++]=
      Serial.println("RFIDList:");
      for(int i=0;i<RFIDCount;i++)
      Serial.println(RFIDList[i]);
      }
//    else if(parm.containsKey("DoorStatus"))
//    {
//        DoorStatus=parm["DoorStatus"];
//      }
//    else if(parm.containsKey("WindowStatus"))
//    {
//      WindowStatus=parm["WindowStatus"];
//      }
//    

   

   
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
    if (strstr(topic, "inTopic"))
    {

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload); 

        if (!error) 
        {
          parmPass(doc.as<JsonVariant>()); //将参数传递后打印输出

        }
    }
//  // Switch on the LED if an 1 was received as first character
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is active low on the ESP-01)
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Device09";
//    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"bysj","123")) {
      Serial.println("connected");
//      // Once connected, publish an announcement...
//      delay(1500);
//      client.publish("outTopic","{\"SystemStart\":\"1\"}");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
String RFIDHEX[35],StrRFID;
void readRFID() {
StrRFID="";
while(1)
{
  byte message[] = {0xbb,0x00,0x39,0x00,0x09,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x06,0x4a,0x7E};
  mySerial.write(message, sizeof(message));
  // put your main code here, to run repeatedly:
  int count=0;

  while (mySerial.available() > 0) {
  
    incomingByte = mySerial.read();
//    value += String(incomingByte,HEX);
//    value += " ";
    //if(incomingByte==0x00)
    RFIDHEX[count++]=String(incomingByte,HEX);
    if(RFIDHEX[count-1].length()==1)
    RFIDHEX[count-1]="0"+RFIDHEX[count-1];
    
  }
//  if(value.length() > 1){
//    Serial.print(value);
//    Serial.println();
    if(count==34)
    {
//      Serial.println("Identification successful:");
      for(int i=20;i<=31;i++)
      {
//        Serial.println(RFIDHEX[i]);
//        Serial.println(RFIDHEX[i].length());
        StrRFID+=RFIDHEX[i];
      }
      
      Serial.println("Identification successful2:");
      Serial.println(StrRFID);
      count=0;
      break;
      }
//    value = "";
    
//  }
  delay(100); 
    
    
}

}
//void handleInterrupt() {
//  delay(100);
//  if (digitalRead(NormolClose)){// 电平高，门开
//    //detachInterrupt(digitalPinToInterrupt(NormolClose)); //屏蔽中断脚位
//    door_open_flag = 0;    
//    Serial.println("door close");
//  }
//  else
//  {
//    door_open_flag = 1;    
//    Serial.println("door open");
//    }
//}
void setup() {

  pinMode(REDLED,OUTPUT);//RED 警报灯
  pinMode(WHITELED,OUTPUT);//WHITE 门口灯
  pinMode(GREENLED,OUTPUT);//GREEN 门内灯
//  pinMode(NormolClose, INPUT);//门磁
//  attachInterrupt(digitalPinToInterrupt(NormolClose), handleInterrupt, CHANGE);
  pinMode(Infrare1Pin,INPUT);//红外1
  pinMode(Infrare2Pin,INPUT);//红外2
  pinMode(RFCHPin,INPUT);//RFID切换
//  pinMode(DoorStatusPin,INPUT);//门磁状态
//  pinMode(DoorPin,OUTPUT);//电机
//  pinMode(AlarmPin,OUTPUT);//报警
  pinMode(IN4,OUTPUT);//电机IN4
  pinMode(IN3,OUTPUT);//电机IN3
  pinMode(Indoor,INPUT);
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  mySerial.begin(115200,SERIAL_8N1,32,33);
  
//  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);                                                 // Init SPI bus
//  mfrc522.PCD_Init();                                              // Init MFRC522 card
//  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
}
int a=0;
void loop() {
   a++;
   if(a==5)
   {
     client.publish("outTopic","{\"SystemStart\":\"1\"}");
    }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(ReTimerStatus==1)
  {
//  int RFIDnum=digitalRead(RFPin);
  int Infrare1num=digitalRead(Infrare1Pin);
  int Infrare2num=digitalRead(Infrare2Pin);
  int RFIDCHnum=digitalRead(RFCHPin);
  int Indoornum=digitalRead(Indoor);
//  String strRFIDnum=String(RFIDnum);
//   String strInfrare1num=String(Infrare1num);
//    String strRFIDCHnum=String(RFIDCHnum);
//  Serial.println("this rfidpin:"+strRFIDnum);
//  Serial.println("this Infrare1num:"+strInfrare1num);
//  Serial.println("this RFIDCHnum:"+strRFIDCHnum);
//  int DoorStatusnum=digitalRead(DoorStatusPin);
  delay(1000);
//  if(DoorStatusnum==0)
//  {
//      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"0\"}");
//      Serial.print("Publish message: ");
//      Serial.println(msg);
//      client.publish("outTopic", msg);
//      //delay(1500);
//      }
//  else
//  {
//          snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"0\"}");
//      Serial.print("Publish message: ");
//      Serial.println(msg);
//      client.publish("outTopic", msg);
//      delay(1500);
//    }
//  if(RFIDCHnum)
//    RFIDList[0]="000000000000000000000001";
//  delay(1000);
  if(RFIDCHnum==1)
  {
      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"0\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
//      delay(1500);
      }
  if(Indoornum==1)
  {
      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
      digitalWrite(IN4,LOW);
      digitalWrite(IN3,HIGH);
      delay(10000);
      digitalWrite(IN4,HIGH);
      digitalWrite(IN3,LOW);
      delay(10000);
      digitalWrite(IN4,LOW);
      digitalWrite(IN3,LOW);
//      delay(1500);
      }
//  if(Infrare1num)
    
//  if((RFIDnum&&Infrare1num)||(RFIDnum&&Infrare2num))
StrRFID="11111111111111111111111111111111";
if(Infrare2num)
{
     digitalWrite(GREENLED,HIGH);
    delay(1500);
    digitalWrite(GREENLED,LOW);
  }
if(Infrare1num)
  {
    digitalWrite(WHITELED,HIGH);
    delay(1500);
    digitalWrite(WHITELED,LOW);
    readRFID();
    int RFflag=0;
    for(int i=0;i<100;i++)
      if(RFIDList[i]==StrRFID)
        RFflag=1;
    if(RFflag)
    {
      String StrRFIDtemp="{\"RFID\":\""+StrRFID+"\",\"DoorS\":\"1\"}";
      snprintf (msg, MSG_BUFFER_SIZE,StrRFIDtemp.c_str());
//      snprintf (msg, MSG_BUFFER_SIZE, "{\"RFID\":\"000000000000000000000000\",\"DoorS\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
      digitalWrite(IN4,LOW);
      digitalWrite(IN3,HIGH);
      delay(10000);
      digitalWrite(IN4,HIGH);
      digitalWrite(IN3,LOW);
      delay(10000);
      digitalWrite(IN4,LOW);
      digitalWrite(IN3,LOW);
    }
    else
    {
      if(StrRFID!="11111111111111111111111111111111")
      {
//      Serial.println(RFIDList[0]);
      snprintf (msg, MSG_BUFFER_SIZE, "{\"Alarm\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
      digitalWrite(REDLED,HIGH);
      delay(1500);
      digitalWrite(REDLED,LOW);
        
        }

      
      }
    }
//    else
//    digitalWrite(DoorPin,LOW);
   

  }

//
//  unsigned long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("outTopic", msg);
//  }
}
