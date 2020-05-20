#pragma once

#include <array>

static const std::array<uint32_t, 256> CRC32_TABLE = [&]() -> const std::array<uint32_t, 256>
{
    std::array<uint32_t, 256> crc32Table;

    const uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (size_t j = 0; j < 8; j++)
        {
            if (c & 1)
            {
                c = polynomial ^ (c >> 1);
            }
            else
            {
                c >>= 1;
            }
        }

        crc32Table[i] = c;
    }

    return crc32Table;
}();

static uint32_t generateCrc32Hash(const uint32_t seed, void* data, const size_t length)
{
    uint32_t c = seed ^ 0xFFFFFFFF;
    const uint8_t* u = (uint8_t*)data;
    for (size_t i = 0; i < length; ++i)
        c = CRC32_TABLE[(c ^ u[i]) & 0xFF] ^ (c >> 8);

    return c ^ 0xFFFFFFFF;
}
