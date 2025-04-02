#include "servo_op.h"

HardwareSerial servo_serial(1);

HerkulexServoBus herkulex_bus(servo_serial);
HerkulexServo    my_servo(herkulex_bus, SERVO_ID);

unsigned long last_update = 0;
unsigned long now = 0;
bool toggle = false;

void initServo() {
  servo_serial.begin(115200, SERIAL_8N1, PIN_SW_RX, PIN_SW_TX);
  my_servo.setTorqueOn();  // turn power on
}

void loop() {
  herkulex_bus.update();
  my_servo.getPosition();
  now = millis();

  if ( (now - last_update) > 1000) {
    // called every 1000 ms
    if (toggle) {
      // move to -90° over a duration of 560ms, set LED to green
      // 512 - 90°/0.325 = 235
      my_servo.setPosition(235, 50, HerkulexLed::Blue);
    } else {
      // move to +90° over a duration of 560ms, set LED to blue
      // 512 + 90°/0.325 = 789
      my_servo.setPosition(1000, 50, HerkulexLed::Purple);
    }

    last_update = now;
    toggle = !toggle;
  }
}