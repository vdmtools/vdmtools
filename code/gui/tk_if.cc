//**
//** WHAT
//**    VDM-SL Toolbox GUI interface functions. 
//** ID
//**    $Id: tk_if.cc,v 1.125 2001/06/12 15:04:26 paulm Exp $
//** COPYRIGHT
//**    (C) Kyushu University
//**

#ifdef _MSC_VER 
#include <windows.h>
#endif

#include <tk.h>
#undef Bool

#include <tcl.h>

#include <stdlib.h>
#include <stdio.h>
#include "tb_string.h"
#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

#undef Status
#include "main_tag.h"
#include "ProjectTypes.h"
#include "astaux.h"

#include "tb_exceptions.h"

#if defined(USEUML) && defined(_MSC_VER)
#include "uml-be.h"
#endif //#if defined(USEUML) && defined(_MSC_VER)

#include <strstream.h>
#ifdef _MSC_VER
// For chdir
#include <direct.h>
#endif
#include "tb_string.h"


//extern L"C" {
//#include L"tcl.h"
//}

#include <iostream.h>
#include <sys/stat.h>
#include <limits.h>
#ifndef MAXPATHLEN
//#include <sys/param.h>
#endif

#include "tbutils.h"
#include "tools.h"
#include "tb_version.h"
#include "val2x.h"
#include "projectval.h"
#include "ToolKit.h"
#include "BaseTools.h"
#include "Repository.h"
#include "UMLTool.h"
// Gross HACK 
#include "asquotes.h"
#include "eval.h"

#include "Interface.h"
#include "Errors.h"
#include "BatchErr.h"
#include "evalstop.h"

#ifdef __alpha
#define CONST_CAST(c) (c)
#else
#define CONST_CAST(c)  const_cast<c>
#endif // __alpha

extern int Tcl_GetHex (Tcl_Interp *interp, wchar_t *str, long *aLong);
extern int VSL_GetClasses (ClientData, Tcl_Interp *, int, wchar_t **);

Tcl_Interp * Local_Interp;

extern Tcl_Interp *guiInterp;

vdm_ToolKit * toolkit; 
ObjectRef * vdm_TK_p;
ObjectRef * vdm_if;
#if defined(USEUML) && defined(_MSC_VER)
UMLMapper * UML;
#endif //#if defined(USEUML) && defined(_MSC_VER)

static bool MainLoopContinue;

static void checkEval(Tcl_Interp * interp, const wchar_t* cmd)
{
  wchar_t * s  = new wchar_t[strlen(cmd)+1];
  strcpy(s, cmd); 
  int code = Tcl_Eval(interp, CONST_CAST(wchar_t*)(s) );
  if (code != TCL_OK) {
    cerr << L"TCL error: " << s << ":" << interp->result << "\n";
  }
  delete [] s; 
}

static void exeTclCommand(const wstring & s) 
{
  checkEval(guiInterp, s.c_str()); 
}

static void EventToCommands(const Record& cmd);

static
void
if_flush_buffers()
{
  vdm_log.flush();
  vdm_err.flush();
}

void startTimerProc(Tcl_Interp * interp)
  // This function should only be called ONCE! It spawns a new thread
  // that listens for events through the Toolbox API
{

  // Start a periodic L"thread" in Tcl that calls tkif_Periodic to get
  // Tcl commands
  wchar_t *cmd = L"proc timerProc {} { " 
    L" set res [tkif_Periodic]; "
    L"after 200 timerProc; }\n"
    L"timerProc";
  checkEval(interp, cmd);
}

class TestInterface : public vdm_Interface
{
private:
  Sequence bt_seq;
  bool meter_initialized;
  
public:
  virtual Generic vdm_GetBtSeq() { return bt_seq; };
  virtual void vdm_SetBtSeq(const Generic & new_bt_seq) {
    if (new_bt_seq.IsSequence())
      bt_seq = new_bt_seq; 
  };
  virtual void vdm_ChangedFile() {};
  virtual void vdm_Log(const type_cL &  msg) {
    vdm_log << Seq2Str (msg) << L"\n" << flush;
  }
  virtual void vdm_ChangedMod()
  {
    // BTCast (toolkit)->vdm_ExecuteCommand (mk_ToolCommand (L"modules"));
  };

  virtual void vdm_Error(const type_cL& ) {
    vdm_log << L"TestInterface::vdm_Error is not yet implemented\n" << flush;
  }

  virtual void vdm_ExecuteMainLoop ()
  {
    ::MainLoopContinue = true;
    if_flush_buffers(); 
    exeTclCommand(L"command`insertPrompt");   
    while (::MainLoopContinue) {
      Tcl_DoOneEvent(0);         // 0==>Process all types of events.      
    }
  }

  virtual void vdm_CallBack (const Record & cmd)
  {
    EventToCommands(cmd);
  }  

  virtual void vdm_InitMeter(const type_cL & title, const type_cL & label)
  {
    Tcl_ResetResult(Local_Interp);                
    Tcl_AppendElement(Local_Interp, L"general`initMeter");
    wstring txt;
    title.GetString(txt);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(txt.c_str()));
    label.GetString(txt);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(txt.c_str()));
    exeTclCommand(Local_Interp->result);   
    meter_initialized = true;
  }


  virtual void vdm_UpdateMeter(const Real& pct, const type_cL& text)
  {
    if(meter_initialized){
      Tcl_ResetResult(Local_Interp);                
      Tcl_AppendElement(Local_Interp, L"general`setMeter");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(pct.ascii().c_str()));
      wstring txt;
      text.GetString(txt);
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(txt.c_str()));
      exeTclCommand(Local_Interp->result);   
    }
  }


  virtual void vdm_DestroyMeter()
  {
    Tcl_ResetResult(Local_Interp);                
    Tcl_AppendElement(Local_Interp, L"general`destroyMeter");
    exeTclCommand(Local_Interp->result);   
    meter_initialized = false;
  }

  virtual void vdm_EnableUserInput () {
    checkEval(guiInterp,L"function`enableInterface");
  }
  virtual void vdm_DisableUserInput () {
    LastCountTime = clock();
    checkEval(guiInterp,L"function`disableInterface");
  }
  virtual void vdm_RefreshInterface() {
    checkEval(guiInterp,L"update");
  }
};

static wstring LogProc;          // Used for controlling where Log output
                                // goes.

static
void
SetLogProc(const wchar_t* lp)
{
  // Sometimes we may be called with an empty log proc. In this case
  // we don't modify it.
  if (strlen(lp)>0) 
    LogProc = lp;
}

//static
//int
//Log(const wchar_t * msg, int n)
//{
//  Tcl_ResetResult(VSL_Interp);
//  if (LogProc == L"") { 
//    Tcl_AppendElement(VSL_Interp, L"log`add");
//  } else {
//    Tcl_AppendElement(VSL_Interp, CONST_CAST(wchar_t*)(LogProc.c_str()));
//  }
//  wstring st(msg, n); 
//  Tcl_AppendElement(VSL_Interp, (wchar_t*) st.c_str());
//  if (Tcl_VarEval(VSL_Interp, VSL_Interp->result, (wchar_t*) NULL) != TCL_OK) {
//    cerr <<L"Error (Message): " << VSL_Interp->result << "\n";
//  }
//  Tcl_ResetResult(VSL_Interp); 
//  return 1;
//}
//
//static
//int
//ELog (const wchar_t*msg, int n)
//{
//  Tcl_ResetResult(VSL_Interp);
//  if (LogProc == L"") { 
//    Tcl_AppendElement(VSL_Interp, L"log`add");
//  } else {
//    Tcl_AppendElement(VSL_Interp, CONST_CAST(wchar_t*)(LogProc.c_str()));
//  }
//  wstring st(msg, n); 
//  Tcl_AppendElement(VSL_Interp, (wchar_t*) st.c_str());
//  if (Tcl_VarEval(VSL_Interp, VSL_Interp->result, (wchar_t*) NULL) != TCL_OK) {
//    cerr <<L"Error (Message): " << VSL_Interp->result << "\n";
//  }
//  Tcl_ResetResult(VSL_Interp); 
//  return 1;
//}


static
int
Log_Event(const wchar_t * msg, int n)
{
  Tcl_ResetResult(Local_Interp);
  if (LogProc == L"") { 
    Tcl_AppendElement(Local_Interp, L"log`add");
  } else {
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(LogProc.c_str()));
  }
  wstring st(msg, n); 
  Tcl_AppendElement(Local_Interp, (wchar_t*) st.c_str());
  exeTclCommand(Local_Interp->result);     
  return 1;
}

static
int
ELog_Event (const wchar_t*msg, int n)
{
  Tcl_ResetResult(Local_Interp);
  if (LogProc == L"") { 
    Tcl_AppendElement(Local_Interp, L"log`add");
  } else {
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(LogProc.c_str()));
  }
  wstring st(msg, n); 
  Tcl_AppendElement(Local_Interp, (wchar_t*) st.c_str());
  exeTclCommand(Local_Interp->result);     
  return 1;
}


