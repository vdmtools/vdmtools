/***
*  * WHAT
*  *    Functions for parsing expressions, patterns and types. 
*  *    The three functions are passed the wstring to be parsed
*  *    and returns false if the parse was OK, otherwise true is
*  *    returned. The meta-iv value created during the parse is
*  *    contained in the seccond argument of the function.
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/parse_string.cc,v $
*  * VERSION
*  *    $Revision: 1.13 $
*  * DATE
*  *    $Date: 2005/07/04 05:30:00 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "parse_string.h"
#include "pexpr.h"           // For pars_vdm_expr()
#include "Errors.h"
#include "intconvquotes.h"
#include "AS.h"
#include "MANGLE.h"
#include "tools.h"
#include "tb_wstring.h"
#include "tb_exceptions.h"
#include "astaux.h"
#include "asquotes.h"

// A class to be passed to pars_vdm_expr for error handling.
// Currently, we simply ignore errors reported by the parser.

// Parses the wstring ws 
bool UMLPARSE::parse_expr(const wstring & ws, Record & res)
{
  string s (TBWSTR::ConvertToHexquad(ws));

  Sequence asts;
  bool err = true;
  try {
    ToolMediator::ExprErrs()->vdm_ClearAll();

    ContextInfo ci;
    err = VDMEXPRPARSER::pars_vdm_expr_string(s, false, ToolMediator::ExprErrs(),
                                              asts, ci, false, TYPE_CI_FileId(CI_FID_NO_CI));

    ToolMediator::ExprErrs()->vdm_AllDone();

    if(!err) {
      res = Record(INT2Q::h2gAS(asts.Hd()));
    }
  }
  catch (TB_Exception & e)  {
    switch (e.GetExType()) {
    default:
      // It must not happen
      wcout << L"Internal Error" << endl;
      wcout << L"parse_expr: " << ws << endl;
      TYPE_AS_ValueDef vd;
      vd.Init(TYPE_AS_PatternName().Init(ASTAUX::MkName(L"pat"), Nil(), NilContextId),
              Nil(),
              TYPE_AS_UndefinedExpr().Init(NilContextId),
              Int(NOT_INITIALISED_AS),
              Bool(true),
              NilContextId);
      res = TYPE_AS_LetExpr().Init(SEQ<TYPE_AS_LocalDef>().ImpAppend(vd),
                                   TYPE_AS_UndefinedExpr().Init(NilContextId),
                                   NilContextId);
      break;
    }
  }

  return err;
}


bool UMLPARSE::str2type(const wstring & s, TYPE_AS_Type & type)
{
  wstring expr (L"let pat : " + s + L" = undefined in undefined");

  TYPE_AS_LetExpr pars_res;
  bool err = parse_expr(expr, pars_res);

  if(!err) {
    // pars_res represents the parsed let expression
    // Field 1 is seq of LocalDef
    TYPE_AS_LocalDef val_def (pars_res.GetSequence(pos_AS_LetExpr_localdef).Hd());
    switch (val_def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        // val_def is actually of type AS`ValueDef
        type = val_def.GetField(pos_AS_ValueDef_tp);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ImplFnDef: {
        // ImplFnDef and ExplFnDef are ignored.
        return true;
      }
    }
  }
  return !err;
}


bool UMLPARSE::str2pat(const wstring & s, TYPE_AS_Pattern & pat)
{
  wstring unmangledS;
  if (MANGLE::IsMangled(s))
    unmangledS = MANGLE::unmangleStem(s);
  else
    unmangledS = s;

//  wstring expr (L"let " + unmangledS + L" = undefined in undefined");
  wstring expr (L"let " + unmangledS + L" = undefined in undefined");

  TYPE_AS_LetExpr pars_res;
  bool err = parse_expr(expr, pars_res);

  if(!err) {
    // pars_res represents the parsed let expression
    TYPE_AS_LocalDef val_def (pars_res.GetSequence(pos_AS_LetExpr_localdef).Hd());
    switch (val_def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        // val_def is actually of type AS`ValueDef
        pat = val_def.GetField(pos_AS_ValueDef_pat);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ImplFnDef: {
        // ImplFnDef and ExplFnDef are ignored.
        return true;
      }
    }
  }
  return !err;
}

bool UMLPARSE::str2expr(const wstring & s, TYPE_AS_Expr & exp)
{
  wstring expr (L"let pat = " + s + L" in undefined");

  TYPE_AS_LetExpr pars_res;
  bool err = parse_expr(expr, pars_res);

  if(!err) {
    // pars_res represents the parsed let expression
    TYPE_AS_LocalDef val_def (pars_res.GetSequence(pos_AS_LetExpr_localdef).Hd());
    switch (val_def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        // val_def is actually of type AS`ValueDef
        exp = val_def.GetField(pos_AS_ValueDef_val);
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ImplFnDef: {
        // ImplFnDef and ExplFnDef are ignored.
        return true;
      }
    }
  }
  return !err;
}

bool UMLPARSE::parse_identifier(const wstring & s)
  // Returns true if s is recognized as an identifier.
{
  TYPE_AS_Pattern pat;
  bool err = str2pat(s, pat);
  return err && pat.Is(TAG_TYPE_AS_PatternName);  // close to an identifier...
}

bool UMLPARSE::ParseIdentifierSeq(const Sequence & s)
{
  wstring str (s.GetString());
  return parse_identifier(str);
}

Tuple UMLPARSE::Seq2Type(const Sequence & s)
{
  wstring str (s.GetString());
  TYPE_AS_Type type;
  if (str2type(str, type))
    return mk_(Bool(true), type);
  else
    return mk_(Bool(false), Nil());
}

Tuple UMLPARSE::Seq2Expr(const Sequence & s)
{
  wstring str (s.GetString());
  TYPE_AS_Expr exp;
  if (str2expr(str, exp))
    return mk_(Bool(true), exp);
  else
    return mk_(Bool(false), Nil());
}

Tuple UMLPARSE::Seq2Pat(const Sequence & s)
{
  wstring str (s.GetString());
  TYPE_AS_Pattern pat;
  if (str2pat(str, pat))
    return mk_(Bool(true), pat);
  else
    return mk_(Bool(false), Nil());
}


