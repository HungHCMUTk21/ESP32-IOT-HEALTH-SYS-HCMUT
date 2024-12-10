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

bool ble_flag = 0;
bool wifi_flag = 0;

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
  turnOffWifi();

  initBLE();
  connectToScale();
  ble_flag = 1;
}

byte dataSendCount = 0;
void loop() {
  if(!ble_flag){
    btStart();
    esp_bt_controller_enable((esp_bt_mode_t)0x01);

    initBLE();
    ble_flag = 1;
  }
  
  dataUpdate();
  connectToScale();
  
  Serial.print("Ambient = "); Serial.print(getAmbTemp());
  Serial.print("*C\tObject = "); Serial.print(getObjTemp()); Serial.println("*C");

  // Output the current BPM, SpO2 and Weight to the serial monitor
  Serial.print("BPM=");
  Serial.print(getBPM());

  Serial.print(", SpO2=");
  Serial.print(getSpO2());

  Serial.print(", Weight=");
  Serial.print(getDefWeight());

  // Check if the sensor reading suggests that no finger is placed on the sensor
  if (!checkFinger())
    Serial.print(" No finger?");
  Serial.println();

  if (validBPM() && validSpO2() && checkFinger()){
    dataSendCount++;
  }

  if (dataSendCount >= 7){
    turnOffBLE();
    ble_flag = 0;

    initWifi(WIFI_SSID, WIFI_PASSWORD);
    sendData();
    dataSendCount = 0;
  }
}