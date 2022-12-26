// AdvancedAnalogPin.h
#ifndef AdvancedAnalogPin_h
#define AdvancedAnalogPin_h

#include "Arduino.h"

class AdvancedAnalogPin {
  private:
    int analogPin;
    unsigned long lastReadTime;
    int minReadTime;

  public:
    AdvancedAnalogPin(int analogPin, int analogPinMode, int minReadTime);
    void updateLastReadTime();
    bool hasTimePassed();
    int getPinValue();
    void print();
};

#endif