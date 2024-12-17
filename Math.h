#pragma once
#define _USE_MATH_DEFINES
#include <corecrt_math_defines.h>
#define SQ(x) (x) * (x)
#define max(a,b) (a>b? a : b) 
#define min(a,b) (a>b? b : a)
#include<string>
#include"GamesEngineeringBase.h"
#include <cmath>
#include <memory.h>
#include <ostream>
#include <algorithm>
#include <iostream>

template<typename T>
static T lerp(const T a, const T b, float t)
{
	return a * (1.0f - t) + (b * t);
}

class Vec3 {
public:
	union {
		float v[3];
		struct { float x, y, z; };
	};
	Vec3() {
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}
	Vec3(float _x, float _y, float _z) {
		v[0] = _x;
		v[1] = _y;
		v[2] = _z;
	}


	Vec3 operator+(const Vec3& pVec)const
	{
		return Vec3(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2]);
	}
	Vec3 operator-(const Vec3& pvec)const {
		return  Vec3(v[0] - pvec.v[0], v[1] - pvec.v[1], v[2] - pvec.v[2]);
	}
	Vec3 operator*(const Vec3& pvec)const {
		return  Vec3(v[0] * pvec.v[0], v[1] * pvec.v[1], v[2] * pvec.v[2]);
	}
	Vec3 operator/(const Vec3& pvec)const {
		return  Vec3(v[0] / pvec.v[0], v[1] / pvec.v[1], v[2] / pvec.v[2]);
	}

	Vec3& operator+=(const Vec3& pVec)
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		return *this;
	}
	Vec3& operator-=(const Vec3& pvec) {
		x -= pvec.x;
		y -= pvec.y;
		z -= pvec.z;
		return *this;
	}
	Vec3& operator*=(const Vec3& pVec)
	{
		v[0] *= pVec.v[0];
		v[1] *= pVec.v[1];
		v[2] *= pVec.v[2];
		return *this;
	}
	Vec3& operator/=(const Vec3& pvec) {
		x /= pvec.x;
		y /= pvec.y;
		z /= pvec.z;
		return *this;
	}

	Vec3 operator+(const float v)const {
		return Vec3(x + v, y + v, z + v);
	}
	Vec3 operator-(const float v)const {
		return Vec3(x - v, y - v, z - v);
	}
	Vec3 operator*(const float v)const {
		return Vec3(x * v, y * v, z * v);
	}
	Vec3 operator/(const float v)const {
		return Vec3(x / v, y / v, z / v);
	}

	Vec3& operator+=(const float v) {
		x += v;
		y += v;
		z += v;
		return *this;
	}
	Vec3& operator-=(const float v) {
		x -= v;
		y -= v;
		z -= v;
		return *this;
	}
	Vec3& operator*=(const float v) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}
	Vec3& operator/=(const float v) {
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}
	float LengthSQ() {
		return SQ(x) + SQ(y) + SQ(z);
	}
	Vec3 operator-() const
	{
		return Vec3(-v[0], -v[1], -v[2]);
	}
	float Length() {
		return sqrtf(LengthSQ());
	}
	
	
	Vec3 normalize()
	{
		float len = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		return Vec3(x * len, y * len, z * len);
	}

	float normalize_GetLength()
	{
		float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		float len = 1.0f / length;
		v[0] *= len; v[1] *= len; v[2] *= len;
		return length;
	}
	
	float Dot(const Vec3& pVec) const
	{
		return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
	}
	Vec3 Cross(const Vec3& v1)
	{
		return Vec3(v1.v[1] * v[2] - v1.v[2] * v[1],
			v1.v[2] * v[0] - v1.v[0] * v[2],
			v1.v[0] * v[1] - v1.v[1] * v[0]);
	}

	Vec3 Max(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(max(v1.v[0], v2.v[0]),
			max(v1.v[1], v2.v[1]),
			max(v1.v[2], v2.v[2]));
	}
	Vec3 Min(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(min(v1.v[0], v2.v[0]),
			min(v1.v[1], v2.v[1]),
			min(v1.v[2], v2.v[2]));
	}
	
	float Max() const
	{
		return max(x, max(y, z));
	}
	float Min() const
	{
		return min(x, min(y, z));
	}

};

class Vec4 {
public:
	union
	{
		float v[4];
		struct { float x, y, z, w; };
	};

