//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Sun 12-Mar-00 by the VDM-SLtoC++ Code Generator
// (v3.3.2 - Thu 02-Dec-99)
//
// Supported compilers:
// 	egcs version 1.1 on SunOS4, SunOS5, Linux, HP-UX10
// 	VC++ version 6.0 on Windows95, Windows NT
//



#include "CI_uAUX.h"

void TYPE_CI_uAUX_FuncReturnInfo::Init(TYPE_AS_Type p1, Bool p2) {
  SetField(1, p1);
  SetField(2, p2);
}

TYPE_AS_Type TYPE_CI_uAUX_FuncReturnInfo::get_retnTp() const { return (TYPE_AS_Type) GetField(1); }

void TYPE_CI_uAUX_FuncReturnInfo::set_retnTp(const TYPE_AS_Type &p) { SetField(1, p); }

Bool TYPE_CI_uAUX_FuncReturnInfo::get_hasPostCond() const { return (Bool) GetField(2); }

void TYPE_CI_uAUX_FuncReturnInfo::set_hasPostCond(const Bool &p) { SetField(2, p); }

void init_CI_uAUX_VDMLib() {
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_CI_uAUX_FuncReturnInfo, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_CI_uAUX_FuncReturnInfo, "CI_uAUX`FuncReturnInfo");
  VDMGetDefaultRecInfoMap().NewTag(TOKEN, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TOKEN, "token");
}
#ifdef DEF_CI_uAUX_USERIMPL

#include "CI_uAUX_userimpl.cc"

#endif


void init_CI_uAUX() { init_CI_uAUX_VDMLib(); }
#ifndef DEF_CI_uAUX_GetASType

TYPE_AS_Type vdm_CI_uAUX_GetASType(const TYPE_CI_ContextId &vdm_CI_uAUX_cid) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tp_urep = vdm_CI_GetTypeInfo((Generic) vdm_CI_uAUX_cid);
    Record vdm_CI_uAUX_tp_uas = vdm_REP2TYPE_TypeRepToType((Generic) vdm_CI_uAUX_tp_urep, (Generic) vdm_CI_NilContextId);
    varRes_3 = vdm_CI_uAUX_tp_uas;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_GetASType

Bool vdm_CI_uAUX_pre_GetASType(const TYPE_CI_ContextId &vdm_CI_uAUX_cid) {
  Bool varRes_3;
{ if ((varRes_3 = vdm_CI_IsAValidContextId((Generic) vdm_CI_uAUX_cid)).GetValue()) 
   varRes_3 = vdm_CI_HasTypeInfo((Generic) vdm_CI_uAUX_cid); }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_GetGroundTypeInfo

TYPE_AS_Type vdm_CI_uAUX_GetGroundTypeInfo(const TYPE_CI_ContextId &vdm_CI_uAUX_cid) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tp_urep = vdm_CI_GetTypeInfo((Generic) vdm_CI_uAUX_cid);
    Record vdm_CI_uAUX_tp = vdm_REP2TYPE_TypeRepToType((Generic) vdm_CI_uAUX_tp_urep, (Generic) vdm_CI_uAUX_cid);
    Record vdm_CI_uAUX_grd_utp;
    Bool cond_12;
    if (vdm_CI_uAUX_tp.IsRecord()) 
      cond_12 = vdm_AS_TypeName == vdm_CI_uAUX_tp.GetTag();
    else 
      cond_12 = false;
    if (cond_12.GetValue()) {
      Record par_14(vdm_AS_Name, length_AS_Name);
      if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_TypeName)) 
        par_14 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_TypeName_name);
      else 
        if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_CompositeType)) 
          par_14 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_CompositeType_name);
        else 
          RunTime("Unknown record field selector");
      vdm_CI_uAUX_grd_utp = vdm_CI_uAUX_LookUpGroundType((Generic) par_14);
    }
    else 
      vdm_CI_uAUX_grd_utp = vdm_CI_uAUX_tp;
    varRes_3 = vdm_CI_uAUX_grd_utp;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_GetGroundTypeInfo

