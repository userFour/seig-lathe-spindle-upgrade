//--------------------------------------------------------------------------------
// Minilathe Firmware
//
//
// Author: Markos         Date: 2023-09-26
//
//--------------------------------------------------------------------------------

#include <ODriveArduino.h>
#include <SoftwareSerial.h>

#define LED 13
#define potPin A1

// Declare mah functions
void blip();
void arming();

// Global Variables
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;
uint16_t interval = 30;  // Period of execution
uint16_t potRead = 0;

// LED blip vars
bool LEDState = false;
uint16_t LEDInterval = 0;  // Period of execution, changes depending on LEDState
uint32_t LEDPreviousMillis = 0;
uint32_t tOn = 1000;  // LED on time
uint32_t tOff = 50;   // LED off time


// --------------------------------------------------------------------------------
// SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP
// --------------------------------------------------------------------------------
void setup() {

  // Setup pins
  pinMode(LED, OUTPUT);
  pinMode(potPin, INPUT);

  // Create objects
  SoftwareSerial odrive_serial(8, 9);  // RX (ODrive TX), TX (ODrive RX)
  ODriveArduino odrive(odrive_serial);

  // Serial Setup
  odrive_serial.begin(19200);  // setup odrive serial

  Serial.begin(9600);  // setup computer serial
  Serial.print(" // -------------------------------------------------------------------------------- \n");
  Serial.print(" TITLE THING \n");
  Serial.print(" // -------------------------------------------------------------------------------- \n\n");

  Serial.println(" Waiting for arming sequence");
  arming();
  Serial.println(" Armed!");

  // Connect to ODrive
  Serial.println(" Waiting for ODrive...");
  while (odrive.getState() == AXIS_STATE_UNDEFINED) {

    delay(100);
  }
  Serial.print(" DC voltage: ");
  Serial.println(odrive.getParameterAsFloat("vbus_voltage"));
  Serial.print(" Motor current: ");
  Serial.println(odrive.getParameterAsFloat("motor.current_control.Iq_measured"));

  // Enable the ODrive
  Serial.println(" Enabling closed loop control...");
  while (odrive.getState() != AXIS_STATE_CLOSED_LOOP_CONTROL) {

    odrive.clearErrors();
    odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);
    delay(10);
  }

  Serial.println(" ODrive running!");
}


// --------------------------------------------------------------------------------
// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
// --------------------------------------------------------------------------------
void loop() {

  currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;  // Keep track of current time

    blip();
    digitalWrite(LED, LEDState);

    // Serial.println(analogRead(potPin));
  }
}

//------------------------------------------------------------------------------
void blip() {

  currentMillis = millis();

  if ((currentMillis - LEDPreviousMillis) >= LEDInterval) {

    LEDPreviousMillis = millis();

    if (LEDState) {
      LEDInterval = tOff;
      LEDState = !LEDState;

    } else {
      LEDInterval = tOn;
      LEDState = !LEDState;
    }

    digitalWrite(LED, LEDState);
  }
}

//------------------------------------------------------------------------------
void arming() {

  // Arming sequence
  bool armFlag = false;
  while (!armFlag) {
    if (analogRead(potPin) < 10) {
      armFlag = true;
      Serial.println(" low OK");
    }
  }

  armFlag = false;
  while (!armFlag) {
    if (analogRead(potPin) > 1000) {
      armFlag = true;
      Serial.println(" high OK");
    }
  }

  armFlag = false;
  while (!armFlag) {
    if (analogRead(potPin) < 10) {
      armFlag = true;
      Serial.println(" low OK");
    }
  }
}
