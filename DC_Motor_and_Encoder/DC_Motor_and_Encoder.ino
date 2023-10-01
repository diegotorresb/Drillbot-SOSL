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

void setup() {
  Serial.begin(9600);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  // setup channel A
  pinMode(12, OUTPUT);  // initiates Motor Channel A pin
  pinMode(9, OUTPUT);   // initiates Brake Channel A pin


  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING); // read encoder values everytime ENCA rises

}

void loop() {

  PID_Controller(2.5);

}

void readEncoder() {                // this function is called whenever ENCA rises
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

void PID_Controller(float num_revs) {
  // target position obtained from parameter (in rev)

  // set target position (in pulses)
  int target = num_revs * ENC_CPR;                // (+) -> CW, (-) -> CCW

  // PID constants
  float kp, kd, ki;

  if (fabs(num_revs) >= 0 && fabs(num_revs) < 6) {
    kp = 4.8;
    kd = 0.02;
    ki = 0.038;
    // this parameters have 6/1024 or less for [1, 3.75]
  }

  // time difference
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

  // print target and measured positions to test algorithm
  Serial.print(target);
  Serial.print(" ");
  Serial.print(pos);
  Serial.println();
  
}
