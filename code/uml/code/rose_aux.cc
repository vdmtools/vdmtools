/***
*  * WHAT
*  *    Auxilary functions used when reading and updating the repository of
*  *    Rose 4.0 and Rose98.
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/rose_aux.cc,v $
*  * VERSION
*  *    $Revision: 1.7 $
*  * DATE
*  *    $Date: 2006/04/19 08:11:25 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "rose_aux.h"
#include "parse_string.h"

CString Trim(CString s)
  // Removes leading and trailing whitespace
{
  s.TrimLeft();
  s.TrimRight();
  return s;
}


CString Attribute2Cstr(const CString & nm, const CString & tp,
                       const CString & initval)
  // Returns a wstring representing the attribute at. Used when
  // displaying warnings to the user.
{
  CString res = nm;
  res += tp.IsEmpty() ? L"" : " : " + tp;
  res += initval.IsEmpty() ? L"" : " = " + initval;
  return res;
}


CString ExtractSubString(const CString & str, const CString & subs)
  // Extracts the subwstring subs from the wstring str.
{
  int start = str.Find(subs);
  if(start>=0) {
    return str.Mid(0, start) + str.Mid(start + subs.GetLength());
  }
  else {
    return str;
  }
}


CString seq2cstr(const Sequence & s)
{
  wstring str;
  s.GetString(str);
  return CString(str.c_str());
}


SEQ<Char> cstr2seq(const CString & cstr)
{
  return SEQ<Char>(wstring(cstr));
}

Bool bstr2bool(const CString & bstr)
{
  if (bstr == L"") {
    return Bool(false);
  }
  else {
    return Bool(true);
  }
}

Bool BOOL2bool(const BOOL & b)
{
  if (b == false) {
    return Bool(false);
  }
  else {
    return Bool(true);
  }
}

void ListSetOfNames(const Set & s)
  // Takes a set of sequence of wchar_t and displays their contents one
  // item at a time in a message box.
{
  Generic gc;
  wstring str;
  for (int i=s.First(gc); i; i=s.Next(gc)) {
    Sequence(gc).GetString(str);
    AfxMessageBox(str.c_str());
  }
}

bool ParseInstVarInit(const wstring & tp, const wstring & initval,
                     TYPE_AUML_ValDef &val)
{
  Generic valinit = Nil ();
  return ParseValDef (tp, initval, valinit, val);
}

bool ParseValuePart(const wstring & tp, const wstring & initval,
                   TYPE_AUML_ValDef &val)
{
  TYPE_AS_UndefinedExpr udef;
  return ParseValDef (tp, initval, udef, val);
}

bool ParseValDef(const wstring & tp,
                 const wstring & initval,
                 const Generic & valinit,
                 TYPE_AUML_ValDef &val)
  // Parses the type in tp and the expression in initval. Returns true
  // if the type and expression is syntax correct. The resulting ast's
  // are returned in the record val.
{
  TYPE_AS_Type as_tp;
  TYPE_AS_Expr as_expr;
  bool success = true;

  success = tp == L"" ? true : UMLPARSE::str2type(tp, as_tp);
  success = success && (initval == L"" ? true : UMLPARSE::str2expr(initval, as_expr));
  
  if(tp == L"") {
    val.set_tp(Nil());
  }
  else {
    val.set_tp(as_tp);
  }
  if(initval == L"") {
    val.set_expr(valinit);
  }
  else {
    val.set_expr(as_expr);
  }
  return success;
}
                               
bool IsOrdered(const CString & rlnm)
{
  return rlnm.Find(RI_ORDERED) >= 0;
}


CString GetRoleName(CString rlnm)
{
  int pos;
  if ((pos = rlnm.Find(RI_ORDERED)) >= 0){
    //  Extracted the role name:
    rlnm = rlnm.Left(pos);
    rlnm.TrimRight();  // remove trailing whitespace...
  }
  return rlnm;
}
