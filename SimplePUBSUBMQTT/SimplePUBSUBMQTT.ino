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
String address    = "lucky.7663handshake.co";               //Your IPaddress or mqtt server url
String serverPort = "1883";               //Your server port
String clientID   = "";               //Your client id < 120 characters
String topic      = "/ShowStatus/";               //Your topic     < 128 characters
String sub_topic  = "/ActCode/";
String payload    = "HelloWorld!";    //Your payload   < 500 characters
String username   = "inhandlebroker";               //username for mqtt server, username <= 100 characters
String password   = "inHandleElectric";               //password for mqtt server, password <= 100 characters 
String current_payload = "";
unsigned int subQoS       = 0;
unsigned int pubQoS       = 0;
unsigned int pubRetained  = 0;
unsigned int pubDuplicate = 0;

const long interval = 5000;           //time in millisecond 
unsigned long previousMillis = 0;
int cnt = 0;

const int relay1 = 12;

bool relay1_status=false;

AIS_SIM7020E_API nb;
void setup() {
  pinMode(relay1, OUTPUT);
  Serial.begin(115200);  
  nb.begin();
  clientID = clientID + nb.getIMSI();
  topic = topic + clientID;
  payload = clientID;
  setupMQTT();
  nb.setCallback(callback);   
  previousMillis = millis();            
}

void control_relay(int relay_id){
  if (relay_id==12){
    if(relay1_status){
      digitalWrite(relay_id, HIGH);
    }else{
      digitalWrite(relay_id, LOW);
    }
  }    
}

void loop() {   
  nb.MQTTresponse();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
        cnt++;
        connectStatus();
        String show_payload=payload + ((relay1_status)? " on": " off");
        nb.publish(topic,show_payload);   
        control_relay(relay1);
//      nb.publish(topic, payload, pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2, retained = 0 or 1, dup = 0 or 1
        previousMillis = currentMillis;  
  }
}

//=========== MQTT Function ================
void setupMQTT(){
  if(!nb.connectMQTT(address,serverPort,clientID,username,password)){ 
     Serial.println("\nconnectMQTT");
  }
    nb.subscribe(sub_topic,subQoS);
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
void callback(String &topic,String &payload, String &QoS,String &retained){
  Serial.println("-------------------------------");
  Serial.println("# Message from Topic \""+topic+"\" : "+nb.toString(payload));
  Serial.println("# QoS = "+QoS);
  if(retained.indexOf(F("1"))!=-1){
    Serial.println("# Retained = "+retained);
  }
  current_payload=nb.toString(payload);

  if(current_payload.length()==1){
    if(current_payload.toInt()==1)
      relay1_status=true;
    else 
      relay1_status=false;
  }
}
