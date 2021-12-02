#include "lcm.hpp"

namespace Utils {
  long long gcd(long long int a, long long int b)
  {
    if (b == 0)
      return a;
    return gcd(b, a % b);
  }
  
  // Function to return LCM of two numbers
  long long lcm(int a, int b)
  {
      return (a / gcd(a, b)) * b;
  }
}

  