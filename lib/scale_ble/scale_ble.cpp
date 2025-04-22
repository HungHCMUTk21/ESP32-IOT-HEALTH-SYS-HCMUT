/** \file max30102.h ******************************************************
*
* Project: ESP32-IOT-HEALTH-SYS-HCMUT
* Filename: scale-ble.h
* Description: This module is a freeRTOS wrapper for the MAX30102 sensor
*
*/
#include "scale_ble.h"

//===============XIAOMI SMART SCALE 2 BLUETOOTH OPERATION PROTOTYPES===============
static BLEAdvertisedDevice* xiaomiScale;
static BLERemoteCharacteristic* remoteChar;
static bool goConnect = false;
static bool connected_flag = false;

float defWeight = 0.0; // definitive weight

bool valid_data = false; // Data for sending flag
bool isDefWeight = false; // Stable weight flag
bool isOnScale = false; // If weight is still on the scale flag

//Base UUIDs
//Weight Scale service
static BLEUUID servUUID(xiaomiScaleServUUID);
static BLEUUID charUUID(xiaomiScaleCharUUID);
static BLEAddress MACaddr(xiaomiScaleMAC);

uint8_t scaleConnectStatus(){
    return connected_flag;
}

// Callback function for BLE characteristc data nofication
void charCallback(BLERemoteCharacteristic* remoteChr, uint8_t* pData, size_t length, bool isNotify){
    // Calculate weight using the data received
    float weight = 0;
    weight = (pData[1] + pData[2] * 256) * 0.005;

    // 5th bit of the first byte represents if weight is stable
    isDefWeight = pData[0] & (1 << 5);
    // 7th bit of the first byte represents if weight is still on the scale
    isOnScale = pData[0] & (1 << 7);

    // register definitive weight
    if (isDefWeight){
        valid_data = true;
        defWeight = weight;
    }
#ifdef BLE_DEBUG
    // Printing received data bytes for console debugging
    Serial.print("Data received. Data length = ");
    Serial.print(length);
    Serial.print(". - Bytes: ");
    // Print each byte of the data
    for (int i = 0; i < length; i++){
        Serial.print(pData[i]);
        Serial.print("  ");
    }
    Serial.println(" ");
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.print(" Kg - ");

    if (isDefWeight){
      Serial.println("(Definitive)");
    } else{
      Serial.println("(Provisional)");
    }
    // End of console debugging
#endif
}

// Search for our Smart Scale using MAC address (connectionMode 0)
void connectModeMAC(BLEAdvertisedDevice &BLEdevice){
#ifdef BLE_DEBUG
    if (BLEdevice.getAddress() != MACaddr){
        Serial.print(".");
    } else{
        Serial.println("");
        Serial.print("Xiaomi Mi Smart Scale 2 weight scale found at MAC address: ");
        Serial.println(BLEdevice.getAddress().toString().c_str());
        BLEDevice::getScan() -> stop();

        Serial.println("Stopping the scan and attempting to connect to the scale");
        xiaomiScale = new BLEAdvertisedDevice(BLEdevice);
        goConnect = true;
    }
#else
    if (BLEdevice.getAddress() == MACaddr){
        BLEDevice::getScan() -> stop();
        xiaomiScale = new BLEAdvertisedDevice(BLEdevice);
        goConnect = true;
    }
#endif
}

// Search for our Smart Scale using device's name (connectionMode 1)
String scaleName = xiaomiScaleName;
void connectModeName(BLEAdvertisedDevice &BLEdevice){
#ifdef BLE_DEBUG
    if (BLEdevice.getName() != xiaomiScaleName){
        Serial.print(".");
    } else{
        Serial.println("");
        Serial.print("Xiaomi Mi Smart Scale 2 weight scale found at MAC address: ");
        Serial.println(BLEdevice.getAddress().toString().c_str());
        BLEDevice::getScan() -> stop();

        Serial.println("Stopping the scan and attempting to connect to the scale");
        xiaomiScale = new BLEAdvertisedDevice(BLEdevice);
        goConnect = true;
    }
#else
        if (BLEdevice.getName() == scaleName){
        BLEDevice::getScan() -> stop();
        xiaomiScale = new BLEAdvertisedDevice(BLEdevice);
        goConnect = true;
    }
#endif
}

