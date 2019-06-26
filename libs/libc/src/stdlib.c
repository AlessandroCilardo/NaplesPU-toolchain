#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <misc.h>

#define ABS_INT_MIN 2147483648U //tolto L - no support to 64

static int randseed = -1;

/*double atof(const char *nptr) {
  double tmp=strtod(nptr,0);
  return tmp;
}*/

int atoi(const char *s) {
  int v=0;
  int sign=1;
  while (isspace(*s)) s++;
  switch (*s) {
  case '-': sign=-1;
  case '+': ++s;
  }
  while (isdigit(*s)) {
    v=v*10+*s-'0'; ++s;
  }
  return sign==-1?-v:v;
}

/*long int atol(const char* s) {
  int v=0;
  int sign=0;
  while (isspace(*s)) ++s;
  switch (*s) {
  case '-': sign=-1;
  case '+': ++s;
  }
  while (isdigit(*s)) {
    v=v*10+*s-'0'; ++s;
  }
  return sign?-v:v;
}

long long int atoll(const char* s) {
  long long int v=0;
  int sign=1;
  while (isspace(*s)) ++s;
  switch (*s) {
  case '-': sign=-1;
  case '+': ++s;
  }
  while (isdigit(*s)) {
    v=v*10+*s-'0'; ++s;
  }
  return sign==-1?-v:v;
}*/
/*
double strtod(const char* s, char** endptr) {
    register const char*  p     = s;
    register long double  value = 0.L;
    int                   sign  = +1;
    long double           factor;
    unsigned int          expo;

    while ( isspace(*p) )
        p++;

    switch (*p) {
    case '-': sign = -1;
    case '+': p++;
    default : break;
    }

    while ( isdigit(*p) )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( isdigit(*p) ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.L;

        switch (*++p) {
        case '-': factor = 0.1;
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.L;
                  p     = s;
                  goto done;
        }

        while ( isdigit(*p) )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != NULL )
        *endptr = (char*)p;

    return value * sign;
}
*/
float strtof(const char* s, char** endptr) {
    register const char*  p     = s;
    register float        value = 0.;
    int                   sign  = +1;
    float                 factor;
    unsigned int          expo;

    while ( isspace(*p) )
        p++;

    switch (*p) {
    case '-': sign = -1;
    case '+': p++;
    default : break;
    }

    while ( isdigit(*p) )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( isdigit(*p) ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.L;

        switch (*++p) {
        case '-': factor = 0.1;
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.L;
                  p     = s;
                  goto done;
        }

        while ( isdigit(*p) )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != NULL )
        *endptr = (char*)p;

    return value * sign;
}

