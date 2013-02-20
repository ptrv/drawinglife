/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "GpsPoint.h"
#include "GpsSegment.h"
#include "GpsData.h"
#include "DBReader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "sqlite3x.hpp"
using namespace sqlite3x;

#include <sqlite3.h>
#include <spatialite.h>

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

DBReader::DBReader(const std::string& dbpath, bool useSpeed)
:
m_dbPath(dbpath),
m_dbconn(0),
m_trans(0),
m_useSpeed(useSpeed)
{
}

DBReader::~DBReader()
{
	SAFE_DELETE(m_dbconn);
	SAFE_DELETE(m_trans);
}

bool DBReader::setupDbConnection()
{

    spatialite_init(0);
    ofLog() << "Spatialite version: " << spatialite_version();

	bool result = false;
	try {
		m_dbconn = new sqlite3_connection(m_dbPath.c_str());
		result = true;
	}
	CATCHDBERRORS
	return result;
}

void DBReader::closeDbConnection()
{
	try {
		m_dbconn->close();
		SAFE_DELETE(m_dbconn);
//		m_dbconn = 0;
	}
	CATCHDBERRORS
}

bool DBReader::getGpsData(GpsData& gpsData, const std::string& query)
{
	bool result = false;
	bool queryFirstOk = false;
	std::stringstream queryMinMax;
	try {
		sqlite3_command cmd(*m_dbconn, query.c_str());
		sqlite3_reader reader=cmd.executereader();

		//DBG_VAL(query);

		int lastSegment = -1;
		std::string user = "";
		GpsSegment gpsSeg;
		std::vector<GpsPoint> gpsPointVec;
		std::vector<GpsSegment> gpsSegmentVec;


		// -----------------------------------------------------------------------------
		// Get all data from query.
		// -----------------------------------------------------------------------------
		while(reader.read())
		{
			GpsPoint gpsPoint;
			if(m_useSpeed)
            {
                gpsPoint.setGpsPoint(reader.getint(0),
                                     reader.getdouble(1),
                                     reader.getdouble(2),
                                     reader.getdouble(4),
                                     reader.getstring(3),
                                     reader.getstring(7),
                                     reader.getdouble(8));
            }
            else
//                gpsPoint.setGpsPoint(reader.getint(0),
//                                     reader.getdouble(1),
//                                     reader.getdouble(2),
//                                     reader.getdouble(4),
//                                     reader.getstring(3),
//                                     reader.getstring(7),
//                                     0.0);
            gpsPoint.setGpsPoint(reader.getint(0),
                                 reader.getdouble(1),
                                 reader.getdouble(2),
                                 reader.getdouble(4),
                                 reader.getstring(3),
                                 "no info",
                                 0.0);

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

//        queryMinMax << "SELECT min(a.longitude), max(a.longitude), min(a.latitude), max(a.latitude) ";
        queryMinMax << "SELECT min(x(a.geom)), max(x(a.geom)), min(y(a.geom)), max(y(a.geom)) ";
		// Retrieve select conditions from original database query.
		queryMinMax << query.substr(posS, (posE - posS));

		//DBG_VAL(queryMinMax.str());

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
		result = true;
	}
	CATCHDBERRORSQ((queryFirstOk ? queryMinMax.str() : query))
	return result;
}

const std::string DBReader::getBasicQueryString()
{
	// This is the part of the query string that all queries have common.
	std::string query;

    if(m_useSpeed)
    {
        query =	"SELECT a.gpsdataid AS gpsdataid, a.latitude AS latitude,"\
                "a.longitude AS longitude, a.time AS time,"\
                "a.elevation AS elevation, a.segment AS segment,"\
                "b.name AS name,"\
                "c.city AS location, "\
                "a.speed AS speed "\
                "FROM gpsdata AS a "\
                "JOIN user AS b ON (a.user = b.userid) "\
                "JOIN location AS c ON (a.location = c.locationid) ";

    }
    else
    {
        query =	"SELECT a.gpsdataid AS gpsdataid, a.latitude AS latitude,"\
                "a.longitude AS longitude, a.time AS time,"\
                "a.elevation AS elevation, a.segment AS segment,"\
                "b.name AS name,"\
                "c.city AS location "\
                "FROM gpsdata AS a "\
                "JOIN user AS b ON (a.user = b.userid) "\
                "JOIN location AS c ON (a.location = c.locationid) ";

    }

    return query;
}
const std::string DBReader::getBasicQueryString2()
{
    // This is the part of the query string that all queries have common.
    std::string query;

    query =	"SELECT a.trackpoint_uid, y(a.geom) AS latitude, x(a.geom) AS longitude,"\
            "a.utctimestamp AS time, a.ele AS elevation,"\
            "a.trkseg_id AS segment, b.username AS name "\
            "FROM trackpoints AS a "\
            "JOIN users AS b ON (a.user_uid = b.user_uid) ";

    return query;
}

bool DBReader::getGpsDataDay(GpsData& gpsData, const std::string& userName, int year, int month, int day)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y-%m-%d', time) = '";
	query << year;
	query << "-";
	query << (month < 10 ? "0" : "");
	query << month;
	query << "-";
	query << (day < 10 ? "0" : "");
	query << day;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}
