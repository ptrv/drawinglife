/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DBREADER_H_
#define _DBREADER_H_

#include "Data.h"
#include <string>
#include "sqlite3x.hpp"
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
	bool getGpsDataDay(GpsData& gpsData, int day);
	bool getGpsDataDayRange(GpsData& gpsData, int dayStart, int dayEnd);
	bool getGpsDataMonth(GpsData& gpsData, int month);
	bool getGpsDataMonthRange(GpsData& gpsData, int monthStart, int monthEnd);
	bool getGpsDataYear(GpsData& gpsData, int year);
	bool getGpsDataYearRange(GpsData& gpsData, int yearStart, int yearEnd);
	
private:
	
	bool getGpsData(GpsData& gpsData, const std::string& query);
	
	// -----------------------------------------------------------------------------
	
	string m_dbPath;
	sqlite3_connection* m_dbconn;
	sqlite3_transaction* m_trans;
	
};
#endif // _DBREADER_H_