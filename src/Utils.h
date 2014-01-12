#ifndef UTILS_H
#define UTILS_H

#include "DrawingLifeIncludes.h"

class DrawingLifeApp;

class Utils
{
public:
    static void calculateGlobalMinMaxValues(DrawingLifeApp& app);

    static float getFPS();

    static void grabScreen();

    static ofxPoint<double> getPointDoubleMin();
    static ofxPoint<double> getPointDoubleMax();

    static const std::string getCurrentGpsInfo(const GpsData* gpsData,
                                               const Walk* walk);
    static const std::string getCurrentGpsInfoDebug(const GpsData* gpsData,
                                                    const Walk* walk,
                                                    const MagicBox* box);

private:
    Utils();
};

#endif // UTILS_H
