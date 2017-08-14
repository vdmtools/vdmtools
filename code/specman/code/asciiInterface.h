/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/asciiInterface.h,v $
*  * VERSION
*  *    $Revision: 1.12 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __asciiInterface_h__
#define __asciiInterface_h__

#include "Interface.h"
#include "Errors.h"
#include "metaiv.h"
#include "projectval.h"
#include "tbutils.h"
#include "tools.h"
#ifdef __linux__
#include <stdlib.h>
#endif // __linux__
#ifdef __CYGWIN__
#include <stdlib.h>
#endif // __CYGWIN__

//
// The ASCII interface class
//
class AsciiInterface : public vdm_Interface
{
public:
  virtual void vdm_ChangedFile() {}
  virtual void vdm_ChangedMod() {}

  virtual void vdm_Log(const type_cL & msg) {
    vdm_log << PTAUX::Seq2Str (msg) << endl << flush;
  }

  virtual void vdm_InterpreterLog(const type_cL & msg) {
    vdm_iplog << PTAUX::Seq2Str (msg) << endl << flush;
  }

  virtual void vdm_Error(const type_cL & msg) {
    vdm_err << PTAUX::Seq2Str (msg) << endl << flush;
  }

  virtual void vdm_ExecuteMainLoop ()
  {
    TOOLS::ExecuteMainLoop ();
  }

  virtual void vdm_CallBack (const Record &) {}
  virtual void vdm_EnableUserInput () {}
  virtual void vdm_DisableUserInput () {
  }
  virtual void vdm_RefreshInterface() {}
};


class AsciiErrors : public vdm_Errors
{
private:

  // emacs is true if the toolbox is running under emacs
  bool emacs;

  // run_time indicates whether the error class currently is
  // reporting a run-time error.
  bool run_time;

//  virtual wstring GetFileName(int fileid)
//  {
//    vdm_ToolMediator * med = ObjGet_vdm_ToolMediator (vdm_mediator);
//    Generic file = med->vdm_GetFileName (fileid);
//    if (file.IsRecord () && Record(file).Is(vdm_FileName))
//      return PTAUX::ExtractFileName(file);
//    else
//      return L"";
//  }

public:

  AsciiErrors ()
  {
    char* _emacs = NULL;
    run_time = true;
    emacs = ((_emacs=getenv("EMACS")) && strcmp(_emacs, "t")==0);
  }

  virtual void vdm_SetFilePosition(const Record & msg)
  {
    int fileid = msg.GetIntValue (pos_ErrMsg_fid);
    int lineid = msg.GetIntValue (pos_ErrMsg_line);
    //    int columnid = Int (msg.GetField (pos_ErrMsg_col)).GetValue();

    //wstring file = GetFileName (fileid);
    wstring file (ToolMediator::GetFileName (fileid));

    if (file.length() > 0) {
      TYPE_ProjectTypes_FileName tfn = ToolMediator::Repos()->vdm_GetTempFileName (PTAUX::mk_FileName (file));
      wstring tmpfn = PTAUX::ExtractFileName (tfn);
      if (tmpfn.length() > 0)
        file = tmpfn;
      if (emacs)
        vdm_err << L"\032\032" << file << L":" << lineid << L":1"
                << endl << flush;

      //if (run_time)
      //vdm_err << L"In " << file << L", l." << lineid
      //<< L", c." << columnid << endl;
    }
    else if (fileid != 0) {
      // vdm_err << L"c." << columnid << L" in the command-line" << endl;
    }
  }

  bool isRuntimeError(const Record & r)
  {
    const type_cLL & tk_l (r.GetSequence(pos_ErrMsg_msg));
    if (!tk_l.IsEmpty() )
    {
      SEQ<Char> tk (tk_l.Hd());
      wstring mes (tk.GetString());
      wstring rt (L"Run-Time Error");
      if (mes.length() > rt.length())
        return (mes.substr(0, rt.length()) == rt);
    }
    return false;
  }

  virtual void vdm_ShowMsg() {
    run_time = false;
    Record msg (vdm_msgs[vdm_msgPtr]);
    vdm_SetFilePosition(msg);
    wstring file (ToolMediator::GetFileName (msg.GetInt (pos_ErrMsg_fid)));

    if (Int(CI_FID_CMD_LINE) == msg.GetInt (pos_ErrMsg_fid))
      file = wstring(L"command line");

    if (isRuntimeError(msg))
    {
      vdm_iplog << PTAUX::ExtractMessageAll(file, msg) << flush;
    }
    else if ((cliOpt) && (cliOpt->IsBatchMode()))
    {
      vdm_err << PTAUX::ExtractMessageAll(file, msg) << flush;
    }
    else
    {
      vdm_log << PTAUX::ExtractMessageAll(file, msg) << flush;
    }
    run_time = true;
  }

  virtual void vdm_EventAddMsg (const Int &vdm_mp, const TYPE_ProjectTypes_Message &vdm_ms)
  {
//vdm_err << vdm_ms << endl;
  }

  virtual void vdm_MakeStatus(const Int & NoErr, const Int & NoWarn)
  {
    wstring ne (NoErr.ascii());
    wstring nw (NoWarn.ascii());
    wstring msg;
    if (NoErr > 0)
      msg = L"Errors: " + ne + L" ";
    if (NoWarn > 0)
      msg = msg + L"Warnings: " + nw + L"\n";
    else if (NoErr > 0)
      msg = msg + L"\n";
//    Sequence s;
//    Token t = Token(msg);
//    s.ImpAppend(t);
    vdm_err << msg << flush;
  }
};

#endif // __asciiInterface_h__