bool DBReader::getGpsDataDayRange(GpsData& gpsData, const std::string& userName, int year, int month, int dayStart, int dayEnd)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y-%m-%d', time) >= '";
	query << year;
	query << "-";
	query << (month < 10 ? "0" : "");
	query << month;
	query << "-";
	query << (dayStart < 10 ? "0" : "");
	query << dayStart;
	query << "' AND strftime('%Y-%m-%d', time) <= '";
	query << year;
	query << "-";
	query << (month < 10 ? "0" : "");
	query << month;
	query << "-";
	query << (dayEnd < 10 ? "0" : "");
	query << dayEnd;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}

bool DBReader::getGpsDataMonth(GpsData& gpsData, const std::string& userName, int year, int month)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y-%m', time) = '";
	query << year;
	query << "-";
	query << (month < 10 ? "0" : "");
	query << month;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}

bool DBReader::getGpsDataMonthRange(GpsData& gpsData, const std::string& userName, int year, int monthStart, int monthEnd)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y-%m', time) >= '";
	query << year;
	query << "-";
	query << (monthStart < 10 ? "0" : "");
	query << monthStart;
	query << "' AND strftime('%Y-%m', time) <= '";
	query << year;
	query << "-";
	query << (monthEnd < 10 ? "0" : "");
	query << monthEnd;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}

bool DBReader::getGpsDataYear(GpsData& gpsData, const std::string& userName, int year)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y', time) = '";
	query << (year < 10 ? "0" : "");
	query << year;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}

bool DBReader::getGpsDataYearRange(GpsData& gpsData, const std::string& userName, int yearStart, int yearEnd)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND strftime('%Y', time) >= '";
	query << (yearStart < 10 ? "0" : "");
	query << yearStart;
	query << "' AND strftime('%Y', time) <= '";
	query << (yearEnd < 10 ? "0" : "");
	query << yearEnd;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}

bool DBReader::getGpsDataCity(GpsData& gpsData, const std::string& userName, const std::string& city)
{
	bool result = false;
	std::stringstream query;
	query << getBasicQueryString();
	query << "WHERE name = '";
	query << userName;
	query << "' AND c.city = '";
	query << city;
	query << "' ORDER BY datetime(time);";
	result = getGpsData(gpsData, query.str());
	return result;
}

bool DBReader::getGpsDataWithSqlFile(GpsData& gpsData, const std::string& userName, const std::string& sqlFileSource)
{
	bool result = false;
	std::stringstream query;
    query << getBasicQueryString2();
    query << sqlFileSource;
//    query << ";";
    DBG_VAL(query.str());
	result = getGpsData(gpsData, query.str());
	return result;
}
