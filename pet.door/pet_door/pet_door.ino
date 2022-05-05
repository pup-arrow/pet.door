#include <ArduinoWebsockets.h>
#include "esp_http_server.h"
#include <WiFi.h>
#include <Preferences.h>
#include "BluetoothSerial.h"


// Definitions Arduino pins connected to input H Bridge 
#define IN1 5 //motor pwm pin values 0-255
#define IN2 6 //motor pwm pin vlues 0-255
#define SW1 7 //top switch
#define SW2 8 //second to top switch
#define SW3 9 // second to bottom switch
#define SW4 10 // bottom switch
#define OPEN_BTN 2 //OPEN_BTN connected to pin 2 external interrupt
#define OPEN_APP 3 //OPEN_APP connected to pin 3 external interrupt
#define PWR_LED 4  //PWR_LED connected to 7
#define FMD 14 //front motion detechtor
#define BMD 15 //back motion detector
#define ON 1
#define OFF 0
#define MAX 255
#define MIN 0

int RFID_PWR = ON; 
int old_FMD;  
int old_BMD;
int cur_FMD = HIGH;
int cur_BMD = HIGH; 

//need to research how to permanently store tag #s and read RFID #s

int reader = OFF;
int tag = OFF;

// code from internet
String ssids_array[50];
String network_string;
String connected_string;

const char* pref_ssid = "";
const char* pref_pass = "";
String client_wifi_ssid;
String client_wifi_password;

const char* bluetooth_name = "petdoor";

long start_wifi_millis;
long wifi_timeout = 10000;
bool bluetooth_disconnect = false;

enum wifi_setup_stages { NONE, SCAN_START, SCAN_COMPLETE, SSID_ENTERED, WAIT_PASS, PASS_ENTERED, WAIT_CONNECT, LOGIN_FAILED };
enum wifi_setup_stages wifi_stage = NONE;
// 
enum states {
  IDLE,
  OPEN,
  CLOSE
};

states state;  //Global variable to store which state we're in
// internet code
using namespace websockets;
WebsocketsServer socket_server;
BluetoothSerial SerialBT;
Preferences preferences;
//
void setup() {
  state = IDLE;  //initate in the idle state
  // Set the output pins
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(OPEN_BTN, INPUT);
  pinMode(OPEN_APP, INPUT);
  pinMode(FMD, INPUT);
  pinMode(BMD, INPUT);
  pinMode(PWR_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH); 
  attachInterrupt(digitalPinToInterrupt(2),openbtnPressed,RISING); 
  attachInterrupt(digitalPinToInterrupt(3),openappPressed,RISING); 

  Serial.begin(115200);
  Serial.println("Booting...");

  preferences.begin("wifi_access", false);
}

bool init_wifi()
{
  String temp_pref_ssid = preferences.getString("pref_ssid");
  String temp_pref_pass = preferences.getString("pref_pass");
  pref_ssid = temp_pref_ssid.c_str();
  pref_pass = temp_pref_pass.c_str();

  Serial.println(pref_ssid);
  Serial.println(pref_pass);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);

  start_wifi_millis = millis();
  WiFi.begin(pref_ssid, pref_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_wifi_millis > wifi_timeout) {
      WiFi.disconnect(true, true);
      return false;
    }
  }
  return true;
}

void scan_wifi_networks()
{
  WiFi.mode(WIFI_STA);
  // WiFi.scanNetworks will return the number of networks found
  int n =  WiFi.scanNetworks();
  if (n == 0) {
    SerialBT.println("no networks found");
  } else {
    SerialBT.println();
    SerialBT.print(n);
    SerialBT.println(" networks found");
    delay(1000);
    for (int i = 0; i < n; ++i) {
      ssids_array[i + 1] = WiFi.SSID(i);
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(ssids_array[i + 1]);
      network_string = i + 1;
      network_string = network_string + ": " + WiFi.SSID(i) + " (Strength:" + WiFi.RSSI(i) + ")";
      SerialBT.println(network_string);
    }
    wifi_stage = SCAN_COMPLETE;
  }
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    wifi_stage = SCAN_START;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == SCAN_COMPLETE) { // data from phone is SSID
    int client_wifi_ssid_id = SerialBT.readString().toInt();
    client_wifi_ssid = ssids_array[client_wifi_ssid_id];
    wifi_stage = SSID_ENTERED;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == WAIT_PASS) { // data from phone is password
    client_wifi_password = SerialBT.readString();
    client_wifi_password.trim();
    wifi_stage = PASS_ENTERED;
  }

}

