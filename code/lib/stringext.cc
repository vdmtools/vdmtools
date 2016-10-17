/***
 *  * WHAT
 *  *    
 *  * FILE
 *  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/stringext.cc,v $
 *  * VERSION
 *  *    $Revision: 1.12 $
 *  * DATE
 *  *    $Date: 2006/06/08 07:23:13 $
 *  * STATUS
 *  *    $State: Exp $
 *  * REFERENCES
 *  *    
 *  * PROJECT
 *  *    IDERS/AFRODITE
 *  * AUTHOR
 *  *    Erik Toubro Nielsen + $Author: vdmtools $
 *
 *  * COPYRIGHT
 *  *    (C) Kyushu University
 ***/

#include "stringext.h"
#include <string>

const wchar_t * STR_RXwhite_and_comma = L" \t\n,"; 
const wchar_t * STR_RXwhite = L" \t\n"; 

int STR_split(const std::wstring & s,
              std::wstring * Sp, int Sn, const wchar_t*p) 
{
  int str_no = 0; 
  string::size_type max = s.length();
  std::wstring sep(p); 

  string::size_type i,j;
  for (j = 0 ; j < max && 
          (i = s.find_first_not_of(sep, j)) != string::npos; ) {
    j = s.find_first_of(sep, i);
    if (j == string::npos)
      j = max;
    if (str_no < Sn)
      Sp[str_no++] = s.substr(i, j-i);
  } 
  return str_no; 
}

Sequence STR_split(const std::wstring & s, const wchar_t * p) 
{
//  int str_no = 0; 
  string::size_type max = s.length();
  std::wstring sep(p); 
  Sequence ret;

  string::size_type i,j;
  for (j = 0 ; j<max && 
          (i = s.find_first_not_of(sep, j)) != string::npos; ) {
    j = s.find_first_of(sep, i);
    if (j == string::npos)
      j = max;
//    if (str_no < Sn)
//      Sp[str_no++] = s.substr(i, j-i);
    ret.ImpAppend(Sequence(s.substr(i, j-i)));
  } 
  return ret; 
}

