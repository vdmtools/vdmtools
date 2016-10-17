// ID
//   $Id: backslash.cc,v 1.16 2006/06/26 05:37:38 vdmtools Exp $
// FUNCTION:
//   Backslashed character and wstring conversion
// COPYRIGHT
//   (c) Kyushu University

// Supported escape conventions:
//   \\  -> '\'
//   \r  -> return wchar_t
//   \n  -> newline wchar_t
//   \t  -> tab wchar_t
//   \f  -> form feed wchar_t
//   \e      escape wchar_t
//   \a          alarm (bell)
//   \xYZ -> wchar_t with hex rep YZ (e.g., \x41 -> A)
//   \cY    control wchar_t Y (\c A == \x01
//   \033        octal wchar_t
//   \L"   ->  "
//   \'   ->  '


// See also SequenceVal::ostream_out in metaiv.cc for printing of Sequence
// values.

#include "metaiv.h"
#include "backslash.h"
//#include <ctype.h>
//#include <stdio.h>
#include <sstream>
#include <string>
#include "tb_wstring.h"
 
bool Backslashed::hex_to_number(const std::wstring & s, int len, int & res)
{
  res = 0;
  for (int i = 0; i < len; i++) {
    res = res * 16;
    wchar_t ch = s[i];
    if (ch >= L'0' && ch <= L'9')
      res += (ch - L'0');
    else if (ch >= L'a' && ch <= L'f')
      res += (ch - L'a') + 10; // 20060626 +10
    else if (ch >= L'A' && ch <= L'F')
      res += (ch - L'A') + 10; // 20060626 +10
    else {
      res = i;
      return false;
    }
  }
  return true;
}

bool Backslashed::octal_to_number(const std::wstring & s, int len, int & res)
{
  res = 0;
  for (int i = 0; i < len; i++) {
    res = res * 8;
    wchar_t ch = s[i];
    if (ch >= L'0' && ch <= L'7')
      res += (ch - L'0');
    else {
      res = i;
      return false;
    }
  }
  return true;
}

// Convert wstring s containing backslashed chars to translated wstring, res
// Any errors are appended to wstring error.

void Backslashed::convert_backslashed(const std::wstring & s, std::wstring & res, std::wstring & error)
{
  res = L"";
  error = L"";
  res.reserve(s.length());
  std::wstring::size_type l = s.length();
  for (std::wstring::size_type i = 0; i < l; i++) {
    if (s[i] == '\\') {
      i++;
      if (i < l) {
        wchar_t c = s[i]; 
        switch (c) {
          case L'U':
          case L'u':
            { int ndig=c==L'u'?4:8;
              res += L"\\";
              res += c;
              i++;
              if(i+ndig<=l) {
                res += s.substr(i,ndig);
                i += (ndig-1);
              } else {
                error += L"Wrong universal character. Expected four/eight hex digits after \\u/U \n";
                res += L'?';
              }
            }
          break;

        case L'n':  res += L'\n'; break;
        case L'r':  res += L'\r'; break;
        case L'a':  res += L'\a'; break;
        case L'b':  res += L'\b'; break;
        case L't':  res += L'\t'; break;
        case L'v':  res += L'\v'; break;
        case L'e':  res += L'\x1b'; break;
        case L'f':  res += L'\f'; break;
        case L'\\': res += L'\\'; break;
          // ' is allowed to be backslashed for convenience with
          // syntax coloring editors
        case L'\'': res += c; break;
          // it must be allowed to escape " otherwise it cannot be
          // included in a text literal.
        case L'\"': res += c; break;
        case L'c':
          {
            i++;
            if (i<l) {
              res += (wchar_t)(toupper(s[i]) ^ 64);
            } else {
              res += L'?';
              error += L"Missing character after \\c\n";
            }
          }
          break;

        case L'x':
          {
            i++;
            if (i+1<l) {
              int hexres;
              std::wstring hx (s.substr(i,2));
              if (hex_to_number(hx, 2, hexres)) {
                res += (wchar_t)hexres;
              } else {
                res += L'?';
                error += L"Illegal hex character in: \\x";
                error += s.substr(i,2);
                error += L"\n";
              }
              i += 1;
            } else {
              error += L"Two missing hex digits after backslash (\\).\n";
              res += L'?';
            }
          }           
          break;

        case L'0': case L'1': case L'2': case L'3':
          {
            if (i+2<l) {
              int octal;
              std::wstring hx (s.substr(i,3));
              if (octal_to_number(hx, 3, octal)) {
                res += (wchar_t)octal;
              } else {
                res += L'?';
                error += L"Illegal octal character in: \\";
                error += s.substr(i,3);
                error += L"\n";
              }
              i += 2;
            } else {
              error += L"Two missing hex digits after backslash (\\).\n";
              res += L'?';
            }
          }                   
          break;
         
        default:
          res += L'?';
          error += L"Invalid backslashed character: \\";
          error += c;
          error += L"\n";
          break;
        }
      } else {
        res += L'?';
        error += L"Missing character after backslash (\\).\n";
      }
    } else {
      res += s[i];
    }
  }
}

