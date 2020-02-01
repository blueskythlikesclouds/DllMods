#pragma once

struct String
{
    char* value;
    uint32_t length;

    void clear();

    String();
    String(const char* value);
    ~String();
};
