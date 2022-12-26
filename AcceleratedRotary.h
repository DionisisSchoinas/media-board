// AcceleratedRotary.h
#ifndef AcceleratedRotary_h
#define AcceleratedRotary_h

#include "Arduino.h"

class AcceleratedRotary {
  private:
    bool isMovingClockwise;
    int numberOfClicks;
    unsigned long lastChangeTime;
    int resetDelay;
    int minSpeed;
    int maxSpeed;

  public:
    AcceleratedRotary(int minSpeed, int maxSpeed, int resetDelay);
    void addClick(int direction);
    int getSpeed();
    void print();
};

#endif