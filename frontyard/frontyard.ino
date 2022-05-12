/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

// permission denied:sudo chmod a+rw /dev/ttyUSB0

#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "rattana 2.4GHz";
const char* password = "Xyvxyv99486";
const char* mqtt_server = "lucky.7663handshake.co";
String username   = "inhandlebroker";//"livingthing_iot"; 
String server_password   = "inHandleElectric";//"thegang617";    
const int relay1 = 12;
const int relay2 = 14;
const int relay3 = 27;
const int relay4 = 26;
const int led_status = 13;

bool relay1_status = false;
bool relay2_status = false;
bool relay3_status = false;
bool relay4_status = false;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(relay1, LOW);   // Turn the LED on (Note that LOW is the voltage level
    relay1_status = false;
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else if ((char)payload[0] == '2'){
    digitalWrite(relay1, HIGH);  // Turn the LED off by making the voltage HIGH  
    relay1_status = true;
  } else if ((char)payload[0] == '3'){
    digitalWrite(relay2, LOW);  // Turn the LED off by making the voltage HIGH  
    relay2_status = false;
  } else if ((char)payload[0] == '4'){
    digitalWrite(relay2, HIGH);  // Turn the LED off by making the voltage HIGH  
    relay2_status = true;
  } else if ((char)payload[0] == '5'){
    digitalWrite(relay3, LOW);  // Turn the LED off by making the voltage HIGH  
    relay3_status = false;
  } else if ((char)payload[0] == '6'){
    digitalWrite(relay3, HIGH);  // Turn the LED off by making the voltage HIGH  
    relay3_status = true;
  } else if ((char)payload[0] == '7'){
    digitalWrite(relay4, LOW);  // Turn the LED off by making the voltage HIGH  
    relay4_status = false;
  } else if ((char)payload[0] == '8'){
    digitalWrite(relay4, HIGH);  // Turn the LED off by making the voltage HIGH  
    relay4_status = true;
  } else {
    ;
  }
}

void reconnect() {
  // Loop until we're reconnected
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),username.c_str(),server_password.c_str())) {
      
      Serial.println("connected");
      // Once connected, publish an announcement...
      
      client.publish("outTopic", "hello");
      // ... and resubscribe
      client.subscribe("inTopic");      
    } else {
      
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
        
}

void setup() {
  pinMode(relay1, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relay2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relay3, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relay4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(led_status, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    digitalWrite(led_status, LOW);
    reconnect();
  }
  digitalWrite(led_status, HIGH);
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    String out_message = "";
      if (relay1_status && relay2_status && relay3_status && relay4_status){
        out_message = "1,1,1,1";
      }else if(!relay1_status && relay2_status && relay3_status && relay4_status){
        out_message = "0,1,1,1";
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
    snprintf (msg, MSG_BUFFER_SIZE, "%s", out_message.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}