	Vec4(float _x = 0, float _y = 0, float _z = 0, float _w = 0)
		: x(_x), y(_y), z(_z), w(_w) {
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}
	void divideByW() {
		x /= w;
		y /= w;
		z /= w;
		w = 1;  
	}
	Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vec4 operator*(float scalar) const { return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); }
	Vec4 operator/(float scalar) const { return Vec4(x / scalar, y / scalar, z / scalar, w / scalar); }

	float dot(const Vec4& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}
	float length() const {
		return std::sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
	}

	Vec4 normalize() const {
		float len = length();
		return Vec4(x / len, y / len, z / len, w / len);
	}

	friend std::ostream& operator<<(std::ostream& os, const Vec4& v) {
		os << "Vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return os;
	}
};
static Vec4 Homo(Vec3& v) {
	return Vec4(v.x, v.y, v.z, 1.0f);
}

class Matrix {
public:
	union
	{
		float a[4][4];
		float m[16];
	};
	float& operator[](int index) {
		return m[index];
	}
	Matrix() {
		identity();
	}
	Matrix(float* othermatrix) {
		memcpy(&m, othermatrix, 16 * sizeof(float));
	}
	Matrix(float n1, float n2, float n3, float n4, float n5, float n6, float n7, float n8, float n9, float n10, float n11, float n12, float n13, float n14, float n15, float n16) {
		m[0] = n1;
		m[1] = n2;
		m[2] = n3;
		m[3] = n4;
		m[4] = n5;
		m[5] = n6;
		m[6] = n7;
		m[7] = n8;
		m[8] = n9;
		m[9] = n10;
		m[10] = n11;
		m[11] = n12;
		m[12] = n13;
		m[13] = n14;
		m[14] = n15;
		m[15] = n16;
	}
	void identity() {
		memset(m, 0, 16 * sizeof(m[0]));
		m[0] = 1;
		m[5] = 1;
		m[10] = 1;
		m[15] = 1;
	}
	Vec3 mulPoint(const Vec3& v)
	{
		Vec3 v1 = Vec3(
			(v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
			(v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
			(v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]);
		float w;
		w = (m[12] * v.x) + (m[13] * v.y) + (m[14] * v.z) + m[15];
		w = 1.0f / w;
		return (v1 * w);
	}
	Matrix PerPro(float height, float width, float radians, float Far, float Near) {
		Matrix PerPro;

		PerPro[0] = 1 / (tan(radians / 2) * (width / height));
		PerPro[1] = 0;
		PerPro[2] = 0;
		PerPro[3] = 0;
		PerPro[4] = 0;
		PerPro[5] = 1 / tan(radians / 2);
		PerPro[6] = 0;
		PerPro[7] = 0;
		PerPro[8] = 0;
		PerPro[9] = 0;
		PerPro[10] = (-Far) / (Far - Near);
		PerPro[11] = ((-Far) * Near) / (Far - Near);
		PerPro[12] = 0;
		PerPro[13] = 0;
		PerPro[14] = -1;
		PerPro[15] = 0;
		return PerPro;
	}

	Matrix lookat(Vec3 from, Vec3 to, Vec3 up) {
		identity();
		Vec3 dir = (from - to).normalize();
		Vec3 right = up.Cross(dir).normalize();
		Vec3 upPrime = dir.Cross(right);
		Matrix result;

		result.m[0] = right.x;
		result.m[1] = right.y;
		result.m[2] = right.z;

		result.m[4] = upPrime.x;
		result.m[5] = upPrime.y;
		result.m[6] = upPrime.z;

		result.m[8] = dir.x;
		result.m[9] = dir.y;
		result.m[10] = dir.z;

		result.m[3] = -from.Dot(right);
		result.m[7] = -from.Dot(upPrime);
		result.m[11] = -from.Dot(dir);

		result.m[12] = 0.f;
		result.m[13] = 0.f;
		result.m[14] = 0.f;
		result.m[15] = 1.f;

		return result;
	}
	Matrix rotationX(float degree) {
		Matrix rot;
		float radians = degree * M_PI / 180.f;
		float _cos = cos(radians);
		float _sin = sin(radians);
		rot.m[0] = 1;   rot.m[1] = 0;    rot.m[2] = 0;      rot.m[3] = 0;
		rot.m[4] = 0;   rot.m[5] = _cos; rot.m[6] = -_sin;  rot.m[7] = 0;
		rot.m[8] = 0;   rot.m[9] = _sin; rot.m[10] = _cos; rot.m[11] = 0;
		rot.m[12] = 0;  rot.m[13] = 0;   rot.m[14] = 0;    rot.m[15] = 1;
		return rot;
	}

	Matrix rotationY(float degree) {
		Matrix rot;
		float radians = degree * M_PI / 180.f;
		float _cos = cos(radians);
		float _sin = sin(radians);
		rot.m[0] = _cos;   rot.m[1] = 0; rot.m[2] = -_sin; rot.m[3] = 0;
		rot.m[4] = 0;      rot.m[5] = 1; rot.m[6] = 0;    rot.m[7] = 0;
		rot.m[8] = _sin;   rot.m[9] = 0; rot.m[10] = _cos; rot.m[11] = 0;
		rot.m[12] = 0;     rot.m[13] = 0; rot.m[14] = 0; rot.m[15] = 1;
		return rot;
	}

	Vec3 rotationZ(const Vec3& v, float degree) {
		float _cos = degree * M_PI / 180.f;
		float _sin = degree * M_PI / 180.f;
		_cos = cos(_cos);
		_sin = sin(_sin);
		identity();
		m[0] = _cos;
		m[1] = -_sin;
		m[4] = _sin;
		m[5] = _cos;
		return mulPoint(v);
	}
	Matrix rotation(Vec3 v) {
		Matrix rotX, rotY, rotZ;
		float radX = v.x * M_PI / 180.f;
		float radY = v.y * M_PI / 180.f;
		float radZ = v.z * M_PI / 180.f;
		rotX.m[0] = 1;   rotX.m[1] = 0;       rotX.m[2] = 0;        rotX.m[3] = 0;
		rotX.m[4] = 0;   rotX.m[5] = cos(radX);  rotX.m[6] = -sin(radX); rotX.m[7] = 0;
		rotX.m[8] = 0;   rotX.m[9] = sin(radX);  rotX.m[10] = cos(radX); rotX.m[11] = 0;
		rotX.m[12] = 0;  rotX.m[13] = 0;      rotX.m[14] = 0;      rotX.m[15] = 1;
		rotY.m[0] = cos(radY);  rotY.m[1] = 0;  rotY.m[2] = sin(radY);   rotY.m[3] = 0;
		rotY.m[4] = 0;       rotY.m[5] = 1;  rotY.m[6] = 0;       rotY.m[7] = 0;
		rotY.m[8] = -sin(radY); rotY.m[9] = 0;  rotY.m[10] = cos(radY);  rotY.m[11] = 0;
		rotY.m[12] = 0;      rotY.m[13] = 0; rotY.m[14] = 0;     rotY.m[15] = 1;
		rotZ.m[0] = cos(radZ);  rotZ.m[1] = -sin(radZ); rotZ.m[2] = 0; rotZ.m[3] = 0;
		rotZ.m[4] = sin(radZ);  rotZ.m[5] = cos(radZ);  rotZ.m[6] = 0; rotZ.m[7] = 0;
		rotZ.m[8] = 0;       rotZ.m[9] = 0;        rotZ.m[10] = 1; rotZ.m[11] = 0;
		rotZ.m[12] = 0;      rotZ.m[13] = 0;      rotZ.m[14] = 0; rotZ.m[15] = 1;
		Matrix finalRot = rotX * rotY * rotZ;
		return finalRot;
	}

	static Matrix translation(Vec3 v) {
		Matrix mat;
		mat.a[0][3] = v.x;
		mat.a[1][3] = v.y;
		mat.a[2][3] = v.z;
		return mat;
	}
	static Matrix translation1(const Vec3& v, Matrix mat) {
		mat.a[0][3] = v.x;
		mat.a[1][3] = v.y;
		mat.a[2][3] = v.z;
		return mat;
	}

	Matrix scaling(const Vec3 v) {
		identity();
		m[0] = v.x;
		m[5] = v.y;
		m[10] = v.z;
		return *this;
	}

	Vec3 projectionO(const Vec3& v) {}

	Matrix mul(const Matrix& matrix) const
	{
		Matrix ret;
		ret.m[0] = m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3];
		ret.m[1] = m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3];
		ret.m[2] = m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3];
		ret.m[3] = m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3];
		ret.m[4] = m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7];
		ret.m[5] = m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7];
		ret.m[6] = m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7];
		ret.m[7] = m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7];
		ret.m[8] = m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11];
		ret.m[9] = m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11];
		ret.m[10] = m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11];
		ret.m[11] = m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11];
		ret.m[12] = m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15];
		ret.m[13] = m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15];
		ret.m[14] = m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15];
		ret.m[15] = m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15];
		return ret;
	}
	Matrix operator*(const Matrix& matrix)
	{
		return mul(matrix);
	}

	Matrix Flip() {
		Matrix temp;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp.a[i][j] = a[j][i];
			}
		}
		return temp;
	}

	Matrix invert()
	{
		Matrix inv;
		inv.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
		inv.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
		inv.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
		inv.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
		inv.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
		inv.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
		inv.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
		inv.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
		inv.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
		inv.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
		inv.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
		inv.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
		inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
		inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
		inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
		inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
		float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];
		if (det == 0) {}
		det = 1.0 / det;
		for (int i = 0; i < 16; i++) {
			inv.m[i] = inv.m[i] * det;
		}
		return inv;
	}
};

