/***
*  * WHAT
*  *    flexlm definitions for VPP and VDM-SL toolbox
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/flexlm/flexlm.cc,v $
*  * VERSION
*  *    $Revision: 1.16 $
*  * DATE
*  *    $Date: 2001/10/19 15:49:50 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    VPP, Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#include <iostream.h>
#include <time.h>		// prototype for time
#include "flexlm.h"
#include "tb_version.h"
#include "tbutils.h"

#include "lm_client.h"  
#include "lm_code.h"
#include "lm_attr.h"

#ifdef FLM

#include "flexlmcheck.h"        // check that the right ENCRYPTION_SEED1,2 are right.
#include "tb_wstring.h"

#ifdef VDMPP
char vdmpp_feature[200];
char* vdmpp_version;
char vppcg_feature[200];
char* vppcg_version;
#endif // VDMPP

#ifdef VDMSL
char vdmsl_feature[200];
char* vdmsl_version;
char vdmcg_feature[200];
char* vdmcg_version;
#endif //VDMSL

//LM_DATA_STATIC; == (line below). 
static LM_HANDLE_PTR lm_job = (LM_HANDLE_PTR) 0;

#if defined(_MSC_VER) || defined(__linux__) 
LM_CODE(code, ENCRYPTION_SEED1, ENCRYPTION_SEED2,
        VENDOR_KEY1, VENDOR_KEY2, VENDOR_KEY3,
	VENDOR_KEY4, VENDOR_KEY5);
#else
LM_CODE(code, ENCRYPTION_CODE_1, ENCRYPTION_CODE_2,
        VENDOR_KEY1, VENDOR_KEY2, VENDOR_KEY3,
	VENDOR_KEY4, VENDOR_KEY5);
#endif

// When did we last do a lc_timer call. Used by Check_lm_periodically
static time_t time_of_last_lc_timer;
// Don't call lc_timer more often than this. 
static const int min_lc_timer_distance = 2*60;

static wstring PrintLicenseError (LM_HANDLE_PTR job);

#ifdef DL_MODULE_ENABLED
class FLM_DL_License
{
private:
  enum { init, no_feature_exists } status;  
  bool Warned;
  bool HasLicense;

public:
  FLM_DL_License(); 
  ~FLM_DL_License(); 
  void Reset(); 
  bool GetDLLicense(); 
};

FLM_DL_License::FLM_DL_License()
{
  Warned = false;
  HasLicense = false;
  status = init; 
}

FLM_DL_License::~FLM_DL_License()
{
}

void
FLM_DL_License::Reset()
{
  if (HasLicense) {
    CheckInLicense(DL_FEATURE);
  }
  HasLicense = false;
  Warned = false;
  status = init; 
}

bool
FLM_DL_License::GetDLLicense()
{
  if (HasLicense)
    return true;

  if (status == no_feature_exists)
    return false;
    
  int rc;
  const string version (wstring2string(tb_version.GetVersionIdForFLEXlm()));   
  rc = lc_checkout(lm_job, DL_FEATURE,
                   const_cast<char*>(version.c_str()), 
                   1, LM_CO_NOWAIT, &code, LM_DUP_NONE);
  if (rc == 0) {
    HasLicense = true;
    return true; 
  }
  else if (!Warned) {
    switch (rc) {
    case NOFEATURE:
      vdm_log << L"\nError: feature '" 
              << DL_FEATURE
#ifdef VDMSL 
              << L"' for DL modules does not exist.\n" 
#else
              << L"' for DL classes does not exist.\n" 
#endif
              << L"(this warning will not be repeated)\n" << flush;
      status = no_feature_exists;
      Warned = true;          
      break;
    case MAXUSERS:
      vdm_log << L"Warning: licensed number of users already reached.\n" 
	      << flush;
      // Warned = true;          
      break;
    default:
      vdm_log << L"Error: while checking out DL license feature:\n"
              << PrintLicenseError(lm_job) << flush;
      Warned = true; 
    }
  }
  return false; 
}

FLM_DL_License * flm_dl_license;
#endif // DL_MODULE_ENABLED

//Initialise the license daemon
wstring InitFLM ()
{
  wstring error;
  int rc; 
  rc = lc_init(lm_job, "XXXX", &code, &lm_job);
  if (rc != 0) {
    error = L"lc_init:";
    error += PrintLicenseError (lm_job);
    return error;
  }
  // Disable flexlm's SIGALRM timer. 
  lc_set_attr(lm_job, LM_A_CHECK_INTERVAL, (LM_A_VAL_TYPE)-1); 
  lc_set_attr(lm_job, LM_A_RETRY_INTERVAL, (LM_A_VAL_TYPE)-1);
  // Read back values code (just for debug)
  // int lm_val_check, lm_val_retry;
  // lc_get_attr(lm_job, LM_A_CHECK_INTERVAL, (short*) &lm_val_check); 
  // lc_get_attr(lm_job, LM_A_RETRY_INTERVAL, (short*) &lm_val_retry);
  // printf(L"LM_Vals set to %d, %d, ", lm_val_check, lm_val_retry);
  
  // int rc = lm_checkout(FEATURE, VERSION, 1, LM_CO_NOWAIT, &code, LM_DUP_NONE);
  const string version (wstring2string(tb_version.GetVersionIdForFLEXlm()));   
  rc = lc_checkout(lm_job, FEATURE, const_cast<char*>(version.c_str()),
                   1,
                   LM_CO_NOWAIT, &code, LM_DUP_NONE);
  if (rc != 0) 
    error = PrintLicenseError (lm_job);

#if DL_MODULE_ENABLED
  flm_dl_license = new FLM_DL_License;
#endif // DL_MODULE_ENABLED
  
  return error;
}

void CheckInLicense (char* feature)
{
  lc_checkin(lm_job, feature, 0);
}

int CheckOutFeature (char* feature, char* version)
{
  int rc = lc_checkout(lm_job, feature, version, 
		       1, LM_CO_NOWAIT, &code, LM_DUP_NONE);
  if (rc != 0) 
    vdm_log << PrintLicenseError (lm_job) << flush;
  return rc;
}

// We are running without flexlm automatically calling lc_timer
// so we must do it ourselves. 
void Check_lm_periodically ()
{
  // Uses time_of_last_lc_timer and min_lc_timer_distance
  time_t t; 
  time(&t);
  if (t-time_of_last_lc_timer > min_lc_timer_distance) {
    lc_timer(lm_job);
    time_of_last_lc_timer = t;
  }
}

#ifdef DL_MODULE_ENABLED
bool FLM_GetDLLicense()
{
  return flm_dl_license->GetDLLicense();
}
  
void FLM_ResetDLLicense()
{
  flm_dl_license->Reset();
}
#endif // DL_MODULE_ENABLED

//This functions prints out the error message corresponding to a given error
//number from the licens manager

wstring PrintLicenseError (LM_HANDLE_PTR job) {
  wstring str (L"License was denied: ");
  str += string2wstring(lc_errstring(job));
  str += L"\n";
  return str;
}


#else

// This is for vdmlite.

wstring InitFLM ()
{
  return wstring(L""); 
}

void CheckInLicense (char* feature)
{

}

int CheckOutFeature (char* feature, char* version)
{
  return 0; 
}

void Check_lm_periodically ()
{
}

#ifdef DL_MODULE_ENABLED
bool FLM_GetDLLicense()
{
  return false; 
}
  
void FLM_ResetDLLicense()
{
}
#endif // DL_MODULE_ENABLED

//This functions prints out the error message corresponding to a given error
//number from the licens manager






#endif //FLM
