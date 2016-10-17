/***
*  * WHAT
*  *    Functions for removing position information from the AST.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/rmposinfo.cc,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Informa
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Ole Storm
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#include "rmposinfo.h"
#include <fstream>
#include <stdlib.h>
#include "tag.h"


Generic RemovePosInfo(Generic g)
  // Removes any position information in the AST in g.
  // Identifies all records, removes the last field if its tag is PosInfo
  // and removes position information recursively on all remaining fields.
{
  metaivType mt = g.gettype();
  
  switch(mt){
  case mt_record:
    {
    Record r(g);
    if (r.Length () >= 1) {
      Generic last(r.GetField(r.Length()));
      if(last.IsRecord() && Record(last).Is(PosInfo)){
	// Remove the last field containing the position information
	Record tr(r.GetTag(), r.Length()-1);
	for(int i=1; i <= r.Length()-1; i++)
	  tr.SetField(i, r.GetField(i));
	r = tr;
      }
      for(int i = 1; i <= r.Length(); i++)
	r.SetField(i, RemovePosInfo(r.GetField(i)));
      }
    return r;
    }
  case mt_sequence:
    {
    Sequence s(g), res;
    Generic elem;
    for(int i = s.First(elem); i; i=s.Next(elem))
      res.ImpAppend(RemovePosInfo(elem));
    return res;
    }

  case mt_set:
    {
    Set s(g), res;
    Generic elem;
    for(int i = s.First(elem); i; i=s.Next(elem))
      res.Insert(RemovePosInfo(elem));
    return res;
    }

  case mt_map:
    {
    Map m(g), res;
    Generic elem;

    for (int i = m.First (elem); i; i = m.Next (elem))
      res.Insert (RemovePosInfo(elem), RemovePosInfo(m[elem]));
    return res;
    }

  case mt_tuple:
    {
    Tuple t(g);

    for(int i=1; i<=t.Length(); i++)
      t.SetField(i, RemovePosInfo(t.GetField(i)));
    return t;
    }

  default:
    // All other non-composite types are simply returned as is.
    return g;
  }
}
    

