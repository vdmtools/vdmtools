/***
*  * WHAT
*  *    Interface to utilities used globally in the Toolbox
*  *    (Base layer)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/tbutils.cc,v $
*  * VERSION
*  *    $Revision: 1.85 $
*  * DATE
*  *    $Date: 2006/06/23 05:34:20 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) 2006 CKS, Japan
***/

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/utsname.h>
#endif // _MSC_VER
#include <time.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>

#include "tbutils.h"
#include "asquotes.h"
#include "libman.h"

#ifdef _MSC_VER
#include <windows.h>
#include <direct.h>   // For getcwd()
#include <io.h> // for _mktemp_s
#endif // _MSC_VER

#include "tokenlist.h"   // To get TokenPos
#include "astaux.h"      // To get GetCid
#include "contextinfo.h"       // To get GetCI
#include "projectval.h"

#ifdef VDMPP
#include "SCHDTP.h"
//#include "objtab.h"
#endif //VDMPP
#include "tb_wstring.h"

#include <string>

#ifdef __APPLE_CC__
#include <CoreFoundation/CoreFoundation.h>
#endif // __APPLE_CC__

//contains the file path_name of where vdmde was started originally.
//The search for a file will always start in the standard path.
Sequence TBUTILS::standard_path;

// functions used by logbuf
logbuf::logbuf (int (*Log)(const wchar_t*, int len))
{
  _Log = Log;
  // Allocate a put buffer area. 
  int len = 1024; 
  wchar_t *buf = new wchar_t[len]; 
  setp(buf, buf + len -1); 
}

void logbuf::SetLog (int (*Log) (const wchar_t*, int len))
{
  _Log = Log;
}

void logbuf::ReDirectLog (int (*Log) (const wchar_t*, int len))
{
  _ReDirLog = _Log;
  _Log = Log;
}

void logbuf::RestoreLog()
{
  _Log = _ReDirLog;
}

int logbuf::sync ()
{
  put_buffer();
  return 0; 
}

void logbuf::put_buffer()
{
  if (pbase() != pptr()) {
    int len = (pptr() - pbase());
    _Log (pbase (), len); 
    setp(pbase(), epptr());
  }
}
 
void logbuf::put_char(int chr)
{
  wchar_t ch = chr; 
  _Log(&ch, 1); 
}

logbuf::~logbuf()
{
  sync();
  delete [] pbase();
}

wstreambuf::int_type logbuf::overflow (wstreambuf::int_type ch)
{
  put_buffer(); 
  if (ch != char_traits<wchar_t>::eof()) {
    if (pbase() == epptr())
      put_char(ch);              // No buffer.
    else
      sputc(ch);
  }
  return 0;
}

static int TBLOG (const wchar_t*msg, int len)
{
#ifdef _MSC_VER
  wcout.flush ();
  wcout.imbue(locale(""));
  locale::global(locale("C"));
  wcout.write(msg,len);
  wcout.flush ();
  locale::global(locale(""));
#else
  wcout.flush ();
  std::string msg2 = TBWSTR::wstring2coutstr( std::wstring( msg, len ) );
  cout.write(msg2.c_str(), msg2.size());
  cout.flush ();
#endif // _MSC_VER
  return 1;
}

static int TBERR (const wchar_t*msg, int len) 
{
#ifdef _MSC_VER
  wcout.flush ();
  wcout.imbue(locale(""));
  locale::global(locale("C"));
  wcout.write(msg,len);
  wcout.flush ();
  locale::global(locale(""));
#else
  wcerr.flush ();
  std::string msg2 = TBWSTR::wstring2coutstr( std::wstring( msg, len ) );
  cerr.write(msg2.c_str(), msg2.size());
  cerr.flush();
#endif // _MSC_VER
  return 1;
}

static int TBIPLOG (const wchar_t* msg, int len)
{
#ifdef _MSC_VER
  wcout.flush ();
  wcout.imbue(locale(""));
  locale::global(locale("C"));
  wcout.write(msg,len);
  wcout.flush ();
  locale::global(locale(""));
#else
  wcout.flush ();
  std::string msg2 = TBWSTR::wstring2coutstr( std::wstring( msg, len ) );
  cout.write(msg2.c_str(), msg2.size());
  cout.flush ();
#endif // _MSC_VER
  return 1;
}

logbuf lbuf (TBLOG);
wostream vdm_log (&lbuf);
logbuf ebuf (TBERR);
wostream vdm_err (&ebuf);
logbuf ipbuf (TBIPLOG);
wostream vdm_iplog (&ipbuf);
 
int TBUTILS::remove_file(const std::wstring & s)
{
#ifdef _MSC_VER
  if (_wunlink(s.c_str()) != 0) {
    //perror(L"failed to remove file");
  }
  return 1; 
#else
  if (unlink(TBWSTR::wstring2fsstr(s).c_str()) != 0) {
    //perror(L"failed to remove file");
  }
  return 1; 
#endif // _MSC_VER
}

bool TBUTILS::file_exists(const std::wstring & name) 
{
  wifstream wif (TBWSTR::wstring2fsstr(name).c_str());
  if (wif){
    // File exists
    wif.close();
    return true;
  }
  else 
    return false;
}

bool TBUTILS::file_copy_overwrite(const std::wstring & to, const std::wstring & from)
{
  if (file_exists(to))
    remove_file(to);

  return file_copy(to, from);
}

// Copies the file 'from' into the file 'to'. Returns false if it
// was no possible to open the file 'from', if the file 'to' could
// not be created and if the file 'to' exists already.
bool TBUTILS::file_copy(const std::wstring & to, const std::wstring & from)
{
  FILE *in, *out;
  if( (in = fopen(TBWSTR::wstring2fsstr(from).c_str(), "r")) == NULL )
    return false;

  // Does the file L"to" exist already?
  if( (out = fopen(TBWSTR::wstring2fsstr(to).c_str(), "r")) != NULL ) {
    fclose(out);
    return false;
  }

  if( (out = fopen(TBWSTR::wstring2fsstr(to).c_str(), "w")) == NULL )
    return false;

  int c;
  while((c = getc(in)) != EOF)
    putc(c, out);

  fclose(in);
  fclose(out);
  return true;
}

