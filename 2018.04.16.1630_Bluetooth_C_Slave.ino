//2018.04.16.1630

// Bluetooth C Slave (floating shelf)

/* This program has two states: DEFAULT & REUNION.
 *  
 * Pressing the button (through inserting & taking out
 * the card) toggles between the two states. The
 * program begins in DEFAULT & should be in DEFAULT
 * most of the time.
 *  
 * In DEFAULT (case 1), the program actuates the servo 
 * motor in response to the input from Master. This is
 * regular operation.
 * 
 * In REUNION (case 2), the card has been inserted into 
 * the slot. At the start of REUNION, the servo &
 * solenoid motors rewind the string in tandem. At the
 * end of REUNION, they unwind the string, preparing
 * the shelf for regular operation in the DEFAULT state.
 */


// Initialization of Bluetooth pins.
#include <SoftwareSerial.h>
const int txPin = 5; //chip's RX pin goes here
const int rxPin = 6; //chip's TX pin goes here
SoftwareSerial BTSerial (rxPin, txPin);

// Initialization of continuous servo.
#include <Servo.h>
const int servoPin = 9;
Servo myServo;

// Remainder of pin declarations.
const int buttonPin = 7; //for reunion check
const int solenoidPinA = 10; //for winding string
const int solenoidPinB = 11; //for winding string
const int ledPin = 13; //LED for diagnostics

// Variable to store Master's live data.
int reading = 0;

// Button state variable declarations.
int buttonState = LOW; //stores input from button
int state = 1; //'1' is DEFAULT, '2' is REUNION state

// Variables for servo turning.
// Continuous servos require time (delay) & velocity.
// 90 is a full stop. 0 is full speed CCW, & 180 CW.
const int shakeVelocity = 180;
const int shakeTime = 550; //ms for 1 rev. at 180 speed
// Variables for reunion (winding string).
const int rewindVelocity = 105; //rewind == retract string
const int unwindVelocity = 75; //unwind == release string
const int rewindTime = 11500; //ms
const int unwindTime = 11500; //ms


void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400);
  
  pinMode(buttonPin, INPUT);
  pinMode(solenoidPinA, OUTPUT);
  pinMode(solenoidPinB, OUTPUT);
  pinMode(ledPin, OUTPUT);
  myServo.attach(servoPin);

  // Ensures motors are all in their resting state.
  digitalWrite(solenoidPinA, LOW);
  digitalWrite(solenoidPinB, LOW);
  myServo.write(90);
}


void loop() {
  // Checks for input from Master (vibration?).
  /* This is at the start so that the program checks
   * for input every time it loops, thus preventing a
   * build-up of stored, unread inputs.
   */
  reading = 0; //resets the previous input.
  if (BTSerial.available() > 0) {
    reading = BTSerial.read();
  }
  // LED lights up if there's an input from Master.
  digitalWrite(ledPin, reading); //for diagnostics
  
  // Checks for button press.
  buttonState = digitalRead(buttonPin);
  
  // If in DEFAULT & vibration was detected, waves.
  if (state == 1) {
    if (reading > 0) {
      myServo.write(shakeVelocity);
      delay(shakeTime);
      myServo.write(90);
    }
    else {
      delay(50);
    }
  }

  // If button was pressed, initiates the appropriate
  // state change.
  switch (state) {
    case 1: //transitions from DEFAULT to REUNION.
      if (buttonState == HIGH) {
        state = 2; //on next button press, will run case 2
        BTSerial.write(state); //sends to Master to stop inputs
        delay(1000); //dramatic pause
        digitalWrite(solenoidPinA, HIGH);
        delay(100);
        myServo.write(rewindVelocity);
        delay(rewindTime);
        myServo.write(90);
      }
      break;
    case 2: //transitions from REUNION to DEFAULT.
      if (buttonState == HIGH) {
        delay(1000); //dramatic pause
        myServo.write(unwindVelocity);
        delay(unwindTime);
        myServo.write(90);
        digitalWrite(solenoidPinA, LOW);
        state = 1; //on next button press, will run case 1
        BTSerial.write(state); //sends to Master to resume inputs
      }
      break;
  }
  delay (10);
}
