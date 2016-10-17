/***
*  * WHAT
*  *    Functions related to generation of test coverage in 
*  *    combination with LaTeX
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/testcoverage.cc,v $
*  * VERSION
*  *    $Revision: 1.85 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:07 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <math.h>
#include "rt_info.h"
#include "genlatex.h"
#include "astaux.h"
#include "tbutils.h"
#include "contextinfo.h"
#include "tools.h" // To get GetCI, GetASTs
#include "specfile.h"
#include "testcoverage.h"

#ifdef VDMPP
#include "AS.h"
#include "MANGLE.h"
#endif // VDMPP
#include "tb_wstring.h"
#include "as2ascii.h"

// stuff defined in vdmde.cc
// (should be defined somewhere else) 96/01/16 (HV)

#ifdef VDMPP
const wchar_t * ModuleStr = L"Class";
const wchar_t * moduleStr = L"class";
#else
const wchar_t * ModuleStr = L"Module";
const wchar_t * moduleStr = L"module";
#endif
const wchar_t * FctStr = L"Function/Operation";
const wchar_t * fctStr = L"function/operation";


//////////////////////
// GLOBAL VARIABLES
//////////////////////

// Token which have been read ahead but not yet returned by GetImplToken
static Sequence ReadAhead;

// Latex Stuff
static std::string MatchInRTIStr ("\\begin{rtinfo}");

static int MatchInRTILen = 14;
static std::string MatchOutRTIStr ("\\end{rtinfo}");


////////////////
// OPERATIONS
////////////////

/**************************************************************************/
/****       scanner part                                               ****/
/**************************************************************************/

//The following tokens are recognized by the vdm_impl scanner (GetImplToken ())
#define ID_TOKEN_TC 1
#define LEFTCUR_TOKEN_TC 2
#define RIGHTCUR_TOKEN_TC 3
#define LEFTBRACK_TOKEN_TC 4
#define RIGHTBRACK_TOKEN_TC 5
#define EXIT_TOKEN_TC 6

////////////////////////
// Name: GetRtInfoToken
// Descr:
// This is the scanner function.- for reading the RTF section
// of a LatexDocument
// All tokens must be seperated with a space character.
////////////////////////
Record TestCoverage::GetRtInfoToken (std::ifstream & istr)
{
  Record token (0,0);

  if (!ReadAhead.IsEmpty ())
    { token = ReadAhead.Hd ();
      ReadAhead.ImpTl ();
      return token;
    }

  std::string str;
  istr >> str;

  str = TBWSTR::wstring2string( TBWSTR::mbstr2wstring( str ) );

  while (1) {
    if (istr.eof ()) {
      return Record (EXIT_TOKEN_TC, 0);
    }
      
    Record rc (0,0);

    std::string::size_type length;
    std::string tcidcharset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"
                              "0123456789"
                              "_#'`."; 
    
    while (str.length() > 0) {
      if (str.find(MatchOutRTIStr)==0) {
        rc = Record (EXIT_TOKEN_TC, 0);
        str.replace(0,MatchOutRTIStr.length(), "");
      }
      else if (str.find("{")==0) {
        rc = Record (LEFTCUR_TOKEN_TC, 0);
        str.replace(0,1, ""); 
      }
      else if (str.find("}")==0) {
        rc = Record (RIGHTCUR_TOKEN_TC, 0);
        str.replace(0,1, ""); 
      }
      else if (str.find("[")==0) {
        rc = Record (LEFTBRACK_TOKEN_TC, 0);
        str.replace(0,1, ""); 
      }
      else if (str.find("]")==0) {
        rc = Record (RIGHTBRACK_TOKEN_TC, 0);
        str.replace(0,1, ""); 
      }
/*
      Commented for multi byte char     
      // This also works if length gets the value string_npos
      else if ((length=str.find_first_not_of(tcidcharset)) != 0) {
        rc = Record (ID_TOKEN_TC, 1);
        rc.SetField (1, SEQ<Char> (TBWSTR::string2wstring(str.substr(0,length))));
        str.replace(0,length, ""); 
      }
      else {
        vdm_iplog << L"Unknown symbol '" << TBWSTR::string2wstring(str).substr(0,1) 
                  << L"'detected in rtinfo section." << endl << flush;
        str = "";
      }
*/
      else
      {
        length = str.size();
// 20130209 -->
/*
        if(( str.find(MatchOutRTIStr) >= 0 ) && ( str.find(MatchOutRTIStr) < length )) length = str.find(MatchOutRTIStr);
        if(( str.find("{") >= 0 ) && ( str.find("{") < length )) length = str.find("{");
        if(( str.find("}") >= 0 ) && ( str.find("}") < length )) length = str.find("}");
        if(( str.find("[") >= 0 ) && ( str.find("[") < length )) length = str.find("[");
        if(( str.find("]") >= 0 ) && ( str.find("]") < length )) length = str.find("]");
*/
        std::string::size_type n = str.find(MatchOutRTIStr);
        if ((n != std::string::npos) && (n < length)) length = n;
        n = str.find("{");
        if ((n != std::string::npos) && (n < length)) length = n;
        n = str.find("}");
        if ((n != std::string::npos) && (n < length)) length = n;
        n = str.find("[");
        if ((n != std::string::npos) && (n < length)) length = n;
        n = str.find("]");
        if ((n != std::string::npos) && (n < length)) length = n;
// <-- 20130209
        rc = Record (ID_TOKEN_TC, 1);
        rc.SetField (1, SEQ<Char> (TBWSTR::string2wstring(str.substr(0,length))));
        str.replace(0, length, "");
      }

      if (rc.GetTag () != 0)
      {
        ReadAhead.ImpAppend (rc);
      }
    }

    if (!ReadAhead.IsEmpty ()) //
      { token = ReadAhead.Hd ();
        ReadAhead.ImpTl ();
        return token;
      }
    istr >> str;
    str = TBWSTR::wstring2string( TBWSTR::mbstr2wstring( str ) );
  }
}


