// ID
//   $Id: backslash.h,v 1.5 2005/01/21 03:16:06 vdmtools Exp $
// FUNCTION: 
//   Backslashed character and wstring conversion
// COPYRIGHT
//   (c) Kyushu University

#ifndef __backslash_h__
#define __backslash_h__

#include <iostream> 
#include <string>

class Backslashed
{
public:
  static void convert_backslashed(const std::wstring & s,
                                  std::wstring & res,
                                  std::wstring & error); 
  static void convert_internal_to_latex_backslash(const std::wstring & s,
                                                  std::wstring & res, 
                                                  bool textlit); 
  static void convert_internal_to_cxx_backslash(const std::wstring & s,
                                                std::wstring & res);
  static void convert_internal_to_java_backslash(const std::wstring & s,
                                                 std::wstring & res);
  static void convert_internal_to_printed_backslash(const std::wstring & s,
                                                    std::wostream & str, 
                                                    bool textlit); 
private:
  static bool hex_to_number(const std::wstring & s, int len, int & res);
  static bool octal_to_number(const std::wstring & s, int len, int & res);
  static std::wstring number_to_hex2(int i);
  static std::wstring ascii_to_oct(int i);
};
#endif // __backslash_h__
