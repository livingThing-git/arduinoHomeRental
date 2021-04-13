from __future__ import print_function

import mysql.connector
from mysql.connector import errorcode

DB_NAME = 'duetodue'

TABLES = {}
TABLES['arduino_payload'] = (
    "CREATE TABLE `arduino_payload` ("
    "  `imsi` VARCHAR(15) NOT NULL,"
    "  `voltage` DOUBLE(5,2) NOT NULL,"
    "  `energy` DOUBLE(6,2) NOT NULL,"
    "  `relay` BOOLEAN NOT NULL,"
    "  `time_stamp` TIMESTAMP NOT NULL,"    
    "  CONSTRAINT PK_Id PRIMARY KEY (imsi,time_stamp)"
    ") ENGINE=InnoDB")