//////////////////////
// Name: PrintHead
// Descr:
//   Print head of Latex testcoverage part
//////////////////////

void TestCoverage::PrintHead (std::ostream& ostr,    // output stream  
                              const std::wstring & suite_name, // name of test suite
                              const std::wstring & mod_name,   // module name
                              const std::wstring & LongString) // the body of the table ???
{
  ostr << "\\begin{tabular}{p{25mm}l}" << endl
       << "{\\bf Test Suite :} & " 
       << TBWSTR::wstring2string(GenLatexOutput::ConvertStr (suite_name)) 
       << " \\\\ " << endl;

  if (! (mod_name == std::wstring ()))
  {
    ostr << "{\\bf " 
         << TBWSTR::wstring2string(ModuleStr) 
         << " :} & " 
         << TBWSTR::wstring2mbstr(GenLatexOutput::ConvertStr (mod_name)) 
         << " \\\\ " << endl;
  }

  ostr << "\\end{tabular}" << endl << endl;
  
  ostr << "\\begin{longtable}{|l|r|r|}\\hline" << endl;
 
  if (LongString.length() > 0)
  {
    ostr << "{\\bf " 
         << TBWSTR::wstring2mbstr(GenLatexOutput::ConvertStr (LongString) )
         << "} & {\\bf \\#Calls} & {\\bf Coverage} \\kill" << endl;
  }

  ostr << "{\\bf Name} & {\\bf \\#Calls} & {\\bf Coverage} \\\\ \\hline\\hline" << endl
       << "\\endhead" << endl;
}

//////////////////////
// Name: PrintTail
// Descr:
//  Print tail of Latex testcoverage  part
//////////////////////

void TestCoverage::PrintTail (std::ostream & ostr)
{
  ostr << "\\end{longtable}" << endl << endl;
}

///////////////////
// Name: PrintTestSuiteItem
// Descr:
//  Print info for given function/operation/method in a 
//  given module/class
//////////////////
 
