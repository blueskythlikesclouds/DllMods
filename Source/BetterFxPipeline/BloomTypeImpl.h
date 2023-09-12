#pragma once

struct BloomTypeImpl
{
    // exposed for deprecated code for the time being
    static inline uint32_t s_bloomType;

    static void init();
};