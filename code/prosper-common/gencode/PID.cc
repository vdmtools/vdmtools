//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Tue 12-Oct-99 by the VDM-SLtoC++ Code Generator
// (v3.3 - Mon 22-Mar-99)
//
// Supported compilers:
// 	egcs version 1.1 on SunOS4, SunOS5, Linux, HP-UX10
// 	VC++ version 6.0 on Windows95, Windows NT
//



#include "PID.h"
static type_14AS_PatternNameCS vdm_PID_MatchVal2Pids(const TYPE_AS_MatchVal &);
static Bool vdm_PID_NameInMultBind(const TYPE_AS_Name &, const TYPE_AS_MultBind &);
static Bool vdm_PID_NameInPatternBind(const TYPE_AS_Name &, const TYPE_AS_PatternBind &);
static Bool vdm_PID_NameInPatternSequence(const TYPE_AS_Name &, const type_FL &);
static type_14AS_PatternNameCS vdm_PID_PatternName2Pids(const TYPE_AS_PatternName &);
static type_14AS_PatternNameCS vdm_PID_RecordPattern2Pids(const TYPE_AS_RecordPattern &);
static type_14AS_PatternNameCS vdm_PID_SeqConcPattern2Pids(const TYPE_AS_SeqConcPattern &);
static type_14AS_PatternNameCS vdm_PID_SeqEnumPattern2Pids(const TYPE_AS_SeqEnumPattern &);
static type_14AS_PatternNameCS vdm_PID_SetEnumPattern2Pids(const TYPE_AS_SetEnumPattern &);
static type_typevariableL vdm_PID_SetToSeq(const type_typevariableS &);
static type_14AS_PatternNameCS vdm_PID_SetUnionPattern2Pids(const TYPE_AS_SetUnionPattern &);
static type_14AS_PatternNameCS vdm_PID_TuplePattern2Pids(const TYPE_AS_TuplePattern &);

void init_PID_VDMLib() {
  VDMGetDefaultRecInfoMap().NewTag(TOKEN, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TOKEN, "token");
}
#ifdef DEF_PID_USERIMPL

#include "PID_userimpl.cc"

#endif


void init_PID() { init_PID_VDMLib(); }
#ifndef DEF_PID_MatchVal2Pids

type_14AS_PatternNameCS vdm_PID_MatchVal2Pids(const TYPE_AS_MatchVal &var_1_1) {
  Record vdm_PID_val;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_MatchVal == var_1_1.GetTag()) {
    Sequence e_l_4;
    for (int i_5 = 1; i_5 <= var_1_1.Length(); i_5++) 
      e_l_4.ImpAppend(var_1_1.GetField(i_5));
    if (succ_2 = 2 == (Int) e_l_4.Length()) 
      vdm_PID_val = (Generic) e_l_4.Hd();
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : MatchVal2Pids failed");
  return (Generic) Set();
}
#endif

#ifndef DEF_PID_NameInBind

