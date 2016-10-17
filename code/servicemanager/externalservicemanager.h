#ifndef _externalservicemanager_h_
#define _externalservicemanager_h_

#include "metaiv.h"

class ExternalService {
 public:
  virtual void Run() = 0;
  virtual ~ExternalService(){};
};

class InternalService {
 public:
  virtual const wchar_t* GetName() = 0;
  virtual Generic MediateRequest(const wchar_t*, const Sequence&) const = 0;
  virtual ~InternalService(){};
};

class ExternalServiceManager {
 public:
  virtual ExternalService* RequestExternalService(const wchar_t*) = 0;
  virtual InternalService* RequestInternalService(const wchar_t*) = 0;
  virtual ~ExternalServiceManager(){};
};

#endif //_externalservicemanager_h_
