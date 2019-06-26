extern unsigned long long int __udivdi3(unsigned long long int dividend, unsigned long long int divisor);

// Unsigned 64-bit integer modulus
unsigned long long int __umoddi3(unsigned long long int value1, unsigned long long int value2) {
    return value1 - __udivdi3(value1, value2) * value2;
}