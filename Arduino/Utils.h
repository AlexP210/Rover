#include <ArduCAM.h>
#include "Wire.h"

bool readFifoBurst(ArduCAM camera);

String getTerm(String command, int index);

bool checkState(int leftDriveDuration, int rightDriveDuration, int leftDriveDirection, int rightDriveDirection, bool sendPhoto, ArduCAM camera);

ArduCAM initializeCamera(int CAMERA_CS);