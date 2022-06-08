
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   22  
#define SS_PIN    21  
#define MISO_PIN  19 
#define MOSI_PIN  23 
#define SCK_PIN   18 

// Definitions Arduino pins connected to input H Bridge 
#define IN2 27 //motor pwm pin values 0-255 (D11)
#define IN1 4 //motor pwm pin vlues 0-255 (D10)
#define SW1 26 //top switch (D12)
#define SW2 2 //second to top switch (D8) 
#define SW3 5 // second to bottom switch (D7)
#define SW4 15 // bottom switch (D5)
#define OPEN_BTN 13 //OPEN_BTN connected to pin 2 external interrupt (D2)
// #define RFID_PWR 12 //OPEN_APP connected to pin 3 external interrupt (D3)
#define PWR_LED 34  //PWR_LED connected to (S4)
#define TIMER_LED 35  //PWR_LED connected to (S5)
#define FMD 36 //front motion detechtor (S0)
#define MAX 255
#define MIN 0

// int RFID_PWR = HIGH; 
int old_FMD;  
int cur_FMD = HIGH;

const char* pet_1 = "";
const char* pet_2 = "";
int stall = 0;
String StrRFID = "";
String ID_pet;


enum states {
  IDLE,
  OPEN,
  CLOSE
};

states state;  //Global variable to store which state we're in


Preferences preferences;
//

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
// Hardcoded credentials for WiFi HotSpot
//mosquitto linux
const char* ssid = "Arrow";
const char* password = "5805747059";
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
// String RFIDList[100];
String ReCntrl[10]={"OpenDoor","TimerStatus","RFIDADD","RFIDDELE"};
int ReOpenDoor=0;
int ReTimerStatus=1;
int RFIDCount=0;
const char *RECERFID = NULL;  
bool door_open_flag; 
HardwareSerial mySerial(1);
unsigned char incomingByte;

