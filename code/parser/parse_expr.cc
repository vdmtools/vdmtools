/***
*  * WHAT
*  *    VDM expression parser
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/parse_expr.cc,v $
*  * VERSION
*  *    $Revision: 1.9 $
*  * DATE
*  *    $Date: 2001/09/27 12:35:43 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Hanne Carlsen + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <stdlib.h>		// prototypes for exit, srand and getopt
#include <time.h>		// prototype for time

#include "metaiv.h"
#include "pexpr.h"
#include "date.h"
#include "tbutils.h"

#include "Errors.h"
#include "ToolMediator.h"
#include "projectval.h"
#include "tb_exceptions.h"
#include "parse_expr.h"
#include "tb_wstring.h"

class AsciiErrors : public vdm_Errors
{
private:

  // emacs is true if the toolbox is running under emacs
  bool emacs;

  // run_time indicates whether the error class currently is
  // reposting a run-time error.
  bool run_time; 
  
  virtual wstring GetFileName(int fileid)
  {
    vdm_ToolMediator * med = ObjGet_vdm_ToolMediator (vdm_mediator);
    Generic file = med->vdm_GetFileName (fileid);
    if (file.IsRecord () && Record(file).Is(vdm_FileName))
      return PTAUX::ExtractFileName(file);
    else
      return L"";
  }
  
public:

  AsciiErrors ()
    : run_time(true)
  {
    char *_emacs; 
    emacs = ((_emacs=getenv("EMACS")) && strcmp(_emacs, "t")==0);
  }
  
  virtual void vdm_SetFilePosition(const Record &msg)
  {
    int fileid = Int (msg.GetField (pos_ErrMsg_fid)).GetValue();
    int lineid = Int (msg.GetField (pos_ErrMsg_line)).GetValue();
    int columnid = Int (msg.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);

    if (file != L"") {
      if (emacs)
	vdm_err << L"\032\032" << file << ":" << lineid << ":1\n";

      if (run_time)
	vdm_err << L"In " << file << ", l." << lineid
		<< L", c." << columnid << "\n";
    }
    else if (fileid != 0)
      vdm_err << L"c." << columnid << " in the command-line\n";
  }

  virtual void vdm_ShowMsg() {
    run_time = false;
    Record msg = vdm_msgs[vdm_msgPtr];
    vdm_SetFilePosition(msg);
    wstring file = GetFileName (Int (msg.GetField (pos_ErrMsg_fid))); 
    vdm_err << PTAUX::ExtractMessageAll(file, msg);
    run_time = true;
  }
  
  virtual void vdm_MakeStatus(const Int &NoErr, const Int &NoWarn)
  {
    wstring msg;
    if (NoErr > 0)
      msg = L"Errors: " + NoErr.ascii();

    if (NoWarn > 0)
    {
      if (NoErr > 0) msg += L" ";
      msg += L"Warnings: " + NoWarn.ascii();
    }

    if ((NoErr > 0) || (NoWarn > 0))
    {
      vdm_err << msg << endl << flush;
    }
  }
};


bool ParseAnExprFromStream(Sequence & exprs, ContextInfo & ci, istream & in)
{
  AsciiErrors * Errs = new AsciiErrors();

  (void) VDMSetErrorHandler(TB_Exception::EXC_ThrowHandler);

  try {
    bool parse_error = VDMEXPRPARSER::pars_vdm_expr_stream (in, true, Errs, exprs, ci,
                                                            false, TYPE_CI_FileId(CI_FID_STDIN));
    if (parse_error) {
      vdm_err <<  L"Some error occured during parsing of the expression" << endl;
    }
    else
      return parse_error;
  }
  catch (TB_Exception & e) {
    vdm_log << L"Internal error" << endl;
    return false;
  }
  return false; 
}

bool ParseAnExprFromString(Sequence & exprs, ContextInfo & ci, string & str)
{
  AsciiErrors * Errs = new AsciiErrors();

  (void) VDMSetErrorHandler(TB_Exception::EXC_ThrowHandler);

  try {
    bool parse_error = VDMEXPRPARSER::pars_vdm_expr_string (str, true, Errs, exprs, ci,
                                                            false, TYPE_CI_FileId(CI_FID_STDIN));
    if (parse_error) {
      vdm_err <<  L"Some error occured during parsing of the expression" << endl;
    }
    else
      return parse_error;
  }
  catch (TB_Exception & e) {
    vdm_log << L"Internal error" << endl;
    return false;
  }
  return false; 
}

// Parses an expression from standard input.

bool ParseAnExpr(Sequence& exprs, ContextInfo& ci)
{
  std::string st;
  char ch;
  ch = std::cin.get ();
  while (ch != (char)EOF)
  {
    st += ch;
    ch = std::cin.get ();
  }
  return ParseAnExprFromString(exprs,ci,st);
}