void TBUTILS::rename_file(const std::wstring & s1, const std::wstring & s2)
{
  if (rename(TBWSTR::wstring2fsstr(s1).c_str(), TBWSTR::wstring2fsstr(s2).c_str()) != 0) {
    std::wstring s; 
    s = L"rename(\"" + s1 + L"\",\"" +s2 + L"\") failed."; 
    wcerr << s.c_str() << endl;
  }
}

std::wstring TBUTILS::ExpandPath (const std::wstring & path, const std::wstring & stnd_path)
{
  if (path.find(L"/") == 0)
    return path;
  else {
    std::wstring res; 
    if (stnd_path == L"" || stnd_path == L".") {
      res = path;
    } else if (stnd_path[stnd_path.length()-1] == L'/') {
      res = stnd_path + path;
    } else {
      res = stnd_path + L"/" + path;
    }
    return res;
  }
}

// tb_JoinPaths.
// These cases are handled:
//  L"A:dir"   + "x.vdm" -> "A:dir\x.vdm"
//  L"A:"      + "x.vdm" -> "A:x.vdm" (not A:\x.vdm which is wrong)
//  L"A:\"     + "x.vdm" -> "A:\x.vdm"
//  L"A:\dir"  + "x.vdm" -> "A:\dir\x.vdm"
//  L"A:\dir\" + "x.vdm" -> "A:\dir\x.vdm"
//  L"A:/dir"  + "x.vdm" -> "A:/dir/x.vdm"
//  L"A:/dir/" + "x.vdm" -> "A:/dir/x.vdm"
//  L"/d"      + "x.vdm" -> "/d/x.vdm"
//  L"/d/"     + "x.vdm" -> "/d/x.vdm"
std::wstring TBUTILS::tb_JoinPaths (const std::wstring & prefix, const std::wstring & name)
{
  if (prefix.empty())
    return name;
  // Now we know prefix is not empty.

#ifdef _MSC_VER
  // Do we have a d: path ? 
  if (prefix.length() == 2 && prefix[1] == ':') {
    return prefix + name;
  }
  // Any trailing separators in prefix?
  std::string::size_type n = prefix.find_last_of(L"\\/");
#else
  // Any trailing separators in prefix?
  std::string::size_type n = prefix.find_last_of(L"/");
#endif // _MSC_VER
  if (n == (prefix.length() - 1)) {
    return prefix + name;
  } else {
    if (n != std::string::npos) {
      // We found a separator in prefix. 
      return prefix + prefix[n] + name;
    } else {
#ifdef _MSC_VER
      return prefix + L'\\' + name;
#else
      return prefix + L'/' + name;
#endif // _MSC_VER
    }
  }
}

void TBUTILS::tb_chdir(const std::wstring & newdir, wostream & wos)
{
#ifdef _MSC_VER
  int res = _chdir(TBWSTR::wstring2fsstr(newdir).c_str());
#else
  int res = chdir(TBWSTR::wstring2fsstr(newdir).c_str());
#endif //_MSC_VER
  if (res != 0)
    wos << L"Unable to change directory to " << newdir << endl;
  else
    wos << L"Changed directory to " << newdir << endl;
}

void TBUTILS::tb_mkdir(const std::wstring & newdir, wostream & wos)
{
  struct stat stbuf;
  if ( stat( TBWSTR::wstring2fsstr(newdir).c_str(), &stbuf ) == -1 )
  {
#ifdef _MSC_VER
    int res = _mkdir(TBWSTR::wstring2fsstr(newdir).c_str());
#else
    int res = mkdir(TBWSTR::wstring2fsstr(newdir).c_str(), 0777);
#endif //_MSC_VER
    if (res != 0)
      wos << L"Unable to make directory ... " << newdir << endl;
    else
      wos << L"Making directory ... " << newdir << endl;
  }
}

