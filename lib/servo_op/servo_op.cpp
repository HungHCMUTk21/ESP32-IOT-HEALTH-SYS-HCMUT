#include "servo_op.h"

HardwareSerial   servo_serial(1);
HerkulexServoBus herkulex_bus(servo_serial);
HerkulexServo    my_servo(herkulex_bus, SERVO_ID);

bool moveStopFlag = false;
bool moveUpFlag = false;
bool moveDownFlag = false;

//position formula: 512 - [desired angle]°/0.325 = desired position.
//+90° (West) is 235; -90° (East) is 789
uint16_t servo_currentPos = SERVO_START_POS; // 512 is the default upright (pointing North / straight up) position.

// Internal functions
void servo_moveUp(){
    my_servo.setTorqueOn();
    my_servo.setPosition(servo_currentPos + SERVO_SPD, 25, HerkulexLed::Blue);
    if(servo_currentPos >= SERVO_MAX_UP_POS){
        servo_setMoveFlag(SERVO_MOVE_STOP);
    }
}

void servo_moveDown(){
    my_servo.setTorqueOn();
    my_servo.setPosition(servo_currentPos - SERVO_SPD, 25, HerkulexLed::Green);
    if(servo_currentPos <= SERVO_MAX_DN_POS){
        servo_setMoveFlag(SERVO_MOVE_STOP);
    }
}

void servo_moveStop(){
    my_servo.setPosition(servo_currentPos, 25, HerkulexLed::Yellow);
    my_servo.setBrake();
}

void servo_moveDefault(){
    my_servo.setTorqueOn();  // turn power on
    my_servo.setPosition(SERVO_START_POS, 50, HerkulexLed::Purple);
    delay(750);
    my_servo.setBrake();
}

void servo_move(){
    // Turn the servo upwards
    if(moveUpFlag){
        servo_moveUp();
    }

    // Turn the servo downwards
    if(moveDownFlag){
        servo_moveDown();
    }

    if(moveStopFlag){
        servo_moveStop();
    }
    delay(150);
}

// External, library accessible functions
void servo_init(){
    servo_serial.begin(115200, SERIAL_8N1, SERVO_PIN_RX, SERVO_PIN_TX);
    delay(500);
    
    servo_moveDefault();
    moveUpFlag = false;
    moveDownFlag = false;
    moveStopFlag = true;

    Serial.println("SERVO-STATUS-1");
    return;
}

void servo_updateSerial(){
    herkulex_bus.update();
    servo_currentPos = my_servo.getPosition();
    servo_move();
}

uint16_t servo_getPos(){
    return my_servo.getPosition();
}

void servo_setMoveFlag(uint8_t flagnum){
    switch (flagnum)
    {
    case SERVO_MOVE_UP:
        moveUpFlag = true;
        moveDownFlag = false;
        moveStopFlag = false;
        break;
    
    case SERVO_MOVE_DOWN:
        moveUpFlag = false;
        moveDownFlag = true;
        moveStopFlag = false;
        break;

    case SERVO_MOVE_STOP:
        moveUpFlag = false;
        moveDownFlag = false;
        moveStopFlag = true;
        break;

    case SERVO_MOVE_DEFAULT:
        servo_moveDefault();
        servo_setMoveFlag(SERVO_MOVE_STOP);
        break;
    
    default:
        moveUpFlag = false;
        moveDownFlag = false;
        moveStopFlag = false;
        break;
    }
}

uint8_t servo_isMoving(){
    return !moveStopFlag;
}