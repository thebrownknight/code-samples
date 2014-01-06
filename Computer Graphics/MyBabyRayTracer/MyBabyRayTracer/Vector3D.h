//////////////////////////////////////////////////////////
/// Filename: Vector3D
/// Author: Nikhil Venkatesh
/// Date: 11/13/13
/// Description: A utility Vector class.
//////////////////////////////////////////////////////////

#ifndef MyBabyRayTracer_Vector3D_h
#define MyBabyRayTracer_Vector3D_h

#include "math.h"

class Vector3D {
    double x, y, z;
    
public:
    
    Vector3D();
    
    Vector3D (double, double, double);
    
    // Method functions
    double getX() { return x; }
    double getY() { return y; }
    double getZ() { return z; }
    
    // Get the magnitude of the vector
    double magnitude () {
        return sqrt((x*x) + (y*y) + (z*z));
    }
    
    Vector3D normalize () {
        double magnitude = sqrt((x*x) + (y*y) + (z*z));
        return Vector3D (x/magnitude, y/magnitude, z/magnitude);
    }
    
    Vector3D negative () {
        return Vector3D (-x, -y, -z);
    }
    
    double dotProduct (Vector3D v) {
        return x * v.getX() + y * v.getY() + z * v.getZ();
    }
    
    Vector3D crossProduct (Vector3D v) {
        return Vector3D (y*v.getZ() - z*v.getY(), z*v.getX() - x*v.getZ(), x*v.getY() - y*v.getX());
    }
    
    Vector3D addVector (Vector3D v) {
        return Vector3D (x + v.getX(), y + v.getY(), z + v.getZ());
    }
    
    Vector3D multScalar (double scalar) {
        return Vector3D (x * scalar, y * scalar, z * scalar);
    }
};

Vector3D::Vector3D () {
	x = 0;
	y = 0;
	z = 0;
}

Vector3D::Vector3D (double i, double j, double k) {
	x = i;
	y = j;
	z = k;
}
#endif

