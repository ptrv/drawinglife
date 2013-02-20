#!/usr/bin/env python
#-*- coding:utf-8 -*-
"""
Started Tue 12 Feb 2013 21:48:02 CET

Based on EtreeExperiments.py

Using the lxml etree parser, import trackpoints to a database

TODO: Just discovered (Sun 17 Feb 2013 19:00:10 CET) that there is the
osgeo ogr GPX driver which is capable of parsing GPX files and
expressing the lat lons as geometries in wkt and wkb which will be the
answer to the LINESTRING conundrum of how to make a python tuple of
floats with a space in between

1. in extractpointsosr(), also collect a tracklines list like the
current lines list except for the tracklines table and write a
function to import this

Copyright 2013 Daniel Belasco Rogers
<http://planbperformance.net/dan>

To create a suitable database:
In spatialite-gui, click on 'Create a&New (empty)SQLite DB' (second tool item)

To prepare the tables needed for this script (populating the users
table with 'Daniel' and 'Sophia'), run the following sql (save as an
sql file and open it in spatialite-gui with 'Execute SQL script'())

-- Creates tables for homebrew gps repository.
-- Tables:
-- users
-- files
-- trackpoints
-- tracklines

PRAGMA foreign_keys=OFF;

BEGIN TRANSACTION;

CREATE TABLE users (
user_uid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
username TEXT NOT NULL,
UNIQUE (username));

INSERT INTO 'users' VALUES (1,'Daniel');
INSERT INTO 'users' VALUES (2,'Sophia');

CREATE TABLE files (
file_uid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
filename TEXT NOT NULL,
md5hash TEXT NOT NULL,
date_entered TEXT NOT NULL,
first_timestamp TEXT NOT NULL,
last_timestamp TEXT NOT NULL,
user_uid INTEGER UNSIGNED NOT NULL,
FOREIGN KEY (user_uid)
REFERENCES users (user_uid),
UNIQUE (md5hash));

CREATE TABLE trackpoints (
trackpoint_uid INTEGER PRIMARY KEY AUTOINCREMENT,
--track_id INTEGER,
trkseg_id INTEGER,
trksegpt_id INTEGER,
ele DOUBLE NOT NULL,
utctimestamp TEXT NOT NULL,
name TEXT,
cmt TEXT,
desc TEXT,
course DOUBLE,
speed DOUBLE,
file_uid INTEGER UNSIGNED NOT NULL,
user_uid INTEGER UNSIGNED NOT NULL,
FOREIGN KEY (file_uid)
REFERENCES file (file_uid),
FOREIGN KEY (user_uid)
REFERENCES users (user_uid),
UNIQUE (utctimestamp, user_uid));

SELECT AddGeometryColumn('trackpoints', 'geom', 4326, 'POINT', 'XY');

CREATE TABLE tracklines (
trkline_uid INTEGER PRIMARY KEY AUTOINCREMENT,
trksegid_fm_trkpts INTEGER,
name TEXT,
cmt TEXT,
desc TEXT,
timestamp_start TEXT NOT NULL,
timestamp_end TEXT NOT NULL,
link TEXT,
type TEXT,
'length(m)' DOUBLE,
'time(sec)' DOUBLE,
'speed(kph)' DOUBLE,
points INTEGER,
file_uid INTEGER UNSIGNED NOT NULL,
user_uid INTEGER UNSIGNED NOT NULL,
FOREIGN KEY (file_uid)
REFERENCES file (file_uid),
FOREIGN KEY (user_uid)
REFERENCES users (user_uid),
UNIQUE (timestamp_start, user_uid));

SELECT AddGeometryColumn('tracklines', 'geom', 4326, 'LINESTRING', 'XY');

COMMIT;

This SQL might also be useful - it drops all the tables in the
database so you can start again

BEGIN TRANSACTION;
DROP TABLE tracklines;
DROP TABLE trackpoints;
DROP TABLE files;
DROP TABLE users;
COMMIT;

"""

