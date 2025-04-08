#include "system_op.h"

void setup(){
  Serial.begin(115200);
  delay(2000);

  // Activates sensors, tries to reactivate them 1 time if it fails.
  if (!initMAX30102())
  {
    initMAX30102();
  }
  if (!initMLX90614())
  {
    initMLX90614();
  }
  
  servo_init();

  initBLE();
  connectToScale();
}

void loop(){
  
  if(!scaleConnectStatus()){
    connectToScale();
  }
  
  servo_updateSerial();
  serial_receive();
}
