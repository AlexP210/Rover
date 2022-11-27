#include <SoftwareSerial.h>
#include "CameraUtils.h"

String command;
String subcommand;
bool commandHandled;

// Define pins
const int LEFT_ENABLE = 2;
const int RIGHT_ENABLE = 3;
const int LEFT_DRIVE_1 = 5;
const int LEFT_DRIVE_2 = 6;
const int RIGHT_DRIVE_1 = 10;
const int RIGHT_DRIVE_2 = 11;
const int RIGHT_MOTOR_1_VOLTAGE = A0;
const int CAMERA_CS = 7;

// State variables
float leftDriveStartTime; float leftDriveDuration; String leftDriveDirection;
float rightDriveStartTime; float rightDriveDuration; String rightDriveDirection;
bool capturePhoto; bool _capturedPhoto; bool sendPhoto;

// State action functions
void driveLeft(float leftDriveStartTime, float leftDriveDuration, String direction) {
    // specify the direction
    int highPin;
    int lowPin;
    if (direction == "FORWARD") {highPin = LEFT_DRIVE_1; lowPin = LEFT_DRIVE_2;}
    else if (direction == "REVERSE") {highPin = LEFT_DRIVE_2; lowPin = LEFT_DRIVE_1;}
    // drive if within the time range
    if (millis() - leftDriveStartTime <= leftDriveDuration*1000) {analogWrite(highPin, 250);analogWrite(lowPin, 0);}
    else {analogWrite(LEFT_DRIVE_1, LOW);analogWrite(LEFT_DRIVE_2, LOW);}
}
void driveRight(float rightDriveStartTime, float rightDriveDuration, String direction) {
    // specify the direction
    int highPin;
    int lowPin;
    if (direction == "FORWARD") {highPin = RIGHT_DRIVE_1; lowPin = RIGHT_DRIVE_2;}
    else if (direction == "REVERSE") {highPin = RIGHT_DRIVE_2; lowPin = RIGHT_DRIVE_1;}
    // drive if within the time range
    if (millis() - rightDriveStartTime <= rightDriveDuration*1000) {analogWrite(highPin, 250);analogWrite(lowPin, 0);}
    else {analogWrite(RIGHT_DRIVE_1, LOW);analogWrite(RIGHT_DRIVE_2, LOW);}
}

// Define the camera
ArduCAM camera(OV2640, CAMERA_CS);

// State checker
bool checkState() {
    if (!(leftDriveDuration >= 0 || rightDriveDuration >= 0)) {return false;}
    if (!(leftDriveDirection == "FORWARD" || leftDriveDirection == "REVERSE")) {return false;}
    if (!(rightDriveDirection == "FORWARD" || rightDriveDirection == "REVERSE")) {return false;}
    return true;
}

// Utils
String getTerm(String command, int index) {
    if (index == 0) {return command.substring(0, command.indexOf(" "));}
    else {return getTerm(command.substring(command.indexOf(" ")+1, command.length()), index - 1);}
}

void setup() {
    // Configure output pins
    pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);
    pinMode(LEFT_ENABLE, OUTPUT); digitalWrite(LEFT_ENABLE, LOW);
    pinMode(RIGHT_ENABLE, OUTPUT); digitalWrite(RIGHT_ENABLE, LOW);
    pinMode(LEFT_DRIVE_1, OUTPUT); digitalWrite(LEFT_DRIVE_1, LOW);
    pinMode(LEFT_DRIVE_2, OUTPUT); digitalWrite(LEFT_DRIVE_2, LOW);
    pinMode(RIGHT_DRIVE_1, OUTPUT); digitalWrite(RIGHT_DRIVE_1, LOW);
    pinMode(RIGHT_DRIVE_2, OUTPUT); digitalWrite(RIGHT_DRIVE_2, LOW);
    pinMode(CAMERA_CS, OUTPUT); digitalWrite(CAMERA_CS, HIGH);
    // Configure input pins
    pinMode(RIGHT_MOTOR_1_VOLTAGE, INPUT);

    Serial.begin(9600);
    SPI.begin();

    // Camera stuff
    camera.write_reg(0x07, 0x80);
    delay(100);
    camera.write_reg(0x07, 0x00);
    delay(100);
    camera.set_format(JPEG);
    camera.InitCAM();
    camera.OV2640_set_JPEG_size(OV2640_320x240);
    delay(1000);
    camera.clear_fifo_flag();


}

