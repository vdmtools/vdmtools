#ifdef _MSC_VER
#define UNICODE
#endif

#include "date.h"
#include <fstream>

//#include "AS.h"
//#include "SEM.h"

#include "rt_info.h"
#include "tools.h"
#include "tbutils.h"
#include "val2x.h"
#include "testcoverage.h"
#include "contextinfo.h"
#include "cli_options.h"
#include "servicemanager.h"
#include "parserservice.h"

#include "CPP.h"

// Interface to the Specification Manager
#include "projectval.h"
#include "ToolKit.h"
#include "BaseTools.h"
#include "Repository.h"
#include "Interface.h"
#include "PromptErr.h"
#include "BatchErr.h"

#include <stdio.h>
#include "tb_getopt.h"
#include <stdlib.h>
#include "tb_exceptions.h"
#include "vdmcg.h"              // Header files for code generator
#include <string.h>
#include "metaiv.h" /* Basic Meta-IV/VDM data types and operations.      */
#include "CI.h"
#include "specfile.h"
#include "javascan.h"

extern bool java_parse (wchar_t*,
                        bool, 
                        vdm_Errors*,
                        TYPE_CPP_File&,
                        TokenList*, 
                        ContextInfo&, 
                        bool, 
                        const TYPE_CI_FileId&);

class AsciiInterface : public vdm_Interface
{
public:
  virtual void vdm_ChangedFile() {}
  virtual void vdm_ChangedMod() {}

  virtual void vdm_Log(const type_cL & msg) {
    vdm_log << Seq2Str (msg) << L"\n" << flush;
  }

  virtual void vdm_Error(const type_cL & msg) {
    vdm_err << Seq2Str (msg) << L"\n" << flush;
  }

  virtual void vdm_ExecuteMainLoop ()
  {
    ExecuteMainLoop ();
  }

  virtual void vdm_CallBack (const Record &) {}