int Tcl_GetHex(Tcl_Interp * /*interp*/, wchar_t *str, long *aLong)
{
    wchar_t		*s = str;

    if (s[0] == '0' && s[1] == 'x')
	s += 2;
    if (sscanf(s, L"%lx", aLong) == 1)
	return TCL_OK;
    else
	return TCL_ERROR;
}

int
tkif_executeCommandPrompt(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
  // Does the same as executeCommand() but adds a command prompt after
  // the command is executed.
{
  // Arguments: command logproc 
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_executeCommandPrompt: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }  
  SetLogProc(argv[2]);
  Tcl_ResetResult(interp);
  Bool cont = BTCast (toolkit)->vdm_ExecuteCommand ( (Generic) mk_ToolCommand (argv[1])); // LTO cast ???
  if_flush_buffers(); 
  ::MainLoopContinue = cont.GetValue();

  // Check if we should insert a prompt. 
  //  if(::MainLoopContinue)
  exeTclCommand(L"command`insertPrompt");
  return TCL_OK; 
}


int
tkif_executeCommand(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // Arguments: command logproc 
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_executeCommand: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }  
  SetLogProc(argv[2]);
  Bool cont = BTCast (toolkit)->vdm_ExecuteCommand ( (Generic) mk_ToolCommand (argv[1])); // LTO cast ???
  if_flush_buffers(); 
  ::MainLoopContinue = cont.GetValue();

  return TCL_OK; 
}


int
tkif_printText(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
#if 0 
  // Arguments: textw 
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_printText: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }  
  Tcl_ResetResult(interp);
#ifdef _MSC_VER 
  extern void tkif_WinPrint();
  tkif_WinPrint();
#else
  // Do nothing
#endif
#endif // Disabled code. 
  return TCL_OK; 
}


static 
void
EventToCommands(const Record& cmd)
{
  int tag;
  tag = cmd.GetTag();
  Tcl_ResetResult(Local_Interp);
  switch (tag) {
  case TAG_TYPE_ProjectTypes_ChangedProjName:
    {
      wstring oldp = ExtractOldProjName(cmd);
      wstring newp = ExtractNewProjName(cmd);
      Tcl_ResetResult(Local_Interp);                
      Tcl_AppendElement(Local_Interp, L"project`changedprojname");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(oldp.c_str()));        
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(newp.c_str()));        
      exeTclCommand(Local_Interp->result);
      break;
    }    
  case TAG_TYPE_ProjectTypes_AddFiles:
    {
      Set f = cmd.GetField(1);
      bool b;
      Generic g;
      Tcl_ResetResult(Local_Interp);
      for (b = f.First(g); b; b=f.Next(g)) {
        wstring s = ExtractFileName(g);
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
      }
      wstring l (Local_Interp->result);
      Tcl_ResetResult(Local_Interp);                
      Tcl_AppendElement(Local_Interp, L"files`addFiles");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(l.c_str()));        
      exeTclCommand(Local_Interp->result);
      break;
    }
  case TAG_TYPE_ProjectTypes_RemoveFiles:
    {
      Set f = cmd.GetField(1);
      bool b;
      Tcl_ResetResult(Local_Interp);
      Generic g; 
      for (b = f.First(g); b; b=f.Next(g)) {
        wstring s = ExtractFileName(g);
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
      }
      wstring l (Local_Interp->result);
      Tcl_ResetResult(Local_Interp);        
      Tcl_AppendElement(Local_Interp, L"files`removeFiles");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(l.c_str()));        
      exeTclCommand(Local_Interp->result);
        
      break;
    }
  case TAG_TYPE_ProjectTypes_AddModules:
    {
      Set f = cmd.GetField(1);
      bool b;
      Generic g; 
      Tcl_ResetResult(Local_Interp);
      for (b = f.First(g); b; b=f.Next(g)) {
        wstring s = ExtractModuleName(g);
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
      }          

      wstring l (Local_Interp->result);
      Tcl_ResetResult(Local_Interp);        
      Tcl_AppendElement(Local_Interp, L"modules`addModules");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(l.c_str()));        
      exeTclCommand(Local_Interp->result);

      break;
    }
  case TAG_TYPE_ProjectTypes_RemoveModules:
    {
      Set f = cmd.GetField(1);
      bool b;
      Generic g;
      Tcl_ResetResult(Local_Interp);
      for (b = f.First(g); b; b=f.Next(g)) {
        wstring s = ExtractModuleName(g);
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
      }
      wstring l (Local_Interp->result);
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, L"modules`removeModules");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(l.c_str()));
      exeTclCommand(Local_Interp->result);
      break;
    }

  case TAG_TYPE_ProjectTypes_ChangedModuleStatus:
    {
      Set f = cmd.GetField(1);
      bool b;
      Generic g; 
      Tcl_ResetResult(Local_Interp);
      for (b = f.First(g); b; b=f.Next(g)) {
        wstring s = ExtractModuleName(g);
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
      }          
      wstring l (Local_Interp->result);
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, L"modules`changeStates");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(l.c_str()));
      exeTclCommand(Local_Interp->result);

      break;
    }

  case TAG_TYPE_ProjectTypes_ChangedFileStatus:
  case TAG_TYPE_ProjectTypes_RemoveFileChangedMark:
    {
      wstring s = ExtractFileName(cmd.GetField(1));
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, L"files`changeState");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
      exeTclCommand(Local_Interp->result);

      break;
    }
    
  case TAG_TYPE_ProjectTypes_BreakNameSet:
    {
      Record asname;
      int num;
      ExtractBreakNameSet(cmd,&asname,&num);
      wstring b = PrintName(asname); 
      Tcl_AppendElement(Local_Interp, L"breakpoint`addBreakName");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(b.c_str()));

      wchar_t buf[10];
      sprintf(buf,L"%d", num);
      Tcl_AppendElement(Local_Interp, buf);

      exeTclCommand(Local_Interp->result);
      Tcl_AppendElement(Local_Interp, buf);
      break; 
    }

  case TAG_TYPE_ProjectTypes_BreakPosSet:
    {
      wstring name;
      int line,col, num;
      wchar_t buf[10];
      
      ExtractBreakPosSet(cmd, &name, &line, &col, &num);

      Tcl_AppendElement(Local_Interp, L"breakpoint`addBreakPos");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(name.c_str()));

      sprintf(buf,L"%d", line);
      Tcl_AppendElement(Local_Interp, buf);

      sprintf(buf,L"%d", col);
      Tcl_AppendElement(Local_Interp, buf);
      
      sprintf(buf,L"%d", num);
      Tcl_AppendElement(Local_Interp, buf);
      
      exeTclCommand(Local_Interp->result);
      break; 
    }

  case TAG_TYPE_ProjectTypes_BreakRemove:
    {
      int num = ExtractBreakRemove(cmd);
      wchar_t buf[10];
      sprintf(buf,L"%d",num);
      
      Tcl_AppendElement(Local_Interp, L"breakpoint`deletebreak");
      Tcl_AppendElement(Local_Interp, buf);
      exeTclCommand(Local_Interp->result);
      break; 
    }

  case TAG_TYPE_ProjectTypes_BreakEnable:
    {
      int num = ExtractBreakEnable(cmd);
      wchar_t buf[10];
      sprintf(buf,L"%d",num);
      
      Tcl_AppendElement(Local_Interp, L"breakpoint`enable");
      Tcl_AppendElement(Local_Interp, buf);
      exeTclCommand(Local_Interp->result);
      break; 
    }

  case TAG_TYPE_ProjectTypes_BreakDisable:
    {
      int num = ExtractBreakRemove(cmd);
      wchar_t buf[10];
      sprintf(buf,L"%d",num);
      
      Tcl_AppendElement(Local_Interp, L"breakpoint`disable");
      Tcl_AppendElement(Local_Interp, buf);
      exeTclCommand(Local_Interp->result);
      break; 
    }

  case TAG_TYPE_ProjectTypes_BackTrace:
    {
      Sequence bt_seq = cmd.GetField(1);
      (InfCast (toolkit)->vdm_SetBtSeq(bt_seq));
      exeTclCommand(L"debug_trace`clear");
      for (int i=1; i<= bt_seq.Length(); i++) {
        Tcl_ResetResult(Local_Interp);
        ostrstream os, os_args; 
        Tuple info (bt_seq[i]);
        wstring name;
        Sequence strName = info.GetField (1);
        strName.GetString(name);
        os << L"#" << i-1 << ": "
           << name;
        // int maxj = args_lv.Length ();
        // for (int j = 1; j <= maxj; j++) {
        //   if (j > 1)
        //     os_args << L", ";
        //   val2stream( Record(args_lv[j]), os_args, Setting_PRINT_FORMAT);
        // }
        os_args << i; 
        os << ends; 
        os_args << ends;      // append \0
          
        Tcl_AppendElement(Local_Interp, L"debug_trace`add"); 
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os.str()));
        os.rdbuf()->freeze(0);
        Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os_args.str()));
        os_args.rdbuf()->freeze(0);
        Generic args (info.GetField (2));
        if (args.IsNil()) {
          Tcl_AppendElement(Local_Interp, L"0");
        }
        else {
          Tcl_AppendElement(Local_Interp, L"1");
        }
        
        // Is it a debug line?
        bool isDebugLine = Bool(info.GetField(3)).GetValue();
        if (isDebugLine) {
           Tcl_AppendElement(Local_Interp, L"1");
        }
        else {
          Tcl_AppendElement(Local_Interp, L"0");
        }
        
        exeTclCommand(Local_Interp->result);
      }          
      Tcl_ResetResult(Local_Interp);
      break;
    }
  case TAG_TYPE_ProjectTypes_BtGotoLevel:
    {
      Tcl_AppendElement(Local_Interp, L"debug_trace`gotoUpDownLevel");

      int num = ExtractBtGotoLevel(cmd);
      wchar_t buf[10];
      sprintf(buf,L"%d",num);

      Tcl_AppendElement(Local_Interp, buf);
      exeTclCommand(Local_Interp->result);
      break; 
    }

  case TAG_TYPE_ProjectTypes_DependInfo:
    {
      Record asname = cmd.GetField(vdm_ProjectTypes::pos_DependInfo_name);
      wstring b = ExtractModuleName(asname);
        
      Set supers(cmd.GetField(vdm_ProjectTypes::pos_DependInfo_supers));
      wstring namesuper;
      Generic modname;
      for (int i = supers.First(modname); i; i = supers.Next(modname)) {
        namesuper += ExtractModuleName(modname);
        namesuper += L" ";
      }

      Set subs(cmd.GetField(vdm_ProjectTypes::pos_DependInfo_subs));
      wstring namesub;
      for (i = subs.First(modname); i; i = subs.Next(modname)) {
        namesub += ExtractModuleName(modname);
        namesub += L" ";
      };

      Set uses(cmd.GetField(vdm_ProjectTypes::pos_DependInfo_uses));
      wstring nameuses;
      for (i = uses.First(modname); i; i = uses.Next(modname)) {
        nameuses += ExtractModuleName(modname);
        nameuses += L" ";
      }

      Set used(cmd.GetField(vdm_ProjectTypes::pos_DependInfo_used));
      wstring nameused;
      for (i = used.First(modname); i; i = used.Next(modname)) {
        nameused += ExtractModuleName(modname);
        nameused += L" ";
      }

      wstring compl;
      if (Bool(cmd.GetField(vdm_ProjectTypes::pos_DependInfo_compl)).GetValue())
        compl = L"1";
      else
        compl = L"0";
        
      Tcl_AppendElement(Local_Interp, L"UpdateDependInfo");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(b.c_str()));
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(namesuper.c_str()));
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(namesub.c_str()));
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(nameuses.c_str()));
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(nameused.c_str()));
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(compl.c_str()));
        
      exeTclCommand(Local_Interp->result);
      break; 
    }
  case TAG_TYPE_ProjectTypes_DependUpdate:
    {
      Set nameset(cmd.GetField(vdm_ProjectTypes::pos_DependUpdate_names));
      wstring names;
      Generic modname;
      for (int i = nameset.First(modname); i; i = nameset.Next(modname)) {
        names += ExtractModuleName(modname);
        names += L" ";
      }

      Tcl_AppendElement(Local_Interp, L"UpdateDepends");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(names.c_str()));
        
      exeTclCommand(Local_Interp->result);
      break; 
    }
  case TAG_TYPE_ProjectTypes_DrawInhTree:
    {
      Sequence graph(cmd.GetField(vdm_ProjectTypes::pos_DrawInhTree_graph));
      Generic g;
      Tcl_AppendElement(Local_Interp, L"inhtree`clearWindows");
      exeTclCommand(Local_Interp->result);
      for (int bb=graph.First(g); bb; bb = graph.Next(g)) {
        Tcl_ResetResult(Local_Interp);
        ostrstream os1, os2, os3, os4;
        if (Record (g).Is(TAG_TYPE_ProjectTypes_Node)) {
          Record node(g);
          Int x(node.GetField(vdm_ProjectTypes::pos_Node_x));
          Int y(node.GetField(vdm_ProjectTypes::pos_Node_y));
          Token nm(node.GetField(vdm_ProjectTypes::pos_Node_name));
          Tcl_AppendElement(Local_Interp, L"PutCanvasNode");
          os1 << x.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os1.str()));
          os1.rdbuf()->freeze(0);
          os2 << y.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os2.str()));
          os2.rdbuf()->freeze(0);
          os3 << nm.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os3.str()));
          os3.rdbuf()->freeze(0);
        }
        else {
          Record line(g);
          Int x1(Tuple (line.GetField(vdm_ProjectTypes::pos_Line_pos1)).GetField(1));
          Int x2(Tuple (line.GetField(vdm_ProjectTypes::pos_Line_pos1)).GetField(2));
          Int y1(Tuple (line.GetField(vdm_ProjectTypes::pos_Line_pos2)).GetField(1));
          Int y2(Tuple (line.GetField(vdm_ProjectTypes::pos_Line_pos2)).GetField(2));
          Tcl_AppendElement(Local_Interp, L"PutCanvasEdge");
          os1 << x1.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os1.str()));
          os1.rdbuf()->freeze(0);
          os2 << x2.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os2.str()));
          os2.rdbuf()->freeze(0);
          os3 << y1.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os3.str()));
          os3.rdbuf()->freeze(0);
          os4 << y2.GetValue() << ends;
          Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(os4.str()));
          os4.rdbuf()->freeze(0);
        }
        exeTclCommand(Local_Interp->result);
      }
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, L"inhtree`markAllSelectedModules");
      exeTclCommand(Local_Interp->result);
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, L"inhtree`resizeWindow");
      exeTclCommand(Local_Interp->result);
      break;
    }
  case TAG_TYPE_ProjectTypes_ClearInhTree:
    {
      Tcl_AppendElement(Local_Interp, L"inhtree`clear");
      exeTclCommand(Local_Interp->result);
      break; 
    }
  case TAG_TYPE_ProjectTypes_ClearDebugWindow:
    {
      Tcl_AppendElement(Local_Interp, L"debug_editor");
      Tcl_AppendElement(Local_Interp, L"reset");
      exeTclCommand(Local_Interp->result);
      break; 
    }
  case TAG_TYPE_ProjectTypes_UpdateSelections:
    {
      Tcl_AppendElement(Local_Interp, L"modules`selectFunctions");
      exeTclCommand(Local_Interp->result);
      break; 
    }
  }
}
  
