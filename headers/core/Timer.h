#pragma once
#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem> 

class Timer
{
public:
    static void startTime();
    static std::int64_t time(std::string title = "");
    static void stopTime(const std::string& filename = "timelog.txt");

private:
    using clock_t = std::chrono::steady_clock;
    using point_t = clock_t::time_point;

    static std::string uniqueName(const std::string& path);

    static point_t                   mStart;
    static point_t                   mLast;
    static std::vector<std::int64_t> mIntervals;
    static std::vector<std::string>  mTitleIntervals;
    static bool                      mRunning;
};
