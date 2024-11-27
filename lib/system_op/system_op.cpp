#include "system_op.h"

//---------VARIABLES---------
// An instance of the MAX30105 class to interact with the sensor
MAX30105* parSensor;

long IRvalue; // Infrared value from the sensor
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data

//---------GENERAL FUNCTIONS---------
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
    byte sampleAverage = 4;     //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;           //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 400;       //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;       //Options: 69, 118, 215, 411
    int adcRange = 8192;        //Options: 2048, 4096, 8192, 16384

    parSensor->setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}

void updateIRvalue(){
    IRvalue = parSensor->getIR();
}

long getIRvalue(){ // Read the infrared value from the sensor
    return IRvalue;
}

bool checkFinger(){
    if (IRvalue < 50000)
        return false;
    else
        return true;
}

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

    if(WiFi.status() != WL_CONNECTED){
        Serial.println("Cannot connect to WiFi, please reset.");
    }else{
        Serial.println();
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        delay(2000);
    }
}

//---------MLX30102 PULSE OXYMETER FUNCTIONS---------
int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

//Calibrate the signal range using initial data (PUT INSIDE SETUP)
void calibrateSignalRange(){
    bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

    //read the first 100 samples, and determine the signal range
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

    //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
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

//---------MAX90614 IR TEMPERATURE SENSOR FUNCTIONS---------
Adafruit_MLX90614 *mlxPtr;

void initMLX(Adafruit_MLX90614 &mlx_sensor, TwoWire &i2cPort){
    mlxPtr = &mlx_sensor;
    Serial.print("Initializing temp sensor...");
    // Initialize sensor
    if (!mlxPtr -> begin(0x5A, &i2cPort)) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
}

float getObjTemp(){
    return mlxPtr -> readObjectTempC();
}

float getAmbTemp(){
    return mlxPtr -> readAmbientTempC();
}

//-------------------------------FIREBASE RTDB CONFIG-------------------------------
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
  }
  else{
    Serial.printf("%s\n", fbconfig.signer.signupError.message.c_str());
  }

  Firebase.begin(&fbconfig, &fbauth);
  Firebase.reconnectWiFi(true);
}

void sendData(){  
  if(Firebase.ready() && validBPM() && validSpO2() && checkFinger()){
    //----------------UPLOAD DATA----------------
    //upload Heartrate
    if(Firebase.RTDB.setInt(&fbdo, "test/heart", getBPM())){
      Serial.println("Heartbeat rate uploaded to RTDB"); //print to Serial
    }else{
      Serial.println("Failed to upload Heartbeat rate");
      Serial.println("REASON: " + fbdo.errorReason()); //report error
    }

    //upload oxy sat
    if(Firebase.RTDB.setInt(&fbdo, "test/o2", getSpO2())){
      Serial.println("Oxygen saturation uploaded to RTDB"); //print to Serial
    }else{
      Serial.println("Failed to upload Oxygen saturation");
      Serial.println("REASON: " + fbdo.errorReason()); //report error
    }

    //upload temp
    if(Firebase.RTDB.setFloat(&fbdo, "test/temp", getObjTemp())){
      Serial.println("Temperature uploaded to RTDB"); //print to Serial
    }else{
      Serial.println("Failed to upload Temperature");
      Serial.println("REASON: " + fbdo.errorReason()); //report error
    }
  }
}