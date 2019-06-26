extern unsigned long long int __udivdi3(unsigned long long int dividend, unsigned long long int divisor);

// Signed 64-bit integer division
long long int __divdi3(long long int value1, long long int value2) {
	int sign1 = (value1 >> 63) && 1;
	int sign2 = (value2 >> 63) && 1;

	// Take absolute values
	unsigned long long int u_value1 = (value1 ^ sign1) - sign1;
	unsigned long long int u_value2 = (value2 ^ sign2) - sign2;

	// Compute result sign
	sign1 ^= sign2;

	// Perform division (will call __udivsi3), then convert sign back
	return (__udivdi3(u_value1, u_value2) ^ sign1) - sign1;
}