std::wstring TBUTILS::tb_getcwd()
{
  char p[1024], *res;
#ifdef _MSC_VER
  if ((res = _getcwd (p, sizeof(p))) != NULL) {
    // Convert backslash in path to forward slash as on Unix. 
    for (char* s = p; s = strchr(s, '\\'); s++) {
      *s = '/';
    }
#else
  if ((res = getcwd (p, sizeof(p))) != NULL) {
#endif // _MSC_VER
    return TBWSTR::fsstr2wstring(std::string(p));
    
  } else {
    return wstring(L"/getcwd/failed!");
  }
}

std::wstring TBUTILS::tb_getbasedir(const std::wstring& path)
{
#ifdef _MSC_VER
  std::string::size_type n = path.find_last_of(L"\\/:");
#else
  std::string::size_type n = path.find_last_of(L"/");
#endif // _MSC_VER
  if (n == std::string::npos) {
    // Did not find any path seps.
    return L".";
  } else {
    // For /x.suf return /
    // For /d/x.suf return /d 
    if (n == 0)
      n = 1; 
#ifdef _MSC_VER
    if ((n == 2) && (path[1] == ':')) // d:/xyz ->d:/
      n = 3;
#endif      
    return path.substr(0, n);
  }
}

SEQ<Char> TBUTILS::tb_getbasename(const std::wstring & path)
{
#ifdef _MSC_VER
  std::string::size_type n = path.find_last_of(L"\\/:");
#else
  std::string::size_type n = path.find_last_of(L"/");
#endif // _MSC_VER 
  if (n == std::string::npos) {
    // Did not find any path seps.
    return SEQ<Char>(path);
  } else {
    // For /x.suf return x.suf
    // For d:/x.suf return x.suf
    return SEQ<Char>(path.substr(n+1));
  }
}

void TBUTILS::ResetSearchPath ()
{
  TBUTILS::standard_path = Sequence();
}

// Input: a directory. 
void TBUTILS::SetDefaultPath (const std::wstring & arg)
{
  SEQ<Char> tk (arg);
  if (! InElems (TBUTILS::standard_path, tk))
    TBUTILS::standard_path.ImpPrepend (tk);
}

void TBUTILS::ShowPath(wostream & wos)
{
  size_t len = TBUTILS::standard_path.Length();
  for (size_t i = 1; i<= len; i++)
    wos << L"  " << SEQ<Char>(TBUTILS::standard_path[i]).GetString()<< endl << flush;
}

void TBUTILS::AddPath(const wstring & path)
{
  SEQ<Char> tk (ExpandPath (path, wstring(SEQ<Char>(TBUTILS::standard_path[1]).GetString ())));
  if (! InElems (TBUTILS::standard_path, tk))
    TBUTILS::standard_path.ImpPrepend (tk);
}

bool TBUTILS::TryOpenFile(const std::wstring & name, wifstream & sp)
{
  sp.open (TBWSTR::wstring2fsstr(name).c_str());
  if (!sp.good ()) {
    sp.close();
    sp.clear();
    return false; 
  } else {
    return true;
  }
}

bool TBUTILS::TryOpenFile(const std::wstring & name, ifstream & sp)
{
  sp.open (TBWSTR::wstring2fsstr(name).c_str());
  if (!sp.good ()) {
    sp.close();
    sp.clear();
    return false; 
  } else {
    return true;
  }
}

bool TBUTILS::TryOpenFile(const std::wstring & name, bool isWide,
                          wifstream & wsp, ifstream & ssp)
{
  if (isWide)
    return TryOpenFile(name, wsp);
  else
    return TryOpenFile(name, ssp);
}

bool TBUTILS::HasPathSpec(const std::wstring & s)
{
#ifdef _MSC_VER
  // Check for d: prefix
  if (s.length()>=2 && s[1]==':')
    return true;
  // This is for WinNT paths. 
  std::string::size_type n = s.find_first_of(L"\\/");
  return (n != std::string::npos);
#else
  // This is for Unix paths. 
  std::string::size_type n = s.find_first_of(L"/");
  return (n != std::string::npos);
#endif // _MSC_VER
}

//#ifdef _MSC_VER
//static wchar_t PATH_SEP = ';';
//#else
//static wchar_t PATH_SEP = ':';
//#endif

std::wstring TBUTILS::tb_Search (const std::wstring & library,
                                 const std::wstring & name)
{
  std::string::size_type index;
  std::wstring path;
  std::wstring lib = library;
  while (lib.length() != 0) {

//    index = lib.find(PATH_SEP);
#ifdef _MSC_VER
    index = lib.find(L';');
#else
    index = lib.find(L':');
#endif // _MSC_VER

    if (index != std::string::npos) {
      path = lib.substr(0,index);
      lib = lib.substr(index+1);
#ifdef _MSC_VER
      path += L'\\' + name;
#else
      path += L'/' + name;
#endif // _MSC_VER
    } else {
#ifdef _MSC_VER
      path = lib + L'\\' + name;
#else
      path = lib + L'/' + name;
#endif // _MSC_VER
      lib = std::wstring(L"");
    }
    
    wifstream pathStream (TBWSTR::wstring2fsstr(path).c_str());
    if (pathStream) {
      pathStream.close();
      return path;
    }
  }

  size_t len = TBUTILS::standard_path.Length();
  for (size_t i = 1; i <= len; i++)
  {
    const SEQ<Char> & seq (TBUTILS::standard_path[i]);
    std::wstring path (seq.GetString());
    wifstream pathStream (TBWSTR::wstring2fsstr(path).c_str());
    if (pathStream) {
      pathStream.close();
      return path;
    }
  }
  return std::wstring(L"");
}

// If the file name is without any path specification then search for
// it using the search paths. Otherwise just try to open it.
// The name of the opened file is returned in fname. 
int TBUTILS::Search_and_OpenFile2(const std::wstring & short_name,
                                  bool isWide,
                                  std::wstring & fname /*ret value*/, 
                                  wifstream & wsp /*ret value*/,
                                  ifstream & ssp /* ret value*/,
                                  int & not_found /*ret value*/) 
{
  Generic g;
  not_found = 1;
  if (TryOpenFile(short_name, isWide, wsp, ssp)) {
    not_found = 0;
    fname = short_name; 
    return not_found;
  }
  // We didn't find the file with short_name
  // If the file is without path prefixes then search for it. 
  if (! HasPathSpec(short_name)) {
    // Now use the path to search for the file. 
    size_t len = TBUTILS::standard_path.Length();
    for (size_t i = 1; i <= len; i++)
    {
      fname = tb_JoinPaths (SEQ<Char>(TBUTILS::standard_path[i]).GetString (), short_name);
      if (TryOpenFile(fname, isWide, wsp, ssp)) {
        not_found = 0;
        break; 
      }
    }
  }
  return not_found; 
}
 
int TBUTILS::Search_and_OpenFile(const std::wstring & short_name,
                                 std::wstring & fname,
                                 wifstream & wsp,
                                 int & not_found)
{
  ifstream ssp;
  return Search_and_OpenFile2(short_name, true, fname, wsp, ssp, not_found);
}

int TBUTILS::Search_and_OpenFile(const std::wstring & short_name,
                                 std::wstring & fname,
                                 ifstream & ssp,
                                 int & not_found)
{
  wifstream wsp;
  return Search_and_OpenFile2(short_name, false, fname, wsp, ssp, not_found);
}

std::wstring TBUTILS::GetAbsolutePath(const std::wstring & filename, const std::wstring & projectfile )
{
  std::wstring ppath = tb_getbasedir( projectfile );
  std::wstring rpath = tb_getbasedir( filename );
  std::wstring apath = rpath;

  std::wstring afile = filename;
  if( ( !rpath.empty() ) && ( rpath[0] == L'.' ) )
  {
    if( !rpath.empty() )
    {
      std::wstring upath = ppath;
      std::wstring dpath;

      std::wstring tpath = rpath.substr( 1 );
      if( !tpath.empty() && ( tpath[0] == L'/' ) )
        tpath = tpath.substr( 1 );

      while( !tpath.empty() )
      {
        std::wstring elem;
        std::string::size_type index = 0;
        if( ( index = tpath.find( L'/' ) ) != std::string::npos )
        {
          elem = tpath.substr( 0, index );
          tpath = tpath.substr( index + 1 );
        }
        else
        {
          elem = tpath;
          tpath = L"";
        }

        if ( elem == L".." )
        {
          if (dpath.empty())
          {
            std::string::size_type idx = upath.find_last_of( L'/' );
            if( idx != std::string::npos )
              upath = upath.substr( 0, idx );
          }
          else
          {
            std::string::size_type idx = dpath.find_last_of( L'/' );
            if( idx != std::string::npos )
              dpath = dpath.substr( 0, idx );
          }
        }
        else
          dpath = dpath + L'/' + elem;
      }

/*
      while( tpath.length() > 0 )
      {
        std::wstring elem = tpath;
        std::string::size_type index = 0;
        if( ( index = tpath.find_last_of( L'/' ) ) != std::string::npos )
        {
          elem = tpath.substr( index + 1 );
        } 
        if( elem == L".." )
        {
          std::string::size_type idx = upath.find_last_of( L'/' );
          if( index != std::string::npos ) upath = upath.substr( 0, idx );
        }
        else
        {
          dpath = L'/' + elem + dpath;
        }
        if( index != std::string::npos )
          tpath = tpath.substr( 0, index );
        else
          tpath = L"";
      }
*/
      apath = upath + dpath;
    }
    afile = apath + L'/' + tb_getbasename( filename ).GetString();
  }
  return afile;
}

std::wstring TBUTILS::GetRelativePath( const std::wstring & filename,
                                       const std::wstring & projectfile )
{
#ifdef _MSC_VER
  if( filename[0] != projectfile[0] ) return filename;
  std::wstring drl = projectfile.substr( 0, 2 );
  std::wstring ppath = tb_getbasedir( projectfile.substr( 2 ) );
  std::wstring apath = tb_getbasedir( filename.substr( 2 ) );
#else
  std::wstring ppath = tb_getbasedir( projectfile );
  std::wstring apath = tb_getbasedir( filename );
#endif // _MSC_VER

  if( ( apath.length() == 1 ) && ( apath[0] == L'/' ) ) apath = L"";

  std::wstring rpath = apath;

  // get common path
  std::wstring cpath = apath;
  while( cpath.length() > 0 )
  {
    if( cpath.length() <= ppath.length() )
    {
      std::wstring p1 = ppath.substr( 0, cpath.length() );
      std::wstring p2 = ppath.substr( cpath.length() );
      if( cpath == p1 )
        if( ( p2.length() == 0 ) || ( p2[0] == L'/' ) ) break;
    }
/*
    if( ppath.find_first_of( cpath ) == 0 )
    {
      if( ( ppath.length() == cpath.length() ) ||
          ( cpath.length() == ppath.find_first_of( L'/', cpath.length() ) ) )
      {
        break;     
      }
    }
    std::string::size_type idx = cpath.find_last_of( L'/' );
    if( idx != std::string::npos )
      cpath = cpath.substr( 0, idx );
*/
    int idx = cpath.length();
    while( ( idx > 0 ) && (cpath[idx] != L'/' ) ) idx--;
    if( idx > 0 )
      cpath = cpath.substr( 0, idx );
    else
      cpath = L"";
  }
  // construct rerative path
  if( cpath.length() > 0 )
  {
    std::wstring dpath = apath.substr( cpath.length() );
    std::wstring upath = ppath.substr( cpath.length() );
    rpath = L".";
    if( upath.length() > 0 )
    {
      std::string::size_type index = 0;
      while( ( index = upath.find_last_of( L'/' ) ) != std::string::npos ) 
      {
        rpath = rpath + L'/' + L"..";
        upath = upath.substr( 0, index );
      }
    }
    if( dpath.length() > 0 )
      rpath = rpath + dpath;
  }
#ifdef _MSC_VER
  else
    rpath = drl + rpath;
#endif // _MSC_VER

  std::wstring rfile = rpath + L'/' + tb_getbasename( filename ).GetString();
  return rfile;
}

bool TBUTILS::InElems (const Sequence & l, const Generic & e)
{  /* Nice utility. MIAN 930415 */
  return l.Elems().InSet(e);
}

bool TBUTILS::ExpectNumericArg(const wchar_t *arg, int & value)
{
   wchar_t *endp; 
   long l = wcstol(arg, &endp, 10); 
           
   if (arg[0] != '\0' && *endp == '\0') {
     value = l;
     return true;
   } else {
     return false;
   }
}

#ifdef _MSC_VER
const std::wstring TBUTILS::MakeWindowsName(const std::wstring & piName)
{
  std::wstring ret = L"";
  for( int i = 0; i < piName.length(); i++ )
  {
    if( L'/' == piName[i] )
      ret += L'\\';
    else
      ret += piName[i];
  }
  return ret; 
}
#else
const std::wstring TBUTILS::MakeWindowsName(const std::wstring & piName)
{
  return piName;
}
#endif //_MSC_VER

std::wstring TBUTILS::GetDateString( const char* format )
{
  time_t t;
  if (time (&t) != -1) {
    char buf [50];
#ifdef _MSC_VER
    setlocale( LC_TIME, "English" );
#else
    setlocale( LC_TIME, "en_US" );
#endif // _MSC_VER
    strftime (buf, 49, format, localtime (&t));
    return TBWSTR::string2wstring(std::string (buf));
  }
  else
  {
    return L"unknown time";
  }
}

std::wstring TBUTILS::GetEnv( const char* name )
{
  const char* env = getenv( name );
  return ( NULL == env ) ? std::wstring(L"") : TBWSTR::string2wstring( std::string(env) );
}

std::wstring TBUTILS::GetHome()
{
#ifdef _MSC_VER
  return GetEnv( "USERPROFILE" );
#else
  return GetEnv( "HOME" );
#endif // _MSC_VER
}

std::wstring TBUTILS::GetCGExt()
{
  std::wstring cgext = GetEnv( "VDMCGEXT" );
  if( 0 == cgext.length() )
  {
#ifdef _MSC_VER
    return std::wstring( L".cpp" );
#else
    return std::wstring( L".cc" );
#endif // _MSC_VER
  }
  else
    return cgext;
}
 
#ifdef _MSC_VER
std::wstring TBUTILS::lcNumeric;  // locale dependant decimal separator
std::wstring TBUTILS::lcGrouping;  // locale dependant decimal separator
#endif // _MSC_VAR

void TBUTILS::setLCNumericEnglish()
{
#ifdef _MSC_VER
  int lnumeric = ::GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, NULL, 0);
  wchar_t* numeric = new wchar_t[lnumeric+1];
  ::GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, numeric, lnumeric);
  lcNumeric = numeric;
  delete[] numeric;

  int lgrouping = ::GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, NULL, 0);
  wchar_t* grouping = new wchar_t[lgrouping+1];
  ::GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, grouping, lgrouping);
  lcGrouping = grouping;
  delete[] grouping;

  ::SetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, L".");
  ::SetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, L"0");
