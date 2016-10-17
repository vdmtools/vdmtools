#ifndef __val2x_h__
#define __val2x_h__

#include "metaiv.h"
#include "semvalues.h"
#include <iostream>

class VAL2X
{
public:
  static void init_EVAL_SEM();
  static void init_STKM();
  static void init_STAT_SEM();
  static void init_POG_TEST();

  static Generic val2generic(const TYPE_SEM_VAL & r, const Set & obj_refs); 
  static wstring val2asc(const TYPE_SEM_VAL & r); 
  static void val2stream(const TYPE_SEM_VAL & r, wostream & str, int ind); 
  static void PrintSemValue (const TYPE_SEM_VAL & val_v, wostream& ostr);

private:
  static void my_space(wostream & os, int indent);

  static wstring ASTypeList2Ascii(const SEQ<TYPE_AS_Type> & tp_l);
#ifdef VDMPP
  static void print_obj_ref_v(wostream & o, const Record & r, VDMFormatter vg, bool simple);
  static wstring getobjrefid(const TYPE_SEM_OBJ_uRef & r);
  static Tuple IsStatic(const TYPE_AS_Name & clsnm, const TYPE_AS_Name & valnm);
  static Generic objval2generic(const TYPE_SEM_OBJ & sem_val,
                                const TYPE_SEM_OBJ_uRef & obj_ref,
                                const Set & obj_refs);
#endif // VDMPP
};

#endif // __val2x_h__
