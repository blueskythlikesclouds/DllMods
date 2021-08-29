#pragma once

class FxaaRenderer
{
    static bool enabled;

public:
    static const std::array<const char*, 7> SHADER_NAMES;

    static void applyPatches();
};
