import random

from paho.mqtt.subscribe import simple
from paho.mqtt.publish import single


class MeasurementMsg:
    voltage: float = 0.0
    power: float = 0.0
    relay_flag: int = 0
    sth_flag:int = 0
    device_id: str = ''
    time: str = 0

    def __init__(self, msg: str)->None:
        infos = msg.strip('{').strip('}').split(',')
        self.voltage = float(infos[0])
        self.power = float(infos[1]) 
        self.relay_flag = int(infos[2])
        self.sth_flag = int(infos[3])
        self.device_id = infos[4]
        self.time = infos[5]

    def show(self):        
        print(f'time: {self.time}')
        print(f'voltage:{self.voltage}')
        print(f'power: {self.power}')
        print(f'relay: {self.relay_flag}')
        print(f'sth_flag: {self.sth_flag}')
        print(f'device_id: {self.device_id}')
        print('=======================================')
        


class MqttInfo:
    broker:str = 'lucky.7663handshake.co'
    port:int = 1883
    msg:str  = ''
    
    def __init__(self, device_id:str):
        self.topic = f'/ESP/{device_id}'
        self.client_id = f'mqtt-commander-{random.randint(0,100)}'
        self.username = 'inhandlebroker'
        self.password = 'inHandleElectric'

    
if __name__=='__main__':    
    device_id = input('please enter a string: ')#'520039400097325'    
    does_continue = True
    while does_continue:
        mqtt_info = MqttInfo(device_id=device_id)    
        topics = [mqtt_info.topic]
        m = simple(topics, hostname=mqtt_info.broker, retained=False, msg_count=1)
        measurement_msg = MeasurementMsg(m.payload.decode())
        measurement_msg.show()
        command = input('please insert mqtt command:')
        single(mqtt_info.topic, command, hostname=mqtt_info.broker)
        m = simple(topics, hostname=mqtt_info.broker, retained=False, msg_count=10)
        print('show to confirm that command is taken in action:')
        for a in m:            
            current_status = MeasurementMsg(a.payload.decode())
            current_status.show()

        does_continue = True if input('would like to retry?[Y/N]')=='Y' else False
    

    
