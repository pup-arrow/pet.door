// Definitions Arduino pins connected to input H Bridge 
int IN1 = 4; 
int IN2 = 5; 
const unit8_t OPEN_BTN = 2; //OPEN_BTN connected to pin 2 external interrupt
const unit8_t OPEN_APP = 3; //OPEN_APP connected to pin 3 external interrupt
const unit8_t PWR_LED = ?;  //PWR_LED connected to ?

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
  digitalWrite(PWR_LED, HIGH);
  attachInterrupt(digitalPinToInterrupt(2),openbtnPressed,RISING); 
  attachInterrupt(digitalPinToInterrupt(3),openappPressed,RISING); 
}

void loop() {
  void openbtnPressed(){ //open button pressed interrupt        
   if (state != OPEN){ // if already in open state do nothing else stop motor
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, HIGH);
    state = OPEN;
   }
  }
  void openappPressed(){ //open signal from app interrupt         
   if (state != OPEN){ // if already in open state do nothing else stop motor
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, HIGH);
    state = OPEN;
   }
  }
  switch(state){
    case IDLE:
    if (state == OPEN){
      break;  // switch to open state
    }
    break;

    case OPEN:
    // Rotates the Motor counter-clockwise 
    digitalWrite(IN1, LOW);  //turns on motor  need to figure out pwm to contrl speed
    digitalWrite(IN2, HIGH); 
    delay(1000); //time motor runs (1s)  how to set a top limit to where dooe is raised every time?
    digitalWrite(IN1, HIGH); //stops motor
    digitalWrite(IN2, HIGH); 
    SEEN:
    delay(4000); //time before door will check to close (4s)
    if (reader == tag){//reader still see tag?
      goto SEEN; //return to delay and check for tag again
    state = CLOSE
    break;

    case CLOSE:
    // Rotate the Motor clockwise 
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, LOW); //turns on motor  need to figure ut pwm to contrl speed
    if (state == OPEN){break;} //if open btn or signal leave close state
    delay(1000); // 1s
    if (SW1 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, HIGH); 
      digitalWrite(IN2, HIGH);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW2 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, HIGH); 
      digitalWrite(IN2, HIGH);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW3 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, HIGH); 
      digitalWrite(IN2, HIGH);
      state = OPEN;
      break;
    }
    delay(1000); 
    if (SW4 != HIGH || state == OPEN){ //obstruction in door 
      digitalWrite(IN1, HIGH); 
      digitalWrite(IN2, HIGH);
      state = OPEN;
      break;
    }
    delay(1000); //time motor runs (1s)
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, HIGH); //stops motor
    state = IDLE;
    break;
  }

}
