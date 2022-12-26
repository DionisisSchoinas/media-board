#include "Arduino.h"
// AcceleratedRotary.cpp
#include "AcceleratedRotary.h"

AcceleratedRotary::AcceleratedRotary(int minSpeed, int maxSpeed, int resetDelay) {
  this-> minSpeed = minSpeed;
  this-> maxSpeed = maxSpeed;
  this-> numberOfClicks = 0;
  this-> isMovingClockwise = false;
  this-> lastChangeTime = millis();
  this-> resetDelay = resetDelay;
}

void AcceleratedRotary::addClick(int direction) {
  if (direction != 1 && direction != -1)
    return;

  bool isClockwise = direction == 1;
  if (isClockwise != isMovingClockwise || millis() - lastChangeTime >= resetDelay) {
    numberOfClicks = 0;
    isMovingClockwise = isClockwise;
  }
  numberOfClicks++;
  lastChangeTime = millis();
}

int AcceleratedRotary::getSpeed() {
  return min(max((int)pow((double)numberOfClicks/15.0, 2.0) + 1, minSpeed), maxSpeed);
}

void AcceleratedRotary::print() {
  Serial.print("minSpeed ");
  Serial.print(minSpeed);
  Serial.print(" maxSpeed ");
  Serial.print(maxSpeed);
  Serial.print(" isMovingClockwise ");
  Serial.print(isMovingClockwise);
  Serial.print(" numberOfClicks ");
  Serial.print(numberOfClicks);
  Serial.print(" speed ");
  Serial.println(getSpeed());
}