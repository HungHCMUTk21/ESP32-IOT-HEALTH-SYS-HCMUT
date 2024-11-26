#ifndef MAX30102_RUN_H_ // include guard
#define MAX30102_RUN_H_
#pragma once

#include <SPI.h>
#include <Wire.h>

#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "heartRate.h"

#define MAX_BRIGHTNESS 255

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

//---------DATA PROTOTYPES---------
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

#endif /* MAX30102_RUN_H */