int
tkif_getFiles(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_getFiles: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Set files;
  files = RepCast(toolkit)->vdm_Files();
  Generic fn;
  bool more; 
  for (more = files.First(fn); more; more = files.Next(fn)) {
    wstring s = ExtractFileName(fn); 
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()) );
  }
  return TCL_OK; 
}

int
tkif_getModules(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_getModules filename        # return modules in file
  // tkif_getModules                 # return all modules.
  
  if (argc != 1 && argc != 2) {
    Tcl_AppendResult(interp,L"tkif_getModules: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Set modules; 
  if (argc == 1) {              // All modules.
    modules = RepCast(toolkit)->vdm_Modules();
  } else {
    modules = RepCast(toolkit)->vdm_ModulesInFile((Generic) mk_FileName(argv[1]));  // LTO cast ???
  }
  Generic fn;
  bool more; 
  for (more = modules.First(fn); more; more = modules.Next(fn)) {
    wstring s = ExtractFileName(fn); 
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()) );
  }
  return TCL_OK; 
}

int
tkif_syntaxCheck(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_syntaxCheck filename-list logprocedure       # syntax check file
  
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_syntaxCheck: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  Sequence s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.ImpAppend(mk_FileName(list_argv[i]));
  }
  Tcl_Free((wchar_t*)list_argv);
  BTCast(toolkit)->vdm_SyntaxCheck ((Generic) s);  // LTO cast ???
  return TCL_OK; 
}

int
tkif_typeCheck(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_typeCheck module logprocedure       # type check module
  
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_typeCheck: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  Sequence s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.ImpAppend(mk_ModuleName(list_argv[i])); 
  }
  Tcl_Free((wchar_t*)list_argv);
  BTCast(toolkit)->vdm_TypeCheck((Generic) s);  // LTO cast ???
  return TCL_OK; 
}

int
tkif_codeGenerate(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_CodeGenerate module logprocedure       # cg module
  
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_CodeGenerate: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  Sequence s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.ImpAppend(mk_ModuleName(list_argv[i])); 
  }
  Tcl_Free((wchar_t*)list_argv);
   
  Generic m = (Quote) L"CPP";
  BTCast(toolkit)->vdm_CodeGenerate((Generic)s,m,false,true, false, false,Nil(),false,Bool(Setting_CG_CHECKPREPOST)); // LTO cast ???
  
 return TCL_OK; 
}

