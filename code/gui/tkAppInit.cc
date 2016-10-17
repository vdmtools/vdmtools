//**
//** WHAT
//**    VDM-SL Toolbox GUI initialisation functions. 
//** ID
//**    $Id: tkAppInit.cc,v 1.45 2001/06/12 15:04:25 paulm Exp $
//** COPYRIGHT
//**    (C) Kyushu University
//**

/* 
 * tkAppInit.cc --
 *
 *	Provides a default version of the Tcl_AppInit procedure for
 *	use in wish and similar Tk-based applications.
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN L"AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <stdlib.h>
#if defined(__alpha) || defined(__GNUG__)
#include <unistd.h>  // to define isatty ().
#endif // __alpha


#if defined(USEUML) && defined(_MSC_VER)
#define NOMINMAX
#undef State
#undef Name
#include "stdafx.h"
#undef PURE  // Somewhere in MFC PURE is defined. This conflicts with
             // definitions in asquotes.h
#endif // defined(USEUML) && defined(_MSC_VER)

#ifdef CORBA_API
#include "init_corba.h"
#endif CORBA_API

#include <tcl.h>
#include <tk.h>                // Watch out for conflict with metaiv.h
                                // tk.h defines Bool.
#include <itcl.h>
#include <itk.h>
#include <tix.h>


#if defined(LINUX) && !defined(_MSC_VER)
#include "crypt.h"
#endif
#include "tb_version.h"

#include "main_tag.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>             // For strerror()
#include <errno.h>

#ifdef _MSC_VER
//--------------------
// From winMain.c: 
/*
 * The following declarations refer to internal Tk routines.  These
 * interfaces are available for use, but are not supported.
 */

EXTERN void             TkConsoleCreate _ANSI_ARGS_((void));
EXTERN int              TkConsoleInit _ANSI_ARGS_((Tcl_Interp *interp));
//--------------------
#include <tk.h>

#define WIN32_LEAN_AND_MEAN
#if defined (_MSC_VER) && !defined(USEUML)
#include <windows.h>
#endif
#undef WIN32_LEAN_AND_MEAN

#include <malloc.h>

#include <itcl.h>
#include <itk.h>

#include <ctype.h>

#endif // _MSC_VER


// Arg, someone defines Bool. 
#undef Bool 
#include "flexlm.h" 
#include "tools.h"              //

#if defined(USEUML) && defined(_MSC_VER)
#include "uml-be.h"

extern UMLMapper * UML;

int APIENTRY
TclMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpszCmdLine,
        int nCmdShow);

//***********************************************************
// Declaration of a class which inherits from CWinApp

#ifndef __AFXWIN_H__
        #error include 'stdafx.h' before including this file for PCH
#endif


class ThisApp : public CWinApp
{
public:
  ThisApp(){}
public:
  virtual BOOL InitInstance();
  virtual int Run();      
};


BOOL ThisApp::InitInstance()
{
  // Initialize OLE libraries. Necessary to use the OLE interface of 
  // Rational Rose
  if (!AfxOleInit()){
    AfxMessageBox(L"Unable to initialize OLE.");
    return 0;
  }
  return TRUE;
}


int ThisApp::Run()
{
  return TclMain(NULL,NULL,m_lpCmdLine,0);
}
//***********************************************************
// The one and only instance of the toolbox.

ThisApp thisApp;

//***********************************************************
#endif // defined(USEUML) && defined(_MSC_VER)


extern L"C" {
  //extern int              Itcl_SafeInit (Tcl_Interp *interp);
  //extern int              Itcl_Init (Tcl_Interp *interp);
  //extern int              Itk_Init (Tcl_Interp *interp);
}

Tcl_Interp	*guiInterp;	/* Keep a copy around */
TBOptions *cliOpt;


extern int tkVsl_Init(Tcl_Interp*);

static void Complain(wstring title, wstring message); 
static void TclComplain(wstring title, Tcl_Interp *interp); 

//************************************************************
// Environment check.
//************************************************************

