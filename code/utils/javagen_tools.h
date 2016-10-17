/***
*  * WHAT
*  *    Top level interface to the tools for manipulating java source files
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/javagen_tools.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2001/11/30 12:44:09 $
*  * AUTHOR
*  *   $Author: mikhail $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __JAVAGEN_TOOLS_H__
#define __JAVAGEN_TOOLS_H__

#include "metaiv.h"
#include "CPP.h"
#include "ProjectTypes.h"

#ifdef VDMPP
class JavaGenTools
{
public:
  static void UpdateJSS(const TYPE_CPP_Module &);
  static bool EvalJavaParse(const TYPE_ProjectTypes_FileName &);
  static bool EvalJavaTypeCheck(const TYPE_ProjectTypes_ModuleName &);
  static bool EvalJavaGenerateVDM(const TYPE_ProjectTypes_ModuleName &,
                                  const Bool &,
                                  const Bool &,
                                  const Bool &);
  static void AfterJavaTypeCheck();
};
#endif //VDMPP

#endif //__JAVAGEN_TOOLS_H__
