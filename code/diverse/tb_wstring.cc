#include "tb_wstring.h"
#include "metaiv.h"

#include <string>

#ifdef _MSC_VER
#include "windows.h"
#else
#include "iconv.h"
#define VDMENCODING "VDMENCODING"
#endif // _MSC_VER

#if defined( __linux__ ) || defined( __CYGWIN__ )
#include <string.h>
#include <stdlib.h>
#endif // __linux__ || __CYGWIN__

std::wstring TBWSTR::vdmcodepage;
std::wstring TBWSTR::vdmencoding;

std::wstring TBWSTR::mbstr2wstr(const std::string& str, bool utf8)
{
  if (str.empty()) {
    return std::wstring( L"" );
  }
  std::string s(str);
#ifndef _MSC_VER
  if ( !utf8 ) {
    try {
      s = convertToUTF8( str, getFileCode() );
    }
    catch(...) {}
  }
#endif // _MSC_VER

  std::string::size_type len = s.size();
  const char *src = s.c_str();
  wchar_t* buf = new wchar_t[len + 1];
  wmemset( buf, L'\0', len );
#ifdef _MSC_VER
  std::string codepage (getFileCode());
  int size = 0;
  if( utf8 || codepage == "65001" ) {// UTF-8
    size = MultiByteToWideChar( CP_UTF8, 0, src, len, buf, len );	
  }
  else {
    if (!codepage.empty()) {
      string lang = "." + codepage;
      setlocale( LC_CTYPE, lang.c_str() );
    }
    size = mbsrtowcs(buf, &src, len, NULL);
  }
  //std::wstring res(buf, buf + size);
  std::wstring res(buf, size);
#else
  setUTF8Locale();
  int size = mbsrtowcs(buf, &src, len, NULL);
  std::wstring res(buf, size);
#endif // _MSC_VER
  delete[] buf;
  return res;  
}

std::string TBWSTR::wstr2mbstr(const std::wstring& ws, bool utf8)
{
  if (ws.empty()) {
    return string( "" );
  }
  size_t len = MB_CUR_MAX * ((2 * ws.length()) + 1);
  char* mbcs = new char[ len + 1 ];
  memset(mbcs,'\0',len);
  const wchar_t *src = ws.c_str();
#ifdef _MSC_VER
  std::string codepage (getFileCode());
  if( utf8 || codepage == "65001" ) { // UTF-8
    WideCharToMultiByte( CP_UTF8, 0, src, ws.size(), mbcs, len, NULL, NULL );	
  }
  else {
    if (!codepage.empty()) {
      string lang = "." + codepage; 
      setlocale( LC_CTYPE, lang.c_str() );
    }
    wcsrtombs(mbcs, &src, len, NULL); 
  }
  std::string res (mbcs);
#else
  setUTF8Locale();
  wcsrtombs(mbcs, &src, len, NULL); 
  std::string res (mbcs);
  if ( !utf8 ) {
    try {
      res = convertFromUTF8( res, getFileCode() );  
    }
    catch(...) {}
  }
#endif //_MSC_VER
  delete[] mbcs;
  return res;
}

//
// string2wstring
//
// This function is used throughout the toolbox to convert an
// arbitrary ascii string in which unicode characters are represented
// using \uXXXX into a wstring in which each such unicode character is
// converted to the corresponding wide char.
std::wstring TBWSTR::string2wstring(const std::string& s)
{
  return hexquadstring2wstring(s);
}

//
// MakeAsciiWString 
//
// This function converts an ascii string into a wstring such that
// each character in the resulting wstring is still an ascii character
// i.e. the \uXXXX unicode representation is preserved (instead of
// being converted into a wide char). This is necessary because in
// some cases a string representing an expression to be passed to the
// parser is constructed.
std::wstring TBWSTR::MakeAsciiWstring(const std::string& s)
{
#ifndef _MSC_VER
  std::wstring ws(s.begin(), s.end());
  return ws;
#else
  std::wstring res;
  for (std::string::const_iterator it = s.begin(); it != s.end(); it++) {
    res.push_back(btowc(*it));
  }
  return res;
#endif //_MSC_VER
}

