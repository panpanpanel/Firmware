#include "Arduino.h"
#include <SPI.h>

void setupStepper();

void setTargetStepperPosition (int pos);

int getActualStepperPosition ();

void getStepperInterruptFlags (byte readInterrupt[4]);

void printStepperData (byte stepperData[4]);
