/***
*  * WHAT
*  *    Main C++ program for the VDM++ sort example
*  * ID
*  *    $Id: sort_pp.cc,v 1.14 2005/05/27 07:46:33 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) 2016 Kyushu University
***/

#include <fstream>
#include "metaiv.h"
#include "SortMachine.h"
#include "Sorter.h"
#include "ExplSort.h"
#include "ImplSort.h"
#include "DoSort.h"
#include "MergeSort.h"

// The main program.

int main(int argc, const char *argv[])
{
  //  let arr1 = [3,5,2,23,1,42,98,31],
  //      arr2 = [3,1,2]:
  type_iL arr1 (mk_sequence(Int(3), Int(5), Int(2), Int(23), Int(1), Int(42), Int(98), Int(31)));
  type_iL arr2 (mk_sequence(Int(3), Int(1), Int(2)));

  // dcl smach : SortMachine := new SortMachine(),
  //     res : seq of int = [];
  type_ref_SortMachine smach (ObjectRef (new vdm_SortMachine ()));
  type_iL res;

  //     def dos : Sorter := new DoSort() in
  //     res = smach.SetAndSort(dos,arr1);
  
  wcout << L"Evaluating DoSort(" << arr1.ascii () << L"):" << endl;
  type_ref_Sorter dos (ObjectRef (new vdm_DoSort ()));
  res = ObjGet_vdm_SortMachine(smach)->vdm_SetAndSort (dos,arr1);
  wcout << res.ascii() << endl << endl;
  
  //     def expls  : Sorter := new ExplSort() in
  //     res = smach.SetAndSort(expls,arr2);
  wcout << L"Evaluating ExplSort(" << arr2.ascii () << L"):" << endl;
  type_ref_Sorter expls (ObjectRef(new vdm_ExplSort ()));
  res = ObjGet_vdm_SortMachine(smach)->vdm_SetAndSort (expls,arr2);
  wcout << res.ascii() << endl << endl;
  
  //     def imps  : Sorter := new ImplSort() in
  //     (res = smach.SetAndSort(imps,arr2)
  //        imps.Post_ImplSorter(arr2,res))
  
  wcout << L"Evaluating ImplSort(" << arr2.ascii () << L"):" << endl;
  type_ref_Sorter imps (ObjectRef(new vdm_ImplSort ()));
  res = ObjGet_vdm_SortMachine(smach)->vdm_SetAndSort (imps,arr2);
  wcout << res.ascii() << endl << endl;

  wcout << L"Evaluating post condition for ImplSort:" << endl;
  Bool p = ObjGet_vdm_ImplSort(imps)->vdm_post_ImplSorter (arr2, res);
  wcout << L"post_ImplSort(" << arr2.ascii () << L"," << res.ascii () << L"):" << endl;
  wcout << p.ascii () << endl << endl;
   
  //     def mergs : Sorter := new MergeSort() in
  //       smach.SetSort(mergs);
  type_ref_Sorter mergs (ObjectRef(new vdm_MergeSort ()));
  ObjGet_vdm_SortMachine(smach)->vdm_SetSort (mergs);
  
  //     res = smach.GoSorting(arr2);
  wcout << L"Evaluating MergeSort(" << arr2.ascii () << L"):" << endl;
  res = ObjGet_vdm_SortMachine(smach)->vdm_GoSorting(arr2);
  wcout << res.ascii() << endl << endl;

  return 0; 
}
