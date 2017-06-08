/***
*  * WHAT
*  *    Command line value parsing
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/vdmpars.h,v $
*  * VERSION
*  *    $Revision: 1.12 $
*  * DATE
*  *    $Date: 2005/07/04 00:02:05 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __vdmpars_h__
#define __vdmpars_h__

#include "metaiv.h"
#include "AS.h"
#include "Errors.h"
#include "contextinfo.h"
#include "scanner.h"

class VDMPARSER
{
public:
  static Sequence modorder;
  static Sequence orderly;

public:
  static bool pars_vdm_string(const string & str,
                              bool,
                              vdm_Errors *,
                              Sequence & res, 
                              ContextInfo & ci,
                              bool reset_ci,
                              const TYPE_CI_FileId & fid);
  static bool pars_vdm_stream(istream & in,
                              bool,
                              vdm_Errors *,
                              Sequence & res, 
                              ContextInfo & ci,
                              bool reset_ci,
                              const TYPE_CI_FileId & fid);

  static TYPE_CI_ContextId SetPos(YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last);
  static TYPE_CI_ContextId SetPos3(Record & node, YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last);
  static TYPE_CI_ContextId SetPos2(Record & node, YYLTYPE & yyltp, YYLTYPE & yyltp_last);
  static TYPE_AS_BoolLit GetAstTrue();
  static TYPE_AS_Ids GetIdentifier(const string & idname);

  static TYPE_AS_Expr StateDesignator2Expr(const TYPE_AS_StateDesignator &);
  static Tuple ProperSDInAsgnStmt(const TYPE_AS_StateDesignator &);

  static void Report (const wchar_t*);
  static void Report (const wchar_t*, YYLTYPE& pos);
  static void Report (const wchar_t*, YYLTYPE& pos, YYLTYPE& pos2);

  static void CheckNoDelimiter(YYLTYPE& pos);

  static wstring GetTokenText(int token);

  static void AddModOrder(const TYPE_AS_Name & nm, bool trailingtex);
  static void AddOrderly(int tag, const Generic & info)
  { VDMPARSER::orderly.ImpAppend(mk_(Int(tag), info)); }

public:
  static Tuple NameOverlapInDefs(const TYPE_AS_Definitions & defblock1,
                                 const Map & def_map,
                                 const wstring & single,
                                 const wstring & plural) ;
#ifdef VDMSL
  static Tuple NameOverlapInStateDef(const TYPE_AS_Definitions & defblock1, const TYPE_AS_StateDef & statedef);
#endif // VDMSL
#ifdef VDMPP
  static Generic GetMethType(const Record &);
#endif // VDMPP

  static bool DomOverlap(const Map &, const Map &);

#ifdef VDMPP
  static Tuple InstVarDomOverlap(const SEQ<TYPE_AS_InstanceVarDef> &);
  static Map MergeOpFnDef(const Map & m1, const Map & m2);
  static TYPE_AS_Name GetMangleName(const Record & rec);
#endif // VDMPP

private:
  static bool pars_vdm(bool,
                       vdm_Errors *,
                       Sequence & res, 
                       ContextInfo & ci,
                       bool reset_ci,
                       const TYPE_CI_FileId & fid);

  static Sequence ReportNameOverlapInDefs(const Set & nms, const wstring & single, const wstring & plural);

public:
  static Sequence * asts;

  static TYPE_CI_FileId FileId;
  static vdm_Errors * Errsp;
  static ContextInfo * pars_context_info_p;
  static bool errors;

#ifdef VDMPP
  static Set mangledOpNames;
  static Set mangledFnNames;
#endif // VDMPP
  static Generic currentClassMod;
};

#ifndef YYINITDEPTH
#define YYINITDEPTH 4000
#endif

#endif // __vdmpars_h__
