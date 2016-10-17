// header file to inout1.cc

struct PointType
{
  int x;
  int y;
};

typedef PointType Point;

extern void PointXY (int x, int y);
extern Point RotatePoint (int x, int y, float rad);
