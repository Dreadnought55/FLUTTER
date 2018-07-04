//2018.04.16.1631

// Bluetooth C Master (card).

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

// Remainder of pin declarations.
const int sensorPin = 2; //vibration sensor, digital
const int ledPin = 13; //LED for diagnostics

int reading = 0; //stores vibration input

// Variable to store Slave's state (DEFAULT/REUNION)
int state = 1;


void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400);
  
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Checks for input from Slave (REUNION?).
  if (BTSerial.available() > 0) {
    state = BTSerial.read();
  }
  
  // If no REUNION, checks for vibration & outputs it.
  if (state == 1) {
    reading = digitalRead(sensorPin);
    Serial.println(reading);
    BTSerial.write(reading); //sends to Slave
  }
  digitalWrite(ledPin, reading); //for diagnostics
  delay(200);
}