Bool vdm_PID_NameInBind(const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_Bind &vdm_PID_bind) {
  Bool varRes_4;
  Record par_6;
  if (((Record) vdm_PID_bind).Is(vdm_AS_TypeBind)) 
    par_6 = ((Record) vdm_PID_bind).GetField(pos_AS_TypeBind_pat);
  else 
    if (((Record) vdm_PID_bind).Is(vdm_AS_SetBind)) 
      par_6 = ((Record) vdm_PID_bind).GetField(pos_AS_SetBind_pat);
    else 
      RunTime("Unknown record field selector");
  varRes_4 = vdm_PID_NameInPattern((Generic) vdm_PID_nm, (Generic) par_6);
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_PID_NameInBindList

Bool vdm_PID_NameInBindList(const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_BindList &vdm_PID_binds) {
  Bool varRes_4;
  Set var2_6;
  Set res_s_7;
  {
    bool succ_13 = true;
    Set e_set_14;    Set riseq_16;    int max_17 = (Int) vdm_PID_binds.Length();    for (int i_18 = 1; i_18 <= max_17; i_18++) 
      riseq_16.Insert((Int) i_18);    e_set_14 = riseq_16;
    Int vdm_PID_i;
    {
      Generic tmpe_21;
      for (int bb_20 = e_set_14.First(tmpe_21); bb_20; bb_20 = e_set_14.Next(tmpe_21)) {
        Int elem_19 = tmpe_21;
        succ_13 = true;        vdm_PID_i = elem_19;        if (succ_13) { res_s_7.Insert(vdm_PID_NameInMultBind((Generic) vdm_PID_nm, (Generic) (Record) vdm_PID_binds[vdm_PID_i.GetValue()])); }
      }
    }
  }
  var2_6 = res_s_7;
  varRes_4 = (Bool) var2_6.InSet((Generic) (Bool) true);
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_PID_NameInMultBind

Bool vdm_PID_NameInMultBind(const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_MultBind &vdm_PID_mbind) {
  Bool varRes_4;
  Sequence par_6;
  if (((Record) vdm_PID_mbind).Is(vdm_AS_MultTypeBind)) 
    par_6 = ((Record) vdm_PID_mbind).GetField(pos_AS_MultTypeBind_pat);
  else 
    if (((Record) vdm_PID_mbind).Is(vdm_AS_MultSetBind)) 
      par_6 = ((Record) vdm_PID_mbind).GetField(pos_AS_MultSetBind_pat);
    else 
      RunTime("Unknown record field selector");
  varRes_4 = vdm_PID_NameInPatternSequence((Generic) vdm_PID_nm, (Generic) par_6);
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_PID_NameInPattern

Bool vdm_PID_NameInPattern(const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_Pattern &vdm_PID_pat) {
  Bool varRes_4;
  bool succ_5 = true;
  {
    succ_5 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_PatternName == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_8;
      for (int i_9 = 1; i_9 <= ((Record) vdm_PID_pat).Length(); i_9++) 
        e_l_8.ImpAppend(((Record) vdm_PID_pat).GetField(i_9));
      if (succ_5 = 2 == (Int) e_l_8.Length()) {
        succ_5 = true;
        if ((Generic) e_l_8.Hd() != vdm_PID_nm) 
          succ_5 = false;
        if (succ_5) {}
      }
    }
    else 
      succ_5 = false;
    if (succ_5) 
      varRes_4 = (Bool) true;
  }
  if (!succ_5) {
    succ_5 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_MatchVal == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_11;
      for (int i_12 = 1; i_12 <= ((Record) vdm_PID_pat).Length(); i_12++) 
        e_l_11.ImpAppend(((Record) vdm_PID_pat).GetField(i_12));
      if (succ_5 = 2 == (Int) e_l_11.Length()) {}
    }
    else 
      succ_5 = false;
    if (succ_5) 
      varRes_4 = (Bool) false;
  }
  if (!succ_5) {
    Record vdm_PID_pat1;
    Record vdm_PID_pat2;
    succ_5 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_SetUnionPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_13;
      for (int i_14 = 1; i_14 <= ((Record) vdm_PID_pat).Length(); i_14++) 
        e_l_13.ImpAppend(((Record) vdm_PID_pat).GetField(i_14));
      if (succ_5 = 3 == (Int) e_l_13.Length()) {
        vdm_PID_pat1 = (Generic) e_l_13.Hd();
        vdm_PID_pat2 = (Generic) e_l_13[2];
      }
    }
    else 
      succ_5 = false;
    if (!succ_5) {
      succ_5 = true;
      if (vdm_PID_pat.IsRecord() && vdm_AS_SeqConcPattern == ((Record) vdm_PID_pat).GetTag()) {
        Sequence e_l_15;
        for (int i_16 = 1; i_16 <= ((Record) vdm_PID_pat).Length(); i_16++) 
          e_l_15.ImpAppend(((Record) vdm_PID_pat).GetField(i_16));
        if (succ_5 = 3 == (Int) e_l_15.Length()) {
          vdm_PID_pat1 = (Generic) e_l_15.Hd();
          vdm_PID_pat2 = (Generic) e_l_15[2];
        }
      }
      else 
        succ_5 = false;
    }
    if (succ_5) {
      Sequence par_18;
      par_18 = Sequence().ImpAppend(vdm_PID_pat1);
      par_18.ImpAppend(vdm_PID_pat2);
      varRes_4 = vdm_PID_NameInPatternSequence((Generic) vdm_PID_nm, (Generic) par_18);
    }
  }
  if (!succ_5) {
    Sequence vdm_PID_pats;
    succ_5 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_SetEnumPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_21;
      for (int i_22 = 1; i_22 <= ((Record) vdm_PID_pat).Length(); i_22++) 
        e_l_21.ImpAppend(((Record) vdm_PID_pat).GetField(i_22));
      if (succ_5 = 2 == (Int) e_l_21.Length()) 
        vdm_PID_pats = (Generic) e_l_21.Hd();
    }
    else 
      succ_5 = false;
    if (!succ_5) {
      succ_5 = true;
      if (vdm_PID_pat.IsRecord() && vdm_AS_SeqEnumPattern == ((Record) vdm_PID_pat).GetTag()) {
        Sequence e_l_23;
        for (int i_24 = 1; i_24 <= ((Record) vdm_PID_pat).Length(); i_24++) 
          e_l_23.ImpAppend(((Record) vdm_PID_pat).GetField(i_24));
        if (succ_5 = 2 == (Int) e_l_23.Length()) 
          vdm_PID_pats = (Generic) e_l_23.Hd();
      }
      else 
        succ_5 = false;
    }
    if (!succ_5) {
      succ_5 = true;
      if (vdm_PID_pat.IsRecord() && vdm_AS_RecordPattern == ((Record) vdm_PID_pat).GetTag()) {
        Sequence e_l_25;
        for (int i_26 = 1; i_26 <= ((Record) vdm_PID_pat).Length(); i_26++) 
          e_l_25.ImpAppend(((Record) vdm_PID_pat).GetField(i_26));
        if (succ_5 = 3 == (Int) e_l_25.Length()) 
          vdm_PID_pats = (Generic) e_l_25[2];
      }
      else 
        succ_5 = false;
    }
    if (!succ_5) {
      succ_5 = true;
      if (vdm_PID_pat.IsRecord() && vdm_AS_TuplePattern == ((Record) vdm_PID_pat).GetTag()) {
        Sequence e_l_27;
        for (int i_28 = 1; i_28 <= ((Record) vdm_PID_pat).Length(); i_28++) 
          e_l_27.ImpAppend(((Record) vdm_PID_pat).GetField(i_28));
        if (succ_5 = 2 == (Int) e_l_27.Length()) 
          vdm_PID_pats = (Generic) e_l_27.Hd();
      }
      else 
        succ_5 = false;
    }
    if (succ_5) 
      varRes_4 = vdm_PID_NameInPatternSequence((Generic) vdm_PID_nm, (Generic) vdm_PID_pats);
  }
  if (!succ_5) 
    varRes_4 = (Bool) false;
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_PID_NameInPatternBind

