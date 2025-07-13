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
Delaunay::Create2DUnconstrainedNew(std::string inPath)
{
	//PopulatePoints(inPath);
	//AddExtraPoints(0.1, 1, 1);
	mMshContainer.readFile(inPath);
	AddExtraPointsSimpleNew(0.5, 0.5, 0);
	//mMshContainer.populateFromGlm(mPoints, mEdges);
	//Timer::startTime();
	Create2DUnconstrainedNew();
	//Timer::time("stopped");
	//Timer::stopTime();
	//std::cout << mPoints.size();
	ForceConstraintsNew();
	return CreateModelNew();
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
	size_t iteration = 0;
	while (mMshContainer.hasFloatPoints())
	{
		iteration++;
		TwoDMeshContainer::TriVec triToDelete;
		TwoDMeshContainer::TriVec triToCreate;
		TwoDMeshContainer::TriHash triVisited;
		TwoDMeshContainer::TriVec triToVisit;
		TwoDMeshContainer::EdgeToSizeTable visitedEdge;

		auto& tri = *mMshContainer.getRemainingTri().begin();
		auto  ptIdx = *mMshContainer.getPointsInTri(tri).begin();
		triToVisit.push_back(tri);
		triVisited.insert(tri);
		while (!triToVisit.empty())
		{
			auto curTri = triToVisit[triToVisit.size() - 1];
			triToVisit.pop_back();
			if (IsPointInCircumference(mMshContainer.getPoints()[ptIdx],
				mMshContainer.getPoints()[curTri[0]],
				mMshContainer.getPoints()[curTri[1]],
				mMshContainer.getPoints()[curTri[2]]))
			{
				auto  neighbors = mMshContainer.getNeighborTri(curTri);
				for (auto& tr : neighbors) 
				{ 
					if(!triVisited.contains(tr))
					{ 
						triToVisit.push_back(tr); 
						triVisited.insert(tr);
					}
				}
				triToDelete.push_back(curTri);
				visitedEdge[{ curTri[0], curTri[1] }]++;
				visitedEdge[{ curTri[1], curTri[2] }]++;
				visitedEdge[{ curTri[0], curTri[2] }]++;
			}
		}
		for (auto& edge : visitedEdge)
		{
			if (edge.second < 2)
			{
				triToCreate.push_back({ edge.first[0], edge.first[1], ptIdx });
				std::sort(triToCreate[triToCreate.size() - 1].begin(), triToCreate[triToCreate.size() - 1].end());
			}
		}
		mMshContainer.removeAndReplaceTri(triToDelete, triToCreate, ptIdx);
	}
	DeleteExtraPointTriNew();
}
void
Delaunay::CreateFirstTriNew()
{
	auto maxMin = mMshContainer.getMinMax();
	double avgX = (maxMin[0] + maxMin[2]) / 2.0;
	double avgY = (maxMin[1] + maxMin[3]) / 2.0;
	double xDiff = maxMin[2] - maxMin[0];
	double yDiff = maxMin[3] - maxMin[1];
	MyVec2 topVertex   = MyVec2(avgX, avgY + 3 * yDiff);
	MyVec2 bottomLeft  = MyVec2(avgX - 2 * xDiff, avgY - yDiff * 2);
	MyVec2 bottomRight = MyVec2(avgX + 2 * xDiff, avgY - yDiff * 2);
	mMshContainer.addPoint(topVertex);
	mMshContainer.addPoint(bottomLeft);
	mMshContainer.addPoint(bottomRight);
	auto   nPoints = mMshContainer.getNumPoints();
	mMshContainer.initializeDelaunay({ nPoints - 3, nPoints - 2, nPoints - 1 });
}
void
Delaunay::DeleteExtraPointTriNew()
{
	TwoDMeshContainer::TriVec toDelete;
	auto nPoints = mMshContainer.getNumPoints();
	for (auto& tri : mMshContainer.getTriangles())
	{
		if (tri[0] >= nPoints - 3 || tri[1] >= nPoints - 3 || tri[2] >= nPoints - 3)
		{
			toDelete.push_back(tri);
		}
	}
	for (auto& tri : toDelete) { mMshContainer.removeTriangle(tri); }
	for (int i = 0; i < 3; i++) { mMshContainer.removeLastPoint(); }
}
void
Delaunay::VerifyDelaunayNew()
{
	for (auto& tri : mMshContainer.getTriangles())
	{
		for (size_t ndIdx = 0; ndIdx < mMshContainer.getPoints().size(); ndIdx++)
		{
			if (mMshContainer.getTriFromPts(ndIdx).size() == 0) { continue; }
			if (IsPointInCircumference(mMshContainer.getPoints()[ndIdx], mMshContainer.getPoints()[tri[0]], mMshContainer.getPoints()[tri[1]], mMshContainer.getPoints()[tri[2]]))
			{
				std::cout << "error detected\n";
			}
		}
	}
}
void 
Delaunay::CompareMshContainerWithOld()
{
	if (mMshContainer.getTriangles().size() != mTris.size())
	{
		throw std::logic_error("tri nums don't equal");
	}
	for (auto& tri : mMshContainer.getTriangles())
	{
		if (mTris.count(tri) != 1)
		{
			throw std::logic_error("found non existant tri");
			std::cout << "here";
		}
	}
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
		if (IsPointInCircumference(mPoints[inPointIdx], mPoints[tri[0]], mPoints[tri[1]], mPoints[tri[2]]))
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
#if SAFE_MODE
		i++;
		if (i > 60)
		{
			std::cout << "a lot of points added";
		}
#endif
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
Mesh*
Delaunay::CreateModelNew()
{
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	for (auto& point : mMshContainer.getPoints())
	{
		vertices.push_back({ point.x, point.y, 0 });
	}
	for (auto& tri : mMshContainer.getTriangles())
	{
		indices.push_back(tri[0]);
		indices.push_back(tri[1]);
		indices.push_back(tri[2]);
	}
	return new Mesh({ std::move(vertices) }, { std::move(indices) });
}
void
Delaunay::ForceConstraintsNew()
{
	auto nEdges = mMshContainer.getNumEdges();
	for (size_t edgeIdx = 0; edgeIdx < nEdges; edgeIdx++)
	{
		auto edgePtIdx = mMshContainer.getEdgeIdx(edgeIdx);
		if(edgePtIdx[1] < edgePtIdx[0]) 
		{
			auto temp = edgePtIdx[1];
			edgePtIdx[1] = edgePtIdx[0];
			edgePtIdx[0] = temp;
		}
		if (mMshContainer.doesEdgeExist(edgePtIdx[0], edgePtIdx[1])) { continue; }
		auto edgeIntersectionData = GetIntersectionsNew(edgePtIdx[0], edgePtIdx[1]);
		HandleIntersectionsNew(std::move(edgeIntersectionData));
	}
}
std::list<Delaunay::IntersectionDataNew>
Delaunay::GetIntersectionsNew(size_t inBeginIdx, size_t inEndIdx)
{
	std::list<IntersectionDataNew> outVal;
	MshCont::EdgeHash visitedEdge;
	MshCont::PointHash visitedPoints;
	auto prevPtId = inBeginIdx;
	visitedPoints.insert(mMshContainer.getPoints()[prevPtId]);
	MshCont::EdgeNds prevEdge = { -1, -1 };
	bool isLastPt = true;
	MshCont::Tri nextTri;
	while (true)
	{
		bool cont = false;
		MshCont::TriPtrHash triOptions;
		if (isLastPt)
		{
			triOptions = mMshContainer.getTriFromPts(prevPtId);
		}
		else
		{
			triOptions = mMshContainer.getTriFromEdge(prevEdge[0], prevEdge[1]);
		}

		for (auto& triPtr : triOptions)
		{
			if (triPtr->at(0) == inEndIdx || triPtr->at(1) == inEndIdx || triPtr->at(2) == inEndIdx) { return outVal; }
			for (int i = 0; i < 3; i++)
			{
				if (!visitedPoints.contains(mMshContainer.getPoints()[triPtr->at(i)]) && IsPointOnLine(mMshContainer.getPoints()[prevPtId],
															                                           mMshContainer.getPoints()[inEndIdx],
															                                           mMshContainer.getPoints()[triPtr->at(i)]))
				{
					visitedPoints.insert(mMshContainer.getPoints()[triPtr->at(i)]);
					cont = true;
					isLastPt = true;
					prevPtId = triPtr->at(i);
					//outVal.push_back({ IntersectionDataNew::TypeIntersection::point, mMshContainer.getPoints()[triPtr->at(i)], {} });
					continue;
				}
				size_t idx1 = triPtr->at(i);
				size_t idx2 = triPtr->at((i+1)%3);
				if (idx2 < idx1) { auto temp = idx2; idx2 = idx1; idx1 = temp; }
				if (!visitedEdge.contains({ idx1, idx2 }) && DoLinesIntersect(mMshContainer.getPoints()[prevPtId],
															                  mMshContainer.getPoints()[inEndIdx],
																		      mMshContainer.getPoints()[idx1],
															                  mMshContainer.getPoints()[idx2]))
				{
					visitedEdge.insert({ idx1, idx2 });
					cont = true;
					isLastPt = false;
					prevEdge = { idx1, idx2 };
					outVal.push_back({ IntersectionDataNew::TypeIntersection::edge, {}, {idx1, idx2} });
					continue;
				}
			}
			if (cont) { continue; }
		}
	}
	return outVal;
}
void 
Delaunay::HandleIntersectionsNew(std::list<IntersectionDataNew> inData)
{
	auto it = inData.begin();
	int i = 0;
	while (!inData.empty())
	{
		i++;
		auto& tris = mMshContainer.getTriFromEdge(it->edge);
#if SAFE_MODE == 1
		if (tris.size() != 2) { throw std::logic_error("edge has dif num of tri than 2"); }
#endif
		auto itr = tris.begin();
		auto& tri1 = **itr;
		itr++;
		auto& tri2 = **itr;
		auto quad = GetQuadFromTri(tri1, tri2);
		if (IsPolygonConvex({ mMshContainer.getPoints()[quad[0]],
							  mMshContainer.getPoints()[quad[1]],
							  mMshContainer.getPoints()[quad[2]],
							  mMshContainer.getPoints()[quad[3]]}))
		{
			MshCont::Tri tri1New = { quad[0], quad[1], quad[3] };
			MshCont::Tri tri2New = { quad[1], quad[2], quad[3] };
			std::sort(tri1New.begin(), tri1New.end());
			std::sort(tri2New.begin(), tri2New.end());
			mMshContainer.removeTriangle(tri1);
			mMshContainer.removeTriangle(tri2);
			mMshContainer.addTriangle(tri1New);
			mMshContainer.addTriangle(tri2New);
			it = inData.erase(it);
		}
		else
		{
			it++;
		}
		if (it == inData.end()) { it = inData.begin(); }
		bool p = false;
		if (p) { return; }
		if (i > 1000)
		{
			return;
		}
	}
}
void 
Delaunay::AddExtraPointsSimpleNew(float inDist, float inFillDist, int jmpForFillPts)
{
	double newMinX, newMinY, newMaxX, newMaxY;
	auto edges = mMshContainer.getMinMax();
	double xDiff = edges[2] - edges[0];
	double yDiff = edges[3] - edges[1];
	double xAvg  = (edges[2] + edges[0])/2;
	double yAvg  = (edges[1] + edges[3])/2;
	newMinX = xAvg - xDiff / 1.8;
	newMaxX = xAvg + xDiff / 1.8;
	newMinY = yAvg - yDiff / 1.8;
	newMaxY = yAvg + yDiff / 1.8;
	size_t nJmpsV = yDiff * 1.12 / inFillDist + 2;
	size_t nJmpsH = xDiff * 1.12 / inFillDist + 2;
	for (size_t i = 0; i < nJmpsV; i++)
	{
		double offset = i % 2 == 0? 0 : inFillDist / 2;
		for (size_t j = 0; j < nJmpsH; j++)
		{
			mMshContainer.addPoint( newMinX + offset + inFillDist * j, newMinY + inFillDist * i);
		}
	}
	auto nEdges = mMshContainer.getNumEdges();
	for (size_t i = 0; i < nEdges; i++)
	{
		auto edge = mMshContainer.getEdgeIdx(i);

	}
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