#else
//  setlocale( LC_NUMERIC, "en_US" );
  setlocale( LC_NUMERIC, "C" );
#endif
}

void TBUTILS::restoreLCNumeric()
{
#ifdef _MSC_VER
  ::SetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, lcNumeric.c_str());
  ::SetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, lcGrouping.c_str());
#endif // _MSC_VAR
}

std::wstring TBUTILS::GetTmpDir()
{
  const char * tmpd = getenv( "TMPDIR" );
  if ( NULL != tmpd )
    return TBWSTR::fsstr2wstring(tmpd);

  const char * tmp = getenv( "TMP" );
  if ( NULL != tmp )
    return TBWSTR::fsstr2wstring(tmp);
 
#if defined( __APPLE_CC__ ) || defined( __SunOS__ )
  return TBWSTR::fsstr2wstring(string("/var/tmp"));
#else
  return TBWSTR::fsstr2wstring(string("/tmp"));
#endif
}

std::wstring TBUTILS::CreateTemplate(const std::wstring & prefix)
{
  std::wstring tdir (GetTmpDir());
#ifdef _MSC_VER
  for( int i = 0; i < tdir.length(); i++ )
    if( tdir[i] == L'\\' ) tdir[i] = L'/';
#endif // _MSC_VER
  std::wstring tl = tdir + L"/" + prefix + L"XXXXXX";
  return tl;
}

