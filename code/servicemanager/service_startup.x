/*
$Id: service_startup.x,v 1.7 2006/05/17 08:18:31 vdmtools Exp $
*/

%{
#include <string.h>
#include <iostream>
#include "metaiv.h"
#include "tb_wstring.h"
#include "ServiceManagerDesc.h"
#include "service_startup.tab.h"

#ifdef _MSC_VER
//define isatty
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif // _MSC_VER

#define YY_NEVER_INTERACTIVE 1
#define YY_ALWAYS_INTERACTIVE 0

#ifdef _MSC_VER
#define SEPCHAR '\\'
#else
#define SEPCHAR '/'
#endif //_MSC_VER

int smd_lineno_local;
wostream *smd_err_stream;
char* smd_charp(void);
%}

/*
%option noyywrap
*/

letter                  [A-Za-z\.]
identifierorkeyword    {letter}([0-9\'_]|{letter})*

filepath         {identifierorkeyword}("/"{identifierorkeyword})*
name             {identifierorkeyword}

%%
{identifierorkeyword} smd_lval.charp = smd_charp(); return (LEX_NAME);
"/"              return ('/');
":"              return (':');
"<description>"  return (LEX_BEGIN_DESCRIPTION);
"</description>" return (LEX_END_DESCRIPTION);
"<name>"         return (LEX_BEGIN_NAME);
"</name>"        return (LEX_END_NAME);
"<location>"     return (LEX_BEGIN_LOCATION);
"</location>"    return (LEX_END_LOCATION);
[ \t]            ;
#.\n             smd_lineno_local++;
\n               smd_lineno_local++;
{letter}         smd_lval.charp = smd_charp(); return (LEX_CHAR);
.                (*smd_err_stream) << L"unrecognized character: " << TBWSTR::string2wstring(smd_text) << endl;

%%

char* smd_charp(void)
{
  char* ptr;

  if(!(ptr=(char*)malloc(smd_leng+1)))
  {
    fprintf(stderr,"smd_lex:No more memory available\n");
    exit(1);
  }
  strncpy(ptr,smd_text,smd_leng+1);
  return (ptr);
}

int smd_error(const char *s)
{
  (*smd_err_stream) << TBWSTR::string2wstring(s) << L": on line " << smd_lineno_local;
  if (smd_text[0])
  {
    (*smd_err_stream) << L" near symbol \"" << TBWSTR::string2wstring(smd_text) << L"\"";
  }
  (*smd_err_stream) << endl;                 
  return 0;
}

void smd_reset()
{
//  yy_flush_buffer(yy_current_buffer);
  YY_FLUSH_BUFFER;
}

int smd_wrap()
{
  return 1;
}
