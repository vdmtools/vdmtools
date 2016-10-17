//-----------------------------------------------------------------
// tcfcylio.cc  type conversion functions for circular cylinder io
//-----------------------------------------------------------------

#include "metaiv.h"
#include "cylio.h"

#ifdef WIN32
#define DLLFUN __declspec(dllexport)
#else
#define DLLFUN 
#endif

extern "C" {
  DLLFUN void InitDLModule(bool init); 
  DLLFUN Generic ExtGetCylinder(Sequence sq1);
  DLLFUN void ExtShowCircCylVol(Sequence sq1);
}

const int tag_CYLINDER_CircCyl = 1; 
const int size_CYLINDER_CircCyl = 3; 

void InitDLModule(bool init)
{
  if (init) {
    VDMGetDefaultRecInfoMap().NewTag(tag_CYLINDER_CircCyl, 
                                     size_CYLINDER_CircCyl); 
    VDMGetDefaultRecInfoMap().SetSymTag(tag_CYLINDER_CircCyl, 
                                        "CYLINDER`CircCyl"); 
  }
}

Generic ExtGetCylinder(Sequence sq1) 
{
  CircCyl cyl;
  Record Rc(tag_CYLINDER_CircCyl,size_CYLINDER_CircCyl );
  
  cyl = GetCircCyl();   // input of cylinder dimension
  Rc.SetField(1, (Real)cyl.rad); // conversion in VDM C++ record class
  Rc.SetField(2, (Real)cyl.height);
  Rc.SetField(3, (Real)cyl.slope);
  return(Rc);    // return Record to the interpreter process               
}

void ExtShowCircCylVol(Sequence sq1) 
{
  CircCyl cyl;
  Record Rc(tag_CYLINDER_CircCyl,size_CYLINDER_CircCyl);
  float vol;

  // extract cylinder dimension and volume from sequence
  Rc =  sq1[1];
  vol = (Real) sq1[2];

  // convert Record in a C++ structure
  cyl.rad = (Real) Rc.GetField(1);
  cyl.height = (Real) Rc.GetField(2);
  cyl.slope = (Real) Rc.GetField(3);

  ShowCircCylVol(cyl, vol); //make output
  return;
}

