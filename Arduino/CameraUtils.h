#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

bool cameraCapture(bool capturePhoto, ArduCAM camera);
bool updateCameraCaptured(ArduCAM camera);
bool cameraSend(bool _capturedPhoto, bool sendPhoto,ArduCAM camera);