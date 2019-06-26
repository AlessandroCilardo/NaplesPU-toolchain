#include <stdint.h>
#include <ctype.h>

int isalnum(int c){
    return (unsigned int) ((c | 0x20) - 'a') < 26u ||
      (unsigned int) (c - '0') < 10u;
}

int isalpha(int c){
    return (unsigned int) ((c | 0x20) - 'a') < 26u;
}

int isblank(int c){
    return c == ' ' || c == '\t';
}

int iscntrl(int c){
    return (unsigned int) c < 32u || c == 127;
}

int isdigit(int c){
    return (unsigned int) (c - '0') < 10u;
}

int isgraph(int c){
    return (unsigned int) (c - '!') < 127u - '!';
}

int islower(int c){
    return (unsigned int) (c - 'a') < 26u;
}

int isprint(int c){
    c &= 0x7f;
    return (c >= 32 && c < 127);
}

int ispunct(int c){
    return isprint(c) && !isalnum(c) && !isspace(c);
}

int isspace(int c){
    return (unsigned int) (c-9) < 5u || c == ' ';
}

int isupper(int c){
    return (unsigned int) (c - 'A') < 26u;
}

int isxdigit(int c){
    return (unsigned int) (c - '0') < 10u || 
      (unsigned int) ((c | 0x20) - 'a') < 6u;
}

int tolower(int val){
    if (val >= 'A' && val <= 'Z')
        return val + ('a' - 'A');

    return val;
}

int toupper(int val){
    if (val >= 'a' && val <= 'z')
        return val - ('a' - 'A');

    return val;
}

int isascii(int c){
    return (unsigned int) c < 128u;
}
