#pragma once

class ShaderData
{
    friend class ShaderCache;

    void* handle;
    size_t length;

    ShaderData(void* handle, size_t length);

public:
    bool isCached() const;

    void* getBytes() const;
    size_t getLength() const;
};

class ShaderCache
{
    static std::list<std::unique_ptr<uint8_t[]>> chunks;
    static std::unordered_map<XXH64_hash_t, ShaderData> shaders;

    static std::string directoryPath;

    static void loadSingle(const std::string& filePath);

public:
    static void init(const std::string& dir);
    static void load();
    static void save();
    static void clean();

    static ShaderData get(void* function, size_t functionSize);
};
