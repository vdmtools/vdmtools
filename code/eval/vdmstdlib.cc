/***
*  * WHAT
*  *
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/vdmstdlib.cc,v $
*  * VERSION
*  *    $Revision: 1.54 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:07 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    IDERS-
*  * PROJECT
*  *    IDERS Project EP8593
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmstdlib.h"
#include "projectval.h"
#include "tools.h"
#include "evaldl.h"
#include "astaux.h"
#include "val2x.h"
#include "tb_wstring.h"
#include "contextinfo.h"
#include "intconvquotes.h"
#include "libman.h"
#include "stackeval.h"
#include "tbutils.h"
#include "tbdebug.h"

#include "backslash.h"
#include "random.h"
#ifdef _MSC_VER
#define _USE_MATH_DEFINES 1
#endif // _MSC_VER
#include <math.h>
#include <fstream>
#include <sstream>
#include "evalaux.h"
#include "tb_exceptions.h"
#include "rt_errmsg.h"
#include "evalexpr.h"
#include "settings.h"

#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif // _MSC_VER

VdmStdLib::VdmStdLib()
{
  this->rnd_seed = -1;
}

void VdmStdLib::InitRandom()
{
  SetSeed(GetSeed());
}

long VdmStdLib::SetSeed(long seed)
{
  this->rnd_seed = seed;
  if ( this->rnd_seed >= 0 ) {
    this->rnd.set_seed((unsigned long)seed);
  }
  return seed;
}

long VdmStdLib::GetSeed()
{
  return this->rnd_seed;
}

int VdmStdLib::GetRandom(int i)
{
  int ret = i;
  if ( this->rnd_seed >= 0 ) {
    ret = this->rnd.get_random (i);
  }
  return ret;
}

// StripDoubleQuotes
// s : seq of char
// ==> seq of char
wstring VdmStdLib::StripDoubleQuotes (const wstring & s)
{
  if (s.size() > 2 && s[0]== L'"' && s[s.size ()-1]== L'"') {
    wstring ss (s.substr (1,s.size ()-2)); // strip leading and trailing  '"'s
    return ss;
  }
  else {
    return s;
  }
}

// IsEq
// nm : AS`Name
// q : seq of char
// ==> bool
bool VdmStdLib::IsEq(const TYPE_AS_Name & nm, const wstring & q)
{
  wstring s (ASTAUX::ASName2String (nm));
#ifdef VDMSL
  return (s == q) || (s == ASTAUX::GetDefaultModName());
#endif //VDMSL
#ifdef VDMPP
  return (s == q);
#endif // VDMPP
}

// EvalStdLibMath
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibMath (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));

  if (arg_lv.IsEmpty()) {
    if (func == L"pi_f") {
      return mk_(Bool(true), mk_SEM_NUM(Real(M_PI)));
    }
    else {
      return mk_(Bool(false), Nil());
    }
  }
  else {
    Sequence arg (EvalState::Sem2M4 (arg_lv, NULL));
    double d = 0;
    if (arg[1].IsReal()) {
      d = Real (arg[1]).GetValue ();
    }
    else if (arg[1].IsInt ()) {
      d = (double) Int (arg[1]).GetValue ();
    }
    else {
      return mk_(Bool(false), Nil());
    }
    if (func == L"sin") {
      return mk_(Bool(true), mk_SEM_NUM(Real(sin(d))));
    }
    else if (func == L"cos") {
      return mk_(Bool(true), mk_SEM_NUM(Real(cos(d))));
    }
    else if (func == L"tan") {
      return mk_(Bool(true), mk_SEM_NUM(Real(tan(d))));
    }
    else if (func == L"cot") {
      return mk_(Bool(true), mk_SEM_NUM(Real(1.0/tan(d))));
    }
    else if (func == L"asin") {
      return mk_(Bool(true), mk_SEM_NUM(Real(asin(d))));
    }
    else if (func == L"acos") {
      return mk_(Bool(true), mk_SEM_NUM(Real(acos(d))));
    }
    else if (func == L"atan") {
      return mk_(Bool(true), mk_SEM_NUM(Real(atan(d))));
    }
    else if (func == L"sqrt") {
      return mk_(Bool(true), mk_SEM_NUM(Real(sqrt(d))));
    }
    else if (func == L"exp") {
      return mk_(Bool(true), mk_SEM_NUM(Real(exp(d))));
    }
    else if (func == L"ln") {
      return mk_(Bool(true), mk_SEM_NUM(Real(log(d))));
    }
    else if (func == L"log") {
      return mk_(Bool(true), mk_SEM_NUM(Real(log10(d))));
    }
    else if (func == L"srand2") {
      return mk_(Bool(true), mk_SEM_NUM(Real((double)SetSeed((long)d))));
    }
    else if (func == L"rand") {
      return mk_(Bool(true), mk_SEM_NUM(Real(GetRandom((int)d))));
    }
    else {
      return mk_(Bool(false), Nil());
    }
  }
}

// filedirecOK
// d : SEM`VAL
// fd :
// ==> bool
bool VdmStdLib::filedirecOK (const TYPE_SEM_VAL & d, ios_base::openmode& fd)
{
  Generic g (EvalState::Convert2M4 (d, NULL));

  if (g.IsNil ()) {
    return false;
  }

  Quote qfd (g);
  wstring fdirec (qfd.GetValue());
  if (fdirec == L"start") {
    fd = ios_base::out;
    return true;
  }
  else if (fdirec == L"append") {
    fd = ios_base::out|ios_base::app;
    return true;
  }
  else {
    return false;
  }
}

// EvalStdLibIO
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibIO (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));

  if (func == L"writeval") {
    // writeval[@p]: @p -> bool
    // writeval(val) == ...
    vdm_iplog << VAL2X::val2asc(arg_lv[1]) << endl << flush;
    return mk_(Bool(true), sem_true);
  }
  else if (func == L"fwriteval") {
    // fwriteval[@p]:seq1 of char * @p * filedirective -> bool
    // fwriteval(filename, val, fdir) ==

    wstring filename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[1]))); // filename
    string filenameMbstr (TBWSTR::wstring2fsstr(filename));

    ofstream out; // wofstream out;
    ios_base::openmode fd;
    if (!filedirecOK(arg_lv[3], fd)) { // filedirective (filedirective = <start>|<append>)
      // wrong file directive
      this->vdmferror = L"Wrong file directive: " + VAL2X::val2asc (arg_lv[3]);
      return mk_(Bool(true), sem_false);
    }

    out.open (filenameMbstr.c_str(), fd);
    if (out) {
      string semMbstr (TBWSTR::wstring2mbstr(VAL2X::val2asc (arg_lv[2]))); // val
      out << semMbstr << endl << flush;
      out.close();
      out.clear();
      return mk_(Bool(true), sem_true);
    }
    else {
      this->vdmferror = L"Failed to open file: " + filename;
      return mk_(Bool(true), sem_false);
    }
  }
  else if (func == L"freadval") {
    // freadval[@p]: seq1 of char -> bool * [@p]
    // freadval(f) ==
    wstring filename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[1])));
    TYPE_ProjectTypes_FileName sp_filenm (PTAUX::mk_FileName(filename));

    wostringstream d;
    Tuple pe_res (TBDEBUG::ParseAndFullyEvalExprs (sp_filenm, d, SEQ<Char>(func)));

    if (pe_res.GetBoolValue (1)) {
      return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_true, pe_res.GetSequence (2).Hd())));
    }
    else {
      this->vdmferror = L"Error while reading file: " + filename;
      return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
    }
  }
  else if (func == L"ferror") {
    // ferror:()  ==> seq of char
    // ferror () ==
    Sequence r (this->vdmferror);
    this->vdmferror = L"";
    return mk_(Bool(true), EvalState::M42Sem (r, NULL));
  }
  else if (func == L"fecho") {
    // fecho: seq of char * seq of char * [filedirective] ==> bool
    // fecho (filename, text, fdir) == ...
    wstring filename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[1]))); // filename
    string filenameMbstr (TBWSTR::wstring2fsstr(filename));

    wstring sem (StripDoubleQuotes (VAL2X::val2asc (arg_lv[2]))); // text

    // convert backslashed char
    wstring sem_q, error;
    Backslashed::convert_backslashed( sem, sem_q, error );

    if ((filename == L"[  ]") || (filename == L"")) {
      if (sem != L"[  ]") {
        vdm_iplog << sem_q << flush;
      }
      return mk_(Bool(true), sem_true);
    }
    else {
      ofstream out;
      ios_base::openmode fd;
      if (!filedirecOK(arg_lv[3], fd)) { // [filedirective] (filedirective = <start>|<append>)
        // wrong file directive
        this->vdmferror = L"Wrong file directive: " + VAL2X::val2asc (arg_lv[3]);
        return mk_(Bool(true), sem_false);
      }
      out.open (filenameMbstr.c_str(),fd);
      if (out) {
        out << TBWSTR::wstring2mbstr(sem_q) << flush;
        out.close();
        out.clear();
        return mk_(Bool(true), sem_true);
      }
      else {
        this->vdmferror = L"Failed to open file: " + filename;
        return mk_(Bool(true), sem_false);
      }
    }
  }
  else if (func == L"print") {
    size_t len_arg_lv = arg_lv.Length();
    for (size_t i = 1; i <= len_arg_lv; i++) {
      vdm_iplog << StripDoubleQuotes (VAL2X::val2asc (arg_lv[i]));
    }
    vdm_iplog << flush;
    return mk_(Bool(true), sem_cont);
  }
  else if (func == L"println") {
    size_t len_arg_lv = arg_lv.Length();
    for (size_t i = 1; i <= len_arg_lv; i++) {
      vdm_iplog << StripDoubleQuotes (VAL2X::val2asc (arg_lv[i]));
    }
    vdm_iplog << endl << flush;
    return mk_(Bool(true), sem_cont);
  }
  return mk_(Bool(false), Nil());

//  return mk_(Bool(true), sem_true);
}

// EvalStdLibCSV
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibCSV (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));

  if (func == L"fwriteval") {
    // fwriteval:seq1 of char * seq of ? * filedirective -> bool
    // fwriteval(filename, val, fdir) ==

    wstring filename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[1]))); // filename
    string filenameMbstr (TBWSTR::wstring2fsstr(filename));

//    if (!arg_lv[2].Is(TAG_TYPE_SEM_SEQ))
//    {
//    }

    ofstream out; // wofstream out;
    ios_base::openmode fd;
    if (!filedirecOK(arg_lv[3], fd)) { // filedirective (filedirective = <start>|<append>)
      // wrong file directive
      this->vdmferror = L"Wrong file directive: " + VAL2X::val2asc (arg_lv[3]);
      return mk_(Bool(true), sem_false);
    }

    out.open (filenameMbstr.c_str(), fd);
    if (out) {
/*
      // TODO semMbstr
      const SEQ<TYPE_SEM_VAL> & arg_l (arg_lv[2].GetSequence(pos_SEM_SEQ_v));
      string semMbstr (TBWSTR::wstring2mbstr(VAL2X::val2asc (arg_lv[2]))); // val
      out << semMbstr << endl << flush;
*/
      out.close();
      out.clear();
      return mk_(Bool(true), sem_true);
    }
    else {
      this->vdmferror = L"Failed to open file: " + filename;
      return mk_(Bool(true), sem_false);
    }
  }
  else if (func == L"freadval") {
    // freadval[@p]: seq1 of char * int -> bool * [@p]
    // freadval(f,index) ==
    wstring filename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[1])));
    string filenameMbstr (TBWSTR::wstring2fsstr(filename));

    Real r (arg_lv[2].GetReal(pos_SEM_NUM_v));
    size_t index = (size_t)(r.GetIntValue());

    ifstream ifs; // wofstream out;
    ifs.open (filenameMbstr.c_str());
    if (ifs.good()) {
      bool found = false;
      size_t count = 0;
      Sequence l;
      while(!ifs.eof() && !found) {
        string line;
        getline(ifs,line);
        //line = TBWSTR::convertCrToNl( line ); // TODO:
        if (!ifs.eof()) {
          count++;
          if (count == index) {
            // TODO: 
/*
            TYPE_ProjectTypes_FileName sp_filenm (PTAUX::mk_FileName(filename));
            wostringstream d;
            Tuple pe_res (TBDEBUG::ParseAndFullyEvalExprs (sp_filenm, d, SEQ<Char>(func)));

            if (pe_res.GetBoolValue (1))
            {
              return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_true, pe_res.GetSequence (2).Hd())));
            }
            else
            {
              this->vdmferror = L"Error while reading file: " + filename;
              return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
            }
*/
            found = true;
          }
        }
      }
      ifs.close();
      if (found) {
        return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_true, TYPE_SEM_SEQ().Init(l))));
      }
      else {
        this->vdmferror = L"Error while reading file: " + filename;
        return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
      }
    }
    else {
      this->vdmferror = L"Failed to open file: " + filename;
      return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
    }
  }
  else if (func == L"flinecount") {
    // flinecount: seq1 of char -> bool * int
    // flinecount(f) ==
    wstring filename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[1])));
    string filenameMbstr (TBWSTR::wstring2fsstr(filename));

    ifstream ifs; // wofstream out;
    ifs.open (filenameMbstr.c_str());
    if (ifs.good()) {
      size_t count = 0;
      while(!ifs.eof()) {
        string line;
        getline(ifs,line);
        //line = TBWSTR::convertCrToNl( line ); // TODO:
        if (!ifs.eof())
          count++;
      }
      ifs.close();
      Real r (count);
      return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_true, TYPE_SEM_NUM().Init(r))));
    }
    else {
      this->vdmferror = L"Failed to open file: " + filename;
      return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
    }
  }
  else if (func == L"ferror") {
    // ferror:()  ==> seq of char
    // ferror () ==
    Sequence r (this->vdmferror);
    this->vdmferror = L"";
    return mk_(Bool(true), EvalState::M42Sem (r, NULL));
  }
  return mk_(Bool(false), Nil());
}

