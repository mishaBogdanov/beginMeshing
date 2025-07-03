#pragma once
#include "glm/glm.hpp"
#include <utility>
#include <array>

std::pair<bool, glm::vec2> GetCenterTriangle(glm::vec2& p1, glm::vec2& p2, glm::vec2& p3);
bool                       IsPointIntCircumference(glm::vec2& pt, glm::vec2& p1, glm::vec2& p2, glm::vec2& p3);
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
class MyVec2
{
public:
	MyVec2(double inX, double inY) { x = inX; y = inY; }
public:
	double x;
	double y;
};
LineIntersectionData GetLineLineIntersection(glm::vec2 line1p1, glm::vec2 line1p2, glm::vec2 line2p1, glm::vec2 line2p2);
bool                 IsPointOnLine          (glm::vec2& lineP1, glm::vec2& lineP2, glm::vec2 p);
bool                 IsPointInTriangle      (auto& pt, auto& triP1, auto& triP2, auto& triP3);
double               CrossSign              (auto& p1, auto& p2, auto& p3);







bool
IsPointInTriangle(auto& pt, auto& triP1, auto& triP2, auto& triP3)
{
	double d1 = 2, d2 = 1, d3 = 0;
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


