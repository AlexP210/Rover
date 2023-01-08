#include "ActionFunctions.h"
#include "Utils.h"
#include <Servo.h>

// Debug flag
bool debug = false;

// Define pins
const int LEFT_ENABLE = 2;
const int RIGHT_ENABLE = 7;
const int LEFT_DRIVE_1 = 5;
const int LEFT_DRIVE_2 = 6;
const int RIGHT_DRIVE_1 = 9;
const int RIGHT_DRIVE_2 = 10;
const int RANGEFINDER_TRIG = 11;
const int RANGEFINDER_OUT = 12;
const int SERVO_CONTROL = 3;
const int LED = 13;
const int TEST_VOLTAGE = A0;

// State variables
float leftDriveStartTime = 0; float leftDriveDuration = 0; int leftDriveDirection = 0;
float rightDriveStartTime = 0; float rightDriveDuration = 0; int rightDriveDirection = 0;

float rangeFinderTriggerStartTime = 0; 
float lastRangeFinderPulseState = 0; float currentRangeFinderPulseState = 0;
float rangeFinderPulseStartTime = 0; float rangeFinderPulseEndTime = 0;
bool rangeFinderReadingReady = false; bool readingRange = false;

int servoAngle = 90; float lastServoCommandTime;
bool servoSweeping = false; float servoSweepStart = 90; float servoSweepEnd = 90; float servoSweepStep = 0;
float servoSweepData[180]; bool servoSweepDataReady;

bool ledState = false;

// Objects
Servo servo;

// Command handling
String command = "";
String subcommand = "";
bool commandHandled = false;
String response = "";

void setup() {
    
    if (debug) {Serial.println("Entering Setup");}

    // Configure output pins
    pinMode(LEFT_ENABLE, OUTPUT); digitalWrite(LEFT_ENABLE, LOW);
    pinMode(RIGHT_ENABLE, OUTPUT); digitalWrite(RIGHT_ENABLE, LOW);
    pinMode(LEFT_DRIVE_1, OUTPUT); digitalWrite(LEFT_DRIVE_1, LOW);
    pinMode(LEFT_DRIVE_2, OUTPUT); digitalWrite(LEFT_DRIVE_2, LOW);
    pinMode(RIGHT_DRIVE_1, OUTPUT); digitalWrite(RIGHT_DRIVE_1, LOW);
    pinMode(RIGHT_DRIVE_2, OUTPUT); digitalWrite(RIGHT_DRIVE_2, LOW);
    pinMode(RANGEFINDER_TRIG, OUTPUT); digitalWrite(RIGHT_DRIVE_2, LOW);
    pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
    // pinMode(SERVO_CONTROL, OUTPUT); digitalWrite(SERVO_CONTROL, LOW);

    // Configure input pins
    pinMode(TEST_VOLTAGE, INPUT);
    pinMode(RANGEFINDER_OUT, INPUT);

    // Initialize sensor/actuator objects
    servo.attach(SERVO_CONTROL);
    servo.write(servoAngle);

    // Open the communication lines
    Serial.begin(9600);

    // Log successful completion of setup
    if (debug) {Serial.println("Exiting Setup");}
}