Bool vdm_PID_NameInPatternBind(const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_PatternBind &vdm_PID_patbind) {
  Bool varRes_4;
  bool succ_5 = true;
  {
    succ_5 = true;
    if (vdm_PID_patbind.IsRecord() && vdm_AS_SetBind == ((Record) vdm_PID_patbind).GetTag()) {
      Sequence e_l_8;
      for (int i_9 = 1; i_9 <= ((Record) vdm_PID_patbind).Length(); i_9++) 
        e_l_8.ImpAppend(((Record) vdm_PID_patbind).GetField(i_9));
      if (succ_5 = 3 == (Int) e_l_8.Length()) {}
    }
    else 
      succ_5 = false;
    if (!succ_5) {
      succ_5 = true;
      if (vdm_PID_patbind.IsRecord() && vdm_AS_TypeBind == ((Record) vdm_PID_patbind).GetTag()) {
        Sequence e_l_10;
        for (int i_11 = 1; i_11 <= ((Record) vdm_PID_patbind).Length(); i_11++) 
          e_l_10.ImpAppend(((Record) vdm_PID_patbind).GetField(i_11));
        if (succ_5 = 3 == (Int) e_l_10.Length()) {}
      }
      else 
        succ_5 = false;
    }
    if (succ_5) 
      varRes_4 = vdm_PID_NameInBind((Generic) vdm_PID_nm, (Generic) vdm_PID_patbind);
  }
  if (!succ_5) 
    varRes_4 = vdm_PID_NameInPattern((Generic) vdm_PID_nm, (Generic) vdm_PID_patbind);
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_PID_NameInPatternSequence

Bool vdm_PID_NameInPatternSequence(const TYPE_AS_Name &vdm_PID_nm, const type_FL &vdm_PID_pats) {
  Bool varRes_4;
  Set var2_6;
  Set res_s_7;
  {
    bool succ_13 = true;
    Set e_set_14;    Set riseq_16;    int max_17 = (Int) vdm_PID_pats.Length();    for (int i_18 = 1; i_18 <= max_17; i_18++) 
      riseq_16.Insert((Int) i_18);    e_set_14 = riseq_16;
    Int vdm_PID_i;
    {
      Generic tmpe_21;
      for (int bb_20 = e_set_14.First(tmpe_21); bb_20; bb_20 = e_set_14.Next(tmpe_21)) {
        Int elem_19 = tmpe_21;
        succ_13 = true;        vdm_PID_i = elem_19;        if (succ_13) { res_s_7.Insert(vdm_PID_NameInPattern((Generic) vdm_PID_nm, (Generic) (Record) vdm_PID_pats[vdm_PID_i.GetValue()])); }
      }
    }
  }
  var2_6 = res_s_7;
  varRes_4 = (Bool) var2_6.InSet((Generic) (Bool) true);
  return (Generic) varRes_4;
}
#endif

#ifndef DEF_PID_Pattern2BindList

TYPE_AS_BindList vdm_PID_Pattern2BindList(const TYPE_AS_Pattern &vdm_PID_pat) {
  Sequence varRes_3;
  {
    Set vdm_PID_pid_uset = vdm_PID_Pattern2Pids((Generic) vdm_PID_pat);
    Sequence vdm_PID_pid_useq = vdm_PID_SetToSeq((Generic) vdm_PID_pid_uset);
    Sequence vdm_PID_bd_ulst;
    {
      Sequence res_l_10;
      Set resBind_s_12;
      Set riseq_16;
      int max_17 = (Int) vdm_PID_pid_useq.Length();
      for (int i_18 = 1; i_18 <= max_17; i_18++) 
        riseq_16.Insert((Int) i_18);
      resBind_s_12 = riseq_16;
      Sequence bind_l_11;
      if ((Int) resBind_s_12.Card() != (Int) 0) 
        bind_l_11 = Sort(resBind_s_12);
      bool succ_13 = true;
      Int vdm_PID_i;
      Generic tmpe_31;
      for (int i_30 = bind_l_11.First(tmpe_31); i_30; i_30 = bind_l_11.Next(tmpe_31)) {
        Int e_14 = tmpe_31;
        vdm_PID_i = e_14;
        if (succ_13) {
          Record reselem_19(vdm_AS_MultTypeBind, length_AS_MultTypeBind);
          reselem_19 = Record(vdm_AS_MultTypeBind, length_AS_MultTypeBind);
          Sequence tmpVar_20;
          tmpVar_20 = Sequence().ImpAppend((Record) vdm_PID_pid_useq[vdm_PID_i.GetValue()]);
          reselem_19.SetField(1, tmpVar_20);
          Record tmpVar_24;
          Int par_25;
          if (((Record) vdm_PID_pid_useq[vdm_PID_i.GetValue()]).Is(vdm_AS_PatternName)) 
            par_25 = ((Record) vdm_PID_pid_useq[vdm_PID_i.GetValue()]).GetField(pos_AS_PatternName_cid);
          else 
            RunTime("Unknown record field selector");
          tmpVar_24 = vdm_CI_uAUX_GetASType((Generic) par_25);
          reselem_19.SetField(2, tmpVar_24);
          reselem_19.SetField(3, vdm_CI_NilContextId);
          res_l_10.ImpAppend(reselem_19);
        }
      }
      vdm_PID_bd_ulst = res_l_10;
    }
    varRes_3 = vdm_PID_bd_ulst;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_Pattern2Pids

type_14AS_PatternNameCS vdm_PID_Pattern2Pids(const TYPE_AS_Pattern &vdm_PID_pat) {
  Generic varRes_3;
  bool succ_4 = true;
  {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_PatternName == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_7;
      for (int i_8 = 1; i_8 <= ((Record) vdm_PID_pat).Length(); i_8++) 
        e_l_7.ImpAppend(((Record) vdm_PID_pat).GetField(i_8));
      if (succ_4 = 2 == (Int) e_l_7.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_PatternName2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_MatchVal == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_10;
      for (int i_11 = 1; i_11 <= ((Record) vdm_PID_pat).Length(); i_11++) 
        e_l_10.ImpAppend(((Record) vdm_PID_pat).GetField(i_11));
      if (succ_4 = 2 == (Int) e_l_10.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_MatchVal2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_SetEnumPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_13;
      for (int i_14 = 1; i_14 <= ((Record) vdm_PID_pat).Length(); i_14++) 
        e_l_13.ImpAppend(((Record) vdm_PID_pat).GetField(i_14));
      if (succ_4 = 2 == (Int) e_l_13.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_SetEnumPattern2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_SetUnionPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_16;
      for (int i_17 = 1; i_17 <= ((Record) vdm_PID_pat).Length(); i_17++) 
        e_l_16.ImpAppend(((Record) vdm_PID_pat).GetField(i_17));
      if (succ_4 = 3 == (Int) e_l_16.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_SetUnionPattern2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_SeqEnumPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_19;
      for (int i_20 = 1; i_20 <= ((Record) vdm_PID_pat).Length(); i_20++) 
        e_l_19.ImpAppend(((Record) vdm_PID_pat).GetField(i_20));
      if (succ_4 = 2 == (Int) e_l_19.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_SeqEnumPattern2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_SeqConcPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_22;
      for (int i_23 = 1; i_23 <= ((Record) vdm_PID_pat).Length(); i_23++) 
        e_l_22.ImpAppend(((Record) vdm_PID_pat).GetField(i_23));
      if (succ_4 = 3 == (Int) e_l_22.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_SeqConcPattern2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_RecordPattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_25;
      for (int i_26 = 1; i_26 <= ((Record) vdm_PID_pat).Length(); i_26++) 
        e_l_25.ImpAppend(((Record) vdm_PID_pat).GetField(i_26));
      if (succ_4 = 3 == (Int) e_l_25.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_RecordPattern2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) {
    succ_4 = true;
    if (vdm_PID_pat.IsRecord() && vdm_AS_TuplePattern == ((Record) vdm_PID_pat).GetTag()) {
      Sequence e_l_28;
      for (int i_29 = 1; i_29 <= ((Record) vdm_PID_pat).Length(); i_29++) 
        e_l_28.ImpAppend(((Record) vdm_PID_pat).GetField(i_29));
      if (succ_4 = 2 == (Int) e_l_28.Length()) {}
    }
    else 
      succ_4 = false;
    if (succ_4) 
      varRes_3 = vdm_PID_TuplePattern2Pids((Generic) vdm_PID_pat);
  }
  if (!succ_4) 
    RunTime("Can not evaluate an undefined expression");
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_PatternName2Pids

type_14AS_PatternNameCS vdm_PID_PatternName2Pids(const TYPE_AS_PatternName &vdm_PID_pat) {
  Set varRes_3;
  Bool cond_5;
  Generic var1_6;
  if (vdm_PID_pat.Is(vdm_AS_PatternName)) 
    var1_6 = vdm_PID_pat.GetField(pos_AS_PatternName_nm);
  else 
    RunTime("Unknown record field selector");
  cond_5 = (Bool) (var1_6 == Nil());
  if (cond_5.GetValue()) 
    varRes_3 = Set();
  else 
    varRes_3 = Set().Insert(vdm_PID_pat);
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_RecordPattern2Pids

type_14AS_PatternNameCS vdm_PID_RecordPattern2Pids(const TYPE_AS_RecordPattern &var_1_1) {
  Int vdm_PID_cid;
  Sequence vdm_PID_els;
  Record vdm_PID_nm(vdm_AS_Name, length_AS_Name);
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_RecordPattern == var_1_1.GetTag()) {
    Sequence e_l_23;
    for (int i_24 = 1; i_24 <= var_1_1.Length(); i_24++) 
      e_l_23.ImpAppend(var_1_1.GetField(i_24));
    if (succ_2 = 3 == (Int) e_l_23.Length()) {
      vdm_PID_nm = (Generic) e_l_23.Hd();
      vdm_PID_els = (Generic) e_l_23[2];
      vdm_PID_cid = (Generic) e_l_23[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : RecordPattern2Pids failed");
  Set varRes_3;
  Set unArg_4;
  Set res_s_5;
  {
    bool succ_10 = true;
    Set e_set_11;    Set riseq_13;    int max_14 = (Int) vdm_PID_els.Length();    for (int i_15 = 1; i_15 <= max_14; i_15++) 
      riseq_13.Insert((Int) i_15);    e_set_11 = riseq_13;
    Int vdm_PID_i;
    {
      Generic tmpe_18;
      for (int bb_17 = e_set_11.First(tmpe_18); bb_17; bb_17 = e_set_11.Next(tmpe_18)) {
        Int elem_16 = tmpe_18;
        succ_10 = true;        vdm_PID_i = elem_16;        if (succ_10) { res_s_5.Insert(vdm_PID_Pattern2Pids((Generic) (Record) vdm_PID_els[vdm_PID_i.GetValue()])); }
      }
    }
  }
  unArg_4 = res_s_5;
  Set rduset_19;  Generic tmpe_22;  for (int bb_21 = unArg_4.First(tmpe_22); bb_21; bb_21 = unArg_4.Next(tmpe_22)) {
    Set e_20 = tmpe_22;
    rduset_19.ImpUnion(e_20);
  }  varRes_3 = rduset_19;
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_SeqConcPattern2Pids

type_14AS_PatternNameCS vdm_PID_SeqConcPattern2Pids(const TYPE_AS_SeqConcPattern &var_1_1) {
  Int vdm_PID_cid;
  Record vdm_PID_lp;
  Record vdm_PID_rp;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_SeqConcPattern == var_1_1.GetTag()) {
    Sequence e_l_8;
    for (int i_9 = 1; i_9 <= var_1_1.Length(); i_9++) 
      e_l_8.ImpAppend(var_1_1.GetField(i_9));
    if (succ_2 = 3 == (Int) e_l_8.Length()) {
      vdm_PID_lp = (Generic) e_l_8.Hd();
      vdm_PID_rp = (Generic) e_l_8[2];
      vdm_PID_cid = (Generic) e_l_8[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : SeqConcPattern2Pids failed");
  Set varRes_3;
  varRes_3 = vdm_PID_Pattern2Pids((Generic) vdm_PID_lp);
  varRes_3.ImpUnion(vdm_PID_Pattern2Pids((Generic) vdm_PID_rp));
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_SeqEnumPattern2Pids

type_14AS_PatternNameCS vdm_PID_SeqEnumPattern2Pids(const TYPE_AS_SeqEnumPattern &var_1_1) {
  Int vdm_PID_cid;
  Sequence vdm_PID_els;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_SeqEnumPattern == var_1_1.GetTag()) {
    Sequence e_l_23;
    for (int i_24 = 1; i_24 <= var_1_1.Length(); i_24++) 
      e_l_23.ImpAppend(var_1_1.GetField(i_24));
    if (succ_2 = 2 == (Int) e_l_23.Length()) {
      vdm_PID_els = (Generic) e_l_23.Hd();
      vdm_PID_cid = (Generic) e_l_23[2];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : SeqEnumPattern2Pids failed");
  Set varRes_3;
  Set unArg_4;
  Set res_s_5;
  {
    bool succ_10 = true;
    Set e_set_11;    Set riseq_13;    int max_14 = (Int) vdm_PID_els.Length();    for (int i_15 = 1; i_15 <= max_14; i_15++) 
      riseq_13.Insert((Int) i_15);    e_set_11 = riseq_13;
    Int vdm_PID_i;
    {
      Generic tmpe_18;
      for (int bb_17 = e_set_11.First(tmpe_18); bb_17; bb_17 = e_set_11.Next(tmpe_18)) {
        Int elem_16 = tmpe_18;
        succ_10 = true;        vdm_PID_i = elem_16;        if (succ_10) { res_s_5.Insert(vdm_PID_Pattern2Pids((Generic) (Record) vdm_PID_els[vdm_PID_i.GetValue()])); }
      }
    }
  }
  unArg_4 = res_s_5;
  Set rduset_19;  Generic tmpe_22;  for (int bb_21 = unArg_4.First(tmpe_22); bb_21; bb_21 = unArg_4.Next(tmpe_22)) {
    Set e_20 = tmpe_22;
    rduset_19.ImpUnion(e_20);
  }  varRes_3 = rduset_19;
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_SetEnumPattern2Pids

type_14AS_PatternNameCS vdm_PID_SetEnumPattern2Pids(const TYPE_AS_SetEnumPattern &var_1_1) {
  Int vdm_PID_cid;
  Sequence vdm_PID_els;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_SetEnumPattern == var_1_1.GetTag()) {
    Sequence e_l_23;
    for (int i_24 = 1; i_24 <= var_1_1.Length(); i_24++) 
      e_l_23.ImpAppend(var_1_1.GetField(i_24));
    if (succ_2 = 2 == (Int) e_l_23.Length()) {
      vdm_PID_els = (Generic) e_l_23.Hd();
      vdm_PID_cid = (Generic) e_l_23[2];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : SetEnumPattern2Pids failed");
  Set varRes_3;
  Set unArg_4;
  Set res_s_5;
  {
    bool succ_10 = true;
    Set e_set_11;    Set riseq_13;    int max_14 = (Int) vdm_PID_els.Length();    for (int i_15 = 1; i_15 <= max_14; i_15++) 
      riseq_13.Insert((Int) i_15);    e_set_11 = riseq_13;
    Int vdm_PID_i;
    {
      Generic tmpe_18;
      for (int bb_17 = e_set_11.First(tmpe_18); bb_17; bb_17 = e_set_11.Next(tmpe_18)) {
        Int elem_16 = tmpe_18;
        succ_10 = true;        vdm_PID_i = elem_16;        if (succ_10) { res_s_5.Insert(vdm_PID_Pattern2Pids((Generic) (Record) vdm_PID_els[vdm_PID_i.GetValue()])); }
      }
    }
  }
  unArg_4 = res_s_5;
  Set rduset_19;  Generic tmpe_22;  for (int bb_21 = unArg_4.First(tmpe_22); bb_21; bb_21 = unArg_4.Next(tmpe_22)) {
    Set e_20 = tmpe_22;
    rduset_19.ImpUnion(e_20);
  }  varRes_3 = rduset_19;
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_SetToSeq

type_typevariableL vdm_PID_SetToSeq(const type_typevariableS &vdm_PID_s) {
  Sequence varRes_3;
  if (((Bool) (vdm_PID_s == Set())).GetValue()) 
    varRes_3 = Sequence();
  else {
    Set tmpSet_8;
    tmpSet_8 = vdm_PID_s;
    Generic vdm_PID_x;
    bool succ_10 = false;
    Generic tmpe_13;
    for (int bb_12 = tmpSet_8.First(tmpe_13); bb_12 && !succ_10; bb_12 = tmpSet_8.Next(tmpe_13)) {
      Generic tmpElem_11 = tmpe_13;
      succ_10 = true;      vdm_PID_x = tmpElem_11;
    }
    if (!succ_10) 
      RunTime("The binding environment was empty");
    Sequence var1_14;
    Set par_15;
    Set var2_17;
    var2_17 = Set().Insert(vdm_PID_x);
    par_15 = vdm_PID_s;
    par_15.ImpDiff(var2_17);
    var1_14 = vdm_PID_SetToSeq((Generic) par_15);
    Sequence var2_19;
    var2_19 = Sequence().ImpAppend(vdm_PID_x);
    varRes_3.ImpConc(var1_14).ImpConc(var2_19);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_SetUnionPattern2Pids

type_14AS_PatternNameCS vdm_PID_SetUnionPattern2Pids(const TYPE_AS_SetUnionPattern &var_1_1) {
  Int vdm_PID_cid;
  Record vdm_PID_lp;
  Record vdm_PID_rp;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_SetUnionPattern == var_1_1.GetTag()) {
    Sequence e_l_8;
    for (int i_9 = 1; i_9 <= var_1_1.Length(); i_9++) 
      e_l_8.ImpAppend(var_1_1.GetField(i_9));
    if (succ_2 = 3 == (Int) e_l_8.Length()) {
      vdm_PID_lp = (Generic) e_l_8.Hd();
      vdm_PID_rp = (Generic) e_l_8[2];
      vdm_PID_cid = (Generic) e_l_8[3];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : SetUnionPattern2Pids failed");
  Set varRes_3;
  varRes_3 = vdm_PID_Pattern2Pids((Generic) vdm_PID_lp);
  varRes_3.ImpUnion(vdm_PID_Pattern2Pids((Generic) vdm_PID_rp));
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_PID_TuplePattern2Pids

type_14AS_PatternNameCS vdm_PID_TuplePattern2Pids(const TYPE_AS_TuplePattern &var_1_1) {
  Int vdm_PID_cid;
  Sequence vdm_PID_els;
  bool succ_2 = true;
  if (var_1_1.IsRecord() && vdm_AS_TuplePattern == var_1_1.GetTag()) {
    Sequence e_l_23;
    for (int i_24 = 1; i_24 <= var_1_1.Length(); i_24++) 
      e_l_23.ImpAppend(var_1_1.GetField(i_24));
    if (succ_2 = 2 == (Int) e_l_23.Length()) {
      vdm_PID_els = (Generic) e_l_23.Hd();
      vdm_PID_cid = (Generic) e_l_23[2];
    }
  }
  else 
    succ_2 = false;
  if (!succ_2) 
    RunTime("Pattern Match in Function Apply in function : TuplePattern2Pids failed");
  Set varRes_3;
  Set unArg_4;
  Set res_s_5;
  {
    bool succ_10 = true;
    Set e_set_11;    Set riseq_13;    int max_14 = (Int) vdm_PID_els.Length();    for (int i_15 = 1; i_15 <= max_14; i_15++) 
      riseq_13.Insert((Int) i_15);    e_set_11 = riseq_13;
    Int vdm_PID_i;
    {
      Generic tmpe_18;
      for (int bb_17 = e_set_11.First(tmpe_18); bb_17; bb_17 = e_set_11.Next(tmpe_18)) {
        Int elem_16 = tmpe_18;
        succ_10 = true;        vdm_PID_i = elem_16;        if (succ_10) { res_s_5.Insert(vdm_PID_Pattern2Pids((Generic) (Record) vdm_PID_els[vdm_PID_i.GetValue()])); }
      }
    }
  }
  unArg_4 = res_s_5;
  Set rduset_19;  Generic tmpe_22;  for (int bb_21 = unArg_4.First(tmpe_22); bb_21; bb_21 = unArg_4.Next(tmpe_22)) {
    Set e_20 = tmpe_22;
    rduset_19.ImpUnion(e_20);
  }  varRes_3 = rduset_19;
  return (Generic) varRes_3;
}
#endif