bool TestCoverage::PrintTestSuiteItem (
                  std::ostream & ostr,   // output stream 
                  const TYPE_AS_Name & modnm,  // module/class name
                  const TYPE_AS_Name & func_name, // function/operation/method name
                  int pm,           // ???
                  int & exprs,       // ???
                  int & covered,     // ???
                  const Set & asts,              // The ast
                  const SpecFileFormatType & fmt, // format to use
                  ContextInfo & ci) // The context info table to use
{
  covered = exprs = 0;
#ifdef VDMSL
  Record mod_name(modnm);
#else
  TYPE_AS_Name mod_name(modnm);
#endif

  ///////////////////////
  // Get Coverage information for Class/Module
  ///////////////////////

  Set asts_q (asts);
  Generic gdefs;
  bool found = false;
#ifdef VDMSL
  Generic ast_g;
  for ( bool bb = asts_q.First(ast_g); bb && !found; bb = asts_q.Next(ast_g) ) {
    if (Record(ast_g).Is(TAG_TYPE_AS_Module)) {
      TYPE_AS_Module ast (ast_g);
      if (mod_name == ast.get_nm()) {
        found = true;
        gdefs = ast.get_defs();
      }
        
    }
    if (Record(ast_g).Is(TAG_TYPE_AS_Definitions)) {
      found = true;
      gdefs = ast_g;
    }
  }
  
#endif //VDMSL
#ifdef VDMPP
  Generic ast_g;
  for ( bool bb = asts_q.First(ast_g); bb && !found; bb = asts_q.Next(ast_g) ) {
    TYPE_AS_Class ast (ast_g);
    if (mod_name == ast.get_nm()) {
      found = true;
      gdefs = ast.get_defs();
    }
  }
#endif //VDMPP
  if (!found)
  {
    vdm_iplog << ModuleStr << L" '" << ASTAUX::ASName2String (mod_name)
              << L"' is not contained in the file " << endl << flush;
    return false;
  }

  /////////////////////////
  // Check if function/operation/method is defined
  /////////////////////////

  // Function map in ast:
  Map fnm;
  if (!gdefs.IsNil())
  {
    TYPE_AS_Definitions defs(gdefs);
    fnm.ImpOverride(defs.GetMap(pos_AS_Definitions_fnm)); 
    fnm.ImpOverride(defs.GetMap(pos_AS_Definitions_opm));
  }
  
  if (! fnm.DomExists (func_name))
  {
    vdm_iplog << L"Function/operation '" << ASTAUX::ASName2String (func_name)
              << L"' is not defined for " << moduleStr 
              << L" '" << ASTAUX::ASName2String (mod_name)
              << L"' in test coverage file." << endl << flush;
    return false;
  }

  /////////////////
  // print item info in Latex 
  /////////////////
  TYPE_AS_FnDef fndef (fnm[func_name]);  
  if ((fndef.Is(TAG_TYPE_AS_ExplFnDef) ||
       fndef.Is(TAG_TYPE_AS_ExplOpDef) ||
       fndef.Is(TAG_TYPE_AS_ExtExplFnDef) ||
       fndef.Is(TAG_TYPE_AS_ExtExplOpDef)))
  {
    Tuple cc (RTINFO::Calc_Coverage(fndef, ci));
    exprs = cc.GetIntValue(1);
    covered = cc.GetIntValue(2);
    double pcov = (int)floor((100.0 * (double)covered)/(double)exprs);

    // Print fct/op name with optional Module name Prefix

    if (fmt == FORMAT_LATEX) {
      if (pm && !(mod_name == ASTAUX::MkNameFromId (ASTAUX::MkId(L"DefaultMod"), NilContextId)))
        ostr << TBWSTR::wstring2mbstr(GenLatexOutput::ConvertStr (ASTAUX::ASName2String (mod_name))) << "`";

#ifdef VDMSL
      TYPE_AS_Name ufunc_name (func_name);
#endif // VDMSL
#ifdef VDMPP
      TYPE_AS_Name ufunc_name (MANGLE::IsMangled(func_name) ? MANGLE::GetUnmangledName(func_name) : func_name);
#endif // VDMPP

      ostr << TBWSTR::wstring2mbstr(GenLatexOutput::ConvertStr (ASTAUX::ASName2String (ufunc_name))) << " & ";

      if (ci.HasTestCoverage(ASTAUX::GetCid(fndef))) {
        ostr << RTINFO::Is_Covered (fndef, ci) << " & ";
        if (pcov == 100)
          ostr << "$\\surd$";
        else
          ostr << pcov <<"\\%";
      }
      else {
        ostr <<  "{\\bf undefined} &  {\\bf undefined} ";
      }
      ostr << " \\\\ \\hline\n";

    } else if (fmt == FORMAT_RTF_WORD) {

      // ROW IN TABLE

      ostr <<"\n\\nowidctlpar\\widctlpar\\intbl\\adjustright {\\fs24 ";

      // name of function
      if (pm &&
          !(mod_name == ASTAUX::MkNameFromId (ASTAUX::MkId(L"DefaultMod"), NilContextId)) &&
          !(mod_name == ASTAUX::MkNameFromVoid())) {
        std::string hqstrName (TBWSTR::wstring2string(ASTAUX::ASName2String(mod_name)));
        std::string rtfName = "";
        RTFWordSpecFileHandler::ConvertHexQuadStrToRTF(hqstrName.c_str(), rtfName);
        ostr << rtfName << "`";
      }

#ifdef VDMSL
      TYPE_AS_Name ufunc_name (func_name);
#endif // VDMSL
#ifdef VDMPP
      TYPE_AS_Name ufunc_name (MANGLE::IsMangled(func_name) ? MANGLE::GetUnmangledName(func_name) : func_name);
#endif // VDMPP

      std::string hqstrFunc (TBWSTR::wstring2string(ASTAUX::ASName2String(ufunc_name)));
      std::string rtfFunc = "";
      RTFWordSpecFileHandler::ConvertHexQuadStrToRTF(hqstrFunc.c_str(), rtfFunc);
      ostr << rtfFunc << " \\cell  ";

      // name of function
      if (ci.HasTestCoverage(ASTAUX::GetCid(fndef))) {
        ostr << RTINFO::Is_Covered (fndef, ci) << " \\cell ";
        ostr << pcov << "%\\cell";
      }
      else {
        ostr <<  "undef\\cell undef \\cell";
      }
      ostr << "}\\pard";  // end of row
      ostr << "\n\\nowidctlpar\\widctlpar\\intbl\\adjustright {\\fs24\\lang1030 \\row }\\pard";
    }
  }
  return true;
}  

