/***
*  * WHAT
*  *    flex source for priority file parser (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/priority.x,v $
*  * VERSION
*  *    $Revision: 1.7 $
*  * DATE
*  *    $Date: 2006/05/17 08:18:31 $
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
#include <string.h>
#include <iostream>
#include "metaiv.h"
#include "priority.tab.h"
#include "tb_wstring.h"

#ifdef _MSC_VER
//define isatty
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif // _MSC_VER

#define YY_NEVER_INTERACTIVE 1
#define YY_ALWAYS_INTERACTIVE 0
wostream *pf_err_stream;
int pf_lineno_local=0;
wchar_t* mk_charp(void);
%}
/*
%option noyywrap
*/

hexdigit                [0-9ABCDEF]|[0-9abcdef]
hexquad                 {hexdigit}{hexdigit}{hexdigit}{hexdigit}

universalcharactername  (\\u{hexquad})|(\\U{hexquad})

letter                  ([A-Za-z]|{universalcharactername})

/*
letter                  [A-Za-z]
*/
identifierorkeyword    {letter}([0-9\'_]|{letter})*

%%
{identifierorkeyword}*          {
                      pf_lval.charp=mk_charp(); return(LEX_CLASSNAME);
                   }
":"                { return(':'); }
";"                { return(';'); }
[0-9]+             {  
                      pf_lval.charp=mk_charp();  return(NUMERICLITERAL); }
[ \t]              ;
#.*\n              pf_lineno_local++;
\n                 pf_lineno_local++;
.                  (*pf_err_stream) << L"unrecognized character: " << TBWSTR::string2wstring(pf_text) << endl;
%%

wchar_t* mk_charp(void)
{
  wchar_t* ptr;

  if(!(ptr=(wchar_t*)malloc((pf_leng+1)*sizeof(wchar_t))))
  {
    fprintf(stderr, "pf_lex:No more memory available\n");
    exit(1);
  }
  string text(pf_text);
  wcsncpy(ptr, TBWSTR::string2wstring(text).c_str(), pf_leng+1);
  return (ptr);
}

int pf_error(const char *s)
{
  (*pf_err_stream) << TBWSTR::string2wstring(string(s)) 
                   << L": on line " << pf_lineno_local;
  if (pf_text[0])
  {
    (*pf_err_stream) << L" near symbol \"" << TBWSTR::string2wstring(pf_text) << L"\"";
  }
  (*pf_err_stream) << endl;                 
  return 0;
}

void pf_reset()
{
//  yy_flush_buffer(yy_current_buffer);
  YY_FLUSH_BUFFER;
}

int pf_wrap()
{
  return 1;
}
