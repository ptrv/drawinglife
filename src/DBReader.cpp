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

#include "../sqlite3x/sqlite3x.hpp"
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

bool DBReader::getGpsData(GpsData& gpsData, const string& query)
{
	bool result = false;
	bool queryFirstOk = false;
	stringstream queryMinMax;
	try {
		sqlite3_command cmd(*m_dbconn, query.c_str());
		sqlite3_reader reader=cmd.executereader();
		
		DBG_VAL(query);
		
		int lastSegment = -1;
		string user = "";
		GpsSegment gpsSeg;
		std::vector<GpsPoint> gpsPointVec;
		std::vector<GpsSegment> gpsSegmentVec;
		

		// -----------------------------------------------------------------------------
		// Get all data from query.
		// -----------------------------------------------------------------------------
		while(reader.read())
		{
			GpsPoint gpsPoint;
			gpsPoint.setGpsPoint(reader.getdouble(1), reader.getdouble(2), reader.getdouble(4), reader.getstring(3));
			
			int currentSegment = reader.getint(5);
			user = reader.getstring(6);
						
			// for logging
//			ofLog(OF_LOG_NOTICE, "userid: %d, lat: %lf, lon: %lf, ele: %lf, time: %s, segment: %d, user: %d", 
//				  reader.getint(0),
//				  reader.getdouble(1),
//				  reader.getdouble(2),
//				  reader.getdouble(3),
//				  reader.getstring(4).c_str(),
//				  currentSegment,
//				  user);
			
			if (currentSegment != lastSegment) 
			{
				// this is true only for first time getting to this point
				if (lastSegment == -1) 
				{
					lastSegment = currentSegment;
					gpsPointVec.push_back(gpsPoint);
				}
				else
				{
					gpsSeg.setGpsSegment(gpsPointVec, lastSegment);
					gpsSegmentVec.push_back(gpsSeg);
					lastSegment = currentSegment;
					gpsPointVec.clear();
					gpsPointVec.push_back(gpsPoint);
				}
			}
			else
			{
				gpsPointVec.push_back(gpsPoint);				
			}
		}
		queryFirstOk = true;
		// -----------------------------------------------------------------------------
		
		gpsSeg.setGpsSegment(gpsPointVec, lastSegment);
		gpsSegmentVec.push_back(gpsSeg);
		
		// -----------------------------------------------------------------------------
		// Get min/max values for query.
		// -----------------------------------------------------------------------------
		size_t posS = query.find("FROM");
		size_t posE = query.find(" ORDER");
		stringstream queryMinMax;
		
		queryMinMax << "SELECT min(a.longitude), max(a.longitude), min(a.latitude), max(a.latitude) ";
		queryMinMax << query.substr(posS, (posE - posS));

		DBG_VAL(queryMinMax.str());
		
		sqlite3_command cmd2(*m_dbconn, queryMinMax.str().c_str());
		sqlite3_reader readerMinMax = cmd2.executereader();
		double minLon, maxLon, minLat, maxLat;
		while(readerMinMax.read())
		{
			minLon = readerMinMax.getdouble(0);
			maxLon = readerMinMax.getdouble(1);
			minLat = readerMinMax.getdouble(2);
			maxLat = readerMinMax.getdouble(3);
		}
		// -----------------------------------------------------------------------------
		gpsData.clear();
		gpsData.setGpsData(gpsSegmentVec, minLon, maxLon, minLat, maxLat, user);
	}
	CATCHDBERRORSQ((queryFirstOk ? queryMinMax.str() : query))
	return result;
}


bool DBReader::getGpsDataDay(GpsData& gpsData, const string& userName, int day)
{
	bool result = false;
	stringstream query;
	
	query << "SELECT 'a'.'gpsdataid' AS 'gpsdataid', 'a'.'latitude' AS 'latitude',"; 
	query << "'a'.'longitude' AS 'longitude', 'a'.'time' AS 'time',";
	query << "'a'.'elevation' AS 'elevation', 'a'.'segment' AS 'segment',";
	query << "'b'.'name' AS 'name' ";
	query << "FROM 'gpsdata' AS 'a' ";
	query << "JOIN 'user' AS 'b' ON ('a'.'user' = 'b'.'userid') ";
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%d', time) = '";
	query << (day < 10 ? "0" : "");
	query << day;
	query << "' ORDER BY segment;";
	result = getGpsData(gpsData, query.str());
	return result;
}
bool DBReader::getGpsDataDayRange(GpsData& gpsData, const string& userName, int dayStart, int dayEnd)
{
	bool result = false;
	stringstream query;
	query << "SELECT 'a'.'gpsdataid' AS 'gpsdataid', 'a'.'latitude' AS 'latitude',"; 
	query << "'a'.'longitude' AS 'longitude', 'a'.'time' AS 'time',";
	query << "'a'.'elevation' AS 'elevation', 'a'.'segment' AS 'segment',";
	query << "'b'.'name' AS 'name' ";
	query << "FROM 'gpsdata' AS 'a' ";
	query << "JOIN 'user' AS 'b' ON ('a'.'user' = 'b'.'userid') ";
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%d', time) >= '";
	query << (dayStart < 10 ? "0" : "");
	query << dayStart;
	query << "' AND strftime('%d', time) <= '";
	query << (dayEnd < 10 ? "0" : "");
	query << dayEnd;
	query << "' ORDER BY segment;";
	result = getGpsData(gpsData, query.str());
	
	return result;
}

