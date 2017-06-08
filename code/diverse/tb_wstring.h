#ifndef __tb_wstring_h__
#define __tb_wstring_h__

#include "metaiv.h"
#include <fstream>
#include <string>

class TBWSTR
{
private:
  static std::wstring vdmcodepage;
  static std::wstring vdmencoding;

public:
  static std::string wstring2mbstr(const std::wstring & s);
  static std::wstring mbstr2wstring(const std::string & s);
  static std::string wstring2coutstr(const std::wstring & s);
  static std::wstring cinstr2wstring(const std::string & s);
  static std::wstring string2wstring(const std::string &);
  static std::wstring MakeAsciiWstring(const std::string &);
  static std::string wstring2string(const std::wstring &);
  static std::wstring hexquadstring2wstring(const std::string & hqs);
  static std::wstring hexquadwstring2wstring(const std::wstring & hqs);
  static Sequence hexquadseq2wseq(const Sequence & hqs);
  static std::string ConvertToHexquad(const std::wstring & ws);
  static void hexquad(int c, char ** converted);
  static std::string mbstr2rtfstring(const std::string & str);
  static std::string hexquard2rtfstring(const std::string & hqstr);
  static std::wstring wstring2wcoutstr( const std::wstring & ws );
  static std::string wstring2utf8str( const std::wstring & s );
  static std::wstring utf8str2wstring( const std::string & s );
  static std::string wstring2fsstr( const std::wstring & s );
  static std::wstring fsstr2wstring( const std::string & s );
  static std::string convertCrToNl( const std::string & line );
  static std::wstring getIOCharSetEnv();
  static void setIOCharSetEnv( const std::wstring & encoding );
  static std::wstring getCodePageEnv();
  static void setCodePageEnv( const std::wstring & encoding );
  static int hiansi2sjis( int c );
  static int hiansi2utf8( int c );
  static std::string hiansi2string( int c );

  static std::string vdm_getline(std::istream & ifs);

  static std::string getFileCode();
  static std::string getConsoleCode();

#ifndef _MSC_VER
  static void setUTF8Locale();
  static bool convertCode( const std::string & fromstr,
                           const std::string & fromcode,
                           std::string& tostr,
                           const std::string & tocode );
  static bool convWithIConv( const std::string & fromstr,
                             const std::string & fromcode,
                             std::string& tostr,
                             const std::string & tocode );
  static std::string convertToUTF8( const std::string & fromstr,
                                    const std::string & code );
  static std::string convertFromUTF8( const std::string & fromstr,
                                      const std::string & code );
#endif // _MSV_VER

private:
  static std::wstring mbstr2wstr(const std::string& str, bool utf8);
  static std::string wstr2mbstr(const std::wstring& s, bool utf8);
  static unsigned long unhexquad(const char* hexquad);
  static unsigned long unhexquadw(const wchar_t* hexquad);
  static std::string unicode2rtf(const std::wstring& unistr);
};
#endif //__tb_wstring_h__
