#pragma once
#include "unordered_map"
#include "unordered_set"
#include "array"
#include "vector"
#include "string"
#include "../core/Definitions.h"


class TwoDMeshContainer
{
public:
    using TriContainer = std::unordered_set<std::array<size_t, 3>, OrderedArrayHash<size_t, 3>, OrderedArrayEqual<size_t, 3>>;
    using TriVec       = std::vector<std::array<size_t, 3>>;
    using PointVec     = std::vector<MyVec2>;
    using SizeVec      = std::vector<size_t>;
    using SizeHash     = std::unordered_set<size_t>;
    using Tri          = std::array<size_t, 3>;
public:
    void        addTriangle                 ( std::array<size_t, 3>    inArray);
    void        removeTriangle              ( std::array<size_t, 3>    inArray);
    void        readFile                    ( std::string              inLocation);
    size_t      addPoint                    ( double                   inX, 
                                              double                   inY);
    size_t      addPoint                    ( MyVec2&                  inPt);
    void        InitializeDelaunay          ( std::array<size_t, 3>    inArray);
    TriVec      GetNeighborTri              ( std::array<size_t, 3>    inArray);
    void        RemoveAndReplaceTri         ( TriVec                   toRemove, 
                                              TriVec                   toReplace,
                                              size_t                   toNotInclude);
    const TriContainer&   GetRemainingTri   ( ) { return mTrianglesWithPoints; }
    std::array<double,4>  GetMaxMin         ( ) { return {mMinX, mMinY, mMaxX, mMaxY}; }
    size_t                getNumPoints      ( ) { return mPoints.size(); }
    bool                  hasFloatPoints    ( ) { return !mTrianglesWithPoints.empty(); }
    const PointVec&       getPoints         ( ) { return mPoints; }
    const SizeHash&       getPointsInTri    ( Tri                       inTri) { return mTriToContainedPoint[inTri];}
private:
    PointVec                                                                                                                         mPoints;
    std::vector<size_t>                                                                                                              mEdges;
    std::unordered_map<size_t, std::unordered_set<const std::array<size_t, 3>*>>                                                     mPointToTri;
    std::unordered_map<std::array<size_t, 2>, std::unordered_set<const std::array<size_t, 3>*>, OrderedArrayHash<size_t, 2>, OrderedArrayEqual<size_t, 2>>      mEdgeToTri;
    std::unordered_map<MyVec2, size_t, Vec2Hash, Vec2Equal>                                                                          mPointToIdx;
    TriContainer                                                                                                                     mTriangles;
    TriContainer                                                                                                                     mTrianglesWithPoints;
    std::unordered_map<std::array<size_t, 3>, std::unordered_set<size_t>, OrderedArrayHash<size_t, 3>, OrderedArrayEqual<size_t, 3>> mTriToContainedPoint;
    double mMaxX, mMaxY, mMinX, mMinY;
};

