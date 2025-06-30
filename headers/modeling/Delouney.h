#pragma once
#include "Mesh.h"
#include "glm/glm.hpp"
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
template<typename T, size_t N>
struct UnorderedArrayHash {
    std::size_t operator()(const std::array<T, N>& arr) const noexcept {
        std::size_t h = 0;
        for (auto x : arr) {
            h ^= int64_t(x) * 9705493367;
        }
        return h;
    }
};
template<typename T, size_t N>
struct UnorderedArrayEqual {
    bool operator()(const std::array<T, N>& a,
        const std::array<T, N>& b) const noexcept {
        auto sorted_a = a;
        auto sorted_b = b;
        std::sort(sorted_a.begin(), sorted_a.end());
        std::sort(sorted_b.begin(), sorted_b.end());
        return sorted_a == sorted_b;
    }
};
template<typename T, size_t N>
struct OrderedArrayEqual {
    bool operator()(const std::array<T, N>& a,
        const std::array<T, N>& b) const noexcept {
        return a == b;
    }
};

class Delouney
{
public:
	Mesh* Create2DUnconstrained     (std::string inPath);
    Mesh* Create2DConstrained       (std::string inPath);
    Mesh* Create2DConsWEdge         (std::string inPath);
    Mesh* Create2DConsSmoothedWEdge (std::string inPath, float pointDist = 0.3, float sclForFillPts = 2.5, int jmpForFillPts = 1);
private:
    struct IntersectionData
    {
        enum TypeIntersection
        {
            point,
            edge,
            edgeEdgePrll,
            noIntersection
        };
        std::array<size_t, 3> tri;

        TypeIntersection type1;
        std::pair<int64_t, int64_t> edge1;
        glm::vec2 point1;

        TypeIntersection type2;
        std::pair<int64_t, int64_t> edge2;
        glm::vec2 point2;
    };
    struct SortedData
    {
        size_t location;
        std::vector<std::array<size_t, 3>> tris;
        std::vector<std::pair<size_t, size_t>> edges;
    };
private:
    void AddExtraPoints(float inDist, float sclForFillPts, int jmpForFillPts);
    void CreateSecondaryLinePoints(float inDist, const glm::vec2& inP1, const glm::vec2& inP2);
    void CreateEdgeLinePoints(float inDist, const glm::vec2& inP1, const glm::vec2& inP2);
    void DeleteOutsideTriangles();
    bool IsTriInsideConstraints(const std::array<size_t, 3>& inTri);
    void Create2DUnconstrained();
    void ForceConstraints();
    void PopulatePoints(std::string inPath);
    void CreateFirstTri();
    std::vector<std::array<size_t, 3>> GetTriContainingPoint(size_t inPointIdx);
    void DeleteAndCreateTri(size_t inPtIdx, std::vector<std::array<size_t, 3>> inTriToDelete);
    void DeleteExtraPointTri();
    Mesh* CreateModel();
    std::vector<IntersectionData> GetTriContainingEdge(glm::vec2 p1, glm::vec2 p2);
    std::vector<SortedData>       GetSortedData(std::vector<IntersectionData> inData, glm::vec2 startingPoint);
    void                          CutTriangles(std::vector<SortedData>& inData);
    IntersectionData IsLineIntersectingTri(glm::vec2 p1, glm::vec2 p2, const std::array<size_t, 3>& inTri);
    size_t AddPoint(float x, float y);
private:
	std::vector<glm::vec2> mPoints;
    std::vector<size_t>    mEdges;
	std::unordered_set<std::array<size_t, 3>, UnorderedArrayHash<size_t, 3>, UnorderedArrayEqual<size_t, 3>> mTris;
    std::unordered_map<std::array<float, 2>, size_t, UnorderedArrayHash<float, 2>, OrderedArrayEqual<float, 2>> mPoint2Idx;

    float mMaxX, mMaxY, mMinX, mMinY;
};

