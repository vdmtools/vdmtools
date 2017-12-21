/***
*  * WHAT
*  *    Interpreter. Error reporting functions.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/rt_errmsg.cc,v $
*  * VERSION
*  *    $Revision: 1.61 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:06 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS 
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include "rt_errmsg.h"
#include "libman.h"
#include "position.h"
#include "projectval.h"
#include "val2x.h"
#include "statsem.h"
#include "tb_wstring.h"
#include "tbutils.h"
#include "tools.h"
#include "settings.h"
#include "RTERR.h"
#include "UTIL.h"

RunTimeMsg RTERR::RTErrMsg;

void RTERR::Reset()
{
  RTERR::RTErrMsg.printed = true;
}

bool RTERR::RunTimeErrorOccured()
{
  return !RTERR::RTErrMsg.printed;
}

TYPE_SEM_ValTp RTERR::ErrorValTp(const wchar_t * Where, const int What,
                                 const Generic & semval, const Generic & type, const Sequence & txts)
{
  return TYPE_SEM_ValTp().Init(ErrorVal(Where,What,semval,type,txts), Nil());
}

TYPE_SEM_VAL RTERR::ErrorVal(const wchar_t * Where, const int What,
                             const Generic & semval, const Generic & type, const Sequence & txts)
{
  Error(Where, What, semval, type, txts);
  return theStackMachine().RuntimeErrorVal();
}

void RTERR::Error(const wchar_t * Where, const int What,
                  const Generic & semval, const Generic & type, const Sequence & txts)
{
  ReportError(Where, What, semval, type, theStackMachine().GetCurCid(), txts);
}

void RTERR::InitError(const wchar_t * Where, const int What,
                      const Generic & exp_v, const Generic & exp_tp, const TYPE_CI_ContextId & cid,
                      const Sequence & txts)
{
  ReportError(Where, What, exp_v, exp_tp, cid, txts);
}

void RTERR::ReportError(const wchar_t * Where, const int What_,
                        const Generic & exp_v, const Generic & exp_tp, const TYPE_CI_ContextId & cid,
                        const Sequence & txts)
{
  POSITION::SetPosition (cid);

// 20091125 -->
//  wstring What (GetWhatStr(What_));
  wstring What;
  wstring lang (TBUTILS:: getLanguage());
  if (TBUTILS::isJapanese() && Settings.UseTranslation())
    What = UTIL::ReplacePercent(GetWhatStrJP(What_), txts).GetString();
  else
    What = UTIL::ReplacePercent(GetWhatStr(What_), txts).GetString();
// <-- 20091125
  
  // output to the file rterr.msg
  if (cliOpt->IsInternalTest()) {
    OutputToRterr(What);
  }

  RTErrMsg.Where = Where;

//  wcout << RTErrMsg.Where << endl << flush;

  // Update message
  RTErrMsg.What = std::wstring(L"Run-Time Error ") + std::wstring(What);

  // Update actual value
  RTErrMsg.exp_v = exp_v;

  // Update actual type
  RTErrMsg.exp_tp = exp_tp;

  // Make this an error message that hasn't yet been printed
  RTErrMsg.printed = false;

  theStackMachine().ThrowRuntimeErrorException();
}

void RTERR::ReportWarning(const wchar_t * Where, const int What_, const TYPE_CI_ContextId & cid, const Sequence & txts)
{
  POSITION::SetPosition (cid);
  wstring What;
  wstring lang (TBUTILS:: getLanguage());
  if (TBUTILS::isJapanese() && Settings.UseTranslation())
    What = UTIL::ReplacePercent(GetWhatStrJP(What_), txts).GetString();
  else
    What = UTIL::ReplacePercent(GetWhatStr(What_), txts).GetString();

  Sequence seq;
  seq.ImpAppend(SEQ<Char>(std::wstring(L"Run-Time Warning ") + std::wstring(What)));

  TYPE_ProjectTypes_WarnMsg w;
  w.Init(POSITION::GetErrFile (), POSITION::GetErrLine (), POSITION::GetErrCol (), seq);

  ToolMediator::Errs()->vdm_AddMsg(w);
}

void RTERR::OutputToRterr(const std::wstring & err)
{
  wofstream out ("rterr2.msg");
  out << L"\"Line " << POSITION::GetErrLine();
  out << L" column " << POSITION::GetErrCol() << L"\n";
  out << err << L"\"";
  out.close();
}

void RTERR::PrintError(wostream & wos)
{
//  wos << RTErrMsg.Where << endl;

  TYPE_ProjectTypes_ErrMsg r (GetErrMsg());

  std::wstring file (ToolMediator::GetFileName(POSITION::GetErrFile()));

  std::wstring smsg (PTAUX::ExtractMessageAll(file, r));

  wos << smsg << flush;
}

std::wstring RTERR::GetWhat()
{
  return RTErrMsg.What;
}

std::wstring RTERR::GetWhere()
{
  return RTErrMsg.What;
}

void RTERR::PrintRunTimeError(wostream & wos)
{
  // Only print this error message if we have not already done so.
  // This is necessary as due to the exception handling, this function
  // could be called several times for the same error message.

  if (!RunTimeErrorOccured())
    return;

// 20090128
//  PrintError(wos);
//  wos << flush;

  Reset();
}

TYPE_ProjectTypes_ErrMsg RTERR::GetErrMsg()
{
  Sequence seq;
  seq.ImpAppend(SEQ<Char>(RTErrMsg.What));

  wstring valstr;
  if (RTErrMsg.exp_v.IsRecord()) {
    if (RTErrMsg.exp_tp.IsRecord())
      valstr = L"  actual value: ";
    else
      valstr = L"  value: ";
    valstr += VAL2X::val2asc(RTErrMsg.exp_v);
    seq.ImpAppend(SEQ<Char>(valstr));
  }

#ifndef NOSS
  wstring typstr;
  if (RTErrMsg.exp_tp.IsRecord()) {
    if (RTErrMsg.exp_v.IsRecord())
      typstr = L"  expected type: ";
    else
      typstr = L"  type: ";
    typstr += GetStatSem().ASType2Ascii(RTErrMsg.exp_tp);
    seq.ImpAppend(SEQ<Char>(typstr));
  }
#endif // NOSS

  TYPE_ProjectTypes_ErrMsg r;
  r.Init(POSITION::GetErrFile (), POSITION::GetErrLine (), POSITION::GetErrCol (), seq);
  return r;
}

// GetWhatStr
SEQ<Char> RTERR::GetWhatStr(int num)
{
  return Sequence(Int(num).ascii() + L": " + GetErrorMessage(num));
}

// GetWhatStrJP
SEQ<Char> RTERR::GetWhatStrJP(int num)
{
  return Sequence(Int(num).ascii() + L": " + TBWSTR::string2wstring(GetErrorMessageJP(num)));
}

int RTERR::GetErrorNum(const Sequence & str)
{
//  return (Int(num).ascii() + L": " + GetErrorMessage(num));
  wstring s (str.GetString());
  int num = 0;
  int len = s.length();
  for (int i = 0; i < len; i++)
  {
    wchar_t c = s[i];
    switch (c) {
      case L'0':
      case L'1':
      case L'2':
      case L'3':
      case L'4':
      case L'5':
      case L'6':
      case L'7':
      case L'8':
      case L'9': {
        num = num * 10;
        num += (c - L'0');
        break;
      }
      case L':':
      default: {
        return num;
        break;
      }
    }
  }
  return 0;
}

wstring RTERR::GetErrorMessage(int num)
{
  switch (num) {
    case RTERR_BOOL_EXPECTED:        return wstring(L"A boolean was expected");
    case RTERR_INT_EXPECTED:         return wstring(L"An integer was expected");
    case RTERR_NUM_EXPECTED:         return wstring(L"A number was expected");
    case RTERR_SET_EXPECTED:         return wstring(L"A set was expected");
    case RTERR_SEQ_EXPECTED:         return wstring(L"A sequence was expected");
    case RTERR_REC_EXPECTED:         return wstring(L"A record was expected");
    case RTERR_MAP_EXPECTED:         return wstring(L"A map was expected");
    case RTERR_TUPLE_EXPECTED:       return wstring(L"A tuple was expected");
    case RTERR_SYMB_LIT_EXPECTED:    return wstring(L"A symbolic literal was expected");
    case RTERR_TWO_BOOL_EXPECTED:    return wstring(L"Two booleans were expected");
    case RTERR_TWO_INT_EXPECTED:     return wstring(L"Two integers were expected");
    case RTERR_TWO_NUM_EXPECTED:     return wstring(L"Two numbers were expected");
    case RTERR_TWO_SET_EXPECTED:     return wstring(L"Two sets were expected");
    case RTERR_TWO_SEQ_EXPECTED:     return wstring(L"Two sequences were expected");
    case RTERR_MAP_OR_SEQ_EXPECTED:  return wstring(L"A map or a sequence was expected");
    case RTERR_TWO_MAP_EXPECTED:     return wstring(L"Two maps were expected");
    case RTERR_SET_AND_MAP_EXPECTED: return wstring(L"A set and a map were expected");
    case RTERR_SEQ_AND_INT_EXPECTED: return wstring(L"A sequence and two integers were expected");
    case RTERR_PATTERNNAME_EXPECTED: return wstring(L"The binding pattern must evaluate to one patternname");
    case RTERR_CHAR_EXPECTED:        return wstring(L"A char was expected");
    case RTERR_ALL_INTS_EXPECTED:    return wstring(L"The binding set can only contain integers");
    case RTERR_ALL_NUMS_EXPECTED:    return wstring(L"The indices for the loop must all be numbers");
    case RTERR_ALL_SETS_EXPECTED:    return wstring(L"All elements to 'dunion' or 'dinter' must be sets");
    case RTERR_ALL_SEQS_EXPECTED:    return wstring(L"All elements to 'conc' must be sequences");
    case RTERR_ALL_MAPS_EXPECTED:    return wstring(L"All elements to 'merge' must be maps");
    case RTERR_OLDID_NOT_IN_OS:      return wstring(L"The old name is not stored in the state");
    case RTERR_MAP_MERGE_DOM_OVERLAP: return wstring(L"Duplicate entries for 'merge' had different values");
    case RTERR_EMPTY_ENV_S:          return wstring(L"The binding environment was empty");
    case RTERR_ZERO_BY_STEP:         return wstring(L"Step length in loop was 0");
    case RTERR_EVAL_PRE_GIVES_FALSE: return wstring(L"The pre-condition evaluated to false");
    case RTERR_EVAL_POST_GIVES_FALSE: return wstring(L"The post-condition evaluated to false");
    case RTERR_ERROR_STATEMENT:      return wstring(L"Can not evaluate 'error' statement");
    case RTERR_UNDEFINED_EXPRESSION: return wstring(L"Can not evaluate 'undefined' expression");
    case RTERR_NO_OTHERS_EXPR:       return wstring(L"No 'others' branch in 'cases' expr");
    case RTERR_WRONG_QUANTIFIER:     return wstring(L"Wrong quantifier");
    case RTERR_PAT_NAME_IN_SEQCOMP:  return wstring(L"Only pattern name is allowed in sequence comprehension");
    case RTERR_ILLEGAL_INDICES:      return wstring(L"Illegal index");
    case RTERR_DUPLICATES_NOT_EQUAL: return wstring(L"Duplicate entries had different values");
//    case RTERR_RECORD_TAG_UNKNOWN:   return wstring(L"Unknown record tag");
    case RTERR_RECORD_SIZE_WRONG:    return wstring(L"Actual record size different from definition");
    case RTERR_RECORD_FIELD_ID_UNKNOWN: return wstring(L"Unknown record field selector");
    case RTERR_ARG_NOT_IN_DOM:       return wstring(L"Argument not found in map domain");
    case RTERR_FCT_V_TYPE_EXPECTED:  return wstring(L"An explicit function/operation was expected");
    case RTERR_UNDEF_ENCOUNTERED:    return wstring(L"Identifier is undefined/not initialized");
    case RTERR_ID_UNKNOWN:           return wstring(L"Unknown identifier");
    case RTERR_OPERATOR_UNKNOWN:     return wstring(L"Unknown operator");
    case RTERR_DIVISION_WITH_ZERO:   return wstring(L"Division with zero");
    case RTERR_ZERO_LENGTH_DETECTED: return wstring(L"The sequence was empty");
    case RTERR_PATTERN_UNKNOWN:      return wstring(L"Unknown pattern");
    case RTERR_SET_TOO_BIG:          return wstring(L"Set too big for 'power' - limit is 25");
    case RTERR_EXPR_UNKNOWN:         return wstring(L"Expression unknown");
    case RTERR_WRONG_NO_OF_ARGS:     return wstring(L"Wrong number of arguments");
    case RTERR_STMT_UNKNOWN:         return wstring(L"Unknown statement");
    case RTERR_REF_UNKNOWN:          return wstring(L"Unknown reference in assign statement");
    case RTERR_TYPE_UNKNOWN:         return wstring(L"Unknown type");
    case RTERR_IS_TYPE_UNKNOWN:      return wstring(L"Unknown type in is_ expression");
    case RTERR_IDENTICAL_FIELDS:     return wstring(L"identical selector names in composite type");
    case RTERR_TYPE_INCOMP:          return wstring(L"Incompatible types found in dynamic type check");
    case RTERR_STATE_INVARIANT_ERROR: return wstring(L"State invariant was broken");
    case RTERR_TAG_MULTIPLE_DEF:     return wstring(L"Multiple definitions of tag");
    case RTERR_NO_UNIQ_ELEM:         return wstring(L"No unique element in 'iota'");
    case RTERR_OP_EXIT_CONT:         return wstring(L"Operation exited or returned no result");
    case RTERR_NO_POLY_FUNC:         return wstring(L"Instantiated function is not polymorphic");
    case RTERR_NO_INJECTIVE_MAP:     return wstring(L"Map is not injective - 'inverse' fails");
    case RTERR_NOT_RNG_DOM_SUBSET:   return wstring(L"The range is not a subset of the domain");
    case RTERR_TWO_FN_OR_MAP_EXPECTED: return wstring(L"Two functions or maps expected for 'comp'");
    case RTERR_FN_OR_MAP_OR_NUM_EXPECTED: return wstring(L"Wrong arguments for '**'");
    case RTERR_NAT_EXPECTED:         return wstring(L"A natural number was expected");
    case RTERR_OP_RETURNED_CONT:     return wstring(L"The operation did not return a value");
    case RTERR_STATE_DESIG_UNKNOWN:  return wstring(L"Unknown state component");
    case RTERR_IMPL_OP_CALL:         return wstring(L"Tried to call an implicit operation");
    case RTERR_ILLEGAL_STATE_INIT:   return wstring(L"Illegal state initialisation");
    case RTERR_ILL_STATE_INIT_PAT:   return wstring(L"Illegal state initialisation pattern");
    case RTERR_REC_PAT_EXPECTED:     return wstring(L"Record pattern expected for state initialisation");
    case RTERR_ALL_PATID_EXPECTED:   return wstring(L"Only pattern identifiers allowed for state initialisation");
    case RTERR_TYPE_BIND_EVAL:       return wstring(L"Can not evaluate type binds");
    case RTERR_FNDEF_EXPECTED:       return wstring(L"Function definition expected");
    case RTERR_IMPL_FN_APPLY:        return wstring(L"Tried to apply an implicit function");
    case RTERR_POLY_NOT_INST:        return wstring(L"The applied polymorphic function is not instantiated");
    case RTERR_MAP_AND_SET_EXPECTED: return wstring(L"A map and a set were expected");
    case RTERR_NOT_EXPL_OP_CALL:     return wstring(L"The called object is not an explicit operation");
    case RTERR_GET_VALUE_EXPR:       return wstring(L"No input value to get_value()");
    case RTERR_OP_CANT_RETURN_A_VALUE: return wstring(L"The operation's range is empty");
    case RTERR_MOD_ALREADY_DEF:      return wstring(L"The module is already defined");
    case RTERR_EQUAL_RENAMINGS:      return wstring(L"Name clash for renaming");
    case RTERR_PARMOD_NOT_DEFINED:   return wstring(L"Parameterised module is not defined");
    case RTERR_NOT_FULLY_INST:       return wstring(L"Parameterised module is not fully instantiated");
    case RTERR_TYPE_ALREADY_DEF:     return wstring(L"The type is already defined");
    case RTERR_MOD_NOT_DEFINED:      return wstring(L"The module is not defined");
    case RTERR_TYPE_NOT_EXPORTED:    return wstring(L"The type is not exported");
    case RTERR_CONSTRUCT_NOT_EXPORTED: return wstring(L"The construct is not exported");
    case RTERR_WRONG_STATE_TYPE:     return wstring(L"Wrong state type");
    case RTERR_NOT_DEFINED_IN_MOD:   return wstring(L"Incorrect use of 'using'");
    case RTERR_LIB_NOT_DEFINED:      return wstring(L"couldn't open library: L");
    case RTERR_LIB_SYMBOL_NOT_DEFINED: return wstring(L"token not defined in library");
    case RTERR_LIB_ARGUMENT_WRONG_NUMBER: return wstring(L"actual number of argument didn't match the number the extern function expected");
    case RTERR_LIB_NOT_DECLARED:     return wstring(L"Not declared in dl module");
    case RTERR_INTERNAL_ERROR:       return wstring(L"Internal error, please report");
    case RTERR_LIB_TYPE_ERROR:       return wstring(L"Incompatible type in dl module call");
    case RTERR_TYPE_NOT_SUPPORTED:   return wstring(L"Type is not yet supported");
    case RTERR_LIB_CLOSE_ERR:        return wstring(L"Couldn't close dynamic library");
    case RTERR_TAG_NOT_IN_NAME_MAP:  return wstring(L"Name not defined in NameMap in dynamic library file");
    case RTERR_LIB_WRONG_SIGN:       return wstring(L"Library function has wrong signature");
    case RTERR_FILE_DOES_NOT_EXISTS: return wstring(L"Library Name not found in the search path");
    case RTERR_CAN_NOT_BE_EVALUATED: return wstring(L"Extern function cannot be evaluated");
    case RTERR_FN_EXIT_CONT:         return wstring(L"Function exited or returned no result");
    case RTERR_LIB_VERSION_NOT_FOUND: return wstring(L"Symbol VDMLibVersion not found in dynamic lib");
    case RTERR_LIB_WRONG_VERSION:    return wstring(L"Version of VDM C++ library");
    case RTERR_EXTENDED_FCT_EXPECTED: return wstring(L"Extended function or operation expected");
    case RTERR_UNEXPECTED_INFLOW:    return wstring(L"Unexpected input flow");
    case RTERR_COUND_NOT_MATCH_OUTFLOW: return wstring(L"Return value could not match output flows");
    case RTERR_NONEMPTYSET_EXPECTED: return wstring(L"A nonempty set was expected");
    case RTERR_NUMBER_ARG_NEQ_NUMBER_TYPE: return wstring(L"Number of arguments does not match number type domain in function or operation");
    case RTERR_TYPE_INCOMP_RETURN:   return wstring(L"Incompatible return type in function or operation application");
    case RTERR_TYPE_INCOMP_APPLY:    return wstring(L"Incompatible type in variables in function or operation application");
    case RTERR_SET_EXP_IN_PATTERN_BIND: return wstring(L"Set expected in pattern bind");
    case RTERR_VALUE_NOT_IN_SETBIND: return wstring(L"Value is not in Set Bind");
    case RTERR_MTHD_EXIT_CONT:       return wstring(L"Method exited or returned no result");
    case RTERR_OBJ_REF_EXP:          return wstring(L"An object reference was expected in the expression");
    case RTERR_CLNM_NOT_DEFINED:     return wstring(L"Class name is not defined: %1");
    case RTERR_TAG_UNKNOWN:          return wstring(L"Tag is unknown within the current scope");
    case RTERR_MULT_DEF_METHS:       return wstring(L"The method name is multiple defined within the current scope");
    case RTERR_MULT_DEF_FCTS:        return wstring(L"The function name is multiple defined within the current scope");
    case RTERR_MULT_INST_VARS:       return wstring(L"The instance variable name is multiple defined within the current scope");
    case RTERR_MULT_VAL_IDS:         return wstring(L"The identifier is multiple defined as a value in the current scope");
    case RTERR_TOPOLOGY_STMT:        return wstring(L"Cannot evaluate topology statement");
    case RTERR_SPEC_STMT:            return wstring(L"Cannot evaluate specification statement");
    case RTERR_INST_ABS_CL:          return wstring(L"Cannot instantiate an abstract class: %1");
    case RTERR_NOT_CL_NAME_IN_NEW_STMT: return wstring(L"Unknown class name in new expression");
    case RTERR_OBJ_REF_EXP_CALL:     return wstring(L"An object reference was expected in call statement");
    case RTERR_NOT_MTHD_NAME:        return wstring(L"A name of a full method was expected in invoke statement");
    case RTERR_INST_INV_BROKEN:      return wstring(L"Instance invariant was broken");
    case RTERR_IND_INH_NOT_SUPPORTED: return wstring(L"Indexed inheritance is not supported");
    case RTERR_UNEXP_RET_VAL_INIT_CL: return wstring(L"Initialisation statement returned a value");
    case RTERR_CIRC_CL_DEPENDENCY:   return wstring(L"Circular inheritance dependency detected");
    case RTERR_MULT_TPS_NAME:        return wstring(L"Multiple defined types with the same name within current scope");
    case RTERR_DB_OBJ_EXISTS:        return wstring(L"The object name already exists. Please destroy the object before creating a new object of the same name");
    case RTERR_DB_OBJ_NOT_EXISTS:    return wstring(L"The name is not an object");
    case RTERR_OBJ_RECORD_EXP:       return wstring(L"An object or record was expected");
    case RTERR_CIRC_CL_INIT:         return wstring(L"Circular dependency in initialisation detected");
    case RTERR_NAME_UNKNOWN:         return wstring(L"Name unknown");
    case RTERR_DC_NOT_PATTERN_NAME:  return wstring(L"An abstract field of a record must only pattern match with a pattern name");
    case RTERR_LOCAL_COMPOSE_TYPEDEF: return wstring(L"The interpreter does not support local type definitions of records");
    case RTERR_NOTYETSPECFCT:        return wstring(L"Cannot evaluate `not yet specified' functions");
    case RTERR_NOTYETSPECOP:         return wstring(L"Cannot evaluate `not yet specified' operations");
    case RTERR_REC_OR_OBJ_EXPECTED:  return wstring(L"A record or object was expected");
    case RTERR_BUG_236:              return wstring(L"You have hit bug number 236, please see the bug report for a work-around");
    case RTERR_EXIT_IN_INIT:         return wstring(L"Exit value returned in initialisation of instance variable");
    case RTERR_SUBRESP:              return wstring(L"Cannot evaluate `sub responsible' functions");
    case RTERR_NUMERIC_SET:          return wstring(L"Quantification in sequence comprehension must be over numeric values");
    case RTERR_WRONG_NO_RES:         return wstring(L"Wrong number of results");
    case RTERR_LOWER_BOUND_NOT_A_NUMBER: return wstring(L"Lower bound is not a number");
    case RTERR_UPPER_BOUND_NOT_A_NUMBER: return wstring(L"Upper bound is not a number");
    case RTERR_STEP_NOT_A_NUMBER:    return wstring(L"Step is not a number");
    case RTERR_UPPER_BOUND_LARGER_THAN_LOWER_BOUND: return wstring(L"Lower bound larger than upper bound");
    case RTERR_LOWER_BOUND_LARGER_THAN_UPPER_BOUND: return wstring(L"Lower bound larger than upper bound");
    case RTERR_STEP_INDEX_IS_ZERO:   return wstring(L"Step index is zero");
    case RTERR_LOOP_EXPR_NOT_AN_EXPR: return wstring(L"Expression in while-statement does not evaluate to an expression");
    case RTERR_TEST_EXPR_NOT_AN_EXPR: return wstring(L"Test expression in if-statement does not evaluate to an expression");
    case RTERR_TUPLE_OUTSIDE_INDEX:  return wstring(L"Tuple selection outside its index");
    case RTERR_INSTVAR_NOT_PUBLIC:   return wstring(L"Instance variable must be public");
    case RTERR_INSTVAR_NOT_IN_SCOPE: return wstring(L"Instance variable is not in scope");
    case RTERR_FUN_NOT_IN_SCOPE:     return wstring(L"Function is not in scope");
    case RTERR_OP_NOT_IN_SCOPE:      return wstring(L"Operation is not in scope");
    case RTERR_VAL_NOT_IN_SCOPE:     return wstring(L"Value is not in scope");
    case RTERR_POLYFUN_NOT_IN_SCOPE: return wstring(L"Polymorphic function is not in scope");
    case RTERR_TYPE_NOT_IN_SCOPE:    return wstring(L"Type is not in scope");
    case RTERR_NOT_IN_SCOPE:         return wstring(L"Construct not in scope");
    case RTERR_MULT_DEF:             return wstring(L"Construct is multiple defined within the current scope");
    case RTERR_INIT_NOT_POSSIBLE:    return wstring(L"Initialisation is not possible");
    case RTERR_INDEXED_ASSIGN:       return wstring(L"Indexed assignment can only be done when initialized");
    case RTERR_OP_IN_GUARD:          return wstring(L"You cannot use an operation inside a permission guard");
    case RTERR_NO_THREAD:            return wstring(L"This class has no thread to start");
    case RTERR_DEADLOCK_DETECTED:    return wstring(L"Deadlock is detected");
    case RTERR_PRE_COND_APPLY_EXPR:  return wstring(L"The special pre-condition application expression is not supported in the interpreter");
    case RTERR_NO_GUARD_IN_INIT:     return wstring(L"An operation with a permission guard may not  be used in the initialisation of an instance variable");
    case RTERR_OP_OR_FUN_NOT_IN_SCOPE: return wstring(L"Operation or function is not in scope");
    case RTERR_OP_DEF_IN_MULTSUPERS: return wstring(L"Operation defined in multiple super classes, so it is not allowed to add permission predicate for class");
    case RTERR_SEL_NONVALID_THREADID:  return wstring(L"A non-valid thread id was used");
    case RTERR_MULT_THREAD_INH:      return wstring(L"More than one thread inherited");
    case RTERR_DLCLASS_NEW_ERROR:    return wstring(L"External dlclass instantiation failed");
    case RTERR_DLCLASS_LOAD_ERROR:   return wstring(L"dlclass loading failed");
    case RTERR_DLCLASS_APPLY_AFTER_DLCLOSE: return wstring(L"apply of dlclass method after dlclose");
    case RTERR_DLCLASS_DELETE_FAILED: return wstring(L"external delete of dlclass failed");
    case RTERR_CLASS_NOT_DLCLASS:    return wstring(L"Class is not a dlclass: ");
#ifdef VDMSL
    case RTERR_NO_DL_LICENSE:        return wstring(L"Initialisation stopped. Failed to check out vdmdl license.");
#endif // VDMSL
#ifdef VDMPP
    case RTERR_NO_DL_LICENSE:        return wstring(L"Initialisation stopped. Failed to check out vppdl license.");
#endif // VDMPP
    case RTERR_DL_NOT_ENABLED:       return wstring(L"dlmodules/dlclasses are not supported in this Toolbox.");
    case RTERR_POLY_CALL:            return wstring(L"Attempt to execute a polymorphic function without instantiating its type variables.");
    case RTERR_APPLY_NONAPPVAL:      return wstring(L"Apply expression contains left hand side which is not a map, sequence or function");
    case RTERR_LOGDLCALL:            return wstring(L"Log of dlcall: " );
    case RTERR_NOCONSTRUCTOR:        return wstring(L"No constructor with this parameter list is in scope");
    case RTERR_MULTIOVERLOADED:      return wstring(L"Unable to resolve overloaded operation call");
    case RTERR_STATIC_NOT_IN_SCOPE:  return wstring(L"Static member is not in scope");
    case RTERR_STATIC_IV_NO_VALUE:   return wstring(L"Static instance variable must be initialized");
    case RTERR_CANNOT_PROCEED_AFTER_RUNTIME_ERROR: return wstring(L"Cannot proceed after a runtime error." );
    case RTERR_NOOVERLOADED:         return wstring(L"No overloaded operation or function with this parameter list is in scope");
    case RTERR_NOOBJECT:             return wstring(L"No object is present");
    case RTERR_CPUIMPLCLASS:         return wstring(L"When a system specification is made CPU is an implicit class");
    case RTERR_BUSIMPLCLASS:         return wstring(L"When a system specification is made BUS is an implicit class");
    case RTERR_DEPLOYINST:           return wstring(L"Only instances can be deployed to a CPU");
    case RTERR_NOBUS:                return wstring(L"No BUS is connecting these two CPUs");
    case RTERR_ONLYDEPLOYORPRIO:     return wstring(L"Only deploy and priority operations can be used in system constructor");
    case RTERR_UNSUPPRIO:            return wstring(L"Unsupported CPU priority scheme");
    case RTERR_UNSUPCPUCAP:          return wstring(L"Unsupported CPU capacity (should be a nat)");
    case RTERR_WRONGCPUCONS:         return wstring(L"Wrong parameters to CPU constructor");
    case RTERR_UNSUPPKIND:           return wstring(L"Unsupported BUS kind");
    case RTERR_UNSUPBUSCAP:          return wstring(L"Unsupported BUS capacity (should be a nat)");
    case RTERR_NOSETOFCPUS:          return wstring(L"This should be a set of CPU names");
    case RTERR_WRONGBUSCONS:         return wstring(L"Wrong parameters to BUS constructor");
    case RTERR_TYPE_INCOMP_IN_FNOP_APPLY: return wstring(L"Incompatible types found in call of function/operation with dynamic type check");
    case RTERR_TYPE_INV_BROKEN:      return wstring(L"Type invariant was broken");
    case RTERR_EVAL_ASSERT_GIVES_FALSE: return wstring(L"The assertion evaluated to false");
    case RTERR_TAG_CYCLIC:           return wstring(L"Circular defined tag detected");
    case RTERR_MULTIPLE_PATTERN:     return wstring(L"Pattern may match multiply");
    case RTERR_DLMODULE_LOAD_ERROR:   return wstring(L"Dlmodule loading failed");
    case RTERR_MEASURE_ERROR:         return wstring(L"Current measure value isn't smaller than previous value");
    case RTERR_NO_RUNNING_THREAD:    return wstring(L"Thread is't started or is already dead");
    case RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED: return wstring(L"A natural number or a tuple of natural numbers was expected");
    case RTERR_PURE_OP_EXIT:         return wstring(L"Pure operation exited");
    case RTERR_PURE_OP_CALL:         return wstring(L"Pure operation call impure operation");


    case RTERR_UNKNOWN_AS_TYPE:      return wstring(L"Internal Error: unknown AS type");
    case RTERR_UNKNOWN_OPERATOR:     return wstring(L"Internal Error: Unknown operator");
    case RTERR_ID_NOT_IN_LIST:       return wstring(L"Internal Error: id not in list");
    case RTERR_UNKNOWN_RECORD_TYPE:  return wstring(L"Unknown record type");
    case RTERR_CIRCULAR_STATIC_DEPENDENCY: return wstring(L"Circular static dependency detected");
    case RTERR_UNKNOWN_OBJECT:       return wstring(L"Internal Error: Unknown object");
    case RTERR_CIRCULAR_TYPE_INV_DEPENDENCY: return wstring(L"Circular type invariant dependency in dynamic type check");
    case RTERR_PERIODIC_THREAD_UNSUPPORTED: return wstring(L"Periodic Thread is't supported.");
    case RTERR_INCOMPATIBLE_BIND_TYPE: return wstring(L"Incompatible type found in pattern binding");
    case RTERR_DLCLASS_CALL_FAILED:  return wstring(L"dlclass call failed");
    case RTERR_INVALID_INSTRUCTION:  return wstring(L"Internal Error: Invalid Instruction");
    case RTERR_UNEXPECTED_PREFIX_OP: return wstring(L"Internal Error: unexpected prefix op");
    case RTERR_UNEXPECTED_BINARY_OP: return wstring(L"Internal Error: unexpected binary op");
    case RTERR_UNEXPECTED_EXPRESSION: return wstring(L"Internal Error: unexpected expression");
    case RTERR_UNEXPECTED_HISTORY_EVENT: return wstring(L"Internal Error: unexpected history event");
    case RTERR_UNEXPECTED_TRACE_EVENT: return wstring(L"Internal Error: unexpected trace event");
    case RTERR_UNABLE_OPEN_LOGFILE:  return wstring(L"Unable to open logfile for writing");
    case RTERR_NO_HISTORY_MAP:       return wstring(L"Internal Error: No HistMap");
    case RTERR_UNKNOWN_VALCOND:      return wstring(L"Internal Error: Unknown ValCond");
    case RTERR_UNKNOWN_VALCOND_KIND: return wstring(L"Internal Error: Unknown ValCond kind");
    case RTERR_ILLEGAL_VALCOND_INDEX: return wstring(L"Internal Error: Illegal index");
    case RTERR_UNKNOWN_VALCOND_RECORD: return wstring(L"Internal Error: Unknown ValCond Record");
    case RTERR_OP_RETURNED_VALUE:    return wstring(L"The operation return a value");
    case RTERR_OP_NAME_EXPECTED:     return wstring(L"A operation name was expected");
    case RTERR_SPORADIC_THREAD_UNSUPPORTED: return wstring(L"Sporadic Thread is't supported.");
    default: {
      return wstring(L"");
    }
  }
}