Bool vdm_CI_uAUX_pre_GetGroundTypeInfo(const TYPE_CI_ContextId &vdm_CI_uAUX_cid) {
  Bool varRes_3;
{ if ((varRes_3 = vdm_CI_IsAValidContextId((Generic) vdm_CI_uAUX_cid)).GetValue()) 
   varRes_3 = vdm_CI_HasTypeInfo((Generic) vdm_CI_uAUX_cid); }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_GetGroundTypeRepInfo

TYPE_REP_TypeRep vdm_CI_uAUX_GetGroundTypeRepInfo(const TYPE_CI_ContextId &vdm_CI_uAUX_cid) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tp = vdm_CI_GetTypeInfo((Generic) vdm_CI_uAUX_cid);
    Record vdm_CI_uAUX_tp_ugrd;
    Bool cond_9;
    if (vdm_CI_uAUX_tp.IsRecord()) 
      cond_9 = vdm_REP_TypeNameRep == vdm_CI_uAUX_tp.GetTag();
    else 
      cond_9 = false;
    if (cond_9.GetValue()) {
      Record par_11(vdm_AS_Name, length_AS_Name);
      if (((Record) vdm_CI_uAUX_tp).Is(vdm_REP_TypeVarRep)) 
        par_11 = ((Record) vdm_CI_uAUX_tp).GetField(pos_REP_TypeVarRep_nm);
      else 
        if (((Record) vdm_CI_uAUX_tp).Is(vdm_REP_TypeParRep)) 
          par_11 = ((Record) vdm_CI_uAUX_tp).GetField(pos_REP_TypeParRep_nm);
        else 
          if (((Record) vdm_CI_uAUX_tp).Is(vdm_REP_TypeNameRep)) 
            par_11 = ((Record) vdm_CI_uAUX_tp).GetField(pos_REP_TypeNameRep_nm);
          else 
            if (((Record) vdm_CI_uAUX_tp).Is(vdm_REP_CompositeTypeRep)) 
              par_11 = ((Record) vdm_CI_uAUX_tp).GetField(pos_REP_CompositeTypeRep_nm);
            else 
              RunTime("Unknown record field selector");
      vdm_CI_uAUX_tp_ugrd = vdm_CI_uAUX_LookUpGroundTypeRep((Generic) par_11);
    }
    else 
      vdm_CI_uAUX_tp_ugrd = vdm_CI_uAUX_tp;
    varRes_3 = vdm_CI_uAUX_tp_ugrd;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_GetGroundTypeRepInfo

Bool vdm_CI_uAUX_pre_GetGroundTypeRepInfo(const TYPE_CI_ContextId &vdm_CI_uAUX_cid) {
  Bool varRes_3;
{ if ((varRes_3 = vdm_CI_IsAValidContextId((Generic) vdm_CI_uAUX_cid)).GetValue()) 
   varRes_3 = vdm_CI_HasTypeInfo((Generic) vdm_CI_uAUX_cid); }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_HasInvariant

Bool vdm_CI_uAUX_HasInvariant(const TYPE_AS_Type &vdm_CI_uAUX_type) {
  Bool varRes_3;
  bool succ_4 = true;
  {
    Record vdm_CI_uAUX_name(vdm_AS_Name, length_AS_Name);
    succ_4 = true;
    if (vdm_CI_uAUX_type.IsRecord() && vdm_AS_TypeName == ((Record) vdm_CI_uAUX_type).GetTag()) {
      Sequence e_l_7;
      for (int i_8 = 1; i_8 <= ((Record) vdm_CI_uAUX_type).Length(); i_8++) 
        e_l_7.ImpAppend(((Record) vdm_CI_uAUX_type).GetField(i_8));
      if (succ_4 = 2 == (Int) e_l_7.Length()) 
        vdm_CI_uAUX_name = (Generic) e_l_7.Hd();
    }
    else 
      succ_4 = false;
    if (succ_4) {
      Generic var1_9;
      if (vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_name).Is(vdm_AS_TypeDef)) 
        var1_9 = vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_name).GetField(pos_AS_TypeDef_Inv);
      else 
        RunTime("Unknown record field selector");
      varRes_3 = (Bool) !(var1_9 == Nil());
    }
  }
  if (!succ_4) 
    varRes_3 = (Bool) false;
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_LookUpFieldsFromTag

type_FL vdm_CI_uAUX_LookUpFieldsFromTag(const TYPE_AS_Name &vdm_CI_uAUX_tag) {
  Sequence varRes_3;
  {
    Map vdm_CI_uAUX_tpmap = vdm_SPEC_uAST_GetTypeDefMap();
    Set vdm_CI_uAUX_comp_utps;
    Set res_s_7;
    {
      bool succ_25 = true;
      Set e1_set_28 = vdm_CI_uAUX_tpmap.Dom();
      Record vdm_CI_uAUX_nm(vdm_AS_Name, length_AS_Name);
      {
        Generic tmpe_31;
        for (int bb_30 = e1_set_28.First(tmpe_31); bb_30; bb_30 = e1_set_28.Next(tmpe_31)) {
          Record elem_29 = tmpe_31;
          succ_25 = true;          vdm_CI_uAUX_nm = elem_29;          if (succ_25) {
            Bool pred_12;
            Bool var1_13;
            Record argTmp_14;
            if (((Record) vdm_CI_uAUX_tpmap[(Generic) vdm_CI_uAUX_nm]).Is(vdm_AS_TypeDef)) 
              argTmp_14 = ((Record) vdm_CI_uAUX_tpmap[(Generic) vdm_CI_uAUX_nm]).GetField(pos_AS_TypeDef_shape);
            else 
              RunTime("Unknown record field selector");
            if (argTmp_14.IsRecord()) 
              var1_13 = vdm_AS_CompositeType == argTmp_14.GetTag();
            else 
              var1_13 = false;
{ if ((pred_12 = var1_13).GetValue()) {
   Bool var2_18;
   Record var1_19(vdm_AS_Name, length_AS_Name);
   Record tmpRec_20;
   if (((Record) vdm_CI_uAUX_tpmap[(Generic) vdm_CI_uAUX_nm]).Is(vdm_AS_TypeDef)) 
     tmpRec_20 = ((Record) vdm_CI_uAUX_tpmap[(Generic) vdm_CI_uAUX_nm]).GetField(pos_AS_TypeDef_shape);
   else 
     RunTime("Unknown record field selector");
   if (((Record) tmpRec_20).Is(vdm_AS_TypeName)) 
     var1_19 = ((Record) tmpRec_20).GetField(pos_AS_TypeName_name);
   else 
     if (((Record) tmpRec_20).Is(vdm_AS_CompositeType)) 
       var1_19 = ((Record) tmpRec_20).GetField(pos_AS_CompositeType_name);
     else 
       RunTime("Unknown record field selector");
   var2_18 = (Bool) (var1_19 == vdm_CI_uAUX_tag);
   pred_12 = var2_18;
 } }
            if (pred_12.GetValue()) {
              Record res_s_8;
              if (((Record) vdm_CI_uAUX_tpmap[(Generic) vdm_CI_uAUX_nm]).Is(vdm_AS_TypeDef)) 
                res_s_8 = ((Record) vdm_CI_uAUX_tpmap[(Generic) vdm_CI_uAUX_nm]).GetField(pos_AS_TypeDef_shape);
              else 
                RunTime("Unknown record field selector");
              res_s_7.Insert(res_s_8);
            }
          }
        }
      }
    }
    vdm_CI_uAUX_comp_utps = res_s_7;
    Record vdm_CI_uAUX_comp_utp;
    {
      Set tmpSet_33;
      tmpSet_33 = vdm_CI_uAUX_comp_utps;
      Record vdm_CI_uAUX_c;
      bool succ_35 = false;
      Generic tmpe_38;
      for (int bb_37 = tmpSet_33.First(tmpe_38); bb_37 && !succ_35; bb_37 = tmpSet_33.Next(tmpe_38)) {
        Record tmpElem_36 = tmpe_38;
        succ_35 = true;        vdm_CI_uAUX_c = tmpElem_36;
      }
      if (!succ_35) 
        RunTime("The binding environment was empty");
      vdm_CI_uAUX_comp_utp = vdm_CI_uAUX_c;
    }
    Sequence vdm_CI_uAUX_tps_ulst;
    {
      Sequence res_l_40;
      Set resBind_s_42;
      Sequence unArg_45;
      if (((Record) vdm_CI_uAUX_comp_utp).Is(vdm_AS_CompositeType)) 
        unArg_45 = ((Record) vdm_CI_uAUX_comp_utp).GetField(pos_AS_CompositeType_fields);
      else 
        RunTime("Unknown record field selector");
      Set riseq_47;      int max_48 = (Int) unArg_45.Length();      for (int i_49 = 1; i_49 <= max_48; i_49++) 
        riseq_47.Insert((Int) i_49);      resBind_s_42 = riseq_47;
      Sequence bind_l_41;
      if ((Int) resBind_s_42.Card() != (Int) 0) 
        bind_l_41 = Sort(resBind_s_42);
      bool succ_43 = true;
      Int vdm_CI_uAUX_i;
      Generic tmpe_56;
      for (int i_55 = bind_l_41.First(tmpe_56); i_55; i_55 = bind_l_41.Next(tmpe_56)) {
        Int e_44 = tmpe_56;
        vdm_CI_uAUX_i = e_44;
        if (succ_43) {
          Record reselem_50;
          Record tmpRec_51(vdm_AS_Field, length_AS_Field);
          Sequence tmp_l_52;
          if (((Record) vdm_CI_uAUX_comp_utp).Is(vdm_AS_CompositeType)) 
            tmp_l_52 = ((Record) vdm_CI_uAUX_comp_utp).GetField(pos_AS_CompositeType_fields);
          else 
            RunTime("Unknown record field selector");
          tmpRec_51 = (Record) tmp_l_52[vdm_CI_uAUX_i.GetValue()];
          if (tmpRec_51.Is(vdm_AS_Field)) 
            reselem_50 = tmpRec_51.GetField(pos_AS_Field_type);
          else 
            RunTime("Unknown record field selector");
          res_l_40.ImpAppend(reselem_50);
        }
      }
      vdm_CI_uAUX_tps_ulst = res_l_40;
    }
    varRes_3 = vdm_CI_uAUX_tps_ulst;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_LookUpGroundType

TYPE_AS_Type vdm_CI_uAUX_LookUpGroundType(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tp;
    if (vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_nm).Is(vdm_AS_TypeDef)) 
      vdm_CI_uAUX_tp = vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_nm).GetField(pos_AS_TypeDef_shape);
    else 
      RunTime("Unknown record field selector");
    Bool cond_9;
    if (vdm_CI_uAUX_tp.IsRecord()) 
      cond_9 = vdm_AS_TypeName == vdm_CI_uAUX_tp.GetTag();
    else 
      cond_9 = false;
    if (cond_9.GetValue()) {
      Record par_11(vdm_AS_Name, length_AS_Name);
      if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_TypeName)) 
        par_11 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_TypeName_name);
      else 
        if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_CompositeType)) 
          par_11 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_CompositeType_name);
        else 
          RunTime("Unknown record field selector");
      varRes_3 = vdm_CI_uAUX_LookUpGroundType((Generic) par_11);
    }
    else 
      varRes_3 = vdm_CI_uAUX_tp;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_LookUpGroundType

