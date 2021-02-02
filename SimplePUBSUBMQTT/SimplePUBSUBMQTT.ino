

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

//#include "AT_SIM7020E.h"
//#include "Magellan_SIM7020E.h"
//#include "board.h"
String address    = "35.240.140.227";               //Your IPaddress or mqtt server url
String serverPort = "1883";               //Your server port
String clientID   = "pong";               //Your client id < 120 characters
String topic      = "/ESP/";               //Your topic     < 128 characters
String payload    = "HelloWorld!";    //Your payload   < 500 characters
String username   = "livingthing_iot";          //username for mqtt server, username <= 100 characters
String password   = "thegang617";  //password for mqtt server, password <= 100 characters 
unsigned int subQoS       = 0;
unsigned int pubQoS       = 0;
unsigned int pubRetained  = 0;
unsigned int pubDuplicate = 0;


const long interval = 5000;           //time in millisecond 
unsigned long previousMillis = 0;
int cnt = 0;

AIS_SIM7020E_API nb;
//AT_SIM7020E std_nb;
void setup() { 
  Serial.begin(115200);
  nb.begin();  
  setupMQTT();
  nb.setCallback(callback);
  topic = topic +nb.getIMEI();
  Serial.print("Show topic: " + topic);   
  previousMillis = millis();            
}
void loop() {   
  nb.MQTTresponse();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
        cnt++;
        connectStatus();
        nb.publish(topic,payload+String(cnt));      
//      nb.publish(topic, payload, pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2, retained = 0 or 1, dup = 0 or 1
        previousMillis = currentMillis;  
  }
}

//=========== MQTT Function ================
void setupMQTT(){
  if(!nb.connectMQTT(address,serverPort,clientID,username,password)){ 
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
void callback(String &topic,String &payload, String &QoS,String &retained){
  Serial.println("-------------------------------");
  Serial.println("# Message from Topic \""+topic+"\" : "+nb.toString(payload));
  Serial.println("# QoS = "+QoS);
  if(retained.indexOf(F("1"))!=-1){
    Serial.println("# Retained = "+retained);
  }
}
