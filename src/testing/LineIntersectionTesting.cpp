#include "../../headers/testing/LineIntersectionTesting.h"
#include <iostream>
#include <string>
bool
LineIntersectionTesting::runTest                ( )
{
	glm::vec2 p1, p2, p3, p4;
	p1 = { 0,0 };
	p2 = { 1,1 };
	p3 = { 0,1 };
	p4 = { 1,2 };
	auto result = GetLineLineIntersection(p1,p2,p3,p4);
	IntersectionDataMatches({ LineIntersectionData::type::noIntersection, {0,0}, {0,0} }, result, 0);
	p1 = { 0,0 };
	p2 = { 1,1 };
	p3 = { 0,1 };
	p4 = { 1,0 };
	result = GetLineLineIntersection(p1, p2, p3, p4);
	IntersectionDataMatches({ LineIntersectionData::type::lineLineIntersection, {0.5,0.5}, {0,0} }, result, 1);
	p1 = { 0,0 };
	p2 = { 1,1 };
	p3 = { 0,1 };
	p4 = { 1,1 };
	result = GetLineLineIntersection(p1, p2, p3, p4);
	IntersectionDataMatches({ LineIntersectionData::type::pointPointOverlap, {1,1}, {0,0} }, result, 3);
	p1 = { 0,0 };
	p2 = { 1,1 };
	p3 = { 0,1 };
	p4 = { 2,1 };
	result = GetLineLineIntersection(p1, p2, p3, p4);
	IntersectionDataMatches({ LineIntersectionData::type::lineContainsPoint, {1,1}, {0,0} }, result, 4);
	p1 = { 0,1 };
	p2 = { 2,1 };
	p3 = { 0, 0 };
	p4 = { 1, 1 };
	result = GetLineLineIntersection(p1, p2, p3, p4);
	IntersectionDataMatches({ LineIntersectionData::type::lineContainsPoint, {1,1}, {0,0} }, result, 5);
	p1 = { 0, 0 };
	p2 = { 1, 1 };
	p3 = { 1, 1 };
	p4 = { 0, 0 };
	result = GetLineLineIntersection(p1, p2, p3, p4);
	IntersectionDataMatches({ LineIntersectionData::type::sameLine, {0,0}, {1,1} }, result, 6);

	return true;
}
bool
LineIntersectionTesting::IntersectionDataMatches(LineIntersectionData inExpected, LineIntersectionData inReceived, int inIdx)
{
	auto direct = DirectVec2Match(inExpected.p1, inReceived.p1) && DirectVec2Match(inExpected.p2, inReceived.p2);
	auto swap   = DirectVec2Match(inExpected.p1, inReceived.p2) && DirectVec2Match(inExpected.p2, inReceived.p1);
	auto outVal = (direct || swap) && inExpected.intersectionType == inReceived.intersectionType;
	if (!outVal) 
	{
		std::cout << "test failed: " + std::to_string(inIdx) << "\n"; 
		std::cout << "direct: " << direct << "\nswap: " << swap << "\nintersection: \n";
	}
	else { std::cout << "success: " + std::to_string(inIdx) << "\n"; }
	return outVal;
}
bool 
LineIntersectionTesting::DirectVec2Match(glm::vec2& a, glm::vec2& b, float epsilon)
{
	if (a.x * (1 + epsilon) < b.x) { return false; }
	if (a.x / (1 + epsilon) > b.x) { return false; }
	if (a.y * (1 + epsilon) < b.y) { return false; }
	if (a.y / (1 + epsilon) > b.y) { return false; }
	return true;
}