int
tkif_javaCodeGenerate(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_javaCodeGenerate module logprocedure       
  
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_javaCodeGenerate: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  Sequence s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.ImpAppend(mk_ModuleName(list_argv[i])); 
  }
  Tcl_Free((wchar_t*)list_argv);

  Generic m = (Quote) L"JAVA";
  BTCast(toolkit)->vdm_CodeGenerate((Generic)s, m,
                                    Bool(Setting_JCG_SKEL), 
                                    Bool(Setting_JCG_GENPREPOST), 
                                    Bool(Setting_JCG_TYPES),
                                    Bool(Setting_JCG_SMALLTYPES),
                                    Setting_JCG_PACKAGE,
                                    Bool(Setting_JCG_CONCUR),
                                    Bool(Setting_JCG_CHECKPREPOST));
  
 return TCL_OK; 
}

int
tkif_prettyPrint(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_prettyPrint module-list logprocedure
  
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_prettyPrint: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  Sequence s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.ImpAppend(mk_FileName(list_argv[i])); 
  }
  Tcl_Free((wchar_t*)list_argv);
  BTCast(toolkit)->vdm_PrettyPrint((Generic) s);  // LTO cast ???
  return TCL_OK; 
}

int
tkif_chdir(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // chdir dir
  
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_chdir: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  int res;
  res = chdir(argv[1]);
  Tcl_AppendResult(interp, res==0 ? L"0" : "-1", (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_getFilesOfModule(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_getFilesOfModule module        # 
  
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_getFilesOfModule: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Set files = RepCast(toolkit)->vdm_FileOfModule((Generic)mk_ModuleName(argv[1])); // LTO cast ???
  bool more;
  Generic fn; 
  for (more = files.First(fn); more; more = files.Next(fn)) {
    wstring s = ExtractFileName(fn);
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()));
  }
  return TCL_OK; 
}

static wchar_t * StatusEnumToString(status_type::status s)
{
  switch (s) {
  case status_type::status_none  :
    return L"none";
  case status_type::status_error :
    return L"error";
  case status_type::status_ok :
  case status_type::status_pos :
  case status_type::status_def :
    return L"ok";
  default:
    return L"unknown"; 
  }
}

int
tkif_getStatus(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // tkif_getStatus module        # get status of module
  
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_getStatus: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  status_type st =
    ExtractStatus(RepCast(toolkit)->vdm_Status (mk_ModuleName(argv[1])));
  Tcl_AppendElement(interp, StatusEnumToString(st.syntax)); 
  Tcl_AppendElement(interp, StatusEnumToString(st.type)); 
  Tcl_AppendElement(interp, StatusEnumToString(st.cg)); 
  Tcl_AppendElement(interp, StatusEnumToString(st.jcg)); 
  Tcl_AppendElement(interp, StatusEnumToString(st.pp)); 
  return TCL_OK; 
}

int
tkif_addFiles(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_addFiles: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  if_flush_buffers(); 
  Tcl_ResetResult(interp);

  Set s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.Insert(mk_FileName(list_argv[i]));
  }
  Tcl_Free((wchar_t*)list_argv);
  RepCast(toolkit)->vdm_AddFiles((Generic) s); 
  return TCL_OK; 
}

int
tkif_removeFiles(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_removeFile: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  if_flush_buffers(); 
  Tcl_ResetResult(interp);

  Set s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.Insert(mk_FileName(list_argv[i]));
  }
  Tcl_Free((wchar_t*)list_argv);
  RepCast(toolkit)->vdm_RemoveFiles((Generic) s); 
  return TCL_OK; 
}

////////////////////////////////////////////////////////////////////////
// allowed Syntax, Typecheck, Codegen, Prettyprint, UpdateDepends, DrawInhTree
////////////////////////////////////////////////////////////////////////

int
tkif_allowedSyntax(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_allowedSyntax: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Tuple al = RepCast(toolkit)->vdm_Allowed(mk_FileName(argv[1]), action_syntax); 
  Bool b = al.GetField (1);
  Tcl_AppendResult(interp, b.GetValue() ? L"1" : "0", (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_allowedTypecheck(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_allowedTypecheck: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Tuple al = RepCast(toolkit)->vdm_Allowed(mk_ModuleName(argv[1]), action_type);
  Bool b = al.GetField (1);
  Tcl_AppendResult(interp, b.GetValue() ? L"1" : "0", (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_allowedPrettyprint(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_allowedTypecheck: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Tuple al = RepCast(toolkit)->vdm_Allowed(mk_FileName(argv[1]), action_pp);
  Bool b = al.GetField (1);
  Tcl_AppendResult(interp, b.GetValue() ? L"1" : "0", (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_allowedCodegen(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_allowedTypecheck: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  // Code generating is allowed if type checking is allowed.
  // The toolbox will call the POS type checker if the module is not
  // POS type checked
  Tuple al = RepCast(toolkit)->vdm_Allowed(mk_ModuleName(argv[1]), action_type);
  Bool b = al.GetField (1);
  Tcl_AppendResult(interp, b.GetValue() ? L"1" : "0", (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_funcTrace(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_funcTrace: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  int index;
  Tcl_GetInt(interp,argv[1],&index);
  ostrstream os,os_args;
  Sequence loc_bt_seq (InfCast (toolkit)->vdm_GetBtSeq());
  Tuple info (loc_bt_seq[index]);

  Sequence nameS (info.GetField (1));
  wstring name;
  nameS.GetString(name);

  // Actually, the list contains tuples where the second element is either
  // a sequence or Nil. However, since this function should not be called
  // with an index to such a tuple, I can assume that it is a sequence.
  // Is this invariant broken, then it will be reported as L"Nil casted to sequence".
  Sequence args_lv (info.GetField (2));
  os << L"#" << index-1 << ": "
     << name
     << ends;
  
  int maxj = args_lv.Length ();
  for (int j = 1; j <= maxj; j++) {
    if (j > 1)
      os_args << L", ";
    val2stream( Record(args_lv[j]), os_args, Setting_PRINT_FORMAT);
  }
  os_args << ends;      // append \0
  
  Tcl_SetResult(interp, CONST_CAST(wchar_t*)(os_args.str()), TCL_VOLATILE);
  os_args.rdbuf()->freeze(0);
  return TCL_OK; 
}

int
tkif_GetRealFileName(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_GetRealFileName: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Record realname;
  realname = RepCast (toolkit)->vdm_GetRealFileName ((Generic) mk_FileName (argv[1]));
  wstring rn = ExtractFileName (realname);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(rn.c_str()),(wchar_t*) NULL);
  return TCL_OK;
}

int
tkif_resetInterfaces(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,
                     L"tkif_resetInterfaces: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  BTCast (toolkit)->vdm_ResetInterfaces();
  return TCL_OK; 
}

int
tkif_GetPossibleInterfaces(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,
                     L"tkif_resetInterfaces: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Set interfaces = BTCast (toolkit)->vdm_GetPossibleInterfaces();
  Generic cl;
  bool more; 
  for (more =interfaces.First(cl); more; more = interfaces.Next(cl)) {
    wstring s = ExtractModuleName(cl); 
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()) );
  }
  return TCL_OK; 
}

// This function is used to regenerate the set of classes which could
// possibly be generated as an interface, when a particular class has 
// been selected or deselected as an interface. 
int
tkif_GetActivatedInterfaces(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,
                     L"tkif_GetActivatedInterfaces: wrong number of parameters",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Set s;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    s.Insert(mk_ModuleName(list_argv[i]));
  }
  Tcl_Free((wchar_t*)list_argv);

  Tcl_ResetResult(interp);
  Set origSetting_JCG_INTERFACES (Setting_JCG_INTERFACES);
  Setting_JCG_INTERFACES = s;
  Set interfaces = BTCast (toolkit)->vdm_GetPossibleInterfaces();
  Setting_JCG_INTERFACES = origSetting_JCG_INTERFACES;
  Generic cl;
  bool more; 
  for (more =interfaces.First(cl); more; more = interfaces.Next(cl)) {
    wstring s = ExtractModuleName(cl); 
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()) );
  }
  return TCL_OK; 
}

int
tkif_updateDepends(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_updateDepends: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Bool b = BTCast (toolkit)->vdm_ClassDepend((Generic) mk_ModuleName(argv[1]));
  return TCL_OK; 
}

int
tkif_compDepend(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_compDepend: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  BTCast (toolkit)->vdm_Depend ();
  return TCL_OK; 
}

int 
tkif_drawInhTree(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_drawInhTree: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Bool b = BTCast (toolkit)->vdm_InhTree();
  return TCL_OK; 
}


int
ConstructGuiMap(Map &gui_settings, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // Construct the gui-settings map
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) {
    wchar_t **elem;
    int n;
    code = Tcl_SplitList(interp, list_argv[i], &n, &elem);
    if (code != TCL_OK)
      return code;
    if(n != 2){
      Tcl_AppendResult(interp,L"ConstructGuiMap: wrong number of parameters.",
                       (wchar_t *)NULL);
      return TCL_ERROR;
    }
    Tuple t(2);
    int val = atoi(elem[1]);
    t.SetField(1, Bool(val/2));
    t.SetField(2, Bool(val%2));
    gui_settings.Insert(Sequence(wstring(elem[0])), t);
    Tcl_Free((wchar_t*)elem);
  }
  Tcl_Free((wchar_t*)list_argv);
  return TCL_OK;
}


int
tkif_Conflicts(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_Conflicts: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  Tcl_ResetResult(interp);
  Map gui_settings;
  int code;
  if( (code = ConstructGuiMap(gui_settings, interp, argc, argv)) != TCL_OK )
    return code;
  wstring s;
  if(UMLCast(toolkit)->vdm_Conflicts((Generic)gui_settings).GetValue())
    s = L"1";
  else
    s = L"0";
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
  return TCL_OK; 
}


int
tkif_SetMapperState(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_SetMapperState: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  wstring s;
  if(UMLCast(toolkit)->vdm_SetMapperState(Int(atoi(argv[1]))).GetValue())
    s = L"1";
  else
    s = L"0";
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
  
  return TCL_OK; 
}


int
tkif_ResetMapper(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_ResetMapper: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  UMLCast(toolkit)->vdm_ResetMapper();
  return TCL_OK; 
}



int
tkif_MapUML(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_MapUML: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  Tcl_ResetResult(interp);
  Map gui_settings;
  int code;
  if( (code = ConstructGuiMap(gui_settings, interp, argc, argv)) != TCL_OK )
    return code;

  UMLCast(toolkit)->vdm_MapUML((Generic)gui_settings);
  return TCL_OK; 
}


int
tkif_SetModelName(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_SetModelName: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  UMLCast(toolkit)->vdm_SetModelName((Generic) Sequence(wstring(argv[1])));

  return TCL_OK; 
}


int
tkif_GetModelName(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_GetModelName: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  wstring s = Seq2Str(UMLCast(toolkit)->vdm_GetModelName());
  Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()) );

  return TCL_OK; 
}


int
tkif_GetClassStatus(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_GetClassStatus: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Map stat = UMLCast(toolkit)->vdm_GetClassStatus();
  Map defaults = UMLCast(toolkit)->vdm_GetDefaultSettings((Generic)stat);

  wstring *arg_list = new wstring[stat.Dom().Card()];
  // Build list of arguments
  Generic fn;
  bool more; 
  int i=0;
  for (more = stat.First(fn); more; more = stat.Next(fn)) {
    Tcl_ResetResult(interp);
    // Class name
    wstring s = Seq2Str(fn);
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(s.c_str()) );
    wstring vpp,uml;
    Tuple t(stat[fn]);
    vpp = Seq2Str(Record(t.GetField(1)).GetField(1));
    uml = Seq2Str(Record(t.GetField(2)).GetField(1));
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(vpp.c_str()) );
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(uml.c_str()) );
    Tuple d(defaults[fn]);
    Bool vp(d.GetField(1)), um(d.GetField(2));
    Int st( 2 * vp.GetValue() + um.GetValue() );
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(st.ascii().c_str()) );
    arg_list[i++] = wstring(interp->result);
  } 
  Tcl_ResetResult(interp);
  for(int j = 0; j<i; j++)
    Tcl_AppendElement(interp, CONST_CAST(wchar_t*)(arg_list[j].c_str()) );

  delete[] arg_list;
  return TCL_OK; 
}


