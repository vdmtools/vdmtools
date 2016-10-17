// Note the code is easily breakable as  my_init_HOL_VDMLib ()
// is a transcription of init_HOL_VDMLib () which is code generated
// if the HOL.vdmsl is changed then also this file may need to be changed. 


void my_init_HOL_VDMLib () {  
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Abs, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Abs, "HOL`Abs");
  AddRecordTag("HOL`Abs", TAG_TYPE_HOL_Abs);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Bool_utype, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Bool_utype, "HOL`Bool_type");
  AddRecordTag("HOL`Bool_utype", TAG_TYPE_HOL_Bool_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Boolean, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Boolean, "HOL`Boolean");
  AddRecordTag("HOL`Boolean", TAG_TYPE_HOL_Boolean);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Char, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Char, "HOL`Char");
  AddRecordTag("HOL`Char", TAG_TYPE_HOL_Char);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Char_utype, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Char_utype, "HOL`Char_type");
  AddRecordTag("HOL`Char_utype", TAG_TYPE_HOL_Char_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Comb, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Comb, "HOL`Comb");
  AddRecordTag("HOL`Comb", TAG_TYPE_HOL_Comb);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Cond, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Cond, "HOL`Cond");
  AddRecordTag("HOL`Cond", TAG_TYPE_HOL_Cond);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Conj, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Conj, "HOL`Conj");
  AddRecordTag("HOL`Conj", TAG_TYPE_HOL_Conj);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Const, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Const, "HOL`Const");
  AddRecordTag("HOL`Const", TAG_TYPE_HOL_Const);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Disj, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Disj, "HOL`Disj");
  AddRecordTag("HOL`Disj", TAG_TYPE_HOL_Disj);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Eq, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Eq, "HOL`Eq");
  AddRecordTag("HOL`Eq", TAG_TYPE_HOL_Eq);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Exists, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Exists, "HOL`Exists");
  AddRecordTag("HOL`Exists", TAG_TYPE_HOL_Exists);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Exists1, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Exists1, "HOL`Exists1");
  AddRecordTag("HOL`Exists1", TAG_TYPE_HOL_Exists1);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Forall, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Forall, "HOL`Forall");
  AddRecordTag("HOL`Forall", TAG_TYPE_HOL_Forall);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Fun_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Fun_utype, "HOL`Fun_type");
  AddRecordTag("HOL`Fun_utype", TAG_TYPE_HOL_Fun_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Imp, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Imp, "HOL`Imp");
  AddRecordTag("HOL`Imp", TAG_TYPE_HOL_Imp);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Ind, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Ind, "HOL`Ind");
  AddRecordTag("HOL`Ind", TAG_TYPE_HOL_Ind);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Ind_utype, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Ind_utype, "HOL`Ind_type");
  AddRecordTag("HOL`Ind_utype", TAG_TYPE_HOL_Ind_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Int_utype, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Int_utype, "HOL`Int_type");
  AddRecordTag("HOL`Int_utype", TAG_TYPE_HOL_Int_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Integer, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Integer, "HOL`Integer");
  AddRecordTag("HOL`Integer", TAG_TYPE_HOL_Integer);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Let, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Let, "HOL`Let");
  AddRecordTag("HOL`Let", TAG_TYPE_HOL_Let);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_List, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_List, "HOL`List");
  AddRecordTag("HOL`List", TAG_TYPE_HOL_List);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_List_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_List_utype, "HOL`List_type");
  AddRecordTag("HOL`List_utype", TAG_TYPE_HOL_List_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Map, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Map, "HOL`Map");
  AddRecordTag("HOL`Map", TAG_TYPE_HOL_Map);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Map_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Map_utype, "HOL`Map_type");
  AddRecordTag("HOL`Map_utype", TAG_TYPE_HOL_Map_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Neg, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Neg, "HOL`Neg");
  AddRecordTag("HOL`Neg", TAG_TYPE_HOL_Neg);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Num_utype, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Num_utype, "HOL`Num_type");
  AddRecordTag("HOL`Num_utype", TAG_TYPE_HOL_Num_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Numeral, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Numeral, "HOL`Numeral");
  AddRecordTag("HOL`Numeral", TAG_TYPE_HOL_Numeral);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Option_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Option_utype, "HOL`Option_type");
  AddRecordTag("HOL`Option_utype", TAG_TYPE_HOL_Option_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Pair, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Pair, "HOL`Pair");
  AddRecordTag("HOL`Pair", TAG_TYPE_HOL_Pair);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Prod_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Prod_utype, "HOL`Prod_type");
  AddRecordTag("HOL`Prod_utype", TAG_TYPE_HOL_Prod_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Proj, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Proj, "HOL`Proj");
  AddRecordTag("HOL`Proj", TAG_TYPE_HOL_Proj);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Real, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Real, "HOL`Real");
  AddRecordTag("HOL`Real", TAG_TYPE_HOL_Real);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Real_utype, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Real_utype, "HOL`Real_type");
  AddRecordTag("HOL`Real_utype", TAG_TYPE_HOL_Real_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Select, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Select, "HOL`Select");
  AddRecordTag("HOL`Select", TAG_TYPE_HOL_Select);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Set, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Set, "HOL`Set");
  AddRecordTag("HOL`Set", TAG_TYPE_HOL_Set);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Set_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Set_utype, "HOL`Set_type");
  AddRecordTag("HOL`Set_utype", TAG_TYPE_HOL_Set_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_String, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_String, "HOL`String");
  AddRecordTag("HOL`String", TAG_TYPE_HOL_String);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_Var, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_Var, "HOL`Var");
  AddRecordTag("HOL`Var", TAG_TYPE_HOL_Var);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_const_udef, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_const_udef, "HOL`const_def");
  AddRecordTag("HOL`const_udef", TAG_TYPE_HOL_const_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_datatype_udef, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_datatype_udef, "HOL`datatype_def");
  AddRecordTag("HOL`datatype_udef", TAG_TYPE_HOL_datatype_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_field, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_field, "HOL`field");
  AddRecordTag("HOL`field", TAG_TYPE_HOL_field);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_func_udef, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_func_udef, "HOL`func_def");
  AddRecordTag("HOL`func_udef", TAG_TYPE_HOL_func_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_po_udef, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_po_udef, "HOL`po_def");
  AddRecordTag("HOL`po_udef", TAG_TYPE_HOL_po_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_pred_udef, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_pred_udef, "HOL`pred_def");
  AddRecordTag("HOL`pred_udef", TAG_TYPE_HOL_pred_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_quote_utype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_quote_utype, "HOL`quote_type");
  AddRecordTag("HOL`quote_utype", TAG_TYPE_HOL_quote_utype);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_record, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_record, "HOL`record");
  AddRecordTag("HOL`record", TAG_TYPE_HOL_record);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_select_udef, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_select_udef, "HOL`select_def");
  AddRecordTag("HOL`select_udef", TAG_TYPE_HOL_select_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_spec, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_spec, "HOL`spec");
  AddRecordTag("HOL`spec", TAG_TYPE_HOL_spec);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_sum, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_sum, "HOL`sum");
  AddRecordTag("HOL`sum", TAG_TYPE_HOL_sum);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_type, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_type, "HOL`type");
  AddRecordTag("HOL`type", TAG_TYPE_HOL_type);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_type_uabbrev_udef, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_type_uabbrev_udef, "HOL`type_abbrev_def");
  AddRecordTag("HOL`type_uabbrev_udef", TAG_TYPE_HOL_type_uabbrev_udef);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_HOL_vartype, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_HOL_vartype, "HOL`vartype");
  AddRecordTag("HOL`vartype", TAG_TYPE_HOL_vartype);
  VDMGetDefaultRecInfoMap().NewTag(TOKEN, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TOKEN, "token");

}

type_cL vdm_TRL_uMEDIATOR_Translation () {
  my_init_HOL_VDMLib ();  // to ensure that record tags are dumped 
                   // with their names and not just a number 
                   // my_init because I have to get ride of the "u" in _u
  TYPE_TRL_uTAR_uDEF_Module vdm_TRL_uMEDIATOR_transl =
    vdm_TRL_uTRA_uDEF_trv_uModule_uwithout_uder_ucon(vdm_SPEC_uAST_GetAst());
  SEQ<Char> temp (vdm_TRL_uMEDIATOR_transl.ascii());
  type_cL temp2 (temp); 

  return  temp2;
}