void setup_wifi() { //Bluetooth to WiFi code too large to hardcoding Wifi setup
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

void parmPass(JsonVariant parm){ //Json means MQTT parameter
  if (parm.containsKey(ReCntrl[0])){ //ReCntrl[0] = app open button
    ReOpenDoor=parm[ReCntrl[0]];
    if(ReOpenDoor){
      state = OPEN;
    }
    snprintf (msg, MSG_BUFFER_SIZE, "{\"DoorStatus\":\"1\"}"); //DoorStatus "1" means door is open
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
  else  if (parm.containsKey(ReCntrl[1])){ //timer on = 1, door working, timer off = 0
    ReTimerStatus=parm[ReCntrl[1]];
    Serial.println("ReTimerStatus");
    Serial.println(ReTimerStatus); 

  }
  else  if (parm.containsKey(ReCntrl[2])){ // Add pet id #, limit is 2 pets
    RECERFID=parm[ReCntrl[2]];
    //adding pet id # into list
    ID_pet = RECERFID;
    String temp_pet1 = preferences.getString("pet_1");
    String temp_pet2 = preferences.getString("pet_2");
    if (temp_pet1.equals("")){
      preferences.putString("pet_1", ID_pet);
    }
    else if (temp_pet2.equals("")){
      preferences.putString("pet_2", ID_pet);
    }
  }
  else  if (parm.containsKey(ReCntrl[3])){ //remove pet ID
    RECERFID=parm[ReCntrl[3]];
    ID_pet = RECERFID;
    String temp_pet1 = preferences.getString("pet_1");
    String temp_pet2 = preferences.getString("pet_2");
    if (temp_pet1.equals(ID_pet)){
      preferences.putString("pet_1", "");
    }
    else if (temp_pet2.equals(ID_pet)){
      preferences.putString("pet_2", "");
    }
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
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Device09";
    // Attempt to connect
    if (client.connect(clientId.c_str(),"bysj","123")) {
      Serial.println("connected");
    // Once connected, publish an announcement...
      client.subscribe("inTopic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again");
      // Wait 5 seconds before retrying
      // delay(5000);
    }
  }
}

void readRFID() {
delay(100); 
StrRFID="";
String RFIDHEX[35];

  byte message[] = {0xbb,0x00,0x39,0x00,0x09,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x06,0x4a,0x7E};
  mySerial.write(message, sizeof(message));
  int count=0;

  while (mySerial.available() > 0) {
    incomingByte = mySerial.read();
    RFIDHEX[count++]=String(incomingByte,HEX);
    if(RFIDHEX[count-1].length()==1)
    RFIDHEX[count-1]="0"+RFIDHEX[count-1];
  }
  if(count==34){
      for(int i=20;i<=31;i++){
        StrRFID+=RFIDHEX[i];
      }
      Serial.println("Identification successful2:");
      Serial.println(StrRFID);
      count=0;
      // break;
      delay(5000);
    }
  // delay(100); 
}

void compare_pet(){
  String temp_pet1 = preferences.getString("pet_1");
  String temp_pet2 = preferences.getString("pet_2");
  if ((StrRFID.equals(temp_pet1) && StrRFID != "")|| (StrRFID.equals(temp_pet2) && StrRFID != "")){
    String StrRFIDtemp="{\"RFID\":\""+StrRFID+"\",\"DoorS\":\"1\"}";
    snprintf (msg, MSG_BUFFER_SIZE,StrRFIDtemp.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    state = OPEN;
  }
}


void setup() {
  state = IDLE;  //initate in the idle state
  pinMode(FMD, INPUT);
  // pinMode(RFCHPin,INPUT);//RFID
  pinMode(SW1, INPUT); 
  pinMode(SW2, INPUT); 
  pinMode(SW3, INPUT); 
  pinMode(SW4, INPUT); 
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(OPEN_BTN, INPUT);
  pinMode(PWR_LED, OUTPUT);
  pinMode(TIMER_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH);
  analogWrite(IN1, MIN); //stops not energized
  analogWrite(IN2, MIN); 
  attachInterrupt(digitalPinToInterrupt(OPEN_BTN),openbtnPressed,CHANGE); 
  preferences.begin("pet_id", false); // allows for saving id #s to permanent memory
  Serial.begin(115200);
  mySerial.begin(115200,SERIAL_8N1,32,33);// initialize the  hardwareserial pin32,33
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN); // Init SPI bus
  mfrc522.PCD_Init();                    // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  switch(state){
    case IDLE:
     Serial.println("IDLE ");
    if (state == OPEN){
      break;  // switch to open state
    }
    if (stall==1){
     client.publish("outTopic","{\"SystemStart\":\"1\"}");
     stall++;
    }
    else if(stall==0){
     delay(5000);
     stall++;
    }
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    old_FMD = cur_FMD;
    cur_FMD = digitalRead(FMD);
    if (old_FMD == LOW && cur_FMD == HIGH){
      //send notification to app for inside movement
      snprintf (msg, MSG_BUFFER_SIZE, "{\"Alarm\":\"1\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }
    if (ReTimerStatus == 1){
      digitalWrite(TIMER_LED, HIGH);
      readRFID();
      compare_pet();
    }
    else{
      digitalWrite(TIMER_LED, LOW);
    }
    break;
  case OPEN:
    Serial.println("OPEN");
    // Rotates the Motor counter-clockwise 
    analogWrite(IN1, MIN);  //turns on motor  (need to figure out pwm to contrl speed)
    analogWrite(IN2, MAX); 
    OPENING:
    if (digitalRead(SW1) == LOW){ //motor runs until last switch cleared
      goto OPENING;
    }
    delay(3000);
    analogWrite(IN1, MAX); //stops motor
    analogWrite(IN2, MAX); 
    SEEN:
    delay(4000); //time before door will check to close (4s)
    //reader still see tag?
    readRFID();
    if ((StrRFID.equals(preferences.getString("pet_1")) && StrRFID != "")|| (StrRFID.equals(preferences.getString("pet_2")) && StrRFID != "")){
      goto SEEN; //return to delay and check for tag again
    }
    state = CLOSE;
    break;

    case CLOSE:
    // Rotate the Motor clockwise 
    analogWrite(IN1, 100); 
    analogWrite(IN2, MIN); //turns on motor  need to figure out pwm to contrl speed
    if (state == OPEN){break;} //if open btn or signal leave close state
    delay(2000); // 1s
    Serial.println("close1");
    if (digitalRead(SW1) != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(2000); 
    Serial.println("close 2");
    if (digitalRead(SW2) != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(2000); 
    if (digitalRead(SW3) != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(2000); 
    if (digitalRead(SW4) != LOW || state == OPEN){ //obstruction in door 
      analogWrite(IN1, MAX); 
      analogWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(2000); //time motor runs (2.5s)
    analogWrite(IN1, MIN); 
    analogWrite(IN2, MIN); //stops motor
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