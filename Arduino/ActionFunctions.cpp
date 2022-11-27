#include "ActionFunctions.h"
#include "Utils.h"

bool doDrive(float driveStartTime, float driveDuration, int direction, int drivePin1, int drivePin2) {
    int highPin;
    int lowPin;
    if (direction == 1) {highPin = drivePin1; lowPin = drivePin2;}
    else if (direction == -1) {highPin = drivePin2; lowPin = drivePin1;}
    // drive if within the time range
    if (millis() - driveStartTime <= driveDuration*1000) {analogWrite(highPin, 250);analogWrite(lowPin, 0);}
    else {analogWrite(drivePin1, LOW);analogWrite(drivePin2, LOW);}
}

// Ask the arducam to take a picture
bool doCameraCapture(bool capturePhoto, ArduCAM camera) {
  camera.flush_fifo();
  camera.clear_fifo_flag();
  camera.start_capture();
  return true;
}

bool doCameraSend(bool _capturedPhoto, bool sendPhoto, ArduCAM camera) {
  if (_capturedPhoto && sendPhoto) {
    readFifoBurst(camera);
    camera.clear_fifo_flag();
    return true;
  }
  return false;
}