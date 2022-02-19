#pragma once

class ShaderData
{
    friend class ShaderCache;

    void* handle;
    size_t length;

    ShaderData(void* handle, size_t length);

public:
    ShaderData(ShaderData&&);
    ShaderData(const ShaderData&) = delete;
    ~ShaderData();

    void* getBytes() const;
    size_t getLength() const;
};

class ShaderCache
{
public:
    static ShaderData get(void* function, size_t functionSize);
};
