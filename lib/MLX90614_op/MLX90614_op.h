/** \file MLX90614_op.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: MLX90614_op.h
* Description: This module is for the MLX90614 contactless temperature sensor
*
*/
#ifndef MLX90614_OP_ // include guard
#define MLX90614_OP_
#pragma once

#include <Wire.h>
#include "Adafruit_MLX90614.h"

//#define MLX90614_DEBUG // Enable MLX90614 serial debugging 
#define MLX90614_SDA 11   // MLX90614 I2C SDA pin
#define MLX90614_SCL 12   // MLX90614 I2C SCL pin
#define MLX90614_ADR 0x5A // MLX90614 I2C address

//===============MAX90614 IR TEMPERATURE SENSOR===============
/**
 * @brief Initialize and setup the MLX90614 IR temperature sensor with its own I2C bus
 * @return True if activated. False if otherwise.
 */
uint8_t initMLX90614();

/**
 * @brief Update Object and Ambient temperature data
 * @return Void.
 */
void updateTempData();

/**
 * @brief Returns the object's temperature (in Celcius)
 * @return object's temperature
 */
float getObjTemp();

/**
 * @brief Returns ambient temperature (in Celcius)
 * @return ambient temperature
 */
float getAmbTemp();

#endif /* MLX90614_OP_ */