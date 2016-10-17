#ifndef __main_tag_h__
#define __main_tag_h__

#include "metaiv.h"

//####################################################################
//############# TAGS
//####################################################################


//#################################
//########## MODULE TAGS ##########
//#################################

#define CG_OFFSET                10000
#define DYNSEM_OFFSET           300000 
#define SEM_OFFSET              400000 
#define GLOBAL_OFFSET           500000 
#define STATSEM_OFFSET          600000
#define SPECMNG_OFFSET          900000
#define PARSER_OFFSET          1200000
#define COMMON_OFFSET          1300000
#define REP_OFFSET             1400000
#define PRETTY_PRINTER_OFFSET  1500000

// diverse offset. E.g. hand coded quote tags.
#define DIV_OFFSET             1600000

#define DEPENDENCY_OFFSET      1700000
#define UMLMAPPER_OFFSET       1800000
#define CONTEXT_OFFSET         1900000
#define POS_OFFSET             2000000
#define MANGLE_OFFSET          2010000
#define UTIL_OFFSET            2020000

#define TAG_INSTRTP            2100000
#define TAG_STKM               2150000
#define TAG_SCHDTP             2200000
#define TAG_DEBUGTP            2205000

#ifdef VICE
#define TAG_TIMEPARSER         2250000 
#define TAG_TIMEMAP            2300000
#define TAG_TIMETRACETP        2350000
#define TAG_VC                 2360000
#endif //VICE

#define TAG_CGTYPES            2400000
#define TAG_ServiceManagerDesc 2450000

#define J2V_OFFSET             2500000
#define POG_OFFSET             3000000
#define POG_AUX_OFFSET         3100000
#define POG_PRETTY_OFFSET      3200000

#define TRANS_OFFSET           3300000

#define POG_TEST_OFFSET        3400000

#define CMD_OFFSET             3500000
#define XMI_OFFSET             3600000

/////////////////////////////////////////////////////////
////// AS + REP + CI + POS
////////////////////////////////////////////////////////

#define TAG_AS     COMMON_OFFSET
#define TAG_REP    REP_OFFSET
#define TAG_CI     CONTEXT_OFFSET
#define TAG_POS    POS_OFFSET
#define TAG_MANGLE MANGLE_OFFSET
#define TAG_UTIL   UTIL_OFFSET

//////////////////////////////////////
// CODE GENERATOR 
//////////////////////////////////////

#define TAG_GLOBAL          (CG_OFFSET + 0)
#define TAG_CGMAIN          (CG_OFFSET + 10000)
#define TAG_EXPR            (CG_OFFSET + 20000)
#define TAG_PM              (CG_OFFSET + 30000)
#define TAG_SD              (CG_OFFSET + 40000)
#define TAG_TPGEN           (CG_OFFSET + 50000)
#define TAG_CGAUX           (CG_OFFSET + 60000)
#define TAG_FD              (CG_OFFSET + 70000)
//#define TAG_CPP    100000 removed in order to insure that all
// record tags in CPP are explictly defined_as(DO AS module)
#define TAG_STMT            (CG_OFFSET + 100000)
#define TAG_VD              (CG_OFFSET + 110000)
#define TAG_BC              (CG_OFFSET + 120000)
#define TAG_DS              (CG_OFFSET + 130000)
#define TAG_MOD             (CG_OFFSET + 140000)
#define TAG_FVD             (CG_OFFSET + 150000)
#define TAG_CLASS           (CG_OFFSET + 160000)     
#define TAG_CONC            (CG_OFFSET + 165000)     
#define TAG_CPP             (CG_OFFSET + 170000)
#define TAG_BCCI            (CG_OFFSET + 175000)
#define TAG_CPP_END         (CG_OFFSET + 179999)