Map VdmStdLib::GetAstOfModuleNames(const SET< SEQ<Char> > & nm_s)
{
  if (nm_s.IsEmpty()) {
    return Map();
  }
  Set nm_s_q (nm_s);
  Map res;
  Generic g;
  for (bool bb = nm_s_q.First(g); bb; bb = nm_s_q.Next(g)) {
    TYPE_ProjectTypes_ModuleName modnm;
    modnm.set_nm(g);
    Generic astval (ToolMediator::GetAST(modnm));
    if (!astval.IsNil()) {
      Record ast (PTAUX::ExtractAstOrFlat(astval));
      res.Insert(g, INT2Q::h2gAS(ast));
    }
    else {
#ifdef VDMSL
      vdm_err << L"Module "
#endif // VDMSL
#ifdef VDMPP
      vdm_err << L"Class "
#endif // VDMPP
              << L"\'" << modnm.get_nm().GetString()
              << L"\' is not defined in current specification" << endl << flush;
    }
  } 
  return res;
}

Map VdmStdLib::GetTokenInfoOfFileNames(const SET< SEQ<Char> > & fn_s)
{
  if (fn_s.IsEmpty()) {
    return Map();
  }

  Set fn_s_q (fn_s);
  Map res;
  Generic g;
  for (bool bb = fn_s_q.First(g); bb; bb = fn_s_q.Next(g)) {
    Generic tis (GetCI().GetTokenInfos(g));
    if (!tis.IsNil()) {
      res.Insert(g, tis);
    }
  }
  return res;
}