class Colour {
public:
	union {
		float v[3];
		struct { float r, g, b; };
	};
	Colour() {
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
	}
	Colour(float _a, float _b, float _c) {
		v[0] = _a;
		v[1] = _b;
		v[2] = _c;
	}
};


class Quaternion {
public:
	float x, y, z, w;
	union {
		struct {
			float a;
			float b;
			float c;
			float d;
		};
		float q[4];
	};

	
	Quaternion() : a(1), b(0), c(0), d(0) {}

	
	Quaternion(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}

	
	Quaternion operator+(const Quaternion& q) const {
		return Quaternion(a + q.a, b + q.b, c + q.c, d + q.d);
	}

	
	Quaternion operator-(const Quaternion& q) const {
		return Quaternion(a - q.a, b - q.b, c - q.c, d - q.d);
	}


	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(
			a * q.a - b * q.b - c * q.c - d * q.d,
			a * q.b + b * q.a + c * q.d - d * q.c,
			a * q.c - b * q.d + c * q.a + d * q.b,
			a * q.d + b * q.c - c * q.b + d * q.a
		);
	}

	Quaternion operator*(float scalar) const {
		return Quaternion(a * scalar, b * scalar, c * scalar, d * scalar);
	}

	
	static float dot(const Quaternion& q1, const Quaternion& q2) {
		return q1.a * q2.a + q1.b * q2.b + q1.c * q2.c + q1.d * q2.d;
	}

	void normalize() {
		float magnitude = std::sqrt(a * a + b * b + c * c + d * d);
		if (magnitude > 0.0f) {
			a /= magnitude;
			b /= magnitude;
			c /= magnitude;
			d /= magnitude;
		}
	}

	
	static Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) {
		return (q1 * (1 - t) + q2 * t).normalized();
	}
	static float Dot(const Quaternion& lhs, const Quaternion& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}

	static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
		float dot = Quaternion::dot(q1, q2);
		if (dot < 0.0f) {
		
			return slerp(q1, Quaternion(-q2.a, -q2.b, -q2.c, -q2.d), t);
		}
		dot = std::fmax(-1.0f, std::fmin(1.0f, dot));
		float theta = std::acos(dot);
		if (theta < 1e-6) {
			return lerp(q1, q2, t);
		}
		float sinTheta = std::sin(theta);
		float weight1 = std::sin((1 - t) * theta) / sinTheta;
		float weight2 = std::sin(t * theta) / sinTheta;
		return (q1 * weight1 + q2 * weight2).normalized();
	}

	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
		float cos_theta = Dot(a, b);
		float sign = (cos_theta < 0.f) ? -1.f : 1.f;
		cos_theta = std::fabs(cos_theta);

		float c1, c2;
		if (cos_theta > 1.f - 1e-5f) {
			c1 = 1.f - t;
			c2 = t * sign;
		}
		else {
			float theta = acos(cos_theta);
			float sin_theta = sin(theta);
			c1 = sin((1.f - t) * theta) / sin_theta;
			c2 = sin(t * theta) / sin_theta * sign;
		}
		return Quaternion(
			c1 * a.x + c2 * b.x,
			c1 * a.y + c2 * b.y,
			c1 * a.z + c2 * b.z,
			c1 * a.w + c2 * b.w
		);
	}

	Matrix toMatrix() const {
		float xx = q[0] * q[0];
		float xy = q[0] * q[1];
		float xz = q[0] * q[2];
		float yy = q[1] * q[1];
		float zz = q[2] * q[2];
		float yz = q[1] * q[2];
		float wx = q[3] * q[0];
		float wy = q[3] * q[1];
		float wz = q[3] * q[2];
		Matrix matrix;
		matrix.m[0] = 1.0f - 2.0f * (yy + zz);
		matrix.m[1] = 2.0f * (xy - wz);
		matrix.m[2] = 2.0f * (xz + wy);
		matrix.m[3] = 0.0;
		matrix.m[4] = 2.0f * (xy + wz);
		matrix.m[5] = 1.0f - 2.0f * (xx + zz);
		matrix.m[6] = 2.0f * (yz - wx);
		matrix.m[7] = 0.0;
		matrix.m[8] = 2.0f * (xz - wy);
		matrix.m[9] = 2.0f * (yz + wx);
		matrix.m[10] = 1.0f - 2.0f * (xx + yy);
		matrix.m[11] = 0.0;
		matrix.m[12] = 0;
		matrix.m[13] = 0;
		matrix.m[14] = 0;
		matrix.m[15] = 1;
		return matrix;
	}

	Quaternion normalized() const {
		Quaternion q(*this);
		q.normalize();
		return q;
	}

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
		os << "(" << q.a << ", " << q.b << ", " << q.c << ", " << q.d << ")";
		return os;
	}

};




