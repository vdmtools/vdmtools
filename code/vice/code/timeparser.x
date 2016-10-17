/***
*  * WHAT
*  *    flex source for timefile parser (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/vice/code/timeparser.x,v $
*  * VERSION
*  *    $Revision: 1.8 $
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
*  *    implementation of timeparser.vdm version 1.2
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

%{
#include <string.h>
#include <iostream>
#include "metaiv.h"
#include "TIMEPARSER.h"
#include "timeparser.tab.h"
#include "tb_wstring.h"

#ifdef _MSC_VER
//define isatty
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif // _MSC_VER

#define YY_NEVER_INTERACTIVE 1
#define YY_ALWAYS_INTERACTIVE 0
wostream *err_stream;
int tp_lineno_local=0;
wchar_t* mk_string(void);
%}
/*
%option noyywrap
*/
%%

"+"                return(LEX_TIME_Plus);
"-"                return(LEX_TIME_Minus);
"*"                return(LEX_TIME_Mult);
"/"                return(LEX_TIME_Div);
"rem"              return(LEX_TIME_Rem);
"mod"              return(LEX_TIME_Mod);
"intdiv"           return(LEX_TIME_Intdiv);
"and"              return(LEX_TIME_And);
"or"               return(LEX_TIME_Or);
"="                return(LEX_TIME_Equal);
">"                return(LEX_TIME_GreaterThan);
">="               return(LEX_TIME_GreaterThanOrEqual);
"abs"              return(LEX_TIME_Abs);
"floor"            return(LEX_TIME_Floor);
"not"              return(LEX_TIME_Not);

AddEnv             return(LEX_TIME_AddEnv);
Addset             return(LEX_TIME_Addset);
AllorExists        return(LEX_TIME_AllorExists);
Always             return(LEX_TIME_Always);
Appendmap          return(LEX_TIME_Appendmap);
Appendseq          return(LEX_TIME_Appendseq);
Appendtup          return(LEX_TIME_Appendtup);
Apply              return(LEX_TIME_Apply);
BindList           return(LEX_TIME_BindList);
BlockStmt          return(LEX_TIME_BlockStmt);
Br                 return(LEX_TIME_Br);
Call               return(LEX_TIME_Call);
Cbr                return(LEX_TIME_Cbr);
Exit               return(LEX_TIME_Exit);
Fieldsel           return(LEX_TIME_Fieldsel);
Guard              return(LEX_TIME_Guard);
History            return(LEX_TIME_History);
Iota               return(LEX_TIME_Iota);
Isexpr             return(LEX_TIME_Isexpr);
Narrowexpr         return(LEX_TIME_Narrowexpr);
Isofbaseclass      return(LEX_TIME_Isofbaseclass);
Isofclass          return(LEX_TIME_Isofclass);
LambdaExpr         return(LEX_TIME_LambdaExpr);
LetBeST            return(LEX_TIME_LetBeST);
Lookup             return(LEX_TIME_Lookup);
Loop               return(LEX_TIME_Loop);
MapInverse         return(LEX_TIME_MapInverse);
MatchPattern       return(LEX_TIME_MatchPattern);
Newobj             return(LEX_TIME_Newobj);
NonDet             return(LEX_TIME_NonDet);
Pattern            return(LEX_TIME_Pattern);
Polyinst           return(LEX_TIME_Polyinst);
RecTrap            return(LEX_TIME_RecTrap);
Reccons            return(LEX_TIME_Reccons);
Recmod             return(LEX_TIME_Recmod);
Return             return(LEX_TIME_Return);
Samebaseclass      return(LEX_TIME_Samebaseclass);
Sameclass          return(LEX_TIME_Sameclass);
Selfexpr           return(LEX_TIME_Selfexpr);
Seqlen             return(LEX_TIME_Seqlen);
Seqmapover         return(LEX_TIME_Seqmapover);
Setcard            return(LEX_TIME_Setcard);
Setrng             return(LEX_TIME_Setrng);
Start              return(LEX_TIME_Start);
Stop               return(LEX_TIME_Stop);
Subseq             return(LEX_TIME_Subseq);
Threadid           return(LEX_TIME_Threadid);
Trap               return(LEX_TIME_Trap);
Tupsel             return(LEX_TIME_Tupsel);
Update             return(LEX_TIME_Update);

add                return(LEX_ASSEMBLY_Add);
branch             return(LEX_ASSEMBLY_Branch);
call               return(LEX_ASSEMBLY_Call);
compare            return(LEX_ASSEMBLY_Compare);
"return"           return(LEX_ASSEMBLY_Return);
sub                return(LEX_ASSEMBLY_Sub);
div                return(LEX_ASSEMBLY_Div);
mul                return(LEX_ASSEMBLY_Mul);
neg                return(LEX_ASSEMBLY_Neg);
fsqrt              return(LEX_ASSEMBLY_Fsqrt);
logic              return(LEX_ASSEMBLY_Logic);
cas                return(LEX_ASSEMBLY_Cas);
push               return(LEX_ASSEMBLY_Push);
pop                return(LEX_ASSEMBLY_Pop);



bool               return(LEX_TIME_BOOL);
nat                return(LEX_TIME_NAT);
nat1               return(LEX_TIME_NATONE);
int                return(LEX_TIME_INT);
rat                return(LEX_TIME_RAT);
real               return(LEX_TIME_REAL);
char               return(LEX_TIME_CHAR);
token              return(LEX_TIME_TOKEN);
"("                return ('(');
")"                return (')');
":"                return(':');
";"                return(';');
[0-9]+             {  
                      tp_lval.str=mk_string();  return(NUMERICLITERAL); }
[ \t]              ;
#.*\n              tp_lineno_local++;
\r                 ;
\n                 tp_lineno_local++;
[a-zA-Z]+          (*err_stream) << L"unrecognized word: " << tp_text << endl;
.                  (*err_stream) << L"unrecognized character: " << tp_text << endl;
%%

wchar_t* mk_string(void)
{
  wchar_t* ptr;

  if(!(ptr=(wchar_t*)malloc((tp_leng+1)*sizeof(wchar_t))))
  {
    fprintf(stderr,"tp_lex:No more memory available\n");
    exit(1);
  }
  string text(tp_text);
  wcsncpy(ptr, TBWSTR::string2wstring(text).c_str(), tp_leng+1);
  return (ptr);
}

int tp_error(const char *s)
{
  (*err_stream) <<  TBWSTR::string2wstring(string(s)) << L": on line " << tp_lineno_local;
  if (tp_text[0])
  {
    (*err_stream) << L" near symbol \"" << tp_text << L"\"";
  }
  (*err_stream) << endl;                 
  return 0;
}

void tp_reset()
{
//  yy_flush_buffer(yy_current_buffer);
  YY_FLUSH_BUFFER;
}

int tp_wrap()
{
  return 1;
}
