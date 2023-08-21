// For Arduino Uno motors and boat components to be connected to a remote
// recieves input of joysticks and controls motors for boat operations

// motor setup
// left motor
#define ENABLEL 5
#define forwardL 3
#define backwardL 4
// right motor
#define ENABLER 10
#define forwardR 9
#define backwardR 2

// Reciever setup
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Globals
const float joy_range = 1024.0; // maximum value for joystick x or y position
const float motor_max = 255;

void setup() {
  // motor
  pinMode(ENABLEL,OUTPUT);
  pinMode(forwardL,OUTPUT);
  pinMode(backwardL,OUTPUT);
  pinMode(ENABLER,OUTPUT);
  pinMode(forwardR,OUTPUT);
  pinMode(backwardR,OUTPUT);
  Serial.begin(9600);

  // reciever
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // connection
  int y_val = 512; // default to neutral joystick position when no connection
  int x_val = 512; 
  if (radio.available()) {
    char message[32] = "";
    radio.read(&message, sizeof(message));
    //Serial.println(message);
    sscanf(message, "%d %d", x_val, y_val); // parse input message to x,y value of joystick location
  }

  // thrust determined by how "up" or "down" the joystick it
  float thrust = 0.0 ;
  float y_percentage = ((float)y_val) / joy_range; // joystick goes 0 to 1024 with 512 being neutral position
  if (y_percentage >= 0.52){ // forward
    float scale_factor = (y_percentage - 0.52) * 2.0; // maps forward movment from 0.52 - 1.0 range to a full 0.0 to 1.0 range
    thrust = motor_max*scale_factor;
  } else if (y_percentage <= 0.48){ // backward
    float scale_factor = ((y_percentage - 0.48) * -1) * 2.0; // maps backwards from 0.48 - 0.0 range to full 0.0 to 1.0 range
    // ^ -1 cuz closer to zero means more force backwards 
    thrust = motor_max*scale_factor;
  }
  float right_thrust = thrust;
  float left_thrust = thrust;
  
  // turn factor, change the amount of power to each motor by scaling it back to turn with joystick
  float straigtness = 1.0; // 1 means joystick completly forward/backward, 0 means completly right or left or neutral
  float x_percentage = ((float)x_val) / joy_range; 
  if (x_percentage >= 0.52){ // right
    straigtness = ((x_percentage - 1)*-1) * 2.0;
    right_thrust = right_thrust * straigtness;
  } else if (x_percentage <= 0.48){ // left
    straigtness = x_percentage * 2.0;
    left_thrust = left_thrust * straigtness;
  }

  // final motor control
  if(y_percentage > 0.48 && y_percentage < 0.52 ){ // if neutral (with wiggle room for noise/drift)
    // no power to motor
    delay(1000); // easy stop
    digitalWrite(ENABLEL,LOW); 
    digitalWrite(ENABLER,LOW);
  } else if (y_percentage >= 0.52){
    // power forward direction
    analogWrite(ENABLEL,(left_thrust)); 
    digitalWrite(forwardL,HIGH);
    digitalWrite(backwardL,LOW);
    analogWrite(ENABLER,right_thrust);
    digitalWrite(forwardR,HIGH);
    digitalWrite(backwardR,LOW);
  } else {
    // power backward direction
    analogWrite(ENABLEL,left_thrust); 
    digitalWrite(forwardL,LOW); 
    digitalWrite(backwardL,HIGH);
    analogWrite(ENABLER,right_thrust); 
    digitalWrite(forwardR,LOW); 
    digitalWrite(backwardR,HIGH);
  }
}