Bool vdm_CI_uAUX_pre_LookUpGroundType(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Bool varRes_3;
  {
    Map vdm_CI_uAUX_tpmap = vdm_SPEC_uAST_GetTypeDefMap();
    varRes_3 = (Bool) vdm_CI_uAUX_tpmap.Dom().InSet((Generic) vdm_CI_uAUX_nm);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_LookUpGroundTypeRep

TYPE_REP_TypeRep vdm_CI_uAUX_LookUpGroundTypeRep(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tp;
    if (vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_nm).Is(vdm_AS_TypeDef)) 
      vdm_CI_uAUX_tp = vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_nm).GetField(pos_AS_TypeDef_shape);
    else 
      RunTime("Unknown record field selector");
    Record vdm_CI_uAUX_grd_utp;
    Bool cond_10;
    if (vdm_CI_uAUX_tp.IsRecord()) 
      cond_10 = vdm_AS_TypeName == vdm_CI_uAUX_tp.GetTag();
    else 
      cond_10 = false;
    if (cond_10.GetValue()) {
      Record par_12(vdm_AS_Name, length_AS_Name);
      if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_TypeName)) 
        par_12 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_TypeName_name);
      else 
        if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_CompositeType)) 
          par_12 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_CompositeType_name);
        else 
          RunTime("Unknown record field selector");
      vdm_CI_uAUX_grd_utp = vdm_CI_uAUX_LookUpGroundType((Generic) par_12);
    }
    else 
      vdm_CI_uAUX_grd_utp = vdm_CI_uAUX_tp;
    varRes_3 = vdm_TYPE2REP_TransType((Generic) vdm_CI_uAUX_grd_utp);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_LookUpGroundTypeRep

