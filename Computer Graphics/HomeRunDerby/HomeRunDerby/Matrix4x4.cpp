//
//  Matrix4x4.cpp
//  HomeRunDerby
//
//  Created by Hadoop on 12/2/13.
//  Copyright (c) 2013 HomeRunDerby. All rights reserved.
//

#include "Matrix4x4.h"

const float DEG2RAD = 3.141593f / 180;

///////////////////////////////////////////////////////////////////////////
// inline functions for Matrix4x4
///////////////////////////////////////////////////////////////////////////
inline Matrix4x4::Matrix4x4()
{
    // initially identity matrix
    identity();
}

inline Matrix4x4::Matrix4x4(const float src[16])
{
    set(src);
}

inline Matrix4x4::Matrix4x4(float xx, float xy, float xz, float xw,
                            float yx, float yy, float yz, float yw,
                            float zx, float zy, float zz, float zw,
                            float wx, float wy, float wz, float ww)
{
    set(xx, xy, xz, xw,  yx, yy, yz, yw,  zx, zy, zz, zw,  wx, wy, wz, ww);
}

inline void Matrix4x4::set(int rowIndex, int colIndex, const float val)
{
    int index = rowIndex * 4 + colIndex;
    m[index] = val;
}


inline void Matrix4x4::set(const float src[16])
{
    m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
    m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
    m[8] = src[8];  m[9] = src[9];  m[10]= src[10]; m[11]= src[11];
    m[12]= src[12]; m[13]= src[13]; m[14]= src[14]; m[15]= src[15];
}



inline void Matrix4x4::set(float xx, float xy, float xz, float xw,
                           float yx, float yy, float yz, float yw,
                           float zx, float zy, float zz, float zw,
                           float wx, float wy, float wz, float ww)
{
    m[0] = xx;  m[1] = xy;  m[2] = xz;  m[3] = xw;
    m[4] = yx;  m[5] = yy;  m[6] = yz;  m[7] = yw;
    m[8] = zx;  m[9] = zy;  m[10]= zz;  m[11]= zw;
    m[12]= wx;  m[13]= wy;  m[14]= wz;  m[15]= ww;
}



inline void Matrix4x4::setRow(int index, const float row[4])
{
    m[index*4] = row[0];  m[index*4 + 1] = row[1];  m[index*4 + 2] = row[2];  m[index*4 + 3] = row[3];
}



inline void Matrix4x4::setRow(int index, const Vector4D& v)
{
    m[index*4] = v.getX();  m[index*4 + 1] = v.getY();  m[index*4 + 2] = v.getZ();  m[index*4 + 3] = v.getW();
}



inline void Matrix4x4::setRow(int index, const Vector3D& v)
{
    m[index*4] = v.getX();  m[index*4 + 1] = v.getY();  m[index*4 + 2] = v.getZ();
}



inline void Matrix4x4::setColumn(int index, const float col[4])
{
    m[index] = col[0];  m[index + 4] = col[1];  m[index + 8] = col[2];  m[index + 12] = col[3];
}



inline void Matrix4x4::setColumn(int index, const Vector4D& v)
{
    m[index] = v.getX();  m[index + 4] = v.getY();  m[index + 8] = v.getZ();  m[index + 12] = v.getW();
}



inline void Matrix4x4::setColumn(int index, const Vector3D& v)
{
    m[index] = v.getX();  m[index + 4] = v.getY();  m[index + 8] = v.getZ();
}



inline const float* Matrix4x4::get() const
{
    return m;
}



inline const float* Matrix4x4::getTranspose()
{
    transpose_m[0] = m[0];   transpose_m[1] = m[4];   transpose_m[2] = m[8];   transpose_m[3] = m[12];
    transpose_m[4] = m[1];   transpose_m[5] = m[5];   transpose_m[6] = m[9];   transpose_m[7] = m[13];
    transpose_m[8] = m[2];   transpose_m[9] = m[6];   transpose_m[10]= m[10];  transpose_m[11]= m[14];
    transpose_m[12]= m[3];   transpose_m[13]= m[7];   transpose_m[14]= m[11];  transpose_m[15]= m[15];
    return transpose_m;
}



inline Matrix4x4& Matrix4x4::identity()
{
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
    return *this;
}



inline Matrix4x4 Matrix4x4::operator+(const Matrix4x4& rhs) const
{
    return Matrix4x4(m[0]+rhs[0],   m[1]+rhs[1],   m[2]+rhs[2],   m[3]+rhs[3],
                     m[4]+rhs[4],   m[5]+rhs[5],   m[6]+rhs[6],   m[7]+rhs[7],
                     m[8]+rhs[8],   m[9]+rhs[9],   m[10]+rhs[10], m[11]+rhs[11],
                     m[12]+rhs[12], m[13]+rhs[13], m[14]+rhs[14], m[15]+rhs[15]);
}

