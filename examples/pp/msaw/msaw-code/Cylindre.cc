//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Wed 05-Nov-97 by the VDM++toC++ Code Generator
// (v6.0e - Tue 04-Nov-97)
//
// Supported compilers:
// 	g++ version 2.7.2 on SunOS4, SunOS5, Linux, HP-UX10
// 	Dec cxx version 5.5 on Alpha OSF1
// 	VC++ version 5.0 on Windows95, Windows NT
//



#include "Cylindre.h"

vdm_Cylindre::vdm_Cylindre() { RegisterAsDerived(vdm_GetId()); }

Bool vdm_Cylindre::vdm_DansCylindre(const Tuple &var_1_1, const Int &var_2_2, const Tuple &var_3_3, const Real &var_4_4, const Int &var_5_5) {
  Real vdm_rayon;
  Int vdm_x1;
  Int vdm_x2;
  Int vdm_y1;
  Int vdm_y2;
  Int vdm_z1;
  Int vdm_z2;
  bool succ_6 = true;
  if (var_1_1.IsTuple()) {
    Sequence e_l_32;
    for (int i_33 = 1; i_33 <= ((Tuple) var_1_1).Length(); i_33++) 
      e_l_32.ImpAppend(((Tuple) var_1_1).GetField(i_33));
    if (succ_6 = 2 == (Int) e_l_32.Length()) {
      vdm_x1 = (Int) e_l_32.Hd();
      vdm_y1 = (Int) e_l_32[2];
    }
  }
  else 
    succ_6 = false;
  vdm_z1 = var_2_2;
  if (var_3_3.IsTuple()) {
    Sequence e_l_34;
    for (int i_35 = 1; i_35 <= ((Tuple) var_3_3).Length(); i_35++) 
      e_l_34.ImpAppend(((Tuple) var_3_3).GetField(i_35));
    if (succ_6 = 2 == (Int) e_l_34.Length()) {
      vdm_x2 = (Int) e_l_34.Hd();
      vdm_y2 = (Int) e_l_34[2];
    }
  }
  else 
    succ_6 = false;
  vdm_rayon = var_4_4;
  vdm_z2 = var_5_5;
  if (!succ_6) 
    RunTime("Pattern Match in Function Apply in function : DansCylindre failed");
  Bool varRes_7;
  Bool var1_8;
  Real var1_9;
  Int var1_10;
  Int var1_11;
  Int unArg_12;
  unArg_12 = (vdm_x1 - vdm_x2).Exp((Int) 2);
  int tmp_17 = unArg_12.GetValue();  if (tmp_17 < 0) 
    var1_11 = (Int) (-tmp_17);
  else 
    var1_11 = (Int) tmp_17;
  Int var2_18;
  Int unArg_19;
  unArg_19 = (vdm_y1 - vdm_y2).Exp((Int) 2);
  int tmp_24 = unArg_19.GetValue();  if (tmp_24 < 0) 
    var2_18 = (Int) (-tmp_24);
  else 
    var2_18 = (Int) tmp_24;
  var1_10 = var1_11 + var2_18;
  var1_9 = var1_10.Exp((Real) 0.5);
  var1_8 = (Bool) ((var1_9.GetValue()) <= ((vdm_rayon / vdm_NMperCoordonnee).GetValue()));
{ if ((varRes_7 = var1_8).GetValue()) 
   varRes_7 = (Bool) ((vdm_z1.GetValue()) <= (vdm_z2.GetValue())); }
  return varRes_7;
}
