#include "metaiv.h"
#include <math.h>

extern "C" {
  void InitDLModule(bool init);
  Generic ExtCos(Sequence sq);
   Generic ExtSin(Sequence sq);
   Generic ExtPI();
}

void InitDLModule(bool init) {
  
}

Generic ExtCos(Sequence sq)
{
  double rad;
  rad = Real(sq[1]);
  return (Real( cos(rad) ));
}

 Generic ExtSin(Sequence sq)
 {
   double rad; 
     rad = Real(sq[1]);
     return (Real( sin(rad) ));
 }

 Generic ExtPI ()
 {
   return(Real(M_PI));
 }
