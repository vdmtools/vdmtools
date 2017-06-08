/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/diverse/mixedtexvdm.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:05 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mixedtexvdm_h__
#define __mixedtexvdm_h__

#include <string>
#include <iostream>

class MIXEDTEXT
{
public:
  static bool vdmmerge (const std::wstring & f1, const std::wstring & f2, const std::wstring & f3);

private:
  static bool MatchMerge (int & pm, char ch, std::ostream & ostr, bool code);

  static std::string MatchMergeStr;
  static int MatchMergeLen;

};

#endif // __mixedtexvdm_h__

