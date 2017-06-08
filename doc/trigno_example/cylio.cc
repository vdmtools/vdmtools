//-----------------------------------------------------------------
// cylio.cc        Circular Cylinder simple I/O functions
//-----------------------------------------------------------------
#include <iostream>
#include "cylio.h"
using namespace std;

CircCyl GetCircCyl() {
  CircCyl in;
    
  cout << endl << endl;
  cout << " Input of Circular Cylinder Dimensions" << endl;
  cout <<"  radius: ";
  cin >> in.rad;

  cout <<"  height: ";
  cin >> in.height;

  cout <<"  slope [rad]: ";
  cin >> in.slope;

  return in;
}


void ShowCircCylVol(CircCyl cyl, float volume) {
  cout << endl << endl;
  cout << " Volume of Circular Cylinder" << endl << endl;
  cout << "Dimensions: " << endl;
  cout << "   radius: " << cyl.rad << endl;
  cout << "   height: " << cyl.height << endl;
  cout << "   slope: "<< cyl.slope << endl << endl;
  cout << " volume: " << volume<< endl << endl;
}
