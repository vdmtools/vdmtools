//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Sat 06-Nov-99 by the VDM-SLtoC++ Code Generator
// (v3.3 - Mon 22-Mar-99)
//
// Supported compilers:
// 	egcs version 1.1 on SunOS4, SunOS5, Linux, HP-UX10
// 	VC++ version 6.0 on Windows95, Windows NT
//



#include "TRF_uTRA_uDEF.h"
static TYPE_TRF_uTAR_uDEF_Document vdm_TRF_uTRA_uDEF_trv_uDocument(const TYPE_AS_Document &);
static TYPE_TRF_uTAR_uDEF_ExplFnDef vdm_TRF_uTRA_uDEF_trv_uExplFunction(const TYPE_AS_ExplFnDef &);
static TYPE_TRF_uTAR_uDEF_ExtExplFnDef vdm_TRF_uTRA_uDEF_trv_uExtExplFunction(const TYPE_AS_ExtExplFnDef &);
static TYPE_TRF_uTAR_uDEF_FnBody vdm_TRF_uTRA_uDEF_trv_uFnBody(const TYPE_AS_FnBody &);
static TYPE_TRF_uTAR_uDEF_ImplFnDef vdm_TRF_uTRA_uDEF_trv_uImplFunction(const TYPE_AS_ImplFnDef &);
static TYPE_TRF_uTAR_uDEF_ImplModule vdm_TRF_uTRA_uDEF_trv_uImplModule(const TYPE_AS_ImplModule &);
static TYPE_TRF_uTAR_uDEF_Interface vdm_TRF_uTRA_uDEF_trv_uInterface(const TYPE_AS_Interface &);
static TYPE_TRF_uTAR_uDEF_Invariant vdm_TRF_uTRA_uDEF_trv_uInvariant(const TYPE_AS_Invariant &);
static TYPE_TRF_uTAR_uDEF_NameType vdm_TRF_uTRA_uDEF_trv_uNameType(const TYPE_AS_NameType &);
static TYPE_TRF_uTAR_uDEF_ParameterTypes vdm_TRF_uTRA_uDEF_trv_uParameterTypes(const TYPE_AS_ParameterTypes &);
static TYPE_TRF_uTAR_uDEF_Parameters vdm_TRF_uTRA_uDEF_trv_uParameters(const TYPE_AS_Parameters &);
static TYPE_TRF_uTAR_uDEF_ParametersList vdm_TRF_uTRA_uDEF_trv_uParametersList(const TYPE_AS_ParametersList &);
static TYPE_TRF_uTAR_uDEF_PatTypePair vdm_TRF_uTRA_uDEF_trv_uPatTypePair(const TYPE_AS_PatTypePair &);
static TYPE_TRF_uTAR_uDEF_TypeVarList vdm_TRF_uTRA_uDEF_trv_uTypeVarList(const TYPE_AS_TypeVarList &);
static TYPE_TRF_uTAR_uDEF_ValueDef vdm_TRF_uTRA_uDEF_trv_uValueDef(const TYPE_AS_ValueDef &);

void init_TRF_uTRA_uDEF_VDMLib() {
  VDMGetDefaultRecInfoMap().NewTag(TOKEN, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TOKEN, "token");
}
#ifdef DEF_TRF_uTRA_uDEF_USERIMPL

#include "TRF_uTRA_uDEF_userimpl.cc"

#endif


void init_TRF_uTRA_uDEF() { init_TRF_uTRA_uDEF_VDMLib(); }
#ifndef DEF_TRF_uTRA_uDEF_trv_uAstCI