Bool vdm_CI_uAUX_pre_LookUpGroundTypeRep(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Bool varRes_3;
  {
    Map vdm_CI_uAUX_tpmap = vdm_SPEC_uAST_GetTypeDefMap();
    varRes_3 = (Bool) vdm_CI_uAUX_tpmap.Dom().InSet((Generic) vdm_CI_uAUX_nm);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_LookUpInvOrGroundType

TYPE_AS_Type vdm_CI_uAUX_LookUpInvOrGroundType(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tpdef = vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_nm);
    Record vdm_CI_uAUX_tp;
    if (vdm_CI_uAUX_tpdef.Is(vdm_AS_TypeDef)) 
      vdm_CI_uAUX_tp = vdm_CI_uAUX_tpdef.GetField(pos_AS_TypeDef_shape);
    else 
      RunTime("Unknown record field selector");
    Record vdm_CI_uAUX_res_utp;
    Bool cond_11;
    Generic var1_12;
    if (vdm_CI_uAUX_tpdef.Is(vdm_AS_TypeDef)) 
      var1_12 = vdm_CI_uAUX_tpdef.GetField(pos_AS_TypeDef_Inv);
    else 
      RunTime("Unknown record field selector");
    cond_11 = (Bool) !(var1_12 == Nil());
    if (cond_11.GetValue()) {
      vdm_CI_uAUX_res_utp = Record(vdm_AS_TypeName, length_AS_TypeName);
      vdm_CI_uAUX_res_utp.SetField(1, vdm_CI_uAUX_nm);
      vdm_CI_uAUX_res_utp.SetField(2, vdm_CI_NilContextId);
    }
    else {
      Bool cond_15;
      if (vdm_CI_uAUX_tp.IsRecord()) 
        cond_15 = vdm_AS_TypeName == vdm_CI_uAUX_tp.GetTag();
      else 
        cond_15 = false;
      if (cond_15.GetValue()) {
        Record par_17(vdm_AS_Name, length_AS_Name);
        if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_TypeName)) 
          par_17 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_TypeName_name);
        else 
          if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_CompositeType)) 
            par_17 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_CompositeType_name);
          else 
            RunTime("Unknown record field selector");
        vdm_CI_uAUX_res_utp = vdm_CI_uAUX_LookUpInvOrGroundType((Generic) par_17);
      }
      else 
        vdm_CI_uAUX_res_utp = vdm_CI_uAUX_tp;
    }
    varRes_3 = vdm_CI_uAUX_res_utp;
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_LookUpInvOrGroundType

