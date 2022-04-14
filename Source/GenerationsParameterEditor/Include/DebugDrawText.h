#pragma once

class DebugDrawText
{
public:
    struct Location
    {
        size_t x;
        size_t y;
    };

    struct Color
    {
        float r;
        float g;
        float b;
        float a;
    };

protected:
    virtual ~DebugDrawText() = default;
    virtual void drawImpl(const char* text, const Location& location, float size, const Color& color) = 0;
    virtual void logImpl(const char* text, float time, size_t priority, const Color& color) = 0;

private:
    static DebugDrawText* getInstanceImpl();

public:
    static DebugDrawText* getInstance()
    {
        static DebugDrawText* instance;
        return instance != nullptr ? instance : instance = getInstanceImpl();
    }

    static void draw(const char* text, const Location& location, const float size, const Color& color)
    {
        getInstance()->drawImpl(text, location, size, color);
    }

    static void draw(const char* text, const Location& location, const float size)
    {
        getInstance()->drawImpl(text, location, size, {1, 1, 1, 1});
    }

    static void draw(const char* text, const Location& location)
    {
        getInstance()->drawImpl(text, location, 1, {1, 1, 1, 1});
    }

    static void log(const char* text, const float time, const size_t priority, const Color& color)
    {
        getInstance()->logImpl(text, time, priority, color);
    }

    static void log(const char* text, const float time, const size_t priority)
    {
        getInstance()->logImpl(text, time, priority, {1, 1, 1, 1});
    }

    static void log(const char* text, const float time)
    {
        getInstance()->logImpl(text, time, (size_t)-1, {1, 1, 1, 1});
    }

    static void log(const char* text)
    {
        getInstance()->logImpl(text, 0, (size_t)-1, {1, 1, 1, 1});
    }
};

class DebugDrawTextNullImpl : public DebugDrawText
{
protected:
    void drawImpl(const char* text, const Location& location, float size, const Color& color) override {}
    void logImpl(const char* text, float time, size_t priority, const Color& color) override {}
};

#ifndef DEBUG_DRAW_TEXT_DLL_EXPORT

inline DebugDrawText* DebugDrawText::getInstanceImpl()
{
    const HMODULE module = GetModuleHandle(TEXT("GenerationsParameterEditor.dll"));

    FARPROC procAddress;
    if (module == nullptr || (procAddress = GetProcAddress(module, "DebugDrawTextGetInstance")) == nullptr)
        return new DebugDrawTextNullImpl();

    return ((DebugDrawText*(*)())procAddress)();
}

// Use this function only for directly passing formatted strings to draw/log functions
inline char* format(const char* fmt, ...)
{
    thread_local static char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    return buffer;
}

#endif