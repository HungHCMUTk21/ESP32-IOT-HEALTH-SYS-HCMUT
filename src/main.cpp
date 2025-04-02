#include "system_op.h"

void setup(){
  Serial.begin(115200);
  delay(3000);

  // Activates sensors, tries to reactivate them 1 time if it fails.
  if (!initMAX30102())
  {
    initMAX30102();
  }
  if (!initMLX90614())
  {
    initMLX90614();
  }
  
  initBLE();
  connectToScale();
}

void loop(){
  connectToScale();
  serial_receive();
}
