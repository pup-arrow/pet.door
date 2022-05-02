//Portion of code to operate motor used from Handson Technology

// Definitions Arduino pins connected to input H Bridge 
int IN1 = 4; 
int IN2 = 5; 
const unit8_t OPEN_BTN = ?; //OPEN_BTN connected to ?

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

}

void loop() {
  switch(state){
    case IDLE:
    if (digitalRead(OPEN_BTN) == HIGH){ // if open button is pushed on door 
      state = OPEN;  // switch to open state
    }
    break;

    case OPEN:
    // Rotates the Motor counter-clockwise digitalWrite(IN1, LOW); 
    digitalWrite(IN2, HIGH); 
    delay(2000); 
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, HIGH); 
    delay(500); 
    break;

    case CLOSE:
    if (digitalRead(OPEN_BTN) == HIGH){ // if open button is pushed on door 
      state = OPEN;  // switch to open state
    }
    // Rotate the Motor clockwise digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, LOW); 
    delay(2000); 
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, HIGH); 
    delay(500); 
    break;
  }

}
