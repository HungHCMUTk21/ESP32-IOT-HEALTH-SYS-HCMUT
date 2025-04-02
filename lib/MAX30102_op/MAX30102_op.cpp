#include "MAX30102_op.h"

uint32_t  aun_ir_buffer[BUFFER_SIZE]; //infrared LED sensor data
uint32_t  aun_red_buffer[BUFFER_SIZE];  //red LED sensor data
float     old_n_spo2;  // Previous SPO2 value
bool      max_activated = false; // Sensor activated and connected flag

uint8_t initMAX30102(){
    pinMode(MAX30102_INT, INPUT);  //pin D10 connects to the interrupt output pin of the MAX30102
    //initialize the MAX30102
    Serial.print("Initializing MAX30102 Pulse Oxymeter sensor --> ");
    if(maxim_max30102_init(MAX30102_SDA, MAX30102_SCL)){
        Serial.println("MAX30102 activated successfully.");
        max_activated = true;
        old_n_spo2 = 0.0;
    }else{
        Serial.println("MAX30102 FAILED to start.");
    }
    return max_activated;
}

float n_spo2, ratio, correl;  //SPO2 value
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
int32_t i;

void updatePoxData(){
    if(max_activated){
        for(i = 0; i < BUFFER_SIZE; i++)
        {
          while(digitalRead(MAX30102_INT) == 1);  //wait until the interrupt pin asserts
          maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO
        }
        //calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using Robert's method
        rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl); 
#ifdef MAX30102_DEBUG
        Serial.println("-----MAX30102 data-----");
      
        Serial.print("SPO2: ");
        Serial.println(n_spo2);
      
        Serial.print("BPM: ");
        Serial.println(n_heart_rate, DEC);
      
        Serial.println("----------------------");
#endif
        old_n_spo2 = n_spo2;
    }
}

int getBPM(){
    return n_heart_rate;
}

float getSpO2(){
    return n_spo2;
}

bool validBPM(){
    return ch_hr_valid;
}

bool validSPO(){
    return ch_spo2_valid;
}