import sys
import os.path
from pyspatialite import dbapi2 as spatialite
import hashlib
from datetime import datetime
from optparse import OptionParser
try:
    from lxml import etree
except ImportError:
    print '*' * 48
    print 'This script needs the python module lxml to work'
    print 'lxml is not currently installed'
    print 'You can get it by typing:'
    print 'sudo apt-get install python-lxml'
    print '*' * 48
    sys.exit(2)
try:
    from osgeo import ogr
except ImportError:
    print '*' * 48
    print 'This script needs the python module osgeo to work'
    print
    print 'You can get it by typing:'
    print 'sudo easy_install GDAL'
    print 'Or install manually from here http://pypi.python.org/pypi/GDAL/'
    print '*' * 48
    sys.exit(2)

DEFAULTDB = "~/dansdocs/databases/emptytest.sqlite"


def parsegpx(filepath):
    """
    parse a gpx file using etree's xml parser and return the root
    object and namespace
    """
    parser = etree.XMLParser(remove_blank_text=True)
    doc = etree.parse(filepath, parser)
    root = doc.getroot()
    ns = root.nsmap[None]
    return root, ns


def checkfile(filepath):
    """
    Checks if file exists at location
    TODO check if it's a GPX file - How?
    """
    if not(os.path.isfile(filepath)):
        print '*' * 48
        print "%s is not a file" % filepath
        print "please retry with a file path e.g. ~/currentGPS/dan2012/1_originalfiles/2012-D-01.gpx"
        print '*' * 48
        sys.exit(2)
    return


def parseargs():
    """
    parse command line arguments and define options etc
    """
    usage = "usage: %prog /path/to/gpx/file.gpx"
    optparser = OptionParser(usage, version="%prog 0.1")
    optparser.add_option("-d",
                         "--database",
                         dest="dbasepath",
                         metavar="FILE",
                         default=DEFAULTDB,
                         help="Define path to alternate database")
    (options, args) = optparser.parse_args()
    if len(args) != 2:
        message = """
Please define input GPX file and user number
1=Daniel 2=Sophia
e.g. python gpx2spatialite ~/Dropbox/planb/currentGPS/dan2013/2013-D-01.gpx 1"""
        # optparser.error("\nplease define input GPX file and user")
        optparser.error("\n" + message)
    filepath, user = args
    checkfile(filepath)
    dbpath = os.path.expanduser(options.dbasepath)
    return filepath, user, dbpath


def getmd5(filepath):
    """
    generates md5 hexdigests from files (necessary for the file table)

    Stolen from:
    http://stackoverflow.com/questions/1131220/get-md5-hash-of-a-files-without-open-it-in-python
    user http://stackoverflow.com/users/8563/nathan-feger
    """
    md5 = hashlib.md5()
    with open(filepath, 'rb') as f:
        for chunk in iter(lambda: f.read(128 * md5.block_size), b''):
            md5.update(chunk)
    return md5.hexdigest()


def enterfile(filepath, cursor, user, firsttimestamp, lasttimestamp):
    """
    Enters the file in the files database table for future tracking
    """
    # define fields required for file insert
    filename = os.path.split(filepath)[1]
    md5hash = getmd5(filepath)
    date_entered = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # build sql
    sql = "INSERT INTO files (file_uid, filename, "
    sql += "md5hash, date_entered, first_timestamp, last_timestamp, user_uid) "
    sql += "VALUES ("
    sql += "NULL, '%s', '%s', '%s', '%s', '%s', %d);" % (filename, md5hash, date_entered, firsttimestamp, lasttimestamp, user)

    # execute insert checking whether the file has already been
    # entered
    try:
        cursor.execute(sql)
    except spatialite.IntegrityError as e:
        print "*" * 43
        print "File already entered. Please try again"
        print e
        print "*" * 43
        sys.exit(2)

    return


