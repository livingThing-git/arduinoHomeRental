import table_definition
import json
from mysql.connector import errorcode
import mysql.connector
from datetime import datetime
# GRANT ALL PRIVILEGES ON database_name.* TO 'username'@'localhost';
# https://chartio.com/resources/tutorials/how-to-grant-all-privileges-on-a-database-in-mysql/
class Mqtt_db:
    db_user = 'supong'
    db_pwd = 'Rattana1984!'
    host_ip = 'localhost'
    database = table_definition.DB_NAME
    table_name = list(table_definition.TABLES.keys())[0]
    base_topic = '/ESP/'    

#   {"voltage":227.70,"energy":0.13,"relay_status":0.00}
    def save_to_table(self,msg_topic, msg_payload):
        print(f'show:"topic:"{msg_topic},"payload"{msg_payload}' )    
        # datetime object containing current date and time
        now = datetime.now()
        
        print("now =", now)

        # dd/mm/YY H:M:S
        dt_string = now.strftime("%Y-%m-%d %H:%M:%S")
        print("date and time =", dt_string)	
        voltage = json.loads(msg_payload)["voltage"]
        energy = json.loads(msg_payload)["energy"]
        relay_status = True if json.loads(msg_payload)["relay_status"]==1.0 else False
        print(f'topic = {msg_topic}')
        print(f'voltage = {voltage}')
        print(f'energy = {energy}')
        print(f'relay_status = {relay_status}')
        con = self.connect_to_mysql()
        if(con):            
            cursor = con.cursor()
            if(not(self.check_table_exists(con, self.table_name))):
                # create_table
                self.create_table(cursor)
            is_table_exist = self.check_table_exists(con, self.table_name)
            # insert into table
            if is_table_exist:
                try:
                    sql = f"INSERT INTO {self.database}.{self.table_name} (imsi,voltage,energy,relay,time_stamp) VALUES (%s,%s,%s,%s,%s)"
                    insert_value = (msg_topic.replace(self.base_topic,""),str(voltage),str(energy),int(relay_status),str(dt_string))
                    cursor.execute(sql,insert_value)
                    con.commit()
                    print("1 record, inserted, ID:", cursor.lastrowid)
                except Exception as e:
                    print(e)
                
        else:
            print(f"cannot connect to database: {self.database}")
        con.close()        
        

    def check_table_exists(self,dbcon, tablename):
        dbcur = dbcon.cursor()
        stmt = f"SHOW TABLES LIKE '{tablename}'"
        dbcur.execute(stmt)
        if dbcur.fetchone():
            dbcur.close()
            return True

        dbcur.close()
        return False


    def create_table(self,cursor):        
        for table_name in table_definition.TABLES:
            table_description = table_definition.TABLES[table_name]
            try:
                print("Creating table {}: ".format(table_name), end='')
                cursor.execute(table_description)                
            except mysql.connector.Error as err:
                if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
                    print("already exists.")
                else:
                    print(err.msg)                
            else:
                print("OK")

    def connect_to_mysql(self):        
        try:
            cnx = mysql.connector.connect(user=self.db_user, 
                                            password = self.db_pwd, 
                                            host=self.host_ip, 
                                            database=self.database, 
                                            use_pure=True)
        except mysql.connector.Error as err:
            if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
                print("Something is wrong with your user name or password"+str(errorcode.ER_ACCESS_DENIED_ERROR))
            elif err.errno == errorcode.ER_BAD_DB_ERROR:
                print("Database does not exist")
            else:
                print(err)
        else:
            if(cnx):
                print(f"connect to database: {self.database} success")
                return cnx
            else:
                cnx.close()
                print(f"cannot connect to database: {self.database}")
                return None
    
            

        

    
