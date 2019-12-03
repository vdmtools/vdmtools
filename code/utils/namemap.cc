/***
*  * WHAT
*  *  Definition of a general name map for all specifications
*  *  involved with the UML mapper.
*  *  Specification: auml.vdm as.vdmpp rep.vdmpp
*  *
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/namemap.cc,v $
*  * VERSION
*  *    $Revision: 1.12 $
*  * DATE
*  *    $Date: 2005/10/25 05:00:25 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "namemap.h"
#include "metaiv.h"
#include "tag.h"

#include "AUML.h"
#include "DEP.h"
#include "UMLERR.h"

#define NewSetOfTags -20


// Tags are defined as an array of structures. Each struct contains the
// tag name and the tag number as defined in global_tag.h or tag.h
// 
// The tag names of one module must be prefixed with L"ModuleName`".
// Each time the origin of the tags in the array changes (i.e. the vdm module 
// in which they are introduced), the new tag definitions should be
// preceded by an entry like:
//   {L"ModuleName`", NewSetOfTags},
// in order to concatenate L"ModuleName`" to the tag names following this
// entry.
// The complete list must end by an entry like:
//   {L"",0}

static struct TagsTable {
  wchar_t* Tag;
  int   TagNum;
}
TAG_TAB[] = {
  // Definition of tags for the abstract UML syntax
  // Automatically generated from AUML.h by using mkmap.pl
  {L"AUML`", NewSetOfTags},
  #include L"AUML_gen_map.cc"

  // Definition of tags for the UMLERR module.
  {L"UMLERR`", NewSetOfTags},
  #include L"UMLERR_gen_map.cc"

  // Definition of tags for the dependency module
  {L"DEP`", NewSetOfTags},
  #include L"DEP_gen_map.cc"


  // Definition of tag map for common_as.vdm Generated semi-automatically
  // by code generating common_as.vdmsl and common_as.vdmpp in two rounds,
  // and for each header file make the tag map automatically by using 
  // mkmap.pl  
  // The resulting tag maps are merged into one file, which is sorted and
  // piped through 'uniq' in order to remove dublicate lines. The result 
  // is seen below:
  // (It was necessary to patch the list in five cases, the generated
  // pairs are commented out, see below)
  {L"AS`", NewSetOfTags},

  {L"ActExpr", ActExpr},
  {L"ActiveExpr", ActiveExpr},
  {L"AllOrExistsExpr", AllOrExistsExpr},
  {L"AllType", AllType},
  {L"AlphaTrace", AlphaTrace},
  {L"AlphabetExpr", AlphabetExpr},
  {L"AlwaysStmt", AlwaysStmt},
  {L"AnswerStmt", AnswerStmt},
  {L"ApplyExpr", ApplyExpr},
  {L"AssignDef", AssignDef},
  {L"AssignStmt", AssignStmt},
  {L"Assumption", Assumption},
  {L"BasicType", BasicType},
  {L"BinaryExpr", BinaryExpr},
  {L"BlockStmt", BlockStmt},
  {L"BoolLit", BoolLit},
  {L"BracketedExpr", BracketedExpr},
  {L"BracketedType", BracketedType},
  {L"CTerm", CTerm},
  {L"CTerm0", CTerm0},
  {L"CTerm1", CTerm1},
  {L"CallStmt", CallStmt},
  {L"CaseAltn", CaseAltn},
  {L"CasesExpr", CasesExpr},
  {L"CasesStmt", CasesStmt},
  {L"CasesStmtAltn", CasesStmtAltn},
  {L"CharLit", CharLit},
  {L"Class", TAG_TYPE_AS_Class},
  {L"CompositeType", CompositeType},
  {L"DefExpr", DefExpr},
  {L"DefStmt", DefStmt},
  {L"Definitions", Definitions},
  {L"DelTrace", DelTrace},
  {L"DelayStmt", DelayStmt},
  {L"Effect", Effect},
  {L"ElseifExpr", ElseifExpr},
  {L"ElseifStmt", ElseifStmt},
  {L"Error", Error},
  {L"ErrorStmt", ErrorStmt},
  {L"ExistsUniqueExpr", ExistsUniqueExpr},
  {L"ExitStmt", ExitStmt},
  {L"ExplFnDef", ExplFnDef},
  {L"ExplOpDef", ExplOpDef},
  {L"ExportSig", ExportSig},
  {L"ExtExplFnDef", ExtExplFnDef},
  {L"ExtExplOpDef", ExtExplOpDef},
  {L"ExtInf", ExtInf},
  {L"FctTypeInstExpr", FctTypeInstExpr},
  {L"Field", Field},
  {L"FieldRef", FieldRef},
  {L"FieldSelectExpr", FieldSelectExpr},
  {L"FinExpr", FinExpr},
  {L"FnBody", TAG_TYPE_AS_FnBody},
  {L"GeneralMapType", GeneralMapType},
  {L"GuardedCommand", GuardedCommand},
  {L"GuardExpr", TAG_TYPE_AS_GuardExpr},
  {L"IdentStmt", IdentStmt},
  {L"IfExpr", IfExpr},
  {L"IfStmt", IfStmt},
  //  {L"DLExportSig", DLExportSig},
  {L"ImplExportSig", DLExportSig},
  {L"ImplFnDef", ImplFnDef},
  //  {L"DLImportSig", DLImportSig},
  {L"ImplImportSig", DLImportSig},
  //  {L"DLInterface", DLInterface},
  {L"ImplInterface", DLInterface},
  // {L"DLModule", DLModule},
  {L"DLModule", DLModule},
  {L"ImplOpDef", ImplOpDef},
  {L"ImportSig", ImportSig},
  {L"IndexForLoopStmt", IndexForLoopStmt},
  {L"InjectiveMapType", InjectiveMapType},
  {L"InstanceInit", InstanceInit},
  {L"InstanceInv", InstanceInv},
  {L"InstantSig", InstantSig},
  {L"Interface", Interface},
  {L"IotaExpr", IotaExpr},
  {L"IsExpr", IsExpr},
  {L"IsOfBaseClassExpr", IsOfBaseClassExpr},
  {L"IsOfClassExpr", IsOfClassExpr},
  {L"LambdaExpr", LambdaExpr},
  {L"LetBeSTExpr", LetBeSTExpr},
  {L"LetBeSTStmt", LetBeSTStmt},
  {L"LetExpr", LetExpr},
  {L"LetStmt", LetStmt},
  {L"MapComprehensionExpr", MapComprehensionExpr},
  {L"MapEnumerationExpr", MapEnumerationExpr},
  {L"MapInverseExpr", MapInverseExpr},
  {L"MapOrSeqRef", MapOrSeqRef},
  {L"Maplet", Maplet},
  {L"MatchVal", MatchVal},
  {L"Module", Module},
  {L"Mutex", TAG_TYPE_AS_Mutex},
  {L"MultSetBind", MultSetBind},
  {L"MultTypeBind", MultTypeBind},
  {L"Name", Name},
  {L"NameType", NameType},
  {L"NewExpr", TAG_TYPE_AS_NewExpr},
  {L"NilLit", NilLit},
  {L"NonDetStmt", NonDetStmt},
  {L"OldName", OldName},
  {L"OpBody", TAG_TYPE_AS_OpBody},
  {L"OpType", OpType},
  {L"OptionalType", OptionalType},
  {L"ParameterSig", ParameterSig},
  {L"PartialFnType", PartialFnType},
  {L"PatTypePair", PatTypePair},
  {L"PatternName", PatternName},
  {L"PerObl", PerObl},
  {L"Permission", TAG_TYPE_AS_Permission},
  {L"PreConditionApplyExpr", TAG_TYPE_AS_PreConditionApplyExpr},
  {L"PrefTrace", PrefTrace},
  {L"PrefixExpr", PrefixExpr},
  {L"ProductType", ProductType},
  {L"ProjTrace", ProjTrace},
  // {L"ProofInfo", ProofInfo},
  {L"QsyncTrace", QsyncTrace},
  {L"QuoteLit", QuoteLit},
  {L"QuoteType", QuoteType},
  {L"RealLit", RealLit},
  {L"RecTrapStmt", RecTrapStmt},
  {L"RecordConstructorExpr", RecordConstructorExpr},
  {L"RecordModification", RecordModification},
  {L"RecordModifierExpr", RecordModifierExpr},
  {L"RecordPattern", RecordPattern},
  {L"ReqExpr", ReqExpr},
  {L"ReturnStmt", ReturnStmt},
  {L"SameBaseClassExpr", SameBaseClassExpr},
  {L"SameClassExpr", SameClassExpr},
  {L"SelectStmt", SelectStmt},
  {L"SelfExpr", SelfExpr},
  {L"Seq0Type", Seq0Type},
  {L"Seq1Type", Seq1Type},
  {L"SeqComprehensionExpr", SeqComprehensionExpr},
  {L"SeqConcPattern", SeqConcPattern},
  {L"SeqEnumPattern", SeqEnumPattern},
  {L"SeqEnumerationExpr", SeqEnumerationExpr},
  {L"SeqForLoopStmt", SeqForLoopStmt},
  {L"SeqModifyMapOverrideExpr", SeqModifyMapOverrideExpr},
  {L"SetBind", SetBind},
  {L"SetComprehensionExpr", SetComprehensionExpr},
  {L"SetEnumPattern", SetEnumPattern},
  {L"SetEnumerationExpr", SetEnumerationExpr},
  {L"SetForLoopStmt", SetForLoopStmt},
  {L"SetRangeExpr", SetRangeExpr},
  {L"SetType", SetType},
  {L"SetUnionPattern", SetUnionPattern},
  {L"SpecificationStmt", SpecificationStmt},
  {L"StartListStmt", StartListStmt},
  {L"StartStmt", StartStmt},
  {L"StateDef", StateDef},
  {L"SubSequenceExpr", SubSequenceExpr},
  {L"SyncTrace", SyncTrace},
  {L"TextLit", TextLit},
  {L"ThreadIdExpr", TAG_TYPE_AS_ThreadIdExpr},
  {L"TimeVarDecl", TimeVarDecl},
  {L"TokenConstructorExpr", TokenConstructorExpr},
  {L"TotalFnType", TotalFnType},
  {L"Trace", Trace},
  {L"TraceExpr", TraceExpr},
  {L"Trap", Trap},
  {L"TrapStmt", TrapStmt},
  {L"TupleConstructorExpr", TupleConstructorExpr},
  {L"TuplePattern", TuplePattern},
  {L"TupleSelectExpr", TAG_TYPE_AS_TupleSelectExpr},  
  {L"TypeBind", TypeBind},
  {L"TypeDef", TypeDef},
  {L"TypeJudgementExpr", TAG_TYPE_AS_TypeJudgementExpr},
  {L"TypeName", TypeName},
  {L"TypeVar", TypeVar},
  {L"UndefinedExpr", UndefinedExpr},
  {L"UnionType", UnionType},
  {L"ValueDef", ValueDef},
  {L"WaitingExpr", WaitingExpr},
  {L"WeaveTrace", WeaveTrace},
  {L"WhileLoopStmt", WhileLoopStmt},
#ifdef VICE
  {L"CurTimeExpr", CurTimeExpr},
#endif // VICE


// Definition of tag map for rep.vdmsl and rep.vdmpp 
// Automatically generated in the same way as with common_as.vdm{sl|pp}
  {L"REP`", NewSetOfTags},

  {L"AllTypeRep", AllTypeRep},
  {L"BasicTypeRep", BasicTypeRep},
  {L"CompositeTypeRep", CompositeTypeRep},
  {L"EmptyMapTypeRep", EmptyMapTypeRep},
  {L"EmptySeqTypeRep", EmptySeqTypeRep},
  {L"EmptySetTypeRep", EmptySetTypeRep},
  {L"ExitTypeRep", ExitTypeRep},
  {L"FieldRep", FieldRep},
  {L"GeneralMapTypeRep", GeneralMapTypeRep},
  {L"InjectiveMapTypeRep", InjectiveMapTypeRep},
  {L"InvTypeRep", InvTypeRep},
  {L"NilTypeRep", NilTypeRep},
  {L"ObjRefTypeRep", ObjRefTypeRep},
  {L"OpTypeRep", OpTypeRep},
  {L"PartialFnTypeRep", PartialFnTypeRep},
  {L"PolyTypeRep", PolyTypeRep},
  {L"ProductTypeRep", ProductTypeRep},
  {L"QuoteTypeRep", QuoteTypeRep},
  {L"SeqTypeRep", SeqTypeRep},
  {L"SetTypeRep", SetTypeRep},
  {L"TmpTypeRep", TmpTypeRep},
  {L"TotalFnTypeRep", TotalFnTypeRep},
  {L"TypeNameRep", TypeNameRep},
  {L"TypeParRep", TypeParRep},
  {L"TypeVarRep", TypeVarRep},
  {L"UnionTypeRep", UnionTypeRep},
  {L"UnitTypeRep", UnitTypeRep},

  // Tag definitions for Position info and coverage counting
  {L"", NewSetOfTags},

  {L"PosInfo", PosInfo},

  {L"",0}
};



Map NameMap()
{
  Map m;
  int i, curr_set=0;

  // Go through the entire array of tag definitions and make the 
  // tag map.
  for(i=0; TAG_TAB[i].TagNum!=0; i++) {
    if(TAG_TAB[i].TagNum == NewSetOfTags) {
      curr_set = i;  // remember the position of the module name
    }
    else {
      m.Insert(Text(wstring(TAG_TAB[curr_set].Tag) + 
                    wstring(TAG_TAB[i].Tag)), Int(TAG_TAB[i].TagNum));
    }
  }
  return m;
}