  virtual void vdm_EnableUserInput () {}
  virtual void vdm_DisableUserInput () {
//    LastCountTime = clock();
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
  
  virtual wstring GetFileName(int fileid)
  {
    vdm_ToolMediator * med = ObjGet_vdm_ToolMediator (vdm_mediator);
    Generic file = med->vdm_GetFileName (fileid);
    if (file.IsRecord () && Record(file).Is(vdm_FileName))
      return ExtractFileName(file);
    else
      return L"";
  }
  
public:

  AsciiErrors ()
  {
    char *_emacs;
    run_time = true;
    emacs = ((_emacs=getenv("EMACS")) && strcmp(_emacs, "t")==0);
  }
  
  virtual void vdm_SetFilePosition(const Record & msg)
  {
    int fileid = Int (msg.GetField (pos_ErrMsg_fid)).GetValue();
    int lineid = Int (msg.GetField (pos_ErrMsg_line)).GetValue();
    //    int columnid = Int (msg.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);

    if (file.length() > 0) {
      Record tfn = Repos()->vdm_GetTempFileName (mk_FileName (file));
      wstring tmpfn = ExtractFileName (tfn);
      if (tmpfn.length() > 0)
        file = tmpfn;
      if (emacs)
        vdm_err << L"\032\032" << file << L":" << lineid << L":1\n" << flush;

      //if (run_time)
      //vdm_err << L"In " << file << L", l." << lineid
      //<< L", c." << columnid << L"\n";
    }
    else if (fileid != 0) {
      // vdm_err << L"c." << columnid << L" in the command-line\n";
    }
  }

  virtual void vdm_ShowMsg() {
    run_time = false;
    Record msg = vdm_msgs[vdm_msgPtr];
    vdm_SetFilePosition(msg);
    wstring file = GetFileName (Int (msg.GetField (pos_ErrMsg_fid))); 
    vdm_err << ExtractMessageAll(file, msg) << flush;
    run_time = true;
  }
  
  virtual void vdm_MakeStatus(const Int & NoErr, const Int & NoWarn)
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

// redifinition of the the error stream.
// used in -b mode

static ofstream error_stream;
static int report_error (const wchar_t* msg, int len)
{
  string msgmbr = wstring2mbstr(wstring (msg,len));

  cerr.write(msgmbr.c_str(), len);
  cerr.flush ();
  if (error_stream) 
    error_stream.write(msgmbr.c_str(), len);
  return 1;
}


#ifdef _MSC_VER
#include <io.h>
#endif // _MSC_VER

#include <signal.h>
void sigfunc(int sig)
{
  char * s = "signal handler called. exiting...\n";
  write(2, s, strlen(s));
  exit(0);
}

// This funcion is called when the user presses the C-c
void StopButton(int sig) {
  TBDEBUG::BreakIP();
}

TBOptions *cliOpt;


#ifdef SHOW_MALLOC_STATS
#include "stdio.h"

struct ShowBuildCount {
  ShowBuildCount() { printf(L"\nInitial buildcount=%d\n", MetaivVal::BuildCount); }
  ~ShowBuildCount() { printf(L"\nInitial buildcount=%d\n", MetaivVal::BuildCount); }
};

ShowBuildCount showbuildcount; /*  */

#endif //SHOW_MALLOC_STATS

void PrintCPPAstTuple (ostream& f, 
                       const Generic& files, 
                       ContextInfo & ci, 
                       const Map & fid_m)
{
  f << "mk_CPP`AstCI(\n"; 
//  f << TransformIntegers (cppast) << L",\n"; 
  f << wstring2mbstr(files.ascii ()) << ",\n"; 
  ci.pp(f, true); 
  f << ", \n";

  // Print the fid_m;
  f << "{ \n";
  bool first = true;
  Generic dom,rng;
  for (bool cont = fid_m.First(dom,rng); cont; cont = fid_m.Next(dom,rng)) {
    if (first) {
      first = false;
    }
    else {
      f << ",\n";
    }
    
    f << wstring2mbstr(dom.ascii()) << " |->" << wstring2mbstr(rng.ascii());
  }
  f  << "}\n";
  f << ")" << "\n"; 
}

int main (int argc, char *argv[]) {
  Set files;
  Map fidMap;
  AsciiErrors * javaErrs = new AsciiErrors();
  ContextInfo ci;
  int fileid = 9;
  char *exec_name = argv[0];
  for (int i = 1; i < argc; i++ )
    if (argv [i]) {
      char *input_filename;
      ifstream inp;
      bool parse_error = true;
      fileid++;
      input_filename = argv[i];
      inp.open (input_filename);
      fidMap.Insert (Int (fileid),Sequence(string2wstring (input_filename))); 
      init_CPP ();
      fprintf (stdout, "%s Begins looking at file '%s'\n", exec_name,input_filename);
      inp.seekg (0);  // rewind the input file
      javasLEXINPUT.ReadFrom (inp);
      TYPE_CPP_File javafile;
      wchar_t* ifn = (wchar_t*)string2wstring(input_filename).c_str();
      parse_error = java_parse (ifn,
                                true,
                                javaErrs,    // pointer to errors
                                javafile,    // the resulting AST
                                ci,          // 
                                true,        // Reset contextinfo
                                TYPE_CI_FileId (fileid));
      if (!parse_error) {
/*
        ofstream outputfile;
        wstring outputfileName (input_filename);
        outputfileName += L".cppas";
        outputfile.open (outputfileName.c_str (), ios::out);
        outputfile << javafile -> ascii () << endl;
        outputfile.close ();
        cout << javafile -> ascii () << endl;
*/
	javafile.SetField(2,Sequence(string2wstring (strlen(input_filename)==0 ? "unknown" : input_filename)));
        files.Insert (javafile);
//        delete javafile;
      }
      else
        cout << "Parse error: \n" << endl;
      inp.close();
      fprintf (stdout,"-- Ends looking at file '%s'\n", input_filename);
//      reset_report();
    }
  string cifileName ("m4cpp");
  ofstream cifile;
  cifile.open (cifileName.c_str (), ios::out);
  PrintCPPAstTuple (cifile, files, ci, fidMap);
  cifile.close ();
  exit (0);
}
