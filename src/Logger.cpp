#include "Logger.h"

#include "ofMain.h"

const char* Logger::APP          = "DrawingLifeApp";
const char* Logger::DB_READER    = "DBReader";
const char* Logger::SETTINGS     = "AppSettings";
const char* Logger::GPS_DATA     = "GpsData";
const char* Logger::WALK         = "Walk";
const char* Logger::MAGIC_BOX    = "MagicBox";
const char* Logger::DATA_LOADER  = "DataLoader";

void Logger::logValue(const char* function, const char* name, const string& value)
{
    ofLog(OF_LOG_VERBOSE, "%s --> %s \n%s : %s\n<--", ofGetTimestampString().c_str(),
          function, name, value.c_str());
}
void Logger::logValue(const char* function, const char* name, int value)
{
    ofLog(OF_LOG_VERBOSE, "%s --> %s \n%s : %d\n<--", ofGetTimestampString().c_str(),
          function, name, value);
}
void Logger::logValue(const char* function, const char* name, unsigned int value)
{
    ofLog(OF_LOG_VERBOSE, "%s --> %s \n%s : %u\n<--", ofGetTimestampString().c_str(),
          function, name, value);
}
void Logger::logValue(const char* function, const char* name, double value)
{
    ofLog(OF_LOG_VERBOSE, "%s --> %s \n%s : %f\n<--", ofGetTimestampString().c_str(),
          function, name, value);
}
void Logger::logValue(const char* function, const char* name, float value)
{
    ofLog(OF_LOG_VERBOSE, "%s --> %s \n%s : %f\n<--", ofGetTimestampString().c_str(),
          function, name, value);
}
void Logger::logValue(const char* function, const char* name, bool value)
{
    ofLog(OF_LOG_VERBOSE, "%s --> %s \n%s : %s\n<--", ofGetTimestampString().c_str(),
          function, name, (value ? "true" : "false"));
}
