#include "Arduino.h"
#include <SPI.h>

void setupStepper();

void homeStepper();

void dataTransfer (byte info[4]);

void setTargetStepperPosition (int pos);

int getActualStepperPosition ();

void getStepperInterruptFlags (byte readInterrupt[4]);

void printStepperData (byte stepperData[4]);
