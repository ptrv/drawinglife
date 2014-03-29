/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>

#ifndef __GNUC__
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

/**
 * \brief Wrapper class for ofLog.
 *
 * Makes easy to debug values of various types.
 */
class Logger {
private:
    Logger(){};
    ~Logger(){};
public:
    static const char* APP;
    static const char* DB_READER;
    static const char* SETTINGS;
    static const char* GPS_DATA;
    static const char* WALK;
    static const char* MAGIC_BOX;
    static const char* DATA_LOADER;

    static void logValue(const char* function, const char* name, const std::string& value);
    static void logValue(const char* function, const char* name, int value);
    static void logValue(const char* function, const char* name, unsigned int value);
    static void logValue(const char* function, const char* name, double value);
    static void logValue(const char* function, const char* name, float value);
    static void logValue(const char* function, const char* name, bool value);
};


#define DBG_VAL(value)					Logger::logValue(__PRETTY_FUNCTION__, #value, value)

#endif // _LOGGER_H_
