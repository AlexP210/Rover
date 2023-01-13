#include "Utils.h"
#include "Rover.h"

#define TERMINATION_CHARACTER '|'

// Debug flag
bool debug = false;

// Command handling
String command = "";
String subcommand = "";
String response = "";
bool commandReceived = false;
bool commandHandled = false;

// Rover object
Rover rover;

void setup() {
    if (debug) Serial.println("Starting initialize.");
    // Open the communication lines
    Serial.begin(9600);
    rover.setup();
    if (debug) Serial.println("Ending initialize.");
}

void loop() {
    // If a command is available, grab it
    if (Serial.available()) {
        char c = Serial.read();
        // As long as c is not the termination character, keep reading, addng to command, and exiting
        if (c != TERMINATION_CHARACTER) {command += c;}
        else {commandReceived = true;}
    }

    if (commandReceived) {

        if (debug) {Serial.print("Received command: "); Serial.println(command);}
        subcommand = getTerm(command, 0);
        commandHandled = false;

        // Alive or Dead commands
        if (subcommand == "PING") {
            commandHandled = true;
        }

        // Commands to drive
        else if (subcommand == "DRIVE") {
            subcommand = getTerm(command, 1);
            if (subcommand == "FORWARD") {
                rover.drive(1, getTerm(command, 3).toFloat()*1000);
                commandHandled = true;
            }
            else if (subcommand == "REVERSE") {
                rover.drive(-1, getTerm(command, 3).toFloat()*1000);
                commandHandled = true;
            }
            else if (subcommand == "TURN") {
                subcommand = getTerm(command, 2);
                if      (subcommand == "LEFT") {
                    rover.turn(-1, getTerm(command, 3).toFloat()*1000);
                    commandHandled = true;
                }
                else if (subcommand == "RIGHT") {
                    rover.turn(1, getTerm(command, 3).toFloat()*1000);
                    commandHandled = true;
                }
            }
            // Commands to enable/disable the drive trains
            else if (subcommand == "ENABLE") {
                rover.toggleDrive(true);
                commandHandled = true;
            }

            // Commands to enable/disable the drive trains
            else if (subcommand == "DISABLE") {
                rover.toggleDrive(false);
                commandHandled = false;
            }
        }

        // Rangefinder commands
        else if (subcommand == "RANGE") {
            subcommand = getTerm(command, 1);
            if (subcommand == "MEASURE") {
                rover.doSingleDistanceMeasurement();
                commandHandled = true;
            }
        }

        // Test commands
        else if (subcommand == "TEST") {
            subcommand = getTerm(command, 1);
            if (subcommand == "LED") {
                rover.led();
                commandHandled = true;
            } 
        }

        else if (subcommand == "SERVO") {
            subcommand = getTerm(command, 1);
            if (subcommand == "SET") {
                rover.servoSet(getTerm(command, 2).toFloat());
                commandHandled = true;
            }
            else if (subcommand == "SWEEP") {
                rover.servoSweep(getTerm(command, 2).toFloat(), getTerm(command, 3).toFloat(), getTerm(command, 4).toFloat());
                commandHandled = true;            
            }

        }

        // Update state variables
        command = "";
        subcommand = "";
        commandReceived = false;

        if (commandHandled) {Serial.print("Command Accepted|");}
        else {Serial.print("Command Not Found|");}
    }

    // Do the actions and get the response
    response = rover.act();
    Serial.print(response);
}


