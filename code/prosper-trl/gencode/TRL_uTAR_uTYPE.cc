//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Wed 10-Nov-99 by the VDM-SLtoC++ Code Generator
// (v3.3 - Mon 22-Mar-99)
//
// Supported compilers:
// 	egcs version 1.1 on SunOS4, SunOS5, Linux, HP-UX10
// 	VC++ version 6.0 on Windows95, Windows NT
//



#include "TRL_uTAR_uTYPE.h"

void TYPE_TRL_uTAR_uTYPE_TYPE::Init(TYPE_HOL_type_uexpr p1, type_cLS p2) {
  SetField(1, p1);
  SetField(2, p2);
}

TYPE_HOL_type_uexpr TYPE_TRL_uTAR_uTYPE_TYPE::get_tp() const { return (TYPE_HOL_type_uexpr) GetField(1); }

void TYPE_TRL_uTAR_uTYPE_TYPE::set_tp(const TYPE_HOL_type_uexpr &p) { SetField(1, p); }

type_cLS TYPE_TRL_uTAR_uTYPE_TYPE::get_fv() const { return (type_cLS) GetField(2); }

void TYPE_TRL_uTAR_uTYPE_TYPE::set_fv(const type_cLS &p) { SetField(2, p); }

void TYPE_TRL_uTAR_uTYPE_FIELD::Init(TYPE_TRL_uTAR_uTYPE_Ids p1, TYPE_TRL_uTAR_uTYPE_Type p2) {
  SetField(1, p1);
  SetField(2, p2);
}

TYPE_TRL_uTAR_uTYPE_Ids TYPE_TRL_uTAR_uTYPE_FIELD::get_sel() const { return (TYPE_TRL_uTAR_uTYPE_Ids) GetField(1); }

void TYPE_TRL_uTAR_uTYPE_FIELD::set_sel(const TYPE_TRL_uTAR_uTYPE_Ids &p) { SetField(1, p); }

TYPE_TRL_uTAR_uTYPE_Type TYPE_TRL_uTAR_uTYPE_FIELD::get_tp() const { return (TYPE_TRL_uTAR_uTYPE_Type) GetField(2); }

void TYPE_TRL_uTAR_uTYPE_FIELD::set_tp(const TYPE_TRL_uTAR_uTYPE_Type &p) { SetField(2, p); }

void init_TRL_uTAR_uTYPE_VDMLib() {
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_TRL_uTAR_uTYPE_FIELD, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_TRL_uTAR_uTYPE_FIELD, "TRL_uTAR_uTYPE`FIELD");
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_TRL_uTAR_uTYPE_TYPE, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_TRL_uTAR_uTYPE_TYPE, "TRL_uTAR_uTYPE`TYPE");
  VDMGetDefaultRecInfoMap().NewTag(TOKEN, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TOKEN, "token");
}
#ifdef DEF_TRL_uTAR_uTYPE_USERIMPL

#include "TRL_uTAR_uTYPE_userimpl.cc"

#endif


void init_TRL_uTAR_uTYPE() { init_TRL_uTAR_uTYPE_VDMLib(); }
#ifndef DEF_TRL_uTAR_uTYPE_tar_uBooleanType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uBooleanType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uBooleanType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Bool_utype, length_HOL_Bool_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uBracketedType

