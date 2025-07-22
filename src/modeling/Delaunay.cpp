#include "../../headers/modeling/Delaunay.h"
#include "../../headers/modeling/MyMath.h"
#include "../../headers/core/Timer.h"]
#include "../../headers/core/CsvWriter.h"
#include "../../headers/core/GlobalVariables.h"
Mesh*
Delaunay::Create2DUnconstrainedNew               ( std::string            inPath)
{
	mMshContainer.readFile(inPath);
	Create2DUnconstrainedNew();
	return CreateModelNew();
}
Mesh* 
Delaunay::Create2DConstrainedNew                 ( std::string            inPath, 
												   double                 inEdgeScls, 
												   double                 inGridScale)
{
	mMshContainer.readFile(inPath);
	Timer::startTime();
	auto points = mMshContainer.getPoints();
	AddExtraPointsSimpleNew(inEdgeScls, inGridScale);
	Timer::time();
	size_t nPoints = mMshContainer.getPoints().size();
	Create2DUnconstrainedNew();
	Timer::time();
	ForceConstraintsNew();
	Timer::time();
	auto nTriBeforeDelete = mMshContainer.getTriangles().size();
	//DeleteOutsideConsTriNew();
	Timer::time();
	Timer::stopTime();
	auto intervals = Timer::getIntervals();
	intervals.push_back(mMshContainer.getPoints().size());
	intervals.push_back(nTriBeforeDelete);
	intervals.push_back(mMshContainer.getTriangles().size());
	intervals.push_back(mMshContainer.getEdgesConstraint().size());
	//CsvWriter::SetData(intervals, "timingData.csv");
	return CreateModelNew();
}
Mesh*         
Delaunay::Create2DConstrainedDebug               ( std::string            inPath, 
                                                   double                 inEdgeScls, 
                                                   double                 inGridScale)
{
	mMshContainer.readFile(inPath);
	Timer::startTime();
	auto points = mMshContainer.getPoints();
	AddExtraPointsSimpleNew(inEdgeScls, inGridScale);
	Timer::time();
	size_t nPoints = mMshContainer.getPoints().size();
	Create2DUnconstrainedNew();
	Timer::time();
	ForceConstraintsNew();
	Timer::time();
	auto nTriBeforeDelete = mMshContainer.getTriangles().size();
	std::vector<MyVec2> outVec;
	
	DeleteOutsideConsTriNew();
	Timer::time();
	//Timer::stopTime();
	//auto intervals = Timer::getIntervals();
	//intervals.push_back(mMshContainer.getPoints().size());
	//intervals.push_back(nTriBeforeDelete);
	//intervals.push_back(mMshContainer.getTriangles().size());
	//intervals.push_back(mMshContainer.getEdgesConstraint().size());
	//CsvWriter::SetData(intervals, "timingData.csv");
	for (size_t i = 0; i < mMshContainer.getNumPoints(); i++)
	{
		auto& tri = mMshContainer.getTriFromPts(i);
		if (tri.size() == 0)
		{
			//outVec.push_back(mMshContainer.getPoints()[i]);
		}
	}
	//debugFunc(outVec);
	return CreateModelNew();
}
void
Delaunay::Create2DUnconstrainedNew               ( )
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
			if (IsPointInCircumCircle(mMshContainer.getPoints()[ptIdx],
				mMshContainer.getPoints()[curTri[0]],
				mMshContainer.getPoints()[curTri[1]],
				mMshContainer.getPoints()[curTri[2]], 1.0))
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
Delaunay::CreateFirstTriNew                      ( )
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
Delaunay::DeleteExtraPointTriNew                 ( )
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
Delaunay::VerifyDelaunayNew                      ( )
{
	for (auto& tri : mMshContainer.getTriangles())
	{
		for (size_t ndIdx = 0; ndIdx < mMshContainer.getPoints().size(); ndIdx++)
		{
			if (mMshContainer.getTriFromPts(ndIdx).size() == 0) { continue; }
			if (IsPointInCircumCircle(mMshContainer.getPoints()[ndIdx], mMshContainer.getPoints()[tri[0]], mMshContainer.getPoints()[tri[1]], mMshContainer.getPoints()[tri[2]]))
			{
				std::cout << "error detected\n";
			}
		}
	}
}
Mesh*
Delaunay::CreateModelNew                         ( )
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
Delaunay::ForceConstraintsNew                    ( )
{
	auto edges = mMshContainer.getEdgesConstraint();
	for (auto edge : edges)
	{
		if(edge[1] < edge[0]) 
		{
			auto temp = edge[1];
			edge[1] = edge[0];
			edge[0] = temp;
		}
		if (mMshContainer.doesTriEdgeExist(edge[0], edge[1])) { continue; }
		auto edgeIntersectionData = GetIntersectionsNew(edge[0], edge[1]);
		HandleIntersectionsNew(std::move(edgeIntersectionData));
	}
}
std::list<Delaunay::IntersectionDataNew>
Delaunay::GetIntersectionsNew                    ( size_t				  inBeginIdx, 
												   size_t                 inEndIdx)
{
	std::list<IntersectionDataNew> outVal;
	MshCont::EdgeHash visitedEdge;
	MshCont::PointHash visitedPoints;
	auto prevPtId = inBeginIdx;
	visitedPoints.insert(mMshContainer.getPoints()[prevPtId]);
	outVal.push_back({ IntersectionDataNew::TypeIntersection::point, inBeginIdx, {} });
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
			if (triPtr->at(0) == inEndIdx || triPtr->at(1) == inEndIdx || triPtr->at(2) == inEndIdx) 
			{ 
				outVal.push_back({ IntersectionDataNew::TypeIntersection::point, inEndIdx, {} });
				return outVal; 
			}
			for (int i = 0; i < 3; i++)
			{
				if (!visitedPoints.contains(mMshContainer.getPoints()[triPtr->at(i)]) && IsPointOnLine(mMshContainer.getPoints()[prevPtId],
															                                           mMshContainer.getPoints()[inEndIdx],
															                                           mMshContainer.getPoints()[triPtr->at(i)], 0.0000001))
				{
					visitedPoints.insert(mMshContainer.getPoints()[triPtr->at(i)]);
					cont = true;
					isLastPt = true;
					prevPtId = triPtr->at(i);
					outVal.push_back({ IntersectionDataNew::TypeIntersection::point, triPtr->at(i), {} });
					continue;
				}
				size_t idx1 = triPtr->at(i);
				size_t idx2 = triPtr->at((i+1)%3);
				if (idx2 < idx1) { auto temp = idx2; idx2 = idx1; idx1 = temp; }
				if (!visitedEdge.contains({ idx1, idx2 }) && DoLinesIntersect(mMshContainer.getPoints()[prevPtId],
															                  mMshContainer.getPoints()[inEndIdx],
																		      mMshContainer.getPoints()[idx1],
															                  mMshContainer.getPoints()[idx2],
																			  - 0.0000001))
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
Delaunay::HandleIntersectionsNew                 ( IntersectList          inData)
{
	auto it = inData.begin();
	size_t prevPt;
	bool isFirst = true;
	while (!inData.empty())
	{
		if (it->type == IntersectionDataNew::TypeIntersection::point)
		{
			if (isFirst)
			{
				prevPt = it->point;
				it = inData.erase(it);
				if (it == inData.end()) { it = inData.begin(); }
				isFirst = false;
				continue;
			}
			mMshContainer.addEdge(prevPt > it->point ? MshCont::EdgeNds{it->point, prevPt} : MshCont::EdgeNds{prevPt, it->point});
			prevPt = it->point;
			it = inData.erase(it);
		}
		else
		{
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
								  mMshContainer.getPoints()[quad[3]] }))
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
		}
		if (it == inData.end()) { it = inData.begin(); }
	}
}
void 
Delaunay::AddExtraPointsSimpleNew                ( double				  inDist, 
												   double                 inFillDist)
{
	AddGeneralNodesNew(inFillDist);
	auto& edges = mMshContainer.getEdgesConstraint();
	for (auto& edge : edges)
	{
		AddEdgeNodesNew(inDist, mMshContainer.getPoints()[edge[0]], mMshContainer.getPoints()[edge[1]]);
	}
}
void
Delaunay::AddGeneralNodesNew                     ( double                 inFillDist)
{
	double newMinX, newMinY, newMaxX, newMaxY;
	auto edges = mMshContainer.getMinMax();
	double xDiff = edges[2] - edges[0];
	double yDiff = edges[3] - edges[1];
	double xAvg = (edges[2] + edges[0]) / 2;
	double yAvg = (edges[1] + edges[3]) / 2;
	newMinX = xAvg - xDiff / 1.8;
	newMaxX = xAvg + xDiff / 1.8;
	newMinY = yAvg - yDiff / 1.8;
	newMaxY = yAvg + yDiff / 1.8;
	size_t nJmpsV = yDiff * 1.12 / inFillDist + 2;
	size_t nJmpsH = xDiff * 1.12 / inFillDist + 2;
	for (size_t i = 0; i < nJmpsV; i++)
	{
		double offset = i % 2 == 0 ? 0 : inFillDist / 2;
		for (size_t j = 0; j < nJmpsH; j++)
		{
			mMshContainer.addPoint(newMinX + offset + inFillDist * j, newMinY + inFillDist * i);
		}
	}
}
void
Delaunay::AddEdgeNodesNew                        ( double                 inDist, 
												   MyVec2                 inPt1, 
												   MyVec2                 inPt2)
{
	float inverseSlope;
	bool  isVertical = false;
	if (inPt2.y - inPt1.y != 0)
	{
		inverseSlope = (-1) * (inPt2.x - inPt1.x) / (inPt2.y - inPt1.y);
	}
	else
	{
		isVertical = true;
	}
	MyVec2 pInverse;
	if (!isVertical)
	{
		pInverse = { 1, inverseSlope };
		pInverse = glm::normalize(pInverse) * inDist;
	}
	else
	{
		pInverse = MyVec2({ 0, 1 }) * inDist;
	}
	MyVec2 normalizedDirection = glm::normalize(inPt2 - inPt1) * inDist;
	MyVec2 curPos = inPt1 + normalizedDirection;
	MyVec2 p2ToP1 = inPt1 - inPt2;
	int i = 0;
	while (glm::dot(curPos - inPt2, p2ToP1) > 0)
	{
		i++;
		if (i > 6000) { std::cout << "a lot of points added"; }
		if (i % 3 == 0)
		{
			auto newPoint1 = curPos + pInverse * 2.5;
			auto newPoint2 = curPos - (normalizedDirection * 0.5) - pInverse * 2.5;
			mMshContainer.addPoint(newPoint1.x, newPoint1.y);
			mMshContainer.addPoint(newPoint2.x, newPoint2.y);
		}
		auto newPoint1 = curPos + pInverse;
		auto newPoint2 = curPos - (normalizedDirection * 0.33) - pInverse;
		auto newPoint3 = curPos - (normalizedDirection * 0.66);
		mMshContainer.addPoint(newPoint1.x, newPoint1.y);
		mMshContainer.addPoint(newPoint2.x, newPoint2.y);
		mMshContainer.addPoint(newPoint3.x, newPoint3.y);
		curPos += normalizedDirection;
	}
}
void
Delaunay::DeleteOutsideConsTriNew                ( )
{
	MshCont::TriHash trisLeft = mMshContainer.getTriangles();
	MshCont::TriVec  trisInPath;
	MshCont::TriVec  trisToVisit;
	MshCont::Tri     curTri;
	MshCont::TriVec  triToDelete;
	MshCont::TriHash triHashaaa;
	bool             isOutside = false;
	auto             tris = *trisLeft.begin();
	trisToVisit.push_back(*trisLeft.begin());
	trisLeft.erase(trisLeft.begin());
	while (true)
	{
		curTri = trisToVisit[trisToVisit.size() - 1];
		trisToVisit.pop_back();
		trisInPath.push_back(curTri);
		if ( mMshContainer.getTriFromEdge({ curTri[0], curTri[1] }).size() == 1 ||
			 mMshContainer.getTriFromEdge({ curTri[1], curTri[2] }).size() == 1 ||
			 mMshContainer.getTriFromEdge({ curTri[0], curTri[2] }).size() == 1)
		{
			isOutside = true;
		}
		auto edgeTris = mMshContainer.getTrisNotAcrossEdge(curTri);
		for (auto& tri : edgeTris)
		{
			if (trisLeft.count(tri) != 0)
			{
				trisLeft.erase(tri);
				trisToVisit.push_back(tri);
			}
		}
		if (trisToVisit.empty())
		{
			if (isOutside)
			{
				for (auto& tri : trisInPath)
				{
					triToDelete.push_back(tri);
				} 
				isOutside = false;
			}
			trisInPath.clear();
			if (trisLeft.empty()) 
			{
				for (auto& tri : triToDelete)
				{
					mMshContainer.removeTriangle(tri);
				}
				return; 
			}
			trisToVisit.push_back(*trisLeft.begin());
			trisLeft.erase(trisLeft.begin());
		}
	}
}


