// type conversion functions inout2.cc

#include "metaiv.h"
#include "inout2.h"

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
      Point pt;
      Tuple tuple(sq1[1]);
      pt.x = ((Int) tuple.GetField(1)).GetValue();
      pt.y = ((Int) tuple.GetField(2)).GetValue();
      PointXY(pt);          
    } 

  Generic Rotate(Sequence sq1)
    {
      float r;
      Point pt1, pt2;
      Tuple Tp(2);
      
      Tuple tuple(sq1[1]);
      pt1.x = ((Int) tuple.GetField(1)).GetValue();
      pt1.y = ((Int) tuple.GetField(2)).GetValue();
      r = (Real)sq1[2];
      
      pt2 = RotatePoint(pt1,r);
      Tp.SetField(1,(Real) pt2.x);
      Tp.SetField(2,(Real) pt2.y);
      return(Tp);
    }
