/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DBREADER_H_
#define _DBREADER_H_

#include "GpsData.h"
#include <string>
#include "sqlite3x.hpp"
using namespace sqlite3x;

/**
 * \brief Class for reading from database using sqlite3x.
 *
 */
class DBReader
{
public:

    static const int DB_QUERY_DAY       = 0; 	///< Database query type constant for day. 
    static const int DB_QUERY_MONTH     = 2;	///< Database query type constant for month. 
    static const int DB_QUERY_YEAR      = 3;	///< Database query type constant for year. 
    static const int DB_QUERY_CITY      = 4;	///< Database query type constant for city. 
    static const int DB_QUERY_SQLFILE   = 5;	///< Database query type constant for sql file. 

// -----------------------------------------------------------------------------

	DBReader(const std::string& dbpath);
	~DBReader();
	/**
	* \brief Open connection to sqlite database.
	* \return true if open connection succeed.
	*/
	bool setupDbConnection();
	/**
	* \brief Close open database connection.
	*/
	void closeDbConnection();
    /**
    * \brief Get Gps data for a day.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param year
    * \param month
    * \param day
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataDay(GpsData& gpsData, const std::string& userName, int year, int month, int day);
    /**
    * \brief Get Gps data for a day range.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param year
    * \param month
    * \param dayStart
    * \param dayEnd
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataDayRange(GpsData& gpsData, const std::string& userName, int year, int month, int dayStart, int dayEnd);
    /**
    * \brief Get Gps data for one month.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param year
    * \param month
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataMonth(GpsData& gpsData, const std::string& userName, int year, int month);
    /**
    * \brief Get Gps data for a month range.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param year
    * \param monthStart
    * \param monthEnd
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataMonthRange(GpsData& gpsData, const std::string& userName, int year, int monthStart, int monthEnd);
    /**
    * \brief Get Gps data for one year.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param year
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataYear(GpsData& gpsData, const std::string& userName, int year);
    /**
    * \brief Get Gps data for a year range.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param yearStart
    * \param yearEnd
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataYearRange(GpsData& gpsData, const std::string& userName, int yearStart, int yearEnd);
    /**
    * \brief Get Gps data for a city.
    * \param gpsData Reference to GpsData object.
    * \param userName
    * \param city name of city.
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataCity(GpsData& gpsData, const std::string& userName, const std::string& city);
    /**
    * \brief Get Gps data with query from sql file.
    * \param gpsData Reference to GpsData object.
    * \param userName.
    * \param path sql file.
    * \return true if getting gpsData succeeded.
    */
	bool getGpsDataWithSqlFile(GpsData& gpsData, const std::string& userName, const std::string& sqlFileSource);

private:
    /**
    * \brief Get Gps data for one day.
    * \param gpsData Reference to GpsData object.
    * \param query string with specified query.
    * \return true if getting gpsData succeeded.
    */
	bool getGpsData(GpsData& gpsData, const std::string& query);

	/**
    * \brief Get basic query.
    * \return string query.
    */
    const string getBasicQueryString();

	// -----------------------------------------------------------------------------

	string m_dbPath;
	sqlite3_connection* m_dbconn;
	sqlite3_transaction* m_trans;

};
#endif // _DBREADER_H_
