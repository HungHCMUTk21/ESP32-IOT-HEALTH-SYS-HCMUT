#include "system_op.h"

String receivedMsg = "";  // Variable to store the complete message
String sendbackMsg = "";
uint8_t sendbackTimes = 0;
char cstr[256];

void serial_sendback(){
    Serial.println(sendbackMsg);
    // Empty the sendback char string
    sendbackMsg = "";
}

void process_string(){
    receivedMsg.trim();
    // Every command will return NUM_SAMPLES of samples.
    // Returning OBJECT TEMPERATURE results
    if (receivedMsg.equals("TEMP-GETOBJ")){
        sendbackMsg = "ACK-";
        sendbackMsg += receivedMsg;
        serial_sendback();
        for (sendbackTimes = 0; sendbackTimes < NUM_SAMPLES; sendbackTimes++){
            updateTempData();
            sendbackMsg = "OBJ-";
            sendbackMsg += getObjTemp();
            serial_sendback();
            delay(1000);
        }
        return;
    }

    // Returning AMBIENT TEMPERATURE results
    if (receivedMsg.equals("TEMP-GETAMB")){
        sendbackMsg = "ACK-";
        sendbackMsg += receivedMsg;
        serial_sendback();
        for (sendbackTimes = 0; sendbackTimes < NUM_SAMPLES; sendbackTimes++){
            updateTempData();
            sendbackMsg = "AMB-";
            sendbackMsg += getAmbTemp();
            serial_sendback();
            delay(1000);
        }
        return;
    }

    // Returning BPM and OXYGEN SATURATION results
    if (receivedMsg.equals("POX-GETDATA")){
        sendbackMsg = "ACK-";
        sendbackMsg += receivedMsg;
        serial_sendback();

        sendbackTimes = 0;
        while(sendbackTimes < NUM_SAMPLES){
            updatePoxData();
            if(validBPM() && validSPO()){
                sendbackMsg = "POX-";
                sendbackMsg += getBPM();
                sendbackMsg += "-";
                sendbackMsg += getSpO2();
                serial_sendback();
                sendbackTimes++;
            }
        }
        return;
    }

    if (receivedMsg.equals("SCALE-GETWEIGHT")){
        sendbackMsg = "ACK-";
        sendbackMsg += receivedMsg;
        serial_sendback();

        sendbackTimes = 0;
        while(sendbackTimes < NUM_SAMPLES){
            if(validWeight()){
                sendbackMsg = "WEIGHT-";
                sendbackMsg += getDefWeight();
                serial_sendback();
                sendbackTimes++;
            }
        }
        return;
    }

    sendbackMsg = "NACK-SYNTAXERROR-";
    sendbackMsg += receivedMsg;
    serial_sendback();
}

void serial_receive(){
    while (Serial.available()) {
        char incomingChar = Serial.read();  // Read each character from the buffer
        if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
            // Print the message
            process_string();
            // Clear the message buffer for the next input
            receivedMsg = "";
        } else if(incomingChar == '\b' && receivedMsg == ""){
            // There's nothing in the string but the user pressed backspace
            receivedMsg = "";
        } else {
            // Append the character to the message string
            receivedMsg += incomingChar;
        }
    }   
}