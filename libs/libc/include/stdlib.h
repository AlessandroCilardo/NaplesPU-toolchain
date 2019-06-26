#pragma once

#include <stddef.h>

#define RAND_MAX 0x7fffffff

typedef int (*cmpfun)(const void *, const void *);

#ifdef __cplusplus
extern "C" {
#endif

//double atof(const char *nptr);
int atoi(const char *num);
//long int atol(const char* s);
//long long int atoll(const char* s);
//double strtod(const char* s, char** endptr);
float strtof(const char* s, char** endptr);
int strtol(const char *nptr, char **endptr, int base);
//long double strtold(const char* s, char** endptr);
//long long int strtoll(const char *nptr, char **endptr, int base);
//unsigned long int strtoul(const char *ptr, char **endptr, int base);
//unsigned long long int strtoull(const char *ptr, char **endptr, int base);

int rand(void);
void srand(unsigned int seed);

void* bsearch (const void* key, const void* base, size_t num,
               size_t size, int (*compare)(const void*,const void*));
void qsort(void *base, size_t nel, size_t width, cmpfun cmp);

int abs(int value);
//long int labs(long int i);
//long long int llabs(long long int i);

#ifdef __cplusplus
}
#endif
