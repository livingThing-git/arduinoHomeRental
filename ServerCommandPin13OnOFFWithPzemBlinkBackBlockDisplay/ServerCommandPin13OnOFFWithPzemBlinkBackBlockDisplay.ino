/* Only command Pin13 from server */
//IOXhop_BC95.h = communication library
#include<IOXhop_BC95.h>
#include <PZEM004Tv30.h>
//Display Library
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>



// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

IOXhop_BC95 nb;
Socket *soc = NULL;
String control_server_ip = "34.87.171.56";
String duetodue_server_ip = "34.126.80.253";
String simIMEI;
int relay =10;
int inPin = 7;
int dbPin = 6;
char c = '0';
String bill = "0";
String kiloWattPerHour = "0";
/* Use software serial for the PZEM
 * Pin 11 Rx (Connects to the Tx pin on the PZEM)
 * Pin 12 Tx (Connects to the Rx pin on the PZEM)
*/
PZEM004Tv30 pzem(11, 12);

unsigned long timeSend = 0, timeSend2 = 0;

void lcdDisplay(){
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Bill: " + bill);
  lcd.setCursor(0, 1);
  lcd.print("kw/hour" + kiloWattPerHour);
}

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcdDisplay();
  // Setup module
  nb.easySetup(true);
  // Socket
  Serial.println("Create new socket");
  if (soc = nb.CreateUDPSocket(9766)) {
    Serial.print("Socket id is ");
    Serial.println(soc->getID());
  } else {
    Serial.println("Create fail !");
  }
  simIMEI = nb.getIMEI();
  soc->onReceiver([](String ip, unsigned long port, byte* data, int len) {
    char message[len + 1];
    memcpy(message, data, len);
    message[len] = 0;
    
    Serial.print("Receiver[" + ip + ":" + String(port) + ": " + String(len) + "]:");
    Serial.print("message:" + String(message));
    if (strcmp(message,"1")==0 && strcmp(ip.c_str(),control_server_ip.c_str())==0){      
      c = '1';
      bill = "1";
      lcdDisplay(); 
    }else if(strcmp(message,"0")==0 && strcmp(ip.c_str(), control_server_ip.c_str())==0) {      
      c = '0';
      bill = "0";
      lcdDisplay();
    }else if(strcmp(message,"10")==0) {      
      c = '3';
    }else if(strcmp(message,"-1")==0 && strcmp(ip.c_str(), control_server_ip.c_str())==0) {
      c = '2';
    }else if(strstr(message, "bill:") != NULL && strcmp(ip.c_str(), control_server_ip.c_str())==0) {
      bill = "hello";
      lcdDisplay();      
    }
  });
  pinMode(relay,OUTPUT);
  pinMode(inPin, INPUT);
  pinMode(dbPin,OUTPUT);
}

void loop() {
  if (soc) {
    if ((millis() - timeSend) >= 4700) { // every 5 sec
      timeSend = millis();
      String text = simIMEI;
      int state = 0;
      if(digitalRead(inPin)==LOW){
        Serial.print("status: Low\n");        
        state = 0;
        if(c=='1'){
          digitalWrite(relay, HIGH);
          Serial.println("relay turned on!\n");
          //strcpy(text, "is on.");
          c = 'a';
          state = 1;
        }
        if(c=='0'){
          digitalWrite(relay, LOW);
          Serial.println("relay turned off!\n");            
          //strcpy(text, "is off.");
          c = 'b';
          state = 0;
        }        
      }
      if(digitalRead(inPin)==HIGH){
        Serial.print("status: High\n");
        state = 1;
        if(c=='1'){
          digitalWrite(relay, HIGH);
          Serial.println("relay turned on!\n");
          //strcpy(text, "is on.");
          c = 'a';
          state = 1;
        }
        if(c=='0'){
          digitalWrite(relay, LOW);
          Serial.println("relay turned off!");            
          //strcpy(text, "is off.");
          c  = 'b';
          state = 0;
        }                
      }

      if(c=='2'){
        pzem.resetEnergy();
        c  = 'b';
      }
      if(c=='3'){
        digitalWrite(dbPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(150);                       // wait for a second
        digitalWrite(dbPin, LOW);    // turn the LED off by making the voltage LOW
        delay(150);                       // wait for a second
        c = 'b';
      }
            
      float voltage = pzem.voltage();      
      float current = pzem.current();      
      float power = pzem.power();            
      float energy = pzem.energy();           
      float frequency = pzem.frequency();
      float pf = pzem.pf();
      String relay = "off";
      if(state==1){
        relay = "on";
      }
      String deviceData = (text+","+String(voltage) + "," + String(current) + "," +
      String(power) + "," + String(energy) + "," + String(frequency) + "," + String(pf) + "," + String(relay));
      
//      Serial.print("deviceData: ");
//      Serial.println(deviceData);
      kiloWattPerHour = ""+String(energy);
      lcdDisplay();      

                               
      Serial.print("Send ");
      if (soc->send(duetodue_server_ip, 9766, deviceData.c_str(), strlen(deviceData.c_str()))) {
        Serial.println("OK");
      } else {
        Serial.println("fail !");
      }
      
    }
  }
  nb.loop();
}