// Callback class for each advertising device in the vicinity
class deviceCallback:public BLEAdvertisedDeviceCallbacks{
  // Class is called every time an advertised device is found
  void onResult(BLEAdvertisedDevice advertisedDevice){
    // A random device found, check if this is our Smart Scale 2
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(servUUID)) {
        if (scaleConnectionMode == 0){
            connectModeMAC(advertisedDevice);
        } else{
            connectModeName(advertisedDevice);
        }
    }
  } 
};

// Callback class for device events / device connections
class ClientCB:public BLEClientCallbacks{
    void onConnect(BLEClient* pclient){
        
    }
    void onDisconnect(BLEClient* pclient){
#ifdef BLE_DEBUG
        Serial.println("Scale disconnected. Reconnecting...");
#endif
        Serial.println("SCALE-STATUS-0");
        connected_flag = false;
    }
};
BLEClient* pClient;
bool initScaleConnection() {
    
#ifdef BLE_DEBUG
    Serial.println("Establishing communications with the scale:");
    pClient = BLEDevice::createClient();
    Serial.println("BLE client created");
    pClient -> setClientCallbacks(new ClientCB());
#else
    BLEClient* pClient = BLEDevice::createClient();
    pClient -> setClientCallbacks(new ClientCB());
#endif

    // Connect to the remote BLE Server.
    pClient -> connect(xiaomiScale);
    Serial.println("SCALE-STATUS-1");

#ifdef BLE_DEBUG
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient -> getService(servUUID);
    if(pRemoteService == nullptr) {
      Serial.println("ERROR - Failed to find service");
      pClient -> disconnect();
      return false;
    }
    Serial.println("Service found.");

    // Obtain a reference to the characteristic we are after in the remote BLE server.
    remoteChar = pRemoteService -> getCharacteristic(charUUID);
    if(remoteChar == nullptr) {
      Serial.print("ERROR - Failed to find characteristic");
      pClient -> disconnect();
      return false;
    }
    Serial.println("Characteristic found. Setting notifs callback.");
    remoteChar -> registerForNotify(charCallback);
    return true;
#else
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient -> getService(servUUID);
    if(pRemoteService == nullptr) {
      Serial.println("SCALE-ERROR-NOSERVICE");
      pClient -> disconnect();
      return false;
    }

    // Obtain a reference to the characteristic we are after in the remote BLE server.
    remoteChar = pRemoteService -> getCharacteristic(charUUID);
    if(remoteChar == nullptr) {
      Serial.print("SCALE-ERROR-NOCHAR");
      pClient -> disconnect();
      return false;
    }
    remoteChar -> registerForNotify(charCallback);
    
    return true;
#endif
}

BLEScan* BLEScanPtr = BLEDevice::getScan();
void initBLE(){
    Serial.println("SCALE-BLE-START");
    
    BLEDevice::init("");
    
    BLEScanPtr -> setAdvertisedDeviceCallbacks(new deviceCallback());
    BLEScanPtr -> setInterval(1280);
    BLEScanPtr -> setWindow(200);
    // Set active scan
    BLEScanPtr -> setActiveScan(false);
    // Scan during 4 seconds
    BLEScanPtr -> start(4, false);
}

bool connectToScale(){
    while (goConnect && !connected_flag){
        connected_flag = initScaleConnection();
        delay(200);
    }
    
    while (!goConnect){
        BLEScanPtr -> start(4, false);
        delay(200);
    }
    return connected_flag;
}

void turnOffBLE(){
    BLEDevice::deinit();
    btStop();
    esp_bt_controller_disable();
}

uint8_t validSendWeight(){
    return valid_data;
}

void resetWeightSendFlag(){
    valid_data = false;
}

uint8_t onScale(){
    return isOnScale;
}

float getDefWeight(){
    return defWeight;
}


