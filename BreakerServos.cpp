#include "BreakerServos.h"

Servo servoA;
int servoAPin = 19;
int servoAState = 0;

Servo servoB;
int servoBPin = 18;
int servoBState = 0;

const int NEUTRAL = 135;
const int FLIP = 110;
const int ON = NEUTRAL - FLIP;
const int OFF = NEUTRAL + FLIP;

void setupServos () {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servoA.setPeriodHertz(50);
  servoA.attach(servoAPin, 500, 2500);
  //servoB.setPeriodHertz(50);
  //servoB.attach(servoBPin, 500, 2500);
}

void moveServoA (int angle) {
  float tempAngle = float(angle);
  servoA.write((int)((tempAngle/270.0)*180.0));
}

void moveServoB (int angle) {
  float tempAngle = float(angle);
  servoB.write((int)((tempAngle/270.0)*180.0));
}

void flipBreakerA () {
  moveServoA (servoAState == ON ? OFF : ON); // flip on or off based on current state
  delay (2000); // wait for operation to complete
  moveServoA (NEUTRAL); // return to middle position
  servoAState = servoAState == ON ? OFF : ON;
}

void flipBreakerB () {
  moveServoB (servoBState == ON ? OFF : ON); // flip on or off based on current state
  delay (2000); // wait for operation to complete
  moveServoB (NEUTRAL); // return to middle position
  servoBState = servoBState == ON ? OFF : ON;
}