#define OFFSET_QUOTE_CG     (CG_OFFSET + 10000)
#define OFFSET_QUOTE_EXPR   (CG_OFFSET + 20000)
#define OFFSET_QUOTE_PM     (CG_OFFSET + 30000)
#define OFFSET_QUOTE_SD     (CG_OFFSET + 40000)
#define OFFSET_QUOTE_TPGEN  (CG_OFFSET + 50000)
#define OFFSET_QUOTE_AUX    (CG_OFFSET + 60000)
#define OFFSET_QUOTE_FD     (CG_OFFSET + 70000)
#define OFFSET_QUOTE_POS    (CG_OFFSET + 80000)
//#define TAG_CPP    100000 removed in order to insure that all
// record tags in CPP are explictly defined_as(DO AS module)
#define OFFSET_QUOTE_STMT   (CG_OFFSET + 100000)
#define OFFSET_QUOTE_VD     (CG_OFFSET + 110000)
#define OFFSET_QUOTE_BC     (CG_OFFSET + 120000)
#define OFFSET_QUOTE_DS     (CG_OFFSET + 130000)
#define OFFSET_QUOTE_MOD    (CG_OFFSET + 140000)
#define OFFSET_QUOTE_FVD    (CG_OFFSET + 150000)
#define OFFSET_QUOTE_CLASS  (CG_OFFSET + 160000)
#define OFFSET_QUOTE_CPP    (CG_OFFSET + 170000)

     
//#define OFFSET_QUOTE_CPP (CG_OFFSET + 180000)
     
//////////////////////////////////////
// SPEC MANAGER
//////////////////////////////////////

#define TAG_AST           (SPECMNG_OFFSET +  100)
#define TAG_BaseTools     (SPECMNG_OFFSET +  200)
#define TAG_BatchErr      (SPECMNG_OFFSET +  300)
#define TAG_CGRepository  (SPECMNG_OFFSET +  400)
#define TAG_Dependency    (SPECMNG_OFFSET +  500)
#define TAG_ErrorState    (SPECMNG_OFFSET +  600) 
#define TAG_Errors        (SPECMNG_OFFSET +  700) 
#define TAG_FileStatus    (SPECMNG_OFFSET +  800) 
#define TAG_FlatSes       (SPECMNG_OFFSET +  900) 
#define TAG_Interface     (SPECMNG_OFFSET + 1100) 
#define TAG_NonPrintErr   (SPECMNG_OFFSET + 1200) 
#define TAG_NoneSes       (SPECMNG_OFFSET + 1300) 
#define TAG_Options       (SPECMNG_OFFSET + 1400) 
#define TAG_ProjectTypes  (SPECMNG_OFFSET + 1500) 
#define TAG_PromptErr     (SPECMNG_OFFSET + 1600) 
#define TAG_RepDatabase   (SPECMNG_OFFSET + 1700) 
#define TAG_Repository    (SPECMNG_OFFSET + 1800) 
#define TAG_SABaseTools   (SPECMNG_OFFSET + 1900) 
#define TAG_ScriptErr     (SPECMNG_OFFSET + 2000) 
#define TAG_StateStore    (SPECMNG_OFFSET + 2100) 
#define TAG_StatusInfo    (SPECMNG_OFFSET + 2200) 
#define TAG_StructSes     (SPECMNG_OFFSET + 2300) 
#define TAG_ToolColleague (SPECMNG_OFFSET + 2400) 
#define TAG_ToolKit       (SPECMNG_OFFSET + 2500) 
#define TAG_ToolMediator  (SPECMNG_OFFSET + 2600) 
#define TAG_UnitStatus    (SPECMNG_OFFSET + 2700) 
#define TAG_UpdateProject (SPECMNG_OFFSET + 2800) 
#define TAG_UpdateSes     (SPECMNG_OFFSET + 2900) 
#define TAG_VDMUnitElem   (SPECMNG_OFFSET + 3000) 
#define TAG_UMLTool       (SPECMNG_OFFSET + 3100)

#define TAG_TYPE_UMLTool_State      (TAG_UMLTool + 0)
#define TAG_TYPE_UMLTool_Modified   (TAG_UMLTool + 1)
#define TAG_TYPE_UMLTool_Deleted    (TAG_UMLTool + 2)
#define TAG_TYPE_UMLTool_Added      (TAG_UMLTool + 3)
#define TAG_TYPE_UMLTool_None       (TAG_UMLTool + 4)
#define TAG_TYPE_UMLTool_Unknown    (TAG_UMLTool + 5)

////////////////////////////////////////////////
/// DEPENDENCY
////////////////////////////////////////////////

#define TAG_DEP           (DEPENDENCY_OFFSET + 0)



////////////////////////////////////////////////
/// STATIC SEMANTICS TAGS
////////////////////////////////////////////////

#define TAG_SSENV  (STATSEM_OFFSET + 0)