TYPE_TRL_uTAR_uTYPE_BracketedType vdm_TRL_uTAR_uTYPE_tar_uBracketedType(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Record vdm_TRL_uTAR_uTYPE_tp(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_tp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uBracketedType failed");
  return (Generic) vdm_TRL_uTAR_uTYPE_tp;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uCharType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uCharType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uCharType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Char_utype, length_HOL_Char_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uCompositeType

TYPE_TRL_uTAR_uTYPE_CompositeType vdm_TRL_uTAR_uTYPE_tar_uCompositeType(const TYPE_TRL_uTAR_uTYPE_Ids &var_1_1, const type_20TRL_uTAR_uTYPE_FIELDCL &var_2_2, const TYPE_CI_ContextId &var_3_3) {
  Sequence vdm_TRL_uTAR_uTYPE_flds;
  Sequence vdm_TRL_uTAR_uTYPE_tag;
  bool succ_4 = true;
  vdm_TRL_uTAR_uTYPE_tag = var_1_1;
  vdm_TRL_uTAR_uTYPE_flds = var_2_2;
  if (succ_4) {}
  if (!succ_4) 
    RunTime("Pattern Match in Function Apply in function : tar_uCompositeType failed");
  Record varRes_5(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Sequence vdm_TRL_uTAR_uTYPE_hol_uflds;
    {
      Sequence res_l_8;
      Set resBind_s_10;
      Set riseq_14;
      int max_15 = (Int) vdm_TRL_uTAR_uTYPE_flds.Length();
      for (int i_16 = 1; i_16 <= max_15; i_16++) 
        riseq_14.Insert((Int) i_16);
      resBind_s_10 = riseq_14;
      Sequence bind_l_9;
      if ((Int) resBind_s_10.Card() != (Int) 0) 
        bind_l_9 = Sort(resBind_s_10);
      bool succ_11 = true;
      Int vdm_TRL_uTAR_uTYPE_i;
      Generic tmpe_28;
      for (int i_27 = bind_l_9.First(tmpe_28); i_27; i_27 = bind_l_9.Next(tmpe_28)) {
        Int e_12 = tmpe_28;
        vdm_TRL_uTAR_uTYPE_i = e_12;
        if (succ_11) {
          Record reselem_17(vdm_HOL_field, length_HOL_field);
          reselem_17 = Record(vdm_HOL_field, length_HOL_field);
          Sequence tmpVar_18;
          if (((Record) vdm_TRL_uTAR_uTYPE_flds[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_FIELD)) 
            tmpVar_18 = ((Record) vdm_TRL_uTAR_uTYPE_flds[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_FIELD_sel);
          else 
            RunTime("Unknown record field selector");
          reselem_17.SetField(1, tmpVar_18);
          Record tmpVar_22;
          Record tmpRec_23(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
          if (((Record) vdm_TRL_uTAR_uTYPE_flds[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_FIELD)) 
            tmpRec_23 = ((Record) vdm_TRL_uTAR_uTYPE_flds[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_FIELD_tp);
          else 
            RunTime("Unknown record field selector");
          if (tmpRec_23.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
            tmpVar_22 = tmpRec_23.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
          else 
            RunTime("Unknown record field selector");
          reselem_17.SetField(2, tmpVar_22);
          res_l_8.ImpAppend(reselem_17);
        }
      }
      vdm_TRL_uTAR_uTYPE_hol_uflds = res_l_8;
    }
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_record, length_HOL_record);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_record, length_HOL_record);
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, vdm_TRL_uTAR_uTYPE_tag);
    vdm_TRL_uTAR_uTYPE_tp.SetField(2, vdm_TRL_uTAR_uTYPE_hol_uflds);
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set unArg_33;
    Set res_s_34;
    {
      bool succ_40 = true;
      Set e_set_41;      Set riseq_43;      int max_44 = (Int) vdm_TRL_uTAR_uTYPE_flds.Length();      for (int i_45 = 1; i_45 <= max_44; i_45++) 
        riseq_43.Insert((Int) i_45);      e_set_41 = riseq_43;
      Int vdm_TRL_uTAR_uTYPE_i;
      {
        Generic tmpe_48;
        for (int bb_47 = e_set_41.First(tmpe_48); bb_47; bb_47 = e_set_41.Next(tmpe_48)) {
          Int elem_46 = tmpe_48;
          succ_40 = true;          vdm_TRL_uTAR_uTYPE_i = elem_46;          if (succ_40) {
            Set res_s_35;
            Record tmpRec_36(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
            if (((Record) vdm_TRL_uTAR_uTYPE_flds[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_FIELD)) 
              tmpRec_36 = ((Record) vdm_TRL_uTAR_uTYPE_flds[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_FIELD_tp);
            else 
              RunTime("Unknown record field selector");
            if (tmpRec_36.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
              res_s_35 = tmpRec_36.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
            else 
              RunTime("Unknown record field selector");
            res_s_34.Insert(res_s_35);
          }
        }
      }
    }
    unArg_33 = res_s_34;
    Set rduset_49;    Generic tmpe_52;    for (int bb_51 = unArg_33.First(tmpe_52); bb_51; bb_51 = unArg_33.Next(tmpe_52)) {
      Set e_50 = tmpe_52;
      rduset_49.ImpUnion(e_50);
    }    vdm_TRL_uTAR_uTYPE_fv = rduset_49;
    varRes_5 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_5.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_5.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_5;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uField

TYPE_TRL_uTAR_uTYPE_Field vdm_TRL_uTAR_uTYPE_tar_uField(const TYPE_TRL_uTAR_uTYPE_Ids &var_1_1, const TYPE_TRL_uTAR_uTYPE_Type &var_2_2, const Bool &var_3_3, const TYPE_CI_ContextId &var_4_4) {
  Sequence vdm_TRL_uTAR_uTYPE_sel;
  Record vdm_TRL_uTAR_uTYPE_tp(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_5 = true;
  vdm_TRL_uTAR_uTYPE_sel = var_1_1;
  vdm_TRL_uTAR_uTYPE_tp = var_2_2;
  if (succ_5) {}
  if (succ_5) {}
  if (!succ_5) 
    RunTime("Pattern Match in Function Apply in function : tar_uField failed");
  Record varRes_6(vdm_TRL_uTAR_uTYPE_FIELD, length_TRL_uTAR_uTYPE_FIELD);
  varRes_6 = Record(vdm_TRL_uTAR_uTYPE_FIELD, length_TRL_uTAR_uTYPE_FIELD);
  varRes_6.SetField(1, vdm_TRL_uTAR_uTYPE_sel);
  varRes_6.SetField(2, vdm_TRL_uTAR_uTYPE_tp);
  return (Generic) varRes_6;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uGeneralMapType

TYPE_TRL_uTAR_uTYPE_GeneralMapType vdm_TRL_uTAR_uTYPE_tar_uGeneralMapType(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_TRL_uTAR_uTYPE_Type &var_2_2, const TYPE_CI_ContextId &var_3_3) {
  Record vdm_TRL_uTAR_uTYPE_mapdom(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  Record vdm_TRL_uTAR_uTYPE_maprng(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_4 = true;
  vdm_TRL_uTAR_uTYPE_mapdom = var_1_1;
  vdm_TRL_uTAR_uTYPE_maprng = var_2_2;
  if (succ_4) {}
  if (!succ_4) 
    RunTime("Pattern Match in Function Apply in function : tar_uGeneralMapType failed");
  Record varRes_5(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_Map_utype, length_HOL_Map_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Map_utype, length_HOL_Map_utype);
    Sequence tmpVar_8;
    Record e_seq_9;
    if (vdm_TRL_uTAR_uTYPE_mapdom.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      e_seq_9 = vdm_TRL_uTAR_uTYPE_mapdom.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    tmpVar_8 = Sequence().ImpAppend(e_seq_9);
    Record e_seq_11;
    if (vdm_TRL_uTAR_uTYPE_maprng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      e_seq_11 = vdm_TRL_uTAR_uTYPE_maprng.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    tmpVar_8.ImpAppend(e_seq_11);
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_8);
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set var1_14;
    if (vdm_TRL_uTAR_uTYPE_mapdom.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var1_14 = vdm_TRL_uTAR_uTYPE_mapdom.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    Set var2_16;
    if (vdm_TRL_uTAR_uTYPE_maprng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var2_16 = vdm_TRL_uTAR_uTYPE_maprng.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_fv = var1_14;
    vdm_TRL_uTAR_uTYPE_fv.ImpUnion(var2_16);
    varRes_5 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_5.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_5.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_5;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uIds

TYPE_TRL_uTAR_uTYPE_Ids vdm_TRL_uTAR_uTYPE_tar_uIds(const TYPE_AS_Ids &vdm_TRL_uTAR_uTYPE_ids) { return (Generic) (Sequence) vdm_TRL_uTAR_uTYPE_ids.Hd(); }
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uInjectiveMapType

TYPE_TRL_uTAR_uTYPE_InjectiveMapType vdm_TRL_uTAR_uTYPE_tar_uInjectiveMapType(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_TRL_uTAR_uTYPE_Type &var_2_2, const TYPE_CI_ContextId &var_3_3) {
  Record vdm_TRL_uTAR_uTYPE_mapdom(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  Record vdm_TRL_uTAR_uTYPE_maprng(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_4 = true;
  vdm_TRL_uTAR_uTYPE_mapdom = var_1_1;
  vdm_TRL_uTAR_uTYPE_maprng = var_2_2;
  if (succ_4) {}
  if (!succ_4) 
    RunTime("Pattern Match in Function Apply in function : tar_uInjectiveMapType failed");
  Record varRes_5(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_Map_utype, length_HOL_Map_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Map_utype, length_HOL_Map_utype);
    Sequence tmpVar_8;
    Record e_seq_9;
    if (vdm_TRL_uTAR_uTYPE_mapdom.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      e_seq_9 = vdm_TRL_uTAR_uTYPE_mapdom.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    tmpVar_8 = Sequence().ImpAppend(e_seq_9);
    Record e_seq_11;
    if (vdm_TRL_uTAR_uTYPE_maprng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      e_seq_11 = vdm_TRL_uTAR_uTYPE_maprng.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    tmpVar_8.ImpAppend(e_seq_11);
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_8);
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set var1_14;
    if (vdm_TRL_uTAR_uTYPE_mapdom.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var1_14 = vdm_TRL_uTAR_uTYPE_mapdom.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    Set var2_16;
    if (vdm_TRL_uTAR_uTYPE_maprng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var2_16 = vdm_TRL_uTAR_uTYPE_maprng.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_fv = var1_14;
    vdm_TRL_uTAR_uTYPE_fv.ImpUnion(var2_16);
    varRes_5 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_5.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_5.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_5;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uIntegerType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uIntegerType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uIntegerType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Int_utype, length_HOL_Int_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uName

TYPE_TRL_uTAR_uTYPE_Name vdm_TRL_uTAR_uTYPE_tar_uName(const TYPE_TRL_uTAR_uTYPE_Ids &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Sequence vdm_TRL_uTAR_uTYPE_ids;
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_ids = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uName failed");
  return (Generic) vdm_TRL_uTAR_uTYPE_ids;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uNatOneType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uNatOneType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uNatOneType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Num_utype, length_HOL_Num_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uNatType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uNatType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uNatType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Num_utype, length_HOL_Num_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uOptionalType

TYPE_TRL_uTAR_uTYPE_OptionalType vdm_TRL_uTAR_uTYPE_tar_uOptionalType(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Record vdm_TRL_uTAR_uTYPE_elemtp(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_elemtp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uOptionalType failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_Option_utype, length_HOL_Option_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Option_utype, length_HOL_Option_utype);
    Record tmpVar_7;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      tmpVar_7 = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_7);
    Set vdm_TRL_uTAR_uTYPE_fv;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      vdm_TRL_uTAR_uTYPE_fv = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uPartialFnType

TYPE_TRL_uTAR_uTYPE_PartialFnType vdm_TRL_uTAR_uTYPE_tar_uPartialFnType(const type_19TRL_uTAR_uTYPE_TYPECL &var_1_1, const TYPE_TRL_uTAR_uTYPE_Type &var_2_2, const TYPE_CI_ContextId &var_3_3) {
  Sequence vdm_TRL_uTAR_uTYPE_fndom;
  Record vdm_TRL_uTAR_uTYPE_fnrng(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_4 = true;
  vdm_TRL_uTAR_uTYPE_fndom = var_1_1;
  vdm_TRL_uTAR_uTYPE_fnrng = var_2_2;
  if (succ_4) {}
  if (!succ_4) 
    RunTime("Pattern Match in Function Apply in function : tar_uPartialFnType failed");
  Record varRes_5(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_fndom_uprod = vdm_TRL_uTAR_uTYPE_tar_uProductType((Generic) vdm_TRL_uTAR_uTYPE_fndom, (Generic) vdm_CI_NilContextId);
    Record vdm_TRL_uTAR_uTYPE_tp;
    if (((Bool) (vdm_TRL_uTAR_uTYPE_fndom == Sequence())).GetValue()) 
      if (vdm_TRL_uTAR_uTYPE_fnrng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
        vdm_TRL_uTAR_uTYPE_tp = vdm_TRL_uTAR_uTYPE_fnrng.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
      else 
        RunTime("Unknown record field selector");
    else {
      vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Fun_utype, length_HOL_Fun_utype);
      Sequence tmpVar_15;
      Record e_seq_16;
      if (vdm_TRL_uTAR_uTYPE_fndom_uprod.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
        e_seq_16 = vdm_TRL_uTAR_uTYPE_fndom_uprod.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
      else 
        RunTime("Unknown record field selector");
      tmpVar_15 = Sequence().ImpAppend(e_seq_16);
      Record e_seq_18;
      if (vdm_TRL_uTAR_uTYPE_fnrng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
        e_seq_18 = vdm_TRL_uTAR_uTYPE_fnrng.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
      else 
        RunTime("Unknown record field selector");
      tmpVar_15.ImpAppend(e_seq_18);
      vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_15);
    }
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set var1_22;
    if (vdm_TRL_uTAR_uTYPE_fndom_uprod.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var1_22 = vdm_TRL_uTAR_uTYPE_fndom_uprod.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    Set var2_24;
    if (vdm_TRL_uTAR_uTYPE_fnrng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var2_24 = vdm_TRL_uTAR_uTYPE_fnrng.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_fv = var1_22;
    vdm_TRL_uTAR_uTYPE_fv.ImpUnion(var2_24);
    varRes_5 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_5.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_5.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_5;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uProductType

TYPE_TRL_uTAR_uTYPE_ProductType vdm_TRL_uTAR_uTYPE_tar_uProductType(const type_19TRL_uTAR_uTYPE_TYPECL &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Sequence vdm_TRL_uTAR_uTYPE_stp;
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_stp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uProductType failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_Prod_utype, length_HOL_Prod_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Prod_utype, length_HOL_Prod_utype);
    Sequence tmpVar_7;
    {
      Sequence res_l_8;
      Set resBind_s_10;
      Set riseq_14;
      int max_15 = (Int) vdm_TRL_uTAR_uTYPE_stp.Length();
      for (int i_16 = 1; i_16 <= max_15; i_16++) 
        riseq_14.Insert((Int) i_16);
      resBind_s_10 = riseq_14;
      Sequence bind_l_9;
      if ((Int) resBind_s_10.Card() != (Int) 0) 
        bind_l_9 = Sort(resBind_s_10);
      bool succ_11 = true;
      Int vdm_TRL_uTAR_uTYPE_i;
      Generic tmpe_22;
      for (int i_21 = bind_l_9.First(tmpe_22); i_21; i_21 = bind_l_9.Next(tmpe_22)) {
        Int e_12 = tmpe_22;
        vdm_TRL_uTAR_uTYPE_i = e_12;
        if (succ_11) {
          Record reselem_17;
          if (((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
            reselem_17 = ((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
          else 
            RunTime("Unknown record field selector");
          res_l_8.ImpAppend(reselem_17);
        }
      }
      tmpVar_7 = res_l_8;
    }
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_7);
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set unArg_24;
    Set res_s_25;
    {
      bool succ_30 = true;
      Set e_set_31;      Set riseq_33;      int max_34 = (Int) vdm_TRL_uTAR_uTYPE_stp.Length();      for (int i_35 = 1; i_35 <= max_34; i_35++) 
        riseq_33.Insert((Int) i_35);      e_set_31 = riseq_33;
      Int vdm_TRL_uTAR_uTYPE_i;
      {
        Generic tmpe_38;
        for (int bb_37 = e_set_31.First(tmpe_38); bb_37; bb_37 = e_set_31.Next(tmpe_38)) {
          Int elem_36 = tmpe_38;
          succ_30 = true;          vdm_TRL_uTAR_uTYPE_i = elem_36;          if (succ_30) {
            Set res_s_26;
            if (((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
              res_s_26 = ((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
            else 
              RunTime("Unknown record field selector");
            res_s_25.Insert(res_s_26);
          }
        }
      }
    }
    unArg_24 = res_s_25;
    Set rduset_39;    Generic tmpe_42;    for (int bb_41 = unArg_24.First(tmpe_42); bb_41; bb_41 = unArg_24.Next(tmpe_42)) {
      Set e_40 = tmpe_42;
      rduset_39.ImpUnion(e_40);
    }    vdm_TRL_uTAR_uTYPE_fv = rduset_39;
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uQuoteType

TYPE_TRL_uTAR_uTYPE_QuoteType vdm_TRL_uTAR_uTYPE_tar_uQuoteType(const TYPE_AS_QuoteLit &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Sequence vdm_TRL_uTAR_uTYPE_str;
  bool succ_3 = true;
  if (var_1_1.IsRecord() && vdm_AS_QuoteLit == var_1_1.GetTag()) {
    Sequence e_l_11;
    for (int i_12 = 1; i_12 <= var_1_1.Length(); i_12++) 
      e_l_11.ImpAppend(var_1_1.GetField(i_12));
    if (succ_3 = 2 == (Int) e_l_11.Length()) 
      vdm_TRL_uTAR_uTYPE_str = (Generic) e_l_11.Hd();
  }
  else 
    succ_3 = false;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uQuoteType failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_quote_utype, length_HOL_quote_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_quote_utype, length_HOL_quote_utype);
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, vdm_TRL_uTAR_uTYPE_str);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uRatType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uRatType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uRatType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Real_utype, length_HOL_Real_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uRealType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uRealType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uRealType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Real_utype, length_HOL_Real_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uSeq0Type

TYPE_TRL_uTAR_uTYPE_Seq0Type vdm_TRL_uTAR_uTYPE_tar_uSeq0Type(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Record vdm_TRL_uTAR_uTYPE_elemtp(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_elemtp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uSeq0Type failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_List_utype, length_HOL_List_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_List_utype, length_HOL_List_utype);
    Record tmpVar_7;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      tmpVar_7 = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_7);
    Set vdm_TRL_uTAR_uTYPE_fv;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      vdm_TRL_uTAR_uTYPE_fv = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uSeq1Type

TYPE_TRL_uTAR_uTYPE_Seq1Type vdm_TRL_uTAR_uTYPE_tar_uSeq1Type(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Record vdm_TRL_uTAR_uTYPE_elemtp(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_elemtp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uSeq1Type failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_List_utype, length_HOL_List_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_List_utype, length_HOL_List_utype);
    Record tmpVar_7;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      tmpVar_7 = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_7);
    Set vdm_TRL_uTAR_uTYPE_fv;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      vdm_TRL_uTAR_uTYPE_fv = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uSetType

TYPE_TRL_uTAR_uTYPE_SetType vdm_TRL_uTAR_uTYPE_tar_uSetType(const TYPE_TRL_uTAR_uTYPE_Type &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Record vdm_TRL_uTAR_uTYPE_elemtp(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_elemtp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uSetType failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_Set_utype, length_HOL_Set_utype);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Set_utype, length_HOL_Set_utype);
    Record tmpVar_7;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      tmpVar_7 = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_7);
    Set vdm_TRL_uTAR_uTYPE_fv;
    if (vdm_TRL_uTAR_uTYPE_elemtp.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      vdm_TRL_uTAR_uTYPE_fv = vdm_TRL_uTAR_uTYPE_elemtp.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uTokenType

TYPE_TRL_uTAR_uTYPE_BasicType vdm_TRL_uTAR_uTYPE_tar_uTokenType(const TYPE_CI_ContextId &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : tar_uTokenType failed");
  Record varRes_3(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Ind_utype, length_HOL_Ind_utype);
    Set vdm_TRL_uTAR_uTYPE_fv = Set();
    varRes_3 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_3.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_3.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uTotalFnType

TYPE_TRL_uTAR_uTYPE_TotalFnType vdm_TRL_uTAR_uTYPE_tar_uTotalFnType(const type_19TRL_uTAR_uTYPE_TYPECL &var_1_1, const TYPE_TRL_uTAR_uTYPE_Type &var_2_2, const TYPE_CI_ContextId &var_3_3) {
  Sequence vdm_TRL_uTAR_uTYPE_fndom;
  Record vdm_TRL_uTAR_uTYPE_fnrng(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  bool succ_4 = true;
  vdm_TRL_uTAR_uTYPE_fndom = var_1_1;
  vdm_TRL_uTAR_uTYPE_fnrng = var_2_2;
  if (succ_4) {}
  if (!succ_4) 
    RunTime("Pattern Match in Function Apply in function : tar_uTotalFnType failed");
  Record varRes_5(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_fndom_uprod = vdm_TRL_uTAR_uTYPE_tar_uProductType((Generic) vdm_TRL_uTAR_uTYPE_fndom, (Generic) vdm_CI_NilContextId);
    Record vdm_TRL_uTAR_uTYPE_tp;
    if (((Bool) (vdm_TRL_uTAR_uTYPE_fndom == Sequence())).GetValue()) 
      if (vdm_TRL_uTAR_uTYPE_fnrng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
        vdm_TRL_uTAR_uTYPE_tp = vdm_TRL_uTAR_uTYPE_fnrng.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
      else 
        RunTime("Unknown record field selector");
    else {
      vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_Fun_utype, length_HOL_Fun_utype);
      Sequence tmpVar_15;
      Record e_seq_16;
      if (vdm_TRL_uTAR_uTYPE_fndom_uprod.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
        e_seq_16 = vdm_TRL_uTAR_uTYPE_fndom_uprod.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
      else 
        RunTime("Unknown record field selector");
      tmpVar_15 = Sequence().ImpAppend(e_seq_16);
      Record e_seq_18;
      if (vdm_TRL_uTAR_uTYPE_fnrng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
        e_seq_18 = vdm_TRL_uTAR_uTYPE_fnrng.GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
      else 
        RunTime("Unknown record field selector");
      tmpVar_15.ImpAppend(e_seq_18);
      vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_15);
    }
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set var1_22;
    if (vdm_TRL_uTAR_uTYPE_fndom_uprod.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var1_22 = vdm_TRL_uTAR_uTYPE_fndom_uprod.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    Set var2_24;
    if (vdm_TRL_uTAR_uTYPE_fnrng.Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
      var2_24 = vdm_TRL_uTAR_uTYPE_fnrng.GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
    else 
      RunTime("Unknown record field selector");
    vdm_TRL_uTAR_uTYPE_fv = var1_22;
    vdm_TRL_uTAR_uTYPE_fv.ImpUnion(var2_24);
    varRes_5 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_5.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_5.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_5;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uTypeName

TYPE_TRL_uTAR_uTYPE_TypeName vdm_TRL_uTAR_uTYPE_tar_uTypeName(const TYPE_TRL_uTAR_uTYPE_Ids &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Sequence vdm_TRL_uTAR_uTYPE_ids;
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_ids = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uTypeName failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_type, length_HOL_type);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_type, length_HOL_type);
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, vdm_TRL_uTAR_uTYPE_ids);
    vdm_TRL_uTAR_uTYPE_tp.SetField(2, Sequence());
    Set vdm_TRL_uTAR_uTYPE_fv;
    vdm_TRL_uTAR_uTYPE_fv = Set().Insert(vdm_TRL_uTAR_uTYPE_ids);
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uTypeVar

TYPE_TRL_uTAR_uTYPE_TypeVar vdm_TRL_uTAR_uTYPE_tar_uTypeVar(const TYPE_TRL_uTAR_uTYPE_Ids &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  bool succ_3 = true;
  if (succ_3) {}
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uTypeVar failed");
  Generic varRes_4;
  RunTime("Can not evaluate an undefined expression");
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_TRL_uTAR_uTYPE_tar_uUnionType

TYPE_TRL_uTAR_uTYPE_UnionType vdm_TRL_uTAR_uTYPE_tar_uUnionType(const type_19TRL_uTAR_uTYPE_TYPECL &var_1_1, const TYPE_CI_ContextId &var_2_2) {
  Sequence vdm_TRL_uTAR_uTYPE_stp;
  bool succ_3 = true;
  vdm_TRL_uTAR_uTYPE_stp = var_1_1;
  if (succ_3) {}
  if (!succ_3) 
    RunTime("Pattern Match in Function Apply in function : tar_uUnionType failed");
  Record varRes_4(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
  {
    Record vdm_TRL_uTAR_uTYPE_tp(vdm_HOL_sum, length_HOL_sum);
    vdm_TRL_uTAR_uTYPE_tp = Record(vdm_HOL_sum, length_HOL_sum);
    Sequence tmpVar_7;
    {
      Sequence res_l_8;
      Set resBind_s_10;
      Set riseq_14;
      int max_15 = (Int) vdm_TRL_uTAR_uTYPE_stp.Length();
      for (int i_16 = 1; i_16 <= max_15; i_16++) 
        riseq_14.Insert((Int) i_16);
      resBind_s_10 = riseq_14;
      Sequence bind_l_9;
      if ((Int) resBind_s_10.Card() != (Int) 0) 
        bind_l_9 = Sort(resBind_s_10);
      bool succ_11 = true;
      Int vdm_TRL_uTAR_uTYPE_i;
      Generic tmpe_22;
      for (int i_21 = bind_l_9.First(tmpe_22); i_21; i_21 = bind_l_9.Next(tmpe_22)) {
        Int e_12 = tmpe_22;
        vdm_TRL_uTAR_uTYPE_i = e_12;
        if (succ_11) {
          Record reselem_17;
          if (((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
            reselem_17 = ((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_TYPE_tp);
          else 
            RunTime("Unknown record field selector");
          res_l_8.ImpAppend(reselem_17);
        }
      }
      tmpVar_7 = res_l_8;
    }
    vdm_TRL_uTAR_uTYPE_tp.SetField(1, tmpVar_7);
    Set vdm_TRL_uTAR_uTYPE_fv;
    Set unArg_24;
    Set res_s_25;
    {
      bool succ_30 = true;
      Set e_set_31;      Set riseq_33;      int max_34 = (Int) vdm_TRL_uTAR_uTYPE_stp.Length();      for (int i_35 = 1; i_35 <= max_34; i_35++) 
        riseq_33.Insert((Int) i_35);      e_set_31 = riseq_33;
      Int vdm_TRL_uTAR_uTYPE_i;
      {
        Generic tmpe_38;
        for (int bb_37 = e_set_31.First(tmpe_38); bb_37; bb_37 = e_set_31.Next(tmpe_38)) {
          Int elem_36 = tmpe_38;
          succ_30 = true;          vdm_TRL_uTAR_uTYPE_i = elem_36;          if (succ_30) {
            Set res_s_26;
            if (((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).Is(vdm_TRL_uTAR_uTYPE_TYPE)) 
              res_s_26 = ((Record) vdm_TRL_uTAR_uTYPE_stp[vdm_TRL_uTAR_uTYPE_i.GetValue()]).GetField(pos_TRL_uTAR_uTYPE_TYPE_fv);
            else 
              RunTime("Unknown record field selector");
            res_s_25.Insert(res_s_26);
          }
        }
      }
    }
    unArg_24 = res_s_25;
    Set rduset_39;    Generic tmpe_42;    for (int bb_41 = unArg_24.First(tmpe_42); bb_41; bb_41 = unArg_24.Next(tmpe_42)) {
      Set e_40 = tmpe_42;
      rduset_39.ImpUnion(e_40);
    }    vdm_TRL_uTAR_uTYPE_fv = rduset_39;
    varRes_4 = Record(vdm_TRL_uTAR_uTYPE_TYPE, length_TRL_uTAR_uTYPE_TYPE);
    varRes_4.SetField(1, vdm_TRL_uTAR_uTYPE_tp);
    varRes_4.SetField(2, vdm_TRL_uTAR_uTYPE_fv);
  }
  return (Generic) varRes_4;
}
#endif

