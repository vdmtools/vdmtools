/***
*  * WHAT
*  *    The stand-alone back-end 
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/test1.cc,v $
*  * VERSION
*  *    $Revision: 1.27 $
*  *    $Revision: 1.27 $
*  * DATE
*  *    $Date: 2006/06/05 06:58:25 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERS/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <fstream>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <time.h>
using namespace std; // for wcerr

#include "metaiv.h"
#include "backend.h"
#include "intconvquotes.h"
#include "astaux.h"
#include "cgtag.h"
#include "test1.h"

#include "astaux.h"
#include "asquotes.h"

#include "parse_expr.h"
#include "contextinfo.h"

#include "cli_options.h"
#include "tb_wstring.h"

#ifndef _MSC_VER
wostream& vdm_log(wcout);
wostream& vdm_err(wcout);
#endif // _MSC_VER

wstring GetDateString( const char* format )
{
  time_t t;
  if (time (&t) != -1) {
    char buf [50];
#ifdef _MSC_VER
    setlocale( LC_TIME, "English" );
#else
    setlocale( LC_TIME, "en_US" );
#endif // _MSC_VER
    strftime (buf, 49, format, localtime (&t));
    return TBWSTR::string2wstring(string (buf));
  }
  else
  {
    return L"unknown time";
  }
}
                                                                                
wstring Seq2Str (const Sequence& msg)
{
  Generic g;
  wstring str;
  msg.GetString(str);
  return str;
}

static ContextInfo global_context_info; 

ContextInfo& GetCI()
{
  return global_context_info; 
}

void GetFileMapNumName(Map& long_map) 
{
}

//void GetFileMapNameNum(Set file_nms, bool all, Map& long_map, Map& short_map) 
void GetFileMapNameNum(Set file_nms, bool all) 
{
}

/*
static Record mk_ErrMsg(int fid, int line, int col, Sequence descr_l)
{
  return Record();
}
*/

Generic ConvASTRec2CPP(Record ast)
{
  int tag (ast.GetTag());
  switch(tag) {
  case TAG_TYPE_AS_PrefixExpr: {
    // Should correspond to a nil context id
    TYPE_AS_RealLit cid (ast.GetField(2));
    if (ast.GetField(1) != Int(NUMMINUS))
      cerr << L"Internal error in ConvASTRec2CPP (1)\n";
    return Int (-1 * (int)cid.get_val().GetValue());
    
  }
  case TAG_TYPE_AS_RecordConstructorExpr: {
    TYPE_AS_Name tagname (ast.GetField(1));
    Sequence fields (ast.GetField(2));
    wstring modulename (ASTAUX::GetFirstId(tagname).GetString());
    wstring sec_name;
    if ( tagname.get_ids().Length() == 2 ) {
      sec_name = ASTAUX::GetSecondId(tagname).GetString();
    }
    else {
      cerr << L"Internal error in ConvASTRec2CPP (2)\n";
    }
      
    wstring name = sec_name + L"_" + modulename;
    int tag = INT2Q::Quote2Integer(Quote(name)).GetValue();
    int len = fields.Length();
    Record res(tag, len);

    if (tag == StringLit_CPP) {
      if (Record (fields[1]).Is(TAG_TYPE_AS_SeqEnumerationExpr) ) {
        Sequence seq ( ((Record) fields[1]).GetField(1));
        if (seq.IsEmpty()) {
          wstring emptystr = L"";
          Token t (emptystr);
          res.SetField(1, t);
          return res;
        }
      }
    }

    for (int i = 1; i <= len; i++) {
      res.SetField(i, ConvAST2CPP(fields[i]));
    }
    return res;
  }
  case TAG_TYPE_AS_SetEnumerationExpr : {
    Set res;
    Sequence els (ast.GetField(1));
    Generic e_g;
    for (bool bb = els.First(e_g); bb; bb = els.Next(e_g)) {
      res.Insert(ConvAST2CPP(e_g));
    }
    return res;
  }
  case TAG_TYPE_AS_SeqEnumerationExpr : {
    Sequence res;
    Sequence els (ast.GetField(1));
    Generic e_g;
    for (bool bb = els.First(e_g); bb; bb = els.Next(e_g)) {
      res.ImpAppend(ConvAST2CPP(e_g));
    }
    return res;
  }
  case TAG_TYPE_AS_BoolLit :
  case TAG_TYPE_AS_RealLit :
  case TAG_TYPE_AS_NumLit :
  case TAG_TYPE_AS_CharLit : {
    return (ast.GetField(1));
  }
  case TAG_TYPE_AS_NilLit : {
    return Nil();
  }
  case TAG_TYPE_AS_QuoteLit : {
    Sequence q(ast.GetField(1));
    wstring q_name;
    q.GetString(q_name);
    return Quote(q_name);
  }
  case TAG_TYPE_AS_TextLit : {
    Sequence seq (ast.GetField(1));
    wstring str;
    if (! seq.GetString(str))
      cerr <<L"Internal error in ConvASTRec2CPP: \n"
              <<L"TextLit\n";
    else
      return (Sequence(str));
  }
  default: {
    cerr <<L"Internal error in ConvASTRec2CPP: \n"
            <<L"Unknown AS tag\n";
  }
  }
  // Dummy return for VC++
  return Generic(); 
}


Generic ConvAST2CPP(Generic ast)
{
  if (ast.IsRecord()) {
    return ConvASTRec2CPP(ast);
  }
  else {
    cerr << L"Internal Error in ConvAST2CPP. Met unknown type" << endl;
    return Generic();
  }
}

//CLIOptions cliOpt;
//const TYPE_CI_ContextId NilContextId = -1;

ContextInfo::~ContextInfo()
{
}

int main(int argc, char* argv[]) 
{
  try {
    // Parse the file

    if (argc != 3) {
      cout << L"Usage: " << argv[0] << " cpp|java file" << endl;
      return -1;
    }

    Generic res;
    //  ContextInfo ci;
    init_AS();
    init_CPP();
    //  bool parse_error = ParseAnExpr(exprs,ci);
    bool parse_ok = CGUTIL::ParseVDMValue(TBWSTR::string2wstring(argv[2]), res);

    if (!parse_ok) {
      cerr << "Some error occurred during parsing of the expression" << endl;
      exit(1);
    }

    enum CGBackEnd::cg_backend_kind kind;

    if (strcmp(argv[1],"java")==0) 
      kind = CGBackEnd::JAVA;
    else if (strcmp(argv[1],"cpp")==0)
      kind = CGBackEnd::CPP;
    else {
      cerr << "Backend called with a wrong argument: " << argv[1] << endl;
      exit(1);
    }
  
    CGBackEnd be;
    be.GenFiles(res, kind);

    return 0;
  }
  catch (...) {
    cerr << "Exiting" << endl;
    exit( 128);
  }
}