Bool vdm_CI_uAUX_pre_LookUpInvOrGroundType(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Bool varRes_3;
  {
    Map vdm_CI_uAUX_tpmap = vdm_SPEC_uAST_GetTypeDefMap();
    varRes_3 = (Bool) vdm_CI_uAUX_tpmap.Dom().InSet((Generic) vdm_CI_uAUX_nm);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_LookUpInvOrGroundTypeRep

TYPE_REP_TypeRep vdm_CI_uAUX_LookUpInvOrGroundTypeRep(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Record varRes_3;
  {
    Record vdm_CI_uAUX_tpdef = vdm_SPEC_uAST_GetTypeDef((Generic) vdm_CI_uAUX_nm);
    Record vdm_CI_uAUX_tp;
    if (vdm_CI_uAUX_tpdef.Is(vdm_AS_TypeDef)) 
      vdm_CI_uAUX_tp = vdm_CI_uAUX_tpdef.GetField(pos_AS_TypeDef_shape);
    else 
      RunTime("Unknown record field selector");
    Record vdm_CI_uAUX_grd_utp;
    Bool cond_11;
    Generic var1_12;
    if (vdm_CI_uAUX_tpdef.Is(vdm_AS_TypeDef)) 
      var1_12 = vdm_CI_uAUX_tpdef.GetField(pos_AS_TypeDef_Inv);
    else 
      RunTime("Unknown record field selector");
    cond_11 = (Bool) !(var1_12 == Nil());
    if (cond_11.GetValue()) {
      vdm_CI_uAUX_grd_utp = Record(vdm_AS_TypeName, length_AS_TypeName);
      vdm_CI_uAUX_grd_utp.SetField(1, vdm_CI_uAUX_nm);
      vdm_CI_uAUX_grd_utp.SetField(2, vdm_CI_NilContextId);
    }
    else {
      Bool cond_15;
      if (vdm_CI_uAUX_tp.IsRecord()) 
        cond_15 = vdm_AS_TypeName == vdm_CI_uAUX_tp.GetTag();
      else 
        cond_15 = false;
      if (cond_15.GetValue()) {
        Record par_17(vdm_AS_Name, length_AS_Name);
        if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_TypeName)) 
          par_17 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_TypeName_name);
        else 
          if (((Record) vdm_CI_uAUX_tp).Is(vdm_AS_CompositeType)) 
            par_17 = ((Record) vdm_CI_uAUX_tp).GetField(pos_AS_CompositeType_name);
          else 
            RunTime("Unknown record field selector");
        vdm_CI_uAUX_grd_utp = vdm_CI_uAUX_LookUpInvOrGroundTypeRep((Generic) par_17);
      }
      else 
        vdm_CI_uAUX_grd_utp = vdm_CI_uAUX_tp;
    }
    varRes_3 = vdm_TYPE2REP_TransType((Generic) vdm_CI_uAUX_grd_utp);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_LookUpInvOrGroundTypeRep

Bool vdm_CI_uAUX_pre_LookUpInvOrGroundTypeRep(const TYPE_AS_Name &vdm_CI_uAUX_nm) {
  Bool varRes_3;
  {
    Map vdm_CI_uAUX_tpmap = vdm_SPEC_uAST_GetTypeDefMap();
    varRes_3 = (Bool) vdm_CI_uAUX_tpmap.Dom().InSet((Generic) vdm_CI_uAUX_nm);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_getFuncReturnInfo

TYPE_CI_uAUX_FuncReturnInfo vdm_CI_uAUX_getFuncReturnInfo(const type_cL &vdm_CI_uAUX_name) {
  Record varRes_3(vdm_CI_uAUX_FuncReturnInfo, length_CI_uAUX_FuncReturnInfo);
  {
    Record vdm_CI_uAUX_as_uname(vdm_AS_Name, length_AS_Name);
    vdm_CI_uAUX_as_uname = Record(vdm_AS_Name, length_AS_Name);
    Sequence tmpVar_6;
    tmpVar_6 = Sequence().ImpAppend(vdm_CI_uAUX_name);
    vdm_CI_uAUX_as_uname.SetField(1, tmpVar_6);
    vdm_CI_uAUX_as_uname.SetField(2, vdm_CI_NilContextId);
    Record vdm_CI_uAUX_fndef = vdm_SPEC_uAST_GetFnDef((Generic) vdm_CI_uAUX_as_uname);
    Bool vdm_CI_uAUX_hasPostCond;
    Generic var1_12;
    if (((Record) vdm_CI_uAUX_fndef).Is(vdm_AS_ImplFnDef)) 
      var1_12 = ((Record) vdm_CI_uAUX_fndef).GetField(pos_AS_ImplFnDef_fnpost);
    else 
      if (((Record) vdm_CI_uAUX_fndef).Is(vdm_AS_ExtExplFnDef)) 
        var1_12 = ((Record) vdm_CI_uAUX_fndef).GetField(pos_AS_ExtExplFnDef_fnpost);
      else 
        if (((Record) vdm_CI_uAUX_fndef).Is(vdm_AS_ExplFnDef)) 
          var1_12 = ((Record) vdm_CI_uAUX_fndef).GetField(pos_AS_ExplFnDef_fnpost);
        else 
          RunTime("Unknown record field selector");
    vdm_CI_uAUX_hasPostCond = (Bool) !(var1_12 == Nil());
    Generic vdm_CI_uAUX_retnTp;
    bool succ_16 = true;
    {
      succ_16 = true;
      if (vdm_CI_uAUX_fndef.IsRecord() && vdm_AS_ExplFnDef == ((Record) vdm_CI_uAUX_fndef).GetTag()) {
        Sequence e_l_19;
        for (int i_20 = 1; i_20 <= ((Record) vdm_CI_uAUX_fndef).Length(); i_20++) 
          e_l_19.ImpAppend(((Record) vdm_CI_uAUX_fndef).GetField(i_20));
        if (succ_16 = 8 == (Int) e_l_19.Length()) {}
      }
      else 
        succ_16 = false;
      if (succ_16) {
        Record tmpRec_21;
        if (((Record) vdm_CI_uAUX_fndef).Is(vdm_AS_ExplFnDef)) 
          tmpRec_21 = ((Record) vdm_CI_uAUX_fndef).GetField(pos_AS_ExplFnDef_tp);
        else 
          RunTime("Unknown record field selector");
        if (((Record) tmpRec_21).Is(vdm_AS_TotalFnType)) 
          vdm_CI_uAUX_retnTp = ((Record) tmpRec_21).GetField(pos_AS_TotalFnType_fnrng);
        else 
          if (((Record) tmpRec_21).Is(vdm_AS_PartialFnType)) 
            vdm_CI_uAUX_retnTp = ((Record) tmpRec_21).GetField(pos_AS_PartialFnType_fnrng);
          else 
            RunTime("Unknown record field selector");
      }
    }
    if (!succ_16) {
      succ_16 = true;
      if (vdm_CI_uAUX_fndef.IsRecord() && vdm_AS_ImplFnDef == ((Record) vdm_CI_uAUX_fndef).GetTag()) {
        Sequence e_l_23;
        for (int i_24 = 1; i_24 <= ((Record) vdm_CI_uAUX_fndef).Length(); i_24++) 
          e_l_23.ImpAppend(((Record) vdm_CI_uAUX_fndef).GetField(i_24));
        if (succ_16 = 7 == (Int) e_l_23.Length()) {}
      }
      else 
        succ_16 = false;
      if (!succ_16) {
        succ_16 = true;
        if (vdm_CI_uAUX_fndef.IsRecord() && vdm_AS_ExtExplFnDef == ((Record) vdm_CI_uAUX_fndef).GetTag()) {
          Sequence e_l_25;
          for (int i_26 = 1; i_26 <= ((Record) vdm_CI_uAUX_fndef).Length(); i_26++) 
            e_l_25.ImpAppend(((Record) vdm_CI_uAUX_fndef).GetField(i_26));
          if (succ_16 = 8 == (Int) e_l_25.Length()) {}
        }
        else 
          succ_16 = false;
      }
      if (succ_16) 
        RunTime("Can not evaluate an undefined expression");
    }
    if (!succ_16) 
      RunTime("No \'others\' branch in CasesExpr");
    varRes_3 = Record(vdm_CI_uAUX_FuncReturnInfo, length_CI_uAUX_FuncReturnInfo);
    varRes_3.SetField(1, vdm_CI_uAUX_retnTp);
    varRes_3.SetField(2, vdm_CI_uAUX_hasPostCond);
  }
  return (Generic) varRes_3;
}
#endif

#ifndef DEF_CI_uAUX_pre_getFuncReturnInfo

Bool vdm_CI_uAUX_pre_getFuncReturnInfo(const type_cL &vdm_CI_uAUX_name) { return (Generic) vdm_CI_uAUX_isFunctionName((Generic) vdm_CI_uAUX_name); }
#endif

#ifndef DEF_CI_uAUX_isFunctionName

Bool vdm_CI_uAUX_isFunctionName(const type_cL &vdm_CI_uAUX_name) {
  Bool varRes_3;
  {
    Record vdm_CI_uAUX_as_uname(vdm_AS_Name, length_AS_Name);
    vdm_CI_uAUX_as_uname = Record(vdm_AS_Name, length_AS_Name);
    Sequence tmpVar_6;
    tmpVar_6 = Sequence().ImpAppend(vdm_CI_uAUX_name);
    vdm_CI_uAUX_as_uname.SetField(1, tmpVar_6);
    vdm_CI_uAUX_as_uname.SetField(2, vdm_CI_NilContextId);
    Sequence vdm_CI_uAUX_fnnames = vdm_SPEC_uAST_GetFnDefNames();
    varRes_3 = (Bool) vdm_CI_uAUX_fnnames.Elems().InSet((Generic) vdm_CI_uAUX_as_uname);
  }
  return (Generic) varRes_3;
}
#endif

