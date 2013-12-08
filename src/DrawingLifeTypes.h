/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _DRAWINGLIFETYPES_H_
#define _DRAWINGLIFETYPES_H_


struct ZoomAnimFrame
{
	float frameTime;
	int frameZoom;
	double frameCenterX;
	double frameCenterY;
	std::string timestamp;
	int gpsId;

};
/**
 * \brief Timeline value with time and id.
 *
 */
struct TimelineObject
{
    std::string timeString;
    time_t secs;
    int id;
    int gpsid;
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
    double speed;
    double lon0;
};

struct LocationImageData
{
    std::string name;
    std::string path;
    GpsCoord gps;
    float width;
    float height;
    int alpha;
    int anchorType;
    float anchorX;
    float anchorY;
    bool anchorShow;
};

struct CurrentImageData
{
    std::string path;
    float width;
    float height;
    int alpha;
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

// -----------------------------------------------------------------------------
// templated version of openFramworks datatypes.
// -----------------------------------------------------------------------------

//----------------------------------------------------------
// ofxPoint
//----------------------------------------------------------
template < class T >
class ofxPoint {
  public:
    virtual ~ofxPoint(){}

    ofxPoint() : x(), y(), z() {}
    ofxPoint(T _x) : x(_x), y(), z() {}
    ofxPoint(T _x, T _y) : x(_x), y(_y), z() {}
    ofxPoint(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

    ofxPoint( const ofxPoint & pnt)
    {
        x = pnt.x;
        y = pnt.y;
        z = pnt.z;
    }

    void set(T _x, T _y, T _z = 0)
    {
        x = _x;
        y = _y;
        z = _z;
    }


	//------ Operators:

  	//Negative
    ofxPoint operator-() const
    {
        return ofxPoint( -x, -y, -z );
    }

    //equality
    bool operator==( const ofxPoint& pnt )
    {
        return (x == pnt.x) && (y == pnt.y) && (z == pnt.z);
    }

	//inequality
    bool operator!=( const ofxPoint& pnt )
    {
        return (x != pnt.x) || (y != pnt.y) || (z != pnt.z);
    }

	//Set
    ofxPoint & operator=( const ofxPoint& pnt )
    {
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
		return *this;
	}

    ofxPoint& operator=( const T& val )
    {
		x = val;
		y = val;
		z = val;
		return *this;
	}

	// Add
    ofxPoint operator+( const ofxPoint& pnt ) const
    {
        return ofxPoint( x+pnt.x, y+pnt.y, z+pnt.z );
    }

    ofxPoint operator+( const T& val ) const
    {
        return ofxPoint( x+val, y+val, z+val );
    }

    ofxPoint & operator+=( const ofxPoint& pnt )
    {
        x+=pnt.x;
        y+=pnt.y;
        z+=pnt.z;
        return *this;
    }

    ofxPoint & operator+=( const T& val )
    {
        x+=val;
        y+=val;
        z+=val;
        return *this;
    }

	// Subtract
    ofxPoint operator-(const ofxPoint& pnt) const
    {
        return ofxPoint( x-pnt.x, y-pnt.y, z-pnt.z );
    }

    ofxPoint operator-(const T& val) const
    {
        return ofxPoint( x-val, y-val, z-val);
    }

    ofxPoint & operator-=( const ofxPoint& pnt )
    {
        x -= pnt.x;
        y -= pnt.y;
        z -= pnt.z;
        return *this;
    }

    ofxPoint& operator-=( const T& val )
    {
        x -= val;
        y -= val;
        z -= val;
        return *this;
    }

	// Multiply
    ofxPoint operator*( const ofxPoint& pnt ) const
    {
        return ofxPoint( x*pnt.x, y*pnt.y, z*pnt.z );
    }

    ofxPoint operator*(const T& val) const
    {
        return ofxPoint( x*val, y*val, z*val);
    }

    ofxPoint & operator*=( const ofxPoint& pnt )
    {
        x*=pnt.x;
        y*=pnt.y;
        z*=pnt.z;
        return *this;
    }

    ofxPoint & operator*=( const T& val )
    {
        x*=val;
        y*=val;
        z*=val;
        return *this;
    }

	// Divide
    ofxPoint operator/( const ofxPoint& pnt ) const
    {
        return ofxPoint(pnt.x!=0 ? x/pnt.x : x,
                        pnt.y!=0 ? y/pnt.y : y,
                        pnt.z!=0 ? z/pnt.z : z);
    }

    ofxPoint operator/( const T&val ) const
    {
        if( val != 0)
        {
            return ofxPoint( x/val, y/val, z/val );
		}
        return ofxPoint(x, y, z );
    }

    ofxPoint& operator/=( const ofxPoint& pnt )
    {
        pnt.x!=0 ? x/=pnt.x : x;
        pnt.y!=0 ? y/=pnt.y : y;
        pnt.z!=0 ? z/=pnt.z : z;

        return *this;
    }

    ofxPoint& operator/=( const T&val )
    {
		if( val != 0 ){
			x /= val;
			y /= val;
			z /= val;
		}

		return *this;
    }

	// union allows us to access the coordinates through
    // both an array 'v' and 'x', 'y', 'z' member varibles
    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        T v[3];
    };

    T& operator[]( int n )
    {
		return v[n];
	}
};

//----------------------------------------------------------
// ofxRectangled
//----------------------------------------------------------

template < class T >
class ofxRectangle
{
  public:
    ofxRectangle() :pos(), width(), height() {}
    ofxRectangle(T _x, T _y, T _w, T _h) : pos(_x, _y), width(_w), height(_h) {}
    virtual ~ofxRectangle(){}

    void set(T px, T py, T w, T h)
    {
        pos.set(px, py);
		width	= w;
		height	= h;
	}

    void set(ofxPoint<T> p, T w, T h)
    {
        pos = p;
        width	= w;
		height	= h;
	}

    void setFromCenter(T px, T py, T w, T h)
    {
        pos.set(px - (w / 2), py - (h / 2));
		width	= w;
		height	= h;
	}

    void setFromCenter(ofxPoint<T> p, T w, T h)
    {
        pos.set(p.x - (w / 2), p.y - (h / 2));
		width	= w;
		height	= h;
	}

    ofxPoint<T> getCenter()
    {
        return ofxPoint<T>(pos.x + width / 2, pos.y + height / 2, 0);
	}

    bool inside (ofxPoint<T> p){
		return inside(p.x, p.y);
	}

    bool inside(T px, T py){
        if( px > pos.x && py > pos.y && px < pos.x + width
                && py < pos.y + height )
        {
		    return true;
		}
		return false;
	}

    inline T getX() const { return pos.x; }
    inline T getY() const { return pos.y; }
    inline T getWidth() const { return width; }
    inline T getHeight() const { return height; }

    inline void setX(const T newX) { pos.x = newX; }
    inline void setY(const T newY) { pos.y = newY; }
    inline void setWidth(const T newWidth) { width = newWidth; }
    inline void setHeight(const T newHeight) { height = newHeight; }

private:
    ofxPoint<T> pos;
    T width, height;

};

// typedefs

class DBReader;
class GpsData;
class Walk;
class MagicBox;

typedef boost::scoped_ptr<DBReader> DBReaderPtr;
typedef std::vector<ZoomAnimFrame> ZoomAnimFrames;
typedef boost::ptr_vector<GpsData> GpsDataVector;
typedef boost::ptr_vector<Walk> WalkVector;
typedef boost::ptr_vector<MagicBox> MagicBoxVector;
typedef std::vector< std::vector<UtmPoint> > UtmPointVector;

#endif // _DRAWINGLIFETYPES_H_
