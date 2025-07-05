#pragma once
#include "glm/glm.hpp"
#include <utility>
#include <array>
class MyVec2
{
public:
	MyVec2() = default;
	MyVec2(const MyVec2& inVec) { x = inVec.x; y = inVec.y; }
	MyVec2(double inX, double inY) { x = inX; y = inY; }
public:
	double x;
	double y;

	MyVec2& operator+=(const MyVec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	friend MyVec2 operator+(const MyVec2& lhs, const MyVec2& rhs)
	{
		MyVec2 result(lhs);
		result += rhs;
		return result;
	}

	MyVec2& operator-=(const MyVec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	friend MyVec2 operator-(const MyVec2& lhs, const MyVec2& rhs)
	{
		MyVec2 result(lhs);
		result -= rhs;
		return result;
	}
};
class MyVec3
{
public:
	MyVec3() = default;
	MyVec3(const MyVec3& inVec) { x = inVec.x; y = inVec.y; z = inVec.z; }
	MyVec3(double inX, double inY, double inZ) { x = inX; y = inY; z = inZ; }
public:
	double x;
	double y;
	double z;

	MyVec3& operator+=(const MyVec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	friend MyVec3 operator+(const MyVec3& lhs, const MyVec3& rhs)
	{
		MyVec3 result(lhs);
		result += rhs;
		return result;
	}

	MyVec3& operator-=(const MyVec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	friend MyVec3 operator-(const MyVec3& lhs, const MyVec3& rhs)
	{
		MyVec3 result(lhs);
		result -= rhs;
		return result;
	}
};
std::pair<bool, glm::vec2> GetCenterTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
std::pair<bool, MyVec2>    GetCenterTriangle(const MyVec2& p1, const MyVec2& p2, const MyVec2& p3);
bool                       IsPointInCircumference(const auto& pt, const auto& p1, const auto& p2, const auto& p3);
struct LineIntersectionData
{
	enum type
	{
		noIntersection,
		lineLineIntersection,
		lineLineOvelap,
		lineContainsLine,
		lineContainsPoint,
		pointPointOverlap,
		sameLine
	};
	type intersectionType;
	glm::vec2 p1, p2;
};

LineIntersectionData GetLineLineIntersection(glm::vec2 line1p1, glm::vec2 line1p2, glm::vec2 line2p1, glm::vec2 line2p2);
bool                 IsPointOnLine          (glm::vec2& lineP1, glm::vec2& lineP2, glm::vec2 p);
bool                 IsPointInTriangle      (auto& pt, auto& triP1, auto& triP2, auto& triP3);
double               CrossSign              (auto& p1, auto& p2, auto& p3);
namespace glm
{
	inline double dot(const MyVec2& in1, const MyVec2& in2)
	{
		return in1.x * in2.x + in1.y * in2.y;
	}
	inline MyVec3 cross(const MyVec3& in1, const MyVec3& in2)
	{
		return {in1.y*in2.z - in1.z*in2.y, in2.x*in1.z - in1.x*in2.z, in1.x * in2.y - in1.y*in2.x};
	}
}







bool
IsPointInTriangle(auto& pt, auto& triP1, auto& triP2, auto& triP3)
{
	double d1, d2, d3;
	bool has_neg, has_pos;

	d1 = CrossSign(pt, triP1, triP2);
	d2 = CrossSign(pt, triP2, triP3);
	d3 = CrossSign(pt, triP3, triP1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}
double
CrossSign(auto& p1, auto& p2, auto& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
bool      IsPointInCircumference(const auto& pt, const auto& p1, const auto& p2, const auto& p3)
{
	auto center = GetCenterTriangle(p1, p2, p3);
	if (!center.first) { return false; }
	auto circ = p1 - center.second;
	auto dist = pt - center.second;
	float radSqr = glm::dot(circ, circ);
	float distSqr = glm::dot(dist, dist);
	return radSqr > distSqr;
}


