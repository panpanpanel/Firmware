#include "BreakerServos.h"

Servo servoA;
int servoAPin = 14; // EDIT THIS?
bool servoAState = 0;

Servo servoB;
int servoBPin = 15; // EDIT THIS?
bool servoBState = 0;

void setupServos () {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servoA.setPeriodHertz(50);
  servoA.attach(servoAPin, 500, 2500);
  servoB.setPeriodHertz(50);
  servoB.attach(servoAPin, 500, 2500);
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
  moveServoA (servoStateA ? 0 : 180); // flip on or off based on current state
  delay (1000); // wait for operation to complete
  moveServoA (90); // return to middle position
}

void flipBreakerB () {
  moveServoB (servoState ? 0 : 180);
  delay (1000);
  moveServoB (90);
}
