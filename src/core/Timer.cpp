#include "../../headers/core/Timer.h"

Timer::point_t                  Timer::mStart{};
Timer::point_t                  Timer::mLast{};
std::vector<std::int64_t>       Timer::mIntervals{};
std::vector<std::string>        Timer::mTitleIntervals{};
bool                            Timer::mRunning = false;

void Timer::startTime()
{
    mStart = clock_t::now();
    mLast = mStart;
    mIntervals.clear();
    mTitleIntervals.clear();
    mRunning = true;
}

std::int64_t Timer::time(std::string inTitle)
{
    if (!mRunning) return 0;

    const point_t now = clock_t::now();
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(now - mLast).count();
    mLast = now;
    mIntervals.push_back(us);
    mTitleIntervals.push_back(inTitle);
    return us;
}

void Timer::stopTime(const std::string& filename)
{
    if (!mRunning) return;
    mRunning = false;
    if (filename == "") return;

    const std::string outName = uniqueName(filename);
    std::ofstream ofs(outName, std::ios::out);

    for (size_t i = 0; i < mIntervals.size(); i++)
    {
        ofs << mTitleIntervals[i] << ", " << mIntervals[i] << "\n";
    }
    ofs.close();
}

std::string Timer::uniqueName(const std::string& path)
{
    namespace fs = std::filesystem;

    fs::path p(path);
    fs::path dir = p.has_parent_path() ? p.parent_path() : fs::current_path();
    fs::path stem = p.stem();
    fs::path ext = p.extension();

    if (!fs::exists(dir / p.filename()))
        return (dir / p.filename()).string();

    std::size_t n = 1;
    while (true)
    {
        fs::path candidate = dir / fs::path(stem.string() + '(' + std::to_string(n) + ')' + ext.string());
        if (!fs::exists(candidate))
            return candidate.string();
        ++n;
    }
}
