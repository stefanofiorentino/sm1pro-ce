#!/usr/bin/python
# sudo apt-get install libmysqlclient-dev
# sudo pip install MySQLdb
import MySQLdb

db = MySQLdb.connect(host="localhost",    # your host, usually localhost
                     user="root",         # your username
                     passwd="admin",      # your password
                     db="Data")           # name of the data base

# you must create a Cursor object. It will let
#  you execute all the queries you need
cur = db.cursor()
cur2 = db.cursor()
sensor_info_cursor = db.cursor()

sensor_info_cursor.execute("""
    SELECT DISTINCT 
        (select `vtmp`.`sensor_type` from `sensor_type` `vtmp` where `vtmp`.`no` = `pm`.`sensor_type`) AS `sensor_type`, 
        (select `vtmp`.`sensor_name` from `sensor_name` `vtmp` where `vtmp`.`no` = `pm`.`sensor_name`) AS `sensor_name`, 
        (select `vtmp`.`data_type` from `data_type` `vtmp` where `vtmp`.`no` = `pm`.`data_type`) AS `data_type` 
    FROM 
        DataM1 pm
    WHERE 
        timestamp IN (SELECT no FROM timestamp WHERE timestamp >= NOW() - INTERVAL 1 DAY)
    """)
sensor_list = {}
for row in sensor_info_cursor.fetchall():
    data_type = row[2]
    sensor_name = row[1]
    sensor_type = row[0]
    if sensor_type not in sensor_list:
        sensor_list[sensor_type] = {}
    if sensor_name not in sensor_list[sensor_type]:
        sensor_list[sensor_type][sensor_name] = {}
    if data_type not in sensor_list[sensor_type][sensor_name]:
        sensor_list[sensor_type][sensor_name][data_type]={}
    print row[0], row[1], row[2]

# Use all the SQL you like
cur.execute("SELECT * FROM timestamp WHERE timestamp >= NOW() - INTERVAL 1 DAY")

# print all the first cell of all the rows
for row in cur.fetchall():
    cur2.execute("""
        select 
            (select `vtmp`.`timestamp` from `timestamp` `vtmp` where `vtmp`.`no` = `pm`.`timestamp`) AS `timestamp`,
            (select `vtmp`.`sensor_type` from `sensor_type` `vtmp` where `vtmp`.`no` = `pm`.`sensor_type`) AS `sensor_type`,
            (select `vtmp`.`sensor_name` from `sensor_name` `vtmp` where `vtmp`.`no` = `pm`.`sensor_name`) AS `sensor_name`,
            (select `vtmp`.`data_type` from `data_type` `vtmp` where `vtmp`.`no` = `pm`.`data_type`) AS `data_type`,
            `pm`.`massimo` AS `massimo`
        from `DataM1` `pm` where timestamp = %s""" % row[0]
    )

    timestamp = {}
    output_string = ("",),
    for row2 in cur2.fetchall():
        timestamp = (row2[0].isoformat(),)
        sensor_type = row2[1]
        sensor_name = row2[2]
        data_type = row2[3]
        output_string = output_string + (sensor_type+", "+sensor_name+", "+data_type+", ",),
        value = str(row2[4])
        if sensor_type in sensor_list:
            if sensor_name in sensor_list[sensor_type]:
                if data_type in sensor_list[sensor_type][sensor_name]:
                    output_string = output_string + (value+", ", ),
    print timestamp + (output_string,)

    # print timestamp+", ",
    # for k in xrange(0, len(sensor_type)):
    #     print sensor_type[k]+", "+sensor_name[k]+", "+data_type[k]+", "+value[k]+", ",
    # print ""
    
db.close()