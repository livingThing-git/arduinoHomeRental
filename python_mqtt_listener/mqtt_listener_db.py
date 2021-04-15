import paho.mqtt.client as mqtt
from Mqtt_db import Mqtt_db

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("/ESP/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):            
    print(msg.topic+" "+str(msg.payload))   
    db = Mqtt_db()
    db.save_to_table(msg.topic,msg.payload)
    
if __name__ == '__main__':
    client = mqtt.Client()    
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("34.87.54.201", 1883, 60)
    # Blocking call that processes network traffic, dispatches callbacks and
    # handles reconnecting.
    # Other loop*() functions are available that give a threaded interface and a
    # manual interface.
    client.loop_forever()