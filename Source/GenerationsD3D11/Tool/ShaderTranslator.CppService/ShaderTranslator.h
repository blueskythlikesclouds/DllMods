#pragma once

class ShaderTranslatorService
{
public:
    static void init(void(*onProcessExit)());
    static void* translate(void* function, int functionSize, int& translatedSize);
    static void* getBytes(void* handle);
    static void free(void* handle);
    static int getVersion();
};