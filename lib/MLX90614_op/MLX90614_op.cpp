#include "MLX90614_op.h"

Adafruit_MLX90614 mlx_sensor; // MLX sensor object
TwoWire mlxI2C = TwoWire(1); // Separate I2C bus for the MLX

double t_obj = 0.0; // Object Temperature
double t_amb = 0.0; // Ambient Temperature

bool mlx_activated = false;

uint8_t initMLX90614(){
#ifdef MLX90614_DEBUG
    Serial.print("Initializing MLX90614 temperature sensor --> ");
    mlxI2C.begin(MLX90614_SDA, MLX90614_SCL);
    // Initialize sensor
    if (!mlx_sensor.begin(MLX90614_ADR, &mlxI2C)) {
        Serial.println("MLX90614 FAILED to start");
    } else{
        Serial.println("MLX90614 activated successfully.");
        mlx_activated = true;
    }
#else
    mlxI2C.begin(MLX90614_SDA, MLX90614_SCL);
    // Initialize sensor
    if (!mlx_sensor.begin(MLX90614_ADR, &mlxI2C)) {
    } else{
        mlx_activated = true;
    }
#endif
    return mlx_activated;
}

void updateTempData(){
    if(mlx_activated){
        t_obj = mlx_sensor.readObjectTempC();
        t_amb = mlx_sensor.readAmbientTempC();
    }
#ifdef MLX90614_DEBUG
        Serial.println("-----MLX90614 temp data-----");
      
        Serial.print("Object temp: ");
        Serial.println(t_obj, 1);
      
        Serial.print("Ambient temp: ");
        Serial.println(t_amb, 1);
      
        Serial.println("----------------------");
#endif
}

float getObjTemp(){
    return t_obj;
}

float getAmbTemp(){
    return t_amb;
}
