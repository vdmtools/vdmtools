// Implementation of command-line options
// $Id: cli_options.cc,v 1.12 2006/06/23 05:34:19 vdmtools Exp $

#include "cli_options.h"
#include "tb_getopt.h"
#include "tbutils.h"
#include "tools.h"
#include "projectval.h"
#include "testcoverage.h"
#include "tb_wstring.h"
#include "contextinfo.h"
#include "settings.h"

#ifdef __linux__
#include <stdlib.h>
#endif // __linux__
#ifdef __Cygwin__
#include <stdlib.h>
#include "getopt.h"
#endif // __Cygwin__


//
// Remaining chars for single char flags
// ABEGHJKVWY

int aflg = 0, pflg = 0, iflg = 0, tflg = 0, fflg = 0, gflg = 0, wflg = 0,
    bflg = 0, dflg = 0, lflg = 0, cflg = 0, jflg = 0, mflg = 0,
    zflg = 0, eflg = 0, sflg = 0, kflg = 0, uflg = 0, nflg = 0, vflg = 0,
    Oflg = 0, Fflg = 0, Lflg = 0, Uflg = 0, Eflg = 0,
    Gflg = 0, Kflg = 0, Bflg = 0, oflg = 0, qflg = 0, yflg = 0,
    primary = 0;

/** Utility functions **************************************************/
/** These functions provide certain basic input/output facilities.    **/

bool CLIOptions::ExpectNumericOptArg(const wchar_t *arg, wchar_t opt, int &value)
{
  wstring errmsg = L"\nOption ";
  errmsg += opt;
  errmsg += L" requires an integer value - ignored.";
  //return TBUTILS::ExpectNumericArg(arg, errmsg, value);
  if (TBUTILS::ExpectNumericArg(arg, value))
    return true;
  vdm_err << errmsg << endl; 
  return false;
}

