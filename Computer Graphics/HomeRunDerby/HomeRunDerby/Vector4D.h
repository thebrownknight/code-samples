//
//  Vector4D.h
//  HomeRunDerby
//
//  Created by Nikhil Venkatesh on 10/16/13.
//  Copyright (c) 2013 Nikhil Venkatesh. All rights reserved.
//

#ifndef HomeRunDerby_Vector4D_h
#define HomeRunDerby_Vector4D_h

#include "Vector3D.h"

const float DEGS2RADIANS = 3.141593f / 180;

///////////////////////////////////////////////////////////////////////////////
// Utility 4D vector class.
///////////////////////////////////////////////////////////////////////////////
class Vector4D
{
private:
    float x;
    float y;
    float z;
    float w;
    
public:
    // constructors
    Vector4D() : x(0), y(0), z(0), w(1) {};
    Vector4D(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
    
    // set the x,y,z, and w components of the vector
    inline void set(float x, float y, float z, float w) {
        this->x = x; this->y = y; this->z = z; this->w = w;
    }
    
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    float getW() const { return w; }
    
    // get the length of the vector
    inline float length() const {
        return sqrtf(x*x + y*y + z*z + w*w);
    }
    
    // distance between two vectors
    inline float distance(const Vector4D& vec) const {
        return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z) + (vec.w-w)*(vec.w-w));
    }
    
    // normalize the vector
    inline Vector4D& normalize() {
        //NOTE: leave w-component untouched
        //@@const float EPSILON = 0.000001f;
        float xxyyzz = x*x + y*y + z*z;
        //@@if(xxyyzz < EPSILON)
        //@@    return *this; // do nothing if it is zero vector
        
        //float invLength = invSqrt(xxyyzz);
        float invLength = 1.0f / sqrtf(xxyyzz);
        x *= invLength;
        y *= invLength;
        z *= invLength;
        return *this;
    }
    
    // dot product
    inline float dot(const Vector4D& rhs) const {
        return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
    }
    
    // cross product
    inline Vector4D cross(const Vector4D& rhs) const {
        return Vector4D(y*rhs.z - z*rhs.y,  z*rhs.x - x*rhs.z,  x*rhs.y - y*rhs.x, 1);
    }
    
    // compare with epsilon
    inline bool equal(const Vector4D& rhs, float epsilon) const {
        return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
        fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
    }
    
    // OPERATORS
    // unary operator (negate)
    inline Vector4D operator-() const {
        return Vector4D(-x, -y, -z, -w);
    }
    // add two vectors together, return a NEW vector object
    inline Vector4D operator+(const Vector4D& rhs) const {
        return Vector4D(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
    }
    // subtract two vectors together, return a NEW vector object
    inline Vector4D operator-(const Vector4D& rhs) const {
        return Vector4D(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
    }
    // add another vector to the current one and update this object
    inline Vector4D& operator+=(const Vector4D& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
    }
    // subtract another vector from current vector and update this object
    inline Vector4D& operator-=(const Vector4D& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
    }
    // multiply vector by a scalar
    inline Vector4D operator*(const float a) const {
        return Vector4D(x*a, y*a, z*a, w*a);
    }
    // multiply vector by another vector, create a new one to return
    inline Vector4D operator*(const Vector4D& rhs) const {
        return Vector4D(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
    }
    // scale and update this object
    inline Vector4D& operator*=(const float a) {
        x *= a; y *= a; z *= a; w *= a; return *this;
    }
    // multiply vector by another vector, and update this object
    inline Vector4D& operator*=(const Vector4D& rhs) {
        x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
    }
    // divide this vector by a scalar, return a new object
    inline Vector4D operator/(const float a) const {
        return Vector4D(x/a, y/a, z/a, w/a);
    }
    // divide this vector by a scalar and update this object
    inline Vector4D& operator/=(const float a) {
        x /= a; y /= a; z /= a; w /= a; return *this;
    }
    // exact compare, no epsilon
    inline bool operator==(const Vector4D& rhs) const {
        return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
    }
    // exact compare, no epsilon
    inline bool operator!=(const Vector4D& rhs) const {
        return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
    }
    
    // subscript operator v[0], v[1]
    inline float operator[](int index) const {
        return (&x)[index];
    }
    // subscript operator v[0], v[1]
    inline float& operator[](int index) {
        return (&x)[index];
    }
    
    Vector4D& Translate(float dx, float dy, float dz)
    {
        x += dx;
        y += dy;
        z += dz;
        return (*this);
    }
    
    Vector4D& Scale(float sx, float sy, float sz)
    {
        x *= sx;
        y *= sy;
        z *= sz;
        return (*this);
    }
    
    Vector4D& RotateAboutX(float angle)
    {
        float s = sinf(angle * DEGS2RADIANS);
        float c = cosf(angle * DEGS2RADIANS);
        
        float ny = c * y - s * z;
        float nz = c * z + s * y;
        
        y = ny;
        z = nz;
        
        return (*this);
    }
    
    Vector4D& RotateAboutY(float angle)
    {
        float s = sinf(angle * DEGS2RADIANS);
        float c = cosf(angle * DEGS2RADIANS);
        
        float nx = c * x + s * z;
        float nz = c * z - s * x;
        
        x = nx;
        z = nz;
        
        return (*this);
    }
    
    Vector4D& RotateAboutZ(float angle)
    {
        float s = sinf(angle * DEGS2RADIANS);
        float c = cosf(angle * DEGS2RADIANS);
        
        float nx = c * x - s * y;
        float ny = c * y + s * x;
        
        x = nx;
        y = ny;
        
        return (*this);
    }
    
    Vector4D& RotateAboutAxis(float angle, const Vector3D& axis)
    {
        float s = sinf(angle * DEGS2RADIANS);
        float c = cosf(angle * DEGS2RADIANS);
        float k = 1.0F - c;
        
        float nx = x * (c + k * axis.getX() * axis.getX()) + y * (k * axis.getX() * axis.getY() - s * axis.getZ()) + z * (k * axis.getX() * axis.getZ() + s * axis.getY());
        float ny = x * (k * axis.getX() * axis.getY() + s * axis.getZ()) + y * (c + k * axis.getY() * axis.getY()) + z * (k * axis.getY() * axis.getZ() - s * axis.getX());
        float nz = x * (k * axis.getX() * axis.getZ() - s * axis.getY()) + y * (k * axis.getY() * axis.getZ() + s * axis.getX()) + z * (c + k * axis.getZ() * axis.getZ());
        
        x = nx;
        y = ny;
        z = nz;
        
        return (*this);
    }
    
    float* makeArray() {
        float *a_ret = new float[4];
        a_ret[0] = x;
        a_ret[1] = y;
        a_ret[2] = z;
        a_ret[3] = w;
        return a_ret;
    }
};
// END OF VECTOR4D /////////////////////////////////////////////////////////////
#endif