def getcurrentfileid(cursor):
    """
    query the database for the id number of the file just entered
    """
    sql = "SELECT seq FROM sqlite_sequence "
    sql += "WHERE name = 'files'"
    cursor.execute(sql)
    file_uid = cursor.fetchone()[0]

    return file_uid


def getlasttrkseg(cursor):
    """
    query the database for the last trkseg_id
    """
    sql = "SELECT trkseg_id FROM trackpoints "
    sql += "ORDER BY trkseg_id DESC LIMIT 1"
    cursor.execute(sql)
    try:
        lasttrkseg = cursor.fetchone()[0]
    except TypeError:
        # there are no tracks in the database
        lasttrkseg = -1
    return lasttrkseg


def getdatasource(driver, filepath):
    """
    open the GPX file using the driver and return the object as datasource
    """
    # open the data source
    datasource = driver.Open(filepath, 0)
    if datasource is None:
        print 'Could not open %s' % filepath
        sys.exit(1)
    return datasource


def timetodatetime(timestring):
    """
    """
    outputdatetime = datetime.strptime(timestring, '%Y/%m/%d %H:%M:%S+00')
    return outputdatetime


def enterpoints(cursor, user, trkpts):
    """
    Enters points in the spatially enabled 'trackpoints' table
    trackpoint columns: trackpoint_uid, trkseg_id, trksegpt_id, ele,
    utctimestamp, name, cmt, desc, course, speed, file_uid, user_uid,
    geom

    trkpts = trkseg_id, trksegpt_id, ele, time, lat, lon

    TODO: get the last trkseg_id from the database and use this to
    offset the trkseg_id of the input rows so that eqach trkseg_id in
    the database is unique and consecutive by file input
    """
    # get just entered file id from file just entered
    file_uid = getcurrentfileid(cursor)

    # get last trackpoint id
    lasttrkseg = getlasttrkseg(cursor)
    #print file_uid, lasttrkseg

    # iterate through lines, forming an sql and updating as we
    # go. Execute many wasn't possible because of forming strings for
    # geometry instead of triggering function
    for line in trkpts:
        trkseg_id, trksegpt_id, ele, time, geom = line
        trkseg_id = lasttrkseg + 1
        sql = "INSERT INTO trackpoints (trkseg_id, trksegpt_id, "
        sql += "ele, utctimestamp, file_uid, user_uid, geom) VALUES "
        sql += "(%d, %d, %f, '%s', %d, %d, GeomFromText('%s', 4326))" % (trkseg_id, trksegpt_id, ele, time, file_uid, user, geom)
        cursor.execute(sql)
    #print sql

    return lasttrkseg, file_uid


def enterlines(cursor, user, trklines, lasttrkseg, file_uid):
    """
    trkline = [trkseg_id, timestamp_start, timestamp_end, linestr]
    """
    for line in trklines:
        trkseg_id, timestamp_start, timestamp_end, linestr = line
        trkseg_id = lasttrkseg + 1
        sql = "INSERT INTO tracklines (trksegid_fm_trkpts, timestamp_start, "
        sql += "timestamp_end, file_uid, user_uid, geom) VALUES "
        sql += "(%d, '%s', '%s', %d, %d, GeomFromText('%s', 4326))" % (trkseg_id, timestamp_start, timestamp_end, file_uid, user, linestr)
        #print sql
        cursor.execute(sql)

    return