///////////////////
// Name: PrintEntireTestSuite
// Descr:
//  Print test coverage for given module/class or
//  all modules/classes if the mod_name parameter is MkName(Token()) 
// Return: The overall testcoverage score
///////////////////

double TestCoverage::PrintEntireTestSuite (std::ofstream& ostr,  // output stream 
                                           const TYPE_AS_Name & modnm, // module/class name or MkName(Token()) for all
                                           const Set & asts_,      // The asts of the modules in the file to pp.
                                           const SpecFileFormatType & fmt, // format to use     
                                           ContextInfo&ci)         // context info to use.
{
  int totexprs = 0, totcoverage = 0;

  TYPE_AS_Name mod_name (modnm);

  TYPE_AS_Name emptyName (ASTAUX::MkNameFromId(ASTAUX::MkId(L""), NilContextId));

  if (mod_name == ASTAUX::MkNameFromVoid () || mod_name == emptyName)
                                     //Print coverage info
                                     //for all modules in the
                                     //in the ast. 
                                     // This corresponds
                                     // to an empty module name
                                     // in rtinfo table.
  {
     // Iterate through all asts.
    Generic ast_g;
    Set asts (asts_); // 20060817
    for (bool ii = asts.First(ast_g); ii; ii = asts.Next(ast_g))
    {
      // Don't generate entries for DL Modules
      Generic def_g = Nil();
#ifdef VDMSL
      if (Record(ast_g).Is(TAG_TYPE_AS_Module) || Record(ast_g).Is(TAG_TYPE_AS_Definitions))
      {
        if (Record(ast_g).Is(TAG_TYPE_AS_Module))
        {
          TYPE_AS_Module mod_ast (ast_g);
          mod_name = (mod_ast.GetRecord(pos_AS_Module_nm));
          def_g = mod_ast.GetField(pos_AS_Module_defs);
        }
        // Flat specification
        else {
          def_g = ast_g;
        }
#endif // VDMSL        
#ifdef VDMPP
      if (Record(ast_g).Is(TAG_TYPE_AS_Class))
      {
        TYPE_AS_Class mod_ast (ast_g);
        mod_name = mod_ast.GetRecord(pos_AS_Class_nm);
        def_g = mod_ast.GetField(pos_AS_Class_defs);
#endif // VDMPP        
        
        // If the definition part of the module is non-empty
        // generate the test coverage info for each definition.
        if (!def_g.IsNil())
        {
          TYPE_AS_Definitions def (def_g);
          const Map & fnm (def.GetMap(pos_AS_Definitions_fnm));
          const Map & opm (def.GetMap(pos_AS_Definitions_opm));
            
          Set dom_fnm (fnm.Dom());
          Generic fnnm;
          for (bool cc = dom_fnm.First(fnnm); cc; cc = dom_fnm.Next(fnnm))
          {
// 20130529 -->
#ifdef VDMPP
            if (ASTAUX::IsSubrespFnOp(fnm[fnnm])) continue;
#endif // VDMPP
// <-- 20130529
            int exprs, covered;
            PrintTestSuiteItem(ostr, mod_name, fnnm, 1, exprs, covered, asts, fmt, ci);
            totexprs += exprs;
            totcoverage += covered;
          }
          
          Set dom_opm (opm.Dom());
          Generic opnm;
          for (bool dd = dom_opm.First(opnm); dd; dd = dom_opm.Next(opnm))
          {
// 20130529 -->
#ifdef VDMPP
            if (ASTAUX::IsSubrespFnOp(opm[opnm])) continue;
#endif // VDMPP
// <-- 20130529
            int exprs, covered;
            PrintTestSuiteItem(ostr, mod_name, opnm, 1, exprs, covered, asts, fmt, ci);
            totexprs += exprs;
            totcoverage += covered;
          }
        }
      }
    }
  }
  else
  {
    // A module name was specified in the rtinfo table
    // Find the ast corresponding to the module name.

    Set asts (asts_); // 20060817
    Generic def_g = Nil();
    bool found = false;
    Generic ast_g;
#ifdef VDMSL
    for (bool bb = asts.First(ast_g); bb && !found; bb = asts.Next(ast_g))
    {
      if (Record(ast_g).Is(TAG_TYPE_AS_Module)) {
        TYPE_AS_Module mod_ast (ast_g);
        if (mod_ast.GetRecord(pos_AS_Module_nm) == mod_name)
        {
          def_g = mod_ast.GetField(pos_AS_Module_defs);
          found = true;
        }
      }
    }
#endif // VDMSL
#ifdef VDMPP
    for (bool bb = asts.First(ast_g); bb && !found; bb = asts.Next(ast_g))
    {
      TYPE_AS_Class mod_ast (ast_g);
      if (mod_ast.GetRecord(pos_AS_Class_nm) == mod_name) 
      {
        def_g = mod_ast.GetField(pos_AS_Class_defs);
        found = true;
      }
    }
#endif //VDMPP

    if (!found) 
    {
      vdm_iplog << ModuleStr << L" '" << ASTAUX::ASName2String (mod_name) << L"' is not found in the file" << endl << flush;
      return 1;
    }

    // If the definition part of the module is non-empty
    // generate the test coverage info for each definition.
    if (!def_g.IsNil())
    {
      TYPE_AS_Definitions def (def_g);
      Map fnm (def.GetMap(pos_AS_Definitions_fnm));
      fnm.ImpOverride(def.GetMap(pos_AS_Definitions_opm));
        
      Set dom_fnm (fnm.Dom());
      Generic fnopnm;
      for (bool cc = dom_fnm.First(fnopnm); cc; cc = dom_fnm.Next(fnopnm) )
      {
        int exprs, covered;
        PrintTestSuiteItem(ostr, mod_name, fnopnm, 1, exprs, covered, asts, fmt, ci);
        totexprs += exprs;
        totcoverage += covered;
      }
    }
  }

  if (totexprs > 0) {
    int percent = (int)floor((100.0 * (double)totcoverage)/(double)totexprs);
    return percent;
  }
  else
    return -1;
}  

///////////////////////////
// Name: EvalListTestSuite
// Descr :
//  Read TestSuite File and lists (to vdm_iplog)
//       the test coverage statistics for every
//       function and method/operation for
//       for each moduleand print.
///////////////////////////

void TestCoverage::EvalListTestSuite (const std::wstring & fname, ContextInfo & ci)
{
  ToolMediator::ResetTestCoverage(ci);
  ToolMediator::LoadTestCoverageFile(fname, ci, SET< SEQ<Char> >(), true);

  int totexprs = 0, totcoverage = 0;

  //////////////////////////////////////
  //// Get all the asts in the
  //// specification manager.
  /////////////////////////

  SEQ<Record> asts (ToolMediator::GetVDMASTs());

  size_t len_asts = asts.Length();
  for (size_t idx = 1; idx <= len_asts; idx++)
  {
#ifdef VDMSL
    if (!asts[idx].Is(TAG_TYPE_AS_Module))
      continue;
    const TYPE_AS_Module & ast (asts[idx]);
    const TYPE_AS_Name & mod_nm (ast.GetRecord(pos_AS_Module_nm));
    const Generic & gdef (ast.GetField(pos_AS_Module_defs));
#endif // VDMSL
#ifdef VDMPP
    const TYPE_AS_Class & ast (asts[idx]);
    const TYPE_AS_Name & mod_nm (ast.GetRecord(pos_AS_Class_nm));
    const Generic & gdef (ast.GetField(pos_AS_Class_defs));
#endif // VDMPP

    if (!gdef.IsNil())
    {
      TYPE_AS_Definitions def (gdef);
      Map fnm (def.GetMap(pos_AS_Definitions_fnm));
      fnm.ImpOverride(def.GetMap(pos_AS_Definitions_opm));
      
// 20130610 -->
      int clmodexprs = 0, clmodcoverage = 0;
// <-- 20130610
      //////////////
      //// Compute coverage for all functions/operations/methods
      /////////////
      Set dom_fnm (fnm.Dom());
      Generic fnm_nm;
      for (bool cc = dom_fnm.First(fnm_nm); cc; cc = dom_fnm.Next(fnm_nm))
      {
        Record fndef (fnm[fnm_nm]);
// 20130529 -->
#ifdef VDMPP
        if (ASTAUX::IsSubrespFnOp(fndef)) continue;
#endif // VDMPP
// <-- 20130529
        Tuple cv (RTINFO::Calc_Coverage(fndef, ci));
        int exprs = cv.GetIntValue(1);
        int covered = cv.GetIntValue(2);
        int pcov = (int)floor((100.0 * (double)covered)/(double)exprs);
        totexprs += exprs;
        totcoverage += covered;
// 20130610 -->
        clmodexprs += exprs;
        clmodcoverage += covered;
// <-- 20130610
        
        vdm_iplog.width ();
        vdm_iplog.width (5);
        vdm_iplog << pcov << L"% ";
        vdm_iplog.width (6);
        vdm_iplog << RTINFO::Is_Covered (fndef, ci) << L"  ";
#ifdef VDMSL
        if (mod_nm != ASTAUX::MkNameFromId (ASTAUX::MkId(L"DefaultMod"), NilContextId))
          vdm_iplog << ASTAUX::ASName2String (mod_nm) << L"`";
        vdm_iplog << ASTAUX::ASName2String (fnm_nm) << endl;
#endif // VDMSL
#ifdef VDMPP
        vdm_iplog << ASTAUX::ASName2String (mod_nm) << L"`";
        if (MANGLE::IsMangled(fnm_nm))
        {
          Tuple t (MANGLE::UnMangle(fnm_nm));
          SEQ<TYPE_AS_Type> tp_l (t.GetSequence(3));
          vdm_iplog << ASTAUX::ASName2String (t.GetRecord(1));
          if (tp_l.IsEmpty())
            vdm_iplog << L"()" << endl;
          else
          {
            AS2ASCII conv;
            vdm_iplog << L"( ";
            size_t len_tp_l = tp_l.Length();
            for (size_t idx = 1; idx <= len_tp_l; idx++)
            {
              if (idx > 1) 
                vdm_iplog << L" * ";
              conv.Type2ASCII(tp_l[idx], vdm_iplog);   
            }
            vdm_iplog << L" )" << endl;
          }
        }
        else
          vdm_iplog << ASTAUX::ASName2String (fnm_nm) << endl;
#endif // VDMPP
      }
// 20130610 -->
#ifdef VDMSL
      if (mod_nm != ASTAUX::MkNameFromId (ASTAUX::MkId(L"DefaultMod"), NilContextId))
      {
#endif // VDMSL
      if (clmodexprs > 0) {
        int percentCoverage = (int)floor ((100.0 * (double)clmodcoverage)/(double)clmodexprs);
        vdm_iplog.width ();
        vdm_iplog.width (5);
        vdm_iplog << percentCoverage << L"%  ";
        vdm_iplog << ASTAUX::ASName2String (mod_nm) << endl;
      }
#ifdef VDMSL
      }
#endif // VDMSL
// <-- 20130610
    }
  }
  
  vdm_iplog << endl;
  vdm_iplog << L"Total Coverage: ";
  if (totexprs > 0) {
    int percentCoverage = (int)floor ((100.0 * (double)totcoverage)/(double)totexprs);
    vdm_iplog << percentCoverage << L"%" << endl;
  }
  else
    vdm_iplog << L"undefined" << endl;
}

/////////////////////////
// Name: EvalLatexRTI
// Descr:
//  Read RT_info part of Latex document from the input stream(istr) using the 
//  parser function GetRtInfoToken and produce complete Latex output of data in Latex table 
/////////////////////////

bool TestCoverage::EvalLatexRTI (std::ifstream & istr, 
                                 std::ofstream & ostr,
                                 const Set & asts,
                                 ContextInfo & ci,
                                 const std::wstring & short_name)
{
  std::wstring mod_name, LongString, func_name;
  int get_mod_name = 1, get_all = 1;
  Record token;

  double pcoverage = 0;

  ////////////////////////
  // Parse/read the  Runtime info
  ////////////////////////

  if ((token = GetRtInfoToken (istr)).Is(LEFTBRACK_TOKEN_TC))
  {
    if (!((token = GetRtInfoToken (istr)).Is(ID_TOKEN_TC)))
    {
       vdm_iplog << L"Syntax error in rtinfo section: " << L"identifier expected after '['." << endl << flush;
       return false;
    }

    LongString = token.GetSequence (1).GetString ();

      
    if (!((token = GetRtInfoToken (istr)).Is(RIGHTBRACK_TOKEN_TC)))
    {
      vdm_iplog << L"Syntax error in rtinfo section: ']' expected." << endl << flush;
      return false;
    }
      
    token = GetRtInfoToken (istr);
  }

  if (!token.Is(LEFTCUR_TOKEN_TC))
  {
    vdm_iplog << L"Syntax error in rtinfo section: '{' expected." << endl << flush;
    return false;
  }

  if (!((token = GetRtInfoToken (istr)).Is(ID_TOKEN_TC)))
  {
     vdm_iplog << L"Syntax error in rtinfo section: " << L"identifier expected after '{'." << endl << flush;
     return false;
  }

  std::wstring suite_name (token.GetSequence (1).GetString ());

  SET< SEQ<Char> > load_files (mk_set(SEQ<Char>(short_name)));

  ToolMediator::ResetTestCoverage(ci);
  ToolMediator::LoadTestCoverageFile(suite_name, ci, load_files, false);

  if (!((token = GetRtInfoToken (istr)).Is(RIGHTCUR_TOKEN_TC)))
  {
    vdm_iplog << L"Syntax error in rtinfo section: '}' expected." << endl << flush;
    return false;
  }
      
  if ((token = GetRtInfoToken (istr)).Is(LEFTBRACK_TOKEN_TC))
  {
    if (!((token = GetRtInfoToken (istr)).Is(ID_TOKEN_TC)))
    {
      vdm_iplog << L"Syntax error in rtinfo section: " << L"identifier expected after '['." << endl << flush;
      return false;
    }

    get_mod_name = 1;
    mod_name = token.GetSequence (1).GetString ();
      
    if (!((token = GetRtInfoToken (istr)).Is(RIGHTBRACK_TOKEN_TC)))
    {
      vdm_iplog << L"Syntax error in rtinfo section: ']' expected." << endl << flush;
      return false;
    }

    token = GetRtInfoToken (istr);
  }

  PrintHead (ostr, suite_name, mod_name, LongString);

  int totcoverage = 0, totexprs = 0;
  while (!token.Is(EXIT_TOKEN_TC)) {

    get_all = 0;
    
    if (!(token.Is(ID_TOKEN_TC)))
    {
      vdm_iplog << L"Syntax error in rtinfo section: " << L"identifier expected." << endl << flush;
      return false;
    }

    func_name = token.GetSequence (1).GetString ();

    if (func_name.find(L"`") != std::string::npos) {
      std::string::size_type i = func_name.find(L"`"); 
      mod_name = func_name.substr(0,i);
      func_name = func_name.substr(i+1);
    }
    
    int exprs, covered;
    PrintTestSuiteItem (ostr,
                        ASTAUX::MkNameFromId (ASTAUX::MkId(mod_name), NilContextId),
                        ASTAUX::MkNameFromId (ASTAUX::MkId(func_name), NilContextId),
                        get_mod_name,
                        exprs, covered,
                        asts,
                        FORMAT_LATEX,
                        ci);
    totexprs += exprs;
    totcoverage += covered;

    token = GetRtInfoToken (istr);
  }
  
  if (get_all)
    // PrintEntireTestSuite return -1 if pcoverage is undefined
    pcoverage = PrintEntireTestSuite (ostr,
                                      ASTAUX::MkNameFromId (ASTAUX::MkId(mod_name), NilContextId),
                                      asts,
                                      FORMAT_LATEX,
                                      ci);
  else if (totexprs > 0)
    pcoverage = (int)floor((100.0 * (double)totcoverage)/(double)totexprs);
  else
    pcoverage = -1;

  ostr << "\\hline" << endl
       << "{\\bf Total Coverage} & & ";
  if (pcoverage != -1)
    ostr << "{\\bf " << pcoverage << "\\%}";
  else
    ostr << "{\\bf undefined}";
  ostr << " \\\\ \\hline" << endl;
  
  PrintTail (ostr);
  return true;
}

//////////////////////
// Name: MatchInRTI
// Descr:
//
//////////////////////

int TestCoverage::MatchInRTI (int pm,          // ??? 
                              char ch,         // ???
                              std::ifstream& istr,  // input
                              std::ofstream& ostr,  // output
                              const Set & asts,
                              ContextInfo&ci,  // Context info table.
                              const std::wstring & short_name) // the file to pretty print
{ 
  if (MatchInRTIStr[pm] == ch)
  {
    if (pm == MatchInRTILen - 1)
    { 
      EvalLatexRTI (istr, ostr, asts, ci, short_name);
      return 0;
    }
    else
      return pm + 1;
  }
  else if (pm && pm < MatchInRTILen)
    ostr << MatchInRTIStr.substr(0, pm);

  ostr.put(ch);
  return 0;
}
                    
////////////////////////
// Name: texvdmrti2texvdm
// Description: parses the istr (the file to pretty print)
//              for the test coverage table.
//              Computes the test coverage table and puts it file f2.
//              Initialises the test coverage counters corresponding
//              to the information in the test coverage file.
//              The test coverage file is the one that is listed
//              in the tex-document.
////////////////////////
bool TestCoverage::texvdmrti2texvdm (std::ifstream& istr,
                                     const std::wstring & f2,
                                     const Set & asts,  // The set of ast corresponding to the file to pretty print.
                                     ContextInfo& ci,
                                     const std::wstring & short_name) // the file to pretty print
{
  std::ofstream ostr (TBWSTR::wstring2string(f2).c_str());
  if (!ostr) {
    vdm_iplog << L"Couldn't open file '" << f2 << L"' for writing" << endl << flush;
    return false;
  }
  
  char ch;
  int pm = 0;

  ReadAhead = Sequence();
  
  while (istr.get (ch))
    pm = MatchInRTI  (pm, ch, istr, ostr, asts, ci, short_name);

  istr.close ();
  ostr.close ();
  return true;
}