std::string TBWSTR::wstring2mbstr(const std::wstring& s)
{
  return wstr2mbstr( s, false );
}

std::string TBWSTR::wstring2coutstr(const std::wstring& s)
{
  std::string res (wstring2utf8str( s ));

#ifndef _MSC_VER
  try {
    res = convertFromUTF8( res, getConsoleCode() );  
  }
  catch(...){}
#endif //_MSC_VER

  return res;
}

std::string TBWSTR::wstring2utf8str(const std::wstring & s)
{
  return wstr2mbstr( s, true );
}

std::string TBWSTR::wstring2string(const std::wstring & ws)
{
  return ConvertToHexquad(ws);
}

wchar_t TBWSTR::unhexquad(const char* hexquad)
{
  return (wchar_t)strtoul(hexquad, (char**) NULL, 16);
}

wchar_t TBWSTR::unhexquadw(const wchar_t* hexquad)
{
  return (wchar_t)wcstoul(hexquad, (wchar_t**) NULL, 16);
}

std::wstring TBWSTR::hexquadstring2wstring(const std::string & hqs)
{
  std::wstring ws;
  std::string::size_type i = 0;
  size_t len_hqs = hqs.size();
  while(i < len_hqs) {
    wchar_t wc = btowc(hqs[i]);
    if ((i+1 < len_hqs) && (wc == L'\\') && (hqs[i+1] == '\\')) {
      ws.push_back(wc);
      ws.push_back(btowc(hqs[i+1]));
      i = i + 2;
    }
    else if ((i+5 < len_hqs) && (wc == L'\\') && (hqs[i+1]=='u')) {
      std::string str(hqs.substr(i+2, 4));
      ws.push_back(unhexquad(str.c_str()));
      i = i + 6;
    }
    else {
      ws.push_back(wc);
      i++;
    }
  }
  return ws;
}

std::wstring TBWSTR::hexquadwstring2wstring(const std::wstring & hqs)
{
  std::wstring ws;
  std::string::size_type i = 0;
  size_t len_hqs = hqs.size();
  while(i < len_hqs) {
    wchar_t wc = hqs[i];
    if ((i+1 < len_hqs) && (wc == L'\\') && (hqs[i+1] == L'\\')) {
      ws.push_back(wc);
      ws.push_back(hqs[i+1]);
      i = i + 2;
    }
    else if ((i+5 < len_hqs) && (wc == L'\\') && (hqs[i+1]==L'u')) {
      ws.push_back(unhexquadw(hqs.substr(i+2, 4).c_str()));
      i = i + 6;
    }
    else {
      ws.push_back(wc);
      i++;
    }
  }
  return ws;
}

Sequence TBWSTR::hexquadseq2wseq(const Sequence & hqs)
{
  Sequence header (L"\\u");
  Sequence ws;
  int64_t len_hqs = hqs.Length();
  int64_t i = 1;
  while(i <= len_hqs) {
    if (i == len_hqs) {
      ws.ImpAppend(hqs[i++]);
    }
    else if ((i+5 <= len_hqs) && (hqs.SubSequence(i, i+1) == header)) {
      std::wstring subseqStr (hqs.SubSequence(i+2, i+6).GetString());
      ws.ImpAppend(Char(unhexquadw(subseqStr.c_str())));
      i = i + 6;
    }
    else {
      ws.ImpAppend(hqs[i++]);
    }
  }
  return ws;
}

std::string TBWSTR::ConvertToHexquad(const std::wstring & ws)
{
  std::string result;
  char converted[9];
  for (std::wstring::const_iterator it = ws.begin(); it != ws.end(); it++) {
    unsigned short wc = *it;
    if(wc < 0x007F) {
      result.append(1, (char) wc);
    }
    else {
      sprintf(converted, "\\u%04X", wc);
      result.append(std::string(converted));
    }
  }
  return result;
}

//
// Convertion of multibyte string to unicode wstring 
//
std::wstring TBWSTR::mbstr2wstring(const std::string& s)
{
  return mbstr2wstr( s, false );
}

