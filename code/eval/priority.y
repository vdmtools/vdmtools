/***
*  * WHAT
*  *    bison source file for priority file parser
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/priority.y,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2006/01/20 02:16:52 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  *
*  * SPECIFICATION VERSION
*  *    implementation of schdtp.vdm version ??
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

%{
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "metaiv.h"
#include "SCHDTP.h"
#include "tb_wstring.h"
#include "tools.h"

#if !defined(__darwin__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif // !defined(__darwin__) && !defined(__FreeBSD__)

#define YYDEBUG 1

extern FILE *pf_in;
extern int pf_error(const char *s);
extern int pf_lex();
extern void pf_reset();
extern wostream* pf_err_stream;
extern int pf_lineno_local; 
extern int pf_debug;

static SEQ<TYPE_SCHDTP_PriorityEntry>* pf = new SEQ<TYPE_SCHDTP_PriorityEntry>();
%}

%union {
  wchar_t* charp;
  int val;
  Sequence* sequence;
  Record* record;
}

%token <charp> NUMERICLITERAL
%token <charp> LEX_CLASSNAME
%type <record> priorityentry
%type <sequence> priorityentries
%type <sequence> priorityfile

%%

priorityfile:        
                 { }
               | priorityentries
                 { *pf = (SEQ<TYPE_SCHDTP_PriorityEntry>)*$1;
                 }
                 
               ;

priorityentries:   priorityentry
                 { $$ = new SEQ<TYPE_SCHDTP_PriorityEntry>();
                   $$ -> ImpAppend(*$1);
                   delete $1;}
               | priorityentries ';' priorityentry
                 { $1->ImpAppend(*$3);
                   delete $3;}
               ;

priorityentry:   LEX_CLASSNAME ':' NUMERICLITERAL
                 { $$ = new TYPE_SCHDTP_PriorityEntry();
                   $$->SetField(1, Sequence(wstring($1))); 
                   $$->SetField(2, Int(atoi(TBWSTR::wstring2string($3).c_str())));
                 }


%%

// It is assumed that fname exists
SEQ<TYPE_SCHDTP_PriorityEntry> pf_main(wostream & os, const wstring & fname)
{
// 20060120 for i18n
// pf_in = fopen(wstring2string(wstring(fname)).c_str(), "r");

  ifstream istr(TBWSTR::wstring2fsstr(fname).c_str());
  string ufname;
  TOOLS::ifstr2ufile(istr, ufname);
  istr.close();
  pf_in = fopen(ufname.c_str(), "r");

  pf_lineno_local = 1;
  pf_err_stream = &os;

  //pf_debug = 1;
  int parse_result = pf_parse();
  fclose(pf_in);
  pf_in = (FILE*) 0;
  pf_reset();

  if(!(parse_result == 0))
    return SEQ<TYPE_SCHDTP_PriorityEntry>();
  else
    return *pf;
}

