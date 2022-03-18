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
int mux_select_0 = 1;
int mux_select_1 = 0;
int mux_select_2 = 1;
int mux_select_3 = 0;
int flipList[] = {0,0,0,0,0,0,0,0};
int flipListSize = 0;

int buttonPin = 0, lastButtonState = 0, buttonState = 0;

void messageHandler(String &topic, String &payload) {
  //Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  int breaker = int(doc["breaker"]);

  flipList[flipListSize] = breaker;
  flipListSize++;

  Serial.println(breaker);
}

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

void startSPI() {
  SPI.begin(16,4,0,2);
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE3));
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

int getBreakerPositionInCm (int breaker) {
  if (breaker == 1) {
    return 1;
  }
  if (breaker == 2) {
    return 2;
  }
  if (breaker == 3) {
    return 3;
  }
}

void adjustFlipList () {
  for (int i = 0 ; i < flipListSize - 1 ; i++) {
    flipList[i] = flipList[i+1];
  }
}

void setup() {
  Serial.begin(115200);

  startSPI();
  
  connectToCloud();

  setupADC();

  setupServos();

  setupStepper();
  homeStepper();
  
  //setupPhaseDifferenceMeasurement ();
}

void loop() {
  if (flipListSize > 0) {
    setTargetStepperPosition(getBreakerPositionInCm(flipList[0]));
    if (getActualStepperPosition() == getBreakerPositionInCm(flipList[0])) {
      flipBreakerA();
      adjustFlipList(); // shift elements in flip list to the left
      flipListSize--;
    }
  }

  float breaker1Current = readADC();
  Serial.println (breaker1Current);
  
  int currents[8] = {0,0,0,0,0,0,0,0};
  currents[0] = breaker1Current;
  publishMessage(1, 120, currents);

  client.loop();
  delay(50);
  
  /*
  if (Serial.available() > 0) {
    // read the incoming byte:
    int dataIn = Serial.parseInt();
    if (dataIn > 0) {
      setTargetStepperPosition(dataIn);
    }
  }*/
  
  /*Phase difference measurement
  if (done_phase) {
    total_diff = getPhaseDifference();
  }
  */
}
