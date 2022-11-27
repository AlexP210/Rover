#include <ArduCAM.h>

bool readFifoBurst(ArduCAM camera);

String getTerm(String command, int index);

bool checkState(int leftDriveDuration, int rightDriveDuration, int leftDriveDirection, int rightDriveDirection);

ArduCAM initializeCamera(ArduCAM camera);