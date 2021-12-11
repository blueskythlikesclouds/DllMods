#pragma once

#include "DebugDrawText.h"

class DebugDrawTextImpl : public DebugDrawText
{
protected:
    enum class TextType
    {
        Free,
        Grouped
    };
    
    struct Text
    {
        TextType type;
        std::string text;
        float time;
        size_t priority;
        Location location;
        float size;
        Color color;
    };

    static std::list<Text> texts;

    ~DebugDrawTextImpl() = default;
    void drawImpl(const char* text, const Location& location, float size, const Color& color) override;
    void logImpl(const char* text, float time, size_t priority, const Color& color) override;

public:
    static bool visible;

    static void update();
};
