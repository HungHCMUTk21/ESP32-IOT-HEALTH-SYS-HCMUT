#include "system_op.h"

//===============VARIABLES===============
// An instance of the MAX30105 class to interact with the sensor
MAX30105* parSensor;

uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data

//===============GENERAL FUNCTIONs===============
void initWifi(const char* ssid, const char* password){
    int count = 0;
    Serial.println("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(250);
        count++;
        if (count > 100) break;
    }

    if (WiFi.status() != WL_CONNECTED){
        Serial.println("Cannot connect to WiFi, please reset.");
    } else{
        Serial.println();
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        delay(2000);
    }
}

void turnOffWifi(){
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

//===============MLX30102 PULSE OXYMETER===============
int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

void init_MAX30102(MAX30105 &refSensor){ // Attempt to initialize the MAX30105 sensor. Check for a successful connection and report.
    parSensor = &refSensor;
    if (!parSensor->begin(Wire, I2C_SPEED_FAST)){ // Start communication using fast I2C speed
        Serial.println("MAX30102 was not found. Please check wiring/power.");
        while(1); // Infinite loop to halt further execution if sensor is not found
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");
}

void setup_MAX30102(){ // Configure sensor with default settings for heart rate monitoring
    uint8_t ledBrightness = 125;    //Options: 0=Off to 255=50mA
    byte sampleAverage = 4;         //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;               //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 400;           //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;           //Options: 69, 118, 215, 411
    int adcRange = 8192;            //Options: 2048, 4096, 8192, 16384

    parSensor->setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}

bool checkFinger(){
    if(parSensor->getIR() < 50000)
        return false;
    else
        return true;
}

//Calibrate the signal range using initial data (PUT INSIDE SETUP)
void calibrateSignalRange(){
    bufferLength = 75; //buffer length of 75 stores 3 seconds of samples running at 25sps

    //read the first 75 samples, and determine the signal range
    for (byte i = 0 ; i < bufferLength ; i++){
        while (parSensor->available() == false) //do we have new data?
            parSensor->check(); //Check the sensor for new data

        redBuffer[i] = parSensor->getRed();
        irBuffer[i] = parSensor->getIR();
        parSensor->nextSample(); //We're finished with this sample so move to next sample

        Serial.print(F("red="));
        Serial.print(redBuffer[i], DEC);
        Serial.print(F(", ir="));
        Serial.println(irBuffer[i], DEC);
    }

    //calculate heart rate and SpO2 after first 75 samples (first 3 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}
// Update data samples and calculate the necessary information. Continuously taking samples from MAX30102. Heart rate and SpO2 are calculated every 1 second
void dataUpdate(){
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++){
        redBuffer[i - 25] = redBuffer[i];
        irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++){
        while (parSensor->available() == false) //do we have new data?
            parSensor->check(); //Check the sensor for new data

        redBuffer[i] = parSensor->getRed();
        irBuffer[i] = parSensor->getIR();
        parSensor->nextSample(); //We're finished with this sample so move to next sample
    }
    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}

// Return Oxygen saturation (SpO2)
int getSpO2(){
    return spo2;
}

// Check if SpO2 data is valid
bool validSpO2(){
    return validSPO2;
}

// Return Heartbeats per minute
float getBPM(){
    return heartRate;
}

// Check if BPM data is valid
bool validBPM(){
    return validHeartRate;
}

//===============MAX90614 IR TEMPERATURE SENSOR===============
Adafruit_MLX90614 *mlxPtr;

void initMLX(Adafruit_MLX90614 &mlx_sensor, TwoWire &i2cPort){
    mlxPtr = &mlx_sensor;
    Serial.print("Initializing temp sensor...");
    // Initialize sensor
    if (!mlxPtr -> begin(0x5A, &i2cPort)) {
        Serial.println("FAILED");
        for(;;);
    } else{
        Serial.println("SUCCESS");
    }
}

float getObjTemp(){
    return mlxPtr -> readObjectTempC();
}

float getAmbTemp(){
    return mlxPtr -> readAmbientTempC();
}

//===============FIREBASE REALTIME DATABASE===============
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth fbauth; // Firebase RTDB authentication variable
FirebaseConfig fbconfig; // Firebase RTDB config variable
bool signupOK = false; // variable to check if Firebase RTDB login success

void setupFirebaseRTDB(){
    //Firebase RTDB setup
    fbconfig.api_key = API_KEY;
    fbconfig.database_url = DATABASE_URL;

    if (Firebase.signUp(&fbconfig, &fbauth, "", "")){
        Serial.println("Firebase signup OK");
        signupOK = true;
    } else{
        Serial.printf("%s\n", fbconfig.signer.signupError.message.c_str());
    }

    Firebase.begin(&fbconfig, &fbauth);
    Firebase.reconnectWiFi(true);
}

void sendData(){  
    if (Firebase.ready()){
        //----------------UPLOAD DATA----------------
        //upload Heartrate
        if (Firebase.RTDB.setInt(&fbdo, "test/heart", getBPM())){
            Serial.println("Heartbeat rate uploaded to RTDB"); //print to Serial
        } else{
            Serial.println("Failed to upload Heartbeat rate");
            Serial.println("REASON: " + fbdo.errorReason()); //report error
        }

        //upload oxy sat
        if (Firebase.RTDB.setInt(&fbdo, "test/o2", getSpO2())){
            Serial.println("Oxygen saturation uploaded to RTDB"); //print to Serial
        } else{
            Serial.println("Failed to upload Oxygen saturation");
            Serial.println("REASON: " + fbdo.errorReason()); //report error
        }

        //upload temp
        if (Firebase.RTDB.setFloat(&fbdo, "test/temp", getObjTemp())){
            Serial.println("Temperature uploaded to RTDB"); //print to Serial
        } else{
            Serial.println("Failed to upload Temperature");
            Serial.println("REASON: " + fbdo.errorReason()); //report error
        }

        //upload weight
        if (Firebase.RTDB.setFloat(&fbdo, "test/weight", getDefWeight())){
            Serial.println("Weight uploaded to RTDB"); //print to Serial
        } else{
            Serial.println("Failed to upload Weight");
            Serial.println("REASON: " + fbdo.errorReason()); //report error
        }
        resetValidWeightFlag();
    }
}

//===============XIAOMI SMART SCALE 2 BLUETOOTH OPERATION PROTOTYPES===============
static BLEAdvertisedDevice* xiaomiScale;
static BLERemoteCharacteristic* remoteChar;
static bool goConnect = false;
static bool connected_flag = false;

float defWeight = 0.0; // definitive weight
float weightTable[3] = {0}; // weight table for determining the definitive weight
uint8_t weightDataCount = 0;
bool validDefWeight = false;
bool validSendWeight = false; 

//Base UUIDs
//Weight Scale service
static BLEUUID servUUID(xiaomiScaleServUUID);
static BLEUUID charUUID(xiaomiScaleCharUUID);
static BLEAddress MACaddr(xiaomiScaleMAC);

void resetValidWeightFlag(){
    validSendWeight = false;
}

bool validWeight(){
    return validDefWeight;
}

float getDefWeight(){
    return defWeight;
}

// Comparing the most recent weight data to determine the definitive weight
void compareWeightData(){
    bool flag01 = false,
         flag12 = false;

    if (weightTable[0] == weightTable[1]) flag01 = true;
    if (weightTable[1] == weightTable[2]) flag12 = true;

    if (flag01 && flag12) {
        validDefWeight = true;
        defWeight = weightTable[0];
    }
}

// Callback function for BLE characteristc data nofication
void charCallback(BLERemoteCharacteristic* remoteChr, uint8_t* pData, size_t length, bool isNotify){
    // Printing received data bytes for console debugging
    Serial.print("Data from scale received. Data length = ");
    Serial.print(length);
    Serial.print(". - Data bytes: ");
    // Print each byte of the data
    for (int i = 0; i < length; i++){
        Serial.print(pData[i]);
        Serial.print("  ");
    }
    Serial.println(" ");
    //End of console debugging

    // Calculate weight using the data received
    float weight = 0;
    weight = (pData[1] + pData[2] * 256) * 0.005;

    //Store the most recent data into the table
    if (weightDataCount >= 3){
      weightDataCount = 0;
    }
    weightTable[weightDataCount] = weight;
    weightDataCount++;

    compareWeightData();

    Serial.print("Three most recent recorded weight values: ");
    Serial.print(weightTable[0]); Serial.print("  ");
    Serial.print(weightTable[1]); Serial.print("  ");
    Serial.println(weightTable[2]);

    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.print(" Kg - ");

    if (validDefWeight){
      Serial.println("(Definitive)");
      validSendWeight = true;
    } else{
      Serial.println("(Provisional)");
    }
    validDefWeight = false;
}

// Search for our Smart Scale using MAC address (connectionMode 0)
void connectModeMAC(BLEAdvertisedDevice &BLEdevice){
    if (BLEdevice.getAddress() != MACaddr){
        Serial.print(".");
    } else{
        Serial.println("");
        Serial.print("Xiaomi Mi Smart Scale 2 weight scale found! ");
        Serial.println(BLEdevice.getAddress().toString().c_str());
        BLEDevice::getScan() -> stop();

        Serial.println("Stopping the scan and attempting to connect to the scale");
        xiaomiScale = new BLEAdvertisedDevice(BLEdevice);
        goConnect = true;
    }
}
// Search for our Smart Scale using device's name (connectionMode 1)
void connectModeName(BLEAdvertisedDevice &BLEdevice){
    if (BLEdevice.getName() != xiaomiScaleName){
        Serial.print(".");
    } else{
        Serial.println("");
        Serial.print("Xiaomi Mi Smart Scale 2 weight scale found! ");
        Serial.println(BLEdevice.getAddress().toString().c_str());
        BLEDevice::getScan() -> stop();

        Serial.println("Stopping the scan and attempting to connect to the scale");
        xiaomiScale = new BLEAdvertisedDevice(BLEdevice);
        goConnect = true;
    }
}

// Callback class for each advertising device in the vicinity
class deviceCallback:public BLEAdvertisedDeviceCallbacks{
  //Class is called every time an advertised device is found
  void onResult(BLEAdvertisedDevice advertisedDevice){
    // A random device found, check if this is our Smart Scale 2
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(servUUID)) {
        if (connectionMode == 0){
            connectModeMAC(advertisedDevice);
        } else{
            connectModeName(advertisedDevice);
        }
    } else{
      Serial.print(".");
    }
  } 
};

// Callback class for device events / device connections
class ClientCB:public BLEClientCallbacks{
  void onConnect(BLEClient* pclient){
  }
  void onDisconnect(BLEClient* pclient){
    Serial.println("Scale disconnected. Reconnecting...");
    connected_flag = false;
  }
};

bool initScaleConnection() {
    Serial.println("Establishing communications with the scale:");
    BLEClient* pClient = BLEDevice::createClient();
    Serial.println("BLE client created");
    pClient -> setClientCallbacks(new ClientCB());

    // Connect to the remove BLE Server.
    pClient -> connect(xiaomiScale);
    Serial.println("Connected to scale");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient -> getService(servUUID);
    if(pRemoteService == nullptr) {
      Serial.println("ERROR - Failed to find service");
      pClient -> disconnect();
      return false;
    }
    Serial.println("Service found");

    remoteChar = pRemoteService -> getCharacteristic(charUUID);
    if(remoteChar == nullptr) {
      Serial.print("ERROR - Failed to find characteristic");
      pClient -> disconnect();
      return false;
    }
    Serial.println("Characteristic found");
    Serial.println("Setting callback for characteristic notification");
    remoteChar -> registerForNotify(charCallback);
    return true;
}
BLEScan* BLEScanPtr = BLEDevice::getScan();
void initBLE(){
    Serial.println("Initializing BLE");
    
    BLEDevice::init("");
    
    BLEScanPtr -> setAdvertisedDeviceCallbacks(new deviceCallback());
    BLEScanPtr -> setInterval(1280);
    BLEScanPtr -> setWindow(200);
    //Set active scan
    BLEScanPtr -> setActiveScan(false);
    //Scan during 4 seconds
    BLEScanPtr -> start(4, false);
}

void connectToScale(){
    while (goConnect && !connected_flag){
        Serial.print(".");
        connected_flag = initScaleConnection();
        delay(200);
    }
    
    while (!goConnect){
        Serial.print(".");
        BLEScanPtr -> start(4, false);
        delay(200);
    }
}

bool turnOffBLE(){
    BLEDevice::deinit();
    btStop();
    esp_bt_controller_disable();
    
    return 0;
}