#define TAG_TYPE_SSENV_TypeRepElem         (TAG_SSENV + 0)
#define TAG_TYPE_SSENV_TagRepElem          (TAG_SSENV + 1)
#define TAG_TYPE_SSENV_TagRepEnv           (TAG_SSENV + 2)
#define TAG_TYPE_SSENV_NameInfo            (TAG_SSENV + 6)
#define TAG_TYPE_SSENV_ParseTypeInfo       (TAG_SSENV + 7) 
#define TAG_TYPE_SSENV_ModuleRep           (TAG_SSENV + 8) 
#define TAG_TYPE_SSENV_ParSigRep           (TAG_SSENV + 9) 
#define TAG_TYPE_SSENV_ExpSigRep           (TAG_SSENV + 10)
#define TAG_TYPE_SSENV_PolyTypeRep         (TAG_SSENV + 11)
#define TAG_TYPE_SSENV_PolyTypeRepElem     (TAG_SSENV + 12) 
#define TAG_TYPE_SSENV_TmpTypeRep          (TAG_SSENV + 13) 
#define TAG_TYPE_SSENV_ExitTypeRep         (TAG_SSENV + 14)
#ifdef VDMPP
#define TAG_TYPE_SSENV_AccessTypeRep       (TAG_SSENV + 15)
#define TAG_TYPE_SSENV_AccessFnTypeRep     (TAG_SSENV + 16)
#define TAG_TYPE_SSENV_AccessPolyTypeRep   (TAG_SSENV + 17)
#define TAG_TYPE_SSENV_AccessOpTypeRep     (TAG_SSENV + 18)
#define TAG_TYPE_SSENV_AccessFieldRep      (TAG_SSENV + 19)
#endif //VDMPP
#define TAG_TYPE_SSENV_ErrMsg              (TAG_SSENV + 20)
#define TAG_TYPE_SSENV_FunctionInfo        (TAG_SSENV + 21)


//////////////////////////////////////////////////////
//// PARSER TAGS
/////////////////////////////////////////////////////

//#define TAG_TYPE_PARSER    (PARSER_OFFSET+0)

//#define TAG_TYPE_PARSER_PosInfo     (-2)
//#define TAG_TYPE_PARSER_SyntaxError (TAG_TYPE_PARSER+1)
//#define TAG_TYPE_PARSER_MSCall      (TAG_TYPE_PARSER+2)
//#define TAG_TYPE_PARSER_LastRes     (TAG_TYPE_PARSER+3)
//#define TAG_TYPE_PARSER_ErrMsg      (TAG_TYPE_PARSER+4)


#define TAG_TYPE_PRETTY_PRINTER  (PRETTY_PRINTER_OFFSET+0)

#define TAG_TYPE_PRETTY_PRINTER_FnDef      (TAG_TYPE_PRETTY_PRINTER+1)
#define TAG_TYPE_PRETTY_PRINTER_OpDef      (TAG_TYPE_PRETTY_PRINTER+2)
#define TAG_TYPE_PRETTY_PRINTER_Texbreak   (TAG_TYPE_PRETTY_PRINTER+3)
#define TAG_TYPE_PRETTY_PRINTER_ModOrder   (TAG_TYPE_PRETTY_PRINTER+4)
#define TAG_TYPE_PRETTY_PRINTER_ClassOrder (TAG_TYPE_PRETTY_PRINTER+5)     
#define TAG_TYPE_PRETTY_PRINTER_InstVarDef (TAG_TYPE_PRETTY_PRINTER+6)     
#define TAG_TYPE_PRETTY_PRINTER_TimeVarDef (TAG_TYPE_PRETTY_PRINTER+6)     
#define TAG_TYPE_PRETTY_PRINTER_SyncDef    (TAG_TYPE_PRETTY_PRINTER+8)     
#define TAG_TYPE_PRETTY_PRINTER_ThreadDef  (TAG_TYPE_PRETTY_PRINTER+9)     
#define TAG_TYPE_PRETTY_PRINTER_Traces     (TAG_TYPE_PRETTY_PRINTER+10)     


//////////////////////////////////////////////////////
//// DYNAMIC SEMANTICS TAGS
//////////////////////////////////////////////////////


