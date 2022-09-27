#pragma once

class MaterialEditor
{
public:
    static bool visible;

    struct Impl;
    static std::unique_ptr<Impl> impl;

    static void init();
    static void update();
};
