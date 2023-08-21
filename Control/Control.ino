// For Arduino Nano remote control with joystick
// gets input of joysticks and sends it to arduino uno to power motors

// Joystick output for x,y direction with ~ 512,512 being origin (neutral joystick)
const float X_pin = A0;
const float Y_pin = A1;

// Transmitter setup
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[]= "00001";

void setup() {
  radio.begin();
  radio.openWritingPipe(address); // write over the x, y value of the joystick position
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  // TODO is this the best way to do it
  // send in 1 message and parse into x and y value in the reciever side?
  const char pos = analogRead(X_pin) + "," + analogRead(Y_pin);
  radio.write(&pos, sizeof(pos));
  ///Serial.println(pos);
  delay(1000);
}
