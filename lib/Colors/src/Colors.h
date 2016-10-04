#ifndef Colors_h
#define Colors_h

#include "Arduino.h"

class Colors
{
  public:
    Colors(int, String, int);
    static const int MAX_VALUE;
    static const int MIN_VALUE;
    static const uint8_t PROGMEM GAMMA[];
    void setValue(float);
    void wantValue(int);
    String getColor();
    int getValue();
    int getWantedValue();
    bool isMigrating();
    bool isPulseDone();
    void tick(bool);
  private:
    void writeValue(int, bool);
    void migrate();
    String    _color;
    int       _migrationFrom;
    int       _migrationShiftCount;
    int       _migrationMaxShiftCount;
    unsigned  _migrationTickCount;
    unsigned  _migrationDelay;
    float     _currentValue;
    int       _pin;
    unsigned  _step;
    int       _wantedValue;
    bool      _isMigrating;
    // pulse
    void      pulse();
    bool      _isPulsing;
    float     _pulseValue;
    unsigned  _pulseStep;
    bool      _isPulseRaise;
    bool      _isPulseDone;
    unsigned  _pulseDelay;
    unsigned  _pulseTickCount;
    unsigned  _pulseRange;
};

#endif