void loop() {

    // If a command is available, grab it
    if (Serial.available()) {
        command = Serial.readStringUntil('\n');
        subcommand = getTerm(command, 0);
        commandHandled = false;
    }

    // Set parameters to determine behavior

    // Alive or Dead commands
    if (subcommand == "HIGH") {
        digitalWrite(LED_BUILTIN, HIGH);
        commandHandled = true;
    }
    else if (subcommand == "LOW") {
        digitalWrite(LED_BUILTIN, LOW);
        commandHandled = true;
    }

    if (subcommand == "VOLTS") {
        float volts = analogRead(RIGHT_MOTOR_1_VOLTAGE)/1024.0 * 5.0;
        Serial.print(volts);
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

    // Commands to drive
    else if (subcommand == "DRIVE") {
        subcommand = getTerm(command, 1);
        if      (subcommand == "LEFT") {
            leftDriveDirection = getTerm(command, 2);
            leftDriveDuration = getTerm(command, 3).toFloat();
            leftDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "RIGHT") {
            rightDriveDirection = getTerm(command, 2);
            rightDriveDuration = getTerm(command, 3).toFloat();
            rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "FORWARD") {
            leftDriveDirection = rightDriveDirection = "FORWARD";
            leftDriveDuration = rightDriveDuration = getTerm(command, 2).toFloat();
            leftDriveStartTime = rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "REVERSE") {
            leftDriveDirection = rightDriveDirection = "REVERSE";
            leftDriveDuration = rightDriveDuration = getTerm(command, 2).toFloat();
            leftDriveStartTime = rightDriveStartTime = millis();
            commandHandled = true;
        }
        else if (subcommand == "TURN") {
            subcommand = getTerm(command, 2);
            if      (subcommand == "LEFT") {
                leftDriveDirection = "FORWARD";
                rightDriveDirection = "REVERSE";
                rightDriveDuration = leftDriveDuration = getTerm(command, 3).toFloat();
                leftDriveStartTime = rightDriveStartTime = millis();
                commandHandled = true;
            }
            else if (subcommand == "RIGHT") {
                leftDriveDirection = "REVERSE";
                rightDriveDirection = "FORWARD";
                rightDriveDuration = leftDriveDuration = getTerm(command, 3).toFloat();
                leftDriveStartTime = rightDriveStartTime = millis();
                commandHandled = true;
            }
        }
    }

    // Camera commands
    else if (command == "CAMERA") {
        subcommand = getTerm(command, 1);
        if      (subcommand == "CAPTURE") {capturePhoto = true;}
        else if (subcommand == "SEND") {sendPhoto = true;}
    }

    // Execute commands
    driveRight(rightDriveStartTime, rightDriveDuration, rightDriveDirection);
    driveLeft(leftDriveStartTime, leftDriveDuration, leftDriveDirection);
    cameraCapture(capturePhoto, camera);
    cameraSend(_capturedPhoto, sendPhoto, camera);

    // Log the results
    if (command != "") {
        if (commandHandled && !checkState()) {Serial.println("COMMAND RECEIVED");}
        else if (commandHandled && checkState()) {Serial.println("COMMAND ACCEPTED");}
        else {Serial.println("COMMAND NOT FOUND");}
    }
    
    // Update state variables
    capturePhoto = false;
    sendPhoto = false;
    _capturedPhoto = updateCameraCaptured(camera);
    command = "";
    subcommand = "";
}