std::wstring Backslashed::number_to_hex2(int i)
{
  return Int(i).ascii();
}

std::wstring Backslashed::ascii_to_oct(int i)
{
  char buf[100];
//  ::sprintf(buf, "%0.3o", i); // 20051219
  ::sprintf(buf, "%03o", i);

  std::wstring res (TBWSTR::string2wstring(buf));
  // Replace space with 0
  for (std::wstring::size_type j = 0, Max = res.length(); j < Max; j++) {
    if (res[j] == L' ') {
      res[j] = L'0';
    }
  }

  return (res);
}

void Backslashed::convert_internal_to_latex_backslash(const std::wstring&s,
                                                      std::wstring & res,
                                                      bool textlit)
{
  // textlit is true if we are creating a text literal
  // false if we are creating a wchar_t literal
  // There is a difference since " should be escaped in a textliteral but
  // does not need it in a wchar_t literal.
  res = L"";
  const std::wstring latex_backslash(L"{\\backslash}");
  std::wstringstream str;
  std::wstring::size_type l = s.length();
  for (std::wstring::size_type i = 0; i < l; i++) {
    wchar_t c = s[i]; 

    switch (c) {
      case L'\n':    str << latex_backslash << L"n"; break;
      case L'\r':    str << latex_backslash << L"r"; break;
      case L'\a':    str << latex_backslash << L"a"; break;
      case L'\b':    str << latex_backslash << L"b"; break;
      case L'\t':    str << latex_backslash << L"t"; break;
      case L'\v':    str << latex_backslash << L"v"; break;
      case L'\x1b':  str << latex_backslash << L"e"; break;
      case L'\f':    str << latex_backslash << L"f"; break;
      case L'\177':  str << latex_backslash << L"c?"; break;
      case L'\\':    str << latex_backslash << latex_backslash; break;

      case L'\"':
        if (textlit)
          str << latex_backslash << L"\\Dquote ";
        else
          str <<  L"\\Dquote ";
        break;

      case L'\'': str << L"\\Quote "; break;

      case L' ': str << L"\\hspace{0.5em}"; break;
      case L'#': str << L"\\#"; break;
      case L'$': str << L"\\hbox{\\textdollar}"; break;
      case L'%': str << L"\\%"; break;
      case L'&': str << L"\\&"; break;
      case L'<': str << L"\\hbox{\\textless}"; break;
      case L'>': str << L"\\hbox{\\textgreater}"; break;
      case L'^': str << L"\\hbox{\\textasciicircum}"; break;
      case L'_': str << L"\\hbox{\\textunderscore}"; break;
      case L'{': str << L"\\hbox{\\textbraceleft}"; break;
      case L'}': str << L"\\hbox{\\textbraceright}"; break;
      case L'~': str << L"\\hbox{\\textasciitilde}"; break;

      default: {
        if (c < 32) {
          str << latex_backslash << L"x" << number_to_hex2(c) << L" ";
        } else {
          str << c;
        }
        break;
      }
    } 
  }
  res.assign(str.str());
  //  str.rdbuf()->freeze(0);
}

