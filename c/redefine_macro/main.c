/* strtok example */
#include <stdio.h>
#include <string.h>

#define A(x) (2 * x)

#ifdef A
#undef A
#define A(x) (3 * x)
#endif


int main ()
{
  printf("%d\r\n", A(10));

  // # Expected output:
  // $ ./a.out
  // 30
  // $

  return 0;
}

