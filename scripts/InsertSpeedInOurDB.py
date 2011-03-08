#!/usr/bin/python

from optparse import OptionParser
import sqlite3
import sys
import os.path
# these two modules are mine - make sure they're in the same folder
from GPSutils import *
from timeConversions import *

class Database:
    def __init__(self, dbpath):
        try:
            self.conn = sqlite3.connect(dbpath)
        except sqlite3.OperationalError: # Can't locate database file
            exit(1)
        self.cursor = self.conn.cursor()

    def get_gpsdata(self, sql):
        """
        """
        gpsdata = []
        self.cursor.execute(sql)
        results = self.cursor.fetchall()
        for line in results:
            gpsdata.append(line)
        return gpsdata

    def get_segment(self, user):
        """
        """
        cmd = "select segment from gpsdata where user = %s group by segment order by datetime(time)" % user
        segments = []
        self.cursor.execute(cmd)
        results = self.cursor.fetchall()
        for line in results:
            segments.append(line[0])
        return segments


    def get_users(self):
        users = []
        cmd = "SELECT * FROM user"
        self.cursor.execute(cmd)
        results = self.cursor.fetchall()
        for user in results:
            users.append(user[0])
        return users
        
    def get_users2(self, newdate):
        users = []
        cmd = "SELECT a.userid AS userid FROM user AS a JOIN gpsdata AS b ON (a.userid = b.user) where b.time not like '"+newdate+"%' group by userid"
        self.cursor.execute(cmd)
        results = self.cursor.fetchall()
        for user in results:
            users.append(user[0])
        return users
    
    def get_timestamps(self, users):
        datetimes = []
        for user in users:
            timestamps = []
            cmd = "SELECT * FROM gpsdata WHERE user = '%s' ORDER BY datetime(time) ASC " % (user)
            self.cursor.execute(cmd)
            results = self.cursor.fetchall()
            for row in results:
                timestamps.append(row[5])
            datetimes.append(timestamps)
        return datetimes

    def change_dates(self, users, olddates, newdate):
        newdates = []
        for olddate in olddates:
            date_new = []
            for date in olddate:
                index_t = date.find("T", 0)
                time = date[index_t:len(date)]
                new_datetime = newdate + time
                date_new.append(new_datetime)
            newdates.append(date_new)
        print 'calculated new dates'
        num_processed = 0
        for i in range(len(users)):
            print 'processing user %d' % users[i]
            for j in range(len(olddates[i])):
                cmd = 'UPDATE gpsdata SET time = "%s" WHERE time = "%s" AND user = %d '%(newdates[i][j],olddates[i][j],users[i] )
                self.cursor.execute(cmd)
                num_processed += 1
        self.conn.commit()
        return num_processed

    def insert_speed(self, gpsdataid, speed):
        """
        """
        cmd = "UPDATE gpsdata SET speed = %f WHERE gpsdataid = %d" % (speed, gpsdataid)
        self.cursor.execute(cmd)      

    def commit_changes(self):
        """
        
        Arguments:
        - `self`:
        """
        self.conn.commit()

    def fix_null_values(self):
        """
        """
        cmd = "UPDATE gpsdata SET speed = 0.0 WHERE speed IS NULL"
        self.cursor.execute(cmd)
                

    def close_handle(self):
        'Closes the connection to the database'
        self.conn.commit() # Make sure all changes are saved
        self.conn.close()

def main():
    usage = "usage: %prog 'database'"
    parser = OptionParser(usage, version="%prog 0.1")
    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.error("\nplease enter the database path")    

    #user, dbpath = args
    dbpath = args[0]
    
    if not(os.path.isfile(dbpath)):
        print "database file does not exist"
        exit(1)

    db = Database(dbpath)

    users = db.get_users()

    for user in users:
        segments = db.get_segment(user)

        for segment in segments:
            #print segment
            sql = "select gpsdataid, latitude, longitude, time from gpsdata where user = %s and segment = %d order by datetime(time)" % (user, segment)
            gpsdata = db.get_gpsdata(sql)
            for i in range(len(gpsdata)-1):
                gpsdataid1, lat1, lon1, time1 = gpsdata[i]
                gpsdataid2, lat2, lon2, time2 = gpsdata[i+1]
                dist = EarthDistance((lat1, lon1), (lat2, lon2))
                try:
                    timeoffset = StringConvert(time2)-StringConvert(time1)
                except:
                    print "StringConvert error, gpsdataid: %d" % (gpsdataid2)
                kph = (dist/timeoffset.seconds)*3.6
                #print i, gpsdataid, lat1, lon1, lat2, lon2, kph
                db.insert_speed(gpsdataid2,kph)

    db.commit_changes()            

    db.fix_null_values()
    
    db.close_handle()
    

if __name__ == '__main__':
    sys.exit(main())
