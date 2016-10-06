#include "Arduino.h"
#include "Colors.h"

const unsigned int ledPin = 8;
const unsigned int buttonPin = 4;
const unsigned int motionSensorPin = 2;
const unsigned int ligthSensorPin = A0;
const unsigned int redLedPin = 10;
const unsigned int greenLedPin = 11;
const unsigned int blueLedPin = 9;

const unsigned int glowSec = 60;
// const unsigned int glowSec = 40;
const unsigned int forceGlowSec = 600;
const unsigned int lightThresholdValue = 20;
const unsigned int delaySec = 1;
// const unsigned int delaySec = 0.1;
const uint8_t PROGMEM GAMMA[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

unsigned int turnOffTime = 0;

String RED_COLOR = "red";
String GREEN_COLOR = "green";
String BLUE_COLOR = "blue";

Colors RedColor(redLedPin, RED_COLOR, 20);
Colors GreenColor(greenLedPin, GREEN_COLOR, 20);
Colors BlueColor(blueLedPin, BLUE_COLOR, 20);

bool isLigthOn = false;

void writeLedValue(int pin, int value, bool useGamma = true) {
  // Serial.print(_color);Serial.print(" write value: ");Serial.println(value);
  int newValue = value;
  if (useGamma) {
    newValue = pgm_read_byte(&GAMMA[value]);
  }
  Serial.print(pin);Serial.print(" write new value: ");Serial.println(newValue);
  analogWrite(pin, newValue);
}

unsigned int now() {
  return round(millis() / 1000);
}

unsigned int getRemainGlowTime() {
  return isLigthOn ? turnOffTime - now() : 0;
}

bool isTimeToTurnOff() {
  return turnOffTime <= now();
}

void turnStripOn() {
  // 1500k
  // writeLedValue(redLedPin, 255);
  // writeLedValue(greenLedPin, 108);
  // writeLedValue(blueLedPin, 0);
  // 2000k
  // writeLedValue(redLedPin, 255);
  // writeLedValue(greenLedPin, 137);
  // writeLedValue(blueLedPin, 14);
  // 2700k
  // writeLedValue(redLedPin, 255);
  // writeLedValue(greenLedPin, 167);
  // writeLedValue(blueLedPin, 87);
  RedColor.wantValue(255);
  GreenColor.wantValue(167);
  BlueColor.wantValue(87);
  // 3000k
  // writeLedValue(redLedPin, 255);
  // writeLedValue(greenLedPin, 177);
  // writeLedValue(blueLedPin, 110);
  // 4500k
  // writeLedValue(redLedPin, 255);
  // writeLedValue(greenLedPin, 218);
  // writeLedValue(blueLedPin, 187);
  // 6000k
  // writeLedValue(redLedPin, 255);
  // writeLedValue(greenLedPin, 246);
  // writeLedValue(blueLedPin, 237);
}

void turnStripOff() {
  // analogWrite(redLedPin, 0);
  // analogWrite(greenLedPin, 0);
  // analogWrite(blueLedPin, 0);
  // writeLedValue(redLedPin, 0);
  // writeLedValue(greenLedPin, 0);
  // writeLedValue(blueLedPin, 0);
  RedColor.wantValue(1);
  GreenColor.wantValue(1);
  BlueColor.wantValue(1);
}

void turnLigthOff() {
  if (isLigthOn) {
    digitalWrite(ledPin, LOW);
    turnStripOff();
    // Serial.println("Turn ligth off");
    isLigthOn = false;
  } else {
    // Serial.println("Ligth already turned off");
  }
}

void turnLigthOn() {
  if (!isLigthOn) {
    digitalWrite(ledPin, HIGH);
    turnStripOn();
    // Serial.println("Turn ligth on");
    isLigthOn = true;
  } else {
    // Serial.println("Ligth already turned on");
  }
}

bool setTurnOffTime(unsigned int sec) {
  // Serial.print("Remaining time: ");Serial.println(getRemainGlowTime());
  if (getRemainGlowTime() < sec) {
    turnOffTime = now() + sec;
    return true;
  }
  return false;
}

void updateTurnOffTime(){
  setTurnOffTime(glowSec);
}

void forceTurnLigthOn() {
  setTurnOffTime(forceGlowSec);
  turnLigthOn();
}

void tick() {
  // if (RedColor.isMigrating() || GreenColor.isMigrating() || BlueColor.isMigrating()) {
    // Serial.println("Tick!");
    RedColor.tick(false);
    GreenColor.tick(false);
    BlueColor.tick(false);
  // }
}

void setup() {
  // Serial.begin(9600);
  pinMode(motionSensorPin, INPUT);
  pinMode(ligthSensorPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}

void loop() {
  unsigned int ligthSensorValue = analogRead(ligthSensorPin);  //Read the analog value
  unsigned int buttonValue = digitalRead(buttonPin);  //Read the analog value
  int motionSensorValue = digitalRead(motionSensorPin);  //Read the analog value
  // Serial.print("Millis: ");Serial.println(millis());
  // Serial.print("Motion sensor value: ");Serial.println(motionSensorValue);
  // Serial.print("Ligth sensor value: ");Serial.println(ligthSensorValue);
  // Serial.print("Button value: ");Serial.println(buttonValue);
  if (buttonValue == HIGH) {
    forceTurnLigthOn();
  } else if (motionSensorValue == HIGH && ligthSensorValue <= lightThresholdValue) {
    updateTurnOffTime();
    turnLigthOn();
  }
  if (isLigthOn) {
    if (isTimeToTurnOff()) {
      turnLigthOff();
    } else {
      // Serial.print("Turn ligth off after: ");Serial.println(getRemainGlowTime());
    }
  }
  tick();
  // delay(delaySec * 1000);
}