inline Matrix4x4 Matrix4x4::operator-(const Matrix4x4& rhs) const
{
    return Matrix4x4(m[0]-rhs[0],   m[1]-rhs[1],   m[2]-rhs[2],   m[3]-rhs[3],
                     m[4]-rhs[4],   m[5]-rhs[5],   m[6]-rhs[6],   m[7]-rhs[7],
                     m[8]-rhs[8],   m[9]-rhs[9],   m[10]-rhs[10], m[11]-rhs[11],
                     m[12]-rhs[12], m[13]-rhs[13], m[14]-rhs[14], m[15]-rhs[15]);
}

inline Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& rhs)
{
    m[0] += rhs[0];    m[1] += rhs[1];    m[2] += rhs[2];    m[3] += rhs[3];
    m[4] += rhs[4];    m[5] += rhs[5];    m[6] += rhs[6];    m[7] += rhs[7];
    m[8] += rhs[8];    m[9] += rhs[9];    m[10] += rhs[10];  m[11] += rhs[11];
    m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
    return *this;
}

inline Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& rhs)
{
    m[0] -= rhs[0];    m[1] -= rhs[1];    m[2] -= rhs[2];    m[3] -= rhs[3];
    m[4] -= rhs[4];    m[5] -= rhs[5];    m[6] -= rhs[6];    m[7] -= rhs[7];
    m[8] -= rhs[8];    m[9] -= rhs[9];    m[10] -= rhs[10];  m[11] -= rhs[11];
    m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
    return *this;
}

// multiplying two matrices together
inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& n) const
{
    return Matrix4x4(m[0]*n[0]  + m[1]*n[4]  + m[2]*n[8]  + m[3]*n[12],   m[0]*n[1]  + m[1]*n[5]  + m[2]*n[9]  + m[3]*n[13],   m[0]*n[2]  + m[1]*n[6]  + m[2]*n[10]  + m[3]*n[14],   m[0]*n[3]  + m[1]*n[7]  + m[2]*n[11]  + m[3]*n[15],
                     m[4]*n[0]  + m[5]*n[4]  + m[6]*n[8]  + m[7]*n[12],   m[4]*n[1]  + m[5]*n[5]  + m[6]*n[9]  + m[7]*n[13],   m[4]*n[2]  + m[5]*n[6]  + m[6]*n[10]  + m[7]*n[14],   m[4]*n[3]  + m[5]*n[7]  + m[6]*n[11]  + m[7]*n[15],
                     m[8]*n[0]  + m[9]*n[4]  + m[10]*n[8] + m[11]*n[12],  m[8]*n[1]  + m[9]*n[5]  + m[10]*n[9] + m[11]*n[13],  m[8]*n[2]  + m[9]*n[6]  + m[10]*n[10] + m[11]*n[14],  m[8]*n[3]  + m[9]*n[7]  + m[10]*n[11] + m[11]*n[15],
                     m[12]*n[0] + m[13]*n[4] + m[14]*n[8] + m[15]*n[12],  m[12]*n[1] + m[13]*n[5] + m[14]*n[9] + m[15]*n[13],  m[12]*n[2] + m[13]*n[6] + m[14]*n[10] + m[15]*n[14],  m[12]*n[3] + m[13]*n[7] + m[14]*n[11] + m[15]*n[15]);
}

// Multiplying two matrices, storing result in the first matrix
inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs)
{
    *this = *this * rhs;
    return *this;
}


// doing a deep comparison of two matrices
inline bool Matrix4x4::operator==(const Matrix4x4& n) const
{
    return (m[0] == n[0])   && (m[1] == n[1])   && (m[2] == n[2])   && (m[3] == n[3]) &&
    (m[4] == n[4])   && (m[5] == n[5])   && (m[6] == n[6])   && (m[7] == n[7]) &&
    (m[8] == n[8])   && (m[9] == n[9])   && (m[10] == n[10]) && (m[11] == n[11]) &&
    (m[12] == n[12]) && (m[13] == n[13]) && (m[14] == n[14]) && (m[15] == n[15]);
}


// doing a deep inequality check of two matrices
inline bool Matrix4x4::operator!=(const Matrix4x4& n) const
{
    return (m[0] != n[0])   || (m[1] != n[1])   || (m[2] != n[2])   || (m[3] != n[3]) ||
    (m[4] != n[4])   || (m[5] != n[5])   || (m[6] != n[6])   || (m[7] != n[7]) ||
    (m[8] != n[8])   || (m[9] != n[9])   || (m[10] != n[10]) || (m[11] != n[11]) ||
    (m[12] != n[12]) || (m[13] != n[13]) || (m[14] != n[14]) || (m[15] != n[15]);
}


