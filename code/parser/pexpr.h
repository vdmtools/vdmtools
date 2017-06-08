/***
*  * WHAT
*  *    Command line expr parsing
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/pexpr.h,v $
*  * VERSION
*  *    $Revision: 1.9 $
*  * DATE
*  *    $Date: 1998/03/30 13:54:56 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: erik $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __pexpr_h__
#define __pexpr_h__

#include "metaiv.h"
#include "AS.h"
#include "Errors.h"
#include "contextinfo.h"
#include "scanner.h"

class VDMEXPRPARSER
{
public:
  static bool pars_vdm_expr_string(const string & str,
                                   bool POS_WANTED,
                                   vdm_Errors *,
                                   Sequence & res, 
                                   ContextInfo & ci,
                                   bool reset_ci,
                                   const TYPE_CI_FileId & fid);

  static bool pars_vdm_expr_stream(istream & in,
                                   bool POS_WANTED,
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

public:
  static Sequence * exprs;

  static TYPE_CI_FileId FileId;
  static vdm_Errors * Errsp;
  static ContextInfo * pars_context_info_p;
  static bool errors;

private:
  static bool pars_vdm_expr(bool POS_WANTED,
                            vdm_Errors *,
                            Sequence & res, 
                            ContextInfo & ci,
                            bool reset_ci,
                            const TYPE_CI_FileId & fid);
};

#ifndef YYINITDEPTH
#define YYINITDEPTH 2000
#endif

#endif // __pexpr_h__

