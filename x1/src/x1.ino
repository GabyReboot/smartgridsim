//Run system and application loops on separate threads so they do not run sequentially
SYSTEM_THREAD(ENABLED);

//logging stuff to terminal so I know what the hecks happening (or at least i look like it)
SerialLogHandler logHandler(LOG_LEVEL_ALL); 

//tell the system we have a toggle led function later 
int togglePower(String command);
int resetAll(String empty);

char myID[5]="X1";

void setup() {
    //Tell the system 
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    
    //enable output to console
    Serial.begin();
    // turn D5 on
    digitalWrite(D4, HIGH);
    
    Mesh.subscribe("PowerNet", commsHandler);
    //register togled function to the cloud
    Particle.function("toglPwr", togglePower);
    Particle.function("reset", resetAll);

}

void loop() {
    Serial.printf("\n\n----------------------------------\n");
    Serial.print("City:Xenon1\n");
    if (analogRead(A5) > 2000 && analogRead(A4) > 3500){
        Mesh.publish("PowerNet", "RESET");
    } else if (analogRead(A5) >= 2000 && analogRead(A5) <= 3900) {
        
        Serial.print("Power is ON\nSelf-generated power levels: ");
        Serial.print(analogRead(A5));
        Serial.printf("\n----------------------------------\n");
        //Mesh.publish("SolCity", "POWER GOOD");
    } else if ((analogRead(A5) > 3900 || analogRead(A5) < 2000) && analogRead(A4) < 2000) {  
        Serial.print("Power is OFF\nOutsourced power levels: ");
        Serial.print(analogRead(A4));
        Serial.print("\n\n----------------------------------\n");
        noPower();
    } else if (analogRead(A5) < 2000 && analogRead(A4) > 3500){
        Serial.print("\nPower supplied by a gracious neighbor");
        Serial.print("\nOutsourced power levels: ");
        Serial.print(analogRead(A4));
    } else {
        Serial.println("\n\nOops! Please pay $5 BTC for me to work again! \n\n");
    }
    delay(2000); 
}

void noPower(){
    Mesh.publish("PowerNet", "X1 - POWER OFF");
    Serial.print("\nAlert sent to mesh network.");
    do {
        Serial.printf("\nWaiting for power...\n");
        delay(3000);
        } while((analogRead(A5) > 3900 || analogRead(A5) < 2000) && analogRead(A4) < 3500); 
}
    

    
//it blinks.    
void blink(){
    delay(500);
    digitalWrite(D7, LOW);
    delay(500);
    digitalWrite(D7, HIGH);
    delay(500);
    digitalWrite(D7, LOW);
    delay(500);
    digitalWrite(D7, HIGH);
    delay(500);
    digitalWrite(D7, LOW);
    delay(500);
    digitalWrite(D7, HIGH);
    delay(500);
    digitalWrite(D7, LOW);
}

//reroute power
void reroute(char who[5]){

    //signal that POWER OFF message was recieved and power is being rerouted
    blink();
    //route power
    delay(1000);
    //do not route if already supplying power to someone else
    if (strstr(who, "A1")){
        digitalWrite(D5,HIGH);
        Serial.print("\n\n______________________________");
        Serial.printf("\nREROUTING POWER TO %s", who);
        //Mesh.publish("PowerNet", "X1 - SUPPLYING POWER");
        Serial.print("\n______________________________");
    } else if (strstr(who, "X2")){
        digitalWrite(D6, HIGH);
        Serial.print("\n\n______________________________");
        Serial.printf("\nREROUTING POWER TO %s", who);
        //Mesh.publish("PowerNet", "X1 - SUPPLYING POWER");
        Serial.print("\n______________________________");
    }
}


void commsHandler(const char *event, const char *data){

    //char a1[5] = "X1";
    char x2[5] = "X2";
    char a1[5] = "A1";
    char lowPwr[12] = "POWER OFF";
    char rts[5]= "RTS";
    char cts[5]= "CTS";

        //handle recieved low power messages
    if (strstr(data, lowPwr)){
        if (strstr(data, a1)){
            char *target[5] = {a1};
            Serial.printf("\n%s Has No Power!! \n", *target);
            delay((random(0,41)/random(10,16))*10000);
            Mesh.publish("PowerNet", "X1 - RTS");
        } else if (strstr(data, x2)){
            char *target[5] = {x2};
            Serial.printf("\n%s Has No Power!! \n", *target);
            delay((random(0,41)/random(10,16))*10000);
            Mesh.publish("PowerNet", "X1 - RTS");
        } else { 
        }
    //handle CTS messages
   } else if (strstr(data, cts)){
        if (strstr(data, myID) && strstr(data,x2)){
          reroute(x2);
        } else if (strstr(data, myID) && strstr(data,a1)){
          reroute(a1);
        }
    //handle RTS messages
    } else if (strstr(data, rts)){
        // parse who sent RTS
         if (strstr(data, a1)){
             //if no one has already sent power
            if (analogRead(A4)<2000){
                //CTS
                 Mesh.publish("PowerNet", "X1 - A1 CTS");
             }
        } else if (strstr(data, x2)){
            if (analogRead(A4)<20000){
                 Mesh.publish("PowerNet", "X1 - X2 CTS");
             }
        } else { 
        } 
    //handle reset messages
    } else if (strstr(data, "RESET")){
        digitalWrite(D5,LOW);
        digitalWrite(D6,LOW);
    } else {}
    //slow down flow
    delay(3000);
}


//cloud commands

int togglePower(String command){
     if (command=="on") {
        digitalWrite(D4,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(D4,LOW);
        return 0;
    }
    else {
        return -1;
    }
}

int resetAll(String empty){
    Mesh.publish("PowerNet", "RESET");
    digitalWrite(D4, HIGH);
    return 0;
}
