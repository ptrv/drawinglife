#!/usr/bin/python
# sqlite3 example

from optparse import OptionParser
import sqlite3
import sys
import os.path

class Database:
    def __init__(self, dbpath):
        try:
            self.conn = sqlite3.connect(dbpath)
        except sqlite3.OperationalError: # Can't locate database file
            exit(1)
        self.cursor = self.conn.cursor()
        
    def get_users(self):
        users = []
        cmd = "SELECT * FROM user"
        self.cursor.execute(cmd)
        results = self.cursor.fetchall()
        for user in results:
            users.append(user[0])

        return users
    
    def get_timestamps(self, users):
        datetimes = []
        for user in users:
            timestamps = []
            cmd = """SELECT * FROM gpsdata WHERE user = "%s" ORDER BY datetime(time) ASC """ %(user)
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
        
        num_processed = 0
        for i in range(len(users)):
            for j in range(len(olddates[i])):
                cmd = """UPDATE gpsdata SET time = "%s" WHERE time = "%s" AND user = %d """ %(newdates[i][j],olddates[i][j],users[i] )
                self.cursor.execute(cmd)
                num_processed += 1
        
        self.conn.commit()
        return num_processed

    def close_handle(self):
        'Closes the connection to the database'
        self.conn.commit() # Make sure all changes are saved
        self.conn.close()

def main():
    usage = "usage: %prog 'database' 'new date'"
    parser = OptionParser(usage, version="%prog 0.1")
    (options, args) = parser.parse_args()
    if len(args) != 2:
        parser.error("\nplease enter the the database and the new date")
    
    dbpath, new_date = args
    
    if not(os.path.isfile(dbpath)):
        print "database file does not exist"
        exit(1)

    db = Database(dbpath)
    
    users = db.get_users()
    print "got %d users" % len(users)
    
    datetimes = db.get_timestamps(users)
    
    print "modifying database..."
    processed_entries = db.change_dates(users,datetimes, new_date)
    
    db.close_handle()
    print "processed %d entries" % processed_entries

if __name__ == '__main__':
    sys.exit(main())
