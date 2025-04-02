/** \file system_op.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: system_op.h
* Description: This module is the freeRTOS wrapper for system tasks.
*
*/
#ifndef SYSTEM_OP_ // include guard
#define SYSTEM_OP_
#pragma once

#include <Arduino.h>
#include "MAX30102_op.h"
#include "MLX90614_op.h"
#include "scale_ble.h"

#define NUM_SAMPLES 5 // Number of samples to send to the serial

void serial_receive();

#endif /* SYSTEM_OP_ */