std::wstring TBUTILS::tb_tempnam(const wchar_t* prefix)
{
#if defined(_MSC_VER)
  std::wstring ret;
  for (int i = 0; i < 100; i++)
  {
    wstring newprefix (prefix);
    newprefix += Int(i).ascii();
    std::wstring tpl = CreateTemplate(newprefix);
    size_t len_tpl = tpl.length() + 1;
    wchar_t * tbuf = new wchar_t[ len_tpl ];
    //wcscpy( tbuf, tpl.c_str() );
    wcscpy_s( tbuf, len_tpl, tpl.c_str() );
    wchar_t * res = _wmktemp( tbuf );
    if( NULL != res ) ret = res;
    delete[] tbuf;
    if (ret.size() > 0) break;
  }
  return ret;
#else 
  // for Linux, Mac OS X, Solaris
  std::wstring tpl = CreateTemplate(wstring(prefix));
  std::string stpl = TBWSTR::wstring2string(tpl);
  size_t len_stpl = stpl.length() + 1;
  char * tbuf = new char[ len_stpl ];
  //strcpy( tbuf, stpl.c_str() );
  memcpy( tbuf, stpl.c_str(), len_stpl );
  int fd = mkstemp( tbuf );
  std::wstring ret (TBWSTR::string2wstring(tbuf));
  delete[] tbuf;

  if( -1 == fd ) return std::wstring(L"");
  close( fd );
  return ret;
#endif // _MSC_VER
}

Set TBUTILS::CheckForLockedFiles(const Set & file_s)
  // Tries to lock all the specification files. The function returns
  // the set of files that it was _not_ possible to lock.
{
#ifdef _MSC_VER
  Set not_locked;
  Generic gc;
  for (bool bb = file_s.First(gc); bb; bb = file_s.Next(gc))
  {
    std::wstring fn (PTAUX::ExtractFileName (TYPE_ProjectTypes_FileName (gc)).c_str());
    HANDLE f = CreateFileW( fn.c_str(),
                   GENERIC_READ | GENERIC_WRITE,
                   0 /* not shared */,
                   NULL /* security attrib */,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL, NULL);
    if(f == INVALID_HANDLE_VALUE)
      // Unable to open file for sharing.
      not_locked.Insert(gc);
    else
      CloseHandle(f);
  }
  return not_locked;
#else
  return Set();
#endif // _MSC_VER
}

std::wstring TBUTILS::GetDefaultCodePage()
{
#ifdef _MSC_VER
  return Int(GetACP()).ascii();
#else
  return L"en_US.UTF-8";
#endif // _MSC_VER
}

// ConvRealLit
// text : seq of char
// ==> bool * [real]
Tuple TBUTILS::ConvRealLit(const char * text)
{
  char * ptr = NULL;
#ifdef _MSC_VER
  if (text == NULL) return mk_(Bool(false), Nil());
/*
  std::string str (text);
  if ((str.length() > 2) &&
      (str[0] == '0') &&
      ((str[1] == 'x') || (str[1] == 'X')))
  {
    int num = 0;
    int len = str.length();
    for (int i = 2; i < len; i++)
    {
      num = num * 16;
      char ch = str[i];
      if (ch >= '0' && ch <= '9')
        num += (ch - '0');
      else if (ch >= 'a' && ch <= 'f')
        num += (ch - 'a') + 10;
      else if (ch >= 'A' && ch <= 'F')
        num += (ch - 'A') + 10;
      else {
        return mk_(Bool(false), Nil());
      }
    }
    return mk_(Bool(true), Real(num));
  }
*/
  if (strlen(text) > 2 ) {
    if ((*text == '0') && ((*(text+1) == 'x') || (*(text+1) == 'X')))
    {
      double hexres = (double)_strtoui64(text, & ptr, 16);
      if ((text != ptr) && (*ptr == '\0'))
        return mk_(Bool(true), Real(hexres));
    }
  }
#endif // _MSC_VER

//  char * ptr = NULL;
//  long double res = strtold(text, & ptr);
  double res = strtod(text, & ptr);
  if ((text != ptr) && (*ptr == '\0'))
    return mk_(Bool(true), Real(res));
  else
    return mk_(Bool(false), Nil());
}

// Used by the typecheker and code generator (err.cc) to
// indicate number of type errors and warnings
int TBUTILS::no_of_errors = 0;
int TBUTILS::no_of_warnings = 0;

void TBUTILS::ClearErrorsCount()
{
  TBUTILS::no_of_errors = 0;
  TBUTILS::no_of_warnings = 0;
}

void TBUTILS::IncrementErrorsCount()
{
  TBUTILS::no_of_errors++;
}

int TBUTILS::GetErrorsCount()
{
  return TBUTILS::no_of_errors;
}

void TBUTILS::IncrementWarningsCount()
{
  TBUTILS::no_of_warnings++;
}

int TBUTILS::GetWarningsCount()
{
  return TBUTILS::no_of_warnings;
}

bool TBUTILS::isJapanese()
{
  return (TBUTILS::getLanguage() == L"ja_JP");  
}

