// type conversion functions inout1.cc

#include "metaiv.h"
#include "inout1.h"

extern "C" {
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
      Tuple tp(2);
      
      
      x = (Int)sq1[1];
      y = (Int)sq1[2];
      r = (Real)sq1[3];
      
      pt =  RotatePoint(x,y,r);
      tp.SetField(1, (Int) pt.x);
      tp.SetField(2, (Int) pt.y);

      return(tp);
    }
