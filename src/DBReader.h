/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DBREADER_H_
#define _DBREADER_H_

#include "Data.h"
#include <string>
#include "../sqlite3x/sqlite3x.hpp"
using namespace sqlite3x;

using namespace std;

/**
 * \brief Class for reading from database using sqlite3x.
 * 
 */
class DBReader {
public:
	DBReader(const string& dbpath);
	~DBReader();
	void setupDbConnection();
	void closeDbConnection();
	bool getGpsDataDay(GpsData& gpsData, const string& userName, int day);
	bool getGpsDataDayRange(GpsData& gpsData, const string& userName, int dayStart, int dayEnd);
	bool getGpsDataMonth(GpsData& gpsData, const string& userName, int month);
	bool getGpsDataMonthRange(GpsData& gpsData, const string& userName, int monthStart, int monthEnd);
	bool getGpsDataYear(GpsData& gpsData, const string& userName, int year);
	bool getGpsDataYearRange(GpsData& gpsData, const string& userName, int yearStart, int yearEnd);
	
private:
	
	bool getGpsData(GpsData& gpsData, const std::string& query);
	
	const string getBasicQueryString();
	
	// -----------------------------------------------------------------------------
	
	string m_dbPath;
	sqlite3_connection* m_dbconn;
	sqlite3_transaction* m_trans;
	
};
#endif // _DBREADER_H_