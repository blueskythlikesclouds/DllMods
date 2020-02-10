#pragma once

#include <list>
#include <utility>

class Status
{
    struct Label
    {
        std::string label;
        float time;

        Label(std::string label, const float time) : label(std::move(label)), time(time)
        {
        }
    };

    static std::list<Label> labels;

public:
    static bool visible;
    static float labelDisplayTime;

    static void addLabel(const std::string& label);

    static void update();
};