int strtol(const char *nptr, char **endptr, int base) {
  int neg=0;
  unsigned int v;
  const char*orig=nptr;

  while(isspace(*nptr)) nptr++;

  if (*nptr == '-' && isalnum(nptr[1])) { neg=-1; ++nptr; }
  v=strtoul(nptr,endptr,base);
  if (endptr && *endptr==nptr) *endptr=(char *)orig;
  if (v>=ABS_INT_MIN) {
    if (v==ABS_INT_MIN && neg) {
      //errno=0;
      return v;
    }
    //errno=ERANGE;
    return (neg?INT_MIN:INT_MAX);
  }
  return (neg?-v:v);
}
/*
long double strtold(const char* s, char** endptr) {
    register const char*  p     = s;
    register long double  value = 0.L;
    int                   sign  = +1;
    long double           factor;
    unsigned int          expo;

    while ( isspace(*p) )
        p++;

    switch (*p) {
    case '-': sign = -1;
    case '+': p++;
    default : break;
    }

    while ( isdigit(*p) )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( isdigit(*p) ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.L;

        switch (*++p) {
        case '-': factor = 0.1;
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.L;
                  p     = s;
                  goto done;
        }

        while ( isdigit(*p) )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != NULL )
        *endptr = (char*)p;

    return value * sign;
}

long long int strtoll(const char *nptr, char **endptr, int base)
{
  int neg=0;
  unsigned long long int v;
  const char*orig=nptr;

  while(isspace(*nptr)) nptr++;

  if (*nptr == '-' && isalnum(nptr[1])) { neg=-1; nptr++; }
  v=strtoull(nptr,endptr,base);
  if (endptr && *endptr==nptr) *endptr=(char *)orig;
  if (v>LLONG_MAX) {
    if (v==0x8000000000000000ull && neg) {
      //errno=0;
      return v;
    }
    //errno=ERANGE;
    return (neg?LLONG_MIN:LLONG_MAX);
  }
  return (neg?-v:v);
}

unsigned long int strtoul(const char *ptr, char **endptr, int base) {
  int neg = 0, overflow = 0;
  unsigned long int v=0;
  const char* orig;
  const char* nptr=ptr;

  while(isspace(*nptr)) ++nptr;

  if (*nptr == '-') { neg=1; nptr++; }
  else if (*nptr == '+') ++nptr;
  orig=nptr;
  if (base==16 && nptr[0]=='0') goto skip0x;
  if (base) {
    register unsigned int b=base-2;
    if (unlikely(b>34)) { 
      //errno=EINVAL; 
      return 0; 
    }
  } else {
    if (*nptr=='0') {
      base=8;
skip0x:
      if ((nptr[1]=='x'||nptr[1]=='X') && isxdigit(nptr[2])) {
	nptr+=2;
	base=16;
      }
    } else
      base=10;
  }
  while(*nptr) {
    register unsigned char c=*nptr;
    c=(c>='a'?c-'a'+10:c>='A'?c-'A'+10:c<='9'?c-'0':0xff);
    if (unlikely(c>=base)) break;	// out of base //
    {
      register unsigned long x=(v&0xff)*base+c;
      register unsigned long w=(v>>8)*base+(x>>8);
      if (w>(ULONG_MAX>>8)) overflow=1;
      v=(w<<8)+(x&0xff);
    }
    ++nptr;
  }
  if (unlikely(nptr==orig)) {	// no conversion done 
    nptr=ptr;
    //errno=EINVAL;
    v=0;
  }
  if (endptr) *endptr=(char *)nptr;
  if (overflow) {
    //errno=ERANGE;
    return ULONG_MAX;
  }
  return (neg?-v:v);
}

unsigned long long int strtoull(const char *ptr, char **endptr, int base)
{
  int neg = 0, overflow = 0;
  long long int v=0;
  const char* orig;
  const char* nptr=ptr;

  while(isspace(*nptr)) ++nptr;

  if (*nptr == '-') { neg=1; nptr++; }
  else if (*nptr == '+') ++nptr;
  orig=nptr;
  if (base==16 && nptr[0]=='0') goto skip0x;
  if (base) {
    register unsigned int b=base-2;
    if (unlikely(b>34)) { 
      //errno=EINVAL; 
      return 0; 
    }
  } else {
    if (*nptr=='0') {
      base=8;
skip0x:
      if (((*(nptr+1)=='x')||(*(nptr+1)=='X')) && isxdigit(nptr[2])) {
	nptr+=2;
	base=16;
      }
    } else
      base=10;
  }
  while(*nptr) {
    register unsigned char c=*nptr;
    c=(c>='a'?c-'a'+10:c>='A'?c-'A'+10:c<='9'?c-'0':0xff);
    if (unlikely(c>=base)) break;	///* out of base 
    {
      register unsigned long x=(v&0xff)*base+c;
      register unsigned long long w=(v>>8)*base+(x>>8);
      if (w>(ULLONG_MAX>>8)) overflow=1;
      v=(w<<8)+(x&0xff);
    }
    ++nptr;
  }
  if (unlikely(nptr==orig)) {	//	/* no conversion done 
    nptr=ptr;
    //errno=EINVAL;
    v=0;
  }
  if (endptr) *endptr=(char *)nptr;
  if (overflow) {
    //errno=ERANGE;
    return ULLONG_MAX;
  }
  return (neg?-v:v);
}
*/
int rand(void) {
    randseed = randseed * 1103515245 + 12345;
    return randseed & 0x7fffffff;
}

void srand(unsigned int seed) {
    randseed = seed;
}

void* bsearch(const void *searchKey, const void *base, size_t num,
              size_t size, int (*compare)(const void*,const void*)) {
    int low = 0;
    int high = num - 1;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        void *midKey = (char*) base + mid * size;
        int compVal = (*compare)(searchKey, midKey);
        if (compVal == 0)
            return midKey;
        else if (compVal < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }

    return NULL;
}

void qsort(void *base, size_t nel, size_t width, cmpfun cmp) {
    unsigned int i, j, k;
    char tmp;

    if (nel == 0)
        return;

    for (i = 0; i < nel - 1; i++)
    {
        for (j = i + 1; j < nel; j++)
        {
            char *elem1 = (char*) base + i * width;
            char *elem2 = (char*) base + j * width;
            if (cmp(elem1, elem2) > 0)
            {
                // swap
                for (k = 0; k < width; k++)
                {
                    tmp = elem1[k];
                    elem1[k] = elem2[k];
                    elem2[k] = tmp;
                }
            }
        }
    }
}

int abs(int i) {
    if (i < 0)
        return -i;

    return i;
}
/*
long int labs(long int i) {
  return i>=0?i:-i; 
}

long long int llabs(long long int i) { 
  if (i<0) i=-i; 
  return i; 
}*/

