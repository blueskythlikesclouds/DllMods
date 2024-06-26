﻿#pragma once

#include "CriticalSection.h"

class PixelShader;
class VertexShader;

class ShaderByteCode
{
public:
    enum class Type
    {
        Managed,
        Reference,
        Unique
    };
    
    void* handle;
    size_t length;
    int hash;
    Type type;

    ShaderByteCode();
    ~ShaderByteCode();

    void* get() const;
};

class ShaderCache
{
    friend class CompilingShadersImageRenderer;
    friend class Device;
    friend class PixelShader;
    friend class VertexShader;

    static CriticalSection criticalSection;

    static tbb::task_group group;

    static std::vector<std::unique_ptr<uint8_t[]>> chunks;

    static std::unordered_map<int, ShaderByteCode> shaderByteCodes;

    static std::unordered_map<int, ComPtr<VertexShader>> vertexShaders;
    static std::unordered_map<int, ComPtr<PixelShader>> pixelShaders;

    template<typename T>
    static ComPtr<T> get(ID3D11Device* device, void* function, size_t functionSize, std::unordered_map<int, ComPtr<T>>& shaders);

public:
    static std::atomic<size_t> compilingShaderCount;

    static std::string directoryPath;

    static void init();

    static void loadSingle(const std::string& filePath);
    static void load();
    static void save();
    static void clean();

    static void getVertexShader(ID3D11Device* device, void* function, size_t functionSize, VertexShader** ppShader);
    static void getPixelShader(ID3D11Device* device, void* function, size_t functionSize, PixelShader** ppShader);
};