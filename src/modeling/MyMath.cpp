#include "../../headers/modeling/MyMath.h"
#include <cmath>

std::pair<bool, glm::vec2> 
GetCenterTriangle                                  (const glm::vec2&                 p1, 
                                                    const glm::vec2&                 p2,
	                                                const glm::vec2&                 p3)
{
	glm::vec3 p1ToP2 = glm::vec3(p2.x - p1.x, p2.y - p1.y, 0);
	glm::vec3 p2ToP3 = glm::vec3(p3.x - p2.x, p3.y - p2.y, 0);
	glm::vec3 cross  = glm::cross(p1ToP2, p2ToP3);
	if (cross.z == 0) { return { false, {0,0} }; }

	auto t = p1.x * p1.x	+	p1.y*p1.y	-	p2.x*p2.x	-	p2.y*p2.y;
	auto u = p1.x * p1.x	+	p1.y*p1.y	-	p3.x*p3.x	-	p3.y*p3.y;
	auto j = (p1.x - p2.x) * (p1.y - p3.y) - (p1.x - p3.x) * (p1.y - p2.y);
	return { true, { (-(p1.y - p2.y) * u + (p1.y - p3.y) * t) / (2 * j),
			 ((p1.x - p2.x)* u - (p1.x - p3.x) * t) / (2 * j)} };
}
std::pair<bool, MyVec2>    
GetCenterTriangle                                  (const MyVec2&                    p1, 
                                                    const MyVec2&                    p2, 
													const MyVec2&                    p3)
{
	glm::vec3 p1ToP2 = glm::vec3(p2.x - p1.x, p2.y - p1.y, 0);
	glm::vec3 p2ToP3 = glm::vec3(p3.x - p2.x, p3.y - p2.y, 0);
	glm::vec3 cross = glm::cross(p1ToP2, p2ToP3);
	if (cross.z == 0) { return { false, {0,0} }; }

	auto t = p1.x * p1.x + p1.y * p1.y - p2.x * p2.x - p2.y * p2.y;
	auto u = p1.x * p1.x + p1.y * p1.y - p3.x * p3.x - p3.y * p3.y;
	auto j = (p1.x - p2.x) * (p1.y - p3.y) - (p1.x - p3.x) * (p1.y - p2.y);
	return { true, { (-(p1.y - p2.y) * u + (p1.y - p3.y) * t) / (2 * j),
			 ((p1.x - p2.x) * u - (p1.x - p3.x) * t) / (2 * j)} };
}
bool
IsPolygonConvex                                    (std::vector<MyVec2>              inPoints)
{
	bool isNegative;
	double firstSpin = 0;;
	for (size_t idx1 = 0; idx1 < inPoints.size(); idx1++)
	{
		size_t idx2 = (idx1 + 1) % inPoints.size();
		size_t idx3 = (idx1 + 2) % inPoints.size();
		MyVec2 vec1 = inPoints[idx2] - inPoints[idx1];
		MyVec2 vec2 = inPoints[idx3] - inPoints[idx2];
		double spin = CrossIn2D(vec1, vec2);
		if (firstSpin == 0 && spin == 0) { continue; }
		else if (firstSpin == 0) { firstSpin = spin; }
		else if (firstSpin * spin <= 0) { return false; }
	}
	return true;
}
double
CrossIn2D                                          (const MyVec2&                    in1,
	                                                const MyVec2&                    in2)
{
	return in1.x * in2.y - in1.y * in2.x;
}
LineIntersectionData 
GetLineLineIntersection                            (glm::vec2                        line1p1, 
	                                                glm::vec2                        line1p2, 
	                                                glm::vec2                        line2p1, 
	                                                glm::vec2                        line2p2)
{
	glm::vec2 overlapPoint;
	bool sharePoint = false;
	if (line1p1 == line2p1)
	{
		if (line1p2 == line2p2) { return { LineIntersectionData::type::sameLine, line1p1, line1p2 }; }
		sharePoint = true;
		overlapPoint = line1p1;
	}
	else if (line1p1 == line2p2)
	{
		if (line1p2 == line2p1) { return { LineIntersectionData::type::sameLine, line1p1, line1p2 }; }
		sharePoint = true;
		overlapPoint = line1p1;
	}
	else if (line1p2 == line2p1) 
	{ 
		sharePoint = true; 
		overlapPoint = line1p2; 
	}
	else if (line1p2 == line2p2) 
	{ 
		sharePoint = true; 
		overlapPoint = line1p2; 
	}

	bool l1Vertical = false;
	bool l2Vertical = false;
	if (line1p2.x < line1p1.x) // making lines in increasing values of x, or if vertical increasing y
	{
		auto temp = line1p2;
		line1p2 = line1p1;
		line1p1 = temp;
	}
	else if (line1p2.x == line1p1.x)
	{
		l1Vertical = true;
		if (line1p2.y < line1p1.y)
		{
			auto temp = line1p2;
			line1p2 = line1p1;
			line1p1 = temp;
		}
	}

	if (line2p2.x < line2p1.x) // making lines in increasing values of x, or if vertical increasing y
	{
		auto temp = line2p2;
		line2p2 = line2p1;
		line2p1 = temp;
	}
	else if (line2p2.x == line2p1.x)
	{
		l2Vertical = true;
		if (line2p2.y < line2p1.y)
		{
			auto temp = line2p2;
			line2p2 = line2p1;
			line2p1 = temp;
		}
	}
	if(line1p2.x < line2p1.x || line1p1.x > line2p2.x) {return { LineIntersectionData::type::noIntersection, {0,0}, {0,0} }; }
	auto maxY1 = line1p1.y;
	maxY1 = std::max(maxY1, line1p2.y);
	auto maxY2 = line2p1.y;
	maxY2 = std::max(maxY2, line2p2.y);
	auto minY1 = line1p1.y;
	minY1 = std::min(minY1, line1p2.y);
	auto minY2 = line2p1.y;
	minY2 = std::min(minY2, line2p2.y);
	if(maxY1 < minY2 || maxY2 < minY1) { return { LineIntersectionData::type::noIntersection, {0,0}, {0,0} }; }

	glm::vec3 line1dir = glm::vec3(line1p2.x - line1p1.x, line1p2.y - line1p1.y, 0);
	glm::vec3 line2dir = glm::vec3(line2p2.x - line2p1.x, line2p2.y - line2p1.y, 0);
	glm::vec3 cross = glm::cross(line1dir, line2dir);
	if (cross.z == 0)
	{
		glm::vec3 line1Begin2Line2Begin = glm::vec3(line2p1.x - line1p1.x, line2p1.y - line1p1.y, 0);
		glm::vec3 cross2 = glm::cross(line1dir, line1Begin2Line2Begin);
		if (cross2.z != 0) { return { LineIntersectionData::type::noIntersection, {0,0}, {0,0} }; }
		if (!l1Vertical)
		{
			if (sharePoint)
			{
				if (line1p1.x == line2p1.x && line1p1.x > line2p2.x) { return { LineIntersectionData::type::lineContainsLine, line1p1, line2p2 }; }
				if (line1p1.x == line2p1.x && line1p1.x < line2p2.x) { return { LineIntersectionData::type::lineContainsLine, line1p1, line2p1 }; }
				                                                       return { LineIntersectionData::type::pointPointOverlap, line1p1, {0,0}};
			}
			else if (line1p1.x < line2p1.x && line1p2.x > line2p2.x) { return { LineIntersectionData::type::lineContainsLine, line2p1, line2p2 }; }
			else if (line2p1.x < line1p1.x && line2p2.x > line1p2.x) { return { LineIntersectionData::type::lineContainsLine, line1p1, line1p2 }; }
			else if (line1p2.x < line2p1.x || line1p1.x > line2p2.x) { return { LineIntersectionData::type::noIntersection, {0,0}, {0,0}       }; }
			else if (line1p2.x > line2p1.x)                          { return { LineIntersectionData::type::lineLineOvelap, line2p1, line1p2   }; }
			else if (line1p1.x < line2p2.x)                          { return { LineIntersectionData::type::lineLineOvelap, line1p1, line2p2   }; }
			else                                                     { throw std::exception("LineIntersectionData GetLineLineIntersection uncovered case x"); return{}; }
		}
		if (sharePoint)
		{
			if (line1p1.y == line2p1.y && line1p1.y > line2p2.y) { return { LineIntersectionData::type::lineContainsLine, line1p1, line2p2 }; }
			if (line1p1.y == line2p1.y && line1p1.y < line2p2.y) { return { LineIntersectionData::type::lineContainsLine, line1p1, line2p1 }; }
			return { LineIntersectionData::type::pointPointOverlap, line1p1, {0,0} };
		}
		else if (line1p1.y < line2p1.y && line1p2.y > line2p2.y)      { return { LineIntersectionData::type::lineContainsLine ,  line2p1  , line2p2 }; }
		else if (line2p1.y < line1p1.y && line2p2.y > line1p2.y) { return { LineIntersectionData::type::lineContainsLine ,  line1p1  , line1p2 }; }
		else if (line1p2.y < line2p1.y || line1p1.y > line2p2.y) { return { LineIntersectionData::type::noIntersection   ,  {0,0}    , {0,0}   }; }
		else if (line1p2.y > line2p1.y)                          { return { LineIntersectionData::type::lineLineOvelap   ,  line2p1  , line1p2 }; }
		else if (line1p1.y < line2p2.y)                          { return { LineIntersectionData::type::lineLineOvelap   ,  line1p1  , line2p2 }; }
		else { throw std::exception("LineIntersectionData GetLineLineIntersection uncovered case y"); return{}; }
	}
	else if (cross.z != 0 && sharePoint) { return { LineIntersectionData::type::pointPointOverlap, overlapPoint, {0,0} }; }

	auto t = ((line1p1.x - line2p1.x) * (line2p1.y - line2p2.y) - (line1p1.y - line2p1.y) * (line2p1.x - line2p2.x)) 
		   / ((line1p1.x - line1p2.x) * (line2p1.y - line2p2.y) - (line1p1.y - line1p2.y) * (line2p1.x - line2p2.x));

	auto u = (-1) * ((line1p1.x - line1p2.x) * (line1p1.y - line2p1.y) - (line1p1.y - line1p2.y) * (line1p1.x - line2p1.x))
		          / ((line1p1.x - line1p2.x) * (line2p1.y - line2p2.y) - (line1p1.y - line1p2.y) * (line2p1.x - line2p2.x)) ;
	if (t > 1 || t < 0 || u > 1 || u < 0) { return { LineIntersectionData::type::noIntersection, {0,0}, {0,0}}; }
	auto intersection = line1p1 + t * (line1p2 - line1p1);
	if (t == 0 || t == 1) { return { LineIntersectionData::type::lineContainsPoint, intersection, {0,0} }; }
	if (u == 0 || u == 1) { return { LineIntersectionData::type::lineContainsPoint, intersection, {0,0} }; }
	if(intersection == line2p1 || intersection == line2p2) { return { LineIntersectionData::type::lineContainsPoint, intersection, {0,0} }; }
	return { LineIntersectionData::type::lineLineIntersection, intersection, {0,0} };
}
std::vector<size_t>
GetQuadFromTri                                     (const std::array<size_t, 3>&     inTri1, 
	                                                const std::array<size_t, 3>&     inTri2)
{
	int idx11 = -1, idx12 = -1, idx13, idx21 = -1, idx22 = -1, idx23;
	std::vector<size_t> outVal;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (inTri1[i] == inTri2[j])
			{
				if (idx11 == -1)
				{
					idx11 = i;
				}
				else
				{
					idx12 = i;
				}
				if (idx21 == -1)
				{
					idx21 = j;
					continue;
				}
				else
				{
					idx22 = j;
					continue;
				}
			}
		}
	}
	if (idx11 == 0 && idx12 == 1) { idx13 = 2; }
	if (idx11 == 1 && idx12 == 2) { idx13 = 0; }
	if (idx11 == 0 && idx12 == 2) { idx13 = 1; }
	if (idx21 == 0 && idx22 == 1) { idx23 = 2; }
	if (idx21 == 1 && idx22 == 2) { idx23 = 0; }
	if (idx21 == 0 && idx22 == 2) { idx23 = 1; }
	outVal = { inTri1[idx11], inTri1[idx13], inTri1[idx12], inTri2[idx23] };
	return outVal;
}





