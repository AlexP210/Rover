#include <Servo.h>
#include <Arduino.h>

class Rover {
    private:

        // Pin configuration

        const int LEFT_ENABLE_PIN = 2;
        const int RIGHT_ENABLE_PIN = 7;
        const int LEFT_DRIVE_1_PIN = 5;
        const int LEFT_DRIVE_2_PIN = 6;
        const int RIGHT_DRIVE_1_PIN = 9;
        const int RIGHT_DRIVE_2_PIN = 10;
        const int RANGEFINDER_TRIG_PIN = 8;
        const int RANGEFINDER_OUT_PIN = 12;
        const int SERVO_CONTROL_PIN = 11;
        const int LED_PIN = 13;

        // class variables

        bool driving = false; bool requestDriveStop = false; bool requestMotorDisable = false;
        float leftDriveStartTime = 0; float leftDriveDuration = 0; int leftDriveDirection = 0;
        float rightDriveStartTime = 0; float rightDriveDuration = 0; int rightDriveDirection = 0;

        bool readingRange = false; bool requestReadingRangeStop = false;
        bool triggerRangeFinder = false; float lastRangeFinderPulseState = 0; float currentRangeFinderPulseState = 0;
        float rangeFinderPulseStartTime = 0; float rangeFinderPulseEndTime = 0;
        bool rangeFinderReadingReady = false; float lastRangeReading;

        bool servoSweeping = false; bool servoSetting; bool didRangeReading; bool requestServoSweepingStop = false; bool requestServoSettingStop;   
        Servo servo; int servoAngle = 90; float lastServoCommandTime = 0; bool servoCommanded = false;;
        float servoSweepStart = 90; float servoSweepEnd = 90; float servoSweepStep = 0;

        bool ledState = false;

        String response;

    public:
        // Methods to create / control the robot 
        void setup();
        void drive(int direction, float duration);
        void turn(int direction, float duration);
        void toggleDrive(bool state);
        void requestStopDrive();
        void stopDrive();
        void safe();
        void doSingleDistanceMeasurement();
        float getSingleDistanceMeasurement();
        void servoSet(float commandedServoAngle);
        void servoSweep(float commandedServoStartAngle, float commandedServoStopAngle, float commandedServoStepAngle);
        float* getServoSweepData();
        void led();
        String act();
};
