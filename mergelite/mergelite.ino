
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
// #define Infrare1Pin 25
// #define Infrare2Pin 5
#define RFCHPin 27
#define REDLED 15
#define WHITELED 14
#define GREENLED 12

// #define IN4 0
// #define IN3 2
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   22  
#define SS_PIN    21  
#define MISO_PIN  19 
#define MOSI_PIN  23 
#define SCK_PIN   18 

// Definitions Arduino pins connected to input H Bridge 
#define IN1 0 //motor pwm pin values 0-255 (D9)
#define IN2 4 //motor pwm pin vlues 0-255 (D10)
#define SW1 26 //top switch (D12)
#define SW2 27 //second to top switch (D11)
#define SW3 2 // second to bottom switch (D8)
#define SW4 5 // bottom switch (D7)
#define OPEN_BTN 13 //OPEN_BTN connected to pin 2 external interrupt (D2)
#define OPEN_APP 3 //OPEN_APP connected to pin 3 external interrupt
#define PWR_LED 25  //PWR_LED connected to (D6)
#define FMD 15 //front motion detechtor (D5)
#define MAX 255
#define MIN 0

int RFID_PWR = HIGH; 
int old_FMD;  
int cur_FMD = HIGH;

// need to research how to permanently store tag #s and read RFID #s

int reader = LOW;
int tag = LOW;

enum states {
  IDLE,
  OPEN,
  CLOSE
};

states state;  //Global variable to store which state we're in
// internet code

Preferences preferences;
//

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
const char* ssid = "Yuriy";
const char* password = "Cy09080701";
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

void parmPass(JsonVariant parm) //Json means MQTT parameter
{
    if (parm.containsKey(ReCntrl[0])) //ReCntrl[0] = app open button
    {
        ReOpenDoor=parm[ReCntrl[0]];
        if(ReOpenDoor)
        {
            digitalWrite(OPEN_APP, HIGH);
          }
      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"1\"}"); //DoorStatus "1" means door is open
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }
    else  if (parm.containsKey(ReCntrl[1]))
    {
      ReTimerStatus=parm[ReCntrl[1]];
      Serial.println("ReTimerStatus");
      Serial.println(ReTimerStatus);
      }
    else  if (parm.containsKey(ReCntrl[2])) // Open if pet ID match RFID
    {
       RECERFID=parm[ReCntrl[2]];
       RFIDList[RFIDCount++]=RECERFID;
       
      Serial.println("RFIDList:");
      for(int i=0;i<RFIDCount;i++)
      Serial.println(RFIDList[i]);
      }
    else  if (parm.containsKey(ReCntrl[3])) //Pet ID
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
      Serial.println("RFIDList:");
      for(int i=0;i<RFIDCount;i++)
      Serial.println(RFIDList[i]);
      }

   

   
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

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Device09";
    // Attempt to connect
    if (client.connect(clientId.c_str(),"bysj","123")) {
      Serial.println("connected");
//      // Once connected, publish an announcement...
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
String StrRFID;
void readRFID()
{
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------
  Serial.println("TEST:");
  byte *p;
  int rfidbit[35];
  int k=0;

  StrRFID="";
  p=mfrc522.uid.uidByte;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
      for(int j=7;j>=0;j--)
      {
         rfidbit[k++]=bitRead(p[i], j);
         Serial.print(rfidbit[k-1]);
         StrRFID+=String(rfidbit[k-1]);
        }
  }

  Serial.println();
  Serial.println(StrRFID);
  Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");


  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
  }


  Serial.println(F("\n**End Reading**\n"));

  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  }
void setup() {
  state = IDLE;  //initate in the idle state
  pinMode(REDLED,OUTPUT);//RED 警报灯
  pinMode(WHITELED,OUTPUT);//WHITE 门口灯
  pinMode(GREENLED,OUTPUT);//GREEN 门内灯
  pinMode(FMD, INPUT);
  pinMode(RFCHPin,INPUT);//RFID切换
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(OPEN_BTN, INPUT);
  pinMode(PWR_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH);
  analogWrite(IN1, MAX); //stops motor
  analogWrite(IN2, MAX); 
  attachInterrupt(digitalPinToInterrupt(OPEN_BTN),openbtnPressed,RISING); 
  attachInterrupt(digitalPinToInterrupt(3),openappPressed,RISING); 
  preferences.begin("pet_id", false); // allows for saving id #s to permanent memory
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);                                                 // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
}
int a=0;
void loop() {
  switch(state){
    case IDLE:
    if (state == OPEN){
      break;  // switch to open state
    }
    if (a==1){
     client.publish("outTopic","{\"SystemStart\":\"1\"}");
    }
    else if(a==0){
     delay(5000);
     a++;
    }
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    if(ReTimerStatus==1)
    {
    // int Infrare1num=digitalRead(Infrare1Pin);
    // int Infrare2num=digitalRead(Infrare2Pin);
    int RFIDCHnum=digitalRead(RFCHPin);
    if(RFIDCHnum==1)
    {
      snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"0\"}"); // 0 means door closed
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }

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
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("outTopic", msg);
        state = OPEN;
        break;
      }
      else
      {
        if(StrRFID!="11111111111111111111111111111111")
        {
          Serial.println(RFIDList[0]);
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
    old_FMD = cur_FMD;
    cur_FMD = digitalRead(FMD);
    if (old_FMD == LOW && cur_FMD == HIGH){
      //send notification to app for inside movement

    }
    break;
  case OPEN:
    // Rotates the Motor counter-clockwise 
    analogWrite(IN1, MIN);  //turns on motor  (need to figure out pwm to contrl speed)
    analogWrite(IN2, MAX); 
    OPENING:
    if (SW1 == LOW){ //motor runs until last switch cleared
      goto OPENING;
    }
    delay(1000);
    analogWrite(IN1, MAX); //stops motor
    analogWrite(IN2, MAX); 
    SEEN:
    delay(4000); //time before door will check to close (4s)
    if (reader == tag){//reader still see tag?
      goto SEEN; //return to delay and check for tag again
    }
    state = CLOSE;
    break;

    case CLOSE:
    // Rotate the Motor clockwise 
    analogWrite(IN1, MAX); 
    analogWrite(IN2, 192); //turns on motor  need to figure out pwm to contrl speed
    if (state == OPEN){break;} //if open btn or signal leave close state
    delay(1000); // 1s
    if (SW1 != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW2 != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW3 != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW4 != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); //time motor runs (1s)
    analogWrite(IN1, MAX); 
    analogWrite(IN2, MAX); //stops motor
    //send door use notification?
    state = IDLE;
    break;
  }
}

void openbtnPressed(){ //open button pressed interrupt        
  if (state != OPEN){ // if already in open state do nothing, else stop motor
  analogWrite(IN1, MAX); 
  analogWrite(IN2, MAX);
  state = OPEN;
  }
}

void openappPressed(){ //open signal from app interrupt         
  if (state != OPEN){ // if already in open state do nothing, else stop motor
  digitalWrite(IN1, MAX); 
  digitalWrite(IN2, MAX);
  state = OPEN;
  }
}