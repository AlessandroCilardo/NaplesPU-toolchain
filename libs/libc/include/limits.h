#pragma once

#define CHAR_BIT 8
#define SCHAR_MIN       (-1 - SCHAR_MAX)
#define SCHAR_MAX       (0x7f)
#define UCHAR_MAX       (SCHAR_MAX * 2 + 1)
#define CHAR_MIN 0
#define CHAR_MAX UCHAR_MAX

#define SHRT_MIN	(-1 - SHRT_MAX)
#define SHRT_MAX	(0x7fff)
#define USHRT_MAX	(SHRT_MAX * 2 + 1)

#define INT_MIN		(-1 - INT_MAX)
#define INT_MAX		(0x7fffffff)
#define UINT_MAX	(INT_MAX * 2u + 1u)
/*
#define LONG_MIN	(-1l - LONG_MAX)
#define LONG_MAX	(0x7fffffffffffffffl)
#define ULONG_MAX	(LONG_MAX * 2ul + 1ul)

#define LLONG_MIN	(-1ll - LLONG_MAX)
#define LLONG_MAX	0x7fffffffffffffffll
#define ULLONG_MAX	(LLONG_MAX * 2ull + 1ull)
*/
