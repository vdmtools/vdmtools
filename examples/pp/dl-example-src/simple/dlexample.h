// example.h
// this file should be generated!
#ifndef _example_h_
#define _example_h_

#include "dlclass.h"

class Example: public DlClass
{
public:
	Generic DlMethodCall (const char* n, const Sequence& p);

	Generic DoSomething (const Sequence& p);
};

#endif