int
tkif_UML_Diff(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_UML_Diff: wrong number of parameters.",
                     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  SetLogProc(argv[2]);
  Tcl_ResetResult(interp);
  Set nm_s;
  int list_argc, code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, argv[1], &list_argc, &list_argv);
  if (code != TCL_OK)
    return code;
  for (int i=0; i<list_argc; i++) 
    nm_s.Insert(Sequence(wstring(list_argv[i])));
  Tcl_Free((wchar_t*)list_argv);
  UMLCast(toolkit)->vdm_Diff((Generic)nm_s);
  return TCL_OK; 
}


//////////////////////////////////////////////////////////////////////
// Project handling
//////////////////////////////////////////////////////////////////////

int
tkif_CleanUp(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_CleanUp: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  if_flush_buffers(); 
  Tcl_ResetResult(interp);

#if defined(USEUML) && defined(_MSC_VER)
  if (UML) 
    delete UML;
#endif //#if defined(USEUML) && defined(_MSC_VER)


  CleanUp();
  return TCL_OK; 
}

int
tkif_NewUnnamedProject(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_NewUnnamedProject: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  RepCast(toolkit)->vdm_NewUnnamedProject();
  return TCL_OK; 
}

int
tkif_StateIsSaved(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_StateIsSaved: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  Bool b = RepCast(toolkit)->vdm_StateIsSaved();
  Tcl_AppendResult(interp, (b.GetValue()) ? L"1" : "0", (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_OpenProject(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_OpenProject: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  if_flush_buffers(); 
  Tcl_ResetResult(interp);
  RepCast(toolkit)->vdm_Open((Generic) mk_FileName(argv[1])); // LTO cast ???
  return TCL_OK; 
}

int
tkif_GetProjectName(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_GetProjectName: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Generic g; 
  g = RepCast(toolkit)->vdm_GetProjectName();
  if (g.IsNil()) {
    Tcl_ResetResult(interp);
  } else {
    Tcl_ResetResult(interp);
    wstring s = ExtractFileName(g);
    Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), NULL);    
  }
    
  return TCL_OK; 
}

int
tkif_SaveAs(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_SaveAs: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Generic g; 
  RepCast(toolkit)->vdm_SaveAs((Generic) mk_FileName(argv[1]));
  Tcl_ResetResult(interp);
  return TCL_OK; 
}

int
tkif_Getcwd(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_Getcwd: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Generic g;
  wstring s;
  tb_Getcwd(s);
  
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), NULL);    
    
  return TCL_OK; 
}

int
tkif_GetBasedir(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_GetBasedir: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  wstring s = tb_getbasedir(wstring(argv[1])); 
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), NULL);    
    
  return TCL_OK; 
}

int
tkif_GetBasename(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_GetBasename: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  wstring s = tb_getbasename(wstring(argv[1])); 
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), NULL);    
    
  return TCL_OK; 
}

int
tkif_tmpBreakFileName(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{

  wstring dir;
#ifdef _MSC_VER
  wchar_t *tmpdir = getenv(L"TMP");
  if (tmpdir == 0) {
    dir = L"c:\\";
  }
  else {
    dir = tmpdir;
  }
  dir += L"\\vdm.bkp";
#else
  dir = L"~/.vdm-breakpoint";
#endif
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(dir.c_str()), NULL);    
    
  return TCL_OK; 
}

int
tkif_JoinPaths(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_JoinPaths: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  wstring s = tb_JoinPaths(argv[1], argv[2]);
  
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), NULL);    
    
  return TCL_OK; 
}

static wstring EditorName;       // For multiple editor/displayers 

int
tkif_SetEditorName(ClientData /*clientData*/,
                   Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_SetEditorName: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  EditorName = argv[1]; 
  return TCL_OK; 
}

int
tkif_GiveToolVersion(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_GiveToolVersion: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  wstring s = tb_version.GiveToolVersionDate();
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_GiveToolTitle(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_GiveToolTitle: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  wstring s = tb_version.GiveToolTitle();
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
  return TCL_OK; 
}

int
tkif_IsVdmslType(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_IsVdmslType: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  TB_Version::TB_Type tb_type = tb_version.GetToolType();
  wstring s;
  if (tb_type == TB_Version::vdmsl)
    s = L"1";
  else
    s = L"0";
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
   return TCL_OK; 
}

int
tkif_IsVdmslLiteType(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_IsVdmslLiteType: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  TB_Version::TB_Type tb_type = tb_version.GetToolType();
  wstring s;
  if (tb_type == TB_Version::vdmsllite)
    s = L"1";
  else
    s = L"0";
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
   return TCL_OK; 
}

int
tkif_IsVdmppType(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_IsVdmppType: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  TB_Version::TB_Type tb_type = tb_version.GetToolType();
  wstring s;
  if ((tb_type == TB_Version::vdmpp) || (tb_type == TB_Version::vice))
    s = L"1";
  else
    s = L"0";
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
   return TCL_OK; 
}

int
tkif_IsUmlType(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  wstring s;
#if defined(USEUML) && defined(_MSC_VER)
  s = L"1";
#else
  s = L"0";
#endif //#if defined(USEUML) && defined(_MSC_VER)
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(s.c_str()), (wchar_t*)NULL); 
  return TCL_OK; 
}

