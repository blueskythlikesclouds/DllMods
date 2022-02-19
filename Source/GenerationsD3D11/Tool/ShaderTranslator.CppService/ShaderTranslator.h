#pragma once

class ShaderTranslatorService
{
public:
    static void init();
    static void* translate(void* function, int functionSize, int& translatedSize);
    static void* getBytes(void* handle);
    static void free(void* handle);
};