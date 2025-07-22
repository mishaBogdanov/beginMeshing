#pragma once
#include<array>
#include <algorithm>
#include "../modeling/MyMath.h"
#include <functional>
#define USE_EDGES 1
#define SAFE_MODE 0

template<typename T, size_t N>
struct OrderedArrayHash 
{
    std::size_t operator()(const std::array<T, N>& arr) const noexcept 
    {
        std::size_t h = 0;
        for (size_t i = 0; i < N; i++) 
        {
            h ^= int64_t(arr[i]) * 9705493367 * i;
        }
        return h;
    }
};
template<typename T, size_t N>
struct OrderedArrayEqual 
{
    bool operator()  (const std::array<T, N>& a,
        const std::array<T, N>& b) const noexcept
    {
        return a == b;
    }
};
template<typename T, size_t N>
struct UnorderedArrayHash 
{
    std::size_t operator()(const std::array<T, N>& arr) const noexcept 
    {
        std::size_t h = 0;
        for (auto x : arr) 
        {
            h ^= int64_t(x) * 9705493367;
        }
        return h;
    }
};
template<typename T, size_t N>
struct UnorderedArrayEqual 
{
    bool operator()(const std::array<T, N>& a,
        const std::array<T, N>& b) const noexcept 
    {
        auto sorted_a = a;
        auto sorted_b = b;
        std::sort(sorted_a.begin(), sorted_a.end());
        std::sort(sorted_b.begin(), sorted_b.end());
        return sorted_a == sorted_b;
    }
};
struct Vec2Hash
{
    std::size_t operator()(const MyVec2& inVec) const noexcept
    {
        return inVec.x * 9705493367 + inVec.y * 7919;
    }
};
struct Vec2Equal
{
    bool operator()(const MyVec2& a, const MyVec2& b) const noexcept
    {
        return a.x == b.x && a.y == b.y;
    }
};
