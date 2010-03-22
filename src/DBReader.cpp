/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "ofMain.h"
#include "DBReader.h"
#include "Logger.h"
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

#include "sqlite3x.hpp"
using namespace sqlite3x;

#include "Data.h"

// --------------------------------------------------------------------------------------
#define CATCHDBERRORS																	\
catch(exception &ex)																	\
{																						\
ofLog(OF_LOG_ERROR, "Database error: %s", ex.what());									\
}																						\
// --------------------------------------------------------------------------------------
#define CATCHDBERRORSQ(query)															\
catch(exception &ex)																	\
{																						\
ofLog(OF_LOG_ERROR, "Database error: %s, \nwith query: %s", ex.what(), query.c_str());	\
}																						\
// --------------------------------------------------------------------------------------

DBReader::DBReader(const string& dbpath)
:
m_dbPath(dbpath),
m_dbconn(NULL),
m_trans(NULL)
{
}

DBReader::~DBReader()
{
	delete m_dbconn;
	delete m_trans;
}

void DBReader::setupDbConnection()
{
	try {
		m_dbconn = new sqlite3_connection(m_dbPath.c_str());
	}
	CATCHDBERRORS
}

void DBReader::closeDbConnection()
{
	try {
		m_dbconn->close();
		m_dbconn = NULL;
	}
	CATCHDBERRORS
}

bool DBReader::getQuery(vector<GpsPoint>& queryVec)
{
	bool result = false;
	queryVec.clear();
	try {
		sqlite3_command cmd(*m_dbconn, "SELECT gpsdataid, latitude, longitude, elevation, time FROM gpsdata WHERE strftime('%d', time) = '09';");
		sqlite3_reader reader=cmd.executereader();
		
		while(reader.read())
		{
			//cout << reader.getint(0) << ": " << reader.getint(1) << reader.getstring(2) << endl;
			stringstream tmp;
			tmp << reader.getint(0) << ": " << reader.getint(1) << reader.getstring(2);
			GpsPoint gpsPoint;
			gpsPoint.setGpsPoint(reader.getdouble(1), reader.getdouble(2), reader.getdouble(3), reader.getstring(4));
			queryVec.push_back(gpsPoint);
			
		}
		result = true;
	}
	CATCHDBERRORS
	return result;
}

bool DBReader::getGpsData(vector<GpsPoint>& gpsData, const string& query)
{
	bool result = false;
	try {
		sqlite3_command cmd(*m_dbconn, query.c_str());
		sqlite3_reader reader=cmd.executereader();
		DBG_VAL(query);
		int lastSegment = -1;
		bool firstSegment = true;
//		GpsData gpsData;
		while(reader.read())
		{
			GpsPoint gpsPoint;
			gpsPoint.setGpsPoint(reader.getdouble(1), reader.getdouble(2), reader.getdouble(3), reader.getstring(4));
			
			int currentSegment = reader.getint(5);
			int user = reader.getint(6);

			ofLog(OF_LOG_NOTICE, "userid: %d, lat: %lf, lon: %lf, ele: %lf, time: %s, segment: %d, user: %d", 
				  reader.getint(0), reader.getdouble(1), reader.getdouble(2), reader.getdouble(3), reader.getstring(4).c_str(), currentSegment, user);
			
			if (currentSegment != lastSegment) 
			{
				if (firstSegment) 
				{
					lastSegment = currentSegment;
					firstSegment = true;
				}
			}
			gpsData.push_back(gpsPoint);
			
		}
	}
	CATCHDBERRORSQ(query)
	return result;
}

bool DBReader::getGpsDataDay(vector<GpsPoint>& queryVec, int day)
{
	bool result = false;
	stringstream query;
	query << "SELECT gpsdataid, latitude, longitude, elevation, time, segment, user FROM gpsdata WHERE strftime('%d', time) = '";
	query << (day < 10 ? "0" : "");
	query << day;
	query << "' ORDER BY segment;";
	result = getGpsData(queryVec, query.str());
	return result;
}
bool DBReader::getGpsDataDayRange(vector<GpsPoint>& queryVec, int dayStart, int dayEnd)
{
	bool result = false;
	stringstream query;
	query << "SELECT gpsdataid, latitude, longitude, elevation, time, segment, user FROM gpsdata WHERE strftime('%d', time) >= '";
	query << (dayStart < 10 ? "0" : "");
	query << dayStart;
	query << "' AND strftime('%d', time) <= '";
	query << (dayEnd < 10 ? "0" : "");
	query << dayEnd;
	query << "' ORDER BY segment;";
	result = getGpsData(queryVec, query.str());
	
	return result;
}

bool DBReader::getGpsDataMonth(vector<GpsPoint>& queryVec, int month)
{
	bool result = false;
	stringstream query;
	query << "SELECT gpsdataid, latitude, longitude, elevation, time, segment, user FROM gpsdata WHERE strftime('%m', time) = '";
	query << (month < 10 ? "0" : "");
	query << month;
	query << "' ORDER BY segment;";
	result = getGpsData(queryVec, query.str());
	
	return result;
}

bool DBReader::getGpsDataMonthRange(vector<GpsPoint>& queryVec, int monthStart, int monthEnd)
{
	bool result = false;
	stringstream query;
	query << "SELECT gpsdataid, latitude, longitude, elevation, time, segment, user FROM gpsdata WHERE strftime('%m', time) >= '";
	query << (monthStart < 10 ? "0" : "");
	query << monthStart;
	query << "' AND strftime('%m', time) <= '";
	query << (monthEnd < 10 ? "0" : "");
	query << monthEnd;
	query << "' ORDER BY segment;";
	result = getGpsData(queryVec, query.str());
	
	return result;
}

bool DBReader::getGpsDataYear(vector<GpsPoint>& queryVec, int year)
{
	bool result = false;
	stringstream query;
	query << "SELECT gpsdataid, latitude, longitude, elevation, time, segment, user FROM gpsdata WHERE strftime('%Y', time) = '";
	query << (year < 10 ? "0" : "");
	query << year;
	query << "' ORDER BY segment;";
	result = getGpsData(queryVec, query.str());
	
	return result;
}

bool DBReader::getGpsDataYearRange(vector<GpsPoint>& queryVec, int yearStart, int yearEnd)
{
	bool result = false;
	stringstream query;
	query << "SELECT gpsdataid, latitude, longitude, elevation, time, segment, user FROM gpsdata WHERE strftime('%Y', time) >= '";
	query << (yearStart < 10 ? "0" : "");
	query << yearStart;
	query << "' AND strftime('%Y', time) <= '";
	query << (yearEnd < 10 ? "0" : "");
	query << yearEnd;
	query << "' ORDER BY segment;";
	result = getGpsData(queryVec, query.str());
	
	return result;
}


