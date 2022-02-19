#include "ShaderCache.h"

#include "ShaderTranslator.h"

#define SHADER_IS_UNCACHED    (1 << 31)
#define SHADER_LENGTH_MASK   ~(1 << 31)

struct ChunkHeader
{
    size_t compressedLength;
    size_t uncompressedLength;

    static size_t computePadding(const size_t compressedLength)
    {
        return ((compressedLength + alignof(ChunkHeader) - 1) & ~(alignof(ChunkHeader) - 1)) - compressedLength;
    }

    static size_t computeTotalLength(const size_t compressedLength)
    {
        return sizeof(ChunkHeader) + compressedLength + computePadding(compressedLength);
    }

    uint8_t* getData() const
    {
        return (uint8_t*)this + sizeof(ChunkHeader);
    }

    ChunkHeader* next() const
    {
        return (ChunkHeader*)((char*)this + computeTotalLength(compressedLength));
    }
};

struct ShaderHeader
{
    XXH64_hash_t hash;
    size_t length;

    static size_t computePadding(const size_t length)
    {
        return ((length + alignof(ShaderHeader) - 1) & ~(alignof(ShaderHeader) - 1)) - length;
    }

    static size_t computeTotalLength(size_t length)
    {
        return sizeof(ShaderHeader) + length + computePadding(length);
    }

    uint8_t* getData() const
    {
        return (uint8_t*)this + sizeof(ShaderHeader);
    }

    ShaderHeader* next() const
    {
        return (ShaderHeader*)((char*)this + computeTotalLength(length));
    }
};

ShaderData::ShaderData(void* handle, const size_t length) : handle(handle), length(length)
{
}

bool ShaderData::isCached() const
{
    return !(length & SHADER_IS_UNCACHED);
}

void* ShaderData::getBytes() const
{
    return length & SHADER_IS_UNCACHED ? ShaderTranslatorService::getBytes(handle) : handle;
}

size_t ShaderData::getLength() const
{
    return length & SHADER_LENGTH_MASK;
}

void ShaderCache::loadSingle(const std::string& filePath)
{
    std::unique_ptr<uint8_t[]> cache;
    size_t length;

    {
        FILE* file = fopen(filePath.c_str(), "rb");
        if (!file)
            return;

        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);

        cache = std::make_unique<uint8_t[]>(length);
        fread(cache.get(), 1, length, file);

        fclose(file);
    }

    const ChunkHeader* chunk = (const ChunkHeader*)cache.get();
    const ChunkHeader* chunkEnd = (const ChunkHeader*)(cache.get() + length);

    while (chunk < chunkEnd)
    {
        std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(chunk->uncompressedLength);

        if (chunk->uncompressedLength == chunk->compressedLength)
            memcpy(data.get(), chunk->getData(), chunk->uncompressedLength);

        else
            LZ4_decompress_safe((const char*)chunk->getData(), (char*)data.get(), chunk->compressedLength, chunk->uncompressedLength);

        const ShaderHeader* shader = (const ShaderHeader*)data.get();
        const ShaderHeader* shaderEnd = (const ShaderHeader*)(data.get() + chunk->uncompressedLength);

        while (shader < shaderEnd)
        {
            shaders.insert(std::make_pair(shader->hash, ShaderData(shader->getData(), shader->length)));
            shader = shader->next();
        }

        chunks.push_back(std::move(data));
        chunk = chunk->next();
    }
}

std::list<std::unique_ptr<uint8_t[]>> ShaderCache::chunks;
std::unordered_map<XXH64_hash_t, ShaderData> ShaderCache::shaders;

std::string ShaderCache::directoryPath;

void ShaderCache::init(const std::string& dir)
{
    ShaderTranslatorService::init();
    directoryPath = dir;
}

void ShaderCache::load()
{
    for (auto& file : std::filesystem::directory_iterator(directoryPath))
    {
        if (!file.is_regular_file())
            continue;

        std::string extension = file.path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

        if (extension == ".shadercache")
            loadSingle(file.path().string());
    }
}

void ShaderCache::save()
{
    if (shaders.empty())
        return;

    ChunkHeader chunk{};

    for (auto& pair : shaders)
    {
        if (!pair.second.isCached())
            chunk.uncompressedLength += ShaderHeader::computeTotalLength(pair.second.getLength());
    }

    if (!chunk.uncompressedLength)
        return;

    const size_t compressBound = LZ4_compressBound((int)chunk.uncompressedLength);
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(chunk.uncompressedLength + compressBound);

    ShaderHeader* shader = (ShaderHeader*)data.get();
    for (auto& pair : shaders)
    {
        if (pair.second.isCached())
            continue;

        shader->hash = pair.first;
        shader->length = pair.second.getLength();
        memcpy(shader->getData(), pair.second.getBytes(), pair.second.getLength());

        shader = shader->next();
    }

    FILE* file = fopen((directoryPath + "/user.shadercache").c_str(), "ab");

    uint8_t* src = data.get();

    chunk.compressedLength = LZ4_compress_default((const char*)data.get(), 
        (char*)(data.get() + chunk.uncompressedLength), chunk.uncompressedLength, compressBound);

    if (chunk.compressedLength == 0)
        chunk.compressedLength = chunk.uncompressedLength;

    else
        src += chunk.uncompressedLength;

    fwrite(&chunk, sizeof(chunk), 1, file);
    fwrite(src, 1, chunk.compressedLength, file);

    uint8_t pad = 0;
    for (size_t i = 0; i < ChunkHeader::computePadding(chunk.compressedLength); i++)
        fwrite(&pad, 1, 1, file);

    fclose(file);
}

void ShaderCache::clean()
{
    shaders = {};

    for (auto& pair : shaders)
    {
        if (!pair.second.isCached())
            ShaderTranslatorService::free(pair.second.handle);
    }

    shaders = {};
}

ShaderData ShaderCache::get(void* function, const size_t functionSize)
{
    if (*(int*)function == MAKEFOURCC('D', 'X', 'B', 'C'))
        return { function, functionSize };

    const XXH64_hash_t hash = XXH64(function, functionSize, 0);

    const auto pair = shaders.find(hash);
    if (pair != shaders.end())
        return pair->second;

    int translatedSize = 0;
    void* handle = ShaderTranslatorService::translate(function, (int)functionSize, translatedSize);

    if (!handle)
        return { function, functionSize };

    ShaderData data = { handle, (size_t)translatedSize | SHADER_IS_UNCACHED };
    shaders.insert(std::make_pair(hash, data));
    return data;
}
