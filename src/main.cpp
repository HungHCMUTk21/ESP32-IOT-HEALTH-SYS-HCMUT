#include "system_op.h"

void setup(){
  Serial.begin(115200);
  delay(2000);

  // Activates sensors, tries to reactivate them 1 time if it fails.

  // Start Pulse oximeter sensor
  Serial.println("POX-START");
  if (!initMAX30102())
  {
    Serial.println("POX-STATUS-0");
    if (!initMAX30102()){
      Serial.println("POX-STATUS-0");
    }else{
      Serial.println("POX-STATUS-1");
    }
  }else{
    Serial.println("POX-STATUS-1");
  }

  // Start Temperature sensor
  Serial.println("TEMP-START");
  if (!initMLX90614())
  {
    Serial.println("TEMP-STATUS-0");
    if (!initMLX90614()){
      Serial.println("TEMP-STATUS-0");
    }else{
      Serial.println("TEMP-STATUS-1");
    }
  }else{
    Serial.println("TEMP-STATUS-1");
  }
  
  Serial.println("SERVO-START");
  servo_init();

  initBLE();
  connectToScale();
}

void loop(){
  servo_updateSerial();

  if(!scaleConnectStatus() && !servo_isMoving() && !system_busy()){
    connectToScale();
  }
  
  serial_receive();
}
