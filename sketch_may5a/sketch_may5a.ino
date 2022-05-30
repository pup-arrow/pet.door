// Definitions Arduino pins connected to input H Bridge 
// #define ON 1
// #define OFF 0
#define MAX 255
#define MIN 0
int IN1 = 0; //motor pwm pin values 0-255
int IN2 = 4; //motor pwm pin vlues 0-255

//PWM setup
// const int freq = 5000;
// const int ledChannel = 0;
// const int ledChannel4 = 4;
// const int resolution = 8;


void setup() {
  // Set the output pins
  Serial.begin(115200);
  // Serial.println("test");
  // pinMode(IN1, OUTPUT); 
  // pinMode(IN2, OUTPUT); 
//  ledcSetup(ledChannel, freq, resolution);
//  ledcSetup(ledChannel4, freq, resolution);
//  ledcAttachPin(IN1, ledChannel);
//  ledcAttachPin(IN2, ledChannel);
//  ledcWrite(ledChannel, MAX);
//  ledcWrite(ledChannel4, MAX);
  // analogWrite(IN1, MAX); //stops motor
  // analogWrite(IN2, MAX);
  
  digitalWrite(IN1, MAX); //stops motor
  digitalWrite(IN2, MAX);
  // Serial.println("after test");
} 

void loop() {
  // Serial.println("start loop");
  // Rotates the Motor counter-clockwise
  // delay(2000); 
  // analogWrite(IN1, MAX); 
  // analogWrite(IN2, MAX); //stops motor
  // ledcWrite(ledChannel, MAX);
  // ledcWrite(ledChannel4, MAX);
  digitalWrite(IN1, MAX); //stops motor
  digitalWrite(IN2, MAX);
  delay(3000);

  // analogWrite(IN1, MAX);  //turns on motor  (need to figure out pwm to contrl speed)
  // analogWrite(IN2, MIN); 
  // ledcWrite(ledChannel, MAX);
  // ledcWrite(ledChannel4, MIN);
  digitalWrite(IN1, MAX); //stops motor
  digitalWrite(IN2, MIN);
  // Serial.println("motor ccw!");

  delay(3000);
  // analogWrite(IN1, MAX); //stops motor
  // analogWrite(IN2, MAX); 
  // Serial.println("off");

  // delay(2000);

  // Rotate the Motor clockwise 
  // analogWrite(IN1, MAX); 
  // analogWrite(IN2, 192); //turns on motor  need to figure out pwm to contrl speed
  // Serial.println("motor cw");
  // ledcWrite(ledChannel, MAX);
  // ledcWrite(ledChannel4, MAX);
  // delay(5000); 
  digitalWrite(IN1, MAX); //stops motor
  digitalWrite(IN2, MAX);
  // analogWrite(IN1, MAX); 
  // analogWrite(IN2, MAX); //stops motor
  
  delay(3000);

  // analogWrite(IN1, MIN); 
  // analogWrite(IN2, MAX); //stops motor
  // ledcWrite(ledChannel4, MAX);
  // ledcWrite(ledChannel, MIN);
  digitalWrite(IN1, MIN); //stops motor
  digitalWrite(IN2, MAX);
  // Serial.println("off");
  delay(3000);

}