#define TAG_TYPE_DYNSEM_GLOBAL   (DYNSEM_OFFSET+0)
#define TAG_TYPE_DYNSEM_SEM      (DYNSEM_OFFSET+1000)
#define TAG_TYPE_DYNSEM_COMPILE  (DYNSEM_OFFSET+2000)

#define TAG_TYPE_GLOBAL                (SEM_OFFSET + 0)
#define TAG_TYPE_SEM                   (SEM_OFFSET + 1000)

#define TAG_SEM                        (SEM_OFFSET + 2000)

#define TAG_TYPE_SEM_LOC               (TAG_TYPE_SEM + 0)
#define TAG_TYPE_SEM_MAP               (TAG_TYPE_SEM + 1)
#define TAG_TYPE_SEM_NIL               (TAG_TYPE_SEM + 2)
#define TAG_TYPE_SEM_NUM               (TAG_TYPE_SEM + 3)
#define TAG_TYPE_SEM_REC               (TAG_TYPE_SEM + 4)
#define TAG_TYPE_SEM_SEQ               (TAG_TYPE_SEM + 5)
#define TAG_TYPE_SEM_SET               (TAG_TYPE_SEM + 6)
#define TAG_TYPE_SEM_BOOL              (TAG_TYPE_SEM + 7)
#define TAG_TYPE_SEM_CHAR              (TAG_TYPE_SEM + 8)
#define TAG_TYPE_SEM_CONT              (TAG_TYPE_SEM + 9)
#define TAG_TYPE_SEM_DLFN              (TAG_TYPE_SEM + 10)
#define TAG_TYPE_SEM_DLOP              (TAG_TYPE_SEM + 11)
#define TAG_TYPE_SEM_EXIT              (TAG_TYPE_SEM + 12)
//#define TAG_TYPE_SEM_DLVAL             (TAG_TYPE_SEM + 13)
#define TAG_TYPE_SEM_QUOTE             (TAG_TYPE_SEM + 14)
#define TAG_TYPE_SEM_TOKEN             (TAG_TYPE_SEM + 15)
#define TAG_TYPE_SEM_TUPLE             (TAG_TYPE_SEM + 16)
#define TAG_TYPE_SEM_UNDEF             (TAG_TYPE_SEM + 17)
#define TAG_TYPE_SEM_ValTp             (TAG_TYPE_SEM + 18)
#define TAG_TYPE_SEM_BlkEnv            (TAG_TYPE_SEM + 19)
#define TAG_TYPE_SEM_ExplFN            (TAG_TYPE_SEM + 20)
#define TAG_TYPE_SEM_ExplOP            (TAG_TYPE_SEM + 21)
#define TAG_TYPE_SEM_ImplFN            (TAG_TYPE_SEM + 22)
#define TAG_TYPE_SEM_ImplOP            (TAG_TYPE_SEM + 23)
#define TAG_TYPE_SEM_RETURN            (TAG_TYPE_SEM + 24)
#define TAG_TYPE_SEM_OBJ_uRef          (TAG_TYPE_SEM + 25)
#define TAG_TYPE_SEM_ExplPOLY          (TAG_TYPE_SEM + 26)
#define TAG_TYPE_SEM_ImplPOLY          (TAG_TYPE_SEM + 27)
#define TAG_TYPE_SEM_CompExplFN        (TAG_TYPE_SEM + 28)
#define TAG_TYPE_SEM_OBJ               (TAG_TYPE_SEM + 29)
#define TAG_TYPE_SEM_History           (TAG_TYPE_SEM + 30)
#define TAG_TYPE_SEM_OverOPFN          (TAG_TYPE_SEM + 31)

#define TAG_TYPE_GLOBAL_State          (TAG_TYPE_GLOBAL + 0)
#define TAG_TYPE_GLOBAL_SigmaMO        (TAG_TYPE_GLOBAL + 1)
#define TAG_TYPE_GLOBAL_SigmaEXP       (TAG_TYPE_GLOBAL + 2)
#define TAG_TYPE_GLOBAL_SigmaIMO       (TAG_TYPE_GLOBAL + 3)
#define TAG_TYPE_GLOBAL_SigmaDEFS      (TAG_TYPE_GLOBAL + 4)
#define TAG_TYPE_GLOBAL_Start          (TAG_TYPE_GLOBAL + 5)
#define TAG_TYPE_GLOBAL_OBJ_uDesc      (TAG_TYPE_GLOBAL + 6)
#define TAG_TYPE_GLOBAL_OBJscope       (TAG_TYPE_GLOBAL + 7)
#define TAG_TYPE_GLOBAL_ObjRefType     (TAG_TYPE_GLOBAL + 8)
#define TAG_TYPE_GLOBAL_SigmaClass     (TAG_TYPE_GLOBAL + 9)
#define TAG_TYPE_GLOBAL_ChanUsage      (TAG_TYPE_GLOBAL + 10)

