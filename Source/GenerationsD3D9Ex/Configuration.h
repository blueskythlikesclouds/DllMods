#pragma once

enum class DisplayMode
{
    FULLSCREEN = 0,
    BORDERLESS = 1,
    BORDERLESS_FULLSCREEN = 2,
    WINDOWED = 3
};

class Configuration
{
public:
    static DisplayMode displayMode;
    static bool allowResizeInWindowed;
    static bool useConfigWorkaround;
    static bool enable10BitOutput;

    static bool load(const std::string& filePath);
};