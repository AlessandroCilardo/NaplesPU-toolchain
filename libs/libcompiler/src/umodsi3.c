
extern unsigned int __udivsi3(unsigned int dividend, unsigned int divisor);

unsigned int __umodsi3(unsigned int val1, unsigned int val2) {
    return val1 - (__udivsi3(val1, val2) * val2);
}