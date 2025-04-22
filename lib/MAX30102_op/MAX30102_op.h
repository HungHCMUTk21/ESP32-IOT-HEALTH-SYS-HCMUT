/** \file max30102.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: max30102_op.h
* Description: This module is for the MAX30102 pulse oximeter sensor
*
*/
#ifndef MAX30102_OP_ // include guard
#define MAX30102_OP_
#pragma once

#include <Wire.h>
#include "algorithm_by_RF.h"
#include "max30102.h"

//#define MAX30102_DEBUG // Enable MAX30102 serial debugging
#define MAX30102_INT 10 // MAX30102 interrupt pin
#define MAX30102_SDA 11 // MAX30102 I2C SDA pin
#define MAX30102_SCL 12 // MAX30102 I2C SCL pin

//#define MAX30102_DEBUG // Enable serial data printing for debugging

/**
 * @brief Initialize the MAX30102 sensor
 * @return True if activated. False if otherwise.
 */
uint8_t initMAX30102();

/**
 * @brief Update MAX30102 sensor data whenever the interrupt pin is called
 * @param none
 * @return Void
 */
void updatePoxData();

/**
 * @brief Return Heartbeats per minute (BPM) value calculated from MAX30102
 * @param none
 * @return Heartbeats per minute
 */
int getBPM();

/**
 * @brief Return Oxygen Saturation (SpO2) value calculated from MAX30102
 * @param none
 * @return Oxygen saturation value
 */
float getSpO2();

/**
 * @brief Check if BPM data is valid
 * @param none
 * @return True if data is valid, false otherwise
 */
bool validBPM();

/**
 * @brief Check if SpO2 data is valid
 * @param none
 * @return True if data is valid, false otherwise
 */
bool validSPO();

#endif /* MAX30102_OP_ */