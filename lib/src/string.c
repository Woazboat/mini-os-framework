#include "string.h"

void *memset(void *dest, int ch, size_t count)
{
        for(char* i = dest; i < (char*)dest + count; ++i)
        {
                *i = ch;
        }

        return dest;
}


void *memcpy(void *dest, const void* src, size_t count)
{
        char *d = dest;
        const char *s = src;
        for(; d < (char*)dest + count; ++d, ++s)
        {
                *d = *s;
        }

        return dest;
}


size_t strlen(const char *str)
{
        size_t len = 0;
        while(*(str+len) != '\0')
                ++len;

        return len;
}
