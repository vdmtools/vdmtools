/***
*  * WHAT
*  *    Auxilary functions used when reading and updating the repository of
*  *    Rose 4.0 and Rose98.
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/rose_aux.h,v $
*  * VERSION
*  *    $Revision: 1.8 $
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

#ifndef __ROSE_AUX_H__
#define __ROSE_AUX_H__

#define NOMINMAX  // In order to avoid redefinition of min and max.
#include "stdafx.h"
#include "metaiv.h"
#include "AUML.h"

#define RI_INHERITANCE 1
#define RI_ITEM        2
#define RI_ASSOCIATION 3

#define RI_TBD         "ToBeDeleted"
#define RI_ORDERED_Q   Quote(L"ORDERED")
#define RI_NORMAL_Q    Quote(L"NORMAL")
#define RI_INPUT_Q     Quote(L"INPUT")
#define RI_EXPL_Q      Quote(L"EXPL")
#define RI_TOTAL_Q     Quote(L"TOTAL")

#define RI_ORDERED     L"ordered"
#define RI_INPUT       L"{input}"

#define RI_MAX_VALUE_SIZE 50
#define RI_TRUNC_STRING   L"... (truncated, do not edit)"

#define ROSE_INSTANCE_VAR_DEF L"<<instance variables>>"
#define ROSE_VALUES_DEF L"<<values>>"
#define ROSE_TIME_VAR_DEF L"<<time variables>>"
#define ROSE_METH_DEF L"<<operations>>"
#define ROSE_FUNC_DEF L"<<functions>>"

#define ROSE_PUBLIC L"PublicAccess"
#define ROSE_PRIVATE L"PrivateAccess"
#define ROSE_PROTECTED L"ProtectedAccess"
#define ROSE_IMPLEMENTATION L"ImplementationAccess"

#define QUOTE_PUBLIC Quote(L"PUBLIC")
#define QUOTE_PRIVATE Quote(L"PRIVATE")
#define QUOTE_PROTECTED Quote(L"PROTECTED")

#define ROSE_INSTANCE_VAR L"instance variable"
#define ROSE_VALUE L"value"
#define ROSE_TIME_VAR L"time variable"
#define ROSE_METH L"operation"
#define ROSE_FUNC L"function"
#define ROSE_STATIC 1


enum {INSTANCE_VAR, TIME_VAR, VALUE_DEF, FUNCTION_DEF, OPERATION_DEF}; 
// Used to signal the current state while parsing the attributes
// and operations of a class in UML

//
// Various functions:
//

CString Attribute2Cstr(const CString & nm, const CString & tp,
                       const CString & initval);
CString Trim(CString);
CString ExtractSubString(const CString & str, const CString & subs);
SEQ<Char> cstr2seq(const CString & cstr);
Bool bstr2bool(const CString & bstr);
Bool BOOL2bool(const BOOL & b);
CString seq2cstr(const Sequence & s);
void ListSetOfNames(const Set & s);
bool ParseInstVarInit(const wstring & tp, const wstring & initval,
                      TYPE_AUML_ValDef &val);
bool ParseValuePart(const wstring & tp, const wstring & initval,
                    TYPE_AUML_ValDef &val);
bool ParseValDef(const wstring & tp, const wstring & initval,
                 const Generic & valinit, TYPE_AUML_ValDef &val);
bool IsOrdered(const CString & rlnm);
CString GetRoleName(CString rlnm);

#endif // __ROSE_AUX_H__

