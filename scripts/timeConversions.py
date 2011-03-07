#!/usr/bin/env python
#-*- coding:utf-8 -*-

"""
    A script to synthesize a set of timestamps for a data set that
    didn't record them. Problem arose 2010-10 when downloading igotu
    GPS and saving in kml format only, then converting to GPX - time
    was missing.

    Generates a new file that has the synthesized time codes in,
    starting with STARTDATE and incrementing by amount of seconds in
    INCREMENTSECS.

    TODO: make it more adaptable to other files, perhaps by choosing a
    file initially and checking whether it has a time code or not

    See also: gpxreplace.py
"""

import sys, tkFileDialog
from datetime import datetime, timedelta

# options are hard coded at the moment for brevity and ease of testing
STARTDATE = "2010-10-15T13:13:00Z"  # when to start the first trackpoint
INCREMENTSECS = 6                  # how many seconds between each trackpoint
GPXORIG = "/home/daniel/Desktop/Dropbox/planb/hamburgHCU/gps/animation/igotu-animation.gpx" # path of gpx file with empty time

def StringConvert(string):
    """
        take an iso format date e.g. 2010-10-15T13:13:00Z and convert
        it to a datetime object
    """
    string = string.replace('T', ' ')
    string = string.replace('-', ' ')
    string = string.replace(':', ' ')
    string = string [:-1]
    strlist = string.split()
    return datetime( int(strlist[0]), int(strlist[1]), int(strlist[2]), int(strlist[3]), int(strlist[4]), int(strlist[5]) )

def main():
    print STARTDATE
    newdate = StringConvert(STARTDATE)
    f = open(GPXORIG, 'r')
    count = 0
    newline = []
    for line in f:
        newline.append(line)
        if line.startswith('<trkpt'): count += 1
        if line.endswith('</ele>\r\n'):
            newdate = newdate + timedelta(seconds = INCREMENTSECS)
            newline.append( '  <time>%s</time>\r\n' % (newdate.isoformat()+'Z'))
    print
    f2 = tkFileDialog.asksaveasfile(title='Choose a file to save to', initialdir="/home/daniel/Desktop/Dropbox")
    for line in newline:
        f2.write(line)
        print line
    f.close()
    f2.close()
    print 'wrote %d lines to file successfully - script ending' % (count)
    print


if __name__ == '__main__':
    sys.exit(main())
