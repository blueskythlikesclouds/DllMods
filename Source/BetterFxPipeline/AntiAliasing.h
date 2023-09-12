#pragma once

struct AntiAliasing
{
    static inline const char* s_shaderNames[] =
    {
        "FxFXAALite",
        "FxFXAA_0",
        "FxFXAA_1",
        "FxFXAA_2",
        "FxFXAA_3",
        "FxFXAA_4",
        "FxFXAA_5"
    };

    static void init();
};