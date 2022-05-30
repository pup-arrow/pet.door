
// Definitions Arduino pins connected to input H Bridge 
#define IN1 0 //motor pwm pin values 0-255 (D9)
#define IN2 4 //motor pwm pin vlues 0-255 (D10)
#define SW1 26 //top switch (D12)
#define SW2 27 //second to top switch (D11)
#define SW3 2 // second to bottom switch (D8)
#define SW4 5 // bottom switch (D7)
#define OPEN_BTN 13 //OPEN_BTN connected to pin 2 external interrupt (D2)
// #define OPEN_APP 3 //OPEN_APP connected to pin 3 external interrupt
#define PWR_LED 25  //PWR_LED connected to (D6)
// #define FMD 15 //front motion detechtor (D5)
// #define BMD 14 //back motion detector (D4)
#define MAX 255
#define MIN 0

int RFID_PWR = HIGH; 
// int old_FMD;  
// int old_BMD;
// int cur_FMD = HIGH;
// int cur_BMD = HIGH; 

// need to research how to permanently store tag #s and read RFID #s

int reader = LOW;
int tag = LOW;

enum states {
  IDLE,
  OPEN,
  CLOSE
};

states state;  //Global variable to store which state we're in

void setup() {
  state = IDLE;  //initate in the idle state
  // Set the output pins
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(OPEN_BTN, INPUT);
  // pinMode(OPEN_APP, INPUT);
  // pinMode(FMD, INPUT);
  // pinMode(BMD, INPUT);
  pinMode(PWR_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH); 
  analogWrite(IN1, MAX); //stops motor
  analogWrite(IN2, MAX);
  attachInterrupt(digitalPinToInterrupt(OPEN_BTN),openbtnPressed,RISING); 
  // attachInterrupt(digitalPinToInterrupt(3),openappPressed,RISING); 
  // Serial.begin(115200);
  // Serial.println("Booting...");
}

void loop() {

  switch(state){
    case IDLE:
    if (state == OPEN){
      break;  // switch to open state
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
  if (state != OPEN){ // if already in open state do nothing else stop motor
  analogWrite(IN1, MAX); 
  analogWrite(IN2, MAX);
  state = OPEN;
  }
}

