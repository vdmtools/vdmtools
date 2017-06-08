#include <iostream.h>
#include "metaiv.h"
#include "cg_aux.h"
#include "WorkSpace.h"
#include "Quadrilateral.h"
#include "Parallelogram.h"
#include "Square.h"

// Auxiliary function to create tuples.

Tuple mk_(int i, int j)
{
  Tuple res(2);
  res.SetField(1, Int (i));
  res.SetField(2, Int (j));
  return res;
}

// The main program.

main()
{
  // Initialise the VDM C++ library with the record tags from
  // the CGinfo file in the NewQuadrilateral directory
  VDMSetTagMap(CGReadTagMap("m4tag_rep"));
  
  // dcl ws: @WorkSpace := WorkSpace!new,
  //     q : @Quadrilateral := Quadrilateral!new,
  vdm_WorkSpace *wsp = new vdm_WorkSpace ();
  ObjectRef ws (wsp);
  ObjectRef q (new vdm_Quadrilateral ());

  // q!SetShape(mk_(0,0), mk_(0,6), mk_(5, 11), mk_(8, -1));
  ObjGet_vdm_Quadrilateral(q)->vdm_SetShape(mk_(0,0), mk_(0,6), mk_(5, 11), mk_(8, -1));
  // ws!Display(1, q);
  wsp->vdm_Display(Int (1), q);

  // q := Parallelogram!new;
  q = ObjectRef(new vdm_Parallelogram());
  // q!SetShape(mk_(10,10), mk_(10,12), mk_(15, 15), mk_(15, 13));
  ObjGet_vdm_Quadrilateral(q)->vdm_SetShape
    (mk_(10,10), mk_(10,12), mk_(15, 15), mk_(15, 13));
  // ws!Display(2, q);
  wsp->vdm_Display(Int (2), q);

  // q := Square!new;
  q = ObjectRef(new vdm_Square());
  // q.SetShape(mk_(-8,6), mk_(-8,12), mk_(0,12), mk_(0, -8));
  ObjGet_vdm_Quadrilateral(q)->vdm_SetShape(mk_(-8,6), mk_(-8,12), mk_(0,12), mk_(0, -8));
  // ws!Display(3, q)
  wsp->vdm_Display(Int (3), q);
  
  // ws!Move(2, mk_(10,20), mk_(3,2));
  wsp->vdm_Move(Int (2), mk_(10,20), mk_(3,2));
  // ws!Move(1, mk_(0,0), mk_(5,7));
  wsp->vdm_Move(Int (1), mk_(0,0), mk_(5,7));
}