extern void api_Periodic();

int
tkif_Periodic(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_Periodic: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  api_Periodic();
  return TCL_OK; 
}

int Get_Value(Tcl_Interp * interp, const wchar_t*mapname, const wchar_t*key)
{
  wchar_t *value;
  value = Tcl_GetVar2(interp,
                      CONST_CAST(wchar_t*)(mapname),
                      CONST_CAST(wchar_t*)(key), TCL_GLOBAL_ONLY);
  if (!value) {
    cerr << L"Get_Value: " << mapname << "(" << key << ")\n";
    return 0;
  }
  return atoi(value); 
}

Set Get_List_Value(Tcl_Interp * interp, const wchar_t*mapname, const wchar_t*key)
{
  wchar_t *setwstring = Tcl_GetVar2(interp,
                      CONST_CAST(wchar_t*)(mapname),
                      CONST_CAST(wchar_t*)(key), TCL_GLOBAL_ONLY);
  if (!setwstring) {
    cerr << L"Get_List_Value: " << mapname << "(" << key << ")\n";
    return Set();
  }

  Set value;
  int list_argc;
  int code;
  wchar_t **list_argv;
  code = Tcl_SplitList(interp, setwstring, &list_argc, &list_argv);
  if (code != TCL_OK){
    cerr << L"Get_List_Value: " << mapname << "(" << key << ")\n";
    return Set();
  }

  for (int i=0; i<list_argc; i++) {
    value.Insert(mk_ModuleName(list_argv[i]));
  }
  Tcl_Free((wchar_t*)list_argv);
  return value;
}

  

wstring Get_String_Value(Tcl_Interp * interp, const wchar_t*mapname, const wchar_t*key)
{
  wchar_t *value;
  value = Tcl_GetVar2(interp,
                      CONST_CAST(wchar_t*)(mapname),
                      CONST_CAST(wchar_t*)(key), TCL_GLOBAL_ONLY);
  if (!value) {
    cerr << L"Get_Value: " << mapname << "(" << key << ")\n";
    return 0;
  }
  return wstring(value); 
}

void Set_Map_String_Value(Tcl_Interp * interp, const wchar_t*mapname, const wchar_t*key, const wstring& value)
{
  wchar_t *val = Tcl_SetVar2(interp,
                          CONST_CAST(wchar_t*)(mapname),
                          CONST_CAST(wchar_t*)(key),
                          CONST_CAST(wchar_t*)(value.c_str()),
                          TCL_GLOBAL_ONLY);
  if (val == 0) {
    cerr
      << L"Set_Value: " << mapname << "(" << key << ") = " << value
      <<  L"\n";
  }
}

void Set_Map_List_Value(Tcl_Interp * interp, const wchar_t*mapname, const wchar_t*key, const Set& value)
{

  bool b;
  Generic g;
  Tcl_ResetResult(Local_Interp);
  for (b = value.First(g); b; b=value.Next(g)) {
    wstring s = ExtractModuleName(g);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()));
  }
  wstring tcl_value (Local_Interp->result);
  Set_Map_String_Value(interp, mapname, key, tcl_value.c_str());
}

void Set_Map_Value(Tcl_Interp * interp, const wchar_t*mapname, const wchar_t*key, int value)
{
  wchar_t charvalue[100];
  sprintf(charvalue, L"%d", value); 
  Set_Map_String_Value(interp, mapname, key, wstring(charvalue)); 
}

int
tkif_Options(ClientData /*clientData*/,
             Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_Options: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  wchar_t * mapname = argv[2]; 
  const wchar_t * NAME_DTC = L"dtc"; 
  const wchar_t * NAME_PRE = L"pre"; 
  const wchar_t * NAME_POST = L"post"; 
  const wchar_t * NAME_INV = L"inv"; 
  const wchar_t * NAME_SEP = L"sep"; 
  const wchar_t * NAME_DEF = L"def"; 
  const wchar_t * NAME_POS = L"pos"; 
  const wchar_t * NAME_FULL = L"full"; 
  const wchar_t * NAME_CG_RTI = L"cg_rti";  //?? 
#ifdef VDMPP
  const wchar_t * NAME_PRIORITY = L"priority";
  const wchar_t * NAME_MAXINSTR = L"maxinstr";
  const wchar_t * NAME_PRIMARYALGORITHM = L"primaryalgorithm";
#endif //VDMPP
  const wchar_t * NAME_CG_CHECKPREPOST = L"cg_checkprepost";  //?? 
  const wchar_t * NAME_INDEX_DEFS = L"index_defs"; 
  const wchar_t * NAME_INDEX_USES = L"index_uses";
  const wchar_t * NAME_PRETTYPRINT_RTI = L"prettyprint_rti";
  const wchar_t * NAME_SEED = L"seed";
  const wchar_t * NAME_PPR = L"ppr";
  const wchar_t * NAME_JCG_SKEL = L"jcg_skel";
  const wchar_t * NAME_JCG_GENPREPOST = L"jcg_genprepost";
  const wchar_t * NAME_JCG_TYPES = L"jcg_types";
  //  const wchar_t * NAME_JCG_SMALLTYPES = L"jcg_smalltypes";
  const wchar_t * NAME_JCG_LONGS = L"jcg_longs";
  const wchar_t * NAME_JCG_PACKAGE = L"jcg_package";
  const wchar_t * NAME_JCG_CONCUR = L"jcg_concur";
  const wchar_t * NAME_JCG_CHECKPREPOST = L"jcg_checkprepost";
  const wchar_t * NAME_JCG_INTERFACES = L"jcg_interfaces";

  
  if (strcmp(argv[1], L"set") == 0) {
    Setting_DTC = Get_Value(interp, mapname, NAME_DTC);
    Setting_PRE = Get_Value(interp, mapname, NAME_PRE);
    Setting_POST = Get_Value(interp, mapname, NAME_POST);
    Setting_INV = Get_Value(interp, mapname, NAME_INV);
    Setting_SEP = Get_Value(interp, mapname, NAME_SEP);
    int def     = Get_Value(interp, mapname, NAME_DEF);
    Setting_DEF = def ? DEF : POS; // pos variable is == !def
    Setting_errlevel = Get_Value(interp, mapname, NAME_FULL) ? PRF : WRN1;     
    int defs    = Get_Value(interp, mapname, NAME_INDEX_DEFS);
    int uses    = Get_Value(interp, mapname, NAME_INDEX_USES);
    Setting_INDEX = defs ? 1 : uses ? 2 : 0; 
#ifdef VDMPP
    Setting_PRIORITY = Get_Value(interp, mapname, NAME_PRIORITY);
    Setting_MAXINSTR = Get_Value(interp, mapname, NAME_MAXINSTR);
    wstring alg_wstring = Get_String_Value(interp, mapname, 
                                         NAME_PRIMARYALGORITHM);
    if (alg_wstring == L"pc")
      Setting_PRIMARYALGORITHM=TAG_TYPE_SCHDTP_PureCooperative;
    else if (alg_wstring == L"ins")
      Setting_PRIMARYALGORITHM=TAG_TYPE_SCHDTP_InstrnumSlice;
    else {
      // Setting_PRIMARYALGORITHM is unchanged;
    }
#endif //VDMPP
    Setting_CG_RTI = Get_Value(interp, mapname, NAME_CG_RTI);
    Setting_CG_CHECKPREPOST = Get_Value(interp, mapname, NAME_CG_CHECKPREPOST);
    Setting_PrettyPrint_RTI = Get_Value(interp, mapname, NAME_PRETTYPRINT_RTI);
    Seed_nondetstmt = Get_Value(interp, mapname, NAME_SEED);
    Setting_PRINT_FORMAT = Get_Value(interp, mapname, NAME_PPR) ? 0 : -1; 
    Setting_JCG_SKEL = Get_Value(interp, mapname, NAME_JCG_SKEL);
    Setting_JCG_GENPREPOST = Get_Value(interp, mapname, NAME_JCG_GENPREPOST);
    Setting_JCG_TYPES = Get_Value(interp, mapname, NAME_JCG_TYPES);
    //    Setting_JCG_SMALLTYPES = Get_Value(interp, mapname, NAME_JCG_SMALLTYPES);
    Setting_JCG_LONGS = Get_Value(interp, mapname, NAME_JCG_LONGS);
    wstring packageStr = Get_String_Value(interp, mapname, NAME_JCG_PACKAGE);
    if (packageStr == L"")
      Setting_JCG_PACKAGE = Nil();
    else
      Setting_JCG_PACKAGE = Token(packageStr);
    Setting_JCG_CONCUR = Get_Value(interp, mapname, NAME_JCG_CONCUR);
    Setting_JCG_CHECKPREPOST = Get_Value(interp, mapname, 
                                         NAME_JCG_CHECKPREPOST);
    Setting_JCG_INTERFACES = Get_List_Value(interp, mapname, 
                                            NAME_JCG_INTERFACES);
  }
  else if (strcmp(argv[1], L"get") == 0) {
    Set_Map_Value(interp, mapname, NAME_DTC, Setting_DTC); 
    Set_Map_Value(interp, mapname, NAME_PRE , Setting_PRE );
    Set_Map_Value(interp, mapname, NAME_POST, Setting_POST);
    Set_Map_Value(interp, mapname, NAME_INV  , Setting_INV );
    Set_Map_Value(interp, mapname, NAME_SEP  , Setting_SEP );
    int def = Setting_DEF == DEF ? 1 : 0; 
    int pos = !def; 
    Set_Map_Value(interp, mapname, NAME_DEF, def);
    Set_Map_Value(interp, mapname, NAME_POS, pos);
    int full = Setting_errlevel == PRF ? 1 : 0; 
    Set_Map_Value(interp, mapname, NAME_FULL, full);    

    int defs = Setting_INDEX == 1 ? 1 : 0; 
    int uses = Setting_INDEX == 2 ? 1 : 0; 
    Set_Map_Value(interp, mapname, NAME_INDEX_DEFS, defs);
    Set_Map_Value(interp, mapname, NAME_INDEX_USES, uses);
#ifdef VDMPP
    Set_Map_Value(interp, mapname, NAME_PRIORITY, Setting_PRIORITY);
    Set_Map_Value(interp, mapname, NAME_MAXINSTR, Setting_MAXINSTR);
    wstring alg_wstring;
    if (Setting_PRIMARYALGORITHM == TAG_TYPE_SCHDTP_PureCooperative)
      alg_wstring = L"pc";
    else // Setting_PRIMARYALGORITHM == TAG_TYPE_SCHDTP_InstrnumSlice
      alg_wstring = L"ins";
    Set_Map_String_Value(interp, mapname, NAME_PRIMARYALGORITHM, alg_wstring);
#endif //VDMPP
    Set_Map_Value(interp, mapname, NAME_CG_RTI, Setting_CG_RTI);
    Set_Map_Value(interp, mapname, NAME_CG_CHECKPREPOST, Setting_CG_CHECKPREPOST);
    Set_Map_Value(interp, mapname, NAME_PRETTYPRINT_RTI, Setting_PrettyPrint_RTI);
    Set_Map_Value(interp, mapname, NAME_SEED, Seed_nondetstmt);
    int ppr = Setting_PRINT_FORMAT == -1 ? 0 : 1;
    Set_Map_Value(interp, mapname, NAME_PPR, ppr); 

    Set_Map_Value(interp, mapname, NAME_JCG_SKEL, Setting_JCG_SKEL);
    Set_Map_Value(interp, mapname, NAME_JCG_GENPREPOST, 
                  Setting_JCG_GENPREPOST);
    Set_Map_Value(interp, mapname, NAME_JCG_TYPES, Setting_JCG_TYPES);
    //    Set_Map_Value(interp, mapname, NAME_JCG_SMALLTYPES, 
    //                  Setting_JCG_SMALLTYPES);
    Set_Map_Value(interp, mapname, NAME_JCG_LONGS, Setting_JCG_LONGS);
    Set_Map_Value(interp, mapname, NAME_JCG_CONCUR, Setting_JCG_CONCUR);
    Set_Map_Value(interp, mapname, NAME_JCG_CHECKPREPOST, 
                  Setting_JCG_CHECKPREPOST);
    wstring packageStr;
    if (Setting_JCG_PACKAGE == Nil())
      packageStr = L"";
    else
      Sequence(Setting_JCG_PACKAGE).GetString(packageStr);
    Set_Map_String_Value(interp, mapname, NAME_JCG_PACKAGE, packageStr);
    Set_Map_List_Value(interp, mapname, NAME_JCG_INTERFACES, Setting_JCG_INTERFACES);
  }
  return TCL_OK; 
}