std::wstring TBWSTR::cinstr2wstring(const std::string& s)
{
  if( s.size() == 0 ) {
    return std::wstring( L"" );
  }
#ifdef _MSC_VER
  return mbstr2wstr( s, false );
#else
  std::string instr(s); 
  try {
    instr = convertToUTF8( instr, getConsoleCode() );
  }
  catch(...){}
  return utf8str2wstring( instr );
#endif // _MSC_VER
}

std::wstring TBWSTR::utf8str2wstring(const std::string& s)
{
  return mbstr2wstr( s, true );
}

std::string TBWSTR::unicode2rtf(const std::wstring& unistr)
{
  std::string res;
  for (std::string::size_type i = 0; i < unistr.length(); i++) {
    wchar_t wc = unistr[i];
    if (wc < 128) {
      res.push_back(wc);
    }
    else {
      std::wstring ws;
      ws.push_back(wc);
      std::string mbstr (wstring2mbstr(ws));
      for ( std::string::size_type j = 0; j < mbstr.length(); j++ ) {
        res.append("\\'");
        char hexadec[3]; 
        sprintf(hexadec, "%2x", 0x000000FF & (unsigned char)mbstr[j]);        
        res.append(hexadec, strlen(hexadec));
      }
    }
  }
  return res;
}

//
// Convertion of string with \uXXXX to the string 
// where a '\u-ed' char is a sequence of 
// its hexadecimal bytes in the national multibyte encoding
// (each byte is preceeded by \').
//
std::string TBWSTR::hexquard2rtfstring(const std::string& hqstr)
{
  return unicode2rtf(hexquadstring2wstring(hqstr));
}

//
// Convertion of multibyte string to the string 
// where a non-ascii char is 
// its hexadecimal byte preceeded by \'.
//
std::string TBWSTR::mbstr2rtfstring(const std::string& str)
{
  return unicode2rtf(mbstr2wstring(str));
}

//
//
//
//
//
std::string TBWSTR::convertCrToNl(const std::string& line )
{
  std::string::size_type len_line = line.length();
  std::string ret;
  if( len_line > 0 ) {
    for( std::string::size_type i = 0; i < len_line; i++ ) {
      char c = line.at(i);
      if( c == '\x0d' ) {
        if( i < len_line - 1 ) {
          ret.push_back('\n');
        }
      } 
      else {
        ret.push_back(c);
      } 
    } 
  } 
  return ret;
} 

std::wstring TBWSTR::getIOCharSetEnv()
{
  if (TBWSTR::vdmencoding.empty()) {
#ifdef _MSC_VER
    TBWSTR::vdmencoding = Int(GetACP()).ascii();
#else
    std::wstring encoding;
    const char* vdmenv = getenv( VDMENCODING );
    if( vdmenv != NULL ) {
      encoding = string2wstring(vdmenv);
    }
    else {
      const char * langenv = getenv( "LANG" );
      if( langenv != NULL ) {
        encoding = string2wstring(langenv);
      }
      else {
        encoding = wstring(L"en_US.UTF-8");
      }
    }
#ifdef __SunOS__
    if( encoding == L"ja" ) {
      encoding = L"ja_JP.eucJP";
    }
#endif // __SunOS__
    TBWSTR::vdmencoding = encoding; 
#endif // _MSC_VER
  }
  return TBWSTR::vdmencoding;
}

void TBWSTR::setIOCharSetEnv( const std::wstring & encoding )
{
  TBWSTR::vdmencoding = encoding;
}

std::wstring TBWSTR::getCodePageEnv()
{
  return TBWSTR::vdmcodepage;
}

void TBWSTR::setCodePageEnv( const std::wstring & encoding )
{
  TBWSTR::vdmcodepage = encoding;
}

std::string TBWSTR::getFileCode()
{
#ifdef _MSC_VER
  return wstring2string( getIOCharSetEnv() );
#else
  std::string filecode;
  std::string encoding (wstring2string( getIOCharSetEnv() ));

  std::string::size_type index = 0;
  if( ( index = encoding.find_last_of( "." ) ) != string::npos ) {
    filecode = encoding.substr( index + 1 );
  }
  else {
    filecode = encoding;
  }
  return filecode;
#endif // _MSC_VER
}

