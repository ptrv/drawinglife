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
#include <stdexcept>

#include "sqlite3x.hpp"
using namespace sqlite3x;

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

DBReader::DBReader(const std::string& dbpath)
:
m_dbPath(dbpath),
m_dbconn(NULL),
m_trans(NULL)
{
}

DBReader::~DBReader()
{
	SAFE_DELETE(m_dbconn);
	SAFE_DELETE(m_trans);
}

bool DBReader::setupDbConnection()
{
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
		m_dbconn = NULL;
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
			gpsPoint.setGpsPoint(reader.getdouble(1), reader.getdouble(2), reader.getdouble(4), reader.getstring(3), reader.getstring(7));

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

		queryMinMax << "SELECT min(a.longitude), max(a.longitude), min(a.latitude), max(a.latitude) ";
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
	return	"SELECT a.gpsdataid AS gpsdataid, a.latitude AS latitude,"\
			"a.longitude AS longitude, a.time AS time,"\
			"a.elevation AS elevation, a.segment AS segment,"\
			"b.name AS name,"\
			"c.city AS location "\
			"FROM gpsdata AS a "\
			"JOIN user AS b ON (a.user = b.userid) "\
			"JOIN location AS c ON (a.location = c.locationid) ";
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
	query << "' ORDER BY segment;";
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
	query << "' ORDER BY segment;";
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
	query << "' ORDER BY segment;";
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
	query << "' ORDER BY segment;";
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
	query << "' ORDER BY segment;";
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
	query << "' ORDER BY segment;";
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
	query << "' ORDER BY time ASC;";
	result = getGpsData(gpsData, query.str());
	return result;
}