int
tkif_getwin(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // Get window at given point: set w [ tkif_getwin $x $y ]
  if (argc != 3) {
    Tcl_AppendResult(interp,L"tkif_getwin: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  Tk_Window tkmainwin = Tk_MainWindow(interp); 
  int x, y;
  Tcl_GetInt(interp, argv[1], &x); 
  Tcl_GetInt(interp, argv[2], &y);
  wchar_t *winpath; 
  Tk_Window w = Tk_CoordsToWindow(x, y, tkmainwin); 
  if (w) 
    winpath = Tk_PathName(w); 
  else
    winpath = Tk_PathName(tkmainwin);

  Tcl_AppendResult(interp, winpath, (wchar_t*)NULL);

  return TCL_OK; 
}

int
tkif_IsFileChanged(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t **argv)
{
  // return true if the file is changed since last syntax check
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_IsFileChanged: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  wstring name = argv[1];
  wstring res;
  if (IsFileModified(name)) {
    res =L"1";
  } else {
    res = L"0";
  }
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, CONST_CAST(wchar_t*)(res.c_str()), (wchar_t*)NULL); 
  return TCL_OK; 
}
  


class GUIErrors : public vdm_Errors
{
public:
  bool running_under_emacs;

private:
  int GetMsgPrt()
  {
    return (int) vdm_msgPtr;
  }

public:
  void 
  SetMsgPtr(int i)
  {
    vdm_msgPtr = Int(i);
  }
  
public:
  GUIErrors() { }
  
  virtual void vdm_SetFilePosition(const Record & msg)
  {
    int fileid = Int (msg.GetField (pos_ErrMsg_fid)).GetValue();
    int lineid = Int (msg.GetField (pos_ErrMsg_line)).GetValue();
    int columnid = Int (msg.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);

    if (file.length() > 0) {
      wchar_t fmt[100];
      Record tfn = Repos()->vdm_GetTempFileName ((Generic)mk_FileName (file));
      wstring tmpfn = ExtractFileName (tfn);
      if (tmpfn.length() > 0)
	file = tmpfn;
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(EditorName.c_str()));
      Tcl_AppendElement(Local_Interp, L"goto");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(file.c_str()) );
      sprintf(fmt, L"%d", lineid); 
      Tcl_AppendElement(Local_Interp, fmt);
      sprintf(fmt, L"%d",  columnid - 1); 
      Tcl_AppendElement(Local_Interp, fmt);
      exeTclCommand(Local_Interp->result);   
    }
    else if (fileid != 0) {
      //vdm_log << L"c." << columnid << " in the command-line\n";
    }
  }

  virtual void vdm_ShowMsg() {
    Record msg = vdm_msgs[vdm_msgPtr];
      
    int fileid = Int (msg.GetField (pos_ErrMsg_fid)).GetValue();

    if (fileid == -10)
      return;

    wstring file = GetFileName (fileid);
      
    vdm_SetFilePosition(msg);

    int i = (int) vdm_msgPtr;
      
    wstring s = ExtractMessageAll(file,msg);

    wchar_t fmt[100]; 
    Tcl_ResetResult(Local_Interp);
    Tcl_AppendElement(Local_Interp, L"error_lister");
    Tcl_AppendElement(Local_Interp, L"showMsg");
    sprintf(fmt, L"%d", i);
    Tcl_AppendElement(Local_Interp, fmt);
    exeTclCommand(Local_Interp->result);   
  }

  virtual void vdm_EventAddMsg(const Int &I,const Record &r)
  {
    Tcl_ResetResult(Local_Interp);
    int errnum = (int)I; 

    int fileid = Int (r.GetField (pos_ErrMsg_fid)).GetValue();
    int line = Int (r.GetField (pos_ErrMsg_line)).GetValue();
    int col = Int (r.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);
    wstring s = ExtractMessageAll(file,r);

    wchar_t fmt[100];
    if (fileid != -10) {
      Tcl_AppendElement(Local_Interp, L"error_lister");
      Tcl_AppendElement(Local_Interp, L"AddErr");
    } else {
      Tcl_AppendElement(Local_Interp, L"command`AddErr");
    }
    sprintf(fmt, L"%d", errnum); 
    Tcl_AppendElement(Local_Interp, fmt);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(file.c_str()) );
    sprintf(fmt, L"%d", line); 
    Tcl_AppendElement(Local_Interp, fmt);
    sprintf(fmt, L"%d",  col-1); 
    Tcl_AppendElement(Local_Interp, fmt);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()) );
    exeTclCommand(Local_Interp->result);   
  }

  virtual void vdm_EventClearAll() {
    // cerr << L"ClearAll " << "\n";
    exeTclCommand(L"error_lister clearAll");
  }

  virtual void vdm_EventAllDone() {
    // Record msg = vdm_msgs[vdm_msgPtr];
    exeTclCommand(L"error_lister allDone");
  }

  void vdm_EventNext()
  {
    exeTclCommand(L"error_lister cb`next"); 
  }
  void vdm_EventPrev()
  {
    exeTclCommand(L"error_lister cb`prev"); 
  }
  void vdm_EventFirst()
  {
    exeTclCommand(L"error_lister cb`first"); 
  }
  void vdm_EventLast()
  {
    exeTclCommand(L"error_lister cb`last"); 
  }

  virtual void vdm_MakeStatus(const Int & NoErr, const  Int & NoWarn)
  {
    wchar_t buf1[30], buf2[30];
    sprintf(buf1, L"%d", NoErr.GetValue());
    sprintf(buf2, L"%d", NoWarn.GetValue()); 
    wstring ne = buf1;
    wstring nw = buf2;
    wstring msg;
    if (NoErr>0 || NoWarn>0) {
      if (NoErr>0)
        msg = L"Errors: " + ne + " ";
      if (NoWarn>0)
        msg = msg + L"Warnings: " + nw; 
      msg += L"\n";
    } else {
      //if (!NoErr && !NoWarn)
      //  msg = L"Ok\n";
    }
    if (msg.length() > 0) {
      Tcl_ResetResult(Local_Interp);
      Tcl_AppendElement(Local_Interp, L"error_lister");
      Tcl_AppendElement(Local_Interp, L"addinfo");
      Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(msg.c_str()) );
      exeTclCommand(Local_Interp->result);   
    }
  }
};

