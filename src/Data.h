/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DATA_H_
#define _DATA_H_

class GpsPoint {
	double latitude;
	double longitude;
	double elevation;
	std::string timestamp;
	
public:
	GpsPoint()
	:
	latitude(0.0),
	longitude(0.0),
	elevation(0.0),
	timestamp("")
	{};
	~GpsPoint(){};
	void setGpsPoint(double lat, double lon, double ele, std::string time)
	{
		latitude = lat;
		longitude = lon;
		elevation = ele;
		timestamp = time;
	}
	double getLatitude() const { return latitude; }
	double getLongitude() const { return longitude; }
	double getElevation() const { return elevation; }
	std::string getTimestamp() const { return timestamp; }
};

class GpsSegment {
	std::vector<GpsPoint*> points;
	int segment;
public:
	GpsSegment()
	:
	points(NULL),
	segment(0)
	{};
	~GpsSegment(){};
	void setGpsSegment(std::vector<GpsPoint*> pts, int seg)
	{
		points = pts;
		segment = seg;
	}
	std::vector<GpsPoint*> getPoints() const { return points; }
	int getSegment() const { return segment; }
};

#endif // _DATA_H_
