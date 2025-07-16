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
    using TriHash          = std::unordered_set<std::array<size_t, 3>, OrderedArrayHash<size_t, 3>, OrderedArrayEqual<size_t, 3>>;
    using TriVec           = std::vector<std::array<size_t, 3>>;
    using EdgeHash         = std::unordered_set<std::array<size_t, 2>, OrderedArrayHash<size_t, 2>, OrderedArrayEqual<size_t, 2>>;
    using EdgeToSizeTable  = std::unordered_map<std::array<size_t, 2>, size_t, OrderedArrayHash<size_t, 2>, OrderedArrayEqual<size_t, 2>>;
    using PointVec         = std::vector<MyVec2>;
    using SizeVec          = std::vector<size_t>;
    using SizeHash         = std::unordered_set<size_t>;
    using Tri              = std::array<size_t, 3>;
    using TriPtrHash       = std::unordered_set<const Tri*>;
    using EdgeNds          = std::array<size_t, 2>;
    using EdgeHash         = std::unordered_set<EdgeNds, OrderedArrayHash<size_t, 2>, OrderedArrayEqual<size_t, 2>>;
    using PointHash        = std::unordered_set<MyVec2, Vec2Hash, Vec2Equal>;
public:
    void                  populateFromGlm      ( std::vector<glm::vec2>&  inPts,
                                                 std::vector<size_t>&     inEdges);
    void                  addTriangle          ( std::array<size_t, 3>    inArray);
    void                  removeTriangle       ( std::array<size_t, 3>    inArray);
    void                  readFile             ( std::string              inLocation);
    size_t                addPoint             ( double                   inX, 
                                                 double                   inY);
    size_t                addPoint             ( MyVec2&                  inPt);
    void                  initializeDelaunay   ( std::array<size_t, 3>    inArray);
    TriVec                getNeighborTri       ( std::array<size_t, 3>    inArray);
    void                  removeAndReplaceTri  ( TriVec                   toRemove, 
                                                 TriVec                   toReplace,
                                                 size_t                   toNotInclude);
    const TriHash&        getRemainingTri      ( ) { return mTrianglesWithPoints; }
    std::array<double,4>  getMinMax            ( ) { return {mMinX, mMinY, mMaxX, mMaxY}; }
    size_t                getNumPoints         ( ) { return mPoints.size(); }
    bool                  hasFloatPoints       ( ) { return !mTrianglesWithPoints.empty(); }
    const PointVec&       getPoints            ( ) { return mPoints; }
    const SizeHash&       getPointsInTri       ( Tri                       inTri) { return mTriToContainedPoint[inTri];}
    const TriHash&        getTriangles         ( ) { return mTriangles;}
    void                  removeLastPoint      ( );
    const TriPtrHash&     getTriFromPts        ( size_t                    inPtIdx) { return mPointToTri[inPtIdx]; }
    const TriPtrHash&     getTriFromEdge       ( size_t                    inPtIdx1,
                                                 size_t                    inPtIdx2);
    const TriPtrHash&     getTriFromEdge       ( EdgeNds                   inPtIds);
    const EdgeHash&       getEdgesConstraint   ( ) {return mEdgesHash;}
    bool                  doesTriEdgeExist     ( size_t                    inPtIdx1,
                                                 size_t                    inPtIdx2);
    void                  addEdge              ( EdgeNds                   inEdge);
    TriVec                getTrisNotAcrossEdge ( Tri                       inTri);
    

private:
    PointVec                                                                                                                         mPoints;
    EdgeHash                                                                                                                         mEdgesHash;
    std::unordered_map<size_t, TriPtrHash>                                                                                           mPointToTri;
    std::unordered_map<std::array<size_t, 2>, TriPtrHash, OrderedArrayHash<size_t, 2>, OrderedArrayEqual<size_t, 2>>                 mEdgeToTri;
    std::unordered_map<MyVec2, size_t, Vec2Hash, Vec2Equal>                                                                          mPointToIdx;
    TriHash                                                                                                                          mTriangles;
    TriHash                                                                                                                          mTrianglesWithPoints;
    std::unordered_map<std::array<size_t, 3>, std::unordered_set<size_t>, OrderedArrayHash<size_t, 3>, OrderedArrayEqual<size_t, 3>> mTriToContainedPoint;
    double mMaxX, mMaxY, mMinX, mMinY;
};

