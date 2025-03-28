#include "Configuration.h"

DisplayMode Configuration::displayMode = DisplayMode::BorderlessFullscreen;
bool Configuration::allowResizeInWindowed = false;
bool Configuration::lowLatencyMode = false;
bool Configuration::disableShaderCompilerNotification = false;
bool Configuration::compileShadersBeforeStarting = false;

bool Configuration::load(const std::string& filePath)
{
    const INIReader reader(filePath);
    if (reader.ParseError() != 0)
        return false;

    displayMode = (DisplayMode)reader.GetInteger("Mod", "DisplayMode", (uint32_t)DisplayMode::BorderlessFullscreen);
    allowResizeInWindowed = reader.GetBoolean("Mod", "AllowResizeInWindowed", false);
    lowLatencyMode = reader.GetBoolean("Mod", "LowLatencyMode", false);
    disableShaderCompilerNotification = reader.GetBoolean("Mod", "DisableShaderCompilerNotification", false);
    compileShadersBeforeStarting = reader.GetBoolean("Mod", "CompileShadersBeforeStarting", false);

    return true;
}
