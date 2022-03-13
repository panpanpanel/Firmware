#include "CustomADC.h"

int ADC_CS_Pin = 14;
const float PER_BIT = 3.13/(float)(0x0FFF); // For ADC Measurement

void setupADC() {
  pinMode(ADC_CS_Pin, OUTPUT);
}

float readADC() {
  digitalWrite(ADC_CS_Pin, LOW);
  SPI.transfer(0b00011010); // CH2
  unsigned short secondTransfer = SPI.transfer16(0x0);
  unsigned short thirdTransfer = SPI.transfer16(0x0);
  Serial.print(secondTransfer, BIN);
  Serial.println(thirdTransfer, BIN);
  unsigned short shifted = (secondTransfer >> 2) & (0x0FFF);
  digitalWrite(ADC_CS_Pin, HIGH);
  float val = shifted*PER_BIT;
  return val;
}
