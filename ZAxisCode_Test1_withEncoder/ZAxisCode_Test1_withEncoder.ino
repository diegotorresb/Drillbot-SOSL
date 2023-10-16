const int forwards = 11;
const int backwards = 10;
#define ENCA 2 // brown
#define ENCB 4 // yellow

// motor encoder ouput pulse per rotation
#define ENC_CPR 1024

int pos = 0;
int currentStateA, lastStateA;

// variable used for estimates from PID
long prevT = 0;
float eprev = 0;
float eintegral = 0;

// variables for moving encoder functions
float pos_linear_actuator, init_pos_linear_actuator;

// variables for void loop
int currentStep = 1;
int inches;
float numRevs;

// boolean to change if PID algorithm is done
bool PID_Done;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(backwards,OUTPUT);
  pinMode(forwards,OUTPUT);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  // setup channel A
  pinMode(12, OUTPUT);  // initiates Motor Channel A pin
  pinMode(9, OUTPUT);   // initiates Brake Channel A pin


  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING); // read encoder values everytime ENCA rises

}

// the loop routine runs over and over again forever:
void loop() {
  //FORWARD 559 --> 4.75 in
  //BACKWARD 456 --> 0.875 in
  inches = 1; 
  numRevs = -1.5;

  if (currentStep == 1) {        // Move Actuator Up
    MoveUp(inches);
    Serial.print("Actuator moved up ");
    Serial.print(inches);
    Serial.println(" inch(es)");
    delay(1500);
    currentStep++;
  } else if (currentStep == 2) { // Turn DC Motor arbitratry Num of Revs
    PID_Controller(numRevs, &PID_Done);
    if(PID_Done == true) {
      currentStep++;
      PID_Tester();
      delay(1000);
    }
  } else if (currentStep == 3) { // Move Actuator Down
    MoveDown(inches);
    Serial.print("Actuator moved down ");
    Serial.print(inches);
    Serial.println(" inch(es)");
    delay(1500);
    currentStep++;
  } else if (currentStep == 4) { // Pseudo Routine is Done
    Serial.println("Done with routine");
    delay(15000);
  }
}


float readPotentiometer(void) {
  return 0.0376214 * (analogRead(A5) - 456) + 0.875;
}

void MoveDown(float in) {
  init_pos_linear_actuator = readPotentiometer();
  pos_linear_actuator = readPotentiometer();
  while (pos_linear_actuator > init_pos_linear_actuator - in){
    analogWrite(backwards, 255);
    analogWrite(forwards, 0); //Activate the relay one direction, they must be different to move the motor
    pos_linear_actuator = readPotentiometer();
    delay(250);
  }
  analogWrite(backwards, 255);
  analogWrite(forwards, 255); //Deactivate both relays to brake the motor
  delay(500);
  return;
}

void MoveUp (float in) {
  init_pos_linear_actuator = readPotentiometer();
  pos_linear_actuator = readPotentiometer();
  while (pos_linear_actuator < init_pos_linear_actuator + in){
    analogWrite(backwards, 0);
    analogWrite(forwards, 255); //Activate the relay one direction, they must be different to move the motor
    pos_linear_actuator = readPotentiometer();
    delay(250);
  }
  analogWrite(backwards, 255);
  analogWrite(forwards, 255);   //Deactivate both relays to brake the motor
  delay(500);
  return;
}

void readEncoder() {                    // this function is called whenever ENCA rises
	// Read the current state of A
	currentStateA = digitalRead(ENCA);

	// If last and current state of A are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateA != lastStateA  && currentStateA == 1){

		// If the B state is different than the A state then
		if (digitalRead(ENCB) != currentStateA) {
		  // the encoder is rotating CCW so decrement
			pos--; 
		} else {
			// Encoder is rotating CW so increment
			pos++;
		}
  }
}

void setMotor(int dir, int pwr) {
  analogWrite(3, pwr);              // spins motor on Channel A at pwr value
  if(dir == 1) {                    // move motor CW
    digitalWrite(12, HIGH);         
    digitalWrite(9, LOW);

  } else if (dir == -1) {           // move motor CCW
    digitalWrite(12, LOW);
    digitalWrite(9, LOW);

  } else {                          // brake motor
    digitalWrite(12, LOW);
    digitalWrite(9, HIGH);
  }
}

void PID_Controller(float num_revs, bool* PID_Done) {     // (+) -> CW, (-) -> CCW
  // target position obtained from parameter (in rev)
  *PID_Done = false;

  // PID constants
  float kp, kd, ki;

  kp = 4.8;
  kd = 0.02;
  ki = 0.038;
  // this parameters have 6/1024 or less for [1, 3.75]


  // set target position (in pulses)
  int target = num_revs * ENC_CPR;                // (+) -> CW, (-) -> CCW

  long currT = micros();                          // current time in microseconds

  float deltaT = ((float)(currT-prevT))/1.0e6;    // change in time in seconds
  prevT = currT;

  // error e(t)
  int e = target - pos;                           // adjust if neeeded

  // derivative 
  float dedt = (e-eprev)/(deltaT);                // finite difference approximation for derivative component

  // integral 
  eintegral = eintegral + e*deltaT;               // finite difference approximation for integral component

  // control signal u(t)
  float u = kp*e + kd*dedt + ki*eintegral;        // this signal tells plant direction and speed to turn motor

  // motor power
  float pwr = fabs(u);
  if (pwr > 255) {
    pwr = 255;
  }

  // motor direction 
  int dir = 1;
  if (u < 0) {
    dir = -1;
  }

  // signal the motor
  setMotor(dir, pwr);

  // store previous error
  eprev = e;

  int myTimerMicros;
  int pos_prev = pos;
    
  if (pos_prev != pos) {
    // restart the TIMER
    myTimerMicros = micros();

    // update to the new state
    pos_prev = pos;
  } 
    
  // has the 3 second TIMER expired? if so do this
  if (micros() - myTimerMicros >= (8000000)) {    // time difference
    // done with actuator!
    *PID_Done = true;
  } /*else {
    // print target and measured positions to test algorithm
    Serial.print("Target: ");
    Serial.print(target);
    Serial.print(" Position: ");
    Serial.print(pos);
    Serial.println();   
  }
  */
  
}

// function to test PID algorithm's accuracy
void PID_Tester() {
  Serial.print("Turned: ");
  Serial.print(pos/1024.0, 6);
  Serial.print(" revs | Goal: ");
  Serial.print(numRevs, 2);
  Serial.print(" revs -> error of ");
  Serial.print(abs(numRevs*ENC_CPR-pos)/1024.0*360.0);
  Serial.println(" degrees");
}