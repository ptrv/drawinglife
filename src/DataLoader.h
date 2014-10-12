#ifndef DATALOADER_H
#define DATALOADER_H

#include "DrawingLifeIncludes.h"

class DrawingLifeApp;
class DBReader;
class GpsData;

class DataLoader
{
public:

    static bool loadGpsDataCity(DrawingLifeApp& app,
                                const StringVec& names,
                                const std::string& city);
    static bool loadGpsDataYearRange(DrawingLifeApp& app,
                                     const StringVec& names,
                                     int yearStart, int yearEnd);
    static bool loadGpsDataWithSqlFile(DrawingLifeApp& app,
                                       const StringVec& sqlFilePaths);
    static bool loadGpsDataAll(DrawingLifeApp& app,
                               const StringVec& names);

    static bool loadCurrentPointImages(DrawingLifeApp& app);
    static void loadLocationImages(DrawingLifeApp& app);
    static void loadSoundPlayers(DrawingLifeApp& app);
    static void loadFonts(DrawingLifeApp& app, DrawingLifeFonts& fonts);

private:

    static void processGpsData(DrawingLifeApp& app);

    typedef boost::function<bool(DBReader*,GpsData&)> tFuncLoadGpsData;
    static bool loadGpsData(DrawingLifeApp& app,
                            const std::vector<tFuncLoadGpsData>& funcVec);

};

#endif // DATALOADER_H
