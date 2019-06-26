
extern unsigned long long int __divdi3(unsigned long long int value1, unsigned long long int value2);

// Signed 64-bit integer modulus
long long int __moddi3(long long int value1, long long int value2) {
    return value1 - __divdi3(value1, value2) * value2;
}