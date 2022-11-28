#include "ActionFunctions.h"
#include "UpdateFunctions.h"
#include "Utils.h"

// Debug flag
bool debug = false;

// Define pins
const int LEFT_ENABLE = 2;
const int RIGHT_ENABLE = 3;
const int LEFT_DRIVE_1 = 5;
const int LEFT_DRIVE_2 = 6;
const int RIGHT_DRIVE_1 = 9;
const int RIGHT_DRIVE_2 = 10;
const int CAMERA_CS = 7;

// State variables
float leftDriveStartTime; float leftDriveDuration; int leftDriveDirection;
float rightDriveStartTime; float rightDriveDuration; int rightDriveDirection;
ArduCAM camera;
bool capturePhoto; bool _capturedPhoto; bool sendPhoto;

// Command handling
String command;
String subcommand;
bool commandHandled;

void setup() {
    
    if (debug) {Serial.println("Entering Setup");}

    // Open the communication lines
    Serial.begin(9600);
    SPI.begin();
    Wire.begin();

    // Configure output pins
    pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);
    pinMode(LEFT_ENABLE, OUTPUT); digitalWrite(LEFT_ENABLE, LOW);
    pinMode(RIGHT_ENABLE, OUTPUT); digitalWrite(RIGHT_ENABLE, LOW);
    pinMode(LEFT_DRIVE_1, OUTPUT); digitalWrite(LEFT_DRIVE_1, LOW);
    pinMode(LEFT_DRIVE_2, OUTPUT); digitalWrite(LEFT_DRIVE_2, LOW);
    pinMode(RIGHT_DRIVE_1, OUTPUT); digitalWrite(RIGHT_DRIVE_1, LOW);
    pinMode(RIGHT_DRIVE_2, OUTPUT); digitalWrite(RIGHT_DRIVE_2, LOW);
    pinMode(CAMERA_CS, OUTPUT); digitalWrite(CAMERA_CS, HIGH);
    
    // Initialize modules
    camera = initializeCamera(CAMERA_CS);

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
            leftDriveDuration = getTerm(command, 3).toFloat();
            leftDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "RIGHT") {
            rightDriveDirection = (getTerm(command, 2) == "FORWARD") ? 1 : ((getTerm(command, 2) == "REVERSE") ? -1 : 0);
            rightDriveDuration = getTerm(command, 3).toFloat();
            rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "FORWARD") {
            leftDriveDirection = rightDriveDirection = 1;
            leftDriveDuration = rightDriveDuration = getTerm(command, 2).toFloat();
            leftDriveStartTime = rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "REVERSE") {
            leftDriveDirection = rightDriveDirection = -1;
            leftDriveDuration = rightDriveDuration = getTerm(command, 2).toFloat();
            leftDriveStartTime = rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "TURN") {
            subcommand = getTerm(command, 2);
            if      (subcommand == "LEFT") {
                leftDriveDirection = 1;
                rightDriveDirection = -1;
                rightDriveDuration = leftDriveDuration = getTerm(command, 3).toFloat();
                leftDriveStartTime = rightDriveStartTime = millis();
                commandHandled = true;
            }
            else if (subcommand == "RIGHT") {
                leftDriveDirection = -1;
                rightDriveDirection = 1;
                rightDriveDuration = leftDriveDuration = getTerm(command, 3).toFloat();
                leftDriveStartTime = rightDriveStartTime = millis();
                commandHandled = true;
            }
        }
    }

    // Camera commands
    else if (subcommand == "CAMERA") {
        subcommand = getTerm(command, 1);
        if      (subcommand == "CAPTURE") {
            capturePhoto = true;
            commandHandled = true;
        }
        else if (subcommand == "SEND") {
            sendPhoto = true;
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
    doCameraCapture(capturePhoto, camera);
    doCameraSend(sendPhoto, camera);
    
    // Update state variables
    capturePhoto = false;
    sendPhoto = false;
    command = "";
    subcommand = "";

}

