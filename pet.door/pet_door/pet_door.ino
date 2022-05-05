// Definitions Arduino pins connected to input H Bridge 
#define IN1 5 //motor pwm pin values 0-255
#define IN2 6 //motor pwm pin vlues 0-255
#define SW1 9 //top switch
#define SW2 10 //second to top switch
#define SW3 11 // second to bottom switch
#define SW4 12 // bottom switch
#define OPEN_BTN 2 //OPEN_BTN connected to pin 2 external interrupt
#define OPEN_APP 3 //OPEN_APP connected to pin 3 external interrupt
#define PWR_LED 7  //PWR_LED connected to 7
#define FMD 4 //front motion detechtor
#define BMD 8 //back motion detector
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
  pinMode(OPEN_APP, INPUT);
  pinMode(FMD, INPUT);
  pinMode(BMD, INPUT);
  pinMode(PWR_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH); 
  attachInterrupt(digitalPinToInterrupt(2),openbtnPressed,RISING); 
  attachInterrupt(digitalPinToInterrupt(3),openappPressed,RISING); 
}

void loop() {
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
