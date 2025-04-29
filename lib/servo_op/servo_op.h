/** \file servo_op.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: servo_op.h
* Description: This module is a wrapper for servo operations.
*
*/
#ifndef SERVO_OP_ // include guard
#define SERVO_OP_
#pragma once

#include <CircularBuffer.hpp>
#include <HerkulexServo.h>

// Setup pins and IDs
#define SERVO_PIN_RX    9
#define SERVO_PIN_TX    8
#define SERVO_ID        0x03

// Move mode flags
#define SERVO_MOVE_UP       1
#define SERVO_MOVE_DOWN     2
#define SERVO_MOVE_STOP     3
#define SERVO_MOVE_DEFAULT  4

// Positional defines
#define SERVO_START_POS     500 // Starting position of the servo.
#define SERVO_MAX_UP_POS    690 // Maximum up-turning position. DO NOT CHANGE.
#define SERVO_MAX_DN_POS    375 // Maximum down-turning position. DO NOT CHANGE.

// For controlling the servo's turning speed
// 40 is the minimum recommended speed, or else the servo suffers from inadequate torque to handle the screen
#define SERVO_SPD   40
/**
 * @brief Start the servo serial communication and set initial position.
 * @return Void.
 */
void servo_init();

/**
 * @brief Update the servo serial port and other variables constantly.
 * @return Void.
 */
void servo_updateSerial();

/**
 * @brief Get the current position of the servo.
 * @return Servo position.
 */
uint16_t servo_getPos();

/**
 * @brief Set the move mode flag of the servo.
 * @return Void.
 */
void servo_setMoveFlag(uint8_t flagnum);

/**
 * @brief Check if the servo is currently turning or not.
 * @return True if the servo is turning, false otherwise.
 */
uint8_t servo_isMoving();

#endif /* SERVO_OP_ */