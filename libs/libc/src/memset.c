#include <stdint.h>

typedef int _vec16i32 __attribute__((__vector_size__(16 * sizeof(int))));

void* memset(void *_dest, int value, unsigned int length)
{
    char *dest = (char*) _dest;
    value &= 0xff;

    // XXX Possibly fill bytes/words until alignment is hit

    if ((((unsigned int) dest) & 63) == 0)
    {
        // Write 64 bytes at a time.
        _vec16i32 reallyWideValue = (vec16i32)(value | (value << 8) | (value << 16)
                                    | (value << 24));
        while (length > 64)
        {
            *((_vec16i32*) dest) = reallyWideValue;
            length -= 64;
            dest += 64;
        }
    }

    if ((((unsigned int) dest) & 3) == 0)
    {
        // Write 4 bytes at a time.
        unsigned wideVal = value | (value << 8) | (value << 16) | (value << 24);
        while (length > 4)
        {
            *((unsigned int*) dest) = wideVal;
            dest += 4;
            length -= 4;
        }
    }

    // Write one byte at a time
    while (length > 0)
    {
        *dest++ = value;
        length--;
    }

    return _dest;
}
