#include <Arduino.h>

#include "system_op.h"

//-------------------------------BPM SPO2 CONFIG-------------------------------
// Create an instance of the MAX30105 class to interact with the sensor
MAX30105 particleSensor;

//-------------------------------MLX90614 CONFIG-------------------------------
// Create an MLX object
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
TwoWire mlxI2C = TwoWire(1); // initiate a second I2C port for the MLX sensor

//-------------------------------WI-FI CONFIG-------------------------------
#define WIFI_SSID     "Dung Hung" // change this based on the name of your wifi
#define WIFI_PASSWORD "19992003" // change to the password of your wifi

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  Serial.println("Initializing...");
 
  Wire.begin(MAX30102_SDA, MAX30102_SCL);
  mlxI2C.begin(MLX90614_SDA, MLX90614_SCL);

  // Attempt to initialize and set up sensors
  initMLX(mlx, mlxI2C);
  init_MAX30102(particleSensor);
  setup_MAX30102();
  calibrateSignalRange();

  initWifi(WIFI_SSID, WIFI_PASSWORD);
  setupFirebaseRTDB();
}

byte dataSendCount = 0;
void loop() {
  updateIRvalue();
  dataUpdate();
  
  Serial.print("Ambient = "); Serial.print(getAmbTemp());
  Serial.print("*C\tObject = "); Serial.print(getObjTemp()); Serial.println("*C");

  // Output the current IR value, BPM, and averaged BPM to the serial monitor
  Serial.print("IR=");
  Serial.print(getIRvalue());
  Serial.print(", BPM=");
  Serial.print(getBPM());
  Serial.print(", BPM valid?=");
  Serial.print(validBPM());
  Serial.print(", SpO2=");
  Serial.print(getSpO2());
  Serial.print(", SpO2 valid?=");
  Serial.print(validSpO2());
 
  // Check if the sensor reading suggests that no finger is placed on the sensor
  if (!checkFinger())
    Serial.print(" No finger?");
 
  Serial.println();

  
  if (dataSendCount >= 5 && validBPM() && validSpO2() && checkFinger()){
    sendData();
    dataSendCount = 0;
    delay(1500);
  }
  dataSendCount++;
}