#include "ActionFunctions.h"
#include "Utils.h"

bool doDrive(float driveStartTime, float driveDuration, int direction, int drivePin1, int drivePin2) {
    // If we're within the driving window, calculate what voltage to write
    // Serial.println(millis() - driveStartTime <= driveDuration);
    if (millis() - driveStartTime <= driveDuration) {
      // Which pin gets the high voltage
      int highPin = drivePin1;
      int lowPin = drivePin2;
      if (direction == 1) {highPin = drivePin1; lowPin = drivePin2;}
      else if (direction == -1) {highPin = drivePin2; lowPin = drivePin1;}
      // The "high voltage" we place on the motor pin should ramp up/down over 5s to get reasonnable acceleration and prevent stalls
      float time = millis();
      float timeSinceDriveStartTime = max(time - driveStartTime, 0);
      float driveEndTime = driveStartTime + driveDuration;
      float timeToDriveEndTime = max(driveEndTime - time, 0);
      int highVoltageCounts = 0;
      // If we're before half, ramp up with slope 255 counts / 1 second; otherwise ramp down with the same slope
      if (timeSinceDriveStartTime < timeToDriveEndTime) {highVoltageCounts = (int)(255.0/1.0 * timeSinceDriveStartTime / 1000.0);}
      else if (timeSinceDriveStartTime >= timeToDriveEndTime) {highVoltageCounts = (int)(255.0/1.0 * timeToDriveEndTime / 1000.0);}
      // Cap it at 255
      highVoltageCounts = min(highVoltageCounts, 255);
      analogWrite(highPin, 255); 
      analogWrite(lowPin, 0);
    }
    else {
      analogWrite(drivePin1, 0); 
      analogWrite(drivePin2, 0);}
}

// Ask the arducam to take a picture
bool doCameraCapture(bool capturePhoto, ArduCAM camera) {
  camera.flush_fifo();
  camera.clear_fifo_flag();
  camera.start_capture();
  return true;
}

bool doCameraSend(bool sendPhoto, ArduCAM camera) {
  if (camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK) && sendPhoto) {
    readFifoBurst(camera);
    camera.clear_fifo_flag();
    return true;
  }
  return false;
}

bool doTestCameraSPI(bool testSPI, ArduCAM camera) {
  //Check if the ArduCAM SPI bus is OK
  if (testSPI) {
    camera.write_reg(ARDUCHIP_TEST1, 0x55);
    Serial.println(camera.read_reg(ARDUCHIP_TEST1));
    return camera.read_reg(ARDUCHIP_TEST1) == 0x55;
  }
}

bool doRangeFinderReading(float rangeFinderTriggerStartTime, 
                          bool lastRangeFinderPulseState, float rangeFinderPulseStartTime, float rangeFinderPulseEndTime, 
                          bool rangeFinderReadingReady,
                          int RANGEFINDER_TRIG, int RANGEFINDER_OUT) {
  // if we're less than 0.1 us after issuing the command, hold the RANGEFINDER_TRIG pin high to trigger a pulse
  if (rangeFinderTriggerStartTime > 0 && millis() - rangeFinderTriggerStartTime <= 0.01) {
    digitalWrite(RANGEFINDER_TRIG, HIGH);
  }
  // Otherwise keep it low
  else {
    digitalWrite(RANGEFINDER_TRIG, LOW);
  }
}

bool doServo(Servo servo, float servoAngle) {
  servo.write(servoAngle);
}

bool doReset(int RESET, bool resetFlag) {
  if (resetFlag) {digitalWrite(RESET, LOW);}
}