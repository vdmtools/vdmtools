/***
*  * WHAT
*  *    Functions related to generation of test coverage in 
*  *    combination with LaTeX
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/testcoverage.h,v $
*  * VERSION
*  *    $Revision: 1.15 $
*  * DATE
*  *    $Date: 2005/12/02 06:28:32 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __testcoverage_h__
#define __testcoverage_h__

#include "metaiv.h"
#include "specfile.h"

#include <fstream>
#include <string>

class TestCoverage
{
public:
  static double PrintEntireTestSuite (std::ofstream & ostr,  // output stream 
                                      const TYPE_AS_Name & modnm,   // module/class name or MkName(Token()) for all
                                      const Set & asts,      // The asts of the modules in the file to pp.
                                      const SpecFileFormatType & fmt,
                                      ContextInfo & ci);  // format to use     

  static void EvalListTestSuite (const std::wstring & fname, ContextInfo & ci);

  static bool texvdmrti2texvdm (std::ifstream & f1,
                                const std::wstring & f2,
                                const Set & asts,
                                ContextInfo & ci,
                                const std::wstring &);

private:
  static Record GetRtInfoToken (std::ifstream & istr);
  static void PrintHead (std::ostream & ostr,    // output stream  
                         const std::wstring & suite_name, // name of test suite
                         const std::wstring & mod_name,   // module name
                         const std::wstring & LongString);  // the body of the table ???
  static void PrintTail (std::ostream & ostr);
  static bool PrintTestSuiteItem (std::ostream & ostr,   // output stream 
                                  const TYPE_AS_Name & modnm,  // module/class name
                                  const TYPE_AS_Name & func_name, // function/operation/method name
                                  int pm,           // ???
                                  int & exprs,       // ???
                                  int & covered,     // ???
                                  const Set & asts,              // The ast
                                  const SpecFileFormatType & fmt, // format to use
                                  ContextInfo & ci);// The context info table to use
  static bool EvalLatexRTI (std::ifstream & istr,
                            std::ofstream & ostr,
                            const Set & asts,
                            ContextInfo & ci,
                            const std::wstring & short_name);
  static int MatchInRTI (int pm,          // ??? 
                         char ch,         // ???
                         std::ifstream & istr,  // input
                         std::ofstream & ostr,  // output
                         const Set & asts,
                         ContextInfo & ci,  // Context info table.
                         const std::wstring & short_name); // the file to pretty print
};
#endif // __testcoverage_h__
