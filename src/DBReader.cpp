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

#ifdef TARGET_OSX
static const std::string libspatialiteDylibPath =
    "libspatialite_mac/mod_spatialite";
#else
#include "sqlite3.h"
#include <spatialite.h>
#endif

//------------------------------------------------------------------------------
#define CATCHDBERRORS                                                           \
catch(const std::exception& ex)                                                 \
{                                                                               \
    ofLogError(Logger::DB_READER) << "Database error: " << ex.what();           \
}                                                                               \
//------------------------------------------------------------------------------
#define CATCHDBERRORSQ(query)                                                   \
catch(const std::exception& ex)                                                 \
{                                                                               \
    ofLogError(Logger::DB_READER) << "Database error: " <<  ex.what()           \
                                  << ", \nwith query: " << query.c_str();       \
}                                                                               \
//------------------------------------------------------------------------------

DBReader::DBReader(const std::string& dbpath, bool useSpeed)
:
m_dbPath(dbpath),
m_useSpeed(useSpeed)
{
}

//------------------------------------------------------------------------------

DBReader::~DBReader()
{
}

//------------------------------------------------------------------------------

bool DBReader::setupDbConnection()
{
    std::string spatialiteVersion;

#ifndef TARGET_OSX
    spatialite_init(0);
    spatialiteVersion = spatialite_version();
#endif
    try
    {
        m_dbconn.reset(new sqlite3_connection(ofToDataPath(m_dbPath, true)));
#ifdef TARGET_OSX
        m_dbconn->enable_load_extension(true);
        std::stringstream loadExtQuery;
        loadExtQuery << "SELECT load_extension('"
                     << ofToDataPath(libspatialiteDylibPath, true)
                     << "')";
        m_dbconn->executenonquery(loadExtQuery.str());
        spatialiteVersion =
            m_dbconn->executestring("SELECT spatialite_version()");

#endif
        ofLogVerbose(Logger::DB_READER)
            << "Spatialite version: " << spatialiteVersion;

        return true;
    }
    CATCHDBERRORS

    return false;
}

//------------------------------------------------------------------------------

