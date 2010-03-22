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
LOG_ERROR("Database error: %s", ex.what());												\
}																						\
// --------------------------------------------------------------------------------------
#define CATCHDBERRORSQ(query)															\
catch(exception &ex)																	\
{																						\
LOG_ERROR("Database error: %s, \nwith query: %s", ex.what(), query);					\
}																						\
// --------------------------------------------------------------------------------------

//DBReader::DBReader()
//{
//	m_settings.loadFile("AppSettings.xml");
//	m_dbPath = m_settings.getValue("settings:database", "test.db");
//	LOG_VERBOSE("Db is: %s ", m_dbPath.c_str());
//}
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