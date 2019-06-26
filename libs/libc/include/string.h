#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t length);
int strcasecmp(const char *str1, const char *str2);
int strncasecmp(const char *str1, const char *str2, size_t length);
size_t strlen(const char *str);
char* strcpy(char *dest, const char *src);
char* strncpy(char *dest, const char *src, size_t length);
char *strchr(const char *string, int c);
void *memchr(const void *_s, int c, size_t n);
char *strcat(char *c, const char *s);

void *memcpy(void *dest, const void *src, size_t length);
void *memset(void *dest, int value, size_t length);
int memcmp(const void *a, const void *b, size_t length);

#ifdef __cplusplus
}
#endif
