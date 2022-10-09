#pragma once

class ShaderTranslator
{
public:
    static void init(const std::string& dir);

    static inline void(*registerOnProcessExit)(void(*)());
    static inline void*(*translate)(void*, int, int&);
    static inline void*(*get)(void*);
    static inline void(*free)(void*);
    static inline int(*version)();
};
