import subprocess
import time

if __name__=="__main__":
    command_to_use="mosquitto_sub"
    host_flag="-h"
    server_name="lucky.7663handshake.co"
    message_flag="-m"    
    topic_flag='-t'
    topic="/ActCode/"
    user_flag="-u"
    user="inhandlebroker"
    pwd_flag="-P"
    pwd="inHandleElectric"
    while True:        
        time.sleep(10)
        if cnt%2==1:
            message="a"        
        else:
            message="1"        
        subprocess.call([command_to_use,host_flag,server_name,message_flag,message,topic_flag,topic,user_flag,user,pwd_flag,pwd])
        cnt=cnt + 1