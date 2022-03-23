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
int mux_select_0 = 1; // pin 25
int mux_select_1 = 1; // pin 26
int mux_select_2 = 0; // pin 32
int mux_select_3 = 0; // pin 33
int flipList[] = {0,0,0,0,0,0,0,0};
int flipListSize = 0;
const int clearCurrentPin = 27;

int buttonPin = 0, lastButtonState = 0, buttonState = 0;

void messageHandler(String &topic, String &payload) {
  //Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  int breaker = int(doc["breaker"]);

  flipList[flipListSize] = breaker;
  flipListSize++;

  //Serial.println(breaker);
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
    return 25;
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
  //delay(5000);
  Serial.begin(115200);

  Serial.println ("1");

  pinMode(clearCurrentPin, OUTPUT);
  digitalWrite(clearCurrentPin, LOW);

  Serial.println ("2");

  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(33, OUTPUT);

  Serial.println ("3");

  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);

  Serial.println ("4");

  startSPI();

  Serial.println ("5");
  
  connectToCloud();

  setupADC();

  Serial.println ("6");

  setupServos();

  Serial.println ("7");

  setupStepper();

  Serial.println ("8");

  homeStepper();
  
  //setupPhaseDifferenceMeasurement ();
}

void loop() {
  int stepperPosition = getActualStepperPosition();
  //Serial.print ("Actual Stepper Position: ");
  //Serial.println (getActualStepperPosition());
  //delay(500);

  // spaces between breakers are 5 cm

  if (flipListSize > 0) {
    setTargetStepperPosition(flipListSize);
    Serial.print ("Actual Stepper Position: ");
    Serial.println (stepperPosition);
    //flipBreakerA();
    //flipListSize--;
    
    //Serial.print ("test ");
    //Serial.println (getBreakerPositionInCm(flipList[0]));
    //setTargetStepperPosition(getBreakerPositionInCm(flipList[0]));
    //if (getActualStepperPosition() == getBreakerPositionInCm(flipList[0])) {
    if (stepperPosition == flipListSize) {
      flipBreakerA();
      delay (500);
      flipBreakerA();
      delay (500);
      //adjustFlipList(); // shift elements in flip list to the left
      //flipListSize--;
    }
  }
  else {
    //byte WRITE_ZERO_ACTUAL_POSITION[4] = {0b10, 0, 0, 0};
    //dataTransfer (WRITE_ZERO_ACTUAL_POSITION);
    //Serial.print ("not flip ");
    //Serial.println (getActualStepperPosition());
  }

  if (millis() > 13000) {
    if (flipListSize == 0){
      flipListSize = 10;
    }
    else if (flipListSize == 10 && stepperPosition == 10) {
      flipListSize = 60;
    }
    else if (flipListSize == 60 && stepperPosition == 60) {
      flipListSize = 0;
    }
  }
  
  digitalWrite(clearCurrentPin, HIGH);
  delay(100);
  digitalWrite(clearCurrentPin, LOW);
  delay(1000);

  float breaker1Current = readADC()*1.75;
  Serial.print ("ADC Reading: ");
  Serial.println (breaker1Current);
  
  //float currents[8] = {1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5};
  //currents[0] = breaker1Current;
  //publishMessage(1, 120, currents);

  //client.loop();
  //delay(50);
  
  
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
