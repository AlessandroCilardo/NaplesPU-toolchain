#pragma once

#define NULL 0

typedef unsigned int size_t;
typedef int ptrdiff_t;

#define offsetof(__type__, __member__) ((size_t) (&((__type__*) 0)->__member__))
