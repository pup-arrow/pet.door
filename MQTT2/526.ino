
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define RFPin 26
#define Infrare1Pin 4
//#define Infrare2Pin 4
#define RFCHPin 27
#define DoorPin 0
#define AlarmPin 2
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
#define MSG_BUFFER_SIZE  (50)
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
          digitalWrite(DoorPin,HIGH);
          delay(1500);
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

void setup() {
  pinMode(RFPin,INPUT);//门磁
  pinMode(Infrare1Pin,INPUT);//红外1
//  pinMode(Infrare2Pin,INPUT);//红外2
  pinMode(RFCHPin,INPUT);//RFID切换
//  pinMode(DoorStatusPin,INPUT);//门磁状态
  pinMode(DoorPin,OUTPUT);//电机
  pinMode(AlarmPin,OUTPUT);//报警
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
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
  if(ReTimerStatus==1)
  {
  int RFIDnum=digitalRead(RFPin);
  int Infrare1num=digitalRead(Infrare1Pin);
//  int Infrare2num=digitalRead(Infrare2Pin);
  int RFIDCHnum=digitalRead(RFCHPin);
  
  String strRFIDnum=String(RFIDnum);
   String strInfrare1num=String(Infrare1num);
    String strRFIDCHnum=String(RFIDCHnum);
//  Serial.println("this rfidpin:"+strRFIDnum);
//  Serial.println("this Infrare1num:"+strInfrare1num);
//  Serial.println("this RFIDCHnum:"+strRFIDCHnum);
//  int DoorStatusnum=digitalRead(DoorStatusPin);
//  delay(1000);
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
  delay(1000);
  if(RFIDCHnum==1)
  {
      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"0\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
//      delay(1500);
      }

//  if(Infrare1num)
    
//  if((RFIDnum&&Infrare1num)||(RFIDnum&&Infrare2num))
if(RFIDnum&&Infrare1num)
  {
    int RFflag=0;
    for(int i=0;i<100;i++)
      if(RFIDList[i]=="000000000000000000000000")
        RFflag=1;
    if(RFflag)
    {
      snprintf (msg, MSG_BUFFER_SIZE, "{\"RFID\":\"000000000000000000000000\",\"DoorS\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
      digitalWrite(DoorPin,HIGH);
      delay(1500);
    }
    else
    {
      Serial.println(RFIDList[0]);
      snprintf (msg, MSG_BUFFER_SIZE, "{\"Alarm\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
      digitalWrite(AlarmPin,HIGH);
      delay(1500);
      digitalWrite(AlarmPin,LOW);
      
      }
    }
    else
    digitalWrite(DoorPin,LOW);
   
  if (!client.connected()) {
    reconnect();
  }
  }
  client.loop();
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
