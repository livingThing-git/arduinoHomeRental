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
const String RESET = "reset";
const String RELAY = "relay";
const int PzemPin = 32;
const int RelayPin = 15;
String address    = "35.240.140.227";               //Your IPaddress or mqtt server url
String serverPort = "1883";               //Your server port
String clientID   = "";               //Your client id < 120 characters
String topic      = "/ESP/";               //Your topic     < 128 characters
String payload    = "";    //Your payload   < 500 characters
String username   = "livingthing_iot";               //username for mqtt server, username <= 100 characters
String password   = "thegang617";               //password for mqtt server, password <= 100 characters 
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
const long interval = 1000;           //time in millisecond 
unsigned long previousMillis = 0;

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
String total_unit = "9977.0";
bool is_reset_pzem = true;
bool is_relay_on = true;
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
  String msg = (metric_value>0)? "\"" + metric_name + "\":" + String(metric_value) : "";
  return (!is_end)? msg+",": msg;
}

String get_payload(float voltage,                    
                   float energy) {
   return "{" + 
          added_payload("voltage",voltage, false) +
          added_payload("energy", energy,true) +                       
           "}"  ;                                                   
 }

void loop() {
  nb.MQTTresponse();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    while (NodeSerial.available() > 0) {   
      float voltage = NodeSerial.parseFloat();
      float energy = NodeSerial.parseFloat();
      if (NodeSerial.read() == '\n')
      {
        Serial.print("voltage: ");    
        Serial.print(voltage); 
        Serial.print("\tenergy: ");
        Serial.println(energy);
        cnt++;
        connectStatus();
        payload = get_payload(voltage,energy);
        nb.publish(topic, payload, pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2, retained = 0 or 1, dup = 0 or 1
        previousMillis = currentMillis;
        lcd.setCursor(0, 0);
        lcd.print("net:");
        lcd.setCursor( 5, 0);
        lcd.print(total_unit);
        lcd.setCursor(12, 0);
        lcd.print("unit");
        lcd.setCursor( 0, 1);
        lcd.print("eng:");
        lcd.setCursor( 5, 1);
        lcd.print(energy);            
        lcd.setCursor(12, 1);
        lcd.print("unit");
        
        
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
  String callback_command = nb.toString(callback_payload);
  is_reset_pzem = (callback_command==RESET) ? true : false;//just for demo
  is_relay_on = (callback_command==RELAY) ? true : false;//just for demo
  if(is_reset_pzem){
    digitalWrite(PzemPin, HIGH);    
  }else{
    digitalWrite(PzemPin, LOW);
  }
  if(is_relay_on){
    digitalWrite(RelayPin, HIGH);    
  }else{
    digitalWrite(RelayPin, LOW);
  }
  
  Serial.println("# QoS = "+QoS);
  if(retained.indexOf(F("1"))!=-1){
    Serial.println("# Retained = "+retained);
  }
}