void callback_show_ip(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    SerialBT.print("ESP32 IP: ");
    SerialBT.println(WiFi.localIP());
    bluetooth_disconnect = true;
  }
}

void disconnect_bluetooth()
{
  delay(1000);
  Serial.println("BT stopping");
  SerialBT.println("Bluetooth disconnecting...");
  delay(1000);
  SerialBT.flush();
  SerialBT.disconnect();
  SerialBT.end();
  Serial.println("BT stopped");
  delay(1000);
  bluetooth_disconnect = false;
}

void loop() {
  if (bluetooth_disconnect)
  {
    disconnect_bluetooth();
  }

  switch (wifi_stage)
  {
    case SCAN_START:
      SerialBT.println("Scanning Wi-Fi networks");
      Serial.println("Scanning Wi-Fi networks");
      scan_wifi_networks();
      SerialBT.println("Please enter the number for your Wi-Fi");
      wifi_stage = SCAN_COMPLETE;
      break;

    case SSID_ENTERED:
      SerialBT.println("Please enter your Wi-Fi password");
      Serial.println("Please enter your Wi-Fi password");
      wifi_stage = WAIT_PASS;
      break;

    case PASS_ENTERED:
      SerialBT.println("Please wait for Wi-Fi connection...");
      Serial.println("Please wait for Wi_Fi connection...");
      wifi_stage = WAIT_CONNECT;
      preferences.putString("pref_ssid", client_wifi_ssid);
      preferences.putString("pref_pass", client_wifi_password);
      if (init_wifi()) { // Connected to WiFi
        connected_string = "ESP32 IP: ";
        connected_string = connected_string + WiFi.localIP().toString();
        SerialBT.println(connected_string);
        Serial.println(connected_string);
        bluetooth_disconnect = true;
      } else { // try again
        wifi_stage = LOGIN_FAILED;
      }
      break;

    case LOGIN_FAILED:
      SerialBT.println("Wi-Fi connection failed");
      Serial.println("Wi-Fi connection failed");
      delay(2000);
      wifi_stage = SCAN_START;
      break;
  }

  switch(state){
    case IDLE:
    if (state == OPEN){
      break;  // switch to open state
    }
    old_FMD = cur_FMD;
    cur_FMD = digitalRead(FMD);
    old_BMD = cur_BMD;
    cur_BMD = digitalRead(BMD);
    if (old_FMD == LOW && cur_FMD == HIGH){
      //send notification to app for inside movement
    }
    if (old_BMD == LOW && cur_BMD == HIGH){
      //send notification to app for outside movement
    }


    break;

    case OPEN:
    // Rotates the Motor counter-clockwise 
    digitalWrite(IN1, MIN);  //turns on motor  (need to figure out pwm to contrl speed)
    digitalWrite(IN2, MAX); 
    OPENING:
    if (SW1 == HIGH){ //motor runs until last switch cleared
      goto OPENING;
    }
    digitalWrite(IN1, MAX); //stops motor
    digitalWrite(IN2, MAX); 
    SEEN:
    delay(4000); //time before door will check to close (4s)
    if (reader == tag){//reader still see tag?
      goto SEEN; //return to delay and check for tag again
    }
    state = CLOSE;
    break;

    case CLOSE:
    // Rotate the Motor clockwise 
    digitalWrite(IN1, MAX); 
    digitalWrite(IN2, 192); //turns on motor  need to figure out pwm to contrl speed
    if (state == OPEN){break;} //if open btn or signal leave close state
    delay(1000); // 1s
    if (SW1 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, MAX); 
      digitalWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW2 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, MAX); 
      digitalWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW3 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, MAX); 
      digitalWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW4 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, MAX); 
      digitalWrite(IN2, MAX);
      state = OPEN;
      break;
    }
    delay(1000); //time motor runs (1s)
    digitalWrite(IN1, MAX); 
    digitalWrite(IN2, MAX); //stops motor
    //send door use notification?
    state = IDLE;
    break;
  }

}

void openbtnPressed(){ //open button pressed interrupt        
  if (state != OPEN){ // if already in open state do nothing else stop motor
  analogWrite(IN1, MAX); 
  analogWrite(IN2, MAX);
  state = OPEN;
  }
}

void openappPressed(){ //open signal from app interrupt         
  if (state != OPEN){ // if already in open state do nothing else stop motor
  digitalWrite(IN1, MAX); 
  digitalWrite(IN2, MAX);
  state = OPEN;
  }
}
