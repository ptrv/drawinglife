//constants.h

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "LatLong-UTMconversion.h"

const double PI = 3.14159265;
const double FOURTHPI = PI / 4;
const double deg2rad = PI / 180;
const double rad2deg = 180.0 / PI;

static Ellipsoid ellipsoid[] = 
{//  id, Ellipsoid name, Equatorial Radius, square of eccentricity	
	Ellipsoid( -1, (char *)"Placeholder", 0, 0),//placeholder only, To allow array indices to match id numbers
	Ellipsoid( 1, (char *)"Airy", 6377563, 0.00667054),
	Ellipsoid( 2, (char *)"Australian National", 6378160, 0.006694542),
	Ellipsoid( 3, (char *)"Bessel 1841", 6377397, 0.006674372),
	Ellipsoid( 4, (char *)"Bessel 1841 (Nambia) ", 6377484, 0.006674372),
	Ellipsoid( 5, (char *)"Clarke 1866", 6378206, 0.006768658),
	Ellipsoid( 6, (char *)"Clarke 1880", 6378249, 0.006803511),
	Ellipsoid( 7, (char *)"Everest", 6377276, 0.006637847),
	Ellipsoid( 8, (char *)"Fischer 1960 (Mercury) ", 6378166, 0.006693422),
	Ellipsoid( 9, (char *)"Fischer 1968", 6378150, 0.006693422),
	Ellipsoid( 10, (char *)"GRS 1967", 6378160, 0.006694605),
	Ellipsoid( 11, (char *)"GRS 1980", 6378137, 0.00669438),
	Ellipsoid( 12, (char *)"Helmert 1906", 6378200, 0.006693422),
	Ellipsoid( 13, (char *)"Hough", 6378270, 0.00672267),
	Ellipsoid( 14, (char *)"International", 6378388, 0.00672267),
	Ellipsoid( 15, (char *)"Krassovsky", 6378245, 0.006693422),
	Ellipsoid( 16, (char *)"Modified Airy", 6377340, 0.00667054),
	Ellipsoid( 17, (char *)"Modified Everest", 6377304, 0.006637847),
	Ellipsoid( 18, (char *)"Modified Fischer 1960", 6378155, 0.006693422),
	Ellipsoid( 19, (char *)"South American 1969", 6378160, 0.006694542),
	Ellipsoid( 20, (char *)"WGS 60", 6378165, 0.006693422),
	Ellipsoid( 21, (char *)"WGS 66", 6378145, 0.006694542),
	Ellipsoid( 22, (char *)"WGS-72", 6378135, 0.006694318),
	Ellipsoid( 23, (char *)"WGS-84", 6378137, 0.00669438)
};

#endif
