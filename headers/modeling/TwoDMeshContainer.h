#pragma once
#include "unordered_map"
#include "unordered_set"
#include "array"
#include "vector"

template<typename T, size_t N>
struct OrderedArrayHash {
    std::size_t operator()(const std::array<T, N>& arr) const noexcept {
        std::size_t h = 0;
        for (size_t i = 0; i < N; i++) {
            h ^= int64_t(arr[i]) * 9705493367 * i;
        }
        return h;
    }
};
template<typename T, size_t N>
struct OrderedArrayEqual {
    bool operator()  ( const std::array<T, N>& a,
                       const std::array<T, N>& b) const noexcept 
    {
        return a == b;
    }
};

class TwoDMeshContainer
{
    void addTriangle(size_t inPtIdx1, size_t inIdx2, size_t inIdx3);
    void addPoint(double inX, double inY);
private:
	std::vector<std::array<double, 2>>                                                                                             mPoints;
    std::vector<size_t>                                                                                                            mEdges;
    std::unordered_map<size_t, std::array<size_t, 3>>                                                                              mPointToTri;
    std::unordered_map<std::array<size_t, 2>, std::array<size_t, 3>, OrderedArrayHash<size_t, 2>, OrderedArrayEqual<size_t, 2>>    mEdgeToTri;
    std::unordered_map<std::array<double, 2>, size_t, OrderedArrayHash<double, 2>, OrderedArrayEqual<double, 2>>                   mPointToIdx;
    std::unordered_set<std::array<double, 3>, OrderedArrayHash<size_t, 3>, OrderedArrayEqual<size_t, 3>>                           mTriangles;
    std::unordered_map<std::array<double, 2>, size_t, OrderedArrayEqual<double, 2>, OrderedArrayEqual<double, 2>>                  mPoint2Idx;

};

