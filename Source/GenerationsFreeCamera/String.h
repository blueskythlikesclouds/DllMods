#pragma once

struct String
{
    void* value;

    void clear();

    String();
    String(const char* value);
    ~String();
};
