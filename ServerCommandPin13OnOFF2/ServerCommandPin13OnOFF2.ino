/* Only command Pin13 from server */

#include<IOXhop_BC95.h>

IOXhop_BC95 nb;
Socket *soc = NULL;
int relay =13;
char c = '0';

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
      char text[100] = "Hello, NB-IoT !";
//      if (c=='1'){
//        if(digitalRead(relay)==LOW){
//          digitalWrite(relay, HIGH);
//          Serial.println("relay turned on!");            
//        }  
//        strcpy(text, "is on.");
//      }        
//      else{
//        if(digitalRead(relay)==HIGH){
//          digitalWrite(relay, LOW);
//          Serial.println("relay turned off!");            
//        }
//        strcpy(text, "is off.");
//      }
      if(digitalRead(relay)==LOW){
        Serial.print("status: Low\n");
        strcpy(text, "is off.");
        if(c=='1'){
          digitalWrite(relay, HIGH);
          Serial.println("relay turned on!\n");
          strcpy(text, "is on.");
          c = 'a';
        }
        if(c=='0'){
          digitalWrite(relay, LOW);
          Serial.println("relay turned off!\n");            
          strcpy(text, "is off.");
          c = 'b';
        }        
      }
      if(digitalRead(relay)==HIGH){
        Serial.print("status: High\n");
        strcpy(text, "is on.");
        if(c=='1'){
          digitalWrite(relay, HIGH);
          Serial.println("relay turned on!\n");
          strcpy(text, "is on.");
          c = 'a';
        }
        if(c=='0'){
          digitalWrite(relay, LOW);
          Serial.println("relay turned off!");            
          strcpy(text, "is off.");
          c  = 'b';
        }                
      }
        
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
