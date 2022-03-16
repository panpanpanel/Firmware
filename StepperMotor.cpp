/*
PULSE_DIV = 8

VMAX = 2047
VMIN = 330

RAMP_DIV = 7
AMAX = 2047

PMUL = 255
PDIV = 0

INTERRUPT_MASK = 00001000

ref_conf = 0x14000300 // NOW 0x14000000
*/
#include "StepperMotor.h"

// KEEP IN MIND THAT THESE VALUES ARE ONLY ABLE TO BE USED ONCE CURRENTLY BECAUSE THEY ARE OVERWRITTEN WHEN THEY ARE SENT TO THE STEPPER DRIVER
const int STEPPER_CS = 2;
byte WRITE_CONFIG[4] = {0b01101000, 0, 0, 0b00100000};
byte WRITE_MIN_VELOCITY[4] = {0b00000100, 0, 1, 0b01001010};
byte WRITE_MAX_VELOCITY[4] = {0b00000110, 0, 0b00000111, 0b11111111};
byte WRITE_DIVS[4] = {0b00011000, 0, 0x87, 0};
byte WRITE_MAX_ACCEL[4] = {0b00001100, 0, 0b00000111, 0b11111111};
byte WRITE_PMUL_PDIV[4] = {0b00010010, 0, 255, 0};
byte WRITE_INTERRUPT_MASK[4] = {0b00010110, 0, 8, 0};
byte WRITE_CONF[4] = {0x14, 0, 0, 0};
byte DUMMY_WRITE_LATCH[4] = {0b11100, 0, 0, 0};
byte WRITE_MAX_ACTUAL_POSITION[4] = {0b10, 0, 0b11111111, 0b11111111};
byte WRITE_MIN_TARGET_POSITION[4] = {0, 0, 0, 0};
byte WRITE_ZERO_ACTUAL_POSITION[4] = {0b10, 0, 0, 0};

void fillArray(byte arrayToFill[4], byte byte0, byte byte1, byte byte2, byte byte3) {
  arrayToFill[0] = byte0;
  arrayToFill[1] = byte1;
  arrayToFill[2] = byte2;
  arrayToFill[3] = byte3;
}

void dataTransfer (byte info[4]) {
  digitalWrite(STEPPER_CS, LOW);
  SPI.transfer(info, 4);
  digitalWrite(STEPPER_CS, HIGH);
  delay (1);
}

void setupStepper () {
  pinMode (STEPPER_CS, OUTPUT);
  dataTransfer(WRITE_CONFIG);
  dataTransfer(WRITE_MIN_VELOCITY);
  dataTransfer(WRITE_MAX_VELOCITY);
  dataTransfer(WRITE_DIVS);
  dataTransfer(WRITE_MAX_ACCEL);
  dataTransfer(WRITE_PMUL_PDIV);
  dataTransfer(WRITE_INTERRUPT_MASK);
  dataTransfer(WRITE_CONF);
}

void homeStepper() {
  //dataTransfer(DUMMY_WRITE_LATCH);
  dataTransfer(WRITE_MAX_ACTUAL_POSITION);
  dataTransfer(WRITE_MIN_TARGET_POSITION);
  byte interruptFlags[4];
  bool limitReached = false;
  while (!limitReached) {
    getStepperInterruptFlags(interruptFlags);
    limitReached = (interruptFlags[0] & 0b00000010) >> 1;
  }
  dataTransfer(WRITE_ZERO_ACTUAL_POSITION);
}

void setTargetStepperPosition (int pos) {
  byte c = (pos & 0xFF);
  byte b = ((pos >> 8) & 0xFF);
  byte a = ((pos >> 16) & 0xFF);
  byte write_x_target[4] = {0, a, b, c};
  dataTransfer(write_x_target);
}

int getActualStepperPosition () {
  byte read_x_actual[4] = {3, 0, 0, 0};
  dataTransfer(read_x_actual);
  int pos = (int) read_x_actual[1] << 16;
  pos |= (int) read_x_actual[2] << 8;
  pos |= (int) read_x_actual[3];
  return pos;
}

void getStepperInterruptFlags (byte readInterrupt[4]) {
  fillArray (readInterrupt, 0b00010111, 0, 0, 0);
  dataTransfer(readInterrupt);
}

void printStepperData (byte stepperData[4]) {
  for (int i = 0 ; i < 4 ; i++){
    Serial.print(stepperData[i], BIN);
    Serial.print(" ");
  }
  Serial.println();
}
