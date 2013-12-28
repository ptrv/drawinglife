#ifndef DATALOADER_H
#define DATALOADER_H

#include "DrawingLifeIncludes.h"

class DrawingLifeApp;
class DBReader;
class GpsData;

class DataLoader
{
public:

    /**
    * \brief
    * \param names vector with name strings.
    * \param city string with city name.
    */
    static bool loadGpsDataCity(DrawingLifeApp& app,
                                const StringVec& names,
                                const std::string& city);
    /**
    * \brief
    * \param names vector with name strings.
    * \param yearStart start year for query.
    * \param yearEnd end year for query.
    */
    static bool loadGpsDataYearRange(DrawingLifeApp& app,
                                     const StringVec& names,
                                     int yearStart, int yearEnd);

    static bool loadGpsDataWithSqlFile(DrawingLifeApp& app,
                                       const StringVec& sqlFilePaths);

    static bool loadCurrentPointImages(DrawingLifeApp& app);

private:

    static void prepareGpsData(DrawingLifeApp& app);
    static void processGpsData(DrawingLifeApp& app);

    typedef boost::function<bool(DBReader*,GpsData&)> tFuncLoadGpsData;
    static bool loadGpsData(DrawingLifeApp& app,
                            const std::vector<tFuncLoadGpsData>& funcVec);

    DataLoader();
};

#endif // DATALOADER_H