// subscript operator for retrieving a value from the matrix
inline float Matrix4x4::operator[](int index) const
{
    return m[index];
}



inline float& Matrix4x4::operator[](int index)
{
    return m[index];
}



inline Matrix4x4 operator-(const Matrix4x4& rhs)
{
    return Matrix4x4(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}



inline Matrix4x4 operator*(float s, const Matrix4x4& rhs)
{
    return Matrix4x4(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}



inline Vector4D operator*(const Vector4D& v, const Matrix4x4& m)
{
    return Vector4D(v.getX()*m[0] + v.getY()*m[4] + v.getZ()*m[8] + v.getW()*m[12],
                    v.getX()*m[1] + v.getY()*m[5] + v.getZ()*m[9] + v.getW()*m[13],
                    v.getX()*m[2] + v.getY()*m[6] + v.getZ()*m[10] + v.getW()*m[14],
                    v.getX()*m[3] + v.getY()*m[7] + v.getZ()*m[11] + v.getW()*m[15]);
}

inline Vector4D Matrix4x4::operator*(const Vector4D& rhs) const
{
    return Vector4D(m[0]*rhs.getX()  + m[1]*rhs.getY()  + m[2]*rhs.getZ()  + m[3]*rhs.getW(),
                    m[4]*rhs.getX()  + m[5]*rhs.getY()  + m[6]*rhs.getZ()  + m[7]*rhs.getW(),
                    m[8]*rhs.getX()  + m[9]*rhs.getY()  + m[10]*rhs.getZ() + m[11]*rhs.getW(),
                    m[12]*rhs.getX() + m[13]*rhs.getY() + m[14]*rhs.getZ() + m[15]*rhs.getW());
}

inline Vector3D operator*(const Vector3D& v, const Matrix4x4& m)
{
    return Vector3D(v.getX()*m[0] + v.getY()*m[4] + v.getZ()*m[8],  v.getX()*m[1] + v.getY()*m[5] + v.getZ()*m[9],  v.getX()*m[2] + v.getY()*m[6] + v.getZ()*m[10]);
}

inline std::ostream& operator<<(std::ostream& os, const Matrix4x4& m)
{
    os << "(" << m[0]  << ",\t" << m[1]  << ",\t" << m[2]  <<  ",\t" << m[3] << ")\n"
    << "(" << m[4]  << ",\t" << m[5]  << ",\t" << m[6]  <<  ",\t" << m[7] << ")\n"
    << "(" << m[8]  << ",\t" << m[9]  << ",\t" << m[10] <<  ",\t" << m[11] << ")\n"
    << "(" << m[12] << ",\t" << m[13] << ",\t" << m[14] <<  ",\t" << m[15] << ")\n";
    return os;
}
// END OF Matrix4x4 INLINE //////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// return determinant of 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
float Matrix4x4::getDeterminant()
{
    return m[0] * getCofactor(m[5],m[6],m[7], m[9],m[10],m[11], m[13],m[14],m[15]) -
    m[1] * getCofactor(m[4],m[6],m[7], m[8],m[10],m[11], m[12],m[14],m[15]) +
    m[2] * getCofactor(m[4],m[5],m[7], m[8],m[9], m[11], m[12],m[13],m[15]) -
    m[3] * getCofactor(m[4],m[5],m[6], m[8],m[9], m[10], m[12],m[13],m[14]);
}

///////////////////////////////////////////////////////////////////////////////
// transpose 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
Matrix4x4& Matrix4x4::transpose()
{
    std::swap(m[1],  m[4]);
    std::swap(m[2],  m[8]);
    std::swap(m[3],  m[12]);
    std::swap(m[6],  m[9]);
    std::swap(m[7],  m[13]);
    std::swap(m[11], m[14]);
    
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// translate this matrix by (x, y, z)
///////////////////////////////////////////////////////////////////////////////
Matrix4x4& Matrix4x4::translate(const Vector3D& v)
{
    return translate(v.getX(), v.getY(), v.getZ());
}

Matrix4x4& Matrix4x4::translate(float x, float y, float z)
{
    m[0] += m[12]*x;   m[1] += m[13]*x;   m[2] += m[14]*x;   m[3] += m[15]*x;
    m[4] += m[12]*y;   m[5] += m[13]*y;   m[6] += m[14]*y;   m[7] += m[15]*y;
    m[8] += m[12]*z;   m[9] += m[13]*z;   m[10]+= m[14]*z;   m[11]+= m[15]*z;
    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// uniform scale
///////////////////////////////////////////////////////////////////////////////
Matrix4x4& Matrix4x4::scale(float s)
{
    return scale(s, s, s);
}

Matrix4x4& Matrix4x4::scale(float x, float y, float z)
{
    m[0] = m[0]*x;   m[1] = m[1]*x;   m[2] = m[2]*x;   m[3] = m[3]*x;
    m[4] = m[4]*y;   m[5] = m[5]*y;   m[6] = m[6]*y;   m[7] = m[7]*y;
    m[8] = m[8]*z;   m[9] = m[9]*z;   m[10]= m[10]*z;  m[11]= m[11]*z;
    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// build a rotation matrix with given angle(degree) and rotation axis, then
// multiply it with this object
///////////////////////////////////////////////////////////////////////////////
Matrix4x4& Matrix4x4::rotate(float angle, const Vector3D& axis)
{
    return rotate(angle, axis.getX(), axis.getY(), axis.getZ());
}

Matrix4x4& Matrix4x4::rotate(float angle, float x, float y, float z)
{
    float c = cosf(angle * DEG2RAD);    // cosine
    float s = sinf(angle * DEG2RAD);    // sine
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float yy = y * y;
    float yz = y * z;
    float zz = z * z;
    
    // build rotation matrix
    Matrix4x4 m;
    m[0] = xx * (1 - c) + c;
    m[1] = xy * (1 - c) - z * s;
    m[2] = xz * (1 - c) + y * s;
    m[3] = 0;
    m[4] = xy * (1 - c) + z * s;
    m[5] = yy * (1 - c) + c;
    m[6] = yz * (1 - c) - x * s;
    m[7] = 0;
    m[8] = xz * (1 - c) - y * s;
    m[9] = yz * (1 - c) + x * s;
    m[10]= zz * (1 - c) + c;
    m[11]= 0;
    m[12]= 0;
    m[13]= 0;
    m[14]= 0;
    m[15]= 1;
    
    // multiply it
    *this = m * (*this);
    
    return *this;
}

Matrix4x4& Matrix4x4::rotateX(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m4 = m[4], m5 = m[5], m6 = m[6],  m7 = m[7],
    m8 = m[8], m9 = m[9], m10= m[10], m11= m[11];
    
    m[4] = m4 * c + m8 *-s;
    m[5] = m5 * c + m9 *-s;
    m[6] = m6 * c + m10*-s;
    m[7] = m7 * c + m11*-s;
    m[8] = m4 * s + m8 * c;
    m[9] = m5 * s + m9 * c;
    m[10]= m6 * s + m10* c;
    m[11]= m7 * s + m11* c;
    
    return *this;
}

Matrix4x4& Matrix4x4::rotateY(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m0 = m[0], m1 = m[1], m2 = m[2],  m3 = m[3],
    m8 = m[8], m9 = m[9], m10= m[10], m11= m[11];
    
    m[0] = m0 * c + m8 * s;
    m[1] = m1 * c + m9 * s;
    m[2] = m2 * c + m10* s;
    m[3] = m3 * c + m11* s;
    m[8] = m0 *-s + m8 * c;
    m[9] = m1 *-s + m9 * c;
    m[10]= m2 *-s + m10* c;
    m[11]= m3 *-s + m11* c;
    
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// rotate the matrix about the z-axis
// params: the angle in degrees
///////////////////////////////////////////////////////////////////////////////
Matrix4x4& Matrix4x4::rotateZ(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m0 = m[0], m1 = m[1], m2 = m[2],  m3 = m[3],
    m4 = m[4], m5 = m[5], m6 = m[6],  m7 = m[7];
    
    m[0] = m0 * c + m4 *-s;
    m[1] = m1 * c + m5 *-s;
    m[2] = m2 * c + m6 *-s;
    m[3] = m3 * c + m7 *-s;
    m[4] = m0 * s + m4 * c;
    m[5] = m1 * s + m5 * c;
    m[6] = m2 * s + m6 * c;
    m[7] = m3 * s + m7 * c;
    
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// compute cofactor of 3x3 minor matrix without sign
// input params are 9 elements of the minor matrix
///////////////////////////////////////////////////////////////////////////////
float Matrix4x4::getCofactor(float m0, float m1, float m2,
                             float m3, float m4, float m5,
                             float m6, float m7, float m8)
{
    return m0 * (m4 * m8 - m5 * m7) -
    m1 * (m3 * m8 - m5 * m6) +
    m2 * (m3 * m7 - m4 * m6);
}
