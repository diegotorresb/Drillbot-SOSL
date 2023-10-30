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
    Serial.println("<Arduino is ready>, Enter number, example: <100> for 100"); // ignore this 
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

    while (Serial.available() > 0 && charRead == true){// this loop takes in the stepper motor name, x or y
      motorName = Serial.read();
      charRead = false;
      newData = false;
    }
 
    while (Serial.available() > 0 && newData == false) {//this part takes in the value that the motor will run to, + or -
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
                stepTime = true; //TEST    **it seems like these tests were successful
               // TEST newerData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void runStepper(){//this function runs the stepper  motor, the limit switch function, when active, will stop the movement
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

void limmySwitch() {//this is currently commented out and a bit buggy, but the gist is to have the arduino constantly read 
    /*  limitSwitch.loop();//the state of the limit switch, either open or closed, and when it changes, the code gets stuck
  //in the loop and cannot continue to move the steppers with stepper.run(). There is probably a better way to do this
  if (limitSwitch.getState() == HIGH){
    limmy = true;
    while (limmy == true){//this while loop infinitely traps the code
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
