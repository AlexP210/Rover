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
bool rangeFinderReadingReady = false; 
float servoAngle = 90; bool servoSweeping = false; float servoSweepStart = 90; float servoSweepEnd = 90; float servoSweepStep = 0;
bool ledState = false;

// Objects
Servo servo;

// Command handling
String command = "";
String subcommand = "";
bool commandHandled = false;

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

    // Commands to enable/disable the drive trains
    else if (subcommand == "ENABLE") {
        subcommand = getTerm(command, 1);
        if (subcommand == "LEFT") {
            digitalWrite(LEFT_ENABLE, HIGH);
            commandHandled = true;
        }
        else if (subcommand == "RIGHT") {
            digitalWrite(RIGHT_ENABLE, HIGH);
            commandHandled = true;
        }
    }

    // Commands to enable/disable the drive trains
    else if (subcommand == "DISABLE") {
        subcommand = getTerm(command, 1);
        if (subcommand == "LEFT") {
            digitalWrite(LEFT_ENABLE, LOW);
            commandHandled = true;
        }
        else if (subcommand == "RIGHT") {
            digitalWrite(RIGHT_ENABLE, LOW);
            commandHandled = true;
        }
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
    }

    // Rangefinder commands
    else if (subcommand == "RANGE") {
        subcommand = getTerm(command, 1);
        if (subcommand == "MEASURE") {
            rangeFinderTriggerStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "READ") {
            if (rangeFinderReadingReady) {
                Serial.println((rangeFinderPulseEndTime - rangeFinderPulseStartTime)/1000.0 * 343.0 / 2);
                rangeFinderReadingReady = false;
                rangeFinderTriggerStartTime = 0;
            }
            else {
                Serial.println("NOT READY");
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
        if (subcommand = "SET") {
            servoAngle = getTerm(command, 2).toFloat();
            commandHandled = true;
        }
        else if (subcommand = "SWEEP") {
            servoSweeping = true;
            servoSweepStart = max(getTerm(command, 2).toFloat(), 15);
            servoAngle = servoSweepStart;
            servoSweepEnd = getTerm(command, 3).toFloat();
            servoSweepStep = getTerm(command, 4).toFloat();
            rangeFinderTriggerStartTime = millis();
            commandHandled = true;
        }

    }

    // Log the results
    if (command != "") {
        bool stateValid = checkState(leftDriveDuration, rightDriveDuration, leftDriveDirection, rightDriveDirection);
        if (commandHandled && !stateValid) {Serial.println("COMMAND REJECTED");}
        else if (commandHandled && stateValid) {Serial.println("COMMAND ACCEPTED");}
        else {Serial.println("COMMAND NOT FOUND");}
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
    // If we're on the rising edge of the pulse, then log the start time of the pulse
    currentRangeFinderPulseState = digitalRead(RANGEFINDER_OUT);
    // Serial.println(currentRangeFinderPulseState);
    if (currentRangeFinderPulseState == HIGH && lastRangeFinderPulseState == LOW) {
        rangeFinderPulseStartTime = millis();
    }
    // If it's the falling edge of the pulse, then set the end time, the ready flag, and re-set the last state. Return
    else if (currentRangeFinderPulseState == LOW && lastRangeFinderPulseState == HIGH) {
        rangeFinderPulseEndTime = millis();
        rangeFinderReadingReady = true;
    }
    // If we get here, then reading is not finished; set the state
    lastRangeFinderPulseState = currentRangeFinderPulseState;


    // // Update the servo sweep angle
    // if (rangeFinderReadingReady && servoSweeping && servoAngle + servoSweepStep <= servoSweepEnd) {
    //     // Report the last value
    //     Serial.println((rangeFinderPulseEndTime - rangeFinderPulseStartTime)/1000.0 * 343.0 / 2);
    //     rangeFinderReadingReady = false;
    //     rangeFinderTriggerStartTime = 0;
    //     servoAngle += servoSweepStep;
    //     rangeFinderTriggerStartTime = millis();
    // }
}