class GuiExprErrors : public vdm_Errors
{
public:
  bool running_under_emacs;

private:
  int GetMsgPrt()
  {
    return (int) vdm_msgPtr;
  }

public:
  void 
  SetMsgPtr(int i)
  {
    vdm_msgPtr = Int(i);
  }
  
  GuiExprErrors() { }

public:

  virtual void vdm_EventAddMsg(const Int & I, const Record & r)
  {
    Tcl_ResetResult(Local_Interp);
    int errnum = (int)I; 

    int fileid = Int (r.GetField (pos_ErrMsg_fid)).GetValue();
    int line = Int (r.GetField (pos_ErrMsg_line)).GetValue();
    int col = Int (r.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);
    wstring s = ExtractMessageAll(file,r);

    wchar_t fmt[100];
    Tcl_AppendElement(Local_Interp, L"command`AddErr");
    sprintf(fmt, L"%d", errnum); 
    Tcl_AppendElement(Local_Interp, fmt);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(file.c_str()) );
    sprintf(fmt, L"%d", line); 
    Tcl_AppendElement(Local_Interp, fmt);
    sprintf(fmt, L"%d",  col-1); 
    Tcl_AppendElement(Local_Interp, fmt);
    Tcl_AppendElement(Local_Interp, CONST_CAST(wchar_t*)(s.c_str()) );
    exeTclCommand(Local_Interp->result);   

    Tcl_ResetResult(Local_Interp);
    // cerr << L"AddMsg " << "\n";
  }

  virtual void vdm_EventClearAll() {
  }

  virtual void vdm_EventAllDone() {
  }

  virtual void vdm_SetFilePosition(const Record & msg)
  {}  

  virtual void vdm_ShowMsg() {} 

  virtual void vdm_MakeStatus(const Int &NoErr, const Int & NoWarn)
  {}
  
};

static GUIErrors * errs ;

int
tkif_SetMsgPtr(ClientData /*clientData*/,
               Tcl_Interp *interp, int argc, wchar_t **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp,L"tkif_SetMsgPtr: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  int i;
  int res = Tcl_GetInt(interp, argv[1], &i);
  if (res == TCL_OK) {
    errs->SetMsgPtr(i);
    errs->vdm_ShowMsg(); 
  }
  return res; 
}


int tkif_StopInterpreter(ClientData /*clientData*/,
               Tcl_Interp *interp, int argc, wchar_t **argv) {
  theDebugger().SetBREAK(true);
  return TCL_OK;
}



int
tkVsl_Init(Tcl_Interp *interp)
{
  Local_Interp = Tcl_CreateInterp();

  Tcl_CreateCommand(interp, L"tkif_Periodic", tkif_Periodic,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_OpenProject", tkif_OpenProject,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetProjectName", tkif_GetProjectName,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_SaveAs", tkif_SaveAs,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_Getcwd", tkif_Getcwd,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_tmpBreakFileName", tkif_tmpBreakFileName,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_JoinPaths", tkif_JoinPaths,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetBasedir", tkif_GetBasedir,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetBasename", tkif_GetBasename,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_NewUnnamedProject", tkif_NewUnnamedProject,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_CleanUp", tkif_CleanUp,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_StateIsSaved", tkif_StateIsSaved,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_executeCommand", tkif_executeCommand,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_executeCommandPrompt", tkif_executeCommandPrompt,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_SetMsgPtr", tkif_SetMsgPtr,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_getFiles", tkif_getFiles,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, L"tkif_getModules", tkif_getModules,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_addFiles", tkif_addFiles,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_syntaxCheck", tkif_syntaxCheck,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_typeCheck", tkif_typeCheck,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_codeGenerate", tkif_codeGenerate,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_javaCodeGenerate", tkif_javaCodeGenerate,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_prettyPrint", tkif_prettyPrint,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_updateDepends", tkif_updateDepends,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_compDepend", tkif_compDepend,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_funcTrace", tkif_funcTrace,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_drawInhTree", tkif_drawInhTree,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_chdir", tkif_chdir,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); 
  Tcl_CreateCommand(interp, L"tkif_getStatus", tkif_getStatus,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_getFilesOfModule", tkif_getFilesOfModule,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_allowedSyntax", tkif_allowedSyntax,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_allowedTypecheck", tkif_allowedTypecheck,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_allowedCodegen", tkif_allowedCodegen,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_allowedPrettyprint", tkif_allowedPrettyprint,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_removeFiles", tkif_removeFiles,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GiveToolVersion", tkif_GiveToolVersion,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GiveToolTitle", tkif_GiveToolTitle,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_IsVdmslType", tkif_IsVdmslType,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_IsVdmslLiteType", tkif_IsVdmslLiteType,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_IsVdmppType", tkif_IsVdmppType,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_IsUmlType", tkif_IsUmlType,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_getwin", tkif_getwin,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_printText", tkif_printText, 
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_Options", tkif_Options,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_SetEditorName", tkif_SetEditorName,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); 
  Tcl_CreateCommand(interp, L"tkif_SetMapperState", tkif_SetMapperState,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_Conflicts", tkif_Conflicts,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_ResetMapper", tkif_ResetMapper,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetClassStatus", tkif_GetClassStatus,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_MapUML", tkif_MapUML,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_SetModelName", tkif_SetModelName,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetModelName", tkif_GetModelName,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_UML_Diff", tkif_UML_Diff,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_IsFileChanged", tkif_IsFileChanged,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_StopInterpreter", tkif_StopInterpreter,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetRealFileName", tkif_GetRealFileName,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_resetInterfaces", tkif_resetInterfaces,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetPossibleInterfaces", 
                    tkif_GetPossibleInterfaces,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, L"tkif_GetActivatedInterfaces", 
                    tkif_GetActivatedInterfaces,
		    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);



  // Function found in bitmap_embed.cc. Created by Makefile. 
  extern void tkif_CreateBitmaps(Tcl_Interp*);
  tkif_CreateBitmaps(interp);
  
  // Adding directorys to the search path should NOT
  // be done by the interface. TO BE FURTHER CONSIDERED!
  // get current working directory
  wchar_t *cwd;
  if ((cwd = getcwd ((wchar_t *) NULL, 256)) != NULL) {
    wstring slashcwd (cwd);
    slashcwd += L"/";
    SetDefaultPath (slashcwd);
  }
  lbuf.SetLog (Log_Event);
  ebuf.SetLog (ELog_Event);
  // Set flush mode to output after each << operation.  
  //vdm_err.setf(ios::unitbuf); 
  //vdm_log.setf(ios::unitbuf);

  (void) VDMSetErrorHandler(EXC_ThrowHandler);

  InitModuleTypes();
  
  InitToolbox(false); 
  toolkit = new vdm_ToolKit ();
  vdm_TK_p = new ObjectRef(toolkit);
  vdm_if = new ObjectRef(new TestInterface ());
  errs = new GUIErrors();
  ObjectRef ERRS (errs);

#if defined(USEUML) && defined(_MSC_VER)
  UML = new UMLMapper();
#endif //#if defined(USEUML) && defined(_MSC_VER)
  
  GuiExprErrors *exprerrs = new GuiExprErrors();
  ObjectRef ExprERRS(exprerrs);
  
  {
    wchar_t *emacs; 
    errs->running_under_emacs =
      ((emacs=getenv(L"EMACS")) && strcmp(emacs, "t")==0);
  }

  ObjectRef base_state (new vdm_ErrorState()); // dummy errorst.
  errs->vdm_InitState((Generic) base_state); // LTO cast ???
  exprerrs->vdm_InitState((Generic) base_state); // LTO cast ???

  toolkit->vdm_Init ((Generic) *vdm_if,(Generic)  ERRS, (Generic) ExprERRS);  // LTO cast ???

  startTimerProc(interp);
  return TCL_OK; 
}
