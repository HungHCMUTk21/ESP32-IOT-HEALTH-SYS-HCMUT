#ifndef SYSTEM_OP_ // include guard
#define SYSTEM_OP_
#pragma once

#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>

#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "Adafruit_MLX90614.h"
#include "Firebase_ESP_Client.h"

//---------DEFINES---------
#define MAX_BRIGHTNESS  255
#define MAX30102_SDA    17
#define MAX30102_SCL    18

#define MLX90614_SDA    11
#define MLX90614_SCL    12

//---------GENERAL FUNCTION PROTOTYPES---------
// Attempt to initialize the MAX30105 sensor. Check for a successful connection and report.
void init_MAX30102(MAX30105 &refSensor);
// Configure sensor with default settings for heart rate monitoring
void setup_MAX30102();
// Check if there's a finger is on the sensor
bool checkFinger();
// Read the infrared value from the sensor
void updateIRvalue();
// Return the infrared value
long getIRvalue();
// connect to Wifi
void initWifi(const char* ssid, const char* password);

//---------MLX30102 PULSE OXYMETER PROTOTYPES---------
// Calibrate the signal range using initial data
void calibrateSignalRange();
// Update data samples and calculate the necessary information. Continuously taking samples from MAX30102. Heart rate and SpO2 are calculated every 1 second
void dataUpdate();
// Return Oxygen saturation (SpO2)
int getSpO2();
// Check if SpO2 data is valid
bool validSpO2();
// Return Heartbeats per minute
float getBPM();
// Check if BPM data is valid
bool validBPM();

//---------MAX90614 IR TEMPERATURE SENSOR PROTOTYPES---------
void initMLX(Adafruit_MLX90614 &mlx_sensor, TwoWire &i2cPort); // function to initially setup the MLX sensor
float getObjTemp(); // function to return the object's temperature
float getAmbTemp(); // function to return ambient temperature

//---------FIREBASE REALTIME DATABASE PROTOTYPES---------
// Insert Firebase project API Key
#define API_KEY "AIzaSyCT51_XOB2G5ErMyPGR7uTo_LTWB8xWtEQ"
// Insert the RTDB URL
#define DATABASE_URL "https://platformio-demo-default-rtdb.asia-southeast1.firebasedatabase.app"

void sendData(); // function to send data to RTDB
void setupFirebaseRTDB(); // initial Firebase RTDB setup

#endif /* SYSTEM_OP_ */