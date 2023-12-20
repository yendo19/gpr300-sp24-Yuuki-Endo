/*
	Author: Eric Winebrenner
*/

#pragma once
#include <math.h>
#include "vec2.h"
#include "ewMath.h"

namespace ew {
	struct Vec3 {
	public:
		static Vec3 Up;
		static Vec3 Right;
		static Vec3 Forward;
	public:
		float x, y, z;

		Vec3() :x(0), y(0), z(0) {};
		Vec3(float x) :x(x), y(x), z(x) {};
		Vec3(float x, float y) :x(x), y(y), z(0) {};
		Vec3(float x, float y, float z) :x(x), y(y), z(z) {};

		inline Vec2 ToVec2() { return Vec2(x, y); };

	//Operator overloads
	public:
		Vec3 operator+(const Vec3& rhs)const
		{
			return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		Vec3 operator-(const Vec3& rhs)const
		{
			return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		Vec3 operator*(const Vec3& rhs)const 
		{
			return Vec3(x * rhs.x, y * rhs.y, z * rhs.z);
		}

		Vec3 operator*(float rhs)const 
		{
			return Vec3(x * rhs, y * rhs, z * rhs);
		}

		Vec3 operator/(float rhs)const 
		{
			return Vec3(x / rhs, y / rhs, z / rhs);
		}

	    Vec3& operator+=(const Vec3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		Vec3& operator-=(const Vec3& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		Vec3& operator*=(float rhs)
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;
			return *this;
		}

		Vec3& operator/=(float rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;
			return *this;
		}

		Vec3 operator-()
		{
			return Vec3(-x, -y, -z);
		}
		//Static functions
	public:
		static float Dot(const Vec3& a, const Vec3& b) 
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		static Vec3 Cross(const Vec3& a, const Vec3& b) 
		{
			return Vec3{
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			};
		}

		/// <summary>
		/// Squared magnitude of a vector
		/// </summary>
		static float SqMagnitude(const Vec3& v)
		{
			return v.x * v.x + v.y * v.y + v.z * v.z;
		}

		static float Magnitude(const Vec3& v)
		{
			return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		static Vec3 Normalize(const Vec3& v)
		{
			float mag = Magnitude(v);
			if (mag == 0)
				return v;
			return v / mag;
		}

		/// <summary>
		/// Distance between two vectors
		/// </summary>
		static Vec3 Distance(const Vec3& a, const Vec3& b) 
		{
			return Magnitude(a - b);
		}
		/// <summary>
		/// Linear interpolation between two vectors, a and b
		/// </summary>
		/// <param name="a">Start value</param>
		/// <param name="b">End value</param>
		/// <param name="t">Value between 0 and 1</param>
		/// <returns></returns>
		static Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
			return ew::Vec3(
				ew::Lerp(a.x, b.x, t),
				ew::Lerp(a.y, b.y, t),
				ew::Lerp(a.z, b.z, t)
			);
		}
	};
}

