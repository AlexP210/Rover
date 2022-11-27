#include "Utils.h"
#include <SPI.h>


bool readFifoBurst(ArduCAM camera)
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

String getTerm(String command, int index) {
    if (index == 0) {return command.substring(0, command.indexOf(" "));}
    else {return getTerm(command.substring(command.indexOf(" ")+1, command.length()), index - 1);}
}

// State checker
bool checkState(int leftDriveDuration, int rightDriveDuration, int leftDriveDirection, int rightDriveDirection) {
    if (!(leftDriveDuration >= 0 || rightDriveDuration >= 0)) {return false;}
    if (!(leftDriveDirection == 1 || leftDriveDirection == -1)) {return false;}
    if (!(rightDriveDirection == 1 || rightDriveDirection == -1)) {return false;}
    return true;
}

ArduCAM initializeCamera(ArduCAM camera){
    camera.write_reg(0x07, 0x80);
    delay(100);
    camera.write_reg(0x07, 0x00);
    delay(100);
    camera.set_format(JPEG);
    camera.InitCAM();
    camera.OV2640_set_JPEG_size(OV2640_320x240);
    delay(1000);
    camera.clear_fifo_flag();
    return camera;
}