//############################
//###### Stack Machine #######
//############################

#define TAG_TYPE_STKM_Guard            (TAG_STKM + 13)
#define TAG_TYPE_STKM_sigma            (TAG_STKM + 14)
#define TAG_TYPE_STKM_BreakAtNextCid   (TAG_STKM + 15)
#define TAG_TYPE_STKM_Breakpoint       (TAG_STKM + 16)
#define TAG_TYPE_STKM_CallStackItem    (TAG_STKM + 17)
#define TAG_TYPE_STKM_DebugCmd         (TAG_STKM + 18)
#define TAG_TYPE_STKM_Continue         (TAG_STKM + 19)
#define TAG_TYPE_STKM_Step             (TAG_STKM + 20)
#define TAG_TYPE_STKM_StepIn           (TAG_STKM + 21)
#define TAG_TYPE_STKM_StepOver         (TAG_STKM + 22)
#define TAG_TYPE_STKM_FieldRef         (TAG_STKM + 23)
#define TAG_TYPE_STKM_MapOrSeqRef      (TAG_STKM + 24)
#define TAG_TYPE_STKM_MatchVal         (TAG_STKM + 25)
#define TAG_TYPE_STKM_PatternName      (TAG_STKM + 26)
#define TAG_TYPE_STKM_SeqConcPattern   (TAG_STKM + 27)
#define TAG_TYPE_STKM_SetUnionPattern  (TAG_STKM + 28)
#define TAG_TYPE_STKM_TuplePattern     (TAG_STKM + 29)
#define TAG_TYPE_STKM_SetEnumPattern   (TAG_STKM + 30)
#define TAG_TYPE_STKM_SeqEnumPattern   (TAG_STKM + 31)
#define TAG_TYPE_STKM_RecordPattern    (TAG_STKM + 32)
#define TAG_TYPE_STKM_MapEnumPattern   (TAG_STKM + 33)
#define TAG_TYPE_STKM_MapMergePattern  (TAG_STKM + 34)
#define TAG_TYPE_STKM_MapletPattern    (TAG_STKM + 35)
#define TAG_TYPE_STKM_Interrupt        (TAG_STKM + 36)
#define TAG_TYPE_STKM_EndOfSlice       (TAG_STKM + 37)
#define TAG_TYPE_STKM_Success          (TAG_STKM + 38)
#define TAG_TYPE_STKM_EvaluatorStatus  (TAG_STKM + 39)
#define TAG_TYPE_STKM_Trap             (TAG_STKM + 40)
#define TAG_TYPE_STKM_Finish           (TAG_STKM + 41)
#define TAG_TYPE_STKM_SingleStep       (TAG_STKM + 42)
#define TAG_TYPE_STKM_CPUSigma         (TAG_STKM + 43)
#define TAG_TYPE_STKM_BUSSigma         (TAG_STKM + 44)
#define TAG_TYPE_STKM_Message          (TAG_STKM + 45)
#define TAG_TYPE_STKM_MessageInfo      (TAG_STKM + 46)
#define TAG_TYPE_STKM_MessageResponse  (TAG_STKM + 47)
#define TAG_TYPE_STKM_EndOfCPUSlice    (TAG_STKM + 48)
#define TAG_TYPE_STKM_MessageBroadcast (TAG_STKM + 49)
#define TAG_TYPE_STKM_MessageReq       (TAG_STKM + 50)
#define TAG_TYPE_STKM_MessageReplyReq  (TAG_STKM + 51)

//############################
//########## UML MAPPER ######
//############################

#define TAG_AUML     (UMLMAPPER_OFFSET + 0)
#define TAG_BUILD    (UMLMAPPER_OFFSET + 10000)
#define TAG_MERGE    (UMLMAPPER_OFFSET + 20000)
#define TAG_UMLERR   (UMLMAPPER_OFFSET + 30000)
#define TAG_ASTMERGE (UMLMAPPER_OFFSET + 40000)