wstring TBUTILS::getLanguage()
{
#if defined _MSC_VER
  int langID = GetUserDefaultLangID();
  int mainID = langID & 0xFF;
  int subID = (langID & 0xFF00) >> 8;
  wstring lang;
  switch( mainID )
  {
    case LANG_AFRIKAANS:                  lang = L"af_ZA"; break;
    case LANG_ALBANIAN:                   lang = L"sq_AL"; break;
    case LANG_ARABIC:
    {
      switch( subID )
      {
        case SUBLANG_ARABIC_SAUDI_ARABIA: lang = L"ar_SA"; break;
        case SUBLANG_ARABIC_IRAQ:         lang = L"ar_IQ"; break;
        case SUBLANG_ARABIC_EGYPT:        lang = L"ar_EG"; break;
        case SUBLANG_ARABIC_LIBYA:        lang = L"ar_LY"; break;
        case SUBLANG_ARABIC_ALGERIA:      lang = L"ar_DZ"; break;
        case SUBLANG_ARABIC_MOROCCO:      lang = L"ar_MA"; break;
        case SUBLANG_ARABIC_TUNISIA:      lang = L"ar_TN"; break;
        case SUBLANG_ARABIC_OMAN:         lang = L"ar_OM"; break;
        case SUBLANG_ARABIC_YEMEN:        lang = L"ar_YE"; break;
        case SUBLANG_ARABIC_SYRIA:        lang = L"ar_SY"; break;
        case SUBLANG_ARABIC_JORDAN:       lang = L"ar_JO"; break;
        case SUBLANG_ARABIC_LEBANON:      lang = L"ar_LB"; break;
        case SUBLANG_ARABIC_KUWAIT:       lang = L"ar_KW"; break;
        case SUBLANG_ARABIC_UAE:          lang = L"ar_AE"; break;
        case SUBLANG_ARABIC_BAHRAIN:      lang = L"ar_BH"; break;
        case SUBLANG_ARABIC_QATAR:        lang = L"ar_QA"; break;
        default: break;
      }
      break;
    }
    case LANG_ARMENIAN:                   lang = L"hy_AM"; break;
    case LANG_ASSAMESE:                   lang = L"";      break;
    case LANG_AZERI:                      lang = L"az_AZ"; break;
    case LANG_BASQUE:                     lang = L"eu_ES"; break;
    case LANG_BELARUSIAN:                 lang = L"be_BY"; break;
    case LANG_BENGALI:                    lang = L"bn_IN"; break;
    case LANG_BULGARIAN:                  lang = L"bg_BG"; break;
    case LANG_CATALAN:                    lang = L"ca_ES"; break;
    case LANG_CHINESE:
    {
      switch( subID )
      {
        case SUBLANG_CHINESE_TRADITIONAL: lang = L"zh_TW"; break;
        case SUBLANG_CHINESE_SIMPLIFIED:  lang = L"zh_CN"; break;
        case SUBLANG_CHINESE_HONGKONG:    lang = L"zh_HK"; break;
        case SUBLANG_CHINESE_SINGAPORE:   lang = L"zh_SG"; break;
        default:                          lang = L"zh_CN"; break;
      }
      break;
    }
//    case LANG_CROATIAN:                   lang = L"hr_HR"; break;
    case LANG_CZECH:                      lang = L"cs_CZ"; break;
    case LANG_DANISH:                     lang = L"da_DK"; break;
    case LANG_DUTCH:
    {
      switch( subID )
      {
        case SUBLANG_DUTCH:               lang = L"nl_NL"; break;
        case SUBLANG_DUTCH_BELGIAN:       lang = L"nl_BE"; break;
        default:                          lang = L"nl_NL"; break;
      }
      break;
    }
    case LANG_ENGLISH:
    {
      switch( subID )
      {
        case SUBLANG_ENGLISH_US:          lang = L"en_US"; break;
        case SUBLANG_ENGLISH_UK:          lang = L"en_GB"; break;
        case SUBLANG_ENGLISH_AUS:         lang = L"en_AU"; break;
        case SUBLANG_ENGLISH_CAN:         lang = L"en_CA"; break;
        case SUBLANG_ENGLISH_NZ:          lang = L"en_NZ"; break;
        case SUBLANG_ENGLISH_EIRE:        lang = L"en_US"; break;
        case SUBLANG_ENGLISH_SOUTH_AFRICA: lang = L"en_ZA"; break;
        case SUBLANG_ENGLISH_JAMAICA:     lang = L"en_US"; break;
        case SUBLANG_ENGLISH_CARIBBEAN:   lang = L"en_US"; break;
        case SUBLANG_ENGLISH_BELIZE:      lang = L"en_US"; break;
        case SUBLANG_ENGLISH_TRINIDAD:    lang = L"en_US"; break;
        case SUBLANG_ENGLISH_PHILIPPINES: lang = L"en_PH"; break;
        case SUBLANG_ENGLISH_ZIMBABWE:    lang = L"en_ZW"; break;
        default:                          lang = L"en_US"; break;
      }
      break;
    }
    case LANG_ESTONIAN:                   lang = L"et_EE"; break;
    case LANG_FAEROESE:                   lang = L"fo_FO"; break;
    case LANG_FARSI:                      lang = L""; break;
    case LANG_FINNISH:                    lang = L"fi_FI"; break;
    case LANG_FRENCH:
    {
      switch( subID )
      {
        case SUBLANG_FRENCH:              lang = L"fr_FR"; break;
        case SUBLANG_FRENCH_BELGIAN:      lang = L"fr_BE"; break;
        case SUBLANG_FRENCH_CANADIAN:     lang = L"fr_CA"; break;
        case SUBLANG_FRENCH_SWISS:        lang = L"fr_CH"; break;
        case SUBLANG_FRENCH_LUXEMBOURG:   lang = L"fr_LU"; break;
        case SUBLANG_FRENCH_MONACO:       lang = L"fr_FR"; break;
        default:                          lang = L"fr_FR"; break;
      }
      break;
    }
    case LANG_GEORGIAN:                   lang = L"ka_GE"; break;
    case LANG_GERMAN:
    {
      switch( subID )
      {
        case SUBLANG_GERMAN:              lang = L"de_DE"; break;
        case SUBLANG_GERMAN_SWISS:        lang = L"de_CH"; break;
        case SUBLANG_GERMAN_AUSTRIAN:     lang = L"de_AT"; break;
        case SUBLANG_GERMAN_LUXEMBOURG:   lang = L"de_LU"; break;
        case SUBLANG_GERMAN_LIECHTENSTEIN: lang = L"de_DE"; break;
        default:                          lang = L"de_DE"; break;
      }
      break;
    }
    case LANG_GREEK:                      lang = L"el_GR"; break;
    case LANG_GUJARATI:                   lang = L""; break;
    case LANG_HEBREW:                     lang = L"he_IL"; break;
    case LANG_HINDI:                      lang = L"hi_IN"; break;
    case LANG_HUNGARIAN:                  lang = L"hu_HU"; break;
    case LANG_ICELANDIC:                  lang = L"is_IS"; break;
    case LANG_INDONESIAN:                 lang = L"id_ID"; break;
    case LANG_ITALIAN:
    {
      switch( subID )
      {
        case SUBLANG_ITALIAN:             lang = L"it_IT"; break;
        case SUBLANG_ITALIAN_SWISS:       lang = L"it_CH"; break;
        default:                          lang = L"it_IT"; break;
      }
      break;
    }
    case LANG_JAPANESE:                   lang = L"ja_JP"; break;
    case LANG_KANNADA:                    lang = L""; break;
    case LANG_KASHMIRI:                   lang = L""; break;
    case LANG_KAZAK:                      lang = L""; break;
    case LANG_KONKANI:                    lang = L""; break;
    case LANG_KOREAN:                     lang = L"ko_KR"; break;
    case LANG_LATVIAN:                    lang = L"lv_LV"; break;
    case LANG_LITHUANIAN:
    {
      switch( subID )
      {
        case SUBLANG_LITHUANIAN:          lang = L"lt_LT"; break;
#if (_MSC_VER < 1400 )
        case SUBLANG_LITHUANIAN_CLASSIC:  lang = L"lt_LT"; break;
#endif
        default:                          lang = L"lt_LT"; break;
      }
      break;
    }
    case LANG_MACEDONIAN:                 lang = L"mk_MK"; break;
    case LANG_MALAY:
    {
      switch( subID )
      {
        case SUBLANG_MALAY_BRUNEI_DARUSSALAM: lang = L"ms_MY"; break;
        case SUBLANG_MALAY_MALAYSIA:      lang = L"ms_MY"; break;
        default:                          lang = L"ms_MY"; break;
      }
      break;
    }
    case LANG_MALAYALAM:                  lang = L""; break;
    case LANG_MANIPURI:                   lang = L""; break;
    case LANG_MARATHI:                    lang = L"mr_IN"; break;
    case LANG_NEPALI:                     lang = L""; break;
    case LANG_NEUTRAL:                    lang = L""; break;
    case LANG_NORWEGIAN:
    {
      switch( subID )
      {
        case SUBLANG_NORWEGIAN_BOKMAL:    lang = L"no_NO"; break;
        case SUBLANG_NORWEGIAN_NYNORSK:   lang = L"nn_NO"; break;
        default:                          lang = L"no_NO"; break;
      }
      break;
    }
    case LANG_ORIYA:                      lang = L""; break;
    case LANG_POLISH:                     lang = L"pl_PL"; break;
    case LANG_PORTUGUESE:
    {
      switch( subID )
      {
        case SUBLANG_PORTUGUESE:          lang = L"pt_PT"; break;
        case SUBLANG_PORTUGUESE_BRAZILIAN: lang = L"pt_BR"; break;
        default:                          lang = L"pt_PT"; break;
      }
      break;
    }
    case LANG_PUNJABI:                    lang = L""; break;
    case LANG_ROMANIAN:                   lang = L"ro_RO"; break;
    case LANG_RUSSIAN:                    lang = L"ru_RU"; break;
    case LANG_SANSKRIT:                   lang = L""; break;
    case LANG_SERBIAN:
    {
      switch( subID )
      {
        case SUBLANG_SERBIAN_LATIN:       lang = L""; break;
        case SUBLANG_SERBIAN_CYRILLIC:    lang = L"sr_YU"; break;
//    case LANG_CROATIAN:                   lang = L"hr_HR"; break;
        default:                          lang = L"hr_HR"; break;
      }
      break;
    }
    case LANG_SINDHI:                     lang = L""; break;
    case LANG_SLOVAK:                     lang = L"sk_SK"; break;
    case LANG_SLOVENIAN:                  lang = L"sl_SL"; break;
    case LANG_SPANISH:
    {
      switch( subID )
      {
        case SUBLANG_SPANISH:             lang = L"es_ES"; break;
        case SUBLANG_SPANISH_MEXICAN:     lang = L"es_MX"; break;
        case SUBLANG_SPANISH_MODERN:      lang = L"es_ES"; break;
        case SUBLANG_SPANISH_GUATEMALA:   lang = L"es_GT"; break;
        case SUBLANG_SPANISH_COSTA_RICA:  lang = L"es_CR"; break;
        case SUBLANG_SPANISH_PANAMA:      lang = L"es_PA"; break;
        case SUBLANG_SPANISH_DOMINICAN_REPUBLIC: lang = L"es_DO"; break;
        case SUBLANG_SPANISH_VENEZUELA:   lang = L"es_VE"; break;
        case SUBLANG_SPANISH_COLOMBIA:    lang = L"es_CO"; break;
        case SUBLANG_SPANISH_PERU:        lang = L"es_PE"; break;
        case SUBLANG_SPANISH_ARGENTINA:   lang = L"es_AR"; break;
        case SUBLANG_SPANISH_ECUADOR:     lang = L"es_EC"; break;
        case SUBLANG_SPANISH_CHILE:       lang = L"es_CL"; break;
        case SUBLANG_SPANISH_URUGUAY:     lang = L"es_UY"; break;
        case SUBLANG_SPANISH_PARAGUAY:    lang = L"es_PY"; break;
        case SUBLANG_SPANISH_BOLIVIA:     lang = L"es_BO"; break;
        case SUBLANG_SPANISH_EL_SALVADOR: lang = L"es_SV"; break;
        case SUBLANG_SPANISH_HONDURAS:    lang = L"es_HN"; break;
        case SUBLANG_SPANISH_NICARAGUA:   lang = L"es_NI"; break;
        case SUBLANG_SPANISH_PUERTO_RICO: lang = L"es_PR"; break;
        default:                          lang = L"es_ES"; break;
      }
      break;
    }
    case LANG_SWAHILI:                    lang = L""; break;
    case LANG_SWEDISH:
    {
      switch( subID )
      {
        case SUBLANG_SWEDISH:             lang = L"sv_SE"; break;
        case SUBLANG_SWEDISH_FINLAND:     lang = L"sv_FI"; break;
        default:                          lang = L"sv_SE"; break;
      }
      break;
    }
    case LANG_TAMIL:                      lang = L"ta_IN"; break;
    case LANG_TATAR:                      lang = L"tt_RU"; break;
    case LANG_TELUGU:                     lang = L"te_IN"; break;
    case LANG_THAI:                       lang = L"th_TH"; break;
    case LANG_TURKISH:                    lang = L"tr_TR"; break;
    case LANG_UKRAINIAN:                  lang = L"uk_UA"; break;
    case LANG_URDU:
    {
      switch( subID )
      {
        case SUBLANG_URDU_INDIA:          lang = L"ur_IN"; break;
        case SUBLANG_URDU_PAKISTAN:       lang = L"ur_PK"; break;
        default:                          lang = L"ur_PK"; break;
      }
      break;
    }
    case LANG_UZBEK:
    {
      switch( subID )
      {
        case SUBLANG_UZBEK_CYRILLIC:      lang = L"uz_UZ"; break;
        case SUBLANG_UZBEK_LATIN:         lang = L"uz_UZ"; break;
        default:                          lang = L"uz_UZ"; break;
      }
      break;
    }
    case LANG_VIETNAMESE:                 lang = L"vi_VN"; break;
    default: break;
  }
  return lang;
#else // Linux & Solaris
#ifdef __APPLE_CC__
#if (__APPLE_CC__ < 5493)
  CFLocaleRef ref = CFLocaleCopyCurrent();
  if (ref != NULL)
  {
    CFStringRef str = CFLocaleGetIdentifier(ref);
    wstring res (string2wstring(CFStringGetCStringPtr( str, kCFStringEncodingUTF8 )));
    CFRelease(ref);
    return res;
  } 
#else
  CFLocaleRef ref = CFLocaleCopyCurrent();
  if (ref != NULL)
  {
    CFStringRef str = CFLocaleGetIdentifier(ref);
    //return string2wstring(CFStringGetCStringPtr( str, kCFStringEncodingUTF8 ));
    //CFIndex len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), kCFStringEncodingUTF8);
    wstring darwin_locale;
    //CFIndex len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), kCFStringEncodingMacRoman);
    CFIndex len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), kCFStringEncodingUTF8);
    if (len > 0)
    {
      char* buf = new char[ len + 1 ];  
      bool ok = CFStringGetCString(str, buf, len, kCFStringEncodingUTF8);
      //bool ok = CFStringGetCString(str, buf, len, kCFStringEncodingMacRoman);
      if (ok)
      {
        darwin_locale = string2wstring(string(buf)); 
      }
      delete [] buf;
    }
    CFRelease(ref);
    if (darwin_locale.size() > 0)
      return darwin_locale;
  }
