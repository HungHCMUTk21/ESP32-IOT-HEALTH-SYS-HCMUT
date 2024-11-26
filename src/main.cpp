#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
#include "MAX30102_run.h"
#include <Adafruit_MLX90614.h>
#include <Firebase_ESP_Client.h>
#include <WiFi.h>

//-------------------------------BPM SPO2 CONFIG-------------------------------
// Create an instance of the MAX30105 class to interact with the sensor
MAX30105 particleSensor;

//-------------------------------MLX90614 CONFIG-------------------------------
// Create an MLX object
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
TwoWire mlxI2C = TwoWire(1); // initiate a second I2C port for the MLX sensor
void initMLX(); // function to initially setup the MLX sensor

//-------------------------------WI-FI CONFIG-------------------------------
#define WIFI_SSID     "Dung Hung" // change this based on the name of your wifi
#define WIFI_PASSWORD "19992003" // change to the password of your wifi

//-------------------------------FIREBASE RTDB CONFIG-------------------------------
// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert Firebase project API Key
#define API_KEY "AIzaSyCT51_XOB2G5ErMyPGR7uTo_LTWB8xWtEQ"
// Insert the RTDB URL
#define DATABASE_URL "https://platformio-demo-default-rtdb.asia-southeast1.firebasedatabase.app"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth fbauth; // Firebase RTDB authentication variable
FirebaseConfig fbconfig; // Firebase RTDB config variable
bool signupOK = false; // variable to check if Firebase RTDB login success
byte dataSendCount = 0;

void sendData(); // function to send data to RTDB
void initWifi(); // connect to Wifi
void setupFirebaseRTDB(); // initial Firebase RTDB setup

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  Serial.println("Initializing...");
 
  Wire.begin(17, 18);
  mlxI2C.begin(11, 12);

  initMLX();

  // Attempt to initialize the MAX30105 sensor. Check for a successful connection and report.
  init_MAX30102(particleSensor);
  setup_MAX30102();
  calibrateSignalRange();

  initWifi();
  setupFirebaseRTDB();
}

void loop() {
  updateIRvalue();
  dataUpdate();
  
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");

  // Output the current IR value, BPM, and averaged BPM to the serial monitor
  Serial.print("IR=");
  Serial.print(getIRvalue());
  Serial.print(", BPM=");
  Serial.print(getBPM());
  Serial.print(", BPM valid?=");
  Serial.print(validBPM());
  Serial.print(", SpO2=");
  Serial.print(getSpO2());
  Serial.print(", SpO2 valid?=");
  Serial.print(validSpO2());
 
  // Check if the sensor reading suggests that no finger is placed on the sensor
  if (!checkFinger())
    Serial.print(" No finger?");
 
  Serial.println();

  
  if (dataSendCount >= 5 && validBPM() && validSpO2() && checkFinger()){
    sendData();
    dataSendCount = 0;
    delay(1500);
  }
  dataSendCount++;
}

//==================FUNCTIONS SECTION==================
void initMLX(){
  Serial.print("Initializing temp sensor..");
  // Initialize sensor
  if (!mlx.begin(0x5A, &mlxI2C)) {
    Serial.println("FAILED");
    for(;;);
  } else {
    Serial.println("SUCCESS");
  }
}

void initWifi(){
  int count = 0;
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

  fbconfig.token_status_callback = tokenStatusCallback;
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
    if(Firebase.RTDB.setFloat(&fbdo, "test/temp", mlx.readObjectTempC())){
      Serial.println("Temperature uploaded to RTDB"); //print to Serial
    }else{
      Serial.println("Failed to upload Temperature");
      Serial.println("REASON: " + fbdo.errorReason()); //report error
    }
  }
}