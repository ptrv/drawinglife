/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DBREADER_H_
#define _DBREADER_H_

#include "GpsData.h"
#include <string>

namespace sqlite3x { class sqlite3_connection; }

class DBReader
{
public:

    static const int DB_QUERY_DAY       = 0; 	///< Database query type constant for day.
    static const int DB_QUERY_MONTH     = 2;	///< Database query type constant for month.
    static const int DB_QUERY_YEAR      = 3;	///< Database query type constant for year.
    static const int DB_QUERY_CITY      = 4;	///< Database query type constant for city.
    static const int DB_QUERY_SQLFILE   = 5;	///< Database query type constant for sql file.

	DBReader(const std::string& dbpath, bool useSpeed=false);
	~DBReader();

    bool setupDbConnection();

    void closeDbConnection();

    bool getGpsDataDay(GpsData& gpsData, const std::string& userName,
                       int year, int month, int day);
    bool getGpsDataDayRange(GpsData& gpsData, const std::string& userName,
                            int year, int month, int dayStart, int dayEnd);
    bool getGpsDataMonth(GpsData& gpsData, const std::string& userName,
                         int year, int month);
    bool getGpsDataMonthRange(GpsData& gpsData, const std::string& userName,
                              int year, int monthStart, int monthEnd);
    bool getGpsDataYear(GpsData& gpsData, const std::string& userName, int year);
    bool getGpsDataYearRange(GpsData& gpsData,
                             const std::string& userName,
                             int yearStart, int yearEnd);
    bool getGpsDataCity(GpsData& gpsData,
                        const std::string& userName,
                        const std::string& city);
    bool getGpsDataWithSqlFile(GpsData& gpsData,
                               const std::string& sqlFileSource);

private:

    bool getGpsData(GpsData& gpsData, const std::string& query);

    const string getBasicQueryString();

	string m_dbPath;
    boost::scoped_ptr<sqlite3x::sqlite3_connection> m_dbconn;

	bool m_useSpeed;

};
#endif // _DBREADER_H_
