/** \file scale_ble.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: scale_ble.h
* Description: This module is the BLE operation controller for the Xiaomi Smart scale 2
*
*/
#ifndef SCALE_BLE_ // include guard
#define SCALE_BLE_
#pragma once

#include <BLEDevice.h>

// #define BLE_DEBUG // Serial debugging

//===============XIAOMI SMART SCALE 2 BLUETOOTH OPERATION PROTOTYPES===============
#define scaleConnectionMode 0 // Mode = 0: connect using MAC address; Mode = 1: connect using device name 
// scale at home: 5c:64:f3:74:f6:c9
// scale at lab: 5c:64:f3:70:cf:69
#define xiaomiScaleMAC      "5c:64:f3:70:cf:69" // connect using MAC address will provide absolute device connection
#define xiaomiScaleName     "MI SCALE2" // connect using device's name might connect to the wrong same-name device
#define xiaomiScaleServUUID "0000181d-0000-1000-8000-00805f9b34fb" // Xiaomi Scale Service UUID
#define xiaomiScaleCharUUID "00002a9d-0000-1000-8000-00805f9b34fb" // Xiaomi Scale Characteristic UUID

/**
 * @brief Check if the weight is definitive weight and ready for sending
 * @return True if Weight is definitive - False otherwise
 */
uint8_t validSendWeight();

/**
 * @brief Data done sending, reset the data sending flag.
 * @return Void.
 */
void resetWeightSendFlag();

/**
 * @brief Check if the weight is still on the scale
 * @return True if Weight still exists on the scale - False otherwise
 */
uint8_t onScale();

/**
 * @brief Return the definiteve weight
 * @return Definitive weight
 */
float getDefWeight();

/**
 * @brief Connect to the Xiaomi Scale after using the selected connection mode
 * @return True if connected successfully, false otherwise
 */
bool connectToScale();

/**
 * @brief Initialize and set up Bluetooth Low Energy (BLE); start scanning for the scale
 * @return Void.
 */
void initBLE();

/**
 * @brief Turn off BLE
 * @return Void.
 */
void turnOffBLE();

/**
 * @brief Checks the current connection status of the scale
 * @return True if connected, false otherwise
 */
uint8_t scaleConnectStatus();

#endif /* SCALE_BLE_ */