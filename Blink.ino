/*
MOSI: 23
MISO: 19
SCK: 18
SS: 5
*/

#include "CloudConnect.h"
#include "BreakerServos.h"
#include "StepperMotor.h"
#include "CustomADC.h"

int total_diff = 0; //THIS MIGHT NEED TO BE VOLATILE
volatile bool done_phase, done_1 = false;
volatile unsigned long long time1, time2, diff;

int buttonPin = 0, lastButtonState = 0, buttonState = 0;

void IRAM_ATTR isr1() {
  time1 = esp_timer_get_time();
  done_1 = true;
}

void IRAM_ATTR isr2() {
  if (done_1) {
    time2 = esp_timer_get_time();
    diff = time2-time1;
    done_phase = true;
  }
}

void setupButton() {
  pinMode(buttonPin, INPUT);
}

void startSPI() {
  // THESE MIGHT NEED TO BE PLAYED AROUND WITH A LITTLE
  SPI.begin(18,19,23,5);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE1));
}

void setupPhaseDifferenceMeasurement () {
  pinMode(26, INPUT);
  pinMode(27, INPUT);

  attachInterrupt(26, isr1, RISING);
  attachInterrupt(27, isr2, RISING);
}

int getPhaseDifference () { // THIS MIGHT NEED TO BE VOLATILE INT BECAUSE 'DIFF' IS VOLATILE AND IT IS THE ROOT VARIABLE FOR ALL THE CALCULATIONS, SO CASTING ISSUES
  float diff_phase = (float)diff/16667;
  float angle_phase = diff_phase * 360;
  int local_diff = 0;
  if (angle_phase < 90) {
    local_diff = angle_phase;
  }
  else if (angle_phase > 270) {
    local_diff = 360 - angle_phase;
  }
  done_phase = false;
  done_1 = false;
  return local_diff;
}

void setup() {
  //Serial.begin(115200);

  //setupButton();

  //startSPI();
  
  //connectToCloud();

  //setupADC();

  //setupPhaseDifferenceMeasurement ();

  //setupServos();

  //setupStepper();
}

void loop() {
  /*Phase difference measurement
  if (done_phase) {
    total_diff = getPhaseDifference();
  }
  */

  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      flipBreakerA();
      flipBreakerB();
    }
  }
  lastButtonState = buttonState;
  
  //publishMessage();

  //float val = readADC();
}
