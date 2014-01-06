//
//  Vector3D.h
//  HomeRunDerby
//
//  Created by Hadoop on 12/2/13.
//  Copyright (c) 2013 HomeRunDerby. All rights reserved.
//

#ifndef HomeRunDerby_Vector3D_h
#define HomeRunDerby_Vector3D_h

#include <iostream>
#include "math.h"

const float DEGREES2RADIANS = 3.141593f / 180;

class Vector3D
{
private:
    double m_elements[3];   // the array of floats for the x,y, and z components of the vector
    
    void initialize(double x, double y, double z) {
        m_elements[0] = x;
        m_elements[1] = y;
        m_elements[2] = z;
    }
    void initialize(const float* values) {
        if (nullptr == values)
            this->initialize(0.0, 0.0, 0.0);
        else {
            m_elements[0] = values[0];
            m_elements[1] = values[1];
            m_elements[2] = values[2];
        }
    }
    void initialize(const double* values) {
        if (nullptr == values)
            this->initialize(0.0, 0.0, 0.0);
        else {
            m_elements[0] = values[0];
            m_elements[1] = values[1];
            m_elements[2] = values[2];
        }
    }
    void add(const Vector3D& v) {
        m_elements[0] += v.m_elements[0];
        m_elements[1] += v.m_elements[1];
        m_elements[2] += v.m_elements[2];
    }
    void subtract(const Vector3D& v) {
        m_elements[0] -= v.m_elements[0];
        m_elements[1] -= v.m_elements[1];
        m_elements[2] -= v.m_elements[2];
    }
    
public:
    // unary operator (negate)
    inline Vector3D operator-() const {
        return Vector3D(-m_elements[0], -m_elements[1], -m_elements[2]);
    }
    
    Vector3D() { this->initialize(0.0, 0.0, 0.0); }
    Vector3D(float x, float y, float z) {
        this->initialize(x, y, z);
    }
    Vector3D(double x, double y, double z) {
        this->initialize(x, y, z);
    }
    explicit Vector3D(const float* values) {
        this->initialize(values);
    }
    explicit Vector3D(const double* values) {
        this->initialize(values);
    }
    // copy constructor that initializes a new vector with values from another vector
    Vector3D(const Vector3D& rhs) {
        this->initialize(rhs.m_elements);
    }
    
    double getX() const { return m_elements[0]; }
    double getY() const { return m_elements[1]; }
    double getZ() const { return m_elements[2]; }
    double operator[] (int i) const { return m_elements[i]; }
    const double* data() const { return m_elements; }
    
    void assign(float x, float y, float z) {
        this->initialize(x, y, z);
    }
    void assign(double x, double y, double z) {
        this->initialize(x, y, z);
    }
    void assign(const float* values) { this->initialize(values); }
    void assign(const double* values) { this->initialize(values); }
    void setX(double x) { m_elements[0] = x; }
    void setY(double y) { m_elements[1] = y; }
    void setZ(double z) { m_elements[2] = z; }
    
    // setting one Vector3D object equal to another one
    Vector3D& operator =(const Vector3D& rhs) {
        if (this != &rhs)
            this->initialize(rhs.m_elements);
        return *this;
    }
    
    // check if two vectors are equal
    bool operator ==(const Vector3D& rhs) const {
        return rhs.m_elements[0] == m_elements[0] &&
        rhs.m_elements[1] == m_elements[1] &&
        rhs.m_elements[2] == m_elements[2];
    }
    
    // check if two vectors are not equal
    bool operator !=(const Vector3D& rhs) const {
        return !(rhs == *this);
    }
    // gets the length of the vector by taking the square root of the dot product with itself
    double length() const {
        return sqrt(this->dotProduct(*this));
    }
    
    // find the distance between two vectors
    double distance(const Vector3D& rhs) const {
        return sqrt(this->dotProduct(*this - rhs));
    }
    
