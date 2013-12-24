/*==========================================================
 Copyright (c) avp::ptr, 2013
 ==========================================================*/

#ifndef OFXPOINT_H
#define OFXPOINT_H

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

#endif // OFXPOINT_H
