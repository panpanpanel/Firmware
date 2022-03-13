#include "Arduino.h"
#include <ESP32Servo.h>

extern Servo servoA;
extern int servoAPin;
extern int servoStateA;

extern Servo servoB;
extern int servoBPin;
extern int servoStateB;

void setupServos ();

void moveServoA (int angle);

void moveServoB (int angle);

void flipBreakerA ();

void flipBreakerB ();