#endif // (__APPLE_CC__ < 5493)
#endif // __APPLE_CC__
  string lang;
  const char * l = getenv( "LANG" );
  if( NULL == l ) return string2wstring(lang);
  lang = string(l);

  std::string::size_type pos = lang.find('.');
  if (pos != string::npos)
    return string2wstring(lang.substr(0, pos));
  else
    return string2wstring(lang);
#endif
}

uint64_t TBUTILS::GetCurTimeMil()
{
#ifdef _MSC_VER
  FILETIME ft;
  unsigned __int64 tm = 0;
  //uint64_t tm = 0;
  GetSystemTimeAsFileTime(&ft);
  tm |= ft.dwHighDateTime;
  tm <<= 32;
  tm |= ft.dwLowDateTime;
  tm /= 10;  /*convert into microseconds*/
  return tm;
#else
  timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;
#endif // _MSC_VER
}

Generic TBUTILS::GetOSName()
{
#ifdef _MSC_VER
  OSVERSIONINFO versionInfo;
  SYSTEM_INFO sysInfo;

  ZeroMemory(&versionInfo, sizeof(OSVERSIONINFO));
  versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&versionInfo);

  ZeroMemory(&sysInfo, sizeof(SYSTEM_INFO));
  GetSystemInfo(&sysInfo);

  wstring sysname (L"Windows");
  wstring release (Int(versionInfo.dwBuildNumber).ascii());
  wstring version (Int(versionInfo.dwMajorVersion).ascii() + L"." + Int(versionInfo.dwMinorVersion).ascii());

  char nodenm[256];
  if (gethostname(nodenm, 256) != 0) 
  {
    if (WSAGetLastError() == WSANOTINITIALISED) { // WinSock not initialized
        WSADATA WSAData;
        WSAStartup(MAKEWORD(1, 0), &WSAData);
        gethostname(nodenm, 256);
        WSACleanup();
    }
    else
      //return WSAGetLastError();
      return Nil();
  }
  wstring nodename (TBWSTR::string2wstring(nodenm));

  wstring machine;
  switch(sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:  { machine = L"x86_64"; break; }
    case PROCESSOR_ARCHITECTURE_IA64:   { machine = L"ia64"; break; }
    case PROCESSOR_ARCHITECTURE_INTEL:  { machine = L"x86"; break; }
    case PROCESSOR_ARCHITECTURE_UNKNOWN: 
    default:                            { machine = L"unknown"; break; }
  }
  return mk_(Sequence(sysname), Sequence(nodename), Sequence(release), Sequence(version), Sequence(machine));
#else
  struct utsname buff;

  if (uname(&buff) == 0)
  {
    string sysname (buff.sysname);
    string nodename (buff.nodename);
    string release (buff.release);
    string version (buff.version);
    string machine (buff.machine);
    return mk_(Sequence(TBWSTR::string2wstring(sysname)),
               Sequence(TBWSTR::string2wstring(nodename)),
               Sequence(TBWSTR::string2wstring(release)),
               Sequence(TBWSTR::string2wstring(version)),
               Sequence(TBWSTR::string2wstring(machine)));
  }
#endif // _MSC_VER
  return Nil();
}
