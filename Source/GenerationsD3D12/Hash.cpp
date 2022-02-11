#include "Hash.h"

size_t Hash::compute(void* data, const size_t bytes)
{
    size_t hash = 0;
    for (size_t i = 0; i < bytes; i++)
    {
        hash = (hash << 4) + ((char*)data)[i];
        hash ^= (hash >> 24);
    }
    return hash;
}
