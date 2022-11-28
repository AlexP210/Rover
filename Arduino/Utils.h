#include <ArduCAM.h>
#include "Wire.h"

bool readFifoBurst(ArduCAM camera);

String getTerm(String command, int index);

bool checkState(int leftDriveDuration, int rightDriveDuration, int leftDriveDirection, int rightDriveDirection);

ArduCAM initializeCamera(int CAMERA_CS);