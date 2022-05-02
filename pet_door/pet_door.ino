

enum states {
  IDLE,
  OPEN,
  CLOSE
};

states state;  //Global variable to store state we're in


void setup() {
  state = IDLE;  //initate in the idle state

}

void loop() {
  switch(state){
    case IDLE:
    break;

    case OPEN:
    break;

    case CLOSE:
    break;
  }

}
