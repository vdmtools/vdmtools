/***
*  * WHAT
*  *    Main C++ program for the VDM-SL sort example
*  * ID
*  *    $Id: sort_ex.cc,v 1.9 2005/05/27 07:47:27 vdmtools Exp $
*  *    This file is distributed as sort_ex.cpp under win32.
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) 2016 Kyushu University
***/

#include <fstream>
#include "metaiv.h"
#include "DefaultMod.h"

int main(int argc, const char * argv[]) 
{
  // Initialize values in DefaultMod
  init_DefaultMod();

  // Constructing the value l = [0, -12, 45]
  type_rL l (mk_sequence(Real(0), Real(-12), Real(45)));

  type_rL res;
  Bool b;

  // res := DoSort(l);
  wcout << L"Evaluating DoSort(" << l.ascii() << L"):" << endl;
  res = vdm_DefaultMod_DoSort(l);
  wcout << res.ascii() << endl << endl;
  
  // res := ExplSort(l);
  wcout << L"Evaluating ExplSort(" << l.ascii() << L"):" << endl;
  res = vdm_DefaultMod_ExplSort(l);
  wcout << res.ascii() << endl << endl;
  
  // res := ImplSort(l);
  wcout << L"Evaluating ImplSort(" << l.ascii() << L"):" << endl;
  res = vdm_DefaultMod_ImplSort(l);
  wcout << res.ascii() << endl << endl;

  // b  := post_ImplSort(l, res);
  wcout << L"Evaluating the post condition of ImplSort." << endl;
  wcout << L"post_ImplSort(" << l.ascii() << L", " << res.ascii() << L"):" << endl;
  b = vdm_DefaultMod_post_ImplSort(l, res);
  wcout << b.ascii() << endl << endl;

  // b  := inv_PosReal(hd l);
  wcout << L"Evaluation the invariant of PosReal." << endl;
  wcout << L"inv_PosReal(" << l.Hd().ascii() << L"):" << endl;
  b = vdm_DefaultMod_inv_PosReal(l.Hd());
  wcout << b.ascii() << endl << endl;

  // inv_PosReal(hd res);
  wcout << L"Evaluation the invariant of PosReal." << endl;
  wcout << L"inv_PosReal(" << res.Hd().ascii() << L"):" << endl;
  b = vdm_DefaultMod_inv_PosReal(res.Hd());
  wcout << b.ascii() << endl << endl;
  
  // res := MergeSort(l);
  // This will imply a run-time error!
  wcout << L"Evaluating MergeSort(" << l.ascii() << L"):" << endl;
  res = vdm_DefaultMod_MergeSort(l);
  wcout << res.ascii() << endl << endl;

  return 0;
}
