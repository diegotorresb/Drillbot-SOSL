const int forwards = 11;
const int backwards = 10;

int choice;
int recVal;
float pos, init_pos, inches;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(backwards,OUTPUT);
  pinMode(forwards,OUTPUT);
  //Setup channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin
}

// the loop routine runs over and over again forever:
void loop() {
  //FORWARD 559 --> 4.75 in
  //BACKWARD 456 --> 0.875 in

  while (Serial.available() == 0)   
  {  }  

  choice = Serial.parseInt(); //Reading the Input integer from Serial port.

  inches = 1; 
  recVal = 1;
  
  switch (choice) {
    case 1:
      MoveUp(inches);
      break;
    case 2:
      MoveDown(inches);
      break;
    case 3:
      MoveMotorCW(recVal);
      break;
    case 4:
      MoveMotorCCW(recVal);
      break;
    default:
      break;
  }
  delay(100);
}

void MoveMotorCW(int recVal){
  digitalWrite(12, HIGH);    //Establishes forward direction of Channel A
  digitalWrite(9, LOW);     //Disengage the Brake for Channel A
  analogWrite(3, 255);       //Spins the motor on Channel A at full speed
  delay(recVal * 1000);
  analogWrite(3,0);          //255 -> full speed, 123 -> half speed
  digitalWrite(9, HIGH);     //Eengage the Brake for Channel A
  delay(2000);
}

void MoveMotorCCW(int recVal) {
  digitalWrite(12, LOW);     //Establishes backward direction of Channel A
  digitalWrite(9, LOW);      //Disengage the Brake for Channel A
  analogWrite(3, 255);       //Spins the motor on Channel A at full speed
  delay(recVal * 1000);
  analogWrite(3,0);          //255 -> full speed, 123 -> half speed
  digitalWrite(9, HIGH);     //Eengage the Brake for Channel A
  delay(2000);
}

float readPotentiometer(void) {
  return 0.0376214 * (analogRead(A5) - 456) + 0.875;
}

void MoveDown(float in) {
  init_pos = readPotentiometer();
  pos = readPotentiometer();
  while (pos > init_pos - in){
    analogWrite(backwards, 255);
    analogWrite(forwards, 0); //Activate the relay one direction, they must be different to move the motor
    pos = readPotentiometer();
    delay(250);
  }
  analogWrite(backwards, 255);
  analogWrite(forwards, 255); //Deactivate both relays to brake the motor
  delay(500);
  return;
}

void MoveUp (float in) {
  init_pos = readPotentiometer();
  pos = readPotentiometer();
  while (pos < init_pos + in){
    analogWrite(backwards, 0);
    analogWrite(forwards, 255); //Activate the relay one direction, they must be different to move the motor
    pos = readPotentiometer();
    delay(250);
  }
  analogWrite(backwards, 255);
  analogWrite(forwards, 255);   //Deactivate both relays to brake the motor
  delay(500);
  return;
}