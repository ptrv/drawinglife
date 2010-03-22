/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DBREADER_H_
#define _DBREADER_H_

#include <string>
#include "sqlite3x.hpp"
using namespace sqlite3x;

using namespace std;

#include "Data.h"
class DBReader {
public:
	//DBReader();
	DBReader(const string& dbpath);
	~DBReader();
	bool getQuery(vector<GpsPoint>& queryVec);
	void setupDbConnection();
	void closeDbConnection();
	bool getGpsDataDay(vector<GpsPoint>& queryVec, int day);
	bool getGpsDataDayRange(vector<GpsPoint>& queryVec, int dayStart, int dayEnd);
	bool getGpsDataMonth(vector<GpsPoint>& queryVec, int month);
	bool getGpsDataMonthRange(vector<GpsPoint>& queryVec, int monthStart, int monthEnd);
	bool getGpsDataYear(vector<GpsPoint>& queryVec, int year);
	bool getGpsDataYearRange(vector<GpsPoint>& queryVec, int yearStart, int yearEnd);
	
private:
	
	bool getGpsData(vector<GpsPoint>& gpsDataVec, const std::string& query);
	
	// -----------------------------------------------------------------------------
	
	string m_dbPath;
	sqlite3_connection* m_dbconn;
	sqlite3_transaction* m_trans;
	
};
#endif // _DBREADER_H_