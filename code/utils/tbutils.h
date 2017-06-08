/***
*  * WHAT
*  *    Interface to utilities used globally in the Toolbox
*  *    (Base layer)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/tbutils.h,v $
*  * VERSION
*  *    $Revision: 1.55 $
*  * DATE
*  *    $Date: 2006/03/09 01:57:32 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#ifndef __tbutils_h__
#define __tbutils_h__

#include "metaiv.h"

#include <fstream>
#include <string>

#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif // _MSC_VER

class logbuf : public wstreambuf {
  int (*_Log) (const wchar_t*, int len); 
  int (*_ReDirLog) (const wchar_t*, int len); 
  void put_buffer(); 
  void put_char(int chr); 
public:
  logbuf (int (*Log)(const wchar_t*, int len));
  ~logbuf(); 
  void SetLog (int (*Log) (const wchar_t*, int len));

  void ReDirectLog (int (*Log) (const wchar_t*, int len));
  void RestoreLog();

  // ReDirectLog() and RestoreLog() allows one to temporarily redirect
  // the log. Use ReDirectLog() to set the temp log function and
  // RestoreLog() to restore to the original log. (OS)

  int sync ();
  wstreambuf::int_type overflow (wstreambuf::int_type ch);
};

// Log and Erros buffers/outstreams unsed in the Toolbox
extern logbuf lbuf;
extern wostream vdm_log;
extern logbuf ebuf;
extern wostream vdm_err;
extern logbuf ipbuf;
extern wostream vdm_iplog;

class TBUTILS
{
public:
  static int remove_file(const std::wstring & s);
  static void rename_file(const std::wstring & s1, const std::wstring & s2);
  static bool file_exists(const std::wstring & name);
  static bool file_copy_overwrite(const std::wstring & to, const std::wstring & from);
  static bool file_copy(const std::wstring & to, const std::wstring & from);
  static std::wstring ExpandPath (const std::wstring & path, const std::wstring & stnd_path);
  static std::wstring tb_JoinPaths (const std::wstring & path, const std::wstring & filename);
  static std::wstring tb_Search (const std::wstring & library, const std::wstring & name);
  static bool HasPathSpec(const std::wstring & s);
  static void ResetSearchPath ();
  static void SetDefaultPath (const std::wstring & arg);
  static void ShowPath(wostream & wos);
  static void AddPath(const wstring & path);
  static int Search_and_OpenFile(const std::wstring & short_name, std::wstring & fname,
                                 ifstream & sp, int & not_found);
  static int Search_and_OpenFile(const std::wstring & short_name, std::wstring & fname,
                                 wifstream & wsp, int & not_found);
  static void tb_chdir(const std::wstring & newdir, wostream & wos);
  static void tb_mkdir(const std::wstring & newdir, wostream & wos);
  static std::wstring tb_getcwd();
  static std::wstring tb_getbasedir(const std::wstring & dir); 
  static SEQ<Char> tb_getbasename(const std::wstring & name); 
  static bool InElems(const Sequence &, const Generic &);
  static bool ExpectNumericArg(const wchar_t *, int &);
  static std::wstring GetDateString( const char* format);
  static std::wstring GetEnv( const char* name );
  static std::wstring GetHome();
  static std::wstring GetCGExt();
  static void setLCNumericEnglish();
  static void restoreLCNumeric();
  static std::wstring GetAbsolutePath( const std::wstring & filename, const std::wstring & projectfile );
  static std::wstring GetRelativePath( const std::wstring & filename, const std::wstring & projectfile );
  static std::wstring tb_tempnam(const wchar_t* prefix);
  static Set CheckForLockedFiles(const Set & file_s);
  static std::wstring GetDefaultCodePage();

  static const std::wstring MakeWindowsName(const std::wstring & piName);
  static Tuple ConvRealLit(const char *);

  static void ClearErrorsCount();
  static void IncrementErrorsCount();
  static int GetErrorsCount();
  static void IncrementWarningsCount();
  static int GetWarningsCount();

  static bool isJapanese();
  static wstring getLanguage();
  
  static uint64_t GetCurTimeMil();
  static Generic GetOSName();
private:
  static bool TryOpenFile(const std::wstring & name, wifstream & sp);
  static bool TryOpenFile(const std::wstring & name, ifstream & sp);
  static bool TryOpenFile(const std::wstring & name, bool isWide, wifstream & wsp, ifstream & ssp);
  static int Search_and_OpenFile2(const std::wstring & short_name,
                                  bool isWide,
                                  std::wstring & fname /*ret value*/,
                                  wifstream & wsp /*ret value*/,
                                  ifstream & ssp /* ret value*/,
                                  int & not_found /*ret value*/);
  static std::wstring GetTmpDir();
  static std::wstring CreateTemplate(const std::wstring & prefix);

  // contains the file path_name of where vdmde was started originally.
  // The search for a file will always start in the standard path.
  static Sequence standard_path;

  static int no_of_errors;
  static int no_of_warnings;

#ifdef _MSC_VER
  static std::wstring lcNumeric;  // locale dependant decimal separator
  static std::wstring lcGrouping;  // locale dependant decimal separator
#endif // _MSC_VAR
};

#endif // __tbutils_h__
