// Definitions Arduino pins connected to input H Bridge 
// #define ON 1
// #define OFF 0
#define MAX 255
#define MIN 0
int IN1 = 0; //motor pwm pin values 0-255
int IN2 = 4; //motor pwm pin vlues 0-255


void setup() {
  // Set the output pins
  Serial.begin(115200);
  // Serial.println("test");
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  // analogWrite(IN1, MAX); //stops motor
  // analogWrite(IN2, MAX);
  // Serial.println("after test");
} 

void loop() {
  // Serial.println("start loop");
  // Rotates the Motor counter-clockwise
  // delay(2000); 
  analogWrite(IN1, MAX); 
  analogWrite(IN2, MAX); //stops motor
  delay(3000);

  analogWrite(IN1, MAX);  //turns on motor  (need to figure out pwm to contrl speed)
  analogWrite(IN2, MIN); 
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

  // delay(5000); 
  analogWrite(IN1, MAX); 
  analogWrite(IN2, MAX); //stops motor
  delay(3000);

  analogWrite(IN1, MIN); 
  analogWrite(IN2, MAX); //stops motor
  // Serial.println("off");
  delay(3000);

}
