#pragma once

struct ParameterManager
{
    // needs to be stored here since the global change is temporary
    // TODO: this is hacky, load it from MTFx's parameter class instead
    static inline bool isEnableVerticalShadow;

    static void init();

    // exposed for deprecated code for the time being
    static void preUpdate();
    static void postUpdate();
};