import table_definition

from mysql.connector import errorcode
import mysql.connector
# GRANT ALL PRIVILEGES ON database_name.* TO 'username'@'localhost';
# https://chartio.com/resources/tutorials/how-to-grant-all-privileges-on-a-database-in-mysql/
class Mqtt_db:
    db_user = 'supong'
    db_pwd = 'Rattana1984!'
    host_ip = 'localhost'
    database = table_definition.DB_NAME
    table_name = list(table_definition.TABLES.keys())[0]

    def show(self,msg_topic, msg_payload):
        print(f'show:"topic:"{msg_topic},"payload"{msg_payload}' )
        con = connect_to_mysql()
        if(con):
            print(f"connect to database: {self.database} success")
            # if(NOT(check_table_exists(con, tablename))):
            #create_table
                    
            #insert into table
        else:
            print(f"cannot connect to database: {self.database}")
        con.close()
        

    def check_table_exists(self,dbcon, tablename):
        dbcur = dbcon.cursor()
        dbcur.execute(f"""
            SELECT COUNT(*)
            FROM {self.database}.{self.tablename}
            WHERE table_name = '{0}'
            """.format(tablename.replace('\'', '\'\'')))
        if dbcur.fetchone()[0] == 1:
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
        cursor.close()        

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
            

        

    
