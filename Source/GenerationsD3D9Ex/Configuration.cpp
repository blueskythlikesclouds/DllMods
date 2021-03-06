﻿#include "Configuration.h"

DisplayMode Configuration::displayMode = DisplayMode::FULLSCREEN;
bool Configuration::allowResizeInWindowed = false;
bool Configuration::useConfigWorkaround = false;
bool Configuration::enable10BitOutput = false;

bool Configuration::load(const std::string& filePath)
{
    const INIReader reader(filePath);
    if (reader.ParseError() != 0)
        return false;

    displayMode = (DisplayMode)reader.GetInteger("Mod", "DisplayMode", (uint32_t)DisplayMode::FULLSCREEN);
    allowResizeInWindowed = reader.GetBoolean("Mod", "AllowResizeInWindowed", false);
    useConfigWorkaround = reader.GetBoolean("Mod", "UseConfigWorkaround", false);
    enable10BitOutput = reader.GetBoolean("Mod", "Enable10BitOutput", false);

    return true;
}