bool CLIOptions::ProcessOptions(int argc, char** argv)
{
  int opt_args = 0;
  int c;

#ifdef VDMSL
//  while ((c = getopt(argc, argv, "EhcjmpwitDIPQxsez:kubrR:aldfnNgGO:C:q")) != -1)
  while ((c = getopt(argc, argv, "abcdefghijklmno:pqrstuwxyz:AC:DEGINO:PQR:VW:")) != -1)
#endif // VDMSL
#ifdef VDMPP
#ifdef VICE
//  while ((c = getopt(argc, argv, "EhcjmpwitDIPQZ:xsez:kubrR:aldfnNgGLO:C:T:M:S:F:X:U:K:B:o")) != -1)
  while ((c = getopt(argc, argv, "abcdefghijklmno:pqrstuwxyz:AB:C:DEF:GIK:LM:NO:PQR:S:T:U:VW:X:Z:")) != -1)
#else
//  while ((c = getopt(argc, argv, "EhcjmpwitDIPQZ:xsez:kubrR:aldfnNgGLO:C:T:M:S:U:K:B:o")) != -1)
  while ((c = getopt(argc, argv, "abcdefghijklmno:pqrstuwxyz:AB:C:DEGIK:LM:NO:PQR:S:T:U:VW:Z:")) != -1)
#endif //VICE
#endif //VDMPP
  {
    switch (c) {
      case 'h': {
        Usage();
        // Usage exits.
        break;
      }
      case 'b' : {
        bflg++;
        primary_flag = FLAG_INTERNAL;
        primary++;
        break;
      }
      case 'C': {
        int l;
        if (ExpectNumericOptArg(TBWSTR::string2wstring(optarg).c_str(), 'C', l))
        {
          int c_flag = Settings.GetCFlag();
          Settings.SetCFlag(c_flag + 1);
        }
        else
          TOOLS::ExitToolbox(1);
        break;
      }
      case 'g': {
        gflg++;
        primary_flag = FLAG_POG;
        primary++;
        break;
      }
      case 'G': {
        Gflg++;
        primary_flag = FLAG_POGTEST;
        primary++;
        break;
      }
      case 'i': {
        iflg++;
        primary_flag = FLAG_INTERPRETER;
        opt_args++;
        primary++;
        break;
      }
      case 'p': {
        pflg++;
        primary_flag = FLAG_SYNTAXCHECK;
        primary++;
        break;
      }
      case 'o': {
        oflg++;
        script_file = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 'q': {
        qflg++;
        break;
      }
      case 'w': {
        wflg++;
        break;
      }
      case 'n': {
        Settings.SetIndex(1);
        nflg = 1;
        break;
      }
      case 'N': {
        Settings.SetIndex(2);
        nflg = 1;
        break;
      }
      case 'd': {
        dflg++;
        break;
      }
      case 'f': {
        fflg++;
        Settings.ErrorLevelPRF();
        break;
      }
      case 'c': {
        cflg++;
        primary_flag = FLAG_CPPCG;
        primary++;
        break;
      }
#ifdef VDMPP
      case 'B': {
        Bflg++;
        class_name = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 'j': {
        jflg++;
        primary_flag = FLAG_JAVACG;
        primary++;
        break;
      }
      case 'm': {
        mflg++;
        break;
      }
      case 'e': {
        eflg++;
        break;
      }
      case 'z': {
        zflg++;
        package_name = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 'K': {
        Kflg++;
        class_names = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 's': {
        sflg++;
        break;
      }
      case 'k': {
        kflg++;
        break;
      }
      case 'u': {
        uflg++;
        break;
      }
      case 'v': {
        vflg++;
        break;
      }
      case 'y': {
        yflg++;
        break;
      }
#endif //VDMPP
      case 'a': {
        aflg++;
        break;
      }
      case 'l': {
        lflg++;
        primary_flag = FLAG_PRETTYPRINT;
        primary++;
        break;
      }
      case 't': {
        tflg++;
        primary_flag = FLAG_TYPECHECKER;
        primary++;
        break;
      }
      case 'x': {
        //vdm_log << L"Note: The TEX option is no longer used." << endl << flush;
        Settings.RTErrExceptionOn();
        break;
      }
      case 'r': {
        Settings.SetPrettyPrintRTI(1);
        Settings.SetCGRTI(1);
        break;
      }
      case 'R': {
        coverage_file = TBWSTR::cinstr2wstring(optarg);
        //         opt_args = opt_args + 1;
        break;
      }
      case 'D': {
        Settings.DtcOn();
        break;
      }
      case 'I': {
        Settings.InvOn();
        Settings.DtcOn(); // 20071030
        break;
      }
      case 'A': {
        Settings.AssertOn();
        break;
      }
      case 'P': {
        Settings.PreOn();
        break;
      }
      case 'Q': {
        Settings.PostOn();
        break;
      }
      case 'V': {
        Settings.VDM10Off();
        break;
      }
      case 'O': {
        Oflg++;
        output_file = TBWSTR::cinstr2wstring(optarg);
        break;
      }
/*
      case 'E': {
        Eflg++;
        primary_flag = FLAG_EXTERNAL;
        primary++;
        break;
      }
*/
      case 'W': {
        wstring encoding (TBWSTR::cinstr2wstring(optarg));
#ifdef _MSC_VER
        wstring enc;
        for( std::string::size_type i = 0; i < encoding.size(); i++ )
          if ((encoding[i] != L'-') && (encoding[i] != L'_') && (encoding[i] != L' '))
            enc += towlower(encoding[i]);

        if (enc == wstring(L"utf8"))
          encoding = wstring (L"65001");
        else if (enc == wstring(L"iso885915"))
          encoding = wstring (L"28605");
        else if ((enc == wstring(L"sjis")) || (enc == wstring(L"shiftjis")))
          encoding = wstring (L"932");
        else if (enc == wstring(L"eucjp"))
          encoding = wstring (L"51932");
        else if (enc == wstring(L"big5"))
          encoding = wstring (L"950");
        else if (enc == wstring(L"euckr"))
          encoding = wstring (L"51949");
        else if (enc == wstring(L"gbk"))
          encoding = wstring (L"936");
        else if (enc == wstring(L"koi8r"))
          encoding = wstring (L"20866");
        else if (enc == wstring(L"iso88598"))
          encoding = wstring (L"28598");
        else if (enc == wstring(L"tscii0"))
        //  encoding = wstring (L"");
          encoding = wstring (L"65001");
#endif // _MSC_VER

        TBWSTR::setIOCharSetEnv(encoding);
        break;
      }
#ifdef VDMPP
      case 'Z': {
        Settings.PriorityBasedOn();
        priority_file = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 'M': {
        int val;
        ExpectNumericOptArg(TBWSTR::string2wstring(optarg).c_str(), 'M', val);
#ifdef VICE
        Settings.SetMaxInstr(val);
        Settings.SetTimeSlice(val);
#else
        Settings.SetMaxInstr(val);
#endif //VICE
        break;
      }
      case 'S': {
        TOOLS::SetPrimarySchedulingAlgorithm(TBWSTR::string2wstring(optarg));
        break;
      }
      case 'U': {
        Uflg++;
        interfaceList = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 'L': {
        Lflg++;
        Settings.SetJCGLongs(true);
        break;
      }
#ifdef VICE
      case 'T': {
        int taskswitch = 0;
        if(ExpectNumericOptArg(TBWSTR::string2wstring(optarg).c_str(), 'T', taskswitch))
        {
          Settings.SetTaskSwitch(taskswitch);
        }
        break;
      }
      case 'F': {
        Fflg++;
        time_file = TBWSTR::cinstr2wstring(optarg);
        break;
      }
      case 'X': {
        int timefactor = 0;
        if(ExpectNumericOptArg(TBWSTR::string2wstring(optarg).c_str(), 'X', timefactor))
        {
          Settings.SetTimeFactor(timefactor);
        }
        break;
      }
#endif //VICE
#endif //VDMPP
    }
  }

  if (iflg && (nflg||fflg||dflg||gflg||cflg||jflg||Gflg))
    Usage();

  if (pflg && (Settings.DTC()||Settings.INV()||
               Settings.PreCheck()||Settings.PostCheck()||
               fflg||nflg||dflg||cflg||jflg||gflg||Gflg))
    Usage();

  if (lflg && (Settings.DTC()||Settings.INV()||
               Settings.PreCheck()||Settings.PostCheck()||
               wflg||fflg||dflg||cflg||jflg||gflg||Gflg))
    Usage();

  if (tflg && (Settings.DTC()||Settings.INV()||
               Settings.PreCheck()||Settings.PostCheck()||
               wflg||nflg||cflg||jflg||gflg||Gflg))
    Usage();

  if (cflg)
    if (Settings.DTC()||Settings.INV()||Settings.PostCheck()||
        wflg||nflg||fflg||dflg||tflg||gflg||jflg||Gflg)
      Usage();

  if (jflg)
    if (Settings.DTC()||Settings.INV()||Settings.PostCheck()||
        wflg||nflg||fflg||dflg||tflg||gflg||cflg||Gflg)
      Usage();

  // Check if the primary option is missing

  if ((primary == 0) &&
      (Settings.DTC()||Settings.INV()||
       Settings.PreCheck()||Settings.PostCheck()||
       nflg||fflg||dflg||gflg||aflg ||Oflg||wflg||Gflg))
    Usage ();

  optionsIndex = optind + opt_args;

  if (primary > 1) // We only allow for exactly one of p,i,t,c,j,b,l or no option
    Usage ();

  if ( primary && !CheckNumberOfArguments(argc, opt_args))
    Usage();

  if (optind < argc) {
    wstring basedir ( TBUTILS::tb_getbasedir( TBWSTR::cinstr2wstring( argv[optind] ) ) );
    TBUTILS::SetDefaultPath ( basedir );
  }

  for(int d = GetOptionsIndex(); d < argc; d++) {
    if (!Oflg || (Oflg && TBWSTR::cinstr2wstring(argv[d]) != output_file)) {
      wstring ws (TBWSTR::cinstr2wstring(argv[d]));
      TYPE_ProjectTypes_FileName file (PTAUX::mk_FileName(ws));
      file_l.ImpAppend(file);
    }
  }

  batchMode = (primary != 0);
  return batchMode;
}

CLIOptions::CLIOptions(): batchMode(false),
                          coverage_file(L""),
                          output_file(L""),
                          package_name(L""),
                          interfaceList(L""),
#ifdef VDMPP
                          priority_file(L""),
#ifdef VICE
                          time_file(L""),
#endif //VICE
#endif //VDMPP
                          file_l(Sequence())
{
}

bool CLIOptions::IsBatchMode()
{
  return batchMode;
}

bool CLIOptions::CheckNumberOfArguments(int argc, int opt_args)
{
  return argc - opt_args >= 3;
}

SEQ<TYPE_ProjectTypes_FileName> CLIOptions::GetFileList()
{
  return file_l;
}

wstring CLIOptions::GetCoverageFile()
{
  return coverage_file;
}

wstring CLIOptions::GetResultFile()
{
  return output_file;
}

PrimaryFlagType CLIOptions::GetPrimaryFlag()
{
  return primary_flag;
}

bool CLIOptions::IsInternalTest()
{
  return (aflg > 0);
}

bool CLIOptions::SaveTempFile()
{
  return (wflg > 0);
}

bool CLIOptions::IsDefTypeCheck()
{
  return (dflg > 0);
}

std::wstring CLIOptions::GetOptionalScript()
{
  return script_file;
}

bool CLIOptions::QuitToolBox()
{
  return (qflg > 0);
}

int CLIOptions::GetOptionsIndex()
{
  return optionsIndex;
}

bool CLIOptions::IsSetException()
{
  return (yflg > 0);
}

#ifdef VDMPP
wstring CLIOptions::GetPriorityFile()
{
  return priority_file;
}

wstring CLIOptions::GetClassNameForPush()
{
  return class_name;
}

wstring CLIOptions::GetJavaPackageName()
{
  return package_name;
}

wstring CLIOptions::GetJavaClassNames()
{
  return class_names;
}

bool CLIOptions::JavaSkeletonsOnly()
{
  return sflg;
}

bool CLIOptions::JavaTypesOnly()
{
  return (uflg > 0);
}

bool CLIOptions::JavaGenPrePost()
{
  return (kflg > 0);
}

bool CLIOptions::JavaGenConc()
{
  return (eflg > 0);
}

bool CLIOptions::JavaGenSmall()
{
  return (mflg > 0);
}

wstring CLIOptions::JavaGetInterfaces()
{
  return interfaceList;
}

bool CLIOptions::JavaVDMPrefix()
{
  return (vflg == 0);
}

#ifdef VICE
wstring CLIOptions::GetTimeFile()
{
  return time_file;
}
#endif //VICE
#endif // VDMPP

void CLIOptions::Usage () {
  vdm_log << TOOLS::giveToolTitle() << L" " << TOOLS::giveToolVersionDate () << endl;
#ifdef VDMSL
  vdm_log << L"Usage: vdmde [options] [specfiles]" << endl;
  vdm_log << L"Run vdmde on the specfiles. " << endl;
#else
  vdm_log << L"Usage: vppde [options] [specfiles]" << endl;
  vdm_log << L"Run vppde on the specfiles. " << endl;
#endif

  vdm_log
    << L"Options can be grouped together (-rN is equal to -r -N)." << endl
    << L"  -h Give this help" << endl
    << L"  [-o scriptfile] [-q] [specfiles]" << endl
    << L"      Start command line tool with reading specfiles." << endl
    << L"      -o scriptfile: Execute scripts in scriptfile." << endl
    << L"      -q: Quit when all scripts have been executed." << endl
    << L"      -W encoding: Set specfiles's encoding." << endl
    << L"  -p [-w] [-R coverage-file] specfiles" << endl
    << L"     Parse specfiles." << endl
    << L"     -w: Write specification parts of rtf file as text in specfile.txt" << endl
    << L"     -R coverage-file: Initialise test coverage file." << endl
    << L"     -W encoding: Set specification files's encoding." << endl
    << L"  -t [-d] [-f] specfiles" << endl
    << L"     Type check the specfiles (possible well-formedness)." << endl
    << L"     -d: definite well-formed type check" << endl
    << L"     -f: Extended type check." << endl
    << L"     -W encoding: Set specfiles's encoding." << endl
#ifdef VDMSL
    << L"  -i [-O output-file] [-R coverage-file] [-D [-I]] [-P] [-Q] argfile specfiles" << endl
#endif //VDMSL
#ifdef VDMPP
    << L"  -i [-O output-file] [-R coverage-file] [-D [-I]] [-P] [-Q]" << endl
    << L"     [-Z priority file] [-M num]"
#ifdef VICE
    << L" [-T num] [-X num] [-F time-file]"
#endif //VICE
    << L" argfile specfiles" << endl
#endif // VDMPP
    << L"     Evaluate expressions from argfile. " << endl
    << L"     -O outfile: Write result to outfile" << endl
    << L"     -R coverage-file: Update test coverage file. Created if non-existing." << endl
    << L"     -D,-I,-P,-Q: Evaluate with dynamic typecheck, invariant check " << endl
    << L"     (requires -D), pre condition check, post condition check.   " << endl
#ifdef VDMPP
    << L"     -Z: Evaluate using priority based scheduling.   " << endl
#ifdef VICE
    << L"     -T num: Use num for task switching constant.   " << endl
    << L"     -X num: Use num as a scaling factor for time.  " << endl
    << L"     -F time-file: Read the specified file and use it as the time map. " << endl
#endif //VICE
    << L"     -M num: Use num as number of instructions per slice"
#ifdef VICE
    << L" or" << endl
    << L"             amount of time per slice, according to primary " << endl
    << L"             scheduling algorithm selected"
#endif //VICE
    << L"" << endl
    << L"     -S algorithm: Use specified scheduling algorithm" << endl
    << L"        Algorithm may be one of: " << endl
    << L"          pure_cooperative - Pure cooperative scheduling" << endl
#ifdef VICE
    << L"          time_slice       - Time sliced scheduling" << endl
#endif //VICE
    << L"          instruction_number_slice " << endl
    << L"                           - Instruction number sliced scheduling" << endl
    << L"     -B classname: Push classname on classes stack." << endl
#endif //VDMPP
    << L"     -W encoding: Set specfiles's encoding." << endl
    << L"     -y : Treat runtime error as specification level exception." << endl
    << L"  -l [-r] [-n] [-N] [-W] specfiles" << endl
    << L"     Pretty print the specfiles " << endl
    << L"     -r: Test coverage coloring" << endl
    << L"     -n,-N: Index of defined, defined and used." << endl
    << L"     -W encoding: Set specfiles's encoding." << endl
    << L"  -c [-r] [-P] [-W] specfiles" << endl
    << L"     Code generate specfiles (C++)." << endl
    << L"     -r: Add position information. " << endl
#ifdef VDMPP
    << L"     -P: Check pre and post conditions " << endl
#endif //VDMPP
    << L"     -W encoding: Set specfiles's encoding." << endl
#ifdef VDMPP
    << L"  -j [-s] [-k] [-u] [-e] [-z package-name] [-P] [-L] [-K classname[{,classname}]] [-U classname[{,classname}]] specfiles" << endl
    << L"     Code generate specfiles (Java)." << endl
    << L"     -s: Code generate only skeletons, except for types. " << endl
    << L"     -u: Code generate only types. " << endl
    //    << L"     -m: Code generate code with \"small\" types (implies the -s option)." << endl
    << L"     -z packagename: Specifies a package name for the generated Java class(es)." << endl
    << L"     -e: Code generates code with concurrency constructs." << endl
    << L"     -k: Code generate pre and post functions/operations. " << endl
    << L"     -P: Check pre and post conditions (implies the -k option)" << endl
    << L"     -L: Code generates integers as longs." << endl
    << L"     -K classname[{,classname}]: Code generates for specified class(es)." << endl
    << L"     -U classname[{,classname}]: Specifies class(es) as interface(s)." << endl
    << L"     -W encoding: Set specfiles's encoding." << endl
#endif //VDMPP
    ;

  vdm_log << flush;
  exit (2);
}
