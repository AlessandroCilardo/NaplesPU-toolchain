unsigned long long int __udivdi3(unsigned long long int dividend, unsigned long long int divisor) {
    if(dividend < divisor || divisor == 0)
    return 0;
  
    long long int res = 0;
  
    while(dividend >= divisor) {
      dividend -= divisor;
      res++;
    }
  
    return res;
}