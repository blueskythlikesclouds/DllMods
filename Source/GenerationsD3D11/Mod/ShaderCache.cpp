#include "ShaderCache.h"

#include "ShaderTranslator.h"

ShaderData::ShaderData(void* handle, size_t length) : handle(handle), length(length)
{
}

ShaderData::ShaderData(ShaderData&& other) : handle(other.handle), length(other.length)
{
    other.handle = nullptr;
    other.length = 0;
}

ShaderData::~ShaderData()
{
    if (!handle)
        return;

    if (length & (1 << 31))
        ShaderTranslatorService::free(handle);

    handle = nullptr;
    length = 0;
}

void* ShaderData::getBytes() const
{
    return length & (1 << 31) ? ShaderTranslatorService::getBytes(handle) : handle;
}

size_t ShaderData::getLength() const
{
    return length & ~(1 << 31);
}

ShaderData ShaderCache::get(void* function, const size_t functionSize)
{
    if (*(int*)function == MAKEFOURCC('D', 'X', 'B', 'C'))
        return { function, functionSize };

    int translatedSize;
    void* handle = ShaderTranslatorService::translate(function, (int)functionSize, translatedSize);

    return { handle ? handle : function, handle ? (size_t)translatedSize | (1 << 31) : functionSize };
}
