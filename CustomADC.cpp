/*
 * CH2: Line voltage
*/
#include "CustomADC.h"

int ADC_CS_Pin = 14;
const float PER_BIT = 3.13/(float)(0x0FFF); // For ADC Measurement

void setupADC() {
  pinMode(ADC_CS_Pin, OUTPUT);
  digitalWrite(ADC_CS_Pin, HIGH);
}

float readADC() {
  digitalWrite(ADC_CS_Pin, LOW);
  SPI.transfer(0b00000110); // CH 3
  unsigned short firstTransfer = SPI.transfer(0b11000000); // CH 3
  unsigned short secondTransfer = SPI.transfer(0);
  uint16_t value = (firstTransfer & 0x0F) << 8 | secondTransfer;
  digitalWrite(ADC_CS_Pin, HIGH);
  float val = value*PER_BIT;
  return val;
}