Map VdmStdLib::GetContextNodeInfoOfFileNames(const SET< SEQ<Char> > & fn_s, const Generic & tcvfnm)
{
  if (fn_s.IsEmpty()) {
    return Map();
  }

  ContextInfo & ci = GetCI();
  if (!tcvfnm.IsNil()) {
    wstring tcvnm (Sequence(tcvfnm).GetString());
    ToolMediator::ResetTestCoverage(ci);
    ToolMediator::LoadTestCoverageFile(tcvnm, ci, fn_s, false);
  }

  Set fn_s_q (fn_s);
  Map res;
  Generic g;
  for (bool bb = fn_s_q.First(g); bb; bb = fn_s_q.Next(g)) {
    Generic cnis (ci.GetContextNodeInfos(g));
    if (!cnis.IsNil()) {
//      res.Insert(g, cnis);
      res.Insert(g, INT2Q::h2gAS(cnis));
    }
  }
  return res;
}

// EvalStdLibVDMToolsWizard
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibVDMToolsWizard (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));
#ifdef VDMSL
  if (func == L"moduleAST") {
#endif // VDMSL
#ifdef VDMPP
  if (func == L"classAST") {
#endif // VDMPP
    TYPE_SEM_VAL modnm (arg_lv[1]);
    wstring modulename (StripDoubleQuotes (VAL2X::val2asc (modnm)));
    SET<TYPE_AS_Id> nm_s;
    nm_s.Insert(SEQ<Char>(modulename));
    Map ast_m (GetAstOfModuleNames(nm_s));
    if (ast_m.IsEmpty()) {
      return mk_(Bool(true), sem_nil);
    }
    else {
      return mk_(Bool(true), EvalState::M42Sem(ast_m[SEQ<Char>(modulename)], NULL));
    }
  }
#ifdef VDMSL
  else if (func == L"moduleASTs") {
#endif // VDMSL
#ifdef VDMPP
  else if (func == L"classASTs") {
#endif // VDMPP
    TYPE_SEM_VAL modnm_s (arg_lv[1]);
    if (!modnm_s.Is(TAG_TYPE_SEM_SET)) {
      return mk_(Bool(true), mk_SEM_SET(Set()));
    }
    SET<TYPE_SEM_VAL> mnm_s (modnm_s.GetSet(pos_SEM_SET_v));
    SET<TYPE_AS_Id> nm_s;
    Generic g;
    for (bool bb = mnm_s.First(g); bb; bb = mnm_s.Next(g)) {
      TYPE_SEM_VAL mnm (g);
      if(mnm.Is(TAG_TYPE_SEM_SEQ)) {
        wstring modulename (StripDoubleQuotes (VAL2X::val2asc (mnm)));
        nm_s.Insert(SEQ<Char>(modulename));
      }
    } 
    return mk_(Bool(true), EvalState::M42Sem(GetAstOfModuleNames(nm_s), NULL));
  }
#ifdef VDMSL
  else if (func == L"allModuleNames") {
#endif // VDMSL
#ifdef VDMPP
  else if (func == L"allClassNames") {
#endif // VDMPP
    SET<TYPE_ProjectTypes_ModuleName> mod_s (ToolMediator::VDMModules ());
    SET<TYPE_ProjectTypes_String> nm_s;
    Generic g;
    for (bool bb = mod_s.First(g); bb; bb = mod_s.Next(g)) {
      TYPE_ProjectTypes_ModuleName mod (g);
      nm_s.Insert(mod.get_nm());
    }
    return mk_(Bool(true), EvalState::M42Sem(nm_s, NULL));
  }
#ifdef VDMSL
  else if (func == L"moduleToFileMap") {
#endif // VDMSL
#ifdef VDMPP
  else if (func == L"classToFileMap") {
#endif // VDMPP
    SET<TYPE_ProjectTypes_ModuleName> mods (ToolMediator::AllModules());
    Map res;
    Generic g;
    for (bool bb = mods.First(g); bb; bb = mods.Next(g)) {
      TYPE_ProjectTypes_ModuleName mod (g);
      SET<TYPE_ProjectTypes_FileName> files (ToolMediator::FileOfModule(mod));
      if (!files.IsEmpty()) {
        res.Insert(mod.get_nm(), files.GetElem().get_nm());
      }
    }
    return mk_(Bool(true), EvalState::M42Sem(res, NULL));
  }
  else if (func == L"docTokenInfoSeq") {
    TYPE_SEM_VAL fnm (arg_lv[1]);
    wstring filename (StripDoubleQuotes (VAL2X::val2asc (fnm)));
    SET<TYPE_AS_Id> nm_s;
    nm_s.Insert(SEQ<Char>(filename));
    Map ast_m (GetTokenInfoOfFileNames(nm_s));
    if (ast_m.IsEmpty()) {
      return mk_(Bool(true), sem_nil);
    }
    else {
      return mk_(Bool(true), EvalState::M42Sem(ast_m[SEQ<Char>(filename)], NULL));
    }
  }
  else if (func == L"docTokenInfoSeqes") {
    TYPE_SEM_VAL fnm_s (arg_lv[1]);
    if (!fnm_s.Is(TAG_TYPE_SEM_SET)) {
      return mk_(Bool(true), mk_SEM_SET(Set()));
    } 
    SET<TYPE_SEM_VAL> filenm_s (fnm_s.GetSet(pos_SEM_SET_v));
    SET<TYPE_AS_Id> nm_s;
    Generic g;
    for (bool bb = filenm_s.First(g); bb; bb = filenm_s.Next(g)) {
      TYPE_SEM_VAL filenm (g);
      if(filenm.Is(TAG_TYPE_SEM_SEQ)) {
        wstring filename (StripDoubleQuotes (VAL2X::val2asc (filenm)));
        nm_s.Insert(SEQ<Char>(filename));
      }
    } 
    return mk_(Bool(true), EvalState::M42Sem(GetTokenInfoOfFileNames(nm_s), NULL));
  }
  else if (func == L"docContextNodeInfoSeq") {
    TYPE_SEM_VAL fnm (arg_lv[1]);
    wstring filename (StripDoubleQuotes (VAL2X::val2asc (fnm)));
    SET<TYPE_AS_Id> nm_s;
    nm_s.Insert(SEQ<Char>(filename));
    
    Generic tcvfnm = Nil();
    if(arg_lv[2].Is(TAG_TYPE_SEM_SEQ)) {
      wstring tcvfilename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[2])));
      if (tcvfilename.length() > 0) {
        tcvfnm = SEQ<Char>(tcvfilename);
      }
    }

    Map ast_m (GetContextNodeInfoOfFileNames(nm_s, tcvfnm));
    if (ast_m.IsEmpty()) {
      return mk_(Bool(true), sem_nil);
    }
    else {
      return mk_(Bool(true), EvalState::M42Sem(ast_m[SEQ<Char>(filename)], NULL));
    }
  }
  else if (func == L"docContextNodeInfoSeqes") {
    TYPE_SEM_VAL fnm_s (arg_lv[1]);
    if (!fnm_s.Is(TAG_TYPE_SEM_SET)) {
      return mk_(Bool(true), mk_SEM_SET(Set()));
    }
    Generic tcvfnm = Nil();
    if(arg_lv[2].Is(TAG_TYPE_SEM_SEQ)) {
      wstring tcvfilename (StripDoubleQuotes (VAL2X::val2asc (arg_lv[2])));
      if (tcvfilename.length() > 0) {
        tcvfnm = SEQ<Char>(tcvfilename);
      }
    }

    SET<TYPE_SEM_VAL> filenm_s (fnm_s.GetSet(pos_SEM_SET_v));
    SET<TYPE_AS_Id> nm_s;
    Generic g;
    for (bool bb = filenm_s.First(g); bb; bb = filenm_s.Next(g)) {
      TYPE_SEM_VAL filenm (g);
      if(filenm.Is(TAG_TYPE_SEM_SEQ)) {
        wstring filename (StripDoubleQuotes (VAL2X::val2asc (filenm)));
        nm_s.Insert(SEQ<Char>(filename));
      }
    } 
    return mk_(Bool(true), EvalState::M42Sem(GetContextNodeInfoOfFileNames(nm_s, tcvfnm), NULL));
  }
  else if (func == L"nodeInfo") {
    TYPE_SEM_VAL cid_s (arg_lv[1]);
    if (!cid_s.Is(TAG_TYPE_SEM_SET)) {
      return mk_(Bool(true), mk_SEM_SET(Set()));
    }
    Map res;
    SET<TYPE_SEM_VAL> ci_s (cid_s.GetSet(pos_SEM_SET_v));
    Generic g;
    for (bool bb = ci_s.First(g); bb; bb = ci_s.Next(g)) {
      TYPE_SEM_VAL cid_val (g);
      if (cid_val.Is(TAG_TYPE_SEM_NUM)) {
        TYPE_CI_ContextId cid (cid_val.GetSet(pos_SEM_SET_v));
        Generic ni (GetCI().GetContextNodeInfo(cid));
        if (!ni.IsNil()) {
          res.Insert(cid, ni);
        }
      }
    } 
    return mk_(Bool(true), EvalState::M42Sem(res, NULL));
  }
  else {
    return mk_(Bool(false), Nil());
  }
}

