// This is a file which provides simple IO
#include <math.h>
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

void PointXY (Point pt)
{
    
  cout <<"\n\n Coordinates";
  cout <<"\n  x: " << pt.x;

  cout <<"\n  y: "<< pt.y;
}

Point RotatePoint (Point pt, float angle)
{
  Point pr;

  int x = pt.x;
  int y = pt.y;

  float rad = M_PI/180*angle;
  float r = sqrt(x*x + y*y);
  float v = acos(x/r);
  float w = v+rad;
  pr.x =  round(r*cos(w));
  pr.y = round(r*sin(w));
  return(pr);
}

