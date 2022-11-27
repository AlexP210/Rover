#include "CameraUtils.h"

// Ask the arducam to take a picture
bool cameraCapture(bool capturePhoto, ArduCAM camera) {
  camera.flush_fifo();
  camera.clear_fifo_flag();
  camera.start_capture();
  return true;
}

bool updateCameraCaptured(ArduCAM camera) {
  return camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK);
}

bool sendPhoto(bool _capturedPhoto, bool sendPhoto, ArduCAM camera) {
  if (_capturedPhoto && sendPhoto) {
    read_fifo_burst(camera);
    camera.clear_fifo_flag();
    return true;
  }
  return false;
}

bool read_fifo_burst(ArduCAM camera)
{
  // Reads from the Arducam buffer and writes to the 
  int temp = 0, temp_last = 0; int length = 0; 
  bool is_header = false;
  // The length of the image
  length = camera.read_fifo_length();
  // 
  if (length >= MAX_FIFO_SIZE) //512 kb
  {
    return -1;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("ACK CMD Size is 0. END"));
    return 0;
  }
  camera.CS_LOW();
  camera.set_fifo_burst();//Set fifo burst mode
  temp =  SPI.transfer(0x00);
  length --;
  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    if (is_header == true)
    {
      Serial.write(temp);
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      Serial.write(temp_last);
      Serial.write(temp);
    }
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    break;
    delayMicroseconds(15);
  }
  camera.CS_HIGH();
  is_header = false;
  return 1;
}
