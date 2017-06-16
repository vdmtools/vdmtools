#include <math.h>
// This is a file which provides simple IO

struct PointType
{
  int x;
  int y;
};

typedef PointType Point;

#include <iostream.h>

int round(double x) {
  return (int) floor(x+0.5);
}

void PointXY (int x, int y)
{
    
  cout <<"\n\n Coordinates";
  cout <<"\n  x: " << x;

  cout <<"\n  y: "<< y;
}

Point RotatePoint (int x, int y, float angle)
{
  Point pt;
  float rad = M_PI/180*angle;
  float r = sqrt(x*x + y*y);
  float v = acos(x/r);
  float w = v+rad;
  pt.x = round(r*cos(w));
  pt.y = round(r*sin(w));
  return(pt);
}