void DBReader::closeDbConnection()
{
    try
    {
        m_dbconn->close();
        m_dbconn.reset();
    }
    CATCHDBERRORS
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataDay(GpsData& gpsData,
                             const std::string& userName,
                             int year, int month, int day)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND strftime('%Y-%m-%d', a.utctimestamp) = '";
    query << year;
    query << "-";
    query << (month < 10 ? "0" : "");
    query << month;
    query << "-";
    query << (day < 10 ? "0" : "");
    query << day;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataDayRange(GpsData& gpsData,
                                  const std::string& userName,
                                  int year, int month, int dayStart, int dayEnd)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND strftime('%Y-%m-%d', a.utctimestamp) >= '";
    query << year;
    query << "-";
    query << (month < 10 ? "0" : "");
    query << month;
    query << "-";
    query << (dayStart < 10 ? "0" : "");
    query << dayStart;
    query << "' AND strftime('%Y-%m-%d', a.utctimestamp) <= '";
    query << year;
    query << "-";
    query << (month < 10 ? "0" : "");
    query << month;
    query << "-";
    query << (dayEnd < 10 ? "0" : "");
    query << dayEnd;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataMonth(GpsData& gpsData,
                               const std::string& userName,
                               int year, int month)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND strftime('%Y-%m', a.utctimestamp) = '";
    query << year;
    query << "-";
    query << (month < 10 ? "0" : "");
    query << month;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataMonthRange(GpsData& gpsData,
                                    const std::string& userName,
                                    int year, int monthStart, int monthEnd)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND strftime('%Y-%m', a.utctimestamp) >= '";
    query << year;
    query << "-";
    query << (monthStart < 10 ? "0" : "");
    query << monthStart;
    query << "' AND strftime('%Y-%m', a.utctimestamp) <= '";
    query << year;
    query << "-";
    query << (monthEnd < 10 ? "0" : "");
    query << monthEnd;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataYear(GpsData& gpsData,
                              const std::string& userName,
                              int year)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND strftime('%Y', a.utctimestamp) = '";
    query << (year < 10 ? "0" : "");
    query << year;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataYearRange(GpsData& gpsData,
                                   const std::string& userName,
                                   int yearStart, int yearEnd)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND strftime('%Y', a.utctimestamp) >= '";
    query << (yearStart < 10 ? "0" : "");
    query << yearStart;
    query << "' AND strftime('%Y', a.utctimestamp) <= '";
    query << (yearEnd < 10 ? "0" : "");
    query << yearEnd;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataCity(GpsData& gpsData,
                              const std::string& userName,
                              const std::string& city)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << "WHERE b.username = '";
    query << userName;
    query << "' AND c.city = '";
    query << city;
    query << "' ORDER BY datetime(a.utctimestamp);";
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------

bool DBReader::getGpsDataWithSqlFile(GpsData& gpsData,
                                     const std::string& sqlFileSource)
{
    std::stringstream query;
    query << getBasicQueryString();
    query << sqlFileSource;
    return getGpsData(gpsData, query.str());
}

//------------------------------------------------------------------------------
bool DBReader::getGpsData(GpsData& gpsData, const std::string& query)
{
    bool queryFirstOk = false;
    stringstream queryMinMax;
    try
    {
        sqlite3_command cmd(*m_dbconn, query);
        sqlite3_reader reader = cmd.executereader();

        int lastSegment = -1;
        string user = "";
        GpsSegment gpsSeg;
        GpsPointVector gpsPointVec;
        GpsSegmentVector gpsSegmentVec;


        // ---------------------------------------------------------------------
        // Get all data from query.
        // ---------------------------------------------------------------------
        while (reader.read())
        {
            GpsPoint gpsPoint;
            const int id = reader.getint(0);
            const double lat = reader.getdouble(1);
            const double lon = reader.getdouble(2);
            const double ele = reader.getdouble(4);
            const string timeStamp = reader.getstring(3);
            const string location = reader.getstring(7);
            const double speed = m_useSpeed ? reader.getdouble(8) : 0.0;
            const int currentSegment = reader.getint(5);
            user = reader.getstring(6);

            gpsPoint.setData(id, lat, lon, ele, timeStamp, location, speed);

            if (currentSegment != lastSegment)
            {
                // this is true only for first time getting to this point
                if (lastSegment == -1)
                {
                    lastSegment = currentSegment;
                }
                else
                {
                    gpsSeg.setGpsSegment(gpsPointVec, lastSegment);
                    gpsSegmentVec.push_back(gpsSeg);
                    lastSegment = currentSegment;
                    gpsPointVec.clear();
                }
            }
            gpsPointVec.push_back(gpsPoint);
        }
        queryFirstOk = true;
        reader.close();
        // -----------------------------------------------------------------------------

        gpsSeg.setGpsSegment(gpsPointVec, lastSegment);
        gpsSegmentVec.push_back(gpsSeg);

        // -----------------------------------------------------------------------------
        // Get min/max values for query.
        // -----------------------------------------------------------------------------
        const size_t posS = query.find("FROM");
        const size_t posE = query.find(" ORDER");

//        queryMinMax << "SELECT min(a.longitude), max(a.longitude), min(a.latitude), max(a.latitude) ";
        queryMinMax << "SELECT min(x(a.geom)), max(x(a.geom)), "
                    << "min(y(a.geom)), max(y(a.geom)) ";
        // Retrieve select conditions from original database query.
        queryMinMax << query.substr(posS, (posE - posS));

        sqlite3_command cmd2(*m_dbconn, queryMinMax.str());
        sqlite3_reader readerMinMax = cmd2.executereader();
        double minLon, maxLon, minLat, maxLat;
        while (readerMinMax.read())
        {
            minLon = readerMinMax.getdouble(0);
            maxLon = readerMinMax.getdouble(1);
            minLat = readerMinMax.getdouble(2);
            maxLat = readerMinMax.getdouble(3);
        }
        readerMinMax.close();
        // -----------------------------------------------------------------------------
        gpsData.clear();
        gpsData.setGpsData(gpsSegmentVec,
                           ofxPoint<double>(minLon, minLat),
                           ofxPoint<double>(maxLon, maxLat),
                           user);
        return true;
    }
    CATCHDBERRORSQ((queryFirstOk ? queryMinMax.str() : query))

    return false;
}

//------------------------------------------------------------------------------

const std::string DBReader::getBasicQueryString()
{
    // This is the part of the query string that all queries have common.
    stringstream query;

    query << "SELECT a.trkpt_uid, y(a.geom) AS latitude, x(a.geom) AS longitude,"
          << "a.utctimestamp AS time, a.ele AS elevation,"
          << "a.trkseg_id AS segment, b.username AS name, "
          << "c.city AS city "
          << (m_useSpeed ? ", a.speed AS speed " : "")
          << "FROM trackpoints AS a "
          << "JOIN users AS b ON (a.user_uid = b.user_uid) "
          << "JOIN citydefs AS c ON (a.citydef_uid = c.citydef_uid) ";

    return query.str();
}

//------------------------------------------------------------------------------
