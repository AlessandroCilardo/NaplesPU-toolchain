#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int strcmp(const char *str1, const char *str2)
{
    while (*str1 && *str1 == *str2)
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

int strncmp(const char *str1, const char *str2, size_t length)
{
    if (length-- == 0)
        return 0;

    while (*str1 && length && *str1 == *str2)
    {
        str1++;
        str2++;
        length--;
    }

    return *str1 - *str2;
}

int strcasecmp(const char *str1, const char *str2)
{
    while (*str1 && toupper(*str1) == toupper(*str2))
    {
        str1++;
        str2++;
    }

    return toupper(*str1) - toupper(*str2);
}

int strncasecmp(const char *str1, const char *str2, size_t length)
{
    if (length-- == 0)
        return 0;

    while (*str1 && length && toupper(*str1) == toupper(*str2))
    {
        str1++;
        str2++;
        length--;
    }

    return toupper(*str1) - toupper(*str2);
}

size_t strlen(const char *str)
{
    unsigned len = 0;
    while (*str++)
        len++;

    return len;
}

char* strcpy(char *dest, const char *src)
{
    char *d = dest;
    while (*src)
        *d++ = *src++;

    *d = 0;
    return dest;
}

char* strncpy(char *dest, const char *src, size_t length)
{
    char *d = dest;
    while (*src && length > 0)
    {
        *d++ = *src++;
        length--;
    }

    if (length > 0)
        *d = 0;

    return dest;
}

char *strchr(const char *string, int c)
{
    for (const char *s = string; *s; s++)
        if (*s == c)
            return (char*) s;

    return 0;
}

void *memchr(const void *_s, int c, size_t n)
{
    const char *s = (const char*) _s;

    for (unsigned int i = 0; i < n; i++)
    {
        if (s[i] == c)
            return (void*) &s[i];
    }

    return 0;
}

char *strcat(char *c, const char *s)
{
    char *ret = c;
    while (*c)
        c++;

    while (*s)
        *c++ = *s++;

    *c = '\0';
    return ret;
}


