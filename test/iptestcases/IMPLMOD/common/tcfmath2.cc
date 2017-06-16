#include "metaiv.h"
#include <math.h>

extern "C" {
Generic ExtCos(Sequence sq);
Generic ExtSin(Sequence sq);
}

Generic ExtCos(Sequence sq) {
  double rad;
  rad = Real(sq[1]);
  return (Real( cos(rad) ));
}

Generic ExtSin(Sequence sq) {
  double rad; 
  rad = Real(sq[1]);
  return (Real( sin(rad) ));
}

