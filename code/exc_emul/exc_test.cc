
#include <iostream.h>
#include <stdio.h>

#include "exc_emul.h" 

int test(int n)
{
  EXC_TRY {
    printf(L"Trying: %d: %2o\n", n, n%4);
    if (n<4) {
      if (n&1) {
        EXC_THROW(n);
      } else {
        EXC_TRY_DONE; 
        return 0;
      }
    } else {
      test(n/4);
      EXC_TRY_DONE;
      printf(L"Returning %d\n",n); 
      return 0; 
    }
  }
  EXC_CATCH {
    printf(L"Catching: %d\n", n);
    printf(L"ExType = %d\n", EXC_Error.ExType);
    if (n&2) {
      EXC_THROW(n);
    } else {
      return 0;
    }
  }
}

void test_it()
{
  long i; 
  for (i=0; i<256; i++) {
    EXC_TRY {
      test(i); 
      EXC_TRY_DONE;                // finish and cleanup EXC_TRY
    }
    EXC_CATCH {
      // Use variable EXC_Error.ExType.
      // Catch code (may include rethrowing: EXC_THROW(error))
      printf(L"Main: Catching\n");
      printf(L"Main: ExType = %d\n", EXC_Error.ExType);
    }
  }
}

main()
{
  test_it(); 
}