def extractpointsosr(datasource):
    """
    parse the gpx file using osgeo's ogr GPX driver and return a list of lines

    line = trkseg_id, trksegpt_id, ele, time, geom

    trackpoint columns: trackpoint_uid, trkseg_id, trksegpt_id, ele,
    utctimestamp, name, cmt, desc, course, speed, file_uid, user_uid,
    geom

    trackline columns: trkline_uid, trksegid_fm_trkpts, name, cmt,
    desc, timestamp_start, timestamp_end, link, type, length(m),
    time(sec), speed(kph), points, file_uid, user_uid, geom
    """
    trklines = []
    trkpts = []
    layer = datasource.GetLayerByName('track_points')
    trkptcount = layer.GetFeatureCount()
    print "\nTrack Point Count %d" % trkptcount
    firsttrkseg = 0
    trkseg_id = 0
    linestr = ogr.Geometry(ogr.wkbLineString)
    # get first timestamp
    feature = layer.GetNextFeature()
    timestamp_start = timetodatetime(feature.GetFieldAsString('time'))
    # free it
    feature.Destroy()

    for feature in layer:
        # have to treat the track_seg_id with caution as it rolls back
        # to 0 with every new track - NOTE the difference between
        # track_seg_id from the GPX driver and my trackseg_id which is
        # unique
        track_seg_id = feature.GetFieldAsInteger('track_seg_id')
        trksegpt_id = feature.GetFieldAsInteger('track_seg_point_id')
        time = timetodatetime(feature.GetFieldAsString('time'))
        ele = float(feature.GetFieldAsString('ele'))
        geom = feature.GetGeometryRef()
        if track_seg_id != firsttrkseg:
            # it's a new segment, so gather the line for the trkline list
            trkline = [trkseg_id, timestamp_start, timestamp_end, linestr.ExportToWkt()]
            trklines.append(trkline)
            # update counting variables
            firsttrkseg = track_seg_id
            trkseg_id += 1
            timestamp_start = time
            # reset linestring geometry
            linestr.Destroy()
            linestr = ogr.Geometry(ogr.wkbLineString)
        timestamp_end = time
        ptline = [trkseg_id, trksegpt_id, ele, time, geom.ExportToWkt()]
        trkpts.append(ptline)
        linestr.AddPoint_2D(geom.GetX(), geom.GetY())
        # destroy the feature before we get the next one
        feature.Destroy()

    # don't throw away last trackline
    trkline = [trkseg_id, timestamp_start, timestamp_end, linestr.ExportToWkt()]
    trklines.append(trkline)


    return trkpts, trklines


def main():
    """
    you know what 'main' does - run everything in the right order and
    print helpful messages to stdout
    """
    # for timing (rough)
    starttime = datetime.now()

    # get file path
    filepath, user, dbpath = parseargs()
    user = int(user)

    # parse the GPX file using ogr's GPX driver and collect the
    # information into a list
    driver = ogr.GetDriverByName('GPX')
    datasource = getdatasource(driver, filepath)
    trkpts, trklines = extractpointsosr(datasource)
    datasource.Destroy()
    endtime = datetime.now()
    print "\nParsing %d points from gpx file took %s " % (len(trkpts), endtime - starttime)

    # connect to the database and make a cursor
    conn = spatialite.connect(dbpath)
    cursor = conn.cursor()

    # enter the file in the file table
    print "Entering file into database"
    firsttimestamp = trkpts[0][3]
    lasttimestamp = trkpts[len(trkpts) - 1][3]
    print "File first timestamp: %s, last timestamp: %s" % (firsttimestamp, lasttimestamp)
    enterfile(filepath, cursor, user, firsttimestamp, lasttimestamp)

    # enter the trackpoints from the gpx file into the trackpoint
    # table (get lasttrkseg and file_uid from this function to pass on
    # to the enterlines function)
    print "Entering points into database"
    lasttrkseg, file_uid = enterpoints(cursor, user, trkpts)
    print "file_uid", file_uid

    # enter the lines into the database - either query the points in
    # the database just entered, or cycle through the list of tupes
    # (lines)
    print "Entering lines into database"
    enterlines(cursor, user, trklines, lasttrkseg, file_uid)

    # commit changes to database and close connection
    conn.commit()
    conn.close()

    endtime = datetime.now()
    print "Script took %s\n" % (endtime - starttime)

if __name__ == '__main__':
    sys.exit(main())
