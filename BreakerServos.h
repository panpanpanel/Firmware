#include "Arduino.h"
#include <ESP32Servo.h>

extern Servo servoA;
extern int servoAPin;
extern bool servoStateA;

extern Servo servoB;
extern int servoBPin;
extern bool servoStateB;

void setupServos ();

void moveServoA (int angle);

void moveServoB (int angle);

void flipBreakerA (bool left);

void flipBreakerB (bool left);
