#include "UpdateFunctions.h"

bool updateCameraCaptured(ArduCAM camera) {
  return camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK);
}