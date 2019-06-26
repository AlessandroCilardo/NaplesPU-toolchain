
unsigned int __udivsi3(unsigned int dividend, unsigned int divisor){
  if(dividend < divisor || divisor == 0)
    return 0;

  int res = 0;

  while(dividend >= divisor) {
    dividend -= divisor;
    res++;
  }

  return res;
}