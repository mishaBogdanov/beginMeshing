#pragma once
#include "glm/glm.hpp"
#include <utility>

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
LineIntersectionData GetLineLineIntersection(glm::vec2 line1p1, glm::vec2 line1p2, glm::vec2 line2p1, glm::vec2 line2p2);
bool                 IsPointOnLine          (glm::vec2& lineP1, glm::vec2& lineP2, glm::vec2 p);


