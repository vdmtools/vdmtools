/***
*  * WHAT
*  *    Command line parsing of creation of objects
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/vppstmt.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 1997/06/23 12:44:52 $
*  * FORMAT
*  *    $State: Exp $
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: storm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __vppstmt_h__
#define __vppstmt_h__

#include "metaiv.h"
#include "AS.h"
#include "Errors.h"
#include "contextinfo.h"
#include "scanner.h"

class VDMSTMTPARSER
{
public:
  static bool pars_vpp_asgn_invoke_stmt(const string & str, bool, vdm_Errors*, TYPE_AS_AssignStmt & res);

  static TYPE_CI_ContextId SetPos(YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last);
  static TYPE_CI_ContextId SetPos3(Record & node, YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last);
  static TYPE_CI_ContextId SetPos2(Record & node, YYLTYPE & yyltp, YYLTYPE & yyltp_last);
  static TYPE_AS_BoolLit GetAstTrue();
  static TYPE_AS_Ids GetIdentifier(const string & idname);

#ifdef VDMPP
  static TYPE_AS_Expr StateDesignator2Expr(const TYPE_AS_StateDesignator &);
  static Tuple ProperSDInAsgnStmt(const TYPE_AS_StateDesignator &);
#endif // VDMPP

  static void Report (const wchar_t*);
  static void Report (const wchar_t*, YYLTYPE& pos);
  static void Report (const wchar_t*, YYLTYPE& pos, YYLTYPE& pos2);

  static void CheckNoDelimiter(YYLTYPE& pos);

public:
  static TYPE_AS_AssignStmt * stmt;

  static TYPE_CI_FileId FileId;
  static vdm_Errors * Errsp;
  static ContextInfo * pars_context_info_p;
  static bool errors;

};

#ifndef YYINITDEPTH
#define YYINITDEPTH 2000
#endif

#endif // __vppstmt_h__
