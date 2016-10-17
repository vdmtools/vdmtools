// header file to inout2.cc

struct PointType
{
  int x;
  int y;
};

typedef PointType Point;

extern void PointXY (Point pt);
extern Point RotatePoint (Point pt, float rad);

