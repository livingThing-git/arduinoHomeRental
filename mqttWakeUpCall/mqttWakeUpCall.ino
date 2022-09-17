#include <Wire.h>
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
String address    = "lucky.7663handshake.co";               //Your IPaddress or mqtt server url
String serverPort = "1883";               //Your server port
String clientID   = "";               //Your client id < 120 characters
String topic      = "/esp/";               //Your topic     < 128 characters
String sub_topic  = "";
String payload    = "HelloWorld!";    //Your payload   < 500 characters
String username   = "inhandlebroker";               //username for mqtt server, username <= 100 characters
String password   = "inHandleElectric";               //password for mqtt server, password <= 100 characters 
int keepalive     = 60;               //keepalive time (second)
int version       = 4;                //MQTT veresion 3(3.1), 4(3.1.1)
int cleansession  = 0;                //cleanssion : 0, 1
int willflag      = 1;                //willflag : 0, 1
unsigned int subQoS       = 1;
unsigned int pubQoS       = 1;
unsigned int will_qos     = 0;
unsigned int will_retain  = 0;
unsigned int pubRetained  = 0;
unsigned int pubDuplicate = 0;

const long interval = 2000;           //time in millisecond 
unsigned long previousMillis = 3600000L;

AIS_SIM7020E_API nb;
  /*  This part is for setupAdvanceMQTT.
   *  parameter for will option (Last will and Testament)
   *  will_topic : String
   *  will_qos   : unsinged int : 0, 1, 2
   *  will_retain: unsinged int : 0, 1
   *  will_msg   : String */
String willOption = nb.willConfig("will_topic",will_qos,will_retain,"will_msg");
int cnt = 0;
const int relay1 = 12;
const int relay2 = 14;
const int relay3 = 33;
const int relay4 = 32;
const int led_status = 13;
const int pub_status = 2;
const int sub_status = 4;


bool relay1_status = false;
bool relay2_status = false;
bool relay3_status = false;
bool relay4_status = false;


void setup() {
    pinMode(relay1, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(relay2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(relay3, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(relay4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(led_status, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(pub_status, OUTPUT);
    pinMode(sub_status, OUTPUT);
    Serial.begin(115200);
    nb.begin();
    clientID = nb.getIMSI();
    topic = topic + clientID;
    sub_topic=sub_topic+topic+"/listen";
    setupMQTT();
    nb.setCallback(callback); 
    previousMillis = millis();                
}

String get_out_message(bool relay1_status, bool relay2_status, bool relay3_status, bool relay4_status){
    String out_message = "";
        if (!relay1_status && relay2_status && relay3_status && relay4_status){
            out_message = "0,1,1,1";
        }else if(relay1_status && relay2_status && relay3_status && relay4_status){
            out_message = "1,1,1,1";
        }else if(relay1_status && !relay2_status && relay3_status && relay4_status){
            out_message = "1,0,1,1";
        }else if(relay1_status && relay2_status && !relay3_status && relay4_status){
            out_message = "1,1,0,1";
        }else if(relay1_status && relay2_status && relay3_status && !relay4_status){
            out_message = "1,1,1,0";
        }else if(!relay1_status && !relay2_status && relay3_status && relay4_status){
            out_message = "0,0,1,1";
        }else if(!relay1_status && relay2_status && !relay3_status && relay4_status){
            out_message = "0,1,0,1";
        }else if(!relay1_status && relay2_status && relay3_status && !relay4_status){
            out_message = "0,1,1,0";
        }else if(relay1_status && !relay2_status && !relay3_status && relay4_status){
            out_message = "1,0,0,1";
        }else if(relay1_status && !relay2_status && relay3_status && !relay4_status){
            out_message = "1,0,1,0";
        }else if(relay1_status && relay2_status && !relay3_status && !relay4_status){
            out_message = "1,1,0,0";
        }else if(!relay1_status && !relay2_status && !relay3_status && relay4_status){
            out_message = "0,0,0,1";
        }else if(!relay1_status && relay2_status && !relay3_status && !relay4_status){
            out_message = "0,1,0,0";
        }else if(relay1_status && !relay2_status && !relay3_status && !relay4_status){
            out_message = "1,0,0,0";
        }else if(!relay1_status && !relay2_status && relay3_status && !relay4_status){
            out_message = "0,0,1,0";
        }else {
            out_message = "0,0,0,0";
        }
    return out_message;
}

void loop() {   
  nb.MQTTresponse();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
        // cnt++;
        connectStatus();        
        payload=get_out_message(relay1_status, relay2_status, relay3_status, relay4_status);
        nb.publish(topic, payload, pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2, retained = 0 or 1, dup = 0 or 1
        //digitalWrite(pub_status, HIGH);
        previousMillis = currentMillis; 
        digitalWrite(pub_status, LOW); 
  } 
  else {
        digitalWrite(pub_status, HIGH);
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
    nb.subscribe(sub_topic,subQoS);
  //  digitalWrite(sub_status, HIGH);
//    nb.unsubscribe(topic); 
}

void connectStatus(){
    if(!nb.MQTTstatus()){        
        if(!nb.NBstatus()){
            digitalWrite(led_status, LOW);
           Serial.println("reconnectNB ");
           nb.begin();           
        }        
       Serial.println("reconnectMQ ");
       setupMQTT();
    }   
    digitalWrite(led_status, HIGH);

}

void callback(String &topic,String &payload, String &QoS,String &retained){
  Serial.println("-------------------------------");
  Serial.println("# Message from Topic \""+topic+"\" : "+nb.toString(payload));
  String payload_input=nb.toString(payload);
  int cmd_char=0;
  if(topic==sub_topic){
    Serial.println("Topic from listen");
    cmd_char=payload_input.toInt();
  }
  
  // Switch on the LED if an 1 was received as first character
  if(payload_input.length()==1){
    if (cmd_char==1) {
      digitalWrite(relay1, LOW);   // Turn the LED on (Note that LOW is the voltage level
      relay1_status = false;
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else if (cmd_char==2){
      digitalWrite(relay1, HIGH);  // Turn the LED off by making the voltage HIGH  
      relay1_status = true;
    } else if (cmd_char==3){
      digitalWrite(relay2, LOW);  // Turn the LED off by making the voltage HIGH  
      relay2_status = false;
    } else if (cmd_char==4){
      digitalWrite(relay2, HIGH);  // Turn the LED off by making the voltage HIGH  
      relay2_status = true;
    } else if (cmd_char==5){
      digitalWrite(relay3, LOW);  // Turn the LED off by making the voltage HIGH  
      relay3_status = false;
    } else if (cmd_char==6){
      digitalWrite(relay3, HIGH);  // Turn the LED off by making the voltage HIGH  
      relay3_status = true;
    } else if (cmd_char==7){
      digitalWrite(relay4, LOW);  // Turn the LED off by making the voltage HIGH  
      relay4_status = false;
    } else if (cmd_char==8){
      digitalWrite(relay4, HIGH);  // Turn the LED off by making the voltage HIGH  
      relay4_status = true;
    } else {
      ;
    }
  }else {
    ;
  }

  Serial.println("# QoS = "+QoS);
  if(retained.indexOf(F("1"))!=-1){
    Serial.println("# Retained = "+retained);
  }
  
}
