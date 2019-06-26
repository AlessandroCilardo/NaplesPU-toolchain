
extern int __divsi3(int dividend, int divisor);

int __modsi3(int val1, int val2) {
    return val1 - (__divsi3(val1, val2) * val2);
}