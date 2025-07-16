#pragma once
#include "Mesh.h"
#include "glm/glm.hpp"
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "../core/Definitions.h"
#include "TwoDMeshContainer.h"
#include <list>
#include <functional>

class Delaunay
{
public:
    Mesh*         Create2DUnconstrainedNew          ( std::string          inPath);
    Mesh*         Create2DConstrainedNew            ( std::string          inPath, 
                                                      double               inEdgeScls, 
                                                      double               inGridScale);
private:

    struct IntersectionDataNew
    {
        enum class TypeIntersection
        {
            point,
            edge,
            edgeEdgePrll,
            noIntersection
        };
        TypeIntersection      type;
        size_t                point;
        MshCont::EdgeNds      edge;
    };
    using MshCont       = TwoDMeshContainer;
    using IntersectList = std::list<IntersectionDataNew>;
private:
    void          Create2DUnconstrainedNew          ( );
    void          CreateFirstTriNew                 ( );
    void          DeleteExtraPointTriNew            ( );
    Mesh*         CreateModelNew                    ( );
    void          ForceConstraintsNew               ( );
    IntersectList GetIntersectionsNew               ( size_t               inBeginIdx, 
                                                      size_t               inEndIdx);
    void          HandleIntersectionsNew            ( IntersectList        inData);
    void          AddExtraPointsSimpleNew           ( double               inDist, 
                                                      double               inFillDist);
    void          AddGeneralNodesNew                ( double               inFillDist);
    void          AddEdgeNodesNew                   ( double               inDist, 
                                                      MyVec2               inPt1, 
                                                      MyVec2               inPt2);
    void          DeleteOutsideConsTriNew           ( );
    void          VerifyDelaunayNew                 ( );
private:
    TwoDMeshContainer      mMshContainer;

    float mMaxX, mMaxY, mMinX, mMinY;
};