//############################
//##########  Trans  #########
//############################

#define TAG_AEX    (TRANS_OFFSET +  5000)
#define TAG_ASX    (TRANS_OFFSET +  10000)
#define TAG_AX     (TRANS_OFFSET +  15000)
#define TAG_FEX    (TRANS_OFFSET +  20000)
#define TAG_FSX    (TRANS_OFFSET +  25000)
#define TAG_FX     (TRANS_OFFSET +  30000)
#define TAG_OPP    (TRANS_OFFSET +  35000)
#define TAG_SB     (TRANS_OFFSET +  40000)
#define TAG_STRIP  (TRANS_OFFSET +  45000)
#define TAG_XF     (TRANS_OFFSET +  50000)

//############################
//##########  Java2Vdm  ######
//############################

#define TAG_J2V       (J2V_OFFSET +  0)
#define TAG_J2VAUX    (J2V_OFFSET +  10000)
#define TAG_J2VBVDM   (J2V_OFFSET +  20000)
#define TAG_J2VCLASS  (J2V_OFFSET +  30000)
#define TAG_J2VENV    (J2V_OFFSET +  40000)
#define TAG_J2VEXPR   (J2V_OFFSET +  50000)
#define TAG_J2VNS     (J2V_OFFSET +  60000)
#define TAG_J2VOP     (J2V_OFFSET +  70000)
#define TAG_J2VSTMT   (J2V_OFFSET +  80000)
#define TAG_J2VTYPE   (J2V_OFFSET +  90000)
#define TAG_JSSAUX    (J2V_OFFSET + 100000)
#define TAG_JSSDEF    (J2V_OFFSET + 110000)
#define TAG_JSSENV    (J2V_OFFSET + 120000)
#define TAG_JSSERRMSG (J2V_OFFSET + 130000)
#define TAG_JSSEXPR   (J2V_OFFSET + 140000)
#define TAG_JSSJAVA   (J2V_OFFSET + 150000)
#define TAG_JSSREP    (J2V_OFFSET + 160000)
#define TAG_JSSSTMT   (J2V_OFFSET + 170000)
#define TAG_JSSVCM1   (J2V_OFFSET + 180000)
#define TAG_JSSERR    (J2V_OFFSET + 190000)
#define TAG_JAVAERR   (J2V_OFFSET + 200000)
#define TAG_J2VERR    (J2V_OFFSET + 210000)
#define TAG_J2VERRMSG (J2V_OFFSET + 220000)


//############################
//########    POG       ######
//############################

#define TAG_POG_uEXPR       (POG_OFFSET + 0)
#define TAG_CHECK           (POG_OFFSET + 10000)
#define TAG_CON_uENV        (POG_OFFSET + 20000)
#define TAG_FRESH           (POG_OFFSET + 30000)
#define TAG_GEN_uINV        (POG_OFFSET + 40000)
#define TAG_GEN_uPO         (POG_OFFSET + 50000)
#define TAG_POGTP           (POG_OFFSET + 60000)
#define TAG_POG_uDEF        (POG_OFFSET + 70000)
#define TAG_POG_uMEDIATOR   (POG_OFFSET + 80000)
#define TAG_POG_uPAT        (POG_OFFSET + 90000)

//############################
//########  POG aux  ######
//############################

#define TAG_PURE_uPAT     (POG_AUX_OFFSET + 0)
#define TAG_CAST          (POG_AUX_OFFSET + 10000)
#define TAG_CI_uAUX       (POG_AUX_OFFSET + 20000)
#define TAG_REP2TYPE      (POG_AUX_OFFSET + 30000)
#define TAG_SPEC_uAST     (POG_AUX_OFFSET + 40000)
#define TAG_TYPE2REP      (POG_AUX_OFFSET + 50000)
#define TAG_PID           (POG_AUX_OFFSET + 60000)


//############################
//########  POG Pretty  ######
//############################

