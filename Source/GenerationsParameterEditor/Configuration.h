#pragma once

class Configuration
{
    static bool visible;

public:
    static constexpr const char* FILE_NAME =
        "GenerationsParameterEditor.ini";

    static bool showDescriptions;
    static bool wrapNames;
    static float bgAlpha;
    static float fontSize;
    static float valueStepAmount;

    static bool getVisible();
    static void setVisible(bool value);

    static void load();
    static void save();

    static void update();
};