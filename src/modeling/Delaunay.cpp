#include "../../headers/modeling/Delaunay.h"
#include "../../headers/modeling/MyMath.h"
#include "../../headers/core/Timer.h"
Mesh*
Delaunay::Create2DUnconstrained(std::string inPath)
{
	PopulatePoints(std::move(inPath));
	Create2DUnconstrained();
	return CreateModel();
}
Mesh*
Delaunay::Create2DConstrained(std::string inPath)
{
	PopulatePoints(std::move(inPath));
	Create2DUnconstrained();
	ForceConstraints();
	return CreateModel();
}
Mesh*
Delaunay::Create2DConsWEdge(std::string inPath)
{
	PopulatePoints(std::move(inPath));
	Create2DUnconstrained();
	ForceConstraints();
	DeleteOutsideTriangles();
	return CreateModel();
}
Mesh* 
Delaunay::Create2DConsWEdgeNew(std::string inPath)
{
	mMshContainer.readFile(std::move(inPath));
	Create2DUnconstrainedNew();
	return nullptr;
}
Mesh*
Delaunay::Create2DConsSmoothedWEdge(std::string inPath, float pointDist, float sclForFillPts, int jmpForFillPts)
{
	PopulatePoints(std::move(inPath));
	AddExtraPoints(pointDist, sclForFillPts, jmpForFillPts);
	Create2DUnconstrained();
	ForceConstraints();
	DeleteOutsideTriangles();
	return CreateModel();
}
void 
Delaunay::Create2DUnconstrained()
{
	CreateFirstTri();
	for (size_t i = 0; i < mPoints.size() - 3; i++)
	{
		auto trisToDelete = GetTriContainingPoint(i);
		DeleteAndCreateTri(i, trisToDelete);
	}
	DeleteExtraPointTri();
}
void
Delaunay::Create2DUnconstrainedNew()
{
	CreateFirstTriNew();
	while (mMshContainer.hasFloatPoints())
	{
		auto& tri = *mMshContainer.GetRemainingTri().begin();
		auto  neighbors = mMshContainer.GetNeighborTri(tri);
		for (auto& neighborTri : neighbors)
		{

		}
	}
}
void
Delaunay::CreateFirstTriNew()
{
	auto maxMin = mMshContainer.GetMaxMin();
	double avgX = (maxMin[0] + maxMin[2]) / 2.0;
	double avgY = (maxMin[1] + maxMin[3]) / 2.0;
	double xDiff = mMaxX - mMinX;
	double yDiff = mMaxY - mMinY;
	MyVec2 topVertex   = MyVec2(avgX, avgY + 3 * xDiff);
	MyVec2 bottomLeft  = MyVec2(avgX - 2 * xDiff, avgY - yDiff * 2);
	MyVec2 bottomRight = MyVec2(avgX + 2 * xDiff, avgY - yDiff * 2);
	mMshContainer.addPoint(topVertex);
	mMshContainer.addPoint(bottomLeft);
	mMshContainer.addPoint(bottomRight);
	auto   nPoints = mMshContainer.getNumPoints();
	mMshContainer.InitializeDelaunay({ nPoints - 3, nPoints - 2, nPoints - 1 });
}
void
Delaunay::DeleteOutsideTriangles()
{
	std::vector<std::array<size_t, 3>> toDelete;
	for (auto& tri : mTris)
	{
		if (!IsTriInsideConstraints(tri))
		{
			toDelete.push_back(tri);
		}
	}
	for (auto& tri : toDelete)
	{
		mTris.erase(tri);
	}
}
bool
Delaunay::IsTriInsideConstraints(const std::array<size_t, 3>& inTri)
{
	auto triCenter = (mPoints[inTri[0]] + mPoints[inTri[1]] + mPoints[inTri[2]]) / 3.0f;
	auto end = triCenter;
	end.y += (mMaxY - end.y) * 2;
	size_t curIntersection = 0;
	for (size_t i = 0; i < mEdges.size() - 1; i++)
	{
		auto intersection = GetLineLineIntersection(triCenter, end, mPoints[mEdges[i]], mPoints[mEdges[i+1]]);
		if (intersection.intersectionType == LineIntersectionData::type::lineContainsPoint) { curIntersection += 1; }
		else if (intersection.intersectionType != LineIntersectionData::type::noIntersection) { curIntersection += 2; }
	}
	if ((curIntersection % 2) != 0) { 
		std::cout << "error";
		throw std::logic_error("somehow got a non 2 value"); 
	}
	curIntersection /= 2;
	return (curIntersection % 2) == 1;
}
void
Delaunay::ForceConstraints()
{

	size_t numEdgeConstrain = mEdges.size() - 1;
	for (size_t i = 0; i < numEdgeConstrain; i++)
	{
		auto p1 = mPoints[mEdges[i]];
		auto p2 = mPoints[mEdges[i + 1]];
		auto intersections = GetTriContainingEdge(p1, p2);
		auto sortedData = GetSortedData(std::move(intersections), mPoints[mEdges[i]]);
		CutTriangles(std::move(sortedData));
	}
}
void
Delaunay::PopulatePoints(std::string inPath)
{
	std::ifstream rdStream(inPath);
	std::string word;
	float x, y;
	size_t idx;
	while (true)
	{
		if (!(rdStream >> word)) {break;};
		if (word == "end_points") { break; }
		x = stof(word);
		rdStream >> y;
		if (x > mMaxX)
		{
			mMaxX = x;
		}
		else if (x < mMinX)
		{
			mMinX = x;
		}
		if (y > mMaxY)
		{
			mMaxY = y;
		}
		else if (y < mMinY)
		{
			mMinY = y;
		}
		AddPoint(x, y);
	}
	if (word == "end_points")
	{
		while (true)
		{
			if (!(rdStream >> idx)) { break; };
			mEdges.push_back(idx);
		}
	}
	rdStream.close();
}
void
Delaunay::CreateFirstTri()
{
	float avgX = (mMaxX + mMinX) / 2.0;
	float avgY = (mMaxY + mMinY) / 2.0;
	float xDiff = mMaxX - mMinX;
	float yDiff = mMaxY - mMinY;
	glm::vec2 topVertex = glm::vec2(avgX, avgY + 3 * xDiff);
	glm::vec2 bottomLeft = glm::vec2(avgX - 2 * xDiff, avgY - yDiff * 2);
	glm::vec2 bottomRight = glm::vec2(avgX + 2 * xDiff, avgY - yDiff * 2);
	mPoints.push_back(topVertex);
	mPoints.push_back(bottomLeft);
	mPoints.push_back(bottomRight);
	mTris.insert({ mPoints.size() - 1, mPoints.size() - 2, mPoints.size() - 3 });
}
std::vector<std::array<size_t, 3>>
Delaunay::GetTriContainingPoint(size_t inPointIdx)
{
	std::vector<std::array<size_t, 3>> outVal;

	for (auto& tri : mTris)
	{
		if (IsPointIntCircumference(mPoints[inPointIdx], mPoints[tri[0]], mPoints[tri[1]], mPoints[tri[2]]))
		{
			outVal.push_back({tri});
		}
	}
	return std::move(outVal);
}
void
Delaunay::AddExtraPoints(float inDist, float sclForFillPts, int jmpForFillPts)
{
	for (size_t i = 0; i < mEdges.size() - 1; i++)
	{
		for (size_t j = i + 2; j < mEdges.size(); j += jmpForFillPts)
		{
			CreateSecondaryLinePoints(inDist * sclForFillPts, mPoints[mEdges[i]], mPoints[mEdges[j]]);
		}
		CreateEdgeLinePoints(inDist / 3, mPoints[mEdges[i]], mPoints[mEdges[i + 1]]);
	}
}
void
Delaunay::CreateSecondaryLinePoints(float inDist, const glm::vec2 inP1, const glm::vec2 inP2)
{
	static std::vector<glm::vec2> p;
	p.push_back(inP1);
	p.push_back(inP2);
	glm::vec2 normalizedDirection = glm::normalize(inP2 - inP1) * inDist;
	glm::vec2 curPos = inP1 + normalizedDirection;
	glm::vec2 p2ToP1 = inP1 - inP2;
	int i = 0;
	while (glm::dot(curPos - inP2, p2ToP1) > 0)
	{
		i++;
		if (i > 60)
		{
			std::cout << "a lot of points added";
		}
		auto newPoint1 = curPos;
		AddPoint(newPoint1.x, newPoint1.y);
		curPos += normalizedDirection;
	}
}
void
Delaunay::CreateEdgeLinePoints(float inDist, const glm::vec2 inP1, const glm::vec2 inP2)
{
	static std::vector<glm::vec2> p;
	p.push_back(inP1);
	p.push_back(inP2);
	float inverseSlope;
	bool  isVertical = false;
	if (inP2.y - inP1.y != 0)
	{
		inverseSlope = (-1) * (inP2.x - inP1.x) / (inP2.y - inP1.y);
	}
	else
	{
		isVertical = true;
	}
	glm::vec2 pInverse;
	if (!isVertical)
	{
		pInverse = { 1, inverseSlope };
		pInverse = glm::normalize(pInverse) * inDist * 0.5f;
	}
	else
	{
		pInverse = glm::vec2({ 0, 1 }) * inDist * 0.5f;
	}
	glm::vec2 normalizedDirection = glm::normalize(inP2 - inP1) * inDist;
	glm::vec2 curPos = inP1 + normalizedDirection;
	glm::vec2 p2ToP1 = inP1 - inP2;
	int i = 0;
	while (glm::dot(curPos - inP2, p2ToP1) > 0)
	{
		i++;
		if (i > 40) { std::cout << "a lot of points added"; }
		if (i % 3 == 0)
		{
			auto newPoint1 = curPos + pInverse * 2.5f;
			auto newPoint2 = curPos + (normalizedDirection * 0.5f) - pInverse * 2.5f;
			AddPoint(newPoint1.x, newPoint1.y);
			AddPoint(newPoint2.x, newPoint2.y);
		}
		auto newPoint1 = curPos + pInverse;
		auto newPoint2 = curPos + (normalizedDirection * 0.5f) - pInverse;
		AddPoint(newPoint1.x, newPoint1.y);
		AddPoint(newPoint2.x, newPoint2.y);
		curPos += normalizedDirection;
	}
}
void 
Delaunay::DeleteAndCreateTri(size_t inPtIdx, std::vector<std::array<size_t, 3>> inTriToDelete)
{
	std::unordered_map<std::array<size_t, 2>, size_t, UnorderedArrayHash<size_t, 2>, UnorderedArrayEqual<size_t, 2>> visitedEdges;

	for (auto& tri : inTriToDelete)
	{
		visitedEdges[{tri[0], tri[1]}]++;
		visitedEdges[{tri[1], tri[2]}]++;
		visitedEdges[{tri[2], tri[0]}]++;
	}

	for (auto& tri : inTriToDelete) { mTris.erase(tri); }

	for (auto& edge : visitedEdges)
	{
		if (edge.second < 2)
		{
			mTris.insert({ edge.first[0], edge.first[1], inPtIdx });
		}
	}
}
void 
Delaunay::DeleteExtraPointTri()
{
	for (auto it = mTris.begin(); it != mTris.end();)
	{
		if ((*it)[0] >= mPoints.size() - 3 || (*it)[1] >= mPoints.size() - 3 || (*it)[2] >= mPoints.size() - 3)
		{
			it = mTris.erase(it);
		}
		else
		{
			it++;
		}
	}
	mPoint2Idx.erase({ mPoints[mPoints.size() - 1].x, mPoints[mPoints.size() - 1].y });
	mPoints.pop_back();
	mPoint2Idx.erase({ mPoints[mPoints.size() - 1].x, mPoints[mPoints.size() - 1].y });
	mPoints.pop_back();
	mPoint2Idx.erase({ mPoints[mPoints.size() - 1].x, mPoints[mPoints.size() - 1].y });
	mPoints.pop_back();
}
Mesh*
Delaunay::CreateModel()
{
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	for (auto& point : mPoints)
	{
		vertices.push_back({ point.x, point.y, 0 });
	}
	for (auto& tri : mTris)
	{
		indices.push_back(tri[0]);
		indices.push_back(tri[1]);
		indices.push_back(tri[2]);
	}
	return new Mesh({ std::move(vertices) }, { std::move(indices) });
}
std::vector<Delaunay::IntersectionData>
Delaunay::GetTriContainingEdge(glm::vec2 p1, glm::vec2 p2)
{
	std::vector<IntersectionData> outVal;
	for (auto& tri : mTris)
	{
		auto intersection = IsLineIntersectingTri(p1, p2, tri);
		if (intersection.type1 != IntersectionData::TypeIntersection::noIntersection)
		{
			outVal.push_back(intersection);
		}
	}
	return std::move(outVal);
}
void
Delaunay::CutTriangles(std::vector<SortedData> inData)
{
	auto GetSharedTri = [](std::vector<std::array<size_t, 3>>& in1, std::vector<std::array<size_t, 3>>& in2)
		{
			for (size_t i = 0; i<in1.size(); i++)
			{
				for (size_t j = 0; j<in2.size(); j++)
				{
					if (in1[i] == in2[j]) { return std::pair<size_t, size_t>({ i,j }); }
				}
			}
			throw std::logic_error("got here. no matching tri in consequtive intersection");
			return std::pair<size_t, size_t>({ -1, -1 });
		};
	auto GetIdxSingle = [this](size_t triId1, size_t triId2, size_t triId3, size_t lnId1, size_t lnId2)
		{
			glm::vec2 line    = mPoints[lnId2]  - mPoints[lnId1];
			glm::vec2 lineTo1 = mPoints[triId1] - mPoints[lnId1];
			glm::vec2 lineTo2 = mPoints[triId2] - mPoints[lnId1];
			glm::vec2 lineTo3 = mPoints[triId3] - mPoints[lnId1];
			float     z1 = line.x * lineTo1.y - lineTo1.x * line.y;
			float     z2 = line.x * lineTo2.y - lineTo2.x * line.y;
			float     z3 = line.x * lineTo3.y - lineTo3.x * line.y;
			if (z1 / z3 > 0) { return triId2; }
			if (z1 / z2 > 0) { return triId3; }
			else { return triId1; }
		};

	for (size_t idx = 0; idx < inData.size() - 1; idx++)
	{
		auto triPairIdx = GetSharedTri(inData[idx].tris, inData[idx + 1].tris);
		auto sharedTri = inData[idx].tris[triPairIdx.first];
		if ((inData[idx].location == sharedTri[0] || inData[idx].location == sharedTri[1] || inData[idx].location == sharedTri[2]) &&
			(inData[idx + 1].location == sharedTri[0] || inData[idx + 1].location == sharedTri[1] || inData[idx + 1].location == sharedTri[2]))
		{
			continue;
		}
		else if ((inData[idx].location == sharedTri[0]) || (inData[idx+1].location == sharedTri[0]))
		{
			mTris.insert({ inData[idx].location , sharedTri[1], inData[idx + 1].location });
			mTris.insert({ inData[idx].location, sharedTri[2], inData[idx + 1].location });
		}
		else if ((inData[idx].location == sharedTri[1]) || (inData[idx + 1].location == sharedTri[1]))
		{
			mTris.insert({ inData[idx].location , sharedTri[0], inData[idx + 1].location });
			mTris.insert({ inData[idx].location, sharedTri[2], inData[idx + 1].location });
		}
		else if ((inData[idx].location == sharedTri[2]) || (inData[idx + 1].location == sharedTri[2]))
		{
			mTris.insert({ inData[idx].location , sharedTri[1], inData[idx + 1].location });
			mTris.insert({ inData[idx].location, sharedTri[0], inData[idx + 1].location });
		}
		else
		{
			auto single = GetIdxSingle(sharedTri[0], sharedTri[1], sharedTri[2], inData[idx].location, inData[idx + 1].location);
			size_t crossedShared = 9999999999999999999;
			mTris.insert({ single, inData[idx].location, inData[idx + 1].location });
			std::unordered_set<size_t> set = { sharedTri[0], sharedTri[1], sharedTri[2] };
			set.erase(single);
			for (auto& i : set)
			{
				if (crossedShared == 9999999999999999999)
				{
					crossedShared = i;
					mTris.insert({ i, inData[idx].location, inData[idx + 1].location });
				}
				else
				{
					if (i == inData[idx].edges[triPairIdx.first].first || i == inData[idx].edges[triPairIdx.first].second)
					{
						mTris.insert({ i, inData[idx].location, crossedShared });
					}
					else
					{
						mTris.insert({ i, inData[idx+1].location, crossedShared });
					}
				}
			}
		}
		mTris.erase(sharedTri);
	}
}
std::vector<Delaunay::SortedData>
Delaunay::GetSortedData(std::vector<IntersectionData> inData, glm::vec2 startingPoint)
{
	struct data
	{
		std::vector < std::array<size_t, 3>> tris;
		std::vector <std::pair<size_t, size_t>> edge;
	};
	std::vector<Delaunay::SortedData> outVal;
	std::unordered_map<size_t, data> pointsMapped;
	for (auto& data : inData)
	{
		auto pointIdx1 = AddPoint(data.point1.x, data.point1.y);
		pointsMapped[pointIdx1].tris.push_back(data.tri);
		pointsMapped[pointIdx1].edge.push_back(data.edge1);
		if (data.type2 != IntersectionData::TypeIntersection::noIntersection)
		{
			auto pointIdx2 = AddPoint(data.point2.x, data.point2.y);
			pointsMapped[pointIdx2].tris.push_back(data.tri);
			pointsMapped[pointIdx2].edge.push_back(data.edge2);
		}
	}
	for (auto& point : pointsMapped) { outVal.push_back({point.first, std::move(point.second.tris), std::move(point.second.edge)}); }
	std::sort(outVal.begin(), outVal.end(), [this, &startingPoint](Delaunay::SortedData& in1, Delaunay::SortedData& in2) 
		{
			glm::vec2 dir1 = mPoints[in1.location] - startingPoint;
			float     scl1 = dir1.x * dir1.x + dir1.y * dir1.y;
			glm::vec2 dir2 = mPoints[in2.location] - startingPoint;
			float     scl2 = dir2.x * dir2.x + dir2.y * dir2.y;
			return scl1 < scl2;
		});
	return outVal;
}
size_t
Delaunay::AddPoint(float x, float y)
{
	auto itr = mPoint2Idx.find({ x,y });
	if (itr == mPoint2Idx.end())
	{
		mPoint2Idx[{x, y}] = mPoints.size();
		mPoints.push_back({ x,y });
		if (x > mMaxX)
		{
			mMaxX = x;
		}
		else if (x < mMinX)
		{
			mMinX = x;
		}
		if (y > mMaxY)
		{
			mMaxY = y;
		}
		else if (y < mMinY)
		{
			mMinY = y;
		}
		return mPoints.size() - 1;
	}
	return itr->second;
}
Delaunay::IntersectionData
Delaunay::IsLineIntersectingTri(glm::vec2 p1, glm::vec2 p2, const std::array<size_t, 3>& inTri)
{
	IntersectionData outData;
	outData.type1 = IntersectionData::TypeIntersection::noIntersection;
	outData.type2 = IntersectionData::TypeIntersection::noIntersection;
	outData.tri      = inTri;
	bool             alreadyCrossedPoint = false;
	size_t           firstCross;
	for (int i = 0; i < 3; i++) // checking for point intersections
	{
		if (IsPointOnLine(p1, p2, mPoints[inTri[i]]))
		{
			if (alreadyCrossedPoint)
			{
				outData.type1 = IntersectionData::TypeIntersection::edgeEdgePrll;
				outData.edge1 = { inTri[firstCross], inTri[i] };
				outData.point2 = mPoints[inTri[i]];
				outData.type2 = IntersectionData::TypeIntersection::edgeEdgePrll;
				outData.edge2 = { inTri[firstCross], inTri[i] };
				return outData;
			}
			else
			{
				alreadyCrossedPoint = true;
				firstCross = i;
				outData.type1 = IntersectionData::TypeIntersection::point;
				outData.edge1 = { -1, -1 };
				outData.point1 = mPoints[inTri[i]];
			}
		}
	}
	for (int i = 0; i < 3; i++) // checking for line line intersections
	{
		int j = (i + 1) % 3;
		auto intersection = GetLineLineIntersection(mPoints[inTri[i]], mPoints[inTri[j]], p1, p2);
		if (intersection.intersectionType == LineIntersectionData::type::noIntersection) { continue; }
		if (intersection.intersectionType == LineIntersectionData::type::lineContainsPoint ||
			intersection.intersectionType == LineIntersectionData::type::pointPointOverlap) { continue; }
		if (intersection.intersectionType != LineIntersectionData::type::lineLineIntersection) { throw std::logic_error("somehow got non linelineintersection"); }
		if (alreadyCrossedPoint)
		{
			outData.type2 = IntersectionData::TypeIntersection::edge;
			outData.edge2 = { inTri[i], inTri[j] };
			outData.point2 = intersection.p1;
			return outData;
		}
		else
		{
			alreadyCrossedPoint = true;
			outData.type1 = IntersectionData::TypeIntersection::point;
			outData.edge1 = { inTri[i], inTri[j] };
			outData.point1 = intersection.p1;
		}
	}
	return outData;
}


