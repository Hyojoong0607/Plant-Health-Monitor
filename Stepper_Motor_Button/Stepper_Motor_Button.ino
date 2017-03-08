#include <Stepper.h>
#include "Wire.h"

//set steps and the connection with MCU
Stepper stepper(100, 2, 3, 4, 5);

// set pin numbers:
#define buttonPinA 8 // the number of the pushbutton pin
#define buttonPinB 9
#define buttonPinC A5
#define buttonPinD A5

// variables will change:
//here int means integer variable.  [.... -5, -4 ... 0 ... 4, 5 ...]
int buttonStateA = 0;
int buttonStateB = 0;  // variable for reading the pushbutton status
int smallLeft = -25;
int largeLeft = -100;
int smallRight = 25;
int largeRight = 100;
int previous = 0;
  int val = 0;

boolean S1_clicked = false;
boolean S2_clicked = false;
boolean S3_clicked = false;
boolean S4_clicked = false;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinA, INPUT);
  pinMode(buttonPinB, INPUT);
  pinMode(buttonPinC, INPUT);
  //pinMode(A0, OUTPUT);
  
  //speed of 180 per minute
  stepper.setSpeed(180);
}

void loop() {

  buttonStateA = digitalRead(buttonPinA);
  buttonStateB = digitalRead(buttonPinB);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
 /* if (buttonStateA == HIGH) {
    // turn left:
    Serial.println("button 9 pressed");
    val = -500;
  }
  if (buttonStateB == HIGH) {
    Serial.println("button 8 pressed");
    val = smallLeft;
  }
*/
  S1_clicked = !digitalRead(buttonPinA);
  if(S1_clicked == true){
    //writehigh
    val = val + largeLeft;
  }
  
  S2_clicked = !digitalRead(buttonPinB);
  if(S2_clicked == true){
    //writehigh
    val = val + smallLeft;
    }

 
  if(analogRead(buttonPinC) > 100 && analogRead(buttonPinC) < 300){
    S3_clicked = true;
      val = val + smallRight;
  }
  if(analogRead(buttonPinD) > 450 && analogRead(buttonPinD) < 550){
    S4_clicked = true;
      val = val + largeRight;
  }

  
  //current reading minus the reading of history
  stepper.step(val - previous);

  //store as prevous value
  previous = val;
}
