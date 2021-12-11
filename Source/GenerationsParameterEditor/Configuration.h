#pragma once

class Configuration
{
    static bool visible;

public:
    static constexpr const char* FILE_NAME =
        "GenerationsParameterEditor.ini";

    static bool loadDebugShaders;
    static bool showDescriptions;
    static bool wrapNames;
    static float bgAlpha;
    static float fontSize;
    static float valueStepAmount;
    static float labelDisplayTime;

    static bool getVisible();
    static void setVisible(bool value);

    static void load();
    static void save();

    static void update();
};