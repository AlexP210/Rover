#include "Rover.h"

void Rover::setup() {
    // Configure output pins
    pinMode(LEFT_ENABLE_PIN, OUTPUT); digitalWrite(LEFT_ENABLE_PIN, LOW);
    pinMode(RIGHT_ENABLE_PIN, OUTPUT); digitalWrite(RIGHT_ENABLE_PIN, LOW);
    pinMode(LEFT_DRIVE_1_PIN, OUTPUT); digitalWrite(LEFT_DRIVE_1_PIN, LOW);
    pinMode(LEFT_DRIVE_2_PIN, OUTPUT); digitalWrite(LEFT_DRIVE_2_PIN, LOW);
    pinMode(RIGHT_DRIVE_1_PIN, OUTPUT); digitalWrite(RIGHT_DRIVE_1_PIN, LOW);
    pinMode(RIGHT_DRIVE_2_PIN, OUTPUT); digitalWrite(RIGHT_DRIVE_2_PIN, LOW);
    pinMode(RANGEFINDER_TRIG_PIN, OUTPUT); digitalWrite(RIGHT_DRIVE_2_PIN, LOW);
    pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN, LOW);
    // pinMode(SERVO_CONTROL_PIN, OUTPUT); analogWrite(SERVO_CONTROL_PIN, 0);
    // Configure input pins
    pinMode(RANGEFINDER_OUT_PIN, INPUT);
    // Initialize sensor/actuator objects
    servo.attach(SERVO_CONTROL_PIN);
    servo.write(servoAngle);
}

void Rover::toggleDrive(bool state) {
    /*
    THis function enables/disables the motor driver chip, to prevent the batteries from discharging if the motors will not be used for a while.
    */
    digitalWrite(LEFT_ENABLE_PIN, state);
    digitalWrite(RIGHT_ENABLE_PIN, state);
}

void Rover::drive(int direction, float duration) {
    /*
    This function initiates a drive by setting the appropriate attributes that will be used by act()
    */
   driving = true;
   rightDriveDirection = leftDriveDirection = direction;
   rightDriveDuration = leftDriveDuration = duration;
   leftDriveStartTime = rightDriveStartTime = millis();
}

void Rover::turn(int direction, float duration) {
    driving = true;
    leftDriveDirection = direction;
    rightDriveDirection = -direction;
    rightDriveDuration = leftDriveDuration = duration;
    leftDriveStartTime = rightDriveStartTime = millis();
}

void Rover::requestStopDrive() {
    requestDriveStop = true;
}

void Rover::stopDrive() {
    rightDriveDirection = leftDriveDirection = 0;
    rightDriveDuration = leftDriveDuration = 0;
    digitalWrite(LEFT_DRIVE_1_PIN, LOW); digitalWrite(LEFT_DRIVE_2_PIN, LOW);
    digitalWrite(RIGHT_DRIVE_1_PIN, LOW); digitalWrite(RIGHT_DRIVE_2_PIN, LOW);
    driving = false;
    requestDriveStop = false;
}

void Rover::safe() {
    requestDriveStop = true;
    requestReadingRangeStop = true;
    requestServoSweepingStop = true;
    requestMotorDisable = true;
}

void Rover::doSingleDistanceMeasurement() {
    readingRange = true;
    rangeFinderReadingReady = false;
}

float Rover::getSingleDistanceMeasurement() {
    if (rangeFinderReadingReady) {return lastRangeReading;}
    else {return NULL;}
}

void Rover::servoSet(float commandedServoAngle) {
    servoSetting = true;
    lastServoCommandTime = millis();
    servoAngle = commandedServoAngle;
    servoCommanded = false;
}

void Rover::servoSweep(float commandedServoStartAngle, float commandedServoStopAngle, float commandedServoStepAngle) {
    servoSweepStart = commandedServoStartAngle;
    servoSweepEnd = commandedServoStopAngle;
    servoSweepStep = commandedServoStepAngle;
    servoSweeping = true;
    rangeFinderReadingReady = false;
    didRangeReading = false;

    servoSet(servoSweepStart);
}

float* Rover::getServoSweepData() {
    // if (servoSweepDataReady) {return servoSweepData;}
    // else {return nullptr;}
}

void Rover::led() {
    ledState = !ledState;
}