TYPE_TRF_uTAR_uDEF_AstCI vdm_TRF_uTRA_uDEF_trv_uAstCI(const TYPE_AS_AstCI &var_1_1) {
  Generic vdm_TRF_uTRA_uDEF_ast;
  Map vdm_TRF_uTRA_uDEF_fidm;
  Sequence vdm_TRF_uTRA_uDEF_tllci;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_AstCI == var_1_1.GetTag()) {
    Sequence e_l_14;
    for (int i_15 = 1; i_15 <= var_1_1.Length(); i_15++) 
      e_l_14.ImpAppend(var_1_1.GetField(i_15));
    if (succ_2 = 3 == (Int) e_l_14.Length()) {
      vdm_TRF_uTRA_uDEF_ast = (Generic) e_l_14.Hd();
      vdm_TRF_uTRA_uDEF_tllci = (Generic) e_l_14[2];
      vdm_TRF_uTRA_uDEF_fidm = (Generic) e_l_14[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uAstCI failed");
  Record varRes_3(vdm_AS_AstCI, length_AS_AstCI);
  {
    Generic vdm_TRF_uTRA_uDEF_trv_uast;
    bool succ_6 = true;
    {
      succ_6 = true;
      if (!vdm_TRF_uTRA_uDEF_ast.IsSequence()) 
        succ_6 = false;
      if (succ_6) 
        vdm_TRF_uTRA_uDEF_trv_uast = vdm_TRF_uTRA_uDEF_trv_uDocument((Generic) vdm_TRF_uTRA_uDEF_ast);
    }
    if (!succ_6) 
      vdm_TRF_uTRA_uDEF_trv_uast = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_ast);
    varRes_3 = vdm_TRF_uTAR_uDEF_tar_uAstCI((Generic) vdm_TRF_uTRA_uDEF_trv_uast, (Generic) vdm_TRF_uTRA_uDEF_tllci, (Generic) vdm_TRF_uTRA_uDEF_fidm);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uDefinitions

TYPE_TRF_uTAR_uDEF_Definitions vdm_TRF_uTRA_uDEF_trv_uDefinitions(const TYPE_AS_Definitions &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Map vdm_TRF_uTRA_uDEF_fnm;
  Map vdm_TRF_uTRA_uDEF_opm;
  Generic vdm_TRF_uTRA_uDEF_stt;
  Map vdm_TRF_uTRA_uDEF_typem;
  Sequence vdm_TRF_uTRA_uDEF_valuem;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_Definitions == var_1_1.GetTag()) {
    Sequence e_l_41;
    for (int i_42 = 1; i_42 <= var_1_1.Length(); i_42++) 
      e_l_41.ImpAppend(var_1_1.GetField(i_42));
    if (succ_2 = 6 == (Int) e_l_41.Length()) {
      vdm_TRF_uTRA_uDEF_typem = (Generic) e_l_41.Hd();
      vdm_TRF_uTRA_uDEF_valuem = (Generic) e_l_41[2];
      vdm_TRF_uTRA_uDEF_fnm = (Generic) e_l_41[3];
      vdm_TRF_uTRA_uDEF_opm = (Generic) e_l_41[4];
      vdm_TRF_uTRA_uDEF_stt = (Generic) e_l_41[5];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_41[6];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uDefinitions failed");
  Record varRes_3(vdm_AS_Definitions, length_AS_Definitions);
  {
    Map vdm_TRF_uTRA_uDEF_trv_utypem;
    Map res_m_6;
    {
      bool succ_12 = true;
      Set e1_set_15 = vdm_TRF_uTRA_uDEF_typem.Dom();
      Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
      {
        Generic tmpe_18;
        for (int bb_17 = e1_set_15.First(tmpe_18); bb_17; bb_17 = e1_set_15.Next(tmpe_18)) {
          Record elem_16 = tmpe_18;
          succ_12 = true;          vdm_TRF_uTRA_uDEF_nm = elem_16;          if (succ_12) 
            res_m_6.Insert(vdm_TRF_uTRA_uDEF_nm, vdm_TRF_uTRA_uDEF_trv_uTypeDef((Generic) (Record) vdm_TRF_uTRA_uDEF_typem[(Generic) vdm_TRF_uTRA_uDEF_nm]));
        }
      }
    }
    vdm_TRF_uTRA_uDEF_trv_utypem = res_m_6;
    Map vdm_TRF_uTRA_uDEF_trv_ufnm;
    Map res_m_20;
    {
      bool succ_26 = true;
      Set e1_set_29 = vdm_TRF_uTRA_uDEF_fnm.Dom();
      Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
      {
        Generic tmpe_32;
        for (int bb_31 = e1_set_29.First(tmpe_32); bb_31; bb_31 = e1_set_29.Next(tmpe_32)) {
          Record elem_30 = tmpe_32;
          succ_26 = true;          vdm_TRF_uTRA_uDEF_nm = elem_30;          if (succ_26) 
            res_m_20.Insert(vdm_TRF_uTRA_uDEF_nm, vdm_TRF_uTRA_uDEF_trv_uFnDef((Generic) (Record) vdm_TRF_uTRA_uDEF_fnm[(Generic) vdm_TRF_uTRA_uDEF_nm]));
        }
      }
    }
    vdm_TRF_uTRA_uDEF_trv_ufnm = res_m_20;
    Sequence vdm_TRF_uTRA_uDEF_trv_uvaluem = vdm_TRF_uTRA_uDEF_trv_uGlobalValueDef((Generic) vdm_TRF_uTRA_uDEF_valuem);
    varRes_3 = vdm_TRF_uTAR_uDEF_tar_uDefinitions((Generic) vdm_TRF_uTRA_uDEF_trv_utypem, (Generic) vdm_TRF_uTRA_uDEF_trv_uvaluem, (Generic) vdm_TRF_uTRA_uDEF_trv_ufnm, (Generic) vdm_TRF_uTRA_uDEF_opm, (Generic) vdm_TRF_uTRA_uDEF_stt, (Generic) vdm_TRF_uTRA_uDEF_cid);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uDocument

TYPE_TRF_uTAR_uDEF_Document vdm_TRF_uTRA_uDEF_trv_uDocument(const TYPE_AS_Document &vdm_TRF_uTRA_uDEF_doc) {
  Sequence varRes_3;
  {
    Sequence vdm_TRF_uTRA_uDEF_trv_udoc;
    {
      Sequence res_l_6;
      Set resBind_s_8;
      Set riseq_12;
      int max_13 = (Int) vdm_TRF_uTRA_uDEF_doc.Length();
      for (int i_14 = 1; i_14 <= max_13; i_14++) 
        riseq_12.Insert((Int) i_14);
      resBind_s_8 = riseq_12;
      Sequence bind_l_7;
      if ((Int) resBind_s_8.Card() != (Int) 0) 
        bind_l_7 = Sort(resBind_s_8);
      bool succ_9 = true;
      Int vdm_TRF_uTRA_uDEF_i;
      Generic tmpe_37;
      for (int i_36 = bind_l_7.First(tmpe_37); i_36; i_36 = bind_l_7.Next(tmpe_37)) {
        Int e_10 = tmpe_37;
        vdm_TRF_uTRA_uDEF_i = e_10;
        if (succ_9) {
          Record reselem_15;
          bool succ_16 = true;
          {
            succ_16 = true;
            if (((Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).IsRecord() && vdm_AS_Definitions == ((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).GetTag()) {
              Sequence e_l_21;
              for (int i_22 = 1; i_22 <= ((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).Length(); i_22++) 
                e_l_21.ImpAppend(((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).GetField(i_22));
              if (succ_16 = 6 == (Int) e_l_21.Length()) {}
            }
            else 
              succ_16 = false;
            if (succ_16) 
              reselem_15 = vdm_TRF_uTRA_uDEF_trv_uDefinitions((Generic) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]);
          }
          if (!succ_16) {
            succ_16 = true;
            if (((Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).IsRecord() && vdm_AS_Module == ((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).GetTag()) {
              Sequence e_l_26;
              for (int i_27 = 1; i_27 <= ((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).Length(); i_27++) 
                e_l_26.ImpAppend(((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).GetField(i_27));
              if (succ_16 = 4 == (Int) e_l_26.Length()) {}
            }
            else 
              succ_16 = false;
            if (succ_16) 
              reselem_15 = vdm_TRF_uTRA_uDEF_trv_uModule((Generic) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]);
          }
          if (!succ_16) {
            succ_16 = true;
            if (((Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).IsRecord() && vdm_AS_ImplModule == ((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).GetTag()) {
              Sequence e_l_31;
              for (int i_32 = 1; i_32 <= ((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).Length(); i_32++) 
                e_l_31.ImpAppend(((Record) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]).GetField(i_32));
              if (succ_16 = 4 == (Int) e_l_31.Length()) {}
            }
            else 
              succ_16 = false;
            if (succ_16) 
              reselem_15 = vdm_TRF_uTRA_uDEF_trv_uImplModule((Generic) (Record) vdm_TRF_uTRA_uDEF_doc[vdm_TRF_uTRA_uDEF_i.GetValue()]);
          }
          if (!succ_16) 
            RunTime("No \'others\' branch in CasesExpr");
          res_l_6.ImpAppend(reselem_15);
        }
      }
      vdm_TRF_uTRA_uDEF_trv_udoc = res_l_6;
    }
    varRes_3 = vdm_TRF_uTAR_uDEF_tar_uDocument((Generic) vdm_TRF_uTRA_uDEF_trv_udoc);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uExplFunction

TYPE_TRF_uTAR_uDEF_ExplFnDef vdm_TRF_uTRA_uDEF_trv_uExplFunction(const TYPE_AS_ExplFnDef &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Record vdm_TRF_uTRA_uDEF_fnbody(vdm_AS_FnBody, length_AS_FnBody);
  Generic vdm_TRF_uTRA_uDEF_fnpost;
  Generic vdm_TRF_uTRA_uDEF_fnpre;
  Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
  Sequence vdm_TRF_uTRA_uDEF_parms;
  Record vdm_TRF_uTRA_uDEF_tp;
  Sequence vdm_TRF_uTRA_uDEF_tv_ul;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_ExplFnDef == var_1_1.GetTag()) {
    Sequence e_l_27;
    for (int i_28 = 1; i_28 <= var_1_1.Length(); i_28++) 
      e_l_27.ImpAppend(var_1_1.GetField(i_28));
    if (succ_2 = 8 == (Int) e_l_27.Length()) {
      vdm_TRF_uTRA_uDEF_nm = (Generic) e_l_27.Hd();
      vdm_TRF_uTRA_uDEF_tv_ul = (Generic) e_l_27[2];
      vdm_TRF_uTRA_uDEF_tp = (Generic) e_l_27[3];
      vdm_TRF_uTRA_uDEF_parms = (Generic) e_l_27[4];
      vdm_TRF_uTRA_uDEF_fnbody = (Generic) e_l_27[5];
      vdm_TRF_uTRA_uDEF_fnpre = (Generic) e_l_27[6];
      vdm_TRF_uTRA_uDEF_fnpost = (Generic) e_l_27[7];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_27[8];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uExplFunction failed");
  Record varRes_3(vdm_AS_ExplFnDef, length_AS_ExplFnDef);
  Generic par_14;
  if (((Bool) (vdm_TRF_uTRA_uDEF_fnpre == Nil())).GetValue()) 
    par_14 = Nil();
  else 
    par_14 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_fnpre);
  Generic par_20;
  if (((Bool) (vdm_TRF_uTRA_uDEF_fnpost == Nil())).GetValue()) 
    par_20 = Nil();
  else 
    par_20 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_fnpost);
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uExplFunction((Generic) vdm_TRF_uTRA_uEXPR_trv_uNameName((Generic) vdm_TRF_uTRA_uDEF_nm), (Generic) vdm_TRF_uTRA_uDEF_trv_uTypeVarList((Generic) vdm_TRF_uTRA_uDEF_tv_ul), (Generic) vdm_TRF_uTRA_uTYPE_trv_uFnType((Generic) vdm_TRF_uTRA_uDEF_tp), (Generic) vdm_TRF_uTRA_uDEF_trv_uParametersList((Generic) vdm_TRF_uTRA_uDEF_parms), (Generic) vdm_TRF_uTRA_uDEF_trv_uFnBody((Generic) vdm_TRF_uTRA_uDEF_fnbody), (Generic) par_14, (Generic) par_20, (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uExtExplFunction

TYPE_TRF_uTAR_uDEF_ExtExplFnDef vdm_TRF_uTRA_uDEF_trv_uExtExplFunction(const TYPE_AS_ExtExplFnDef &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Record vdm_TRF_uTRA_uDEF_fnbody(vdm_AS_FnBody, length_AS_FnBody);
  Generic vdm_TRF_uTRA_uDEF_fnpost;
  Generic vdm_TRF_uTRA_uDEF_fnpre;
  Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
  Sequence vdm_TRF_uTRA_uDEF_partps;
  Sequence vdm_TRF_uTRA_uDEF_resnmtps;
  Sequence vdm_TRF_uTRA_uDEF_tv_ul;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_ExtExplFnDef == var_1_1.GetTag()) {
    Sequence e_l_41;
    for (int i_42 = 1; i_42 <= var_1_1.Length(); i_42++) 
      e_l_41.ImpAppend(var_1_1.GetField(i_42));
    if (succ_2 = 8 == (Int) e_l_41.Length()) {
      vdm_TRF_uTRA_uDEF_nm = (Generic) e_l_41.Hd();
      vdm_TRF_uTRA_uDEF_tv_ul = (Generic) e_l_41[2];
      vdm_TRF_uTRA_uDEF_partps = (Generic) e_l_41[3];
      vdm_TRF_uTRA_uDEF_resnmtps = (Generic) e_l_41[4];
      vdm_TRF_uTRA_uDEF_fnbody = (Generic) e_l_41[5];
      vdm_TRF_uTRA_uDEF_fnpre = (Generic) e_l_41[6];
      vdm_TRF_uTRA_uDEF_fnpost = (Generic) e_l_41[7];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_41[8];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uExtExplFunction failed");
  Record varRes_3(vdm_AS_ExplFnDef, length_AS_ExplFnDef);
  Sequence par_10;
  {
    Sequence res_l_11;
    Set resBind_s_13;
    Set riseq_17;
    int max_18 = (Int) vdm_TRF_uTRA_uDEF_resnmtps.Length();
    for (int i_19 = 1; i_19 <= max_18; i_19++) 
      riseq_17.Insert((Int) i_19);
    resBind_s_13 = riseq_17;
    Sequence bind_l_12;
    if ((Int) resBind_s_13.Card() != (Int) 0) 
      bind_l_12 = Sort(resBind_s_13);
    bool succ_14 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_25;
    for (int i_24 = bind_l_12.First(tmpe_25); i_24; i_24 = bind_l_12.Next(tmpe_25)) {
      Int e_15 = tmpe_25;
      vdm_TRF_uTRA_uDEF_i = e_15;
      if (succ_14) 
        res_l_11.ImpAppend(vdm_TRF_uTRA_uDEF_trv_uNameType((Generic) (Record) vdm_TRF_uTRA_uDEF_resnmtps[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    par_10 = res_l_11;
  }
  Generic par_28;
  if (((Bool) (vdm_TRF_uTRA_uDEF_fnpre == Nil())).GetValue()) 
    par_28 = Nil();
  else 
    par_28 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_fnpre);
  Generic par_34;
  if (((Bool) (vdm_TRF_uTRA_uDEF_fnpost == Nil())).GetValue()) 
    par_34 = Nil();
  else 
    par_34 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_fnpost);
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uExtExplFunction((Generic) vdm_TRF_uTRA_uEXPR_trv_uNameName((Generic) vdm_TRF_uTRA_uDEF_nm), (Generic) vdm_TRF_uTRA_uDEF_trv_uTypeVarList((Generic) vdm_TRF_uTRA_uDEF_tv_ul), (Generic) vdm_TRF_uTRA_uDEF_trv_uParameterTypes((Generic) vdm_TRF_uTRA_uDEF_partps), (Generic) par_10, (Generic) vdm_TRF_uTRA_uDEF_trv_uFnBody((Generic) vdm_TRF_uTRA_uDEF_fnbody), (Generic) par_28, (Generic) par_34, (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uFnBody

TYPE_TRF_uTAR_uDEF_FnBody vdm_TRF_uTRA_uDEF_trv_uFnBody(const TYPE_AS_FnBody &var_1_1) {
  Generic vdm_TRF_uTRA_uDEF_body;
  Int vdm_TRF_uTRA_uDEF_cid;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_FnBody == var_1_1.GetTag()) {
    Sequence e_l_11;
    for (int i_12 = 1; i_12 <= var_1_1.Length(); i_12++) 
      e_l_11.ImpAppend(var_1_1.GetField(i_12));
    if (succ_2 = 2 == (Int) e_l_11.Length()) {
      vdm_TRF_uTRA_uDEF_body = (Generic) e_l_11.Hd();
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_11[2];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uFnBody failed");
  Record varRes_3(vdm_TRF_uTAR_uDEF_FnBody, length_TRF_uTAR_uDEF_FnBody);
  Generic par_4;
  if (((Bool) (vdm_TRF_uTRA_uDEF_body == (Quote) "NOTYETSPEC")).GetValue()) 
    par_4 = (Quote) "NOTYETSPEC";
  else 
    par_4 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_body);
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uFnBody((Generic) par_4, (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uFnDef

TYPE_TRF_uTAR_uDEF_FnDef vdm_TRF_uTRA_uDEF_trv_uFnDef(const TYPE_AS_FnDef &vdm_TRF_uTRA_uDEF_fndef) {
  Record varRes_3(vdm_AS_ExplFnDef, length_AS_ExplFnDef);
  bool succ_4 = true;
  {
    succ_4 = true;
    if (vdm_TRF_uTRA_uDEF_fndef.IsRecord() && vdm_AS_ExplFnDef == ((Record) vdm_TRF_uTRA_uDEF_fndef).GetTag()) {
      Sequence e_l_7;
      for (int i_8 = 1; i_8 <= ((Record) vdm_TRF_uTRA_uDEF_fndef).Length(); i_8++) 
        e_l_7.ImpAppend(((Record) vdm_TRF_uTRA_uDEF_fndef).GetField(i_8));
      if (succ_4 = 8 == (Int) e_l_7.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_TRF_uTRA_uDEF_trv_uExplFunction((Generic) vdm_TRF_uTRA_uDEF_fndef);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_TRF_uTRA_uDEF_fndef.IsRecord() && vdm_AS_ImplFnDef == ((Record) vdm_TRF_uTRA_uDEF_fndef).GetTag()) {
      Sequence e_l_10;
      for (int i_11 = 1; i_11 <= ((Record) vdm_TRF_uTRA_uDEF_fndef).Length(); i_11++) 
        e_l_10.ImpAppend(((Record) vdm_TRF_uTRA_uDEF_fndef).GetField(i_11));
      if (succ_4 = 7 == (Int) e_l_10.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_TRF_uTRA_uDEF_trv_uImplFunction((Generic) vdm_TRF_uTRA_uDEF_fndef);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_TRF_uTRA_uDEF_fndef.IsRecord() && vdm_AS_ExtExplFnDef == ((Record) vdm_TRF_uTRA_uDEF_fndef).GetTag()) {
      Sequence e_l_13;
      for (int i_14 = 1; i_14 <= ((Record) vdm_TRF_uTRA_uDEF_fndef).Length(); i_14++) 
        e_l_13.ImpAppend(((Record) vdm_TRF_uTRA_uDEF_fndef).GetField(i_14));
      if (succ_4 = 8 == (Int) e_l_13.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_TRF_uTRA_uDEF_trv_uExtExplFunction((Generic) vdm_TRF_uTRA_uDEF_fndef);
  }
  if (!succ_4) 
    RunTime("No \'others\' branch in CasesExpr");
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uGlobalValueDef

type_11AS_ValueDefCL vdm_TRF_uTRA_uDEF_trv_uGlobalValueDef(const type_11AS_ValueDefCL &vdm_TRF_uTRA_uDEF_vals) {
  Sequence varRes_3;
  Sequence par_4;
  {
    Sequence res_l_5;
    Set resBind_s_7;
    Set riseq_11;
    int max_12 = (Int) vdm_TRF_uTRA_uDEF_vals.Length();
    for (int i_13 = 1; i_13 <= max_12; i_13++) 
      riseq_11.Insert((Int) i_13);
    resBind_s_7 = riseq_11;
    Sequence bind_l_6;
    if ((Int) resBind_s_7.Card() != (Int) 0) 
      bind_l_6 = Sort(resBind_s_7);
    bool succ_8 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_19;
    for (int i_18 = bind_l_6.First(tmpe_19); i_18; i_18 = bind_l_6.Next(tmpe_19)) {
      Int e_9 = tmpe_19;
      vdm_TRF_uTRA_uDEF_i = e_9;
      if (succ_8) 
        res_l_5.ImpAppend(vdm_TRF_uTRA_uDEF_trv_uValueDef((Generic) (Record) vdm_TRF_uTRA_uDEF_vals[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    par_4 = res_l_5;
  }
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uGlobalValueDef((Generic) par_4);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uImplFunction

TYPE_TRF_uTAR_uDEF_ImplFnDef vdm_TRF_uTRA_uDEF_trv_uImplFunction(const TYPE_AS_ImplFnDef &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Record vdm_TRF_uTRA_uDEF_fnpost;
  Generic vdm_TRF_uTRA_uDEF_fnpre;
  Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
  Sequence vdm_TRF_uTRA_uDEF_partps;
  Sequence vdm_TRF_uTRA_uDEF_resnmtps;
  Sequence vdm_TRF_uTRA_uDEF_tv_ul;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_ImplFnDef == var_1_1.GetTag()) {
    Sequence e_l_35;
    for (int i_36 = 1; i_36 <= var_1_1.Length(); i_36++) 
      e_l_35.ImpAppend(var_1_1.GetField(i_36));
    if (succ_2 = 7 == (Int) e_l_35.Length()) {
      vdm_TRF_uTRA_uDEF_nm = (Generic) e_l_35.Hd();
      vdm_TRF_uTRA_uDEF_tv_ul = (Generic) e_l_35[2];
      vdm_TRF_uTRA_uDEF_partps = (Generic) e_l_35[3];
      vdm_TRF_uTRA_uDEF_resnmtps = (Generic) e_l_35[4];
      vdm_TRF_uTRA_uDEF_fnpre = (Generic) e_l_35[5];
      vdm_TRF_uTRA_uDEF_fnpost = (Generic) e_l_35[6];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_35[7];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uImplFunction failed");
  Record varRes_3(vdm_AS_ExplFnDef, length_AS_ExplFnDef);
  Sequence par_10;
  {
    Sequence res_l_11;
    Set resBind_s_13;
    Set riseq_17;
    int max_18 = (Int) vdm_TRF_uTRA_uDEF_resnmtps.Length();
    for (int i_19 = 1; i_19 <= max_18; i_19++) 
      riseq_17.Insert((Int) i_19);
    resBind_s_13 = riseq_17;
    Sequence bind_l_12;
    if ((Int) resBind_s_13.Card() != (Int) 0) 
      bind_l_12 = Sort(resBind_s_13);
    bool succ_14 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_25;
    for (int i_24 = bind_l_12.First(tmpe_25); i_24; i_24 = bind_l_12.Next(tmpe_25)) {
      Int e_15 = tmpe_25;
      vdm_TRF_uTRA_uDEF_i = e_15;
      if (succ_14) 
        res_l_11.ImpAppend(vdm_TRF_uTRA_uDEF_trv_uNameType((Generic) (Record) vdm_TRF_uTRA_uDEF_resnmtps[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    par_10 = res_l_11;
  }
  Generic par_26;
  if (((Bool) (vdm_TRF_uTRA_uDEF_fnpre == Nil())).GetValue()) 
    par_26 = Nil();
  else 
    par_26 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_fnpre);
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uImplFunction((Generic) vdm_TRF_uTRA_uEXPR_trv_uNameName((Generic) vdm_TRF_uTRA_uDEF_nm), (Generic) vdm_TRF_uTRA_uDEF_trv_uTypeVarList((Generic) vdm_TRF_uTRA_uDEF_tv_ul), (Generic) vdm_TRF_uTRA_uDEF_trv_uParameterTypes((Generic) vdm_TRF_uTRA_uDEF_partps), (Generic) par_10, (Generic) par_26, (Generic) vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_fnpost), (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uImplModule

TYPE_TRF_uTAR_uDEF_ImplModule vdm_TRF_uTRA_uDEF_trv_uImplModule(const TYPE_AS_ImplModule &var_1_1) {
  bool succ_2 = true;
  if (succ_2) {}
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uImplModule failed");
  Generic varRes_3;
  RunTime("Can not evaluate an undefined expression");
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uInterface

TYPE_TRF_uTAR_uDEF_Interface vdm_TRF_uTRA_uDEF_trv_uInterface(const TYPE_AS_Interface &vdm_TRF_uTRA_uDEF_intf) { return (Generic) vdm_TRF_uTRA_uDEF_intf; }
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uInvariant

TYPE_TRF_uTAR_uDEF_Invariant vdm_TRF_uTRA_uDEF_trv_uInvariant(const TYPE_AS_Invariant &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Record vdm_TRF_uTRA_uDEF_expr;
  Record vdm_TRF_uTRA_uDEF_pat;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_Invariant == var_1_1.GetTag()) {
    Sequence e_l_9;
    for (int i_10 = 1; i_10 <= var_1_1.Length(); i_10++) 
      e_l_9.ImpAppend(var_1_1.GetField(i_10));
    if (succ_2 = 3 == (Int) e_l_9.Length()) {
      vdm_TRF_uTRA_uDEF_pat = (Generic) e_l_9.Hd();
      vdm_TRF_uTRA_uDEF_expr = (Generic) e_l_9[2];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_9[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uInvariant failed");
  return (Generic) vdm_TRF_uTAR_uDEF_tar_uInvariant((Generic) vdm_TRF_uTRA_uPAT_trv_uPattern((Generic) vdm_TRF_uTRA_uDEF_pat), (Generic) vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_expr), (Generic) vdm_TRF_uTRA_uDEF_cid);
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uLocalDef

TYPE_TRF_uTAR_uDEF_LocalDef vdm_TRF_uTRA_uDEF_trv_uLocalDef(const TYPE_AS_LocalDef &vdm_TRF_uTRA_uDEF_val) {
  Record varRes_3;
  bool succ_4 = true;
  {
    succ_4 = true;
    if (vdm_TRF_uTRA_uDEF_val.IsRecord() && vdm_AS_ValueDef == ((Record) vdm_TRF_uTRA_uDEF_val).GetTag()) {
      Sequence e_l_7;
      for (int i_8 = 1; i_8 <= ((Record) vdm_TRF_uTRA_uDEF_val).Length(); i_8++) 
        e_l_7.ImpAppend(((Record) vdm_TRF_uTRA_uDEF_val).GetField(i_8));
      if (succ_4 = 4 == (Int) e_l_7.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_TRF_uTRA_uDEF_trv_uValueDef((Generic) vdm_TRF_uTRA_uDEF_val);
  }
  if (!succ_4) 
    varRes_3 = vdm_TRF_uTRA_uDEF_trv_uFnDef((Generic) vdm_TRF_uTRA_uDEF_val);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uModule

TYPE_TRF_uTAR_uDEF_Module vdm_TRF_uTRA_uDEF_trv_uModule(const TYPE_AS_Module &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Generic vdm_TRF_uTRA_uDEF_defs;
  Record vdm_TRF_uTRA_uDEF_intf(vdm_AS_Interface, length_AS_Interface);
  Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_Module == var_1_1.GetTag()) {
    Sequence e_l_15;
    for (int i_16 = 1; i_16 <= var_1_1.Length(); i_16++) 
      e_l_15.ImpAppend(var_1_1.GetField(i_16));
    if (succ_2 = 4 == (Int) e_l_15.Length()) {
      vdm_TRF_uTRA_uDEF_nm = (Generic) e_l_15.Hd();
      vdm_TRF_uTRA_uDEF_intf = (Generic) e_l_15[2];
      vdm_TRF_uTRA_uDEF_defs = (Generic) e_l_15[3];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_15[4];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uModule failed");
  Record varRes_3(vdm_AS_Module, length_AS_Module);
  Generic par_8;
  if (((Bool) (vdm_TRF_uTRA_uDEF_defs == Nil())).GetValue()) 
    par_8 = Nil();
  else 
    par_8 = vdm_TRF_uTRA_uDEF_trv_uDefinitions((Generic) vdm_TRF_uTRA_uDEF_defs);
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uModule((Generic) vdm_TRF_uTRA_uEXPR_trv_uNameName((Generic) vdm_TRF_uTRA_uDEF_nm), (Generic) vdm_TRF_uTRA_uDEF_trv_uInterface((Generic) vdm_TRF_uTRA_uDEF_intf), (Generic) par_8, (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uNameType

TYPE_TRF_uTAR_uDEF_NameType vdm_TRF_uTRA_uDEF_trv_uNameType(const TYPE_AS_NameType &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
  Record vdm_TRF_uTRA_uDEF_tp;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_NameType == var_1_1.GetTag()) {
    Sequence e_l_9;
    for (int i_10 = 1; i_10 <= var_1_1.Length(); i_10++) 
      e_l_9.ImpAppend(var_1_1.GetField(i_10));
    if (succ_2 = 3 == (Int) e_l_9.Length()) {
      vdm_TRF_uTRA_uDEF_nm = (Generic) e_l_9.Hd();
      vdm_TRF_uTRA_uDEF_tp = (Generic) e_l_9[2];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_9[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uNameType failed");
  return (Generic) vdm_TRF_uTAR_uDEF_tar_uNameType((Generic) vdm_TRF_uTRA_uEXPR_trv_uNameName((Generic) vdm_TRF_uTRA_uDEF_nm), (Generic) vdm_TRF_uTRA_uTYPE_trv_uType((Generic) vdm_TRF_uTRA_uDEF_tp), (Generic) vdm_TRF_uTRA_uDEF_cid);
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uParameterTypes

TYPE_TRF_uTAR_uDEF_ParameterTypes vdm_TRF_uTRA_uDEF_trv_uParameterTypes(const TYPE_AS_ParameterTypes &vdm_TRF_uTRA_uDEF_parmtps) {
  Sequence varRes_3;
  {
    Sequence res_l_4;
    Set resBind_s_6;
    Set riseq_10;
    int max_11 = (Int) vdm_TRF_uTRA_uDEF_parmtps.Length();
    for (int i_12 = 1; i_12 <= max_11; i_12++) 
      riseq_10.Insert((Int) i_12);
    resBind_s_6 = riseq_10;
    Sequence bind_l_5;
    if ((Int) resBind_s_6.Card() != (Int) 0) 
      bind_l_5 = Sort(resBind_s_6);
    bool succ_7 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_18;
    for (int i_17 = bind_l_5.First(tmpe_18); i_17; i_17 = bind_l_5.Next(tmpe_18)) {
      Int e_8 = tmpe_18;
      vdm_TRF_uTRA_uDEF_i = e_8;
      if (succ_7) 
        res_l_4.ImpAppend(vdm_TRF_uTRA_uDEF_trv_uPatTypePair((Generic) (Record) vdm_TRF_uTRA_uDEF_parmtps[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    varRes_3 = res_l_4;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uParameters

TYPE_TRF_uTAR_uDEF_Parameters vdm_TRF_uTRA_uDEF_trv_uParameters(const TYPE_AS_Parameters &vdm_TRF_uTRA_uDEF_parms) {
  Sequence varRes_3;
  {
    Sequence res_l_4;
    Set resBind_s_6;
    Set riseq_10;
    int max_11 = (Int) vdm_TRF_uTRA_uDEF_parms.Length();
    for (int i_12 = 1; i_12 <= max_11; i_12++) 
      riseq_10.Insert((Int) i_12);
    resBind_s_6 = riseq_10;
    Sequence bind_l_5;
    if ((Int) resBind_s_6.Card() != (Int) 0) 
      bind_l_5 = Sort(resBind_s_6);
    bool succ_7 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_18;
    for (int i_17 = bind_l_5.First(tmpe_18); i_17; i_17 = bind_l_5.Next(tmpe_18)) {
      Int e_8 = tmpe_18;
      vdm_TRF_uTRA_uDEF_i = e_8;
      if (succ_7) 
        res_l_4.ImpAppend(vdm_TRF_uTRA_uPAT_trv_uPattern((Generic) (Record) vdm_TRF_uTRA_uDEF_parms[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    varRes_3 = res_l_4;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uParametersList

TYPE_TRF_uTAR_uDEF_ParametersList vdm_TRF_uTRA_uDEF_trv_uParametersList(const TYPE_AS_ParametersList &vdm_TRF_uTRA_uDEF_parms) {
  Sequence varRes_3;
  {
    Sequence res_l_4;
    Set resBind_s_6;
    Set riseq_10;
    int max_11 = (Int) vdm_TRF_uTRA_uDEF_parms.Length();
    for (int i_12 = 1; i_12 <= max_11; i_12++) 
      riseq_10.Insert((Int) i_12);
    resBind_s_6 = riseq_10;
    Sequence bind_l_5;
    if ((Int) resBind_s_6.Card() != (Int) 0) 
      bind_l_5 = Sort(resBind_s_6);
    bool succ_7 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_18;
    for (int i_17 = bind_l_5.First(tmpe_18); i_17; i_17 = bind_l_5.Next(tmpe_18)) {
      Int e_8 = tmpe_18;
      vdm_TRF_uTRA_uDEF_i = e_8;
      if (succ_7) 
        res_l_4.ImpAppend(vdm_TRF_uTRA_uDEF_trv_uParameters((Generic) (Sequence) vdm_TRF_uTRA_uDEF_parms[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    varRes_3 = res_l_4;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uPatTypePair

TYPE_TRF_uTAR_uDEF_PatTypePair vdm_TRF_uTRA_uDEF_trv_uPatTypePair(const TYPE_AS_PatTypePair &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Sequence vdm_TRF_uTRA_uDEF_pats;
  Record vdm_TRF_uTRA_uDEF_tp;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_PatTypePair == var_1_1.GetTag()) {
    Sequence e_l_23;
    for (int i_24 = 1; i_24 <= var_1_1.Length(); i_24++) 
      e_l_23.ImpAppend(var_1_1.GetField(i_24));
    if (succ_2 = 3 == (Int) e_l_23.Length()) {
      vdm_TRF_uTRA_uDEF_pats = (Generic) e_l_23.Hd();
      vdm_TRF_uTRA_uDEF_tp = (Generic) e_l_23[2];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_23[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uPatTypePair failed");
  Sequence varRes_3;
  Sequence par_4;
  {
    Sequence res_l_5;
    Set resBind_s_7;
    Set riseq_11;
    int max_12 = (Int) vdm_TRF_uTRA_uDEF_pats.Length();
    for (int i_13 = 1; i_13 <= max_12; i_13++) 
      riseq_11.Insert((Int) i_13);
    resBind_s_7 = riseq_11;
    Sequence bind_l_6;
    if ((Int) resBind_s_7.Card() != (Int) 0) 
      bind_l_6 = Sort(resBind_s_7);
    bool succ_8 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_19;
    for (int i_18 = bind_l_6.First(tmpe_19); i_18; i_18 = bind_l_6.Next(tmpe_19)) {
      Int e_9 = tmpe_19;
      vdm_TRF_uTRA_uDEF_i = e_9;
      if (succ_8) 
        res_l_5.ImpAppend(vdm_TRF_uTRA_uPAT_trv_uPattern((Generic) (Record) vdm_TRF_uTRA_uDEF_pats[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    par_4 = res_l_5;
  }
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uPatTypePair((Generic) par_4, (Generic) vdm_TRF_uTRA_uTYPE_trv_uType((Generic) vdm_TRF_uTRA_uDEF_tp), (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uTypeDef

TYPE_TRF_uTAR_uDEF_TypeDef vdm_TRF_uTRA_uDEF_trv_uTypeDef(const TYPE_AS_TypeDef &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Generic vdm_TRF_uTRA_uDEF_fninv;
  Record vdm_TRF_uTRA_uDEF_nm(vdm_AS_Name, length_AS_Name);
  Record vdm_TRF_uTRA_uDEF_tp;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_TypeDef == var_1_1.GetTag()) {
    Sequence e_l_16;
    for (int i_17 = 1; i_17 <= var_1_1.Length(); i_17++) 
      e_l_16.ImpAppend(var_1_1.GetField(i_17));
    if (succ_2 = 4 == (Int) e_l_16.Length()) {
      vdm_TRF_uTRA_uDEF_nm = (Generic) e_l_16.Hd();
      vdm_TRF_uTRA_uDEF_tp = (Generic) e_l_16[2];
      vdm_TRF_uTRA_uDEF_fninv = (Generic) e_l_16[3];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_16[4];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uTypeDef failed");
  Record varRes_3(vdm_AS_TypeDef, length_AS_TypeDef);
  Generic par_9;
  if (((Bool) (vdm_TRF_uTRA_uDEF_fninv == Nil())).GetValue()) 
    par_9 = Nil();
  else 
    par_9 = vdm_TRF_uTRA_uDEF_trv_uInvariant((Generic) vdm_TRF_uTRA_uDEF_fninv);
  varRes_3 = vdm_TRF_uTAR_uDEF_tar_uTypeDef((Generic) vdm_TRF_uTRA_uEXPR_trv_uNameName((Generic) vdm_TRF_uTRA_uDEF_nm), (Generic) vdm_TRF_uTRA_uDEF_tp, (Generic) vdm_TRF_uTRA_uTYPE_trv_uType((Generic) vdm_TRF_uTRA_uDEF_tp), (Generic) par_9, (Generic) vdm_TRF_uTRA_uDEF_cid);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uTypeVarList

TYPE_TRF_uTAR_uDEF_TypeVarList vdm_TRF_uTRA_uDEF_trv_uTypeVarList(const TYPE_AS_TypeVarList &vdm_TRF_uTRA_uDEF_tv_ulst) {
  Sequence varRes_3;
  {
    Sequence res_l_4;
    Set resBind_s_6;
    Set riseq_10;
    int max_11 = (Int) vdm_TRF_uTRA_uDEF_tv_ulst.Length();
    for (int i_12 = 1; i_12 <= max_11; i_12++) 
      riseq_10.Insert((Int) i_12);
    resBind_s_6 = riseq_10;
    Sequence bind_l_5;
    if ((Int) resBind_s_6.Card() != (Int) 0) 
      bind_l_5 = Sort(resBind_s_6);
    bool succ_7 = true;
    Int vdm_TRF_uTRA_uDEF_i;
    Generic tmpe_18;
    for (int i_17 = bind_l_5.First(tmpe_18); i_17; i_17 = bind_l_5.Next(tmpe_18)) {
      Int e_8 = tmpe_18;
      vdm_TRF_uTRA_uDEF_i = e_8;
      if (succ_7) 
        res_l_4.ImpAppend(vdm_TRF_uTRA_uTYPE_trv_uTypeVar((Generic) (Record) vdm_TRF_uTRA_uDEF_tv_ulst[vdm_TRF_uTRA_uDEF_i.GetValue()]));
    }
    varRes_3 = res_l_4;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_TRF_uTRA_uDEF_trv_uValueDef

TYPE_TRF_uTAR_uDEF_ValueDef vdm_TRF_uTRA_uDEF_trv_uValueDef(const TYPE_AS_ValueDef &var_1_1) {
  Int vdm_TRF_uTRA_uDEF_cid;
  Record vdm_TRF_uTRA_uDEF_pat;
  Generic vdm_TRF_uTRA_uDEF_tp;
  Record vdm_TRF_uTRA_uDEF_val;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_ValueDef == var_1_1.GetTag()) {
    Sequence e_l_19;
    for (int i_20 = 1; i_20 <= var_1_1.Length(); i_20++) 
      e_l_19.ImpAppend(var_1_1.GetField(i_20));
    if (succ_2 = 4 == (Int) e_l_19.Length()) {
      vdm_TRF_uTRA_uDEF_pat = (Generic) e_l_19.Hd();
      vdm_TRF_uTRA_uDEF_tp = (Generic) e_l_19[2];
      vdm_TRF_uTRA_uDEF_val = (Generic) e_l_19[3];
      vdm_TRF_uTRA_uDEF_cid = (Generic) e_l_19[4];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : trv_uValueDef failed");
  Record varRes_3(vdm_TRF_uTAR_uDEF_ValueDef, length_TRF_uTAR_uDEF_ValueDef);
  {
    Record vdm_TRF_uTRA_uDEF_pat1 = vdm_TRF_uTRA_uPAT_trv_uPattern((Generic) vdm_TRF_uTRA_uDEF_pat);
    Generic vdm_TRF_uTRA_uDEF_tp1;
    if (((Bool) (vdm_TRF_uTRA_uDEF_tp == Nil())).GetValue()) 
      vdm_TRF_uTRA_uDEF_tp1 = Nil();
    else 
      vdm_TRF_uTRA_uDEF_tp1 = vdm_TRF_uTRA_uTYPE_trv_uType((Generic) vdm_TRF_uTRA_uDEF_tp);
    Record vdm_TRF_uTRA_uDEF_val1 = vdm_TRF_uTRA_uEXPR_trv_uExpr((Generic) vdm_TRF_uTRA_uDEF_val);
    varRes_3 = vdm_TRF_uTAR_uDEF_tar_uValueDef((Generic) vdm_TRF_uTRA_uDEF_pat1, (Generic) vdm_TRF_uTRA_uDEF_tp1, (Generic) vdm_TRF_uTRA_uDEF_val1, (Generic) vdm_TRF_uTRA_uDEF_cid);
  }
  return (Generic) varRes_3;
}
#endif

