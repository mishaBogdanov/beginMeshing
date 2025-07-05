#include "../../headers/modeling/TwoDMeshContainer.h"
#include <iostream>
#include <fstream>

void
TwoDMeshContainer::addTriangle            ( std::array<size_t, 3>           inArray)
{
#if SAFE_MODE == 1
	std::array<size_t, 3> sortedArr = inArray;
	std::sort(sortedArr.begin(), sortedArr.end());
	if (sortedArr != inArray) { throw std::logic_error("passed unsorted array"); }
#endif
	auto itr = mTriangles.emplace(inArray);
	mPointToTri[inArray[0]].insert(&*itr.first);
	mPointToTri[inArray[1]].insert(&*itr.first);
	mPointToTri[inArray[2]].insert(&*itr.first);
#if USE_EDGES == 1
	mEdgeToTri[{inArray[0], inArray[1]}].insert(&*itr.first);
	mEdgeToTri[{inArray[1], inArray[2]}].insert(&*itr.first);
	mEdgeToTri[{inArray[0], inArray[2]}].insert(&*itr.first);
#endif
}
void
TwoDMeshContainer::populateFromGlm        ( std::vector<glm::vec2>&         inPts,
                                            std::vector<size_t>&            inEdges)
{
	for (auto& pt : inPts)
	{
		addPoint(pt.x, pt.y);
	}
	mEdges = inEdges;
}
void
TwoDMeshContainer::removeTriangle         ( std::array<size_t, 3>           inArray)  // NOTE: does not modify mTriToContainedPoint
{
#if SAFE_MODE == 1
	std::array<size_t, 3> sortedArr = inArray;
	std::sort(sortedArr.begin(), sortedArr.end());
	if (sortedArr != inArray) { throw std::logic_error("passed unsorted array"); }
#endif
	auto itr = mTriangles.find(inArray);
	if(itr == mTriangles.end())
	{
#if SAFE_MODE == 1
		throw std::logic_error("triangle trying to be deleted doesn't exist");
#endif
		return;
	}
	mPointToTri[inArray[0]].erase(&*itr);
	mPointToTri[inArray[1]].erase(&*itr);
	mPointToTri[inArray[2]].erase(&*itr);
#if USE_EDGES == 1
	mEdgeToTri[{inArray[0], inArray[1]}].erase(&*itr);
	mEdgeToTri[{inArray[1], inArray[2]}].erase(&*itr);
	mEdgeToTri[{inArray[0], inArray[2]}].erase(&*itr);
#endif
	mTriangles.erase(itr);
}
void
TwoDMeshContainer::readFile               ( std::string                     inLocation)
{
	std::ifstream rdStream(inLocation);
	std::string word;
	float x, y;
	size_t idx;
	while (true)
	{
		if (!(rdStream >> word)) { break; };
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
		addPoint(x, y);
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
size_t
TwoDMeshContainer::addPoint               ( double                          x,
					                        double                          y)
{
	auto itr = mPointToIdx.find({x,y});
	if (itr == mPointToIdx.end())
	{
		mPointToIdx[{x,y}] = mPoints.size();
		mPoints.push_back({x,y});
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
size_t
TwoDMeshContainer::addPoint               ( MyVec2&                         inVec)
{
	auto itr = mPointToIdx.find(inVec);
	if (itr == mPointToIdx.end())
	{
		mPointToIdx[inVec] = mPoints.size();
		mPoints.push_back(inVec);
		if (inVec.x > mMaxX)
		{
			mMaxX = inVec.x;
		}
		else if (inVec.x < mMinX)
		{
			mMinX = inVec.x;
		}
		if (inVec.y > mMaxY)
		{
			mMaxY = inVec.y;
		}
		else if (inVec.y < mMinY)
		{
			mMinY = inVec.y;
		}
		return mPoints.size() - 1;
	}
	return itr->second;
}
void
TwoDMeshContainer::initializeDelaunay     ( std::array<size_t, 3>           inArray)
{
#if SAFE_MODE == 1
	std::array<size_t, 3> sortedArr = inArray;
	std::sort(sortedArr.begin(), sortedArr.end());
	if (sortedArr != inArray) { throw std::logic_error("passed unsorted array"); }
#endif

	addTriangle(inArray);
	mTrianglesWithPoints.insert(inArray);
	auto itr = mTriToContainedPoint.emplace( inArray, std::unordered_set<size_t>{} );
	for (size_t i = 0; i < mPoints.size() - 3; i++)
	{
		itr.first->second.insert(i);
	}
}
std::vector<std::array<size_t, 3>>
TwoDMeshContainer::getNeighborTri         ( std::array<size_t, 3>           inArray)
{
#if SAFE_MODE == 1
	std::array<size_t, 3> sortedArr = inArray;
	std::sort(sortedArr.begin(), sortedArr.end());
	if (sortedArr != inArray) { throw std::logic_error("passed unsorted array"); }
#endif
	std::vector<std::array<size_t, 3>> outVal;
	std::unordered_set<std::array<size_t, 3>, OrderedArrayHash<size_t, 3>, OrderedArrayEqual<size_t, 3>> triCollected;
	for (int i = 0; i < 3; i++)
	{
		auto& foundTris = mPointToTri[inArray[i]];
		for (auto& triPtr : foundTris)
		{
			triCollected.insert(*triPtr);
		}
	}
	triCollected.erase(inArray);
	for (auto& tri : triCollected) { outVal.push_back(tri); }
	return std::move(outVal);
}
void
TwoDMeshContainer::removeAndReplaceTri    ( TriVec                          toRemove, 
	                                        TriVec                          toInsert,
	                                        size_t                          toNotInclude)
{
#if SAFE_MODE == 1
	for (auto& tri : toRemove)
	{
		std::array<size_t, 3> sortedArr = tri;
		std::sort(sortedArr.begin(), sortedArr.end());
		if (sortedArr != tri) { throw std::logic_error("passed unsorted array"); }
	}
	for (auto& tri : toInsert)
	{
		std::array<size_t, 3> sortedArr = tri;
		std::sort(sortedArr.begin(), sortedArr.end());
		if (sortedArr != tri) { throw std::logic_error("passed unsorted array"); }
	}
#endif
	for (auto& triRmv : toRemove)
	{
		for (auto& ptIdx : mTriToContainedPoint[triRmv])
		{
			for (auto& tri : toInsert)
			{
				if (ptIdx == toNotInclude) { continue; }
				if (IsPointInTriangle(mPoints[ptIdx], mPoints[tri[0]], mPoints[tri[1]], mPoints[tri[2]]))
				{
					mTriToContainedPoint[tri].insert(ptIdx);
					mTrianglesWithPoints.insert(tri);
					continue;
				}
			}
		}
		mTriToContainedPoint.erase(triRmv);
		mTrianglesWithPoints.erase(triRmv);
	}
	for (auto& tri : toRemove) { removeTriangle(std::move(tri)); }
	for (auto& tri : toInsert) { addTriangle(std::move(tri)); }
}
void
TwoDMeshContainer::removeLastPoint        ( )
{
	mPointToIdx.erase(mPoints[mPoints.size() - 1]);
	mPointToTri.erase(mPoints.size() - 1);
	mPoints.pop_back();
}