String Rover::act() {
    // The response string
    response = "";

    // Handle driving actions
    if (driving) {
        // Handle what to do if there is a request to disable motors or stop driving
        if (requestMotorDisable || requestDriveStop) {
            if (requestDriveStop) {
                stopDrive();
                response += "DRIVE STOPPED|";
            }
            if (requestMotorDisable) {
                toggleDrive(false);
                requestMotorDisable = false;
            }
        }
        // If we are ok to drive, and the drive command we received is still valid, then
        else if (millis() - leftDriveStartTime < leftDriveDuration) {
            // Check our drive directions and assign the pins to receive high and low voltage
            int leftHighPin, leftLowPin, rightHighPin, rightLowPin;
            if (leftDriveDirection == 1) {leftHighPin = LEFT_DRIVE_1_PIN; leftLowPin = LEFT_DRIVE_2_PIN;}
            else if (leftDriveDirection == -1) {leftHighPin = LEFT_DRIVE_2_PIN; leftLowPin = LEFT_DRIVE_1_PIN;}
            if (rightDriveDirection == 1) {rightHighPin = RIGHT_DRIVE_1_PIN; rightLowPin = RIGHT_DRIVE_2_PIN;}
            else if (rightDriveDirection == -1) {rightHighPin = RIGHT_DRIVE_2_PIN; rightLowPin = RIGHT_DRIVE_1_PIN;}
            // Give high and low voltage
            digitalWrite(leftHighPin, HIGH); digitalWrite(leftLowPin, LOW);
            digitalWrite(rightHighPin, HIGH); digitalWrite(rightLowPin, LOW);
        }
        // If the command is no longer valid but no stop requested (i.e. drive timer just expired)
        else {
            stopDrive();
            response += "DRIVE COMPLETED|";
        }
    }
    
    // Handle range-finding actions
    if (readingRange) {
        // If there is a request to stop reading range, re-set all attributes
        if (requestReadingRangeStop) {
            triggerRangeFinder = false;
            rangeFinderReadingReady = false;
            rangeFinderPulseStartTime = 0;
            rangeFinderPulseEndTime = 0;
            readingRange = false;
            requestReadingRangeStop = false;
            response += "RANGE READ STOPPED|";
        }
        // Otherwise, if the trigger is started but it hasn't been 0.1 ms, keep the trigger pin up
        else {
            digitalWrite(RANGEFINDER_TRIG_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(RANGEFINDER_TRIG_PIN, LOW);
            lastRangeReading = pulseIn(RANGEFINDER_OUT_PIN, HIGH)/1000000.0 * 343.0 / 2;
            response += "RANGE @ ";
            response += String(servoAngle); 
            response += String(" = ");
            response += String(lastRangeReading);
            response += String("|");
            response += String("RANGE MEASUREMENT DONE|");
            readingRange = false;
            rangeFinderReadingReady = true;
        }
        // // If the trigger pulse is over, then grab echo pin voltage check for the echo pulse
        // else {
        //     currentRangeFinderPulseState = digitalRead(RANGEFINDER_OUT_PIN);
        //     if (currentRangeFinderPulseState == HIGH && lastRangeFinderPulseState == LOW) {
        //         rangeFinderPulseStartTime = millis();
        //     }
        //     else if (currentRangeFinderPulseState == LOW && lastRangeFinderPulseState == HIGH) {
        //         rangeFinderPulseEndTime = millis();
        //         lastRangeReading = (rangeFinderPulseEndTime - rangeFinderPulseStartTime)/1000.0 * 343.0 / 2;
        //         rangeFinderReadingReady = true;
        //         response += "RANGE MEASUREMENT @ " + String(servoAngle) + " degrees: " + String(lastRangeReading)+ "|";
        //         response += "RANGE MEASUREMENT DONE|";
        //         readingRange = false;
        //         triggerRangeFinder = false;
        //     }
        //     lastRangeFinderPulseState = currentRangeFinderPulseState;
        // }
    }

    // If The servo is commanded
    if (servoSetting) {
        if (requestServoSettingStop) {
            servoSetting = false;
            requestServoSettingStop = false;
        }
        else if (millis() - lastServoCommandTime < 2000 && !servoCommanded) {
            servo.write(servoAngle);
            servoCommanded = true;
        }
        else if (millis() - lastServoCommandTime > 2000) {
            servoSetting = false;
            response += "SERVO MOVED TO: " + String(servoAngle) + "|";
        }
    }

    if (servoSweeping) {
        if (requestServoSweepingStop) {
            servoSweepStart = 90;
            servoSweepEnd = 90;
            servoSweepStep = 0;
            servoSweeping = false;
            requestServoSweepingStop = false;
        }
        else if (!servoSetting && !rangeFinderReadingReady) {
            doSingleDistanceMeasurement();
        }
        else if (rangeFinderReadingReady) {
            if (servoAngle < servoSweepEnd) {servoSet(servoAngle + servoSweepStep);rangeFinderReadingReady=false;}
            else if (servoAngle == servoSweepEnd) {
                servoSweeping = false;
                response += "SERVO SWEEP DONE|";
                servoSet(90);
            }
        }
    }

    digitalWrite(LED_PIN, ledState);
    return response;
}