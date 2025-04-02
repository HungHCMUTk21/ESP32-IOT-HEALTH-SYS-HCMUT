#include "firebase_op.h"

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

bool uploadBPM(){
    if (Firebase.RTDB.setInt(&fbdo, "test/heart", getBPM())){
        Serial.println("Heartbeat rate uploaded to RTDB"); //print to Serial
        return true;
    } else{
        Serial.println("Failed to upload Heartbeat rate");
        Serial.println("REASON: " + fbdo.errorReason()); //report error
        return false;
    }
}

bool uploadSpO2(){
    if (Firebase.RTDB.setInt(&fbdo, "test/o2", getSpO2())){
        Serial.println("Oxygen saturation uploaded to RTDB"); //print to Serial
        return true;
    } else{
        Serial.println("Failed to upload Oxygen saturation");
        Serial.println("REASON: " + fbdo.errorReason()); //report error
        return false;
    }
}

bool uploadTemp(){
    if (Firebase.RTDB.setFloat(&fbdo, "test/temp", getObjTemp())){
        Serial.println("Temperature uploaded to RTDB"); //print to Serial
        return true;
    } else{
        Serial.println("Failed to upload Temperature");
        Serial.println("REASON: " + fbdo.errorReason()); //report error
        return false;
    }
}

// bool uploadWeight(){
//     if (Firebase.RTDB.setFloat(&fbdo, "test/weight", getDefWeight())){
//         Serial.println("Weight uploaded to RTDB"); //print to Serial
//     } else{
//         Serial.println("Failed to upload Weight");
//         Serial.println("REASON: " + fbdo.errorReason()); //report error
//         return false;
//     }
//     return true;
//     resetValidWeightFlag();
// }

void sendData(){  //----------------UPLOAD DATA----------------
    if (Firebase.ready()){
        //upload Heartrate; retry if failed.
        if(!uploadBPM()) uploadBPM();
        
        //upload oxy sat
        if(!uploadSpO2()) uploadSpO2();

        //upload temp
        if(!uploadTemp()) uploadTemp();

        //upload weight
        // if(!uploadWeight()) uploadWeight();
    }
}