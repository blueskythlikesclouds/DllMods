#pragma once

class VertexShader;

class ShaderData
{
    friend class ShaderCache;

    void* handle;
    size_t length;
    XXH64_hash_t hash;

    ShaderData(void* handle, size_t length, XXH64_hash_t hash);

public:
    bool isCached() const;

    void* getBytes() const;
    size_t getLength() const;
    XXH64_hash_t getHash() const;
};

class ShaderCache
{
    static std::vector<std::unique_ptr<uint8_t[]>> chunks;
    static std::unordered_map<XXH64_hash_t, ShaderData> shaders;

    static std::unordered_map<XXH64_hash_t, ComPtr<VertexShader>> vertexShaders;
    static std::unordered_map<XXH64_hash_t, ComPtr<ID3D11PixelShader>> pixelShaders;

    static std::string directoryPath;

    static void loadSingle(const std::string& filePath);

public:
    static void init(const std::string& dir);
    static void load();
    static void save();
    static void clean();

    static ShaderData get(void* function, size_t functionSize);

    static void getVertexShader(ID3D11Device* device, DWORD* pFunction, DWORD FunctionSize, VertexShader** ppShader);
    static void getPixelShader(ID3D11Device* device, DWORD* pFunction, DWORD FunctionSize, ID3D11PixelShader** ppShader);
};
