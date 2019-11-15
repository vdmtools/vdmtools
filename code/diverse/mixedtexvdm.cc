/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/diverse/mixedtexvdm.cc,v $
*  * VERSION
*  *    $Revision: 1.16 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "mixedtexvdm.h"
#include "tb_wstring.h"
#include "tbutils.h"

std::string MIXEDTEXT::MatchMergeStr ("##!!#!");
int MIXEDTEXT::MatchMergeLen = 6;

bool MIXEDTEXT::MatchMerge (int & pm, char ch, std::ostream & ostr, bool code)
{
  if (MatchMergeStr[pm] == ch) {
    if (pm == (MatchMergeLen - 1)) {
      if (code) {
        ostr << "\\begin{vdm}";
      }
      else {
        ostr << "\\end{vdm}";
      }
      pm = 0;
      return true;
    }
    else {
      pm++;
    }
  }
  else if (pm) {
    ostr << MatchMergeStr.substr(0, pm);
    pm = 0;
  }

  if (!pm) {
    ostr.put(ch);
  }
  return false;
}
                    
bool MIXEDTEXT::vdmmerge (const std::wstring & f1,
                          const std::wstring & f2,
                          const std::wstring & f3)
{
  ifstream istr1 (TBWSTR::wstring2fsstr(f1).c_str());
  string str1 (TBWSTR::wstring2string(f1));
  if (!istr1) {
    vdm_log << L"Couldn't open file '" << f1 << L"' for reading\n";
    return false;
  }

  ifstream istr2 (TBWSTR::wstring2fsstr(f2).c_str());
  string str2 (TBWSTR::wstring2string(f2));
  if (!istr2) {
    vdm_log << L"Couldn't open file '" << f2 << L"' for reading\n";
    return false;
  }

  ofstream ostr (TBWSTR::wstring2fsstr(f3).c_str());
  if (!ostr) {
    vdm_log << L"Couldn't open file '" << f3 << L"' for writing\n";
    return false;
  }
  
  char ch;
  int pm = 0;
  bool match = false;
                
  while (!match && istr1.get (ch)) {
    match = MatchMerge  (pm, ch, ostr, true);
  }
  while (!istr1.eof ()) {
    match = false;
    while (!match && istr2.get (ch)) {
      match = MatchMerge  (pm, ch, ostr, false);
    }
    if (istr2.eof ()) {
      ostr << "\\end{vdm}";
    }
    match = false;
    while (!match && istr1.get (ch))
      match = MatchMerge  (pm, ch, ostr, true);
    }

  istr1.close ();
  istr2.close ();
  ostr.close ();
  return true;
}