std::string TBWSTR::getConsoleCode()
{
#ifdef _MSC_VER
  return wstring2string( getIOCharSetEnv() ); // dummy
#else
  std::string code;
  std::string lang;
  const char* l = getenv( "LANG" );
  if( NULL != l ) {
    lang = l;
  }
#ifdef __SunOS__
  if( lang == "ja" ) {
    lang = "ja_JP.eucJP";
  }
#endif // __SunOS__
  std::string::size_type index = 0;
  if( ( index = lang.find_last_of( "." ) ) != string::npos ) {
    code = lang.substr( index + 1 );
  }
  if( code.empty() ) {
    code = "UTF8";
  }
  return code;
#endif // _MSC_VER
}

#ifndef _MSC_VER
void TBWSTR::setUTF8Locale()
{
  std::string lc_ctype( "en_US.UTF-8" );
#ifndef __SunOS__
  std::string lang (wstring2string( getIOCharSetEnv() ));
  std::string::size_type index = 0;
  if( ( index = lang.find_last_of( "." ) ) != string::npos ) {
    lc_ctype = lang.substr( 0, index ) + "." + "UTF-8";
  }
#endif // __SunOS__
  setlocale( LC_CTYPE, lc_ctype.c_str() );
}

std::string TBWSTR::convertToUTF8(const std::string& fromstr, const std::string& code )
{
  string utf8( "UTF-8" );
  string ret;
  if( !convertCode( fromstr, code, ret, utf8 ) ) {
    throw "ERROR";
  }
  return ret;
}

std::string TBWSTR::convertFromUTF8(const std::string& fromstr, const std::string& code )
{
  string utf8( "UTF-8" );
  string ret;
  if( !convertCode( fromstr, utf8, ret, code ) ) {
    throw "ERROR";
  }
  return ret;
}

bool TBWSTR::convertCode(const std::string& fromstr,
                         const std::string& fromcode,
                         std::string& tostr,
                         const std::string& tocode )
{
  // \x5c(backslash) \x7e(tilde)
  if( ( fromstr.length() == 0 ) || ( fromcode.length() == 0 ) ||
      ( tocode.length() == 0 ) || ( fromcode == tocode ) ) {
    tostr = fromstr;
  }
  else if( fromcode == "SJIS" ) {
    std::string tmpstr;
    if( !convWithIConv( fromstr, fromcode, tmpstr, tocode ) ) {
      return false;
    }
    tostr = "";
    std::string::size_type index = 0;
    std::string::size_type len_tmpstr = tmpstr.length();
    while( index < len_tmpstr ) {
      char c = tmpstr[index];
      if( ( index < len_tmpstr - 1 ) &&
          ( '\xc2' == c ) && ( '\xa5' == tmpstr[index + 1] ) ) {
         tostr.push_back('\x5c');
         index += 2;
      }
      else if( ( index < len_tmpstr - 2 ) &&
               ( '\xe2' == c ) &&
               ( '\x80' == tmpstr[index + 1] ) &&
               ( '\xbe' == tmpstr[index + 2] ) ) {
         tostr.push_back('\x7e');
         index += 3;
      }
      else {
        tostr.push_back(c);
        index++;
      }
    }
  }
  else if ( tocode == "SJIS" ) {
    tostr = "";
    std::string tmpstr = "";
    std::string::size_type index = 0;
    std::string::size_type len_fromstr = fromstr.length();
    while ( index < len_fromstr ) {
      char c = fromstr[ index ];
      if ( ( '\x5c' == c ) || ( '\x7e' == c ) ) {
        if ( !tmpstr.empty() ) {
          std::string buf;
          if ( !convWithIConv( tmpstr, fromcode, buf, tocode ) ) return false;
          tostr.append(buf);
          tmpstr = "";
        }
        tostr.push_back(c);
      }
      else {
        tmpstr.push_back(c);
      }
      index++;
    }
    if ( !tmpstr.empty() ) {
      std::string buf;
      if ( !convWithIConv( tmpstr, fromcode, buf, tocode ) ) {
        return false;
      }
      tostr.append(buf);
    }
  }
  else {
    if ( !convWithIConv( fromstr, fromcode, tostr, tocode ) ) {
      return false;
    }
  }
  return true;
}

