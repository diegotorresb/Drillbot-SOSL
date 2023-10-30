#include<AccelStepper.h>
AccelStepper stepper(1,9,8);
AccelStepper stepper2(1,7,6);

const byte numChars = 32;
char receivedChars[numChars];
char confirmTxt;
int newNum;
bool charRead = true;
bool stepTime = false;
bool gtg;

int xData[] = {1000, 2000};
int yData[] = { 1000, 2000};/// example, code runs to (1000,1000) then (1000,2000) then (2000,1000), then (2000,2000)
// No Matlab required. Above tables are the positions you want the iterate through. It returns to zero, zero at the end. To start the program, open serial monitor and enter "g"
void setup() {
    stepper.setMaxSpeed(400);
    stepper.setAcceleration(40);
    stepper2.setMaxSpeed(400);
    stepper2.setAcceleration(40);
    Serial.begin(9600);
    Serial.println("Arduino is ready, enter g to start");
}

void loop() {
    recvStep();
    runStepper();
    returnToZero(); 
    while (gtg = true){// traps the code after it returns to zero
    }
}

void recvStep() {// initiates the code to iterate through the table
    while (Serial.available() == 0){
    }
    while (Serial.available() > 0 && charRead == true){
      confirmTxt = Serial.read();
      if (confirmTxt == 'g'){
        charRead = false;
        stepTime = true;
      }
    }
}

void runStepper(){
  if (stepTime == true){
    stepTime = false;
    for (int i = 0; i < 2; i++){   // change depending on size of tables, for a 3x3, you must have i = j = 3
      for (int j = 0; j < 2; j++){// not sure about non square tables
        gtg = false;
        while (gtg == false){
          stepper.moveTo(xData[j]);
          stepper.run();
          stepper2.moveTo(yData[i]);
          stepper2.run();
          if (stepper.distanceToGo() == 0 && stepper2.distanceToGo() == 0){
            gtg = true;
          }
        }
      }
    }
  }
}

void returnToZero(){
  stepper.moveTo(0);
  stepper.runToPosition();//run to position stops code until it gets to position
  stepper2.moveTo(0);
  stepper2.runToPosition();
}
