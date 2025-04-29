# HCMUT Health Station
This is an Arduino C++ code for the HCMUT Healt Station, using MLX90614 contactless IR sensor, MAX30102 particle sensor for pulse oximetry, BLE integration for the Xiaomi Smart Scale 2 and HerkuleX DRS-0201 smart servo control.

These are the following third-party libraries that are used in this project (with their respective dependecies):
* [Adafruit-MLX90614-Library](https://github.com/adafruit/Adafruit-MLX90614-Library)
* [MAX30102_by_RF](https://github.com/aromring/MAX30102_by_RF)
* [HerkulexServo](https://github.com/cesarvandevelde/HerkulexServo)

## System overview
This system was implemented using an ESP32-S3 N16R8, coded using PlatformIO, with the appropriate `platformio.ini` file.

You can interact with the system using a normal USB Serial COM port on your computer with a COM serial port monitor at **115200 baud rate** such as an Arduino/PlatformIO intergrated Serial monitor, with the included `TRANSMISSION_PROTOCOL.txt` file, which lists all of the commands that you can interact with the system.

Everything works well, so if your system isn't working, you should change the pinouts.

## Library wrappers
All of the operations are included and explained in their respective `_op` libraries. You can find all defines for pins, functions and includes in these wrapper libraries. All of the `_op` libraries can be found in their respective folders:
* `MAX30102_op` - handles the MAX30102 pulse oximetry sensor
* `MLX90614_op` - handles the MLX90614 IR sensor
* `scale_ble` - handles BLE operations for the Xiaomi Smart Scale 2
* `servo_op` - handles the servo operations
* `system_op` - handles system serial receives and returns

## Transmission protocol over Serial port
This is the table of the aforementioned `TRANSMISSION_PROTOCOL.txt` file. All of these incoming message handler can be found in the `system_op` library.

### Computer side requests:

#### Temperature (MLX90614 IR sensor):
|Command          | Description                                        |
| --------------- |:--------------------------------------------------:|
| `TEMP-GETOBJ`   | Get object's temperature from MLX90614 IR sensor   |
| `TEMP-GETAMB`   | Get ambient temperature from MLX90614 IR sensor    |

#### Pulse oximetry (MAX30102 particle sensor):
|Command          | Description                                |
| --------------- |:------------------------------------------:|
| `POX-GETDATA`   | Get data from MAX30102 particle sensor     |

#### Weight (Xiaomi Smart Scale 2 through BLE):
|Command                  | Description                                                 |
| ----------------------- |:-----------------------------------------------------------:|
| `SCALE-GETWEIGHT`       | Get definitive weight from Scale through BLE connection     |

#### Servo (HerkuleX DRS-0201):
|Command              | Description                                           |
| ------------------- |:-----------------------------------------------------:|
| `SERVO-MOVEUP`      | Turn the servo upwards                                |
| `SERVO-MOVEDN `     | Turn the servo downwards                              |
| `SERVO-STOP`        | Stop the servo's movement at the current position     |
| `SERVO-MOVEDEFAULT` | Turn the servo back to its default position           |
| `SERVO-GETANGLE`    | Get the servo's current angle                         |

### ESP32-S3 side returns:
|Command                                  | Description                                                         |
| --------------------------------------- |:-----------------------------------------------------------------:  |
| `ACK-[computer message]`                | Sent back to the Serial after every **valid** message received      |
| `NACK-SYNTAXERROR-[computer message]`   | Sent back to the Serial after every **invalid** message received    |
#### Temperature (MLX90614 IR sensor):
|Command                      | Description                                               |
| --------------------------- |:---------------------------------------------------------:|
| `TEMP-START`                | Starting MLX90614 IR sensor                               |
| `TEMP-STATUS-0`             | MLX90614 IR sensor currently not operational              |
| `TEMP-STATUS-1`             | MLX90614 IR sensor currently operational                  |
| `OBJ-[object temperature]`  | Returns object temperature of the command `TEMP-GETOBJ`   |
| `AMB-[ambient temperature]` | Returns ambient temperature of the command `TEMP-GETAMB`  |

#### Pulse oximetry (MAX30102 particle sensor):
|Command                            | Description                                                                  |
| --------------------------------- |:----------------------------------------------------------------------------:|
| `POX-START`                       | Starting MAX30102 particle sensor                                            |
| `POX-STATUS-0`                    | MAX30102 particle sensor currently not operational                           |
| `POX-STATUS-1`                    | MAX30102 particle sensor currently operational                               |
| `POX-[BPM number]-[SpO2 number]`  | Returns pulse oximetry data (BPM and SpO2) from the command `POX-GETDATA`    |

#### Weight (Xiaomi Smart Scale 2 through BLE):
|Command                    | Description                                                                       |
| --------------------------|:---------------------------------------------------------------------------------:|
| `SCALE-ERROR-NODATA`      | Scale has no weight data to return                                                |
| `SCALE-ERROR-NOSERVICE`   | Scale has the wrong Service UUID                                                  |
| `SCALE-ERROR-NOCHAR`      | Scale has the wrong Characteristic UUID                                           |
| `SCALE-STATUS-0`          | BLE connection to the scale is disconnected                                       |
| `SCALE-STATUS-1`          | BLE connection to the scale is connected                                          |
| `WEIGHT-[weight number]`  | Definitive weight (in kg) return from Scale from the command `SCALE-GETWEIGHT`    |

#### Servo (HerkuleX DRS-0201):
|Command                        | Description                                                       |
| ----------------------------- |:-----------------------------------------------------------------:|
| `SERVO-START`                 | Starting servo UART connection                                    |
| `SERVO-STATUS-1 `             | Servo UART port is opened                                         |
| `SERVO-STATUS-0`              | Servo UART port is closed                                         |
| `ANGLE-[current angle number]`| Returns the servo current angle from the command `SERVO-GETANGLE` |