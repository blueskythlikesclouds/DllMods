#include "ScopedProfiler.h"

#ifdef ENABLE_SCOPED_PROFILER

std::map<const char*, ScopedProfiler::Result> ScopedProfiler::results;

ScopedProfiler::ScopedProfiler(const char* name) : name(name), time(std::chrono::system_clock::now())
{
}

ScopedProfiler::~ScopedProfiler()
{
    auto& result = results[name];
    result.averagedNanoseconds *= result.iterationCount;
    result.averagedNanoseconds += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time).count();
    result.averagedNanoseconds /= (double)(++result.iterationCount);
}

#include "../../GenerationsParameterEditor/Include/DebugDrawText.h"

extern "C" void __declspec(dllexport) OnFrame()
{
    for (auto& result : ScopedProfiler::results)
        DebugDrawText::log(format("%s: %f", result.first, result.second.averagedNanoseconds));
}

#endif
