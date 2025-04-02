#ifndef FIREBASE_OP_ // include guard
#define FIREBASE_OP_
#pragma once

#include <WiFi.h>
#include "Firebase_ESP_Client.h"

//===============FIREBASE REALTIME DATABASE===============
// Insert Firebase project API Key
#define API_KEY "AIzaSyCT51_XOB2G5ErMyPGR7uTo_LTWB8xWtEQ"
// Insert the RTDB URL
#define DATABASE_URL "https://platformio-demo-default-rtdb.asia-southeast1.firebasedatabase.app"

/**
 * @brief Set up Firebase's RTDB class object with the necessary parameters and connect to the RTDB
 * @return Void.
 */
void setupFirebaseRTDB();

/**
 * @brief Send all the necessary data (SpO2, BPM, Object temperature and Body weight) to Firebase's Realtime Database (RTDB)
 * @return Void.
 */
void sendData();

#endif /* FIREBASE_OP_ */