void loop() {

    // If a command is available, grab it
    if (Serial.available()) {
        command = Serial.readStringUntil('\n');
        if (debug) {Serial.println("Received command: " + command);}
        subcommand = getTerm(command, 0);
        commandHandled = false;
    }
    // Alive or Dead commands
    if (subcommand == "PING") {
        commandHandled = true;
    }

    // Commands to drive
    else if (subcommand == "DRIVE") {
        subcommand = getTerm(command, 1);
        if      (subcommand == "LEFT") {
            leftDriveDirection = (getTerm(command, 2) == "FORWARD") ? 1 : ((getTerm(command, 2) == "REVERSE") ? -1 : 0);
            leftDriveDuration = getTerm(command, 3).toFloat()*1000;
            leftDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "RIGHT") {
            rightDriveDirection = (getTerm(command, 2) == "FORWARD") ? 1 : ((getTerm(command, 2) == "REVERSE") ? -1 : 0);
            rightDriveDuration = getTerm(command, 3).toFloat()*1000;
            rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "FORWARD") {
            leftDriveDirection = rightDriveDirection = 1;
            leftDriveDuration = rightDriveDuration = getTerm(command, 2).toFloat()*1000;
            leftDriveStartTime = rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "REVERSE") {
            leftDriveDirection = rightDriveDirection = -1;
            leftDriveDuration = rightDriveDuration = getTerm(command, 2).toFloat()*1000;
            leftDriveStartTime = rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "TURN") {
            subcommand = getTerm(command, 2);
            if      (subcommand == "LEFT") {
                leftDriveDirection = 1;
                rightDriveDirection = -1;
                rightDriveDuration = leftDriveDuration = getTerm(command, 3).toFloat()*1000;
                leftDriveStartTime = rightDriveStartTime = millis();
                commandHandled = true;
            }
            else if (subcommand == "RIGHT") {
                leftDriveDirection = -1;
                rightDriveDirection = 1;
                rightDriveDuration = leftDriveDuration = getTerm(command, 3).toFloat()*1000;
                leftDriveStartTime = rightDriveStartTime = millis();
                commandHandled = true;
            }
        }
        // Commands to enable/disable the drive trains
        else if (subcommand == "ENABLE") {
            subcommand = getTerm(command, 2);
            if (subcommand == "LEFT") {
                digitalWrite(LEFT_ENABLE, HIGH);
                Serial.print("<LOG>ENABLE LEFT: ENABLED|");
                commandHandled = true;
            }
            else if (subcommand == "RIGHT") {
                digitalWrite(RIGHT_ENABLE, HIGH);
                Serial.print("<LOG>ENABLE RIGHT: ENABLED|");
                commandHandled = true;
            }
        }

        // Commands to enable/disable the drive trains
        else if (subcommand == "DISABLE") {
            subcommand = getTerm(command, 2);
            if (subcommand == "LEFT") {
                digitalWrite(LEFT_ENABLE, LOW);
                response = "<LOG>DISABLE LEFT: DISABLED|";
                commandHandled = true;
            }
            else if (subcommand == "RIGHT") {
                digitalWrite(RIGHT_ENABLE, LOW);
                response = "<LOG>DISABLE RIGHT: DISABLED|";
                commandHandled = true;
            }
        }
    }

    // Rangefinder commands
    else if (subcommand == "RANGE") {
        subcommand = getTerm(command, 1);
        if (subcommand == "MEASURE") {
            rangeFinderTriggerStartTime = millis();
            readingRange = true;
            commandHandled = true;
        }
        else if (subcommand == "READ") {
            if (rangeFinderReadingReady) {
                Serial.println((rangeFinderPulseEndTime - rangeFinderPulseStartTime)/1000.0 * 343.0 / 2);
                rangeFinderReadingReady = false;
                readingRange = false;
                rangeFinderTriggerStartTime = 0;
            }
            else {
                Serial.println("<LOG>RANGE READ: NOT READY|");
            }
            commandHandled = true;
        }
    }

    // Test commands
    else if (subcommand == "TEST") {
        subcommand = getTerm(command, 1);
        if (subcommand == "VOLTAGE") {
            Serial.println(analogRead(TEST_VOLTAGE)/1024.0 * 5.0);
            commandHandled = true;
        }
        else if (subcommand == "LED") {
            ledState = !ledState;
            digitalWrite(LED, ledState);
            commandHandled = true;
        } 
    }

    else if (subcommand == "SERVO") {
        subcommand = getTerm(command, 1);
        if (subcommand == "SET") {
            if (!servoSweeping) {
                servoAngle = getTerm(command, 2).toFloat();
                lastServoCommandTime = millis();
                commandHandled = true;
            }
            else {
                Serial.println("SERVO IS SWEEPING|");
            }
        }
        else if (subcommand == "SWEEP") {
            subcommand = getTerm(command, 2);
            if (subcommand == "MEASURE") {
                servoSweeping = true;
                servoSweepStart = max(getTerm(command, 3).toFloat(), 15);
                servoSweepEnd = min(getTerm(command, 4).toFloat(), 175);
                servoSweepStep = getTerm(command, 5).toFloat();         
                servoAngle = servoSweepStart;
                lastServoCommandTime = millis();
                commandHandled = true;
            }
            else if (subcommand == "READ") {
                for (int i = 0; i < 180; i++) {
                    Serial.print(servoSweepData[i]); Serial.print(", ");
                }
                Serial.println("");
                servoSweeping = false;
                servoSweepDataReady = false;
                commandHandled = true;
            }
        }

    }

    // Log the results
    if (command != "") {
        bool stateValid = checkState(leftDriveDuration, rightDriveDuration, leftDriveDirection, rightDriveDirection);
        if (commandHandled && !stateValid) {Serial.println("<ACK>COMMAND REJECTED|");}
        else if (commandHandled && stateValid) {Serial.println("<ACK>COMMAND ACCEPTED|");}
        else {Serial.println("<ACK>COMMAND NOT FOUND|");}
    }

    // Execute all processes
    doDrive(rightDriveStartTime, rightDriveDuration, rightDriveDirection, RIGHT_DRIVE_1, RIGHT_DRIVE_2);
    doDrive(leftDriveStartTime, leftDriveDuration, leftDriveDirection, LEFT_DRIVE_1, LEFT_DRIVE_2);
    doRangeFinderReading(rangeFinderTriggerStartTime, 
                         lastRangeFinderPulseState, rangeFinderPulseStartTime, rangeFinderPulseEndTime,
                         rangeFinderReadingReady,
                         RANGEFINDER_TRIG, RANGEFINDER_OUT);
    doServo(servo, servoAngle);
    
    // Update state variables
    command = "";
    subcommand = "";

    /* If we're in the middle of a range reading, then do the following:
        1. Get the current pulse state
        2. Compare it to the last pulse state to find the start and end of the pulse
        3. If we found te tail end of the pulse, then set the flag that the reading is ready */
    if (readingRange) {
        currentRangeFinderPulseState = digitalRead(RANGEFINDER_OUT);
        if (currentRangeFinderPulseState == HIGH && lastRangeFinderPulseState == LOW) {
            rangeFinderPulseStartTime = millis();
        }
        else if (currentRangeFinderPulseState == LOW && lastRangeFinderPulseState == HIGH) {
            rangeFinderPulseEndTime = millis();
            rangeFinderReadingReady = true;
        }
        lastRangeFinderPulseState = currentRangeFinderPulseState;
    }

    /* If we're in the middle of a servo sweep, do the following:
        1. If we're done sweeping, do nothing.
        1. If it's been < 1 s since the last servo command, do nothing
        2. If it's been < 1 s AND we're not range reading, then start the range reading.
        3. If it's been > 1 s AND we are range reading AND the result is not ready, do nothing
        4. If it's been > 1 s AND we are range reading AND the result is ready, add the result to the array and update the angle.*/
    if (servoSweeping) {
        if (millis() - lastServoCommandTime < 1000 || servoSweepDataReady) {
            ;
        }
        else if (!readingRange) {
            rangeFinderTriggerStartTime = millis();
            readingRange = true;
            Serial.println("Starting new reading");
        }
        else if (!rangeFinderReadingReady) {
            ;
        }
        else {
            servoSweepData[servoAngle] = (rangeFinderPulseEndTime - rangeFinderPulseStartTime)/1000.0 * 343.0 / 2;
            rangeFinderReadingReady = false;
            readingRange = false;
            rangeFinderTriggerStartTime = 0;

            if (servoAngle < servoSweepEnd) {
                servoAngle = servoAngle + servoSweepStep;
                lastServoCommandTime = millis();
            }
            else if (servoAngle == servoSweepEnd) {
                servoSweepDataReady = true;
                servoAngle = 90;
            }
        }
    }
}


