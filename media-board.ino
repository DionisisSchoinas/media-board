#include "AdvancedDigitalPin.h"
#include "AdvancedAnalogPin.h"
#include "AcceleratedRotary.h"

#include "HID-Project.h"

/* 
  ==== LEONARDO ====
*/
#define BUTTON_1_PIN 8
#define BUTTON_2_PIN 9
#define BUTTON_2_LED_PIN 4
#define BUTTON_3_PIN 10

#define VOLUME_ENCODER_CLK 7
#define VOLUME_ENCODER_DT  6
#define VOLUME_ENCODER_SW  14

#define ZOOM_ENCODER_CLK 0
#define ZOOM_ENCODER_DT  2
#define ZOOM_ENCODER_SW  3

#define JOYSTICK_X A0
#define JOYSTICK_Y A2
#define JOYSTICK_SW 21 //A3

// Buttons
AdvancedDigitalPin button1(BUTTON_1_PIN, INPUT_PULLUP, 100);
AdvancedDigitalPin button2(BUTTON_2_PIN, INPUT_PULLUP, 100);
AdvancedDigitalPin button3(BUTTON_3_PIN, INPUT_PULLUP, 100);

// Volume Encoder
AdvancedDigitalPin volumeEncoderCLK(VOLUME_ENCODER_CLK, INPUT);
AdvancedDigitalPin volumeEncoderDT(VOLUME_ENCODER_DT, INPUT);
AcceleratedRotary  volumeAccelaration(1, 10, 500);
AdvancedDigitalPin volumeEncoderSW(VOLUME_ENCODER_SW, INPUT_PULLUP, 100);

// Zoom Encoder
AdvancedDigitalPin zoomEncoderCLK(ZOOM_ENCODER_CLK, INPUT);
AdvancedDigitalPin zoomEncoderDT(ZOOM_ENCODER_DT, INPUT);
AdvancedDigitalPin zoomEncoderSW(ZOOM_ENCODER_SW, INPUT_PULLUP, 100);
AdvancedDigitalPin zoomChange(50);

// Joystick
AdvancedDigitalPin joystickSW(JOYSTICK_SW, INPUT_PULLUP, 100);
AdvancedAnalogPin joystick_X(JOYSTICK_X, INPUT, 100);
AdvancedAnalogPin joystick_Y(JOYSTICK_Y, INPUT, 100);

int joystickMappedValues[] = {0, 1, 5, 15};
int repeatDelay = 250;
unsigned long lastRepeat = 0;

void setup() {
  //Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();
  Consumer.begin();
  
  pinMode(BUTTON_2_LED_PIN, INPUT_PULLUP);
  digitalWrite(BUTTON_2_LED_PIN, LOW);
  
  // Volume rotary encoder
  attachInterrupt(digitalPinToInterrupt(VOLUME_ENCODER_CLK), volumeAdjust, CHANGE);
  
  // Zoom rotary encoder
  attachInterrupt(digitalPinToInterrupt(ZOOM_ENCODER_CLK), zoomAdjust, CHANGE);
}

void loop() {
  // Reset every 48 hours, if a reset hasn't happened yet
  if (millis() > 17280000) 
    asm volatile ("jmp 0");
  
  // Left Ctrl + m
  if (isFirstPress(button1)) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('m');
    Keyboard.releaseAll();
  }

  // Left Ctrl + Left Alt + Left Windows Key + m
  if (isFirstPress(button2)) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('m');
    Keyboard.releaseAll();
    // Swap LED state
    digitalWrite(BUTTON_2_LED_PIN, !digitalRead(BUTTON_2_LED_PIN));
  }
  
  
  if (isFirstPress(button3)){
    Consumer.press(MEDIA_PLAY_PAUSE);
    Consumer.release(MEDIA_PLAY_PAUSE);
  }
  
  // Consumer Mute
  if (isFirstPress(volumeEncoderSW)){
    Consumer.press(MEDIA_VOLUME_MUTE);
    Consumer.release(MEDIA_VOLUME_MUTE);
  }
    
  // Ctrl + 0
  if (isFirstPress(zoomEncoderSW)){
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('0');
    Keyboard.releaseAll();
  }
  
/*
  if (isFirstPress(joystickSW)) {
    // Joystick press
  }
  */
  
  detectJoystickMovement();
}

void detectJoystickMovement() {
  int xMov = getMappedJoystickValue(joystick_X.getPinValue());
  int yMov = -getMappedJoystickValue(joystick_Y.getPinValue());

  // Scroll wheel Up/Down  
  if (xMov != 0 && joystick_X.hasTimePassed()) {
    joystick_X.updateLastReadTime();
    Mouse.move(0, 0, xMov);
  }
  
  // Left Shift + Scroll wheel Up/Down 
  if (yMov != 0 && joystick_Y.hasTimePassed()) {
    joystick_Y.updateLastReadTime();
    Keyboard.press(KEY_LEFT_SHIFT);
    Mouse.move(0, 0, yMov);
    Keyboard.releaseAll();
  }
}

