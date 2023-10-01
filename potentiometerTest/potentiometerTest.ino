const int forwards = 11;
const int backwards = 10;

int sensorVal;
int recVal;
bool start = true;
float pos, init_pos, inches;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(backwards,OUTPUT);
  pinMode(forwards,OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  //FORWARD 559 -- 4.75 in

  //BACKWARD 456 -- 0.875 in

  while (Serial.available() == 0)   
  {  }  
  inches = Serial.parseFloat(); //Reading the Input string from Serial port.
  
  MoveUp(inches);
  delay(100);
  MoveDown(inches);

}

float readPotentiometer(void) {
  return 0.0376214 * (analogRead(A5) - 456) + 0.875;
}

void MoveDown(float in) {
  init_pos = readPotentiometer();
  pos = readPotentiometer();
  while (pos > init_pos - in){
    analogWrite(backwards, 255);
    analogWrite(forwards, 0);//Activate the relay one direction, they must be different to move the motor
    pos = readPotentiometer();
    delay(250);
  }
  analogWrite(backwards, 255);
  analogWrite(forwards, 255);//Deactivate both relays to brake the motor
  delay(500);
  return;
}

void MoveUp (float in) {
  init_pos = readPotentiometer();
  pos = readPotentiometer();
  while (pos < init_pos + in){
    analogWrite(backwards, 0);
    analogWrite(forwards, 255);//Activate the relay one direction, they must be different to move the motor
    pos = readPotentiometer();
    delay(250);
  }
  analogWrite(backwards, 255);
  analogWrite(forwards, 255);//Deactivate both relays to brake the motor
  delay(500);
  return;
}