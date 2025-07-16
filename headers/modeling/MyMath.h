#pragma once
#include "glm/glm.hpp"
#include <utility>
#include <array>
#include <stdexcept>
#include <vector>
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
	MyVec2& operator/=(const double& rhs)
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}

	friend MyVec2 operator/(const MyVec2& lhs, const double& rhs)
	{
		MyVec2 result(lhs);
		result /= rhs;
		return result;
	}
	MyVec2& operator*=(const double& rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	friend MyVec2 operator*(const MyVec2& lhs, const double& rhs)
	{
		MyVec2 result(lhs);
		result *= rhs;
		return result;
	}

	bool operator==(const MyVec2& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y);
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

	bool operator==(const MyVec3& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) && (rhs.z == z);
	}
	MyVec3& operator/=(const double& rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	friend MyVec3 operator/(const MyVec3& lhs, const double& rhs)
	{
		MyVec3 result(lhs);
		result /= rhs;
		return result;
	}
	MyVec3& operator*=(const double& rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	friend MyVec3 operator*(const MyVec3& lhs, const double& rhs)
	{
		MyVec3 result(lhs);
		result *= rhs;
		return result;
	}
};
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
namespace glm
{
	inline double dot(const MyVec2& in1, const MyVec2& in2)
	{
		return in1.x * in2.x + in1.y * in2.y;
	}
	inline double dot(const MyVec3& in1, const MyVec3& in2)
	{
		return in1.x * in2.x + in1.y * in2.y + in1.z * in2.z;
	}
	inline MyVec3 cross(const MyVec3& in1, const MyVec3& in2)
	{
		return { in1.y * in2.z - in1.z * in2.y, in2.x * in1.z - in1.x * in2.z, in1.x * in2.y - in1.y * in2.x };
	}
	inline MyVec2 normalize(const MyVec2& in1)
	{
		return in1 / sqrt(in1.x*in1.x + in1.y*in1.y);
	}
	inline MyVec3 normalize(const MyVec3& in1)
	{
		return in1 / sqrt(in1.x * in1.x + in1.y * in1.y);
	}
}


std::pair<bool, glm::vec2> GetCenterTriangle          (const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
std::pair<bool, MyVec2>    GetCenterTriangle          (const MyVec2& p1, const MyVec2& p2, const MyVec2& p3);
bool                       IsPointInCircumference     (const auto& pt, const auto& p1, const auto& p2, const auto& p3);
bool                       IsPolygonConvex            (std::vector<MyVec2> inPoints);
LineIntersectionData       GetLineLineIntersection    (glm::vec2 line1p1, glm::vec2 line1p2, glm::vec2 line2p1, glm::vec2 line2p2);
bool                       DoLinesIntersect           (const auto& line1p1, const auto& line1p2, const auto& line2p1, const auto& line2p2);
bool                       DoLinesIntersect           (const auto& line1p1, const auto& line1p2, const auto& line2p1, const auto& line2p2, double leniance);
bool                       IsPointOnLine              (auto& lineP1, auto& lineP2, auto& p);
bool                       IsPointOnLine              (auto& lineP1, auto& lineP2, auto& p, double leniance);
bool                       IsPointInTriangle          (auto& pt, auto& triP1, auto& triP2, auto& triP3);
double                     CrossIn2D                  (const MyVec2& in1, const MyVec2& in2);
double                     CrossSign                  (auto& p1, auto& p2, auto& p3);
std::vector<size_t>        GetQuadFromTri             (const std::array<size_t, 3>&inTri1, const std::array<size_t, 3>& inTri2);




bool
DoLinesIntersect(const auto& line1p1, const auto& line1p2, const auto& line2p1, const auto& line2p2)
{
	MyVec3 l1         = MyVec3(line1p2.x - line1p1.x, line1p2.y - line1p1.y, 0);
	MyVec3 l2         = MyVec3(line2p2.x - line2p1.x, line2p2.y - line2p1.y, 0);
	MyVec3 l1p1Tol2p1 = MyVec3(line2p1.x - line1p1.x, line2p1.y - line1p1.y, 0);
	MyVec3 l1p1Tol2p2 = MyVec3(line2p2.x - line1p1.x, line2p2.y - line1p1.y, 0);
	MyVec3 l2p1Tol1p1 = MyVec3(line1p1.x - line2p1.x, line1p1.y - line2p1.y, 0);
	MyVec3 l2p1Tol1p2 = MyVec3(line1p2.x - line2p1.x, line1p2.y - line2p1.y, 0);
	bool  req1 = glm::dot(glm::cross(l1, l1p1Tol2p1), glm::cross(l1, l1p1Tol2p2)) < 0;
	bool  req2 = glm::dot(glm::cross(l2, l2p1Tol1p1), glm::cross(l2, l2p1Tol1p2)) < 0;
	return req1 && req2;
}
bool
DoLinesIntersect(const auto& line1p1, const auto& line1p2, const auto& line2p1, const auto& line2p2, double leniance)
{
	MyVec3 l1         = glm::normalize(MyVec3(line1p2.x - line1p1.x, line1p2.y - line1p1.y, 0));
	MyVec3 l2         = glm::normalize(MyVec3(line2p2.x - line2p1.x, line2p2.y - line2p1.y, 0));
	MyVec3 l1p1Tol2p1 = glm::normalize(MyVec3(line2p1.x - line1p1.x, line2p1.y - line1p1.y, 0));
	MyVec3 l1p1Tol2p2 = glm::normalize(MyVec3(line2p2.x - line1p1.x, line2p2.y - line1p1.y, 0));
	MyVec3 l2p1Tol1p1 = glm::normalize(MyVec3(line1p1.x - line2p1.x, line1p1.y - line2p1.y, 0));
	MyVec3 l2p1Tol1p2 = glm::normalize(MyVec3(line1p2.x - line2p1.x, line1p2.y - line2p1.y, 0));
	bool  req1 = glm::dot(glm::cross(l1, l1p1Tol2p1), glm::cross(l1, l1p1Tol2p2)) < leniance;
	bool  req2 = glm::dot(glm::cross(l2, l2p1Tol1p1), glm::cross(l2, l2p1Tol1p2)) < leniance;
	return req1 && req2;
}
bool
IsPointOnLine(auto& lineP1, auto& lineP2, auto& p)
{
	if (lineP1 == p || lineP2 == p) { return true; }
	MyVec3 dirP1ToP2 = MyVec3(lineP2.x - lineP1.x, lineP2.y - lineP1.y, 0);
	auto dirP1Top = MyVec3(p.x - lineP1.x, p.y - lineP1.y, 0);
	auto cross = glm::cross(dirP1ToP2, dirP1Top);
	if (cross.z != 0) { return false; }
	if (dirP1ToP2.x != 0)
	{
		auto rslt = (dirP1Top.x / dirP1ToP2.x);
		return rslt >= 0 && rslt <= 1;
	}
	else if (dirP1ToP2.y != 0)
	{
		auto rslt = (dirP1Top.y / dirP1ToP2.y);
		return rslt >= 0 && rslt <= 1;
	}
	throw std::logic_error("somehow .x and .y are both 0 in dirP1ToP2");
	return 0;
}
bool
IsPointOnLine(auto& lineP1, auto& lineP2, auto& p, double leniance)
{
	if (lineP1 == p || lineP2 == p) { return true; }
	MyVec3 dirP1ToP2 = MyVec3(lineP2.x - lineP1.x, lineP2.y - lineP1.y, 0);
	MyVec3 normDirP1ToP2 = glm::normalize(dirP1ToP2);
	MyVec3 dirP1Top = MyVec3(p.x - lineP1.x, p.y - lineP1.y, 0);
	MyVec3 normDirP1ToP = glm::normalize(dirP1Top);
	auto cross = glm::cross(normDirP1ToP2, normDirP1ToP);
	if (abs(cross.z) > leniance) { return false; }
	if (dirP1ToP2.x != 0)
	{
		auto rslt = (dirP1Top.x / dirP1ToP2.x);
		return rslt >= 0 && rslt <= 1;
	}
	else if (dirP1ToP2.y != 0)
	{
		auto rslt = (dirP1Top.y / dirP1ToP2.y);
		return rslt >= 0 && rslt <= 1;
	}
	throw std::logic_error("somehow .x and .y are both 0 in dirP1ToP2");
	return 0;
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


