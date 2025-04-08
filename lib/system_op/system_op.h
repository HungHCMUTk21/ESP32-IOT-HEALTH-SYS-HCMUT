/** \file system_op.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: system_op.h
* Description: This module is a wrapper for system tasks and processing.
*
*/
#ifndef SYSTEM_OP_ // include guard
#define SYSTEM_OP_
#pragma once

#include <Arduino.h>
#include "MAX30102_op.h"
#include "MLX90614_op.h"
#include "scale_ble.h"
#include "servo_op.h"

#define NUM_SAMPLES 5 // Number of samples to send to the serial

/**
 * @brief Initialize and start serial receive and processing commands
 * @return Void.
 */
void serial_receive();

#endif /* SYSTEM_OP_ */