/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DBREADER_H_
#define _DBREADER_H_

#include <string>
#include "sqlite3x.hpp"
using namespace sqlite3x;
//#include "ofxXmlSettings.h"
//#include "ofxSQLiteHeaders.h"

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

	
private:
	string m_dbPath;
	sqlite3_connection* m_dbconn;
	sqlite3_transaction* m_trans;
	
};
#endif // _DBREADER_H_