#include "ShaderCache.h"
#include "PixelShader.h"
#include "ShaderTranslator.h"
#include "VertexShader.h"

struct ChunkHeader
{
    size_t version;
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
    XXH32_hash_t hash;
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

ShaderByteCode::ShaderByteCode() : handle(nullptr), length(0), hash(0), type(Type::Reference)
{
}

ShaderByteCode::~ShaderByteCode()
{
    if (type == Type::Managed)
        ShaderTranslator::free(handle);

    else if (type == Type::Unique)
        operator delete(handle);
}

void* ShaderByteCode::get() const
{
    return type == Type::Managed ? ShaderTranslator::get(handle) : handle;
}

CriticalSection ShaderCache::criticalSection;

tbb::task_group ShaderCache::group;

std::vector<std::unique_ptr<uint8_t[]>> ShaderCache::chunks;

std::unordered_map<int, ShaderByteCode> ShaderCache::shaderByteCodes;

std::unordered_map<int, ComPtr<VertexShader>> ShaderCache::vertexShaders;
std::unordered_map<int, ComPtr<PixelShader>> ShaderCache::pixelShaders;

std::atomic<size_t> ShaderCache::compilingShaderCount;

std::string ShaderCache::directoryPath;

HOOK(void, __fastcall, GameplayFlowStageEnter, 0xD05530, void* This)
{
    ShaderCache::clean();
    originalGameplayFlowStageEnter(This);
}

void ShaderCache::init()
{
    INSTALL_HOOK(GameplayFlowStageEnter);

    ShaderTranslator::init(directoryPath);
    ShaderTranslator::registerOnProcessExit(save);
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
        if (chunk->version != ShaderTranslator::version() || !chunk->compressedLength || !chunk->uncompressedLength)
        {
            chunk = chunk->next();
            continue;
        }

        std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(chunk->uncompressedLength);

        if (chunk->uncompressedLength == chunk->compressedLength)
            memcpy(data.get(), chunk->getData(), chunk->uncompressedLength);

        else
            LZ4_decompress_safe((const char*)chunk->getData(), (char*)data.get(), chunk->compressedLength, chunk->uncompressedLength);

        const ShaderHeader* shader = (const ShaderHeader*)data.get();
        const ShaderHeader* shaderEnd = (const ShaderHeader*)(data.get() + chunk->uncompressedLength);

        while (shader < shaderEnd)
        {
            ShaderByteCode byteCode;
            byteCode.handle = shader->getData();
            byteCode.length = shader->length;
            byteCode.hash = shader->hash;
            byteCode.type = ShaderByteCode::Type::Reference;

            shaderByteCodes.insert(std::make_pair(shader->hash, std::move(byteCode)));

            shader = shader->next();
        }

        chunks.push_back(std::move(data));
        chunk = chunk->next();
    }
}

void ShaderCache::load()
{
    loadSingle(directoryPath + "/pregenerated_shader_cache.bin");
    loadSingle(directoryPath + "/runtime_generated_shader_cache.bin");
}

namespace
{
    template<typename T>
    void cleanSwap(T& value)
    {
        std::swap(value, T());
    }
}

void ShaderCache::save()
{
    std::lock_guard lock(criticalSection);

    if (shaderByteCodes.empty())
        return;

    ChunkHeader chunk{};

    for (auto& pair : shaderByteCodes)
    {
        if (pair.second.type == ShaderByteCode::Type::Managed)
            chunk.uncompressedLength += ShaderHeader::computeTotalLength(pair.second.length);
    }

    if (!chunk.uncompressedLength)
        return;

    const size_t compressBound = LZ4_compressBound((int)chunk.uncompressedLength);
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(chunk.uncompressedLength + compressBound);

    ShaderHeader* shader = (ShaderHeader*)data.get();
    for (auto& pair : shaderByteCodes)
    {
        if (pair.second.type != ShaderByteCode::Type::Managed)
            continue;

        shader->hash = pair.first;
        shader->length = pair.second.length;
        memcpy(shader->getData(), pair.second.get(), pair.second.length);

        shader = shader->next();
    }

    cleanSwap(shaderByteCodes);

    FILE* file = fopen((directoryPath + "/runtime_generated_shader_cache.bin").c_str(), "ab");
    if (!file)
    {
        MessageBox(nullptr, TEXT("Unable to open \"runtime_generated_shader_cache.bin\" in mod directory."), TEXT("GenerationsD3D11"), MB_ICONERROR);
        return;
    }

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
    cleanSwap(chunks);

    {
        std::lock_guard lock(criticalSection);

        for (auto it = shaderByteCodes.begin(); it != shaderByteCodes.end();)
        {
            if (it->second.type != ShaderByteCode::Type::Managed)
                it = shaderByteCodes.erase(it);
            else
                ++it;
        }
    }

    cleanSwap(vertexShaders);
    cleanSwap(pixelShaders);
}

template<typename T>
ComPtr<T> ShaderCache::get(ID3D11Device* device, void* function, const size_t functionSize, std::unordered_map<int, ComPtr<T>>& shaders)
{
    const int hash = XXH32(function, functionSize, 0);

    criticalSection.lock();
    auto& shader = shaders[hash];
    criticalSection.unlock();

    if (!shader)
    {
        if (*(int*)function == MAKEFOURCC('D', 'X', 'B', 'C'))
            shader.Attach(new T(device, function, functionSize));

        else
        {
            criticalSection.lock();
            auto& byteCode = shaderByteCodes[hash];
            criticalSection.unlock();

            if (byteCode.handle)
                shader.Attach(new T(device, byteCode.handle, byteCode.length));

            else
            {
                void* alsoFunction = operator new(functionSize);
                memcpy(alsoFunction, function, functionSize);

                shader.Attach(new T([alsoDevice = ComPtr<ID3D11Device>(device), alsoFunction, functionSize, hash](T* alsoShader)
                {
                    ++compilingShaderCount;

                    int translatedSize;
                    void* handle = ShaderTranslator::translate(alsoFunction, functionSize, translatedSize);
                    operator delete(alsoFunction);

                    criticalSection.lock();
                    auto & code = shaderByteCodes[hash];
                    criticalSection.unlock();

                    code.handle = handle;
                    code.length = translatedSize;
                    code.hash = hash;
                    code.type = ShaderByteCode::Type::Managed;

                    alsoShader->update(alsoDevice.Get(), code.get(), code.length);

                    --compilingShaderCount;
                }));
            }
        }
    }

    return shader;
}

void ShaderCache::getVertexShader(ID3D11Device* device, void* function, size_t functionSize, VertexShader** ppShader)
{
    get(device, function, functionSize, vertexShaders).CopyTo(ppShader);
}

void ShaderCache::getPixelShader(ID3D11Device* device, void* function, size_t functionSize, PixelShader** ppShader)
{
    get(device, function, functionSize, pixelShaders).CopyTo(ppShader);
}