// Header file for command-line parser
// $Id: cli_options.h,v 1.3 2001/09/06 10:09:56 mikhail Exp $

#ifndef __cli_options_h__
#define __cli_options_h__

#include "metaiv.h"
#include "ProjectTypes.h"
#include <string>

typedef enum { FLAG_SYNTAXCHECK = 0,
               FLAG_INTERPRETER,
               FLAG_TYPECHECKER,
               FLAG_CPPCG,
               FLAG_JAVACG,
               FLAG_PRETTYPRINT,
               FLAG_INTERNAL,
	       FLAG_POG,
               FLAG_POGTEST,
               FLAG_EXTERNAL} PrimaryFlagType;

class TBOptions {
 public:
  virtual bool IsBatchMode() = 0;
  virtual bool IsInternalTest() = 0;
  virtual bool IsCLI() = 0;
  virtual ~TBOptions(){};
};

class GUIOptions : public TBOptions {
 public:
  bool IsBatchMode(){ return false; }
  bool IsInternalTest(){ return false; }
  bool IsCLI() { return false; }
};

class CLIOptions : public TBOptions {

 public:
  CLIOptions();
  ~CLIOptions(){};
  bool ProcessOptions(int argc, char** argv);
  bool IsBatchMode();
  void Usage();

  PrimaryFlagType GetPrimaryFlag();
  std::wstring GetCoverageFile();

  bool IsInternalTest(); // -a flag
  std::wstring GetResultFile(); // -O flag
  bool SaveTempFile(); // -w flag
  bool IsDefTypeCheck(); // -d flag
  std::wstring GetOptionalScript(); // -o flag
  bool QuitToolBox(); // -q flag
  bool IsSetException(); // -y flag

#ifdef VDMPP
  std::wstring GetClassNameForPush();
  std::wstring GetPriorityFile();

  std::wstring GetJavaPackageName();
  std::wstring GetJavaClassNames();
  bool JavaSkeletonsOnly();
  bool JavaTypesOnly();
  bool JavaGenPrePost();
  bool JavaGenConc();
  bool JavaGenSmall();
  std::wstring JavaGetInterfaces();
  bool JavaVDMPrefix();
#endif //VDMPP

  SEQ<TYPE_ProjectTypes_FileName> GetFileList();
  int GetOptionsIndex();

#ifdef VICE
  std::wstring GetTimeFile();
#endif //VICE
  bool IsCLI() { return true; }

 private:
  bool batchMode;
  PrimaryFlagType primary_flag;
  std::wstring coverage_file;
  std::wstring output_file;
  std::wstring package_name;
  std::wstring class_names;
  std::wstring interfaceList;
  int optionsIndex;
  std::wstring class_name;
  std::wstring script_file;
#ifdef VDMPP
  std::wstring priority_file;
#ifdef VICE
  std::wstring time_file;
#endif //VICE
#endif //VDMPP
  SEQ<TYPE_ProjectTypes_FileName> file_l;

  bool CheckNumberOfArguments(int,int);
  bool ExpectNumericOptArg(const wchar_t *arg, wchar_t opt, int &value);
};

extern TBOptions *cliOpt;

#endif //__cli_options_h__

