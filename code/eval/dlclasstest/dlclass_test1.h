// example.h
// this file should be generated!
#ifndef _example_h_
#define _example_h_

#include "dlclass.h"

class Example: public DlClass
{
public:
  Generic DlMethodCall (const wchar_t* n, const Sequence& p);

  Generic ReturnList (const Sequence& p);
};

#endif
