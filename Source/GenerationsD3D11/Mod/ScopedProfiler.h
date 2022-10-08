#pragma once

//#define ENABLE_SCOPED_PROFILER

#ifndef ENABLE_SCOPED_PROFILER
#define SCOPED_PROFILER(name) ((void)0)
#else
#define SCOPED_PROFILER(name) ScopedProfiler _scoped_profiler_(__FUNCTION__ "(): " name)

struct ScopedProfiler
{
    struct Result
    {
        double averagedNanoseconds{};
        size_t iterationCount{};
    };

    static std::map<const char*, Result> results;

    const char* name;
    std::chrono::system_clock::time_point time;

    ScopedProfiler(const char* name);
    ScopedProfiler(const ScopedProfiler&) = delete;
    ScopedProfiler(ScopedProfiler&&) = delete;
    ~ScopedProfiler();
};
#endif