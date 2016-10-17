//---------------------------------------------------------------
// tcfmath.cc   type conversion functions for libmath.so
//---------------------------------------------------------------
#include "metaiv.h"
#include <math.h>

#ifdef WIN32
#define DLLFUN __declspec(dllexport)
#define M_PI   3.14
#else
#define DLLFUN 
#endif

extern "C" {
  DLLFUN void InitDLModule(bool init); 
  DLLFUN Generic ExtCos(Sequence sq);
  DLLFUN Generic ExtSin(Sequence sq);
  DLLFUN Generic ExtPI ();
}

void InitDLModule(bool init)
{
  // This function is called by the Toolbox when modules are
  // initialised and before they are unloaded.
  // init is true on after load and false before unload. 
}

Generic ExtCos(Sequence sq) {
  return (Real( cos((Real)sq[1]) ));
}

Generic ExtSin(Sequence sq) {
  return (Real( sin((Real) sq[1]) ));
}

Generic ExtPI () {
  return(Real(M_PI));
}










