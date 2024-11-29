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
/**
 * @brief Connect the ESP32 to a WiFi access point (AP) under station (STA) mode
 * @param ssid SSID of the target AP
 * @param password Password of the target AP
 * @return Void.
 */
void initWifi(const char* ssid, const char* password);

//---------MLX30102 PULSE OXYMETER PROTOTYPES---------
/**
 * @brief Attempt to initialize the MAX30105 sensor. Check for a successful connection and report.
 * @param refSensor pass-by-reference MAX30105 object 
 * @return Void.
 */
void init_MAX30102(MAX30105 &refSensor);

/**
 * @brief Setup the MAX30102 pulse oxymeter sensor with the preferred parameters
 * @return Void.
 */
void setup_MAX30102();

/**
 * @brief Check if there's a finger on the sensor
 * @return True if finger present - False otherwise
 */
bool checkFinger();

/**
 * @brief Read the infrared value from the MAX30102 sensor
 * @return Void.
 */
void updateIRvalue();

/**
 * @brief Return the infrared value from the MAX30102 sensor
 * @return IR value of the sensor
 */
long getIRvalue();

/**
 * @brief Calibrate the signal range using initial data
 * @return Void.
 */
void calibrateSignalRange();

/**
 * @brief Update data samples and calculate the necessary information. Continuously taking samples from MAX30102. Heart rate and SpO2 are calculated every 1 second
 * @return Void.
 */
void dataUpdate();

/**
 * @brief Return oxygen saturation (SpO2) value calculated from MAX30102
 * @return Oxygen saturation value
 */
int getSpO2();

/**
 * @brief Check if SpO2 data is valid
 * @return True if the data is valid - False otherwise
 */
bool validSpO2();

/**
 * @brief Return Heartbeats per minute (BPM) value calculated from MAX30102
 * @return Oxygen saturation value
 */
float getBPM();

/**
 * @brief Check if BPM data is valid
 * @return True if the data is valid - False otherwise
 */
bool validBPM();

//---------MAX90614 IR TEMPERATURE SENSOR PROTOTYPES---------
/**
 * @brief Initialize and setup the MLX90614 IR temperature sensor with its own I2C bus
 * @param mlx_sensor Pass-by-reference MLX90614 sensor class
 * @param i2cPort Pass-by-reference I2C TwoWire class
 * @return Void.
 */
void initMLX(Adafruit_MLX90614 &mlx_sensor, TwoWire &i2cPort);

/**
 * @brief Return the object's temperature (in Celcius)
 * @return Object's temperature in Celcius
 */
float getObjTemp();

/**
 * @brief Return ambient temperature (in Celcius)
 * @return Ambient temperature in Celcius
 */
float getAmbTemp();

//---------FIREBASE REALTIME DATABASE PROTOTYPES---------
// Insert Firebase project API Key
#define API_KEY "AIzaSyCT51_XOB2G5ErMyPGR7uTo_LTWB8xWtEQ"
// Insert the RTDB URL
#define DATABASE_URL "https://platformio-demo-default-rtdb.asia-southeast1.firebasedatabase.app"

/**
 * @brief Set up Firebase's RTDB class object with the necessary parameters and connect to the RTDB
 * @return Void.
 */
void setupFirebaseRTDB(); // initial Firebase RTDB setup

/**
 * @brief Send all the necessary data (SpO2, BPM, Object temperature and Body weight) to Firebase's Realtime Database (RTDB)
 * @return Void.
 */
void sendData(); // function to send data to RTDB

#endif /* SYSTEM_OP_ */