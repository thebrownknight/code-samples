//
//  Matrix4x4.h
//  HomeRunDerby
//
//  Created by Hadoop on 12/2/13.
//  Copyright (c) 2013 HomeRunDerby. All rights reserved.
//

#ifndef __HomeRunDerby__Matrix4x4__
#define __HomeRunDerby__Matrix4x4__

#include "Vector3D.h"
#include "Vector4D.h"
#include <iostream>
///////////////////////////////////////////////////////////////////////////
// 4x4 matrix
///////////////////////////////////////////////////////////////////////////
class Matrix4x4
{
public:
    // constructors
    Matrix4x4();  // init with identity
    Matrix4x4(const float src[16]);
    Matrix4x4(float xx, float xy, float xz, float xw,
              float yx, float yy, float yz, float yw,
              float zx, float zy, float zz, float zw,
              float wx, float wy, float wz, float ww);
    
    void        set(const float src[16]);
    void        set(float xx, float xy, float xz, float xw,
                    float yx, float yy, float yz, float yw,
                    float zx, float zy, float zz, float zw,
                    float wx, float wy, float wz, float ww);
    void        set(int rowIndex, int colIndex, const float val);
    void        setRow(int index, const float row[4]);
    void        setRow(int index, const Vector3D& v);
    void        setRow(int index, const Vector4D& v);
    void        setColumn(int index, const float col[4]);
    void        setColumn(int index, const Vector3D& v);
    void        setColumn(int index, const Vector4D& v);
    
    const float* get() const;
    const float* getTranspose();                        // return transposed matrix
    float        getDeterminant();
    
    Matrix4x4&    identity();
    Matrix4x4&    transpose();                            // transpose itself and return reference
    
    // transform matrix
    Matrix4x4&    translate(float x, float y, float z);   // translation by (x,y,z)
    Matrix4x4&    translate(const Vector3D& v);            //
    Matrix4x4&    rotate(float angle, const Vector3D& axis); // rotate angle(degree) along the given axis
    Matrix4x4&    rotate(float angle, float x, float y, float z);
    Matrix4x4&    rotateX(float angle);                   // rotate on X-axis with degree
    Matrix4x4&    rotateY(float angle);                   // rotate on Y-axis with degree
    Matrix4x4&    rotateZ(float angle);                   // rotate on Z-axis with degree
    Matrix4x4&    scale(float scale);                     // uniform scale
    Matrix4x4&    scale(float sx, float sy, float sz);    // scale by (sx, sy, sz) on each axis
    
    // operators
    Matrix4x4     operator+(const Matrix4x4& rhs) const;    // add rhs
    Matrix4x4     operator-(const Matrix4x4& rhs) const;    // subtract rhs
    Matrix4x4&    operator+=(const Matrix4x4& rhs);         // add rhs and update this object
    Matrix4x4&    operator-=(const Matrix4x4& rhs);         // subtract rhs and update this object
    Vector4D     operator*(const Vector4D& rhs) const;    // multiplication: v' = M * v
    Matrix4x4     operator*(const Matrix4x4& rhs) const;    // multiplication: M3 = M1 * M2
    Matrix4x4&    operator*=(const Matrix4x4& rhs);         // multiplication: M1' = M1 * M2
    bool        operator==(const Matrix4x4& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Matrix4x4& rhs) const;   // exact compare, no epsilon
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]
    
    friend Matrix4x4 operator-(const Matrix4x4& m);                     // unary operator (-)
    friend Matrix4x4 operator*(float scalar, const Matrix4x4& m);       // pre-multiplication
    friend Vector3D operator*(const Vector3D& vec, const Matrix4x4& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& m);
    
protected:
    
private:
    float       getCofactor(float m0, float m1, float m2,
                            float m3, float m4, float m5,
                            float m6, float m7, float m8);
    
    float m[16];
    float transpose_m[16];                                       // transpose m
    
};

#endif /* defined(__HomeRunDerby__Matrix4x4__) */
