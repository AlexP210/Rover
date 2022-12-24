#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

bool doDrive(float driveStartTime, float driveDuration, int direction, int drivePin1, int drivePin2);
bool doCameraCapture(bool capturePhoto, ArduCAM camera);
bool doCameraSend(bool sendPhoto, ArduCAM camera);
bool doTestCameraSPI(bool testSPI, ArduCAM camera);
bool doRangeFinderReading(float rangeFinderTriggerStartTime, 
                          bool lastRangeFinderPulseState, float rangeFinderPulseStartTime, float rangeFinderPulseEndTime, 
                          bool rangeFinderReadingReady,
                          int RANGEFINDER_TRIG, int RANGEFINDER_OUT);