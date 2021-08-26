#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
/*
 Supported DEVIO NB-DEVKIT I Board 
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |
*/
#include "AIS_SIM7020E_API.h"
// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int PzemPin = 32;
const int RelayPin = 15;
String address    = "125.26.235.193";//"35.185.180.247";               //Your IPaddress or mqtt server url
String serverPort = "1883";               //Your server port
String clientID   = "";               //Your client id < 120 characters
String topic      = "/ESP/";               //Your topic     < 128 characters
String payload    = "";    //Your payload   < 500 characters
String username   = "pi";//"livingthing_iot";               //username for mqtt server, username <= 100 characters
String password   = "raspberry";//"thegang617";               //password for mqtt server, password <= 100 characters 
int keepalive     = 60;               //keepalive time (second)
int version       = 3;                //MQTT veresion 3(3.1), 4(3.1.1)
int cleansession  = 1;                //cleanssion : 0, 1
int willflag      = 1;                //willflag : 0, 1
unsigned int subQoS       = 1;
unsigned int pubQoS       = 1;
unsigned int will_qos     = 1;
unsigned int will_retain  = 1;
unsigned int pubRetained  = 1;
unsigned int pubDuplicate = 0;
//test to change 20210315 10:51
const long interval = 5000;           //time in millisecond 
unsigned long previousMillis = 0;
bool is_pzem_reset = false;
AIS_SIM7020E_API nb;
  /*  This part is for setupAdvanceMQTT.
   *  parameter for will option (Last will and Testament)
   *  will_topic : String
   *  will_qos   : unsinged int : 0, 1, 2
   *  will_retain: unsinged int : 0, 1
   *  will_msg   : String */
String willOption = nb.willConfig("will_topic",will_qos,will_retain,"will_msg");
int cnt = 0;
SoftwareSerial NodeSerial(12, 14); // RX | TX
String total_unit = "InHandle";

void setup() {
 
  pinMode(12, INPUT);
  pinMode(14, OUTPUT);
  pinMode(PzemPin, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  Serial.begin(115200);
  NodeSerial.begin(57600);
  nb.begin();
  clientID = nb.getIMSI();
  topic = topic + clientID;
  setupMQTT();
  nb.setCallback(callback);
  previousMillis = millis();
  lcd.begin();
}

String added_payload(String metric_name, float metric_value, bool is_end ){
  String msg = (metric_value>=0)? "\"" + metric_name + "\":" + String(metric_value) : "";
  return (!is_end)? msg+",": msg;
}

String get_payload(float voltage,                    
                   float energy,
                   float relay_status) {
   return "{" + 
          added_payload("voltage",voltage, false) +
          added_payload("energy", energy,false) +
          added_payload("relay_status", relay_status,true) +
           "}"  ;                                                   
 }

void loop() {
  nb.MQTTresponse();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    while (NodeSerial.available() > 0) {   
      float voltage = NodeSerial.parseFloat();
      float energy = NodeSerial.parseFloat();
      int relay_status = digitalRead(RelayPin);
      if (NodeSerial.read() == '\n')
      {
        previousMillis = currentMillis;
        connectStatus();        
        Serial.print("voltage: ");    
        Serial.print(voltage); 
        Serial.print("\tenergy: ");
        Serial.println(energy);
        Serial.print("\trelay_status: ");
        Serial.println(String(relay_status));
                
        payload = get_payload(voltage,energy,relay_status);
        nb.publish(topic, payload, pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2, retained = 0 or 1, dup = 0 or 1        
//        lcd.setCursor(0, 0);
//        lcd.print("net:");
//        lcd.setCursor( 5, 0);      
        lcd.setCursor( 4, 0);      
        //please change total_unit here after server code finish
        lcd.print(total_unit);
//        lcd.setCursor(12, 0);
//        lcd.print("unit");
        lcd.setCursor( 0, 1);
        lcd.print("eng:");
        lcd.setCursor( 5, 1);
        lcd.print(energy);            
        lcd.setCursor(12, 1);
        lcd.print("unit");
        if(is_pzem_reset){
          digitalWrite(PzemPin, HIGH);
          cnt++;
          Serial.print("cnt:");
          Serial.println(cnt);
          if(cnt>49){
            Serial.print("cnt:");
            Serial.println(cnt);
            digitalWrite(PzemPin, LOW);
            cnt = 0;
            is_pzem_reset = false;
          }
        }
        
        
        // nb.MQTTresponse();
      }
    }
  }
}

//=========== MQTT Function ================

void setupMQTT(){
  /*  parameter for setup advance MQTT
   *  version    : MQTT veresion 3(3.1), 4(3.1.1)
   *  cleanssion : 0, 1
   *  willflag   : 0, 1
   *  **PLEASE ADD PARAMETER IN willOption** */
   if(!nb.connectAdvanceMQTT(address,serverPort,clientID,username,password,keepalive,version,cleansession,willflag,willOption)){
     Serial.println("\nconnectMQTT");
    }
    nb.subscribe(topic,subQoS);
  //  nb.unsubscribe(topic); 
}

void connectStatus(){
    if(!nb.MQTTstatus()){
        if(!nb.NBstatus()){
           Serial.println("reconnectNB ");
           nb.begin();
        }
       Serial.println("reconnectMQ ");
       setupMQTT();
    }   
}

void callback(String &topic,String &callback_payload, String &QoS,String &retained){
  Serial.println("-------------------------------");
  Serial.println("# Message from Topic \""+topic+"\" : "+nb.toString(callback_payload));
  //pzem callback json =  {"command_type":"pzem","value":"a"} or {"command_type":"pzem","value":"b"}
  //Relay callback json = {"command_type":"relay","value":"c"}or {"command_type":"relay","value":"d"}
  //total_unit callback json = {"command_type":"total_unit","value":"xxxxx"}
  String callback_command = nb.toString(callback_payload);
  char alpha_cmd  = callback_command[0];
  switch(alpha_cmd){
     case 'a'://reset pzem to high
      is_pzem_reset = true;
      digitalWrite(PzemPin, HIGH);
      break;
     case 'b'://reset pzem to low
     is_pzem_reset = false;
      digitalWrite(PzemPin, LOW);
      break;
     case 'c'://reset relay to high
      digitalWrite(RelayPin,HIGH);
      break;
     case 'd'://reset relay to low
      digitalWrite(RelayPin, LOW);
      break;
     default:
     ;     
  }
  
  Serial.println("# QoS = "+QoS);
  if(retained.indexOf(F("1"))!=-1){
    Serial.println("# Retained = "+retained);
  }
}
