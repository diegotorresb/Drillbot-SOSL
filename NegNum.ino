#include<AccelStepper.h>
#include<ezButton.h>
AccelStepper stepper(1,9,8);
AccelStepper stepper2(1,7,6);
ezButton limitSwitch(5);
#define LimitSwitchPin 2

const byte numChars = 32;
char receivedChars[numChars];
char motorName;
int newNum;
bool charRead = true;
bool stepTime = false;
bool newerData;
boolean newData = true;
bool limmy = false;

void setup() { // Black Green Red Blue (A+ A- B+ B-)
    stepper.setMaxSpeed(400);
    stepper.setAcceleration(40);
    stepper2.setMaxSpeed(400);
    stepper2.setAcceleration(40);
    Serial.begin(9600);
    limitSwitch.setDebounceTime(50);
    Serial.println("<Arduino is ready>, Enter number, example: <100> for 100");
}

void loop() {
    recvStep();
    runStepper();
}

void recvStep() { //this function reads the value given through matlab, the start/end marker is how it knows what values are being inputed
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && charRead == true){
      motorName = Serial.read();
      charRead = false;
      newData = false;
    }
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
                charRead = true;
                newNum = atoi(receivedChars); // TEST
                stepTime = true; //TEST
               // TEST newerData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void runStepper(){
  if (motorName == 'x'){
    while (stepTime == true){
      stepper.moveTo(newNum);
      stepper.run();
      limmySwitch();
      if (stepper.distanceToGo() == 0){
        stepTime = false;
      }
    }
  }
  else if (motorName == 'y'){
     while (stepTime == true){
      stepper2.moveTo(newNum);
      stepper2.run();
      limmySwitch();
      if (stepper2.distanceToGo() == 0){
        stepTime = false;
      }
    }
  }
}

void limmySwitch() {
/*  limitSwitch.loop();
  
  if (limitSwitch.getState() == HIGH){
    limmy = true;
    while (limmy == true){
      //find way to make steppers stop and return to zero?
    }
  }*/
  return;
}

void returnToZero(){
  stepper.moveTo(0);
  stepper.runToPosition();
  stepper2.moveTo(0);
  stepper2.runToPosition();
}
