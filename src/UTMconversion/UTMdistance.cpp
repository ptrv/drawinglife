#include "UTMdistance.h"

double UTMdistance::GetScaleFactor(double e)
{
    return ScaleFactor / cos(e / EarthRadius);
}
double UTMdistance::calcEuclidean(double fromE, double fromN, double toE, double toN)
{
    return sqrt(pow(fromE - toE, 2) + pow(fromN - toN, 2));
}
double UTMdistance::calcLinearAverage(double fromE, double fromN, double toE, double toN)
{
    double factor = GetScaleFactor((fromE + toE) * 0.5 - FalseEasting);
    return sqrt(pow((toE - fromE) * factor, 2) + pow(toN - fromN, 2));
}
double UTMdistance::calcSimpsons(double fromE, double fromN, double toE, double toN)
{
    double f1 = GetScaleFactor(fromE - FalseEasting);
    double f2 = GetScaleFactor(toE - FalseEasting);
    double fm = GetScaleFactor((fromE + toE) * 0.5 - FalseEasting);
    double dx = (toE - fromE) / 6 * (f1 + 4 * fm + f2);
    double dy = toN - fromN;
    return sqrt(pow(dx, 2) + pow(dy, 2));
}
double UTMdistance::calcIntegral(double fromE, double fromN, double toE, double toN)
{
    double er0 = (fromE - FalseEasting) / (2 * EarthRadius); ;
    double er1 = (toE - FalseEasting) / (2 * EarthRadius);
    double c0 = cos(er0);
    double c1 = cos(er1);
    double s0 = sin(er0);
    double s1 = sin(er1);
    double dx = ScaleFactor * EarthRadius * (log(c1 + s1) - log(c1 - s1) - log(c0 + s0) + log(c0 - s0));
    double dy = toN - fromN;
    return sqrt(pow(dx, 2) + pow(dy, 2));
}