// EvalStdLibVDMByteUtil
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibVDMByteUtil (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));
  if (func == L"And") {
    // And: (int | seq of int) * (int | seq of int) -> (int | seq of int)
    // And(b1, b2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2)) {
        return mk_(Bool(true), mk_SEM_NUM(a1.GetReal(pos_SEM_NUM_v).HexAnd(a2.GetReal(pos_SEM_NUM_v))));
      }
      else if (a1.Is(TAG_TYPE_SEM_SEQ) && a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_NUM> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        const SEQ<TYPE_SEM_NUM> & bs2 (a2.GetSequence(pos_SEM_SEQ_v));
        if (bs1.Length() == bs2.Length()) {
          Sequence res;
          for (size_t i = bs1.Length(); i > 0; i--) {
            res.ImpPrepend(mk_SEM_NUM(bs1[i].GetReal(pos_SEM_NUM_v).HexAnd(bs2[i].GetReal(pos_SEM_NUM_v))));
          }
          return mk_(Bool(true), mk_SEM_SEQ(res));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"Or") {
    // Or: (int | seq of int) * (int | seq of int) -> (int | seq of int)
    // Or(b1, b2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2)) {
        return mk_(Bool(true), mk_SEM_NUM(a1.GetReal(pos_SEM_NUM_v).HexOr(a2.GetReal(pos_SEM_NUM_v))));
      }
      else if (a1.Is(TAG_TYPE_SEM_SEQ) && a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_NUM> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        const SEQ<TYPE_SEM_NUM> & bs2 (a2.GetSequence(pos_SEM_SEQ_v));
        if (bs1.Length() == bs2.Length()) {
          Sequence res;
          for (size_t i = bs1.Length(); i > 0; i--) {
            res.ImpPrepend(mk_SEM_NUM(bs1[i].GetReal(pos_SEM_NUM_v).HexOr(bs2[i].GetReal(pos_SEM_NUM_v))));
          }
          return mk_(Bool(true), mk_SEM_SEQ(res));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"Exor") {
    // Exor: (int | seq of int) * (int | seq of int) -> (int | seq of int)
    // Exor(b1, b2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2)) {
        return mk_(Bool(true), mk_SEM_NUM(a1.GetReal(pos_SEM_NUM_v).HexExor(a2.GetReal(pos_SEM_NUM_v))));
      }
      else if (a1.Is(TAG_TYPE_SEM_SEQ) && a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_NUM> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        const SEQ<TYPE_SEM_NUM> & bs2 (a2.GetSequence(pos_SEM_SEQ_v));
        if (bs1.Length() == bs2.Length()) {
          Sequence res;
          for (size_t i = bs1.Length(); i > 0; i--) {
            res.ImpPrepend(mk_SEM_NUM(bs1[i].GetReal(pos_SEM_NUM_v).HexExor(bs2[i].GetReal(pos_SEM_NUM_v))));
          }
          return mk_(Bool(true), mk_SEM_SEQ(res));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"Add") {
    // Add: seq of int * seq of int -> seq of int
    // Add(b1, b2) ==  ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (a1.Is(TAG_TYPE_SEM_SEQ) && a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_NUM> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        const SEQ<TYPE_SEM_NUM> & bs2 (a2.GetSequence(pos_SEM_SEQ_v));
        if (bs1.Length() == bs2.Length()) {
          Sequence res;
          size_t len = bs1.Length();
          int carry = 0;
          for (size_t i = 1; i <= len; i++) {
            int b1 = (int)bs1[i].GetRealValue(pos_SEM_NUM_v);
            int b2 = (int)bs2[i].GetRealValue(pos_SEM_NUM_v);
            int n = b1 + b2 + carry;;
            carry = n / 0x100;
            res.ImpAppend(mk_SEM_NUM(Real(n & 0xff)));
          }
          return mk_(Bool(true), mk_SEM_SEQ(res));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"Sub") {
    // Sub: seq of int * seq of int -> seq of int
    // Sub(b1, b2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (a1.Is(TAG_TYPE_SEM_SEQ) && a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_NUM> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        const SEQ<TYPE_SEM_NUM> & bs2 (a2.GetSequence(pos_SEM_SEQ_v));
        if (bs1.Length() == bs2.Length()) {
          Sequence res;
          size_t len = bs1.Length();
          int carry = 0;
          for (size_t i = 1; i <= len; i++) {
            int b1 = (int)bs1[i].GetRealValue(pos_SEM_NUM_v);
            int b2 = (int)bs2[i].GetRealValue(pos_SEM_NUM_v);
            int n = b1 - b2 - carry;;
            if (n < 0) {
              carry = 1;
              n = n + 0x100; 
            }
            else {
              carry = 0;
            }
            res.ImpAppend(mk_SEM_NUM(Real(n)));
          }
          if (carry == 0) {
            return mk_(Bool(true), mk_SEM_SEQ(res));
          }
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"ShiftRight") {
    // ShiftRight: int * int -> int
    // ShiftRight(b1, b2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2)) {
        return mk_(Bool(true), mk_SEM_NUM(a1.GetReal(pos_SEM_NUM_v).HexByteShiftRight(a2.GetReal(pos_SEM_NUM_v))));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"ShiftLeft") {
    // ShiftLeft: int * int -> int
    // ShiftLeft(b1, b2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2)) {
        return mk_(Bool(true), mk_SEM_NUM(a1.GetReal(pos_SEM_NUM_v).HexByteShiftLeft(a2.GetReal(pos_SEM_NUM_v))));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"CreateMaskByte") {
    // CreateMaskByte:  int * int -> int
    // CreateMaskByte(fromBitNum, toBitNum) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2)) {
        int fb = (int)a1.GetRealValue(pos_SEM_NUM_v);
        int tb = (int)a2.GetRealValue(pos_SEM_NUM_v);
        if ((fb >= 1) && (fb <= 8) && (tb >= 1) && (tb <= 8) && (fb <= tb)) {
          return mk_(Bool(true), mk_SEM_NUM(Real(CreateMaskByte(fb, tb))));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"MaskedByteValueOf") {
    // MaskedByteValueOf: int * int * int -> int
    // MaskedByteValueOf(byte, fromBitNum, toBitNum) == ...
    if (arg_lv.Length() == 3) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      const TYPE_SEM_VAL & a3 (arg_lv[3]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2) && AUX::IsInt(a3)) {
        int b = (int)a1.GetRealValue(pos_SEM_NUM_v);
        int fb = (int)a2.GetRealValue(pos_SEM_NUM_v);
        int tb = (int)a3.GetRealValue(pos_SEM_NUM_v);
        if ((fb >= 1) && (fb <= 8) && (tb >= 1) && (tb <= 8) && (fb <= tb)) {
          int n = CreateMaskByte(fb, tb);
          return mk_(Bool(true), mk_SEM_NUM(Real((b & n) >> (fb - 1))));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"ByteToBits") {
    // ByteToBits: int -> seq of int
    // ByteToBits(byte) == ...
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      if (AUX::IsInt(a1)) {
        int b = (int)a1.GetRealValue(pos_SEM_NUM_v);
        Sequence s;
        for (int i = 0; i < 8; i++) {
          s.ImpPrepend(mk_SEM_NUM(Real(b & 0x1)));
          b = b >> 1;
        } 
        return mk_(Bool(true), mk_SEM_SEQ(s));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"BitsToByte") {
    // BitsToByte: seq of int -> int
    // BitsToByte(bits) == ...
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      if (a1.Is(TAG_TYPE_SEM_SEQ)) {
        SEQ<TYPE_SEM_NUM> s (a1);
        int n = 0;
        for (int i = 8; i > 0; i--) {
          n = (n << 1) + (int)s[i].GetRealValue(pos_SEM_NUM_v);
        }
        return mk_(Bool(true), mk_SEM_NUM(Real(n)));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"CompareTo") {
    // CompareTo : seq of int * seq of int -> int
    // CompareTo (byteSeq1, byteSeq2) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (a1.Is(TAG_TYPE_SEM_SEQ) && a2.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_NUM> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        const SEQ<TYPE_SEM_NUM> & bs2 (a2.GetSequence(pos_SEM_SEQ_v));
        if (bs1.Length() == bs2.Length()) {
          if (bs1 == bs2)
            return mk_(Bool(true), mk_SEM_NUM(Real(0)));
          
          int eq = 0;
          for (size_t i = bs1.Length(); (i > 0) && (eq == 0); i--) {
            eq = bs1[i].GetReal(pos_SEM_NUM_v).Compare(bs2[i].GetReal(pos_SEM_NUM_v));
          }
          return mk_(Bool(true), mk_SEM_NUM(Real(eq)));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"CreateMaskByteSeq") {
    // CreateMaskByteSeq: int * int * int * int * int -> seq of int
    // CreateMaskByteSeq(fromByteNum, fromBitNum, toByteNum, toBitNum, length) == ...
    if (arg_lv.Length() == 5) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      const TYPE_SEM_VAL & a3 (arg_lv[3]);
      const TYPE_SEM_VAL & a4 (arg_lv[4]);
      const TYPE_SEM_VAL & a5 (arg_lv[5]);
      if (AUX::IsInt(a1) && AUX::IsInt(a2) && AUX::IsInt(a3) && AUX::IsInt(a4) && AUX::IsInt(a5)) {
        int fby = (int)a1.GetRealValue(pos_SEM_NUM_v);
        int fbi = (int)a2.GetRealValue(pos_SEM_NUM_v);
        int tby = (int)a3.GetRealValue(pos_SEM_NUM_v);
        int tbi = (int)a4.GetRealValue(pos_SEM_NUM_v);
        int len = (int)a5.GetRealValue(pos_SEM_NUM_v);
        if ((len >= 1) &&
            (fby >= 1) && (fby <= len) && (tby >= 1) && (tby <= len) &&
            (fby <= tby) &&
            (fbi >= 1) && (fbi <= 8) && (tbi >= 1) && (tbi <= 8)) {
          Sequence res;
          for (int i = 1; i <= len; i++) {
            res.ImpAppend(mk_SEM_NUM(Real(0)));
          }
          for (int j = fby; j <= tby; j++) {
            if ((j == fby) && (j == tby)) {
              res.ImpModify(j, mk_SEM_NUM(Real(CreateMaskByte(tbi, fbi))));
            }
            else if (j == fby) {
              res.ImpModify(j, mk_SEM_NUM(Real(CreateMaskByte(1, fbi))));
            }
            else if (j == tby) {
              res.ImpModify(j, mk_SEM_NUM(Real(CreateMaskByte(tbi, 8))));
            }
            else {
              res.ImpModify(j, mk_SEM_NUM(Real(CreateMaskByte(1, 8))));
            }
          }
          return mk_(Bool(true), mk_SEM_SEQ(res));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"HexStrToInt") {
    // HexStrToInt: seq1 of char -> int
    // HexStrToInt(hexstr) == ...
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      if (a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_CHAR> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        size_t len = bs1.Length();
        if (len > 0) {
          int v = 0;
          for (size_t i = 1; i <= len; i++) {
            v = v * 16 + bs1[i].GetChar(pos_SEM_CHAR_v).ToHexNum();
          }
          return mk_(Bool(true), mk_SEM_NUM(Real(v)));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"DecStrToInt") {
    // DecStrToInt: seq1 of char -> int
    // DecStrToInt(decstr) == ...
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      if (a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_CHAR> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        size_t len = bs1.Length();
        if (len > 0) {
          int v = 0;
          for (size_t i = 1; i <= len; i++) {
            v = v * 10 + bs1[i].GetChar(pos_SEM_CHAR_v).ToDecNum();
          }
          return mk_(Bool(true), mk_SEM_NUM(Real(v)));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"BinaryStrToInt") {
    // BinaryStrToInt: seq1 of char -> int
    // BinaryStrToInt(binstr) == ...
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      if (a1.Is(TAG_TYPE_SEM_SEQ)) {
        const SEQ<TYPE_SEM_CHAR> & bs1 (a1.GetSequence(pos_SEM_SEQ_v));
        size_t len = bs1.Length();
        if (len > 0) {
          int v = 0;
          for (size_t i = 1; i <= len; i++) {
            v = v * 2 + bs1[i].GetChar(pos_SEM_CHAR_v).ToBinNum();
          }
          return mk_(Bool(true), mk_SEM_NUM(Real(v)));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"IntToHexStr") {
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1)) {
        int b1 = (int)a1.GetRealValue(pos_SEM_NUM_v);
        int size = 0;
        if (AUX::IsInt(a2)) {
          size = (int)a2.GetRealValue(pos_SEM_NUM_v);
        }
        SEQ<TYPE_SEM_CHAR> res; 
        if (size > 0) {
          for (int i = 1; i <= size; i++) {
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 16)));
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar((b1/16) % 16)));
            b1 = b1/256;
          }
        }
        else {
          res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 16)));
          res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar((b1/16) % 16)));
          b1 = b1/256;
          while(b1 > 0) {
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 16)));
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar((b1/16) % 16)));
            b1 = b1/256;
          }
        }
        return mk_(Bool(true), mk_SEM_SEQ(res));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"IntToDecStr") {
    // IntToDecStr: int * nat -> seq of char
    // IntToDecStr(num, size) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1)) {
        int b1 = (int)a1.GetRealValue(pos_SEM_NUM_v);
        int size = 0;
        if (AUX::IsInt(a2)) {
          size = (int)a2.GetRealValue(pos_SEM_NUM_v);
        }
        SEQ<TYPE_SEM_CHAR> res; 
        if (size > 0) {
          for (int i = 1; i <= size; i++) {
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 10)));
            b1 = b1/10;
          }
        }
        else {
          res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 10)));
          b1 = b1/10;
          while(b1 > 0) {
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 10)));
            b1 = b1/10;
          }
        }
        return mk_(Bool(true), mk_SEM_SEQ(res));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"IntToBinaryStr") {
    // IntToBinaryStr: int * nat -> seq of char
    // IntToBinaryStr(num, size) == ...
    if (arg_lv.Length() == 2) {
      const TYPE_SEM_VAL & a1 (arg_lv[1]);
      const TYPE_SEM_VAL & a2 (arg_lv[2]);
      if (AUX::IsInt(a1)) {
        int b1 = (int)a1.GetRealValue(pos_SEM_NUM_v);
        int size = 0;
        if (AUX::IsInt(a2)) {
          size = (int)a2.GetRealValue(pos_SEM_NUM_v);
        }
        SEQ<TYPE_SEM_CHAR> res; 
        if(size > 0) {
          for (int i = 1; i <= size; i++) {
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 2)));
            b1 = b1/2;
          }
        }
        else {
          res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 2)));
          b1 = b1/2;
          while(b1 > 0) {
            res.ImpPrepend(TYPE_SEM_CHAR().Init(NumToHexChar(b1 % 2)));
            b1 = b1/2;
          }
        }
        return mk_(Bool(true), mk_SEM_SEQ(res));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else
    return mk_(Bool(false), Nil());
}

