#ifndef _externalservice_h_
#define _externalservice_h_

#ifdef _MSC_VER
#define DLLFUN __declspec(dllexport)
#else
#define DLLFUN 
#endif // _MSC_VER

#include "externalservicemanager.h"

extern L"C" {

  DLLFUN ExternalService* Register(const ExternalServiceManager*);
  DLLFUN void Deregister();

}

#endif //_externalservice_h_