bool TBWSTR::convWithIConv(const std::string& fromstr,
                           const std::string& fromcode,
                           std::string& tostr,
                           const std::string& tocode )
{
  if ( fromstr.empty() || ( fromcode == tocode ) ) {
    tostr = fromstr;
    return true;
  }

  bool ret = false;
  size_t inbytesleft = fromstr.length();
  size_t outbytesleft = inbytesleft * 3;
  char* tmpibuf = new char[ inbytesleft + 1 ];
  char* tmpobuf = new char[ outbytesleft + 1 ];
  memcpy( tmpibuf, fromstr.c_str(), inbytesleft + 1 );
  memset( tmpobuf, '\0', outbytesleft + 1 );

#if (defined( __APPLE_CC__ ) && (__APPLE_CC__ < 5493)) || defined( __SunOS__ ) || defined( __FreeBSD__ )
  const char * ibuf = tmpibuf;
#else
  char * ibuf = tmpibuf;
#endif
  char * obuf = tmpobuf;

  iconv_t cd = iconv_open( tocode.c_str() , fromcode.c_str() );
  if ( cd != (iconv_t)(-1) ) {
    iconv( cd, &ibuf, &inbytesleft, &obuf, &outbytesleft );
    iconv_close( cd );
    tostr = tmpobuf;
    ret = ( inbytesleft == 0 );
  }
  delete[] tmpibuf;
  delete[] tmpobuf;
  return ret;
}
#endif // _MSC_VER

std::wstring TBWSTR::wstring2wcoutstr( const std::wstring& ws )
{
  if ( ws.empty() ) {
    return wstring(L"");
  }

  std::string s = wstring2mbstr( ws );
  std::wstring res;
  for (std::string::const_iterator it = s.begin(); it != s.end(); it++) {
    res.push_back(( *it + 256 )%256);
  }
  return res;
}

std::string TBWSTR::wstring2fsstr( const std::wstring& s )
{
#ifdef _MSC_VER
  std::wstring curiocharset (getIOCharSetEnv());
  setIOCharSetEnv(getCodePageEnv()); 
  std::string res (wstring2mbstr( s ));
  setIOCharSetEnv(curiocharset); 
  return res;
#else
  return wstring2utf8str( s );
#endif // _MSC_VER
}

std::wstring TBWSTR::fsstr2wstring( const std::string& s )
{
#ifdef _MSC_VER
  std::wstring curiocharset (getIOCharSetEnv());
  setIOCharSetEnv(getCodePageEnv()); 
  std::wstring res (mbstr2wstring( s ));
  setIOCharSetEnv(curiocharset); 
  return res;
#else
  return utf8str2wstring( s );
#endif // _MSC_VER
}

int TBWSTR::hiansi2sjis( int c )
{
  if ( c < 0x80 ) {
    return c;
  }
  switch (c) {
    case 0x89: {
      return 0x81F1;
    }
    default: {
      return 0x8148;
    }
  }
}

int TBWSTR::hiansi2utf8( int c )
{
  if ( c < 0x80 ) {
    return c;
  }
  else if( c < 0xC0 ) {
    return 0xc2 * 256 + c % 256;
  }
  else {
    return 0xc3 * 256 + c % 256;
  }
}

std::string TBWSTR::hiansi2string( int c )
{
  std::string code (getFileCode());

  int a = 0;
#ifdef _MSC_VER
  if( code == "932" ) {
#else
  if( code == "SJIS" ) {
#endif // _MSC_VER
    a = hiansi2sjis( c );
  }
  else {
    a = hiansi2utf8( c ); 
  }
  std::string res;
  if( (a / 256) > 0 ) {
    res.push_back(a/256);
  }
  res.push_back(a % 256);
  return res;
}

std::string TBWSTR::vdm_getline(std::istream & ifs)
{
  string s;
  while ((ifs.peek() != EOF) && (ifs.peek() != '\r') && (ifs.peek() != '\n')) {
    s.push_back(ifs.get());
  }
  if (ifs.peek() != EOF) {
    char c = (char)ifs.get();
    if ((c == '\r') && ((ifs.peek() != EOF) && (ifs.peek() == '\n')))
      ifs.get();
  }
  return s;
}

