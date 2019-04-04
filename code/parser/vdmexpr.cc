/***
*  * WHAT
*  *    VDM expression parser
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/vdmexpr.cc,v $
*  * VERSION
*  *    $Revision: 1.35 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:41 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IPTES - ESPRIT programme, project no. 5570.
*  * AUTHOR
*  *    Kees de Bruin (KDB) + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <stdlib.h>		// prototypes for exit, srand and getopt
#include <time.h>		// prototype for time

#include "pexpr.h"
#include "tbutils.h"
#include "tools.h"
#include "metaiv.h"
#include "date.h"
#include "intconvquotes.h"

#include "Errors.h"
#include "ToolMediator.h"
#include "projectval.h"
#include "BatchErr.h"
#include "tb_exceptions.h"
#include "AS.h"
#include "REP.h"

vdm_ToolKit * toolkit = NULL;

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
    if (file.IsRecord () && Record(file).Is(vdm_FileName)) {
      return ExtractFileName(file);
    }
    else {
      return L"";
    }
  }
  
public:

  AsciiErrors () : run_time (true) {
    wchar_t *_emacs; 
    emacs = ((_emacs=getenv(L"EMACS")) && strcmp(_emacs, "t")==0);
  }
  
  virtual void vdm_SetFilePosition(const Record & msg)
  {
    int fileid = Int (msg.GetField (pos_ErrMsg_fid)).GetValue();
    int lineid = Int (msg.GetField (pos_ErrMsg_line)).GetValue();
    int columnid = Int (msg.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);

    if (file != L"") {
      if (emacs) {
	vdm_err << L"\032\032" << file << ":" << lineid << ":1\n";
      }
      if (run_time) {
	vdm_err << L"In " << file << ", l." << lineid << L", c." << columnid << "\n";
      }
    }
    else if (fileid != 0) {
      vdm_err << L"c." << columnid << " in the command-line\n";
    }
  }

  virtual void vdm_ShowMsg() {
    run_time = false;
    Record msg = vdm_msgs[vdm_msgPtr];
    vdm_SetFilePosition(msg);
    wstring file = GetFileName (Int (msg.GetField (pos_ErrMsg_fid))); 
    vdm_err << ExtractMessageAll(file, msg);
    run_time = true;
  }
  
  virtual void vdm_MakeStatus(const Int & NoErr, const Int & NoWarn)
  {
    wstring msg;
    if (NoErr > 0) {
      msg = L"Errors: " + NoErr.ascii();
    }
    if (NoWarn > 0) {
      if (NoErr > 0) {
        msg += L" ";
      }
      msg += L"Warnings: " + NoWarn.ascii();
    }
    
    if ((NoErr > 0) || (NoWarn > 0)) {
      vdm_err << msg << endl << flush;
    }
  }
};


int main()
{

  AsciiErrors * Errs = new AsciiErrors();

  ObjectRef batch_state (new vdm_BatchErr()); ///LTO ???

  //  type_ref_ErrorState batch_state (new vdm_BatchErr());
  Errs->vdm_InitState((Generic) batch_state); // LTO cast ???


  (void) VDMSetErrorHandler(EXC_ThrowHandler);

  // Initialise AS and REP types.
  init_AS();
  init_REP();
  
  try {

    int result = 0;
    Record ARGS;
    LEXINPUT.ReadFrom (cin);
    Sequence exprs; 
    ContextInfo contextinfo; 
    result = pars_vdm_expr (true,
                            Errs,
                            exprs, 
                            contextinfo,
                            true,
                            TYPE_CI_FileId(CI_FID_STDIN));
#ifdef VDMPP
    //    result = pars_afro_vdmpp_expr (true, exprs);
#endif // VDMPP
    if (!result) {
      ARGS = Record (exprs[1]);
    }
    else {
      vdm_err <<  L"Some errors occured during parsing of the expression\n";
      return (2);
    }         
    // Output ast 
    {
      ofstream m4pp(L"m4pp"); 
      Map m; 
      m.Insert(Int(CI_FID_STDIN),Sequence(L"stdin"));
      PrintAstTuple(m4pp, ARGS, contextinfo, m); 
    }
    return 0;
    
  }
  catch (TB_Exception & e) {
    vdm_log << L"Internal error\n";
    return 1;
  }
}