#ifdef VDMSL
char const * VDMROOT = L"VDMSLROOT";
#endif
#ifdef VDMPP
char const * VDMROOT = L"VDMPPROOT";
#endif

char * program_name; 

static
bool
checkfordir(wstring s)
{
  struct stat statv; 
  int e = stat(s.c_str(), &statv);
  if (e != 0) {
    // Complain(L"Error",
    //          wstring(L"Checking directory `") + s + "': " + strerror(errno));
    // return false; 
  }
  if ((e==0) && (statv.st_mode & S_IFMT) == S_IFDIR) {
    return true; 
  } else {
    Complain(wstring(L"Error"),
             wstring(L"The path ") + s + " should point to a directory\n"); 
    return false; 
  }
}

#ifdef _MSC_VER
#define DIRSEP '\\'
#else
#define DIRSEP '/'
#endif

#ifdef _MSC_VER
wstring WinFormatMessage(long err)
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                (LPTSTR) &lpMsgBuf,
                0,
                NULL);
  wstring s(static_cast<wchar_t*>(lpMsgBuf));
  LocalFree(lpMsgBuf);
  return s; 
}

bool
FindRegEntry(const wstring & valuename,
             wstring & value, wstring & errtxt)
{
  HKEY hcpl;
  long err;
  wstring key = wstring(L"SOFTWARE\\VDMTOOLS\\")
    + tb_version.GiveToolTitle()
    + L"\\" + tb_version.GetVersionId(); 
  
  err = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     key.c_str(), 0,
                     KEY_READ,
                     &hcpl);
  if (err != ERROR_SUCCESS) {
    wstring e = L"Error RegOpenKeyEx";
    e += L"(" + key + "):\n" + WinFormatMessage(err);
    errtxt = e;;
    return false;
  } else {
    unsigned wchar_t buffer[80];
    unsigned long size = sizeof(buffer); 
    err = RegQueryValueEx(hcpl,
                          valuename.c_str(),
                          0,
                          (unsigned long*)0, 
                          buffer, &size);
    if (err == ERROR_SUCCESS) {
      value = (wchar_t*)buffer;
      errtxt = L"";
      return true; 
    } else {
      wstring e = L"RegQueryValueEx error:\n";
      e += L"(" + valuename + ")\n\t" + WinFormatMessage(err);
      errtxt = e;
      return false;
    }      
  }
}

#endif

static wstring TCL_LIBRARY, TK_LIBRARY, ITCL_LIBRARY, ITK_LIBRARY,
  TIX_LIBRARY, UML_LIBRARY;

bool checkenvvars() 
{

  wchar_t * r = getenv(VDMROOT);
  
  wstring rootdir; 
  if (!r) {
#ifdef _MSC_VER
    wstring value, err; 
    if (! FindRegEntry(L"ROOTDIR", rootdir, err)) {
      wstring e = L"The environment variable ";
      e += wstring(VDMROOT) + L" is not set \nand the registry lookup\n";
      e += L"failed with this error: \n"; 
      e += err; 
      e += L"\nThe Toolbox does not seem to be properly installed."; 
      Complain(L"Error", e); 
      return false; 
    }
#else
    Complain(L"Error",
             wstring(L"You don't have a ") + VDMROOT +
             L" environment variable.\n" + "exiting...\n"); 
    return false;
#endif
  } else {
    rootdir = r; 
  }

  if (rootdir == L"ignore")
    return true; 

#ifdef _MSC_VER
#ifdef USEUML
  UML_LIBRARY  = rootdir + L"\\uml"; 
#endif // USEUML
  TCL_LIBRARY  = rootdir + L"\\lib\\tcl"; 
  TK_LIBRARY   = rootdir + L"\\lib\\tk"; 
  ITCL_LIBRARY = rootdir + L"\\lib\\itcl";  
  ITK_LIBRARY  = rootdir + L"\\lib\\itk"; 
  TIX_LIBRARY  = rootdir + L"\\lib\\tix"; 
#else
  TCL_LIBRARY  = rootdir + L"/lib/tcl"; 
  TK_LIBRARY   = rootdir + L"/lib/tk"; 
  ITCL_LIBRARY = rootdir + L"/lib/itcl";  
  ITK_LIBRARY  = rootdir + L"/lib/itk"; 
  TIX_LIBRARY  = rootdir + L"/lib/tix"; 
#endif
  
  if (! (checkfordir(TCL_LIBRARY) &&
         checkfordir(TK_LIBRARY) &&
         checkfordir(ITCL_LIBRARY) &&
         checkfordir(ITK_LIBRARY) &&
         checkfordir(TIX_LIBRARY)
#ifdef USEUML
         && checkfordir(UML_LIBRARY)
#endif // USEUML
         )) {
    return false; 
  }

  return true; 
}


