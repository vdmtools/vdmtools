#include "lm_client.h"  
#include "lm_code.h"
#include "lm_attr.h"

#include <iostream>
#include <string.h>

char feature[200];
char version[200];

//LM_DATA_STATIC; == (line below). 
static LM_HANDLE_PTR lm_job = (LM_HANDLE_PTR) 0;

LM_CODE(code, ENCRYPTION_SEED1, ENCRYPTION_SEED2,
        VENDOR_KEY1, VENDOR_KEY2, VENDOR_KEY3,
	VENDOR_KEY4, VENDOR_KEY5);

#include "timing.h"

int main(int argc, char * argv[]) 
{
  Timing t;  
  strcpy(feature, (argc>1) ? argv[1] : "vppjv");
  strcpy(version, (argc>2) ? argv[2] : "6.7");

  std::cerr << "feature=" << feature 
            << '\n' 
            << "vers=" << version << '\n'; 
  t.start("before lc_init");
  int rc; 

  rc = lc_init(lm_job, "XXXX", &code, &lm_job);
  if (rc != 0) {
    std::cerr << "error: lc_init: " 
              << rc << '=' 
              << lc_errstring (lm_job) << "\n";
    return 1;
  }
  t.stop("after lc_init");

  t.start("before checkout");
  rc = lc_checkout(lm_job, feature, version, 1, 
                   LM_CO_NOWAIT, &code, LM_DUP_NONE);
  t.stop("after checkout");
  if (rc != 0) {
    std::cerr << "res=" 
              << rc << '=' 
              << lc_errstring (lm_job) << "\n";
  }
  return 0;
}
