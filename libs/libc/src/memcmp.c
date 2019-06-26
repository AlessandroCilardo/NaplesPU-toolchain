int memcmp(const void *_str1, const void *_str2, unsigned int len)
{
    const char *str1 = _str1;
    const char *str2 = _str2;

    while (len--)
    {
        int diff = *str1++ - *str2++;
        if (diff)
            return diff;
    }

    return 0;
}
