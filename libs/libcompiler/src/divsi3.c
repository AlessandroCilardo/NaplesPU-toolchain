
extern unsigned int __udivsi3(unsigned int dividend, unsigned int divisor);


int __divsi3(int value1, int value2)
{
  //return (int)(((float)dividend)/((float)divisor));

  int sign1 = (value1 >> 31) && 1;
  int sign2 = (value2 >> 31) && 1;

  // Take absolute values
  unsigned int u_value1 = (value1 ^ sign1) - sign1;
  unsigned int u_value2 = (value2 ^ sign2) - sign2;

  // Compute result sign
  sign1 ^= sign2;

  // Perform division (will call __udivsi3), then convert sign back
  return (__udivsi3(u_value1, u_value2) ^ sign1) - sign1;
}
