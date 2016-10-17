/***
*  * WHAT
*  *    Integer Record Tag definitions and error messages for
*  *    the interpreter
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/rt_errmsg.h,v $
*  * VERSION
*  *    $Revision: 1.33 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:10 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __rt_errmsg__
#define __rt_errmsg__

#include "metaiv.h"
#include <iostream>
#include "CI.h"
#include "ProjectTypes.h"
#include <string>
#include "SEM.h"

class RunTimeMsg {
public:
  std::wstring Where;
  std::wstring What;
  Generic exp_v;
  Generic exp_tp;
  bool printed;

  RunTimeMsg() {}
};


enum RT_ThrowType {
  ERR_IP = 100,
  ERR_CG,                 // fatal error in code generator
  ERR_CouldNotReadFile,
  ERR_CouldNotSaveFile,
  ERR_CouldNotOpenFile,
  ERR_CouldNotCloseFile,
  ERR_RTF_Parser,         // fatal error occured while parsing RTF file
  ERR_TC
}; 

class RTERR
{
private:
  static RunTimeMsg RTErrMsg;

public:
  static void InitError(const wchar_t * Where, const int What,
                        const Generic & exp_v, const Generic & exp_tp, const TYPE_CI_ContextId & cid,
                        const Sequence & txts);
  static void ReportError(const wchar_t * Where, const int What,
                          const Generic & exp_v, const Generic & exp_tp, const TYPE_CI_ContextId & cid,
                          const Sequence & txts);
  static void ReportWarning(const wchar_t * Where, const int What, const TYPE_CI_ContextId & cid,
                            const Sequence & txts);
  static void OutputToRterr(const std::wstring & err);
  static void Reset();
  static bool RunTimeErrorOccured();
  static void PrintError(wostream & wos);
  static std::wstring GetWhat();
  static std::wstring GetWhere();

  static TYPE_SEM_VAL ErrorVal(const wchar_t * Where, const int What,
                               const Generic & semval, const Generic & type, const Sequence & txts);
  static void Error(const wchar_t * Where, const int What,
                    const Generic & semval, const Generic & type, const Sequence & txts);

  static void PrintRunTimeError(wostream & wos);
  static TYPE_ProjectTypes_ErrMsg GetErrMsg();
  static wstring GetErrorMessage(int num);
  static string GetErrorMessageJP(int num);
  static SEQ<Char> GetWhatStr(int num);
  static SEQ<Char> GetWhatStrJP(int num);
  static int GetErrorNum(const Sequence & str);
};


// These should be prefixed with RTERR or something to avoid name clases!
// 10 May. 1999 15:48 -- Jesper K. Pedersen
// -> #define BOOL_EXPECTED L"1: A boolean was expected."
// -> #define INT_EXPECTED  L"2: An integer was expected."
// -> #define NUM_EXPECTED  L"3: A number was expected"
// -> #define SET_EXPECTED L"4: A set was expected"
// -> #define SEQ_EXPECTED L"5: A sequence was expected"
// -> #define REC_EXPECTED L"6: A record was expected"
// -> #define MAP_EXPECTED L"7: A map was expected"
// -> #define TUPLE_EXPECTED L"8: A tuple was expected"
// -> #define SYMB_LIT_EXPECTED L"9: A symbolic literal was expected"
// -> //#define TEXT_EXPECTED L"10: A sequence of chars was expected"
// -> #define TWO_BOOL_EXPECTED L"11: Two booleans were expected"
// -> #define TWO_INT_EXPECTED L"12: Two integers were expected"
// -> #define TWO_NUM_EXPECTED L"13: Two numbers were expected"
// -> #define TWO_SET_EXPECTED L"14: Two sets were expected"
// -> #define TWO_SEQ_EXPECTED L"15: Two sequences were expected"
// -> #define MAP_OR_SEQ_EXPECTED L"16: A map or a sequence was expected"
// -> #define TWO_MAP_EXPECTED L"17: Two maps were expected"
// -> #define SET_AND_MAP_EXPECTED L"18: A set and a map were expected"
// -> #define SEQ_AND_INT_EXPECTED L"19: A sequence and two integers were expected"
// -> #define PATTERNNAME_EXPECTED L"20: The binding pattern must evaluate to one patternname"
// -> #define CHAR_EXPECTED L"21: A wchar_t was expected"
// -> #define ALL_INTS_EXPECTED L"22: The binding set can only contain integers"
// -> #define ALL_NUMS_EXPECTED L"23: The indices for the loop must all be numbers"
// -> #define ALL_SETS_EXPECTED L"24: All elements to 'dunion' or 'dinter' must be sets"
// -> #define ALL_SEQS_EXPECTED L"25: All elements to 'conc' must be sequences"
// -> #define ALL_MAPS_EXPECTED L"27: All elements to 'merge' must be maps"
// -> #define OLDID_NOT_IN_OS L"51: The old name is not stored in the state"
// -> #define MAP_MERGE_DOM_OVERLAP L"52: Duplicate entries for 'merge' had different values"
// -> #define EMPTY_ENV_S L"53: The binding environment was empty"
// -> //#define EMPTY_STATE_DESIGNATOR L"54: empty was state was designator"
// -> //#define NIL_STATE_COMPONENT L"55: nil was state was component"
// -> //#define NAME_NOT_IN_GS_OR_LS L"56: name was not was in was gs was or was ls"
// -> #define ZERO_BY_STEP L"57: Step length in loop was 0"
// -> #define EVAL_PRE_GIVES_FALSE L"58: The pre-condition evaluated to false"
// -> #define EVAL_POST_GIVES_FALSE L"59: The post-condition evaluated to false"
// -> //#define OP_EXPECTED L"60: op_expected"
// -> #define ERROR_STATEMENT L"61: Can not evaluate 'error' statement"
// -> #define UNDEFINED_EXPRESSION L"62: Can not evaluate 'undefined' expression"
// -> #define NO_OTHERS_EXPR L"63: No 'others' branch in 'cases' expr"
// -> #define WRONG_QUANTIFIER L"64: Wrong quantifier"
// -> #define ENV_S_TOO_BIG L"65: No loose patterns in sequence comprehension"
// -> #define ILLEGAL_INDICES L"66: Illegal index"
// -> #define DUPLICATES_NOT_EQUAL L"67: Duplicate entries had different values"
// -> #define RECORD_TAG_UNKNOWN L"68: Unknown record tag"
// -> #define RECORD_SIZE_WRONG L"69: Actual record size different from definition"
// -> #define RECORD_FIELD_ID_UNKNOWN L"70: Unknown record field selector"
// -> #define ARG_NOT_IN_DOM L"71: Argument not found in map domain"
// -> #define FCT_V_TYPE_EXPECTED L"72: An explicit function/operation was expected"
// -> #define UNDEF_ENCOUNTERED L"73: Identifier is undefined/not initialized"
// -> #define ID_UNKNOWN L"74: Unknown identifier"
// -> #define OPERATOR_UNKNOWN L"75: Unknown operator"
// -> #define DIVISION_WITH_ZERO L"76: Division with zero"
// -> #define ZERO_LENGTH_DETECTED L"77: The sequence was empty"
// -> #define PATTERN_UNKNOWN L"78: Unknown pattern"
// -> #define SET_TOO_BIG L"79: Set too big for 'power' - limit is 16"
// -> #define EXPR_UNKNOWN L"80: Expression unknown"
// -> #define WRONG_NO_OF_ARGS L"81: Wrong number of arguments"
// -> //#define OP_RETURNED_NIL L"82: Operation returned nil"
// -> #define STMT_UNKNOWN L"83: Unknown statement"
// -> #define REF_UNKNOWN L"84: Unknown reference in assign statement"
// -> #define TYPE_UNKNOWN L"85: Unknown type"
// -> #define IS_TYPE_UNKNOWN L"86: Unknown type in is_ expression"
// -> #define IDENTICAL_FIELDS L"89: identical selector names in composite type"
// -> //#define WRONG_NO_OF_RESULTS L"87: wrong no of results"
// -> //#define WRONG_NUMBER_OF_FIELDS_IN_STATE L"88: wrong number of fields in state"
// -> //#define MS_ALREADY_DEFINED              L"90: ms already defined"
// -> //#define MO_ALREADY_DEFINED              L"91: mo already defined"
// -> //#define MS_NOT_DEFINED                  L"92: ms not defined"
// -> //#define MO_NOT_DEFINED                  L"93: mo not defined"
// -> //#define DD_NOT_DEFINED                  L"94: dd not defined"
// -> //#define TAG_ALREADY_DEFINED             L"95: tag already defined"
// -> //#define IMPORT_NOT_IN_EXPORT            L"96: import not in export"
// -> //#define OBJECT_UNKNOWN                  L"97: object unknown"
// -> #define TYPE_INCOMP L"98: Incompatible types found in dynamic type check"
// -> #define STATE_INVARIANT_ERROR L"99: State invariant was broken"
// -> #define TAG_MULTIPLE_DEF L"100: Multiple definitions of tag"
// -> 
// -> 
// -> ///** Error numbers for specialized integration versions ******************/
// -> //
// -> //#define PARSE_ERROR L"101: PARSE ERROR"
// -> //#define ONLY_ONE_DD L"102: ONLY ONE DD"
// -> //#define NO_SUCH_FILE L"103: NO SUCH FILE"
// -> //#define MINISPEC_UNKNOWN L"104: MINISPEC_UNKNOWN"
// -> //#define NO_MODULES L"105: NO MODULES"
// -> //#define OPTION_UNKNOWN L"106: OPTION UNKNOWN"
// -> 
// -> 
// -> // Error numbers added for extending to full VDM-SL syntax
// -> 
// -> #define NO_UNIQ_ELEM L"110: No unique element in 'iota'"
// -> #define OP_EXIT_CONT L"111: Operation exited or returned no result"
// -> #define NO_POLY_FUNC L"112: Instantiated function is not polymorhic"
// -> #define NO_INJECTIVE_MAP L"113: Map is not injective - 'inverse' fails"
// -> #define NOT_RNG_DOM_SUBSET L"114: The range is not a subset of the domain"
// -> #define TWO_FN_OR_MAP_EXPECTED L"115: Two functions or maps expected for 'comp'"
// -> #define FN_OR_MAP_OR_NUM_EXPECTED L"116: Wrong arguments for '**'"
// -> #define NAT_EXPECTED L"117: A natural number was expected"
// -> //#define NOT_OP_APPLY L"118: not op apply"
// -> #define OP_RETURNED_CONT L"119: The operation did not return a value"
// -> #define STATE_DESIG_UNKNOWN L"120: Unknown state component"
// -> #define IMPL_OP_CALL L"121: Tried to call an implicit operation"
// -> #define ILLEGAL_STATE_INIT L"122: Illegal state initialisation"
// -> #define ILL_STATE_INIT_PAT L"123: Illegal state initialisation pattern"
// -> #define REC_PAT_EXPECTED L"124: Record pattern expected for state initialisation"
// -> #define ALL_PATID_EXPECTED L"125: Only pattern identifiers allowed for state initialisation"
// -> #define TYPE_BIND_EVAL L"126: Can not evaluate type binds"
// -> #define FNDEF_EXPECTED L"127: Function definition expected"
// -> #define IMPL_FN_APPLY L"128: Tried to apply an implicit function"
// -> #define POLY_NOT_INST L"129: The applied polymorhic function is not instantiated"
// -> #define MAP_AND_SET_EXPECTED L"130: A map and a set were expected"
// -> #define NOT_EXPL_OP_CALL L"131: The called object is not an explicit operation"
// -> #define GET_VALUE_EXPR L"132: No input value to get_value()"
// -> #define OP_CANT_RETURN_A_VALUE L"133: The operation's range is empty"
// -> // Error numbers added for module stuff
// -> 
// -> #define MOD_ALREADY_DEF L"150: The module is already defined"
// -> #define EQUAL_RENAMINGS L"151: Name clash for renaming"
// -> #define PARMOD_NOT_DEFINED L"152: Parameterised module is not defined"
// -> #define NOT_FULLY_INST L"153: Parameterised module is not fully instantiated"
// -> #define TYPE_ALREADY_DEF L"154: The type is already defined"
// -> #define MOD_NOT_DEFINED L"155: The module is not defined"
// -> #define TYPE_NOT_EXPORTED L"156: The type is not exported"
// -> #define CONSTRUCT_NOT_EXPORTED L"157: The construct is not exported"
// -> #define WRONG_STATE_TYPE L"158: Wrong state type"
// -> #define NOT_DEFINED_IN_MOD L"159: Incorrect use of 'using'"
// -> #define LIB_NOT_DEFINED L"160: couldn't open library: "
// -> #define LIB_SYMBOL_NOT_DEFINED L"161: token not defined in library"
// -> #define LIB_ARGUMENT_WRONG_NUMBER L"162: actual number of argument didn't match the number the extern function expected"
// -> #define LIB_NOT_DECLARED L"163: Not declared in dl module"
// -> #define INTERNAL_ERROR L"164: Internal error, please report"
// -> #define LIB_TYPE_ERROR L"165: Incompatible type in dl module call"
// -> #define TYPE_NOT_SUPPORTED L"166: Type is not yet supported"
// -> #define LIB_CLOSE_ERR L"167: Couldn't close dynamic library"
// -> #define TAG_NOT_IN_NAME_MAP L"168: Name not defined in NameMap in dynamic library file"
// -> #define LIB_WRONG_SIGN L"169: Library function has wrong signature"
// -> #define FILE_DOES_NOT_EXISTS L"170: Library Name not found in the search path"
// -> #define CAN_NOT_BE_EVALUATED L"171: Extern function cannot be evaluated"
// -> #define FN_EXIT_CONT L"172: Function exited or returned no result"
// -> #define LIB_VERSION_NOT_FOUND L"173: Symbol VDMLibVersion not found in dynamic lib"
// -> #define LIB_WRONG_VERSION L"174: Version of VDM C++ library"
// -> #define EXTENDED_FCT_EXPECTED L"175: Extended function or operation expected"
// -> #define UNEXPECTED_INFLOW L"176: Unexpected input flow"
// -> #define COUND_NOT_MATCH_OUTFLOW L" 177: Return value could not match output flows"
// -> 
// -> #define NONEMPTYSET_EXPECTED L"200: A nonempty set was expected"
// -> #define NUMBER_ARG_NEQ_NUMBER_TYPE L"201: Number of arguments does not match number type domain in function or operation"
// -> #define TYPE_INCOMP_RETURN L"202: Incompatible return type in function or operation application"
// -> #define TYPE_INCOMP_APPLY  L"203: Incompatible type in variables in function or operation application"
// -> #define SET_EXP_IN_PATTERN_BIND L"204: Set Expected in pattern bind in Define Expression"
// -> #define VALUE_NOT_IN_SETBIND L"205: Value in Def Expression is not in Set Bind"
// -> #define MTHD_EXIT_CONT L"206: Method exited or returned no result"
// -> #define OBJ_REF_EXP L"207: An object reference was expected in the expression"
// -> #define CLNM_NOT_DEFINED L"208: Class name is not defined"
// -> #define TAG_UNKNOWN L"209: Tag is unknown within the current scope"
// -> #define CLNM_NOT_IN_SCOPE L"210: Class name not is not within the current scope"
// -> #define MULT_DEF_METHS L"211: The method name is multiple defined within the current scope"
// -> #define MULT_DEF_FCTS L"212: The function name is multiple defined within the current scope"
// -> #define MULT_INST_VARS L"213: The instance variable name is multiple defined within the current scope"
// -> #define MULT_VAL_IDS L"214: The identifier is multiple defined as a value in the current scope"
// -> #define TOPOLOGY_STMT L"215: Cannot evaluate topology statement"
// -> #define SPEC_STMT L"216: Cannot evaluate specification statement"
// -> #define INST_ABS_CL L"217: Cannot instantiate an abstract class"
// -> #define NOT_CL_NAME_IN_NEW_STMT L"218: Unknown class name in new statement"
// -> #define OBJ_REF_EXP_CALL L"219: An object reference was expected in call statement"
// -> #define NOT_MTHD_NAME L"220: A name of a full method was expected in invoke statement"
// -> #define INST_INV_BROKEN L"221: Instance invariant was broken"
// -> #define IND_INH_NOT_SUPPORTED L"222: Indexed inheritance is not supported"
// -> #define UNEXP_RET_VAL_INIT_CL L"223: Initialisation statement returned a value"
// -> #define CIRC_CL_DEPENDENCY L"224: Circular inheritance dependecy detected"
// -> #define MULT_TPS_NAME L"225: Multiple defined types with the same name within current scope"
// -> #define DB_OBJ_EXISTS L"226: The object name already exists. Please destroy the object before creating a new object of the same name"
// -> #define DB_OBJ_NOT_EXISTS L"227: The name is not an object"
// -> #define OBJ_RECORD_EXP L"228: An object or record was expected"
// -> #define CIRC_CL_INIT L"229: Circular dependency in initialisation detected"
// -> #define NAME_UNKNOWN L"230: Name unknown"
// -> #define DC_NOT_PATTERN_NAME L"231: An abstract field of a record must only pattern match with a pattern name"
// -> #define LOCAL_COMPOSE_TYPEDEF L"232: The interpreter does not support local type definitions of records"
// -> #define NOTYETSPECFCT L"233: Cannot evaluate `not yet specified' functions"
// -> #define NOTYETSPECOP L"234: Cannot evaluate `not yet specified' operations"
// -> #define REC_OR_OBJ_EXPECTED L"235: A record or object was expected"
// -> #define BUG_263 L"236: You has hit bug number 263, please see the bug report for a work-around"
// -> #define EXIT_IN_INIT L"237: Exit value returned in initialisation of instance variable"
// -> #define SUBRESP L"238: Cannot evaluate `sub responsible' functions"
// -> #define INTERPRETER_STOPPED L"239: Interpreter stopped"

#endif // __rt_errmsg__
