/* Only command Pin13 from server */

#include<IOXhop_BC95.h>
#include <PZEM004Tv30.h>

IOXhop_BC95 nb;
Socket *soc = NULL;
int relay =13;
char c = '0';
/* Use software serial for the PZEM
 * Pin 11 Rx (Connects to the Tx pin on the PZEM)
 * Pin 12 Tx (Connects to the Rx pin on the PZEM)
*/
PZEM004Tv30 pzem(11, 12);

unsigned long timeSend = 0, timeSend2 = 0;

void setup() {
  Serial.begin(9600);
  // Setup module
  nb.easySetup(true);
  // Socket
  Serial.println("Create new socket");
  if (soc = nb.CreateUDPSocket(9765)) {
    Serial.print("Socket id is ");
    Serial.println(soc->getID());
  } else {
    Serial.println("Create fail !");
  }
  soc->onReceiver([](String ip, unsigned long port, byte* data, int len) {
    char message[len + 1];
    memcpy(message, data, len);
    message[len] = 0;
    Serial.print("Receiver[" + ip + ":" + String(port) + ": " + String(len) + "]:");
    Serial.println(message);
    if (strcmp(message,"1")==0){      
      c = '1';
    }else {      
      c = '0';
    }
  });
  pinMode(relay,OUTPUT);
}

void loop() {
  if (soc) {
    if ((millis() - timeSend) >= 5000) { // every 5 sec
      timeSend = millis();
      char text[100] = "Hello, ";
      int state = 0;
      if(digitalRead(relay)==LOW){
        Serial.print("status: Low\n");
        //strcpy(text, "is off.");
        state = 0;
        if(c=='1'){
          digitalWrite(relay, HIGH);
          //Serial.println("relay turned on!\n");
          //strcpy(text, "is on.");
          c = 'a';
          state = 1;
        }
        if(c=='0'){
          digitalWrite(relay, LOW);
          //Serial.println("relay turned off!\n");            
          //strcpy(text, "is off.");
          c = 'b';
          state = 0;
        }        
      }
      if(digitalRead(relay)==HIGH){
        Serial.print("status: High\n");
//        strcpy(text, "is on.");
        state = 1;
        if(c=='1'){
          digitalWrite(relay, HIGH);
          //Serial.println("relay turned on!\n");
          //strcpy(text, "is on.");
          c = 'a';
          state = 1;
        }
        if(c=='0'){
          digitalWrite(relay, LOW);
          //Serial.println("relay turned off!");            
          //strcpy(text, "is off.");
          c  = 'b';
          state = 0;
        }                
      }
      
      char voltageStr[6];
      float voltage = pzem.voltage();
      sprintf(voltageStr, "%f", voltage);
      char voltageComma[7] = ",";
      strcat(voltageComma,voltageStr);

      char currentStr[6];
      float current = pzem.current();
      sprintf(currentStr, "%f", current);
      char currentComma[7] = ",";
      strcat(currentComma, currentStr);

      char powerStr[6];
      float power = pzem.power();
      sprintf(powerStr, "%f", power);
      char powerComma[7] = ",";
      strcat(powerComma, powerStr);

      char energyStr[6];
      float energy = pzem.energy();
      sprintf(energyStr, "%f", energy);
      char energyComma[7] = ",";
      strcat(energyComma, energyStr);

      char frequencyStr[6];
      float frequency = pzem.frequency();
      sprintf(frequencyStr, "%f", frequency);
      char frequencyComma[7] = ",";
      strcat(frequencyComma, frequencyStr);

      char pfStr[6];
      float pf = pzem.pf();
      sprintf(pfStr, "%f", pf);
      char pfComma[7] = ",";
      strcat(pfComma, pfStr);

      strcat(text, voltageComma);
      strcat(text, currentComma);
      strcat(text, powerComma);
      strcat(text, energyComma);
      strcat(text, frequencyComma);
      strcat(text, pfComma);
      if(state==0)
        strcat(text, "on");
      else
        strcat(text, "off"); 
                    
      Serial.print("Send ");
      if (soc->send("34.126.80.253", 9765, text, strlen(text))) {
        Serial.println("OK");
      } else {
        Serial.println("fail !");
      }
      
    }
  }
  nb.loop();
}