//************************************************************














#ifdef EMBEDDED
extern wchar_t* TCL_CODE_STRINGS[];

char *embed_arraytowstring(wchar_t **src)
{
  wchar_t *dest, *dp, *sp;

  wchar_t **s0; 
  int length; 
  for (length=0, s0=src; *s0; s0++) 
    length += strlen(*s0); 

  dest = (wchar_t*) malloc(length+1);
  if (dest == 0) {
    fprintf(stderr, L"Internal unpack error\n");
    exit(2);
  }

  for (dp=dest ; (sp=*src) != 0; src++) {
    while (*sp != '\0')
      (*dp++ = *sp++);
  }
  *dp = '\0';
  return dest;
}

int RunVSLCmd(Tcl_Interp *interp)
{
  int code;
  wchar_t *src;
  
  src = embed_arraytowstring(TCL_CODE_STRINGS);
  code = Tcl_Eval(interp, src);
  if (code != TCL_OK) {
    TclComplain(L"Error during initialisation:", interp); 
  }
  free(src); 
  return code; 
}

int
tkif_RUN(ClientData /*clientData*/, Tcl_Interp *interp, int argc, wchar_t ** /*argv*/)
{
  if (argc != 1) {
    Tcl_AppendResult(interp,L"tkif_RUN: wrong number of parameters.",
		     (wchar_t *)NULL);
    return TCL_ERROR;
  }
  Tcl_ResetResult(interp);
  return RunVSLCmd(interp); 
}

#endif /* EMBEDDED */

//Tk_Window tkmainwin;