void Backslashed::convert_internal_to_cxx_backslash(const std::wstring&s,
                                                    std::wstring & res)
{
  res = L"";
  std::wstringstream str;
  std::wstring::size_type l = s.length();
  for (std::wstring::size_type i = 0; i < l; i++) {
    wchar_t c = s[i]; 
    switch (c) {
      case L'\n':  str << L"\\n"; break;
      case L'\r':  str << L"\\r"; break;
      case L'\a':  str << L"\\a"; break;
      case L'\b':  str << L"\\b"; break;
      case L'\t':  str << L"\\t"; break;
      case L'\v':  str << L"\\v"; break;
      case L'\f':  str << L"\\f"; break;

      case L'\"':  str << L"\\\""; break;
      case L'\'':  str << L"\\\'"; break;
      case L'\\':  str << L"\\\\"; break;
      default:
     
        if (c<32
            // Dec cxx V6.0-021 cannot parse embedded wchar_t with ascii value 255
            || c==(wchar_t)'\xff'
            ) {
          str << L"\\x" << number_to_hex2(c);
        } else {
          str << c;
        }
        break;
    }
  }
  res.assign(str.str());
  //  str.rdbuf()->freeze(0);
}

void Backslashed::convert_internal_to_java_backslash(const std::wstring&s,
                                                     std::wstring & res)
{
  res = L"";
  std::wstringstream str;
  std::wstring::size_type l = s.length();
  for (std::wstring::size_type i = 0; i < l; i++) {
    wchar_t c = s[i]; 
    switch (c) {
      case L'\n':  str << L"\\n"; break;
        //    case L'\r':  str << L"\\r"; break;
        //    case L'\a':  str << L"\\a"; break;
        //    case L'\b':  str << L"\\b"; break;
      case L'\t':  str << L"\\t"; break;
        //    case L'\v':  str << L"\\v"; break;
        //    case L'\f':  str << L"\\f"; break;

      case L'\r':  str << L"\\r"; break; // 20121220

      case L'\"':  str << L"\\\""; break;
      case L'\'':  str << L"\\\'"; break;
      case L'\\':  str << L"\\\\"; break;

      default:
        if (c<32
            // Dec cxx V6.0-021 cannot parse embedded wchar_t with ascii value 255
            || c==(wchar_t)'\xff'
            ) {
          str << L"\\" << ascii_to_oct(c);
        } else {
          str << c;
        }
        break;
    }
  }
  res.assign(str.str());
}

// Internal function for VDM library.

// Tcl
// cannot output \000
// outputs \r as newline

void Backslashed::convert_internal_to_printed_backslash(const std::wstring&s,
                                                        std::wostream & ostr,
                                                        bool textlit)
{
  // textlit is true if we are creating a text literal
  // false if we are creating a wchar_t literal
  // There is a difference since " should be escaped in a textliteral but
  // does not need it in a wchar_t literal.
  std::wstring::size_type l = s.length();
  std::wstring str = L"";
  for (std::wstring::size_type i = 0; i < l; i++) {
    wchar_t c = s[i]; 

    switch (c) {
      case L'\n':    str.append(1, c); break;
      case L'\r':    str += L"\\r"; break;
      case L'\a':    str += L"\\a"; break;
      case L'\b':    str += L"\\b"; break;
      case L'\t':    str +=c; break;
      case L'\v':    str += L"\\v"; break;
      case L'\x1b':  str += L"\\e"; break;
      case L'\f':    str += L"\\f"; break;
      case L'\177':  str += L"\\c?"; break;
      case L'\\':    str += L"\\\\"; break;

      case L'\"':
        if (textlit)
          str += L"\\\"";
        else
          str +=  L"\"";
        break;

      case L'\'': str += L"'"; break;

      default:
        //      if (c<32) {
        //        str << L"\\x" << number_to_hex2(c);
        //      } else {
          str.append(1, c);
          //      }
        break;
    }
  }
  ostr << str.c_str();
}
