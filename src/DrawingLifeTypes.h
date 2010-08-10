#ifndef _DRAWINGLIFETYPES_H_
#define _DRAWINGLIFETYPES_H_


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

// -----------------------------------------------------------------------------
// double version of openFramworks datatypes.
// -----------------------------------------------------------------------------

//----------------------------------------------------------
// ofxPointd
//----------------------------------------------------------
class ofxPointd {
  public:
	virtual ~ofxPointd(){}

    ofxPointd( double _x=0.0f, double _y=0.0f, double _z=0.0f ) {
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
	ofxRectangled(){ x = y = width = height = 0; };
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
		x		= px - w*0.5f;
		y		= py - h*0.5f;
		width	= w;
		height	= h;
	}

	void setFromCenter(ofxPointd pos, double w, double h){
		x		= pos.x - w*0.5f;
		y		= pos.y - h*0.5f;
		width	= w;
		height	= h;
	}

	ofxPointd getCenter(){
		return ofxPointd(x + width * 0.5f, y + height * 0.5f, 0);
	}

	bool inside (ofxPointd p){
		return inside(p.x, p.y);
	}

	bool inside(double px, double py){
		if( px < x && py < y && px > x + width && py > y + height ){
		    return false;
		}
		return true;
	}

   double x;
   double y;
   double width;
   double height;

};

#endif // _DRAWINGLIFETYPES_H_
