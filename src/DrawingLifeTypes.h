/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _DRAWINGLIFETYPES_H_
#define _DRAWINGLIFETYPES_H_


/**
 * \brief Timeline value with time and id.
 *
 */
struct TimelineObject
{
    std::string timeString;
    time_t secs;
    int personId;
	int segmentId;
	int pointId;
    bool operator() (TimelineObject lhs, TimelineObject rhs)
    {
        return lhs.secs < rhs.secs;
    }
};

/**
 * \brief Data for the database query
 *
 */
struct DBQueryData
{
    int type;
    int dayStart;
    int dayEnt;
    int monthStart;
    int monthEnd;
    int yearStart;
    int yearEnd;
    std::string city;
};

/**
 * \brief Represents latitude and longitude value of an Gps coordinate.
 *
 */
struct GpsCoord
{
    double lat;
    double lon;
};
/**
 * \brief Represents x and y value of an UTM coordinate.
 *
 */
struct UtmPoint
{
    double x;
    double y;
};

struct GpsDataIndex
{
    int gpsPoint;
    int gpsSegment;
    int point;
};

struct GpsRegion
{
    double lon0;
    double minLon;
    double maxLon;
};

struct Point2D
{
	float x;
	float y;
};

inline Point2D MakePoint2D( float x, float y)
{
	Point2D point;
	point.x = x;
	point.y = y;
	return point;
}

struct ColorRGBA
{
	int r;
	int g;
	int b;
	int a;
};

inline ColorRGBA MakeColorRGBA( int r, int g, int b, int a)
{
	ColorRGBA color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

inline int MakeHexARGB( int r, int g, int b, int a = 255)
{
	return 0xFF000000 | a << 24 | r << 16 | g << 8 | b;
}

inline int MakeHexARGB( ColorRGBA color)
{
	return 0xFF000000 | color.a << 24 | color.r << 16 | color.g << 8 | color.b;
}

inline void ofSetColorWithAlpha( int hexColor)
{
	int a = (hexColor >> 24) & 0xff;
	int r = (hexColor >> 16) & 0xff;
	int g = (hexColor >>  8) & 0xff;
	int b = (hexColor >>  0) & 0xff;
	ofSetColor(r, g, b, a);
}


inline tm MakeTimeStruct(std::string timeString)
{
	// Parse the date components from the string.
	int year, month, day, hour, min, sec;
	sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &min, &sec);

	// Get current system time and initialize 
	// the tm struct with the current time.
	time_t t;
	time(&t);
	struct tm tm;
	tm = *localtime(&t);

	// Set values for the tm components.
	tm.tm_sec = sec;
	tm.tm_min = min;
	tm.tm_hour = hour;
	tm.tm_mday = day;
	tm.tm_mon = month - 1; // Months are stored with a range from 0 to 11.
	tm.tm_year = year - 1900; // The year is stored as a delta from the reference year 1900.
	// tm_wday will not be set by this function and has the initialization value.
	// tm_yday will not be set by this function and has the initialization value.
	tm.tm_isdst = -1; // Daylight saving = Information is not available.

	return tm;
}



/**
 * \brief Returns the length of an array.
 * \param The array variable.
 * \return The array length. 
**/
template<typename T, int size>
inline int GetArrayLength(T(&)[size]) { return size; }



// -----------------------------------------------------------------------------
// double version of openFramworks datatypes.
// -----------------------------------------------------------------------------

//----------------------------------------------------------
// ofxPointd
//----------------------------------------------------------
class ofxPointd {
  public:
	virtual ~ofxPointd(){}

    ofxPointd( double _x=0.0, double _y=0.0, double _z=0.0 ) {
        x = _x;
        y = _y;
        z = _z;
    }

    ofxPointd( const ofxPointd & pnt){
        x = pnt.x;
        y = pnt.y;
        z = pnt.z;
    }

    void set(double _x, double _y, double _z = 0){
        x = _x;
        y = _y;
        z = _z;
    }


	//------ Operators:

  	//Negative
    ofxPointd operator-() const {
        return ofxPointd( -x, -y, -z );
    }

    //equality
    bool operator==( const ofxPointd& pnt ) {
        return (x == pnt.x) && (y == pnt.y) && (z == pnt.z);
    }

	//inequality
    bool operator!=( const ofxPointd& pnt ) {
        return (x != pnt.x) || (y != pnt.y) || (z != pnt.z);
    }

	//Set
	ofxPointd & operator=( const ofxPointd& pnt ){
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
		return *this;
	}