#define TAG_PP_uTAR_uDEF        (POG_PRETTY_OFFSET + 0)
#define TAG_PP_uTAR_uEXPR       (POG_PRETTY_OFFSET + 1000)
#define TAG_PP_uTAR_uPAT        (POG_PRETTY_OFFSET + 2000)
#define TAG_PP_uTAR_uPO         (POG_PRETTY_OFFSET + 3000)
#define TAG_PP_uTAR_uTYPE       (POG_PRETTY_OFFSET + 4000)
#define TAG_PP_uTRA_uDEF        (POG_PRETTY_OFFSET + 5000)
#define TAG_PP_uTRA_uEXPR       (POG_PRETTY_OFFSET + 6000)
#define TAG_PP_uTRA_uPAT        (POG_PRETTY_OFFSET + 7000)
#define TAG_PP_uTRA_uPO         (POG_PRETTY_OFFSET + 8000)
#define TAG_PP_uTRA_uTYPE       (POG_PRETTY_OFFSET + 9000)
#define TAG_PP_uMEDIATOR        (POG_PRETTY_OFFSET + 10000)

//############################
//########  POG TEST    ######
//############################

#define TAG_POG_TEST            (POG_TEST_OFFSET + 0)

//############################
//### Hand coded quotes
//############################
//
//The quotes that both occur in 
//the AS and the CPP module
//are defined here, such that
//they are uniquely defined no
//matter in which order the header
//files of these modules are
//included.

#define TAG_quote_STRUCT  (DIV_OFFSET + 0)
#define TAG_quote_EQ      (DIV_OFFSET + 1)
#define TAG_quote_OR      (DIV_OFFSET + 2)
#define TAG_quote_AND     (DIV_OFFSET + 3)
#define TAG_quote_INTEGER (DIV_OFFSET + 4)
#define TAG_quote_NAT     (DIV_OFFSET + 5)
#define TAG_quote_RAT     (DIV_OFFSET + 6)
#define TAG_quote_CHAR    (DIV_OFFSET + 7)
#define TAG_quote_REAL    (DIV_OFFSET + 8)
#define TAG_quote_TOKEN   (DIV_OFFSET + 9)
#define TAG_quote_NATONE  (DIV_OFFSET + 10)
#define TAG_quote_BOOLEAN (DIV_OFFSET + 11)

//STATSEM quotes
#define TAG_quote_EMPTYMAP (DIV_OFFSET + 104 ) // Defined in statsem expr.vdm.
#define TAG_quote_PURE     (DIV_OFFSET + 105 ) // Defined in statem def.vdm
#define TAG_quote_INV      (DIV_OFFSET + 106 ) // Defined in statem def.vdm
#define TAG_quote_POST     (DIV_OFFSET + 107 ) // Defined in statem def.vdm
#define TAG_quote_PRE      (DIV_OFFSET + 108 ) // Defined in statem def.vdm
#define TAG_quote_REN      (DIV_OFFSET + 109) // Defined in statem def.vdm
#define TAG_quote_OP       (DIV_OFFSET + 110) // Defined in statem test.vdm
#define TAG_quote_FUN      (DIV_OFFSET + 111) // Defined in statem test.vdm
#define TAG_quote_VAL      (DIV_OFFSET + 112) // Defined in statem aux.vdm
#define TAG_quote_PAR      (DIV_OFFSET + 113) // Defined in statem aux.vdm
#define TAG_quote_CUR      (DIV_OFFSET + 114) // Defined in statem env.vdm
#define TAG_quote_OLD      (DIV_OFFSET + 115) // Defined in statem env.vdm
#define TAG_quote_VALUE    (DIV_OFFSET + 116) // Defined in statem env.vdm
#define TAG_quote_IMPL     (DIV_OFFSET + 117) // Defined in statem env.vdm
#define TAG_quote_NOTOVERLAP (DIV_OFFSET + 118) // Defined in statem aux.vdm
#define TAG_quote_TYPE     (DIV_OFFSET + 119) // Defined in statem aux.vdm
#define TAG_quote_STATE    (DIV_OFFSET + 120) // Defined in statem aux.vdm
#define TAG_quote_TAG      (DIV_OFFSET + 121) // Defined in statem aux.vdm
#define TAG_quote_PREM     (DIV_OFFSET + 122) // Defined in statem aux.vdm
#define TAG_quote_PTHREAD  (DIV_OFFSET + 123) // Defined in statem def.vdm
#define TAG_quote_REM      (DIV_OFFSET + 124) // Defined in statem def.vdm
#define TAG_quote_ADD      (DIV_OFFSET + 125) // Defined in statem def.vdm
#define TAG_quote_VAR      (DIV_OFFSET + 126) // Defined in statem def.vdm
#define TAG_quote_PERM     (DIV_OFFSET + 127) // Defined in statem def.vdm
#define TAG_quote_DEF      (DIV_OFFSET + 128) // Defined in statem def.vdm
#define TAG_quote_POS      (DIV_OFFSET + 129) // Defined in statem def.vdm
#define TAG_quote_OK       (DIV_OFFSET + 130) // Defined in statem def.vdm
#define TAG_quote_NONE     (DIV_OFFSET + 131) // Defined in statem def.vdm
#define TAG_quote_INIT     (DIV_OFFSET + 132) // Defined in statem def.vdm
#define TAG_quote_EXCEP    (DIV_OFFSET + 133) // Defined in statem def.vdm
#define TAG_quote_PER      (DIV_OFFSET + 134) // Defined in statem vcm.vdm
#define TAG_quote_DECL     (DIV_OFFSET + 135) // Defined in statem vcm.vdm
#define TAG_quote_OBJECT   (DIV_OFFSET + 136) // Defined in statem env.vdm
#define TAG_quote_LOCAL    (DIV_OFFSET + 137) // Defined in statem env.vdm
#define TAG_quote_GLOBAL   (DIV_OFFSET + 138) // Defined in statem env.vdm