int vdmgde_init(Tcl_Interp *interp)
{
  if (tkVsl_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }

  

#ifdef FLM
  wstring error;
  error = InitFLM (); 
  if (error.length() > 0) {
    Complain(L"License denied", error); 
    ExitToolbox (0);
  }
#endif //FLM

#ifdef EMBEDDED
  if (RunVSLCmd(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  Tcl_CreateCommand(interp, L"tkif_RUN", tkif_RUN,
                    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
#else
  //if (Tcl_Eval(interp, L"source main.tcl") == TCL_ERROR) {
  // return TCL_ERROR;
  //}
#endif

  cliOpt = new GUIOptions;

#if defined(USEUML) && defined(_MSC_VER)
  // Now the single instance, UML (declared in tk_if.cc), of UMLMapper
  // has been created.
  if(!UML){
    Complain(L"Fatal error", "UML instance was initialised. Unable to call SetUmlRoot()"); 
    ExitToolbox (0);    
  }
  UML->SetUmlRoot(UML_LIBRARY.c_str());
#endif // USEUML && _MSC_VER
  return TCL_OK; 
}


static Tcl_Interp *interp;      /* Interpreter for this application. */

static Tk_Window mainWindow;    /* The main window for the application.  If
                                 * NULL then the application no longer
                                 * exists. */

#ifdef EMBEDDED

/*
 * Global variables used by the main program:
 */

static Tcl_DString command;     /* Used to assemble lines of terminal input
                                 * into Tcl commands. */
static Tcl_DString line;        /* Used to read the next line from the
                                 * terminal input. */
static int tty;                 /* Non-zero means standard input is a
                                 * terminal-like device.  Zero means it's
                                 * a file. */

static
void
SetEnv(Tcl_Interp *interp, wstring e, wstring v)
{
  if (v.length() > 0) {
    Tcl_SetVar2(interp, L"env", (wchar_t*)e.c_str(), (wchar_t*) v.c_str(), TCL_GLOBAL_ONLY);
  }
}

void 
tkif_Tk_Main(int argc,  wchar_t **argv, Tcl_AppInitProc *appInitProc)
{
  // Copied from tkMain.c in tk4.2. 
  wchar_t *args; 
  wchar_t buf[20];
  Tcl_Channel inChannel, outChannel, errChannel;

  Tcl_FindExecutable(argv[0]);
  interp = Tcl_CreateInterp();
#ifdef TCL_MEM_DEBUG
  Tcl_InitMemory(interp);
#endif

  SetEnv(interp, L"TCL_LIBRARY", TCL_LIBRARY); 
  SetEnv(interp, L"TK_LIBRARY", TK_LIBRARY); 
  SetEnv(interp, L"ITCL_LIBRARY", ITCL_LIBRARY); 
  SetEnv(interp, L"ITK_LIBRARY", ITK_LIBRARY); 
  SetEnv(interp, L"TIX_LIBRARY", TIX_LIBRARY); 

  /*
   * Make command-line arguments available in the Tcl variables L"argc"
   * and L"argv".
   */

  args = Tcl_Merge(argc-1, argv+1);
  Tcl_SetVar(interp, L"argv", args, TCL_GLOBAL_ONLY);
  ckfree(args);
  sprintf(buf, L"%d", argc-1);
  Tcl_SetVar(interp, L"argc", buf, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, L"argv0", argv[0], TCL_GLOBAL_ONLY);

  /*
   * Set the L"tcl_interactive" variable.
   */

  /*
   * For now, under Windows, we assume we are not running as a console mode
   * app, so we need to use the GUI console.  In order to enable this, we
   * always claim to be running on a tty.  This probably isn't the right
   * way to do it.
   */

#ifdef __WIN32__
  tty = 1;
#else
  tty = isatty(0);
#endif
  Tcl_SetVar(interp, L"tcl_interactive",
             (wchar_t*)((tty) ? L"1" : "0"), TCL_GLOBAL_ONLY);

  /*
   * Invoke application-specific initialization.
   */

  if ((*appInitProc)(interp) != TCL_OK) {
    return; 
  }

  /*
   * Invoke the script specified on the command line, if any.
   */
#if 0    // Don't source when embedded. 
    /*
     * Establish a channel handler for stdin.
     */
  
  inChannel = Tcl_GetStdChannel(TCL_STDIN);
  if (inChannel) {
    Tcl_CreateChannelHandler(inChannel, TCL_READABLE, StdinProc,
                             (ClientData) inChannel);
  }
  if (tty) {
    Prompt(interp, 0);
  }
#endif // 
  outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel) {
    Tcl_Flush(outChannel);
  }
  Tcl_DStringInit(&command);
  Tcl_DStringInit(&line);
  Tcl_ResetResult(interp);

  /*
   * Loop infinitely, waiting for commands to execute.  When there
   * are no windows left, Tk_MainLoop returns and we exit.
   */

  Tk_MainLoop();
  Tcl_DeleteInterp(interp);
  Tcl_Exit(0);

  //error:
  /*
   * The following statement guarantees that the errorInfo
   * variable is set properly.
   */

  Tcl_AddErrorInfo(interp, L"");
  errChannel = Tcl_GetStdChannel(TCL_STDERR);
  if (errChannel) {
    Tcl_Write(errChannel, Tcl_GetVar(interp, L"errorInfo", TCL_GLOBAL_ONLY),
              -1);
    Tcl_Write(errChannel, L"\n", 1);
  }
  Tcl_DeleteInterp(interp);
  Tcl_Exit(1);

}

#endif // EMBEDDED

static
void
TclComplain(wstring title, Tcl_Interp *interp)
{
  wchar_t *p = Tcl_GetVar(interp, L"errorInfo", TCL_GLOBAL_ONLY);
  if (!p)
    p = interp->result;
#ifdef _MSC_VER
  MessageBox(NULL, p, title.c_str(), MB_OK|MB_ICONSTOP);
#else 
  cerr << title << L":" << p << "\n"; 
#endif
}

static 
void 
Complain(wstring title, wstring message) 
{
#ifdef _MSC_VER
  MessageBox(NULL, message.c_str(), title.c_str(), MB_OK|MB_ICONSTOP );
#else 
  cerr << message << L"\n"; 
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

extern L"C" 
int
xTcl_AppInit(Tcl_Interp * interp)
{

  guiInterp = interp;

#if  defined(_MSC_VER)
  // This does not depend on EMBEDDED being defined. 

  /*
   * Create the console channels and install them as the standard
   * channels.  All I/O will be discarded until TkConsoleInit is
   * called to attach the console to a text widget.
   */
  TkConsoleCreate();
#endif
  if (Tcl_Init(interp) == TCL_ERROR) {
    TclComplain(L"Tcl Init Error", interp); 
    return TCL_ERROR;
  }

  if (Tk_Init(interp) == TCL_ERROR) {
    TclComplain(L"Tk Init Error", interp); 
    return TCL_ERROR;
  }
  Tcl_StaticPackage(interp, L"Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);

  if (Itcl_Init(interp) == TCL_ERROR) {
    TclComplain(L"Itcl Init Error", interp); 
    return TCL_ERROR;
  }
  Tcl_StaticPackage(interp, L"Itcl", Itcl_Init, (Tcl_PackageInitProc *) NULL);

  if (Itk_Init(interp) == TCL_ERROR) {
    TclComplain(L"Itk Init Error", interp); 
    return TCL_ERROR;
  }
  Tcl_StaticPackage(interp, L"Itk", Itk_Init, (Tcl_PackageInitProc *) NULL);

  if (Tix_Init(interp) == TCL_ERROR) {
    TclComplain(L"Tix Init Error", interp); 
    return TCL_ERROR;
  }
  Tcl_StaticPackage(interp, L"Tix", Tix_Init, (Tcl_PackageInitProc *) NULL);



    /*
     *  This is itkwish, so import all [incr Tcl] commands by
     *  default into the global namespace.  Fix up the autoloader
     *  to do the same.
     */
    if (Tcl_Import(interp, Tcl_GetGlobalNamespace(interp),
            L"::itk::*", /* allowOverwrite */ 1) != TCL_OK) {
        return TCL_ERROR;
    }

    if (Tcl_Import(interp, Tcl_GetGlobalNamespace(interp),
            L"::itcl::*", /* allowOverwrite */ 1) != TCL_OK) {
        return TCL_ERROR;
    }

    if (Tcl_Eval(interp, L"auto_mkindex_parser::slavehook { _%@namespace import -force ::itcl::* ::itk::* }") != TCL_OK) {
        return TCL_ERROR;
    }




  /*
   * Initialize the console only if we are running as an interactive
   * application.
   */

#if defined(_MSC_VER) && ! defined(EMBEDDED)
  if (strcmp(Tcl_GetVar(interp, L"tcl_interactive", TCL_GLOBAL_ONLY), "1")
      == 0) {
    if (TkConsoleInit(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }
  }
#endif

  return vdmgde_init(interp); 
}


static
void 
CommonMain(int argc, wchar_t**argv)
{
  
  if (!checkenvvars()) {
    exit(1);
  }
  

#ifndef FLM
  crypt_test();
#endif    

#ifdef CORBA_API
  wstring err;
  if(!init_corba_api(argc, argv, err)){
    Complain(L"Unable to initialise CORBA API", err); 
    ExitToolbox (0);
  }
#endif


#ifdef EMBEDDED
  tkif_Tk_Main(argc, argv, xTcl_AppInit);
#else
  Tk_Main(argc, argv, xTcl_AppInit);
#endif
}





#ifndef _MSC_VER                // Are we on Unix?
// Yes, this is unix. 
int
main(int argc,  wchar_t **argv)
{
  // #ifdef EMBEDDED
  /* To prevent TCL from entering interactive mode we
   * connects stdin to /dev/null
   */
  // freopen(L"/dev/null", "r", stdin);
  // #endif /* EMBEDDED */
  
  CommonMain(argc, argv); 
  return 0;
}
#endif // Unix


#ifdef _MSC_VER

static void WishPanic _ANSI_ARGS_(TCL_VARARGS(wchar_t *,format));

/*
 *----------------------------------------------------------------------
 *
 * WishPanic --
 *
 *	Display a message and exit.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Exits the program.
 *
 *----------------------------------------------------------------------
 */

void
WishPanic TCL_VARARGS_DEF(wchar_t *,arg1)
{
    va_list argList;
    wchar_t buf[1024];
    wchar_t *format;
    
    format = TCL_VARARGS_START(wchar_t *,arg1,argList);
    vsprintf(buf, format, argList);

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBox(NULL, buf, L"Fatal Error in Wish",
	MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
    ExitProcess(1);
}

/*
 *----------------------------------------------------------------------
 *
 * WinMain --
 *
 *	Main entry point from Windows.
 *
 * Results:
 *	Returns false if initialization fails, otherwise it never
 *	returns. 
 *
 * Side effects:
 *	Just about anything, since from here we call arbitrary Tcl code.
 *
 *----------------------------------------------------------------------
 */

#ifdef USEUML
int APIENTRY
TclMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpszCmdLine,
        int nCmdShow)
#else
int APIENTRY
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpszCmdLine,
        int nCmdShow)
#endif // USEUML
{
  wchar_t **argv, **argvlist, *p;
  int argc, size, i;
  wchar_t buffer[MAX_PATH];

  Tcl_SetPanicProc(WishPanic);

  /*
   * Increase the application queue size from default value of 8.
   * At the default value, cross application SendMessage of WM_KILLFOCUS
   * will fail because the handler will not be able to do a PostMessage!
   * This is only needed for Windows 3.x, since NT dynamically expands
   * the queue.
   */
  SetMessageQueue(64);

  /*
   * Precompute an overly pessimistic guess at the number of arguments
   * in the command line by counting non-space spans.  Note that we
   * have to allow room for the executable name and the trailing NULL
   * argument.
   */

  for (size = 3, p = lpszCmdLine; *p != '\0'; p++) {
    if (isspace(*p)) {
      size++;
      while (isspace(*p)) {
        p++;
      }
      if (*p == '\0') {
        break;
      }
    }
  }
  argvlist = (wchar_t **) ckalloc((unsigned) (size * sizeof(wchar_t *)));
  argv = argvlist;

  /*
   * Parse the Windows command line wstring.  If an argument begins with a
   * double quote, then spaces are considered part of the argument until the
   * next double quote.  The argument terminates at the second quote.  Note
   * that this is different from the usual Unix semantics.
   */

  for (i = 1, p = lpszCmdLine; *p != '\0'; i++) {
    while (isspace(*p)) {
      p++;
    }
    if (*p == '\0') {
      break;
    }
    if (*p == '"') {
      p++;
      argv[i] = p;
      while ((*p != '\0') && (*p != '"')) {
        p++;
      }
    } else {
      argv[i] = p;
      while (*p != '\0' && !isspace(*p)) {
        p++;
      }
    }
    if (*p != '\0') {
      *p = '\0';
      p++;
    }
  }
  argv[i] = NULL;
  argc = i;

  /*
   * Since Windows programs don't get passed the command name as the
   * first argument, we need to fetch it explicitly.
   */

  GetModuleFileName(NULL, buffer, sizeof(buffer));
  argv[0] = buffer;
  
  CommonMain(argc, argv); 
  return 1;
}

// Dummy main
int main(int argc, wchar_t** argv)
{
  MessageBox(NULL, L"this is main()", "Tk Init Error", MB_OK|MB_ICONSTOP);
  return 1; 
}

#endif
  