	ofxPointd & operator=( const double& val ){
		x = val;
		y = val;
		z = val;
		return *this;
	}

	// Add
    ofxPointd operator+( const ofxPointd& pnt ) const {
        return ofxPointd( x+pnt.x, y+pnt.y, z+pnt.z );
    }

    ofxPointd operator+( const double& val ) const {
        return ofxPointd( x+val, y+val, z+val );
    }

	ofxPointd & operator+=( const ofxPointd& pnt ) {
        x+=pnt.x;
        y+=pnt.y;
        z+=pnt.z;
        return *this;
    }

	ofxPointd & operator+=( const double & val ) {
        x+=val;
        y+=val;
        z+=val;
        return *this;
    }

	// Subtract
    ofxPointd operator-(const ofxPointd& pnt) const {
        return ofxPointd( x-pnt.x, y-pnt.y, z-pnt.z );
    }

    ofxPointd operator-(const double& val) const {
        return ofxPointd( x-val, y-val, z-val);
    }

    ofxPointd & operator-=( const ofxPointd& pnt ) {
        x -= pnt.x;
        y -= pnt.y;
        z -= pnt.z;
        return *this;
    }

    ofxPointd & operator-=( const double & val ) {
        x -= val;
        y -= val;
        z -= val;
        return *this;
    }

	// Multiply
    ofxPointd operator*( const ofxPointd& pnt ) const {
        return ofxPointd( x*pnt.x, y*pnt.y, z*pnt.z );
    }

    ofxPointd operator*(const double& val) const {
        return ofxPointd( x*val, y*val, z*val);
    }

    ofxPointd & operator*=( const ofxPointd& pnt ) {
        x*=pnt.x;
        y*=pnt.y;
        z*=pnt.z;
        return *this;
    }

    ofxPointd & operator*=( const double & val ) {
        x*=val;
        y*=val;
        z*=val;
        return *this;
    }

	// Divide
    ofxPointd operator/( const ofxPointd& pnt ) const {
        return ofxPointd( pnt.x!=0 ? x/pnt.x : x , pnt.y!=0 ? y/pnt.y : y, pnt.z!=0 ? z/pnt.z : z );
    }

    ofxPointd operator/( const double &val ) const {
		if( val != 0){
			return ofxPointd( x/val, y/val, z/val );
		}
        return ofxPointd(x, y, z );
    }

    ofxPointd& operator/=( const ofxPointd& pnt ) {
        pnt.x!=0 ? x/=pnt.x : x;
        pnt.y!=0 ? y/=pnt.y : y;
        pnt.z!=0 ? z/=pnt.z : z;

        return *this;
    }

    ofxPointd& operator/=( const double &val ) {
		if( val != 0 ){
			x /= val;
			y /= val;
			z /= val;
		}

		return *this;
    }

	// union allows us to access the coordinates through
    // both an array 'v' and 'x', 'y', 'z' member varibles
    union  {
        struct {
            double x;
            double y;
            double z;
        };
        double v[3];
    };

	double& operator[]( int n ){
		return v[n];
	}
};

//----------------------------------------------------------
// ofxRectangled
//----------------------------------------------------------

class ofxRectangled {
  public:
	ofxRectangled(){ x = y = width = height = 0.0; };
	ofxRectangled(double _x, double _y, double _w, double _h){
		x = _x;
		y = _y;
		width = _w;
		height = _h;
	}
	virtual ~ofxRectangled(){}

	void set(double px, double py, double w, double h){
		x		= px;
		y		= py;
		width	= w;
		height	= h;
	}

	void set(ofxPointd pos, double w, double h){
		x		= pos.x;
		y		= pos.y;
		width	= w;
		height	= h;
	}

	void setFromCenter(double px, double py, double w, double h){
		x		= px - w*0.5;
		y		= py - h*0.5;
		width	= w;
		height	= h;
	}

	void setFromCenter(ofxPointd pos, double w, double h){
		x		= pos.x - w*0.5;
		y		= pos.y - h*0.5;
		width	= w;
		height	= h;
	}

	ofxPointd getCenter(){
		return ofxPointd(x + width * 0.5, y + height * 0.5, 0);
	}

	bool inside (ofxPointd p){
		return inside(p.x, p.y);
	}

	bool inside(double px, double py){
		if( px > x && py > y && px < x + width && py < y + height ){
		    return true;
		}
		return false;
	}

   double x;
   double y;
   double width;
   double height;

};

#endif // _DRAWINGLIFETYPES_H_