bool DBReader::getGpsDataMonth(GpsData& gpsData, const string& userName, int month)
{
	bool result = false;
	stringstream query;
	query << "SELECT 'a'.'gpsdataid' AS 'gpsdataid', 'a'.'latitude' AS 'latitude',"; 
	query << "'a'.'longitude' AS 'longitude', 'a'.'time' AS 'time',";
	query << "'a'.'elevation' AS 'elevation', 'a'.'segment' AS 'segment',";
	query << "'b'.'name' AS 'name' ";
	query << "FROM 'gpsdata' AS 'a' ";
	query << "JOIN 'user' AS 'b' ON ('a'.'user' = 'b'.'userid') ";
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%m', time) >= '";
	query << (month < 10 ? "0" : "");
	query << month;
	query << "' ORDER BY segment;";
	result = getGpsData(gpsData, query.str());
	
	return result;
}

bool DBReader::getGpsDataMonthRange(GpsData& gpsData, const string& userName, int monthStart, int monthEnd)
{
	bool result = false;
	stringstream query;
	query << "SELECT 'a'.'gpsdataid' AS 'gpsdataid', 'a'.'latitude' AS 'latitude',"; 
	query << "'a'.'longitude' AS 'longitude', 'a'.'time' AS 'time',";
	query << "'a'.'elevation' AS 'elevation', 'a'.'segment' AS 'segment',";
	query << "'b'.'name' AS 'name' ";
	query << "FROM 'gpsdata' AS 'a' ";
	query << "JOIN 'user' AS 'b' ON ('a'.'user' = 'b'.'userid') ";
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%m', time) >= '";
	query << (monthStart < 10 ? "0" : "");
	query << monthStart;
	query << "' AND strftime('%m', time) <= '";
	query << (monthEnd < 10 ? "0" : "");
	query << monthEnd;
	query << "' ORDER BY segment;";
	result = getGpsData(gpsData, query.str());
	
	return result;
}

bool DBReader::getGpsDataYear(GpsData& gpsData, const string& userName, int year)
{
	bool result = false;
	stringstream query;
	query << "SELECT 'a'.'gpsdataid' AS 'gpsdataid', 'a'.'latitude' AS 'latitude',"; 
	query << "'a'.'longitude' AS 'longitude', 'a'.'time' AS 'time',";
	query << "'a'.'elevation' AS 'elevation', 'a'.'segment' AS 'segment',";
	query << "'b'.'name' AS 'name' ";
	query << "FROM 'gpsdata' AS 'a' ";
	query << "JOIN 'user' AS 'b' ON ('a'.'user' = 'b'.'userid') ";
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y', time) >= '";
	query << (year < 10 ? "0" : "");
	query << year;
	query << "' ORDER BY segment;";
	result = getGpsData(gpsData, query.str());
	
	return result;
}

bool DBReader::getGpsDataYearRange(GpsData& gpsData, const string& userName, int yearStart, int yearEnd)
{
	bool result = false;
	stringstream query;
	query << "SELECT 'a'.'gpsdataid' AS 'gpsdataid', 'a'.'latitude' AS 'latitude',"; 
	query << "'a'.'longitude' AS 'longitude', 'a'.'time' AS 'time',";
	query << "'a'.'elevation' AS 'elevation', 'a'.'segment' AS 'segment',";
	query << "'b'.'name' AS 'name' ";
	query << "FROM 'gpsdata' AS 'a' ";
	query << "JOIN 'user' AS 'b' ON ('a'.'user' = 'b'.'userid') ";
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y', time) >= '";
	query << (yearStart < 10 ? "0" : "");
	query << yearStart;
	query << "' AND strftime('%Y', time) <= '";
	query << (yearEnd < 10 ? "0" : "");
	query << yearEnd;
	query << "' ORDER BY segment;";
	result = getGpsData(gpsData, query.str());
	
	return result;
}


