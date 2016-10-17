//---------------------------------------------------------------
// tcfmath.cc   type conversion functions for libmath.so
//---------------------------------------------------------------
#include "metaiv.h"
#include <math.h>

#ifdef _MSC_VER
#define DLLFUN __declspec(dllexport)
#define M_PI   3.14
#else
#define DLLFUN 
#endif // _MSC_VER

extern "C" {
  DLLFUN void InitDLModule(bool init); 
  DLLFUN void ExtCos(const Sequence & sq, Generic & res);
  DLLFUN void ExtSin(const Sequence & sq, Generic & res);
  DLLFUN void ExtPI (Generic & res);
}

void InitDLModule(bool init)
{
  // This function is called by the Toolbox when modules are
  // initialised and before they are unloaded.
  // init is true on after load and false before unload. 
}

void ExtCos(const Sequence & sq, Generic & res) {
  res = (Real( cos((Real)sq[1]) ));
}

void ExtSin(const Sequence & sq, Generic & res) {
  res = (Real( sin((Real) sq[1]) ));
}

void ExtPI (Generic & res) {
  res = (Real(M_PI));
}