// DYNSEM quotes
//#define TAG_quote_APPROX     (DIV_OFFSET + 300) 20060105
#define TAG_quote_INPUT      (DIV_OFFSET + 301)

// TPGEN quotes
#define TAG_quote_CONST      (DIV_OFFSET + 400)
#define TAG_quote_SET        (DIV_OFFSET + 401)
#define TAG_quote_SEQ        (DIV_OFFSET + 402)
#define TAG_quote_MAP        (DIV_OFFSET + 403)
#define TAG_quote_PRODUCT    (DIV_OFFSET + 404)
#define TAG_quote_COMPOSITE  (DIV_OFFSET + 405)
#define TAG_quote_OBJREF     (DIV_OFFSET + 406)
#define TAG_quote_H          (DIV_OFFSET + 407)
#define TAG_quote_CC         (DIV_OFFSET + 408)
#define TAG_quote_ANONYM     (DIV_OFFSET + 409)
#define TAG_quote_CCANONYM   (DIV_OFFSET + 410)

#define TAG_TYPE_TOOLS_OriginTag (DIV_OFFSET+500) // Tag Origin defined
                                                  // in tools.cc
#define TAG_TYPE_PROJECTVAL_FlatStore (DIV_OFFSET+501) // Tag FlatStore in
                                                       //projectval.

//############################
//########   TOOL CMD   ######
//############################
 
// The following tags are used to define the command Records used by
// the GUI to give commands to the ToolboxThread
#define TAG_TYPE_CMD_LoadProject       CMD_OFFSET + 1
#define TAG_TYPE_CMD_SyntaxCheck       CMD_OFFSET + 2
#define TAG_TYPE_CMD_TypeCheck         CMD_OFFSET + 3
#define TAG_TYPE_CMD_CodeGenerate      CMD_OFFSET + 4
#define TAG_TYPE_CMD_JavaGenerate      CMD_OFFSET + 5
#define TAG_TYPE_CMD_PrettyPrint       CMD_OFFSET + 6
#define TAG_TYPE_CMD_VDMGenerate       CMD_OFFSET + 7
#define TAG_TYPE_CMD_ExecuteCommand    CMD_OFFSET + 8
#define TAG_TYPE_CMD_AddFiles          CMD_OFFSET + 9
#define TAG_TYPE_CMD_RemoveFiles       CMD_OFFSET + 10
#define TAG_TYPE_CMD_SaveProjectAs     CMD_OFFSET + 11
#define TAG_TYPE_CMD_NewUnnamedProject CMD_OFFSET + 12
#define TAG_TYPE_CMD_ResetMapper       CMD_OFFSET + 13
#define TAG_TYPE_CMD_SetModelName      CMD_OFFSET + 14
#define TAG_TYPE_CMD_UMLDiff           CMD_OFFSET + 15
#define TAG_TYPE_CMD_GenPOG            CMD_OFFSET + 16
#define TAG_TYPE_CMD_CallAPI           CMD_OFFSET + 17

#endif // __main_tag_h__

