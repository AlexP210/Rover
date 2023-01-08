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
        const int RANGEFINDER_TRIG_PIN = 11;
        const int RANGEFINDER_OUT_PIN = 12;
        const int SERVO_CONTROL_PIN = 3;
        const int LED_PIN = 13;

        // class variables

        bool driving = false; bool requestDriveStop = false; bool requestMotorDisable = false;
        float leftDriveStartTime = 0; float leftDriveDuration = 0; int leftDriveDirection = 0;
        float rightDriveStartTime = 0; float rightDriveDuration = 0; int rightDriveDirection = 0;

        bool readingRange = false; bool requestReadingRangeStop = false;
        float rangeFinderTriggerStartTime = 0; float lastRangeFinderPulseState = 0; float currentRangeFinderPulseState = 0;
        float rangeFinderPulseStartTime = 0; float rangeFinderPulseEndTime = 0;
        bool rangeFinderReadingReady = false; float lastRangeReading;

        bool servoSweeping = false; bool servoSetting; bool requestServoSweepingStop = false; bool requestServoSettingStop;   
        Servo servo; int servoAngle = 90; float lastServoCommandTime;
        float servoSweepStart = 90; float servoSweepEnd = 90; float servoSweepStep = 0;
        float servoSweepData[180]; bool servoSweepDataReady;

        bool ledState = false;

    public:
        // Methods to create / control the robot
        Rover();
        void drive(int direction, float duration);
        void turn(int direction, float duration);
        void toggleDrive(bool state);
        void stop();
        void safe();
        void doSingleDistanceMeasurement();
        float getSingleDistanceMeasurement();
        void servoSet(float commandedServoAngle);
        void servoSweep(float commandedServoStartAngle, float commandedServoStopAngle, float commandedServoStepAngle);
        float* getServoSweepData();
        String act();
};
