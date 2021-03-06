#include "Arduino.h"
#include "Colors.h"

const int Colors::MIN_VALUE = 0;
const int Colors::MAX_VALUE = 255;
const uint8_t PROGMEM Colors::GAMMA[] = {
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

Colors::Colors(int pin, String color, int step)
{
  pinMode(pin, OUTPUT);
  _color = color;
  _pin = pin;
  _step = unsigned(step);
  _migrationFrom = 0;
  _currentValue = MIN_VALUE;
  _wantedValue = _currentValue;
  _migrationShiftCount = 0;
  _migrationMaxShiftCount = 100;
  _migrationTickCount = 0;
  _migrationDelay = 30;

  _isMigrating = false;
  _isPulseDone = true;
  _pulseValue = 0;
  _pulseStep = 1;
  _pulseRange = 70;
  // _pulseDelay = 10;
  _pulseDelay = 200;

}

void Colors::writeValue(int value, bool useGamma = true) {
  // Serial.print(_color);Serial.print(" write value: ");Serial.println(value);

  int newValue = value;
  if (useGamma) {
    newValue = pgm_read_byte(&GAMMA[value]);
  }
  // Serial.print(getColor());Serial.print(" write new value: ");Serial.println(newValue);
  analogWrite(_pin, newValue);
}

void Colors::setValue(float value) {
  writeValue(value);
  _currentValue = value;
}

String Colors::getColor() {
  return _color;
}

int Colors::getValue() {
  return _currentValue;
}

int Colors::getWantedValue() {
  return _wantedValue;
}

void Colors::wantValue(int value) {
  // Serial.print("Whant :");Serial.println(value);
  _migrationFrom = _currentValue;
  _migrationShiftCount = 0;
  _isMigrating = true;
  _wantedValue = value;
}

void Colors::migrate() {
  if (_wantedValue == _currentValue) {
    _isMigrating = false;
    return;
  }

  if (_migrationTickCount == 0 || _migrationTickCount % _migrationDelay != 0) {
    _migrationTickCount++;
    return;
  }
  _migrationTickCount = 0;
  _migrationShiftCount++;

  float newValue;
  if (_migrationShiftCount == _migrationMaxShiftCount) {
    newValue = getWantedValue();
  } else {
    newValue = abs(_currentValue - (float)(_migrationFrom - _wantedValue) / _migrationMaxShiftCount);
  }
  // Serial.print(getColor());Serial.print(" wanted ");Serial.println(getWantedValue());
  // Serial.print(getColor());Serial.print(" shift count ");Serial.println(_migrationShiftCount);
  // Serial.print(getColor());Serial.print(" diff from-wanted ");Serial.println((float)(_migrationFrom - _wantedValue));
  // Serial.print(getColor());Serial.print(" migrate ");Serial.println(newValue);

  setValue(newValue);
}

bool Colors::isMigrating() {
  return _isMigrating;
}

bool Colors::isPulseDone() {
  return _isPulseDone;
}

void Colors::pulse() {
  // Serial.print(getColor());Serial.print("_value_");Serial.println(getValue());
  // Serial.print(getColor());Serial.print("_wantedValue_");Serial.println(getWantedValue());

  if (_currentValue == MIN_VALUE || _wantedValue != _currentValue) {
    return;
  }
  // Serial.print(getColor());Serial.print("_pulseTickCount");Serial.println(_pulseTickCount);

  if (_pulseTickCount == 0 || _pulseTickCount % _pulseDelay != 0) {
    _pulseTickCount++;
    return;
  }
  // Serial.print("Tick count ");Serial.print(getColor());Serial.print(" ");Serial.println(_pulseTickCount);
  _pulseTickCount = 0;
  float lowValue = _currentValue * _pulseRange / 100;
  float higthValue = _currentValue;

  // Serial.print(getColor());Serial.print(" lowValue: ");Serial.println(lowValue);
  // Serial.print(getColor());Serial.print(" higthValue: ");Serial.println(higthValue);
  _pulseValue = _pulseValue + ((higthValue - lowValue)/100 * (_isPulseRaise ? 1 : -1));
  // Serial.print("Pulse value: ");Serial.println(_pulseValue);
  // Serial.print("_isPulseRaise: ");Serial.println(_isPulseRaise ? "true" : "false");

  float newValue = getValue() + _pulseValue;
  int currentPulseIteration = (higthValue - newValue) / ((higthValue - lowValue) / 100);
  // Serial.print(getColor());Serial.print(" currentPulseIteration: ");Serial.println(currentPulseIteration);

  // Serial.print(getColor());Serial.print(" newValue: ");Serial.println(newValue);

  writeValue(newValue);
  _isPulseDone = false;
  if (currentPulseIteration == 100 || currentPulseIteration == 0) {
    _isPulseRaise = !_isPulseRaise;
    _isPulseDone = true;
    // Serial.print("Change pulse raise to: ");Serial.println(_isPulseRaise ? "true" : "false");
  }
}

void Colors::tick(bool enablePulse = false) {
  migrate();

  // Serial.print(getColor());Serial.print(" migrating ");Serial.println(isMigrating() ? 1 : 0);
  // Serial.print(getColor());Serial.print(" enable pulse ");Serial.println(enablePulse ? 1 : 0);
  if(enablePulse && !isMigrating()) {
    // Serial.println("Led pulse!!! ");
    // pulse();
  }
}
