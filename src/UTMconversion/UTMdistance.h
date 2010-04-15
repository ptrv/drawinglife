#ifndef _UTMDISTANCE_H_
#define _UTMDISTANCE_H_

#include <cmath>

/**
 * \brief Class for calculating UTM distances.
 *
 * Different functions for calculating accurate UTM distance.
 * http://www.sharpgis.net/post/2007/10/14/Accurate-distance-calculations-in-UTM-projections.aspx
 *
 */
class UTMdistance
{
public:
    enum Method
    {
        /// Euclidean distance.
        Euclidean = 0,
        /// Applies an average scalefactor linearly to the distance reduction.
        Linear = 1,
        /// Use Simpsons Rule to apply the scale reduction.
        Simpsons = 2,
        /// Uses integrals for finding the exact scale-reduced distance.
        Integral = 3
    };
    static double Distance(double fromE, double fromN, double toE, double toN, Method method)
    {
        switch (method)
        {
        case Euclidean:
            return calcEuclidean(fromE, fromN, toE, toN);
        case Linear:
            return calcLinearAverage(fromE, fromN, toE, toN);
        case Simpsons:
            return calcSimpsons(fromE, fromN, toE, toN);
        case Integral:
        default:
            return calcIntegral(fromE, fromN, toE, toN);
            break;
        }
    }
private:
    static double GetScaleFactor(double e);
	static double calcEuclidean(double fromE, double fromN, double toE, double toN);
	static double calcLinearAverage(double fromE, double fromN, double toE, double toN);
	static double calcSimpsons(double fromE, double fromN, double toE, double toN);
	static double calcIntegral(double fromE, double fromN, double toE, double toN);

    UTMdistance();
    virtual ~UTMdistance();

    static const double FalseEasting = 500000.0;
    static const double EarthRadius = 6378137.0;
    static const double ScaleFactor = 0.9996;
};

#endif // _UTMDISTANCE_H_
