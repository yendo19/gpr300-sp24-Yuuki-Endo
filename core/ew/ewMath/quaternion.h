#pragma once
#include "vec3.h"
#include "mat4.h"
#include "ewMath.h"

namespace ew {
	struct Quaternion {
		static Quaternion Identity;

		float x, y, z, w;
		Quaternion() = default;
		Quaternion(float x, float y, float z, float w) :
			x(x), y(y), z(z), w(w) {
		};
		const Vec3& GetVectorPart(void)const {
			return (reinterpret_cast<const Vec3&>(x));
		}
		//Quaternion-float operators
		Quaternion& operator*=(float rhs);
		Quaternion& operator/=(float rhs);
		friend Quaternion operator*(Quaternion lhs, float rhs);
		friend Quaternion operator*(float lhs, Quaternion rhs);
		friend Quaternion operator/(Quaternion lhs, float rhs);

		//Quaternion-Quaternion operators
		friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
		friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);
		Quaternion& operator*=(const Quaternion& rhs);

	};
	inline Quaternion& Quaternion::operator*=(float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		this->w *= rhs;
		return *this;
	}
	inline Quaternion operator*(Quaternion lhs, float rhs)
	{
		lhs *= rhs;
		return lhs;
	}
	inline Quaternion operator*(float lhs, Quaternion rhs)
	{
		rhs *= lhs;
		return rhs;
	}
	inline Quaternion& Quaternion::operator/=(float rhs)
	{
		*this *= (1.0f / rhs);
		return *this;
	}
	inline Quaternion& Quaternion::operator*=(const Quaternion& rhs)
	{
		Quaternion result = *this * rhs;
		this->x = result.x;
		this->y = result.y;
		this->z = result.z;
		this->w = result.w;
		return *this;
	}
	inline Quaternion operator/(Quaternion lhs, float rhs)
	{
		lhs /= rhs;
		return lhs;
	}
	inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) 
	{
		//TODO
		return Quaternion(
			lhs.x + rhs.x,
			lhs.y + rhs.y,
			lhs.z + rhs.z,
			lhs.w + rhs.w
		);
	}
	inline Quaternion operator*(const Quaternion& left, const Quaternion& right) 
	{
		const float a = left.y * right.z - left.z * right.y;
		const float b = left.z * right.x - left.x * right.z;
		const float c = left.x * right.y - left.y * right.x;
		const float d = left.x * right.x + left.y * right.y + left.z * right.z;

		return Quaternion(
			left.x * right.w + right.x * left.w + a,
			left.y * right.w + right.y * left.w + b,
			left.z * right.w + right.z * left.w + c,
			left.w * right.w - d
		);
	}

	//HELPER FUNCTIONS
	
	//Based on GLM Slerp implementation
	inline Quaternion Slerp(const Quaternion& qa, const Quaternion& qb, float t) 
	{
		Quaternion z = qb;

		float cosTheta = qa.x * qb.x + qa.y * qb.y + qa.z * qb.z + qa.w * qb.w;
		//If cosTheta < 0, interpolation will take long way around.
		//Negate one quaternion to take short way.
		if (cosTheta < 0) {
			z = Quaternion(-qb.x, -qb.y, -qb.z, -qb.w);
			cosTheta = -cosTheta;
		}
		//If cosTheta is close to 1, use linear interpolation. Avoids sin(0) becoming 0 denominator
		if (cosTheta > 1.0 - FLT_EPSILON) {
			return Quaternion(
				ew::Lerp(qa.x, z.x, t),
				ew::Lerp(qa.y, z.y, t),
				ew::Lerp(qa.z, z.z, t),
				ew::Lerp(qa.w, z.w, t)
			);
		}
		else {
			// Essential Mathematics, page 467
			float angle = acosf(cosTheta);
			return (sinf((1 - t) * angle) * qa + sin(t * angle) * z) / sinf(angle);
		}
	}

	/// <summary>
	/// Normalized quaternion to rotation matrix conversion.
	/// Returns 4x4 for easy combination with full affine matrix
	/// </summary>
	/// <param name="q">Normalized quaternion</param>
	/// <returns></returns>
	inline Mat4 ToRotationMatrix(const Quaternion& q) 
	{
		float x2 = q.x * q.x;
		float y2 = q.y * q.y;
		float z2 = q.z * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;
		return Mat4(
			1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
			2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	/// <summary>
	/// Rotates vector v by quaternion q. Assumes q is unit quaternion
	/// </summary>
	/// <param name="v"></param>
	/// <param name="q"></param>
	/// <returns></returns>
	inline Vec3 RotateVec3(const Vec3& v, const Quaternion& q) 
	{
		const Vec3& b = q.GetVectorPart();
		float b2 = b.x * b.x + b.y * b.y + b.z * b.z;
		return (v * (q.w * q.w - b2) + b * Vec3::Dot(v, b) * 2.0f) + Vec3::Cross(b, v) * (q.w * 2.0f);
	}

	/// <summary>
	/// Normalizes a quaternion
	/// </summary>
	/// <param name="q">Original quaternion</param>
	/// <returns>Normalized quaternion</returns>
	inline Quaternion Normalize(const Quaternion& q) 
	{
		float norm = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return Quaternion(q.x / norm, q.y / norm, q.z / norm, q.w / norm);
	}

	/// <summary>
	/// Constructs a rotation around an axis by a angle
	/// </summary>
	/// <param name="axis">Axis to rotate around</param>
	/// <param name="angle">Angle in radians</param>
	/// <returns></returns>
	inline Quaternion AngleAxis(const Vec3& axis, float angle) 
	{
		float s = sinf(angle / 2);
		return Quaternion(
			axis.x * s,
			axis.y * s,
			axis.z * s,
			cosf(angle/2)
		);
	}
}