// CreateMaskByte
// from : int
// to : int
// -> int
int VdmStdLib::CreateMaskByte(int from, int to)
{
  int n = 0; 
  for (int i = from; i <= to; i++) {
    n += 1 << (i - 1);
  }
  return n;
}

/*
int VdmStdLib::HexCharToNum(const wchar_t & c)
{
  switch(c) {
    case L'0': return 0;
    case L'1': return 1;
    case L'2': return 2;
    case L'3': return 3;
    case L'4': return 4;
    case L'5': return 5;
    case L'6': return 6;
    case L'7': return 7;
    case L'8': return 8;
    case L'9': return 9;
    case L'a':
    case L'A': return 10;
    case L'b':
    case L'B': return 11;
    case L'c':
    case L'C': return 12;
    case L'd':
    case L'D': return 13;
    case L'e':
    case L'E': return 14;
    case L'f':
    case L'F': return 15;
    default: return 0;
  }
}

int VdmStdLib::DecCharToNum(const wchar_t & c)
{
  switch(c) {
    case L'0': return 0;
    case L'1': return 1;
    case L'2': return 2;
    case L'3': return 3;
    case L'4': return 4;
    case L'5': return 5;
    case L'6': return 6;
    case L'7': return 7;
    case L'8': return 8;
    case L'9': return 9;
    default: return 0;
  }
}

int VdmStdLib::BinaryCharToNum(const wchar_t & c)
{
  switch(c) {
    case L'0': return 0;
    case L'1': return 1;
    default: return 0;
  }
}
*/

wchar_t VdmStdLib::NumToHexChar(int i)
{
  switch(i) {
    case 0: return L'0';
    case 1: return L'1';
    case 2: return L'2';
    case 3: return L'3';
    case 4: return L'4';
    case 5: return L'5';
    case 6: return L'6';
    case 7: return L'7';
    case 8: return L'8';
    case 9: return L'9';
    case 10: return L'a';
    case 11: return L'b';
    case 12: return L'c';
    case 13: return L'd';
    case 14: return L'e';
    case 15: return L'f';
    default: return L'?';
  }
}

// EvalStdLibVDMUtil
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibVDMUtil (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));
  if (func == L"set2seq") {
    // set2seq[@T] : set of @T +> seq of @T
    // set2seq(x) == ...
    if (arg_lv.Length() == 1 ) {
      TYPE_SEM_VAL val (arg_lv[1]);
      if (val.Is(TAG_TYPE_SEM_SET)) {
        return mk_(Bool(true), mk_SEM_SEQ(val.GetSet(pos_SEM_SET_v).ToSequence()));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if ((func == L"get_file_pos") || (func == L"P")) {
    // get_file_pos : () +> [ seq of char * nat * nat * seq of char * seq of char ]
    // get_file_pos() == ...
    if (theStackMachine().CallStackLevel() > 1) {
      TYPE_CI_ContextId cid (theStackMachine().HeadCS().get_oldCid());
      if (cid != NilContextId ) {
        SEQ<Char> clsmodnm;
        SEQ<Char> fnopnm;
        TYPE_STKM_CallStackItem csi (theStackMachine().GetCS(Int(2))); 
        if (csi.GetIntValue(pos_STKM_CallStackItem_type) == CallStackItemType::CS_FNOP) {
          const Generic & nmOrDesc (csi.GetField(pos_STKM_CallStackItem_nmOrDesc)); 
          if (nmOrDesc.Is(TAG_TYPE_AS_Name)) {
            TYPE_AS_Name nm (nmOrDesc);
            const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
            switch (ids.Length()) {
              case 2: {
                clsmodnm = ids[1];
                fnopnm = ids[2];
                break;
              }
              case 1: {
                fnopnm = ids[1];
                break;
              }
            }
          }
        }
        
//        unsigned int ci = (unsigned int)(cid.GetValue());
//        int fid = ci >> 22; // node_bits
        Tuple t (ContextInfo::SplitCid(cid));
        int64_t fid = t.GetIntValue(1);
        if ( fid >= 10 ) {
          Tuple t (GetCI().GetFileLineColPos(cid));
          Tuple res (mk_(t.GetField(1), t.GetField(2), t.GetField(3), clsmodnm, fnopnm));
          return mk_(Bool(true), EvalState::M42Sem(res, NULL));
        }
      }
    }
    return mk_(Bool(true), sem_nil);
  }
  else if (func == L"val2seq_of_char") {
    // val2seq_of_char[@T] : @T +> seq of char
    // val2seq_of_char(x) ==
    if (arg_lv.Length() == 1) {
      Sequence s (VAL2X::val2asc(arg_lv[1])); // SEM`VAL
      return mk_(Bool(true), EvalState::M42Sem(s, NULL));
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"seq_of_char2val") {
    // seq_of_char2val[@p]:seq1 of char -> bool * [@p]
    // seq_of_char2val(s) ==
    if (arg_lv.Length() == 1) {
      Generic g (EvalState::Convert2M4(arg_lv[1], NULL)); // seq1 of char
      if (g.IsSequence()) {
        wstring str (Sequence(g).GetString());

        SEQ<TYPE_SEM_BlkEnv> te (theStackMachine().TopEnvL());
        theStackMachine().PopEnvL ();
        SEQ<TYPE_SEM_BlkEnv> te2 (theStackMachine().TopEnvL());
        te2.ImpPrepend(te[1]);
        theStackMachine().PushEnvL(te2);
        wostringstream d;
        Tuple pe_res (TBDEBUG::ParseAndFullyEvalExprs(PTAUX::mk_ToolCommand (str), d, SEQ<Char>(L"seq_of_char2val")));

        if (pe_res.GetBoolValue (1)) {
          return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_true, pe_res.GetSequence (2).Hd())));
        }
        else {
          this->vdmferror = L"Error while parsing string: \"" + str + L"\"";
          return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"eval") {
    // eval[@p]:seq1 of char -> bool * [@p]
    // eval(s) ==
    if (arg_lv.Length() == 1) {
      Generic g (EvalState::Convert2M4(arg_lv[1], NULL)); // seq1 of char
      if (g.IsSequence()) {
        wstring str (Sequence(g).GetString());

        wostringstream d;
        Tuple pres (TOOLS::ParseExprs (PTAUX::mk_ToolCommand (str), d)); // bool * seq os AS`Expr
        if (!pres.GetBoolValue(1)) {
          this->vdmferror = L"Error while parsing string: \"" + str + L"\"";
          return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
        }

#ifdef VDMPP
        bool objpushed = false;
        if (theStackMachine().HasCurObjRef()) {
          TYPE_SEM_OBJ_uRef ref (theStackMachine().GetCurObjRef());
          theStackMachine().PushCurObj(ref,Nil(),Nil());
          objpushed = true; 
        }
#endif // VDMPP
        Tuple el (theStackMachine().GetEnvLLengths());
        bool envpushed = false;
        if (el.GetIntValue(1) >= 2) {
          SEQ<TYPE_SEM_BlkEnv> env_l (theStackMachine().TopEnvL());
          theStackMachine().PopEnvL();
          SEQ<TYPE_SEM_BlkEnv> pre_env_l (theStackMachine().TopEnvL());
          theStackMachine().PushEnvL(env_l);
          theStackMachine().PushEnvL(pre_env_l);
          envpushed = true;
        }

        Tuple res (theStackMachine().EvalUninterruptedCmd(pres.GetSequence(2)[1],
                                                          TYPE_STKM_SubProgram(),
                                                          TYPE_STKM_SubProgram(),
                                                          SEQ<Char>(L"eval")));
        if (envpushed) {
          theStackMachine().PopEnvL();
        }
#ifdef VDMPP
        if (objpushed) {
          theStackMachine().PopCurObj();
        }
#endif // VDMPP

        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (eval_state.Is(TAG_TYPE_STKM_Success)) {
          TYPE_SEM_VAL val (res.GetField(2));
          if (val.Is(TAG_TYPE_SEM_CONT)) {
            val = sem_nil;
          }
          return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_true, val)));
        }
        this->vdmferror = L"Error while eval string: \"" + str + L"\"";
        return mk_(Bool(true), mk_SEM_TUPLE(mk_sequence(sem_false, sem_nil)));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"cast") {
    // cast[@T1, @T2] : @T1 -> @T2
    // cast(s) ==
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & semv (arg_lv[1]);
      return mk_(Bool(true), semv);
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"clone") {
    // clone[@T] : @T -> @T
    // clone(s) ==
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & semv (arg_lv[1]);
#ifdef VDMPP
      if (semv.Is(TAG_TYPE_SEM_OBJ_uRef)) {
        TYPE_GLOBAL_OBJ_uDesc desc (theState().Lookup_obj_tab(semv));
        const TYPE_SEM_OBJ & semobj (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
        const Generic & dlclassp (desc.GetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr));
        TYPE_SEM_OBJ_uRef ref (theStackMachine().MakeNewObj(semobj, dlclassp));
        return mk_(Bool(true), ref);
      }
      else
#endif // VDMPP
        return mk_(Bool(true), semv);
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"equals") {
    // equals[@T1, @T2] : @T1, @T2 -> bool
    // equals(s1, s2) ==
    if (arg_lv.Length() == 2) {
      return mk_(Bool(true), EXPR::EvalEquals(arg_lv[1], arg_lv[2]));
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"control") {
    if (arg_lv.Length() == 2) {
      Generic g (EvalState::Convert2M4(arg_lv[1], NULL)); // seq1 of char
      if ((g == Quote(L"set")) || (g == Quote(L"unset"))) {
        Generic attr (EvalState::Convert2M4(arg_lv[2], NULL)); // seq1 of char
        if (attr == SEQ<Char>(L"exception")) {
          if (g == Quote(L"set")) {
            Settings.RTErrExceptionOn();
          }
          else {
            Settings.RTErrExceptionOff();
          }
          return mk_(Bool(true), sem_cont);
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
#ifdef VDMPP
  else if (func == L"classname") {
    // classname[@T] : @T -> [seq1 of char]
    // classname(s) ==
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & semv (arg_lv[1]);
      if (semv.Is(TAG_TYPE_SEM_OBJ_uRef)) {
        TYPE_AS_Name nm (semv.GetRecord(pos_SEM_OBJ_uRef_tp));
        TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
        if (!ids.IsEmpty()) {
          return mk_(Bool(true), EvalState::M42Sem (ids[1], NULL));
        }
      }
    }
    return mk_(Bool(false), Nil());
  }
#endif // VDMPP
  else if (func == L"char2code") {
    // char2code : char -> nat
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & semv (arg_lv[1]);
      if (semv.Is(TAG_TYPE_SEM_CHAR)) {
        const Char & c (semv.GetChar(pos_SEM_CHAR_v));
        Real r ((unsigned long)(c.GetValue()));
        return mk_(Bool(true), TYPE_SEM_NUM().Init(r));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"code2char") {
    // code2char : nat -> char
    if (arg_lv.Length() == 1) {
      const TYPE_SEM_VAL & semv (arg_lv[1]);
      if (semv.Is(TAG_TYPE_SEM_NUM)) {
        const Real & r (semv.GetReal(pos_SEM_NUM_v));
        Char c ((wchar_t)(r.GetIntValue()));
        return mk_(Bool(true), TYPE_SEM_CHAR().Init(c));
      }
    }
    return mk_(Bool(false), Nil());
  }
  else if (func == L"current_time") {
#ifdef _MSC_VER
#ifdef _MSC_EXTENSIONS
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tmpres /= 1000;  /*convert into miliseconds*/
    Int tm (tmpres);
#else
    timeval tv;
    gettimeofday(&tv, NULL);
    Int tm (((int64_t)(tv.tv_sec)) * 1000 + ((int64_t)(tv.tv_usec))/1000);
#endif // _MSC_VER
    Real r (tm);
    return mk_(Bool(true), TYPE_SEM_NUM().Init(r));
  }
  else if (func == L"cdate") {
    time_t timer;
    time(&timer);
    tm * t = localtime(&timer);
    Tuple res (mk_(Int(t->tm_year + 1900), Int(t->tm_mon + 1), Int(t->tm_mday),
                   Int(t->tm_hour), Int(t->tm_min), Int(t->tm_sec)));
    return mk_(Bool(true), EvalState::M42Sem(res, NULL));
  }
  else if (func == L"uname") {
    return mk_(Bool(true), EvalState::M42Sem (TBUTILS::GetOSName(), NULL));
  }
  else {
    return mk_(Bool(false), Nil());
  }
}

#ifdef VDMPP
// EvalStdLibThreadUtil
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::EvalStdLibThreadUtil (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  wstring func (ASTAUX::ASName2String (fnname));
  if (func == L"thread_status") {
    Set tid_s (theScheduler().GiveAllThreads().Dom());
    Map res;
    Generic tid;
    for (bool bb = tid_s.First(tid); bb; bb = tid_s.Next(tid)) {
      TYPE_SCHDTP_ThreadStatus tst (theScheduler().GetThreadStatus(tid));
      Generic st = Nil();
      switch(tst.GetTag()) {
        case TAG_TYPE_SCHDTP_Running:    { st = Quote(L"Running");    break; }
        case TAG_TYPE_SCHDTP_Blocked:    { st = Quote(L"Blocked");    break; }
        case TAG_TYPE_SCHDTP_MaxReached: { st = Quote(L"MaxReached"); break; }
        case TAG_TYPE_SCHDTP_Sleeping:   { st = Quote(L"Sleeping");   break; }
#ifdef VICE
        case TAG_TYPE_SCHDTP_SyncOp:     { st = Quote(L"SyncOp");     break; }
        case TAG_TYPE_SCHDTP_SyncOpRet:  { st = Quote(L"SyncOpRet");  break; }
        case TAG_TYPE_SCHDTP_Completed:  { st = Quote(L"Completed");  break; }
#endif // VICE
      }
      res.ImpModify(tid, st);
    }
    return mk_(Bool(true), EvalState::M42Sem (res, NULL));
  }
  if (func == L"set_mode") {
    if (arg_lv.Length() == 1) {
      Generic g (EvalState::Convert2M4(arg_lv[1], NULL)); // seq1 of char
      if (g == Quote(L"PureCooperative")) {
        Settings.SetPureCooperative();
        return mk_(Bool(true), sem_nil);
      }
      else if (g == Quote(L"InstructionNumberSlice")) {
        Settings.SetInstrnumSlice();
        return mk_(Bool(true), sem_nil);
      }
#ifdef VICE
      else if (g == Quote(L"TimeSlice")) {
        Settings.SetTimeSlice();
        return mk_(Bool(true), sem_nil);
      }
#endif // VICE
    }
    return mk_(Bool(false), Nil());
  }
  else {
    return mk_(Bool(false), Nil());
  }
}
#endif // VDMPP

// modname : AS`Name
// fnname : AS`Name
// arg_lv : seq of SEM`VAL
// ==> bool * SEM`VAL
Tuple VdmStdLib::IsVdmStdLib (const TYPE_AS_Name & modname,
                              const TYPE_AS_Name & fnname,
                              const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  if (IsEq (modname, L"MATH")) {
    return EvalStdLibMath (fnname, arg_lv);
  }
  else if (IsEq (modname, L"IO")) {
    return EvalStdLibIO (fnname, arg_lv);
  }
  else if (IsEq (modname, L"CSV")) {
    return EvalStdLibCSV (fnname, arg_lv);
  }
  else if (IsEq (modname, L"VDMToolsWizard")) {
    return EvalStdLibVDMToolsWizard (fnname, arg_lv);
  }
  else if (IsEq (modname, L"VDMByteUtil")) {
    return EvalStdLibVDMByteUtil (fnname, arg_lv);
  }
  else if (IsEq (modname, L"VDMUtil")) {
    return EvalStdLibVDMUtil (fnname, arg_lv);
  }
//  else if (IsEq (modname, L"INSTR")) {
//    return EvalStdLibINSTR (fnname, arg_lv);
//  }
#ifdef VDMPP
  else if (IsEq (modname, L"VDMThreadUtil")) {
    return EvalStdLibThreadUtil (fnname, arg_lv);
  }
#endif // VDMPP
  else {
    return mk_(Bool(false), Nil());
  }
}

