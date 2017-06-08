#ifndef _parserservice_h_
#define _parserservice_h_

#include "externalservicemanager.h"

class ParserService : public InternalService {
 public:
  const wchar_t* GetName(){ return L"Parser"; }
  Generic MediateRequest(const wchar_t*, const Sequence&) const;

 private:
  Generic GetASTS() const;
  Generic GetAST(const wchar_t*) const;
  Set GetClassNames() const;
};

#endif //_parserservice_h_