    // find the dot product of two vectors
    double dotProduct(const Vector3D& rhs) const {
        return (m_elements[0]*rhs.m_elements[0]) +
        (m_elements[1]*rhs.m_elements[1]) +
        (m_elements[2]*rhs.m_elements[2]);
    }
    
    // find the cross product of two vectors
    Vector3D crossProduct(const Vector3D& rhs) const {
        return Vector3D(
                        m_elements[1]*rhs.m_elements[2] - rhs.m_elements[1]*m_elements[2],
                        -(m_elements[0]*rhs.m_elements[2] - rhs.m_elements[0]*m_elements[2]),
                        m_elements[0]*rhs.m_elements[1] - rhs.m_elements[0]*m_elements[1]);
    }
    
    Vector3D& RotateAboutX(float angle)
    {
        float s = sinf(angle * DEGREES2RADIANS);
        float c = cosf(angle * DEGREES2RADIANS);
        
        float ny = c * m_elements[1] - s * m_elements[2];
        float nz = c * m_elements[2] + s * m_elements[1];
        
        m_elements[1] = ny;
        m_elements[2] = nz;
        
        return (*this);
    }
    
    Vector3D& RotateAboutY(float angle)
    {
        float s = sinf(angle * DEGREES2RADIANS);
        float c = cosf(angle * DEGREES2RADIANS);
        
        float nx = c * m_elements[0] + s * m_elements[2];
        float nz = c * m_elements[2] - s * m_elements[0];
        
        m_elements[0] = nx;
        m_elements[2] = nz;
        
        return (*this);
    }
    
    Vector3D& RotateAboutZ(float angle)
    {
        float s = sinf(angle * DEGREES2RADIANS);
        float c = cosf(angle * DEGREES2RADIANS);
        
        float nx = c * m_elements[0] - s * m_elements[1];
        float ny = c * m_elements[1] + s * m_elements[0];
        
        m_elements[0] = nx;
        m_elements[1] = ny;
        
        return (*this);
    }
    
    Vector3D& RotateAboutAxis(float angle, const Vector3D& axis)
    {
        float s = sinf(angle * DEGREES2RADIANS);
        float c = cosf(angle * DEGREES2RADIANS);
        float k = 1.0F - c;
        float x = m_elements[0];
        float y = m_elements[1];
        float z = m_elements[2];
        
        float nx = x * (c + k * axis.getX() * axis.getX()) + y * (k * axis.getX() * axis.getY() - s * axis.getZ()) + z * (k * axis.getX() * axis.getZ() + s * axis.getY());
        float ny = x * (k * axis.getX() * axis.getY() + s * axis.getZ()) + y * (c + k * axis.getY() * axis.getY()) + z * (k * axis.getY() * axis.getZ() - s * axis.getX());
        float nz = x * (k * axis.getX() * axis.getZ() - s * axis.getY()) + y * (k * axis.getY() * axis.getZ() + s * axis.getX()) + z * (c + k * axis.getZ() * axis.getZ());
        
        x = nx;
        y = ny;
        z = nz;
        
        return (*this);
    }
    
    Vector3D operator + (const Vector3D& rhs) const {
        Vector3D result(*this); result.add(rhs); return result;
    }
    Vector3D operator - (const Vector3D& rhs) const {
        Vector3D result(*this); result.subtract(rhs); return result;
    }
    
    void normalize() { this->scale(1.0 / this->length()); }
    void scale(double scalar) {
        m_elements[0] *= scalar;
        m_elements[1] *= scalar;
        m_elements[2] *= scalar;
    }
    Vector3D& operator += (const Vector3D& rhs) {
        this->add(rhs); return *this;
    }
    Vector3D& operator -= (const Vector3D& rhs) {
        this->subtract(rhs); return *this;
    }
    
    friend Vector3D operator * (double scalar, const Vector3D& v) {
        Vector3D result(v);
        result.scale(scalar);
        return result;
    }
    friend Vector3D operator * (const Vector3D& v, double scalar) {
        return scalar * v;
    }
};

#endif /* defined(__HomeRunDerby__Vector3D__) */