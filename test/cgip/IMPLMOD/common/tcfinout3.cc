// type conversion functions inout3.cc

#include "metaiv.h"
#include "inout1.h"
#include <math.h>

extern "C"
{
  void InitDLModule(bool init);
  void Place(Sequence sq1);
  Generic Rotate(Sequence sq1);
  }

void InitDLModule(bool init) {

}

  void Place(Sequence sq1)
    {
      int x, y;
      x = (Int)sq1[1];
      y = (Int)sq1[2];
      PointXY(x,y);          
    } 

  Generic Rotate(Sequence sq1)
    {
      int x, y;
      float r;
      Point pt;
      Tuple tp(sq1[1]);
      r = (Real) sq1[2];
      
      x = Int(tp.GetField(1)).GetValue();
      y = Int(tp.GetField(2)).GetValue();
      
      pt =  RotatePoint(x,y,r);
      tp.SetField(1, (Int) pt.x);
      tp.SetField(2, (Int) pt.y);

      return(tp);
    }