void volumeAdjust() {
  int rotation = checkEncoderRotation(volumeEncoderCLK, volumeEncoderDT);
  volumeAccelaration.addClick(rotation);
  switch (rotation) {
    case 1:
      // Consumer Volume Up
      for (int i=0;i<volumeAccelaration.getSpeed(); i++) {
        Consumer.press(MEDIA_VOLUME_UP);
        Consumer.release(MEDIA_VOLUME_UP);
      }
      break;
    case -1:
      // Consumer Volume Down
      for (int i=0;i<volumeAccelaration.getSpeed(); i++) {
        Consumer.press(MEDIA_VOLUME_DOWN);
        Consumer.release(MEDIA_VOLUME_DOWN);
      }\
      break;
  }
}

void zoomAdjust() {
  int rotation = checkEncoderRotation(zoomEncoderCLK, zoomEncoderDT);  
  if (rotation != zoomChange.getState() && !zoomChange.hasTimePassed())
    return;
  zoomChange.setState(rotation);

  switch (rotation) {
    case 1:
      // Left Ctrl + Numpad '+'
      Keyboard.press(KEY_LEFT_CTRL);
      Mouse.move(0, 0, 1);
      Keyboard.releaseAll();
      break;
    case -1:
      // Left Ctrl + Numpad '-'
      Keyboard.press(KEY_LEFT_CTRL);
      Mouse.move(0, 0, -1);
      Keyboard.releaseAll();
      break;
  }
}

int getMappedJoystickValue(int pinValue) {
  if (pinValue < 0 || pinValue > 1023)
    return 0;

  if (pinValue <= 30) // [0,30] = 31
    return -getMappedJoystickValueWithDelay(3);
  if (pinValue <= 100) // (30,100] = 70
    return -getMappedJoystickValueWithDelay(2);
  else if (pinValue <= 250) // (100,250] = 150
    return -getMappedJoystickValueWithDelay(1);
  else if (pinValue <= 490) // (250,490] = 240
    return -getMappedJoystickValueWithDelay(0);
  else if (pinValue <= 530) // (490,530]  20--(511)--20 = 39
    return 0;
  else if (pinValue <= 770) // (530,770] = 240
    return getMappedJoystickValueWithDelay(0);
  else if (pinValue <= 920) // (770,920] = 150
    return getMappedJoystickValueWithDelay(1);
  else if (pinValue <= 990) // (920,990] = 70
    return getMappedJoystickValueWithDelay(2);
  else // (990,1023] = 33
    return getMappedJoystickValueWithDelay(3);  
}

int getMappedJoystickValueWithDelay(int index) {
  if (index > 3 || index < 0) {
    return 0;
  }

  int val = joystickMappedValues[index];

  if (val != 0) {
    return val;
  }
  
  if (lastRepeat <= millis() - repeatDelay) {
    lastRepeat = millis();
    return 1;
  }

  return 0;
}

/*
  Returns TRUE the first time the PIN reads LOW and then locks the pin until a HIGH is read.
  Allows PIN to trigger code ONLY ONCE while held.
  An additional check is made before checking the PIN status. This check is whether or not enough time has passed since the last STATE change
*/
bool isFirstPress(AdvancedDigitalPin &b) {
  if (b.hasStateChangedTooFast())
    return false;
    
  int sensorVal = b.getCurrentPinState();
  if (b.getState() == sensorVal)
    return false;

  if (sensorVal == LOW) {
    b.setState(LOW);
    return true;
  } 
  else if (sensorVal == HIGH) {
    b.setState(HIGH);
  }
  return false;  
}

int checkEncoderRotation(AdvancedDigitalPin &clk, AdvancedDigitalPin &dt) {  
  /*
  Serial.print(clk.getCurrentPinState());
  Serial.print(" == ");
  Serial.print(clk.getState());
  Serial.print(" || ");
  Serial.print(dt.getCurrentPinState());
  Serial.print(" == ");
  Serial.println(dt.getState());
  */
	if (clk.getCurrentPinState() != clk.getState()) { // CLK has changed
    clk.setState();
    if (dt.getCurrentPinState() != dt.getState()) { // DT has changed
      dt.setState();
      if (dt.getState() != clk.getState()) {
        return 1; // Clockwise
      } 
      else {
        return -1; // Counter clockwise
      }
		} 
	}  
  return 0;
}
