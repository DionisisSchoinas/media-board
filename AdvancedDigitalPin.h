// AdvancedDigitalPin.h
#ifndef AdvancedDigitalPin_h
#define AdvancedDigitalPin_h

#include "Arduino.h"

class AdvancedDigitalPin {
  private:
    int digitalPin;
    int state;
    unsigned long lastStateChangeTime;
    int minStateHoldTime;
    void updateLastStateChangeTime();

  public:
    AdvancedDigitalPin(int digitalPin, int pinMode, int minStateHoldTime);
    AdvancedDigitalPin(int digitalPin, int pinMode);
    AdvancedDigitalPin(int minStateHoldTime);
    bool hasTimePassed();
    bool hasStateChangedTooFast();
    void setState();
    void setState(int newState);
    int getState();
    int getCurrentPinState();
    int getDigitalPin();
    void print();
};

#endif