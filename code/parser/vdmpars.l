@Lv @% This is a fweb document to be processed by ftangle and fweave

@* This fweb document contains the source for the VDM-SL and VDM++ Tools. 

@% Comment: at-sign (@) is the command prefix and must be duplicated (@@)
@% where is is needed verbatim. 

@a@% Here comes the code: 
%{
/*** 
*  * WHAT
*  *   Lexical analysis of VDMSL and VDM++
*  * Id
*  *   $Id: vdmpars.l,v 1.75 2006/03/15 08:43:45 vdmtools Exp $
*  * COPYRIGHT
*  *   Kyushu University 2016
***/

/******************************************************************************/
/** definitions part                                                         **/
/******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#ifndef __APPLE_CC__
#ifndef __FreeBSD__
#include <malloc.h>
#endif
#endif

#include "scanner.h"
#include "tb_wstring.h"
#include "settings.h"

#include <deque>
#include <map>

@#if VDMSL
#include "parssl.tab.h"
@#endif
@#if VICE
#include "parsrt.tab.h"
@#else
@#if VDMPP
#include "parspp.tab.h"
@#endif //VDMPP
@#endif //VICE
  
#ifdef _MSC_VER
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif // _MSC_VER

#define YY_NEVER_INTERACTIVE 1

int LXINPUT::String_getchars (char*p, int maxsize)
{
  if (s_index == (int) st.length())  {
    return 0;
  }
  else {
    int rest = st.length() - s_index; 
    int m = rest < maxsize ? rest : maxsize; 
    memcpy(p, &st[s_index], m);
    s_index += m;
    return m;
  }
}

int LXINPUT::Stream_getchars (char*p, int maxsize)
{
  if (!is->fail()) {
    is->read(p, maxsize);
    return is->gcount();
  } else {
    return 0;
  }
}

void LXINPUT::ReadFrom(const string & s) 
{
  st = s; 
  s_index = 0; 
  current_getchars = &LXINPUT::String_getchars;
  current_reread = &LXINPUT::String_reread; 
}

void LXINPUT::ReadFrom(istream & s) 
{
  is = &s;
@#if VDMPP
  yy_switch_to_buffer (yy_create_buffer (0, YY_BUF_SIZE));
@#endif //VDMPP
  current_getchars = &LXINPUT::Stream_getchars; 
  current_reread = &LXINPUT::Stream_reread; 
}

#define DONE                    1
#define TAB_WIDTH               1 
/* Safe assumption as a tab is only one character anyway */
#define SPACE_WIDTH             1
#define LETTER_WIDTH            1
/************** #define PREFIX_SEPARATOR        '-' **********************/
/* #define LEXDEBUG */

LexPos SCANNER::CurrentPos;
LexPos SCANNER::NextPos; 

/////////////////////////
// Handling of sections and the --LINE directive
// Section line counts are relative to the current section
/////////////////////////


///////////////////////////////////
string SCANNER::yytextstring;
                                 /* This is assigned by INSERT_TOKEN2
                                 * access this instead of using
                                 * yytext.
                                 * 
                                 * Accessing extern char *yytext in
                                 * other files causes problems when
                                 * yytext has been modified by unput.
                                 * */

//#if VDMPP
// These functions are used to mark if the last token was LEX_CLASS
// If so, then the identifier following the token is the class name,
// which is used later to decide if an operation is a constructor or not.
bool SCANNER::lastWasLEX_CLASS_MOD;
//#endif //VDMPP

std::deque<string> SCANNER::Saved_Strings;

void SCANNER::Save_LexStringP(YYSTYPE*yylval, const char *p)
{
  Saved_Strings.push_front(string(p)); 
  yylval->LexStringP = &Saved_Strings[0];
}

void (*(SCANNER::ReportP)) (const wchar_t*, YYLTYPE & pos);

const char * SCANNER::skipwsafter(const char*s, const char *after) 
{
  const char *p = ::strstr(s, after); 
  if (!p) {
    return 0; 
  }
  p += strlen(after); 
  while (*p && isspace(*p)) {
    p++; 
  }
  return p; 
}

@#if VDMSL
int parssllex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int pexprsllex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
@#endif 

@#if VICE
int parsrtlex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int pexprrtlex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int pstmtrtlex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int pexprrttclex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
@#endif

@#if VDMPP
int parspplex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int pexprpplex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int pstmtpplex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
int exprpptclex(YYSTYPE *yylval, YYLTYPE*yylpos) {return yylex(yylval, yylpos);}
@#endif 

%}

hexdigit                [0-9A-Fa-f]
hexquad                 {hexdigit}{hexdigit}{hexdigit}{hexdigit}

universalcharactername  (\\u{hexquad})|(\\U{hexquad})

letter                  ([A-Za-z]|#[A-Za-z]|{universalcharactername})
digit                   [0-9]
prime                   \`
hyphen                  _
hook                    ~

rtfuniversalcharacter  \\u{hexquad}[A-Za-z]

identifierorkeyword    {letter}([0-9\'_]|{letter})*

numericliteral         {digit}+|0x{hexdigit}+|0X{hexdigit}+
realliteral            [0-9]+(("."[0-9]+)|([Ee]("+"|"-")?[0-9]+)|("."[0-9]+[Ee]("+"|"-")?[0-9]+))

embeddedctrlchar           [\000-\037]
/* backslashed: 
 * \\
 * \cX
 * \xFF
 * \n \r \a \b \t \v \e \f
 * \031
 */
backslashed            \\c.|\\x..|\\[\\nrabtvef\'\"]|\\[0-3][0-7][0-7]
highbitchar            [\200-\377]
deletechar             \177

/* Every char in the ascii interval 32-126 except \ (0134, 0x5C)
 * where \. is accepted; the parser validates .
 * Embedded control characters are also allowed. 
 */
characterliteral       "'"([\040-\133\135-\176]|{embeddedctrlchar}|{backslashed}|{deletechar}|{highbitchar}|{universalcharactername}|{rtfuniversalcharacter})"'"

doublequote            \"\"
/* Every char in the ascii interval 32-126 except " which must be written as "" 
 * But for now "" is not included 
 */
textliteral            \"([\040-\041\043-\133\135-\176]|{embeddedctrlchar}|{backslashed}|{deletechar}|{highbitchar}|{universalcharactername})*\"

whitespace              ([ \t]+)
separators              (({whitespace}|[\r\n])+)
separator               ([ \t\r\n])
opt_whitespace          ([ \t]*)

newline                 "\n"

quoteliteral           \<{identifierorkeyword}\>

decimalpoint           "."
exponentsign           "E"

comments                --.*

linedirec             ^--LINE

ccommentbegin		"/*"
ccommentend		"*/"

%a 1500
%o 2000
%p 1500

%%

{linedirec}{whitespace}{numericliteral}{whitespace}{numericliteral}{opt_whitespace} {
                          int sec, line;
                          SCANNER::AdjustLineColumn(yytext);
                          sscanf(yytext,"--LINE %d %d",&sec,&line);
                          SCANNER::SetNewLine(sec, line);
                        }

{comments}              { SCANNER::AdjustLineColumn(yytext);
                          //yytext[strlen(yytext)-1] = 0; // remove last newline in text
                          SCANNER::INSERT_TOKEN2(yylpos, LEX_COMMENT,yytext);
                        }
"/*"			{  // TOKEN LIST ??? LTO
                           char commentchar[2];
                           commentchar[1] = '\0';

			   SCANNER::AdjustLineColumn (yytext);
			
			 loop:
			   while (((commentchar[0] = yyinput()) !='\0') && (commentchar[0] != '*'))
			     SCANNER::AdjustLineColumn (commentchar);
			   
			   switch ((commentchar[0] = yyinput()))
			     {
			     case '\0': break;
			     case '/': break;
			     case '*': unput('*');
			     default: SCANNER::AdjustLineColumn (commentchar); goto loop;
			     }
			   SCANNER::AdjustLineColumn (commentchar);
			}
{hook}                  {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_HOOK, yytext);
                           return (LEX_HOOK);
                        }
{characterliteral}	{  SCANNER::AdjustLineColumn (yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_char_lit, yytext);
                           // Strip the quotes from the character
                           int64_t i;
                           for (i = 0; (yytext[i] = yytext[i+1]) != '\0'; i++); 
                           yytext[i-1] = '\0';
                           SCANNER::Save_LexStringP(yylval, yytext);
                           return (LEX_char_lit);
                        }
{quoteliteral}		{  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_quote_lit, yytext);
                           // Strip the angle brackets from the quote
                           int64_t i;
                           for (i = 0; (yytext[i] = yytext[i+1]) != '\0'; i++); 
                           yytext[i-1] = '\0';
                           SCANNER::Save_LexStringP(yylval, yytext);
                           return(LEX_quote_lit);
                        }
{textliteral}           {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_text_lit, yytext);
                           // Strip the quotes from the string
                           int64_t i;
                           for (i = 0; (yytext[i] = yytext[i+1]) != '\0'; i++);
                           yytext[i-1] = '\0';
                           SCANNER::Save_LexStringP(yylval, yytext);
                           return(LEX_text_lit);
                        }
{realliteral}           {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_real_lit, yytext);
                           SCANNER::Save_LexStringP(yylval, yytext); 
                           return (LEX_real_lit);
                        }
{numericliteral}        {  SCANNER::AdjustLineColumn(yytext);
                           //SCANNER::INSERT_TOKEN2(yylpos, LEX_real_lit, yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_int_lit, yytext);
                           SCANNER::Save_LexStringP(yylval, yytext);
                           //return (LEX_real_lit);
                           return (LEX_int_lit);
                        }
{prime}                 {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_PRIME, NULL);
                           return (LEX_PRIME);
                        }
in{separators}{identifierorkeyword}       {  
                           // This can match in set, in v, in settp 
                           // Check for in set 
                           const char *id = SCANNER::skipwsafter(yytext, "in"); 
                           // now id points to first non-space char after in
                           if (strcmp(id, "set") == 0) { 
                             SCANNER::AdjustLineColumn(yytext);
                             SCANNER::INSERT_TOKEN2(yylpos, LEX_IN_SET, yytext);
                             return (LEX_IN_SET);    
                           }
                           else if (strcmp(id, "seq") == 0) { 
                             SCANNER::AdjustLineColumn(yytext);
                             SCANNER::INSERT_TOKEN2(yylpos, LEX_IN_SEQ, yytext);
                             return (LEX_IN_SEQ);    
                           }
                           else {
                             // push back separators and identifierorkeyword
                             // lex_unput changes yytext.
                             SCANNER::AdjustLineColumn("in");
                             SCANNER::INSERT_TOKEN2(yylpos, LEX_IN,"in");
                             SCANNER::lex_unput(yytext+strlen("in")); 
                             return LEX_IN; 
                           }
                        }
not{separators}in{separators}{identifierorkeyword}  {  
                          // Check for not in set.
                           const char *id = SCANNER::skipwsafter(yytext, "in"); 
                           if (strcmp(id, "set") == 0) {
                              SCANNER::AdjustLineColumn(yytext);
                              SCANNER::INSERT_TOKEN2(yylpos, LEX_NOT_IN_SET,yytext);
                              return (LEX_NOT_IN_SET);    
                           } else {
                             // Push back `{separators} in identifierorkeyword'
                             // and return LEX_NOT
                             // lex_unput changes yytext.
                             SCANNER::AdjustLineColumn("not");
                             SCANNER::INSERT_TOKEN2(yylpos, LEX_NOT,"not");
                             SCANNER::lex_unput(yytext+strlen("not")); 
                             return LEX_IN; 
                           }
                        }
"is_"/{identifierorkeyword}  {  
                           SCANNER::AdjustLineColumn(yytext); 
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_IS_,yytext);
			   return (LEX_IS_);
                        }
"mk_"/{identifierorkeyword}  {  
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_MK_,yytext);
			   return (LEX_MK_);
                        }
@#if VDMPP
"obj_"/{identifierorkeyword}  {  
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_OBJ_,yytext);
			   return (LEX_OBJ_);
                        }
@#endif //VDMPP
"__FILE__"              {
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_FILE_MACRO,yytext);
			   return (LEX_FILE_MACRO);
                        }
"__LINE__"              {
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LINE_MACRO,yytext);
			   return (LEX_LINE_MACRO);
                        }
"__COLUMN__"            {
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LINE_MACRO,yytext);
			   return (LEX_COLUMN_MACRO);
                        }
"__CLMOD__"              {
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LINE_MACRO,yytext);
			   return (LEX_CLMOD_MACRO);
                        }
"__FNOP__"              {
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LINE_MACRO,yytext);
			   return (LEX_FNOP_MACRO);
                        }
{identifierorkeyword}	{  int Token;
                           SCANNER::AdjustLineColumn(yytext);
                           SCANNER::Save_LexStringP(yylval, yytext);
			   if ((Token = SCANNER::SearchReservedWord (yytext)) >= 0) {
@#if VDMSL
                             if (Token == LEX_MODULE || Token == LEX_DLMODULE) {
                               SCANNER::setLastWasLEX_CLASS_MOD (true);
                             }
@#endif //VDMSL
@#if VDMPP
                             if (Token == LEX_CLASS || Token == LEX_DLCLASS
@#if VICE
                                 || Token == LEX_SYSTEM
@#endif VICE
                             ) {
                               SCANNER::setLastWasLEX_CLASS_MOD (true);
                             }
@#endif //VDMPP
                             SCANNER::INSERT_TOKEN2(yylpos, Token, yytext);
                             return (Token); 
                           } else {
                             if (Settings.Assertion()) {
                               if (strcmp(yytext, "assert") == 0) {
                                 SCANNER::INSERT_TOKEN2(yylpos, LEX_ASSERT,yytext);
			         return (LEX_ASSERT);
                               }
                             }
                             if (Settings.MapOne()) { 
                               if (strcmp(yytext, "map1") == 0) {
                                 SCANNER::INSERT_TOKEN2(yylpos, LEX_MAP1,yytext);
			         return (LEX_MAP1);
                               }
                               else if (strcmp(yytext, "inmap1") == 0) {
                                 SCANNER::INSERT_TOKEN2(yylpos, LEX_INMAP1,yytext);
			         return (LEX_INMAP1);
                               }
                             }
                             SCANNER::INSERT_TOKEN2(yylpos, LEX_identifier,yytext);
                             return (LEX_identifier);
                           }
                        }
\${identifierorkeyword}	{  SCANNER::AdjustLineColumn(yytext);
                           // Do not save the $ from the name. 
                           SCANNER::Save_LexStringP(yylval, yytext+1); 
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_dollar_identifier,yytext);
                           return (LEX_dollar_identifier);		   
                         }
{separator}             {  SCANNER::AdjustLineColumn (yytext);
                        }

%{
/** Section in reference document: 10.3 ***************************************/
/** Deviations from the standard/Remarks:                                    **/
/** - The rule for '()' has been omitted, it is scanned as '(' followed by   **/
/**   ')'.                                                                   **/
/** - Symbols consisting of a number of letters can be found in the reserved **/
/**   word table (see below).                                                **/
%}

","                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, ',',NULL);
                           return (',');
                        }
@#if VDMPP
"!"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '!',NULL);
                           return ('!');
                        }
@#endif //VDMPP
":"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, ':',NULL);
                           return (':');
                        }
":-"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_DONTCARE,NULL);
                           return (LEX_DONTCARE);
                        }
";"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, ';',NULL);
                           return (';');
                        }
"="                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_EQUAL,NULL);
                           return (LEX_EQUAL);
                        }
")"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, ')',NULL);
                           return (')');
                        }
"("                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '(',NULL);
                           return ('(');
                        }
"|"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '|',NULL);
                           return ('|');
                        }
"-"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '-',NULL);
                           return ('-');
                        }
"["                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '[',NULL);
                           return ('[');        
                        }
"]"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, ']',NULL);
                           return (']');
                        }
"@@"                     { SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '@@',NULL);
                           return ('@@');
                        }
"{"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '{',NULL);
                           return ('{');
                        }
"}"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '}',NULL);
                           return ('}');
                        }
"+"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '+',NULL);
                           return ('+');
                        }
"/"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '/',NULL);
                           return ('/');
                        }
"<"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LESS_THAN,NULL);
                           return (LEX_LESS_THAN);
                        }
">"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_GREATER_THAN,NULL);
                           return (LEX_GREATER_THAN);
                        }
"."                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '.',NULL);
                           return ('.');
                        }
".#"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_DOTHASH,NULL);
                           return (LEX_DOTHASH);
                        }
"&"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_RAISED_DOT,NULL);
                           return (LEX_RAISED_DOT);
                        }
"*"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, '*',NULL);
                           return ('*');
                        }
"<="                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LESS_THAN_OR_EQUAL,NULL);
                           return (LEX_LESS_THAN_OR_EQUAL);
                        }
">="                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_GREATER_THAN_OR_EQUAL,NULL);
                           return (LEX_GREATER_THAN_OR_EQUAL);
                        }
"<>"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_NOT_EQUAL,NULL);
                           return (LEX_NOT_EQUAL);
                        }
"==>"                   {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_OPERATION_ARROW,NULL);
                           return (LEX_OPERATION_ARROW);
                        }
"->"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_ARROW,NULL);
                           return (LEX_ARROW);
                        }
"+>"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_TARROW,NULL);
                           return (LEX_TARROW);
                        }
"=>"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_IMPLY,NULL);
                           return (LEX_IMPLY);
                        }
"<=>"                   {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LOGICAL_EQUIVALENCE,NULL);
                           return (LEX_LOGICAL_EQUIVALENCE);
                        }
"|->"                   {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_BAR_ARROW,NULL);
                           return (LEX_BAR_ARROW);
                        }
"=="                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_IS_DEFINED_AS,NULL);
                           return (LEX_IS_DEFINED_AS);
                        }
"||"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_NONDET,NULL);
                           return (LEX_NONDET);
                        }
":="                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_ASSIGN,NULL);
                           return (LEX_ASSIGN);
                        }
","({separator}*)"..."({separator}*)","  {  SCANNER::AdjustLineColumn (yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_RANGE_OVER, yytext);
                           return (LEX_RANGE_OVER);
                        }
"::"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_DOUBLE_COLON,NULL);
                           return (LEX_DOUBLE_COLON);
                        }
"**"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_EXP_OR_ITERATE,NULL);
                           return (LEX_EXP_OR_ITERATE);
                        }
"++"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_MODIFY_BY,NULL);
                           return (LEX_MODIFY_BY);
                        }
"<:"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_MAP_DOMAIN_RESTRICT_TO,NULL);
                           return (LEX_MAP_DOMAIN_RESTRICT_TO);
                        }
"<-:"                   {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_MAP_DOMAIN_RESTRICT_BY,NULL);
                           return (LEX_MAP_DOMAIN_RESTRICT_BY);
                        }
":>"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_MAP_RANGE_RESTRICT_TO,NULL);
                           return (LEX_MAP_RANGE_RESTRICT_TO);

                        }
":->"                   {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_MAP_RANGE_RESTRICT_BY,NULL);
                           return (LEX_MAP_RANGE_RESTRICT_BY);
                        }
"^"                     {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_SEQUENCE_CONCATENATE,NULL);
                           return (LEX_SEQUENCE_CONCATENATE);
                        }
"$$"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_LAST_RESULT,NULL);
                           return (LEX_LAST_RESULT);
                        }
"##!!#!"                {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_TEXBREAK,NULL);
                           return (LEX_TEXBREAK);
                        }
"\\"                    {  SCANNER::AdjustLineColumn(yytext);
                           SCANNER::INSERT_TOKEN2(yylpos, LEX_SET_MINUS,NULL);
                           return (LEX_SET_MINUS);
                        }
.			{  SCANNER::AdjustLineColumn(yytext);
			   return (unsigned char)(yytext[0]); // THE illegal char
			   // return (ILLEGAL_CHAR);
			   // fprintf(stderr, "Illegal character \"%s\" encountered.\n",yytext);
			   // fprintf(stderr, "Aborting...\n");
			   // exit(0);
			}
<<EOF>>                 {  SCANNER::IniPos(yylpos); // So we have the position of EOF. 
                           SCANNER::yytextstring = "";  
			   return 0; // EOF
                        }
			   
%%

/** Section in reference document: 9.10.3, 10.3 (7) ***************************/
/** Deviations from the standard/Remarks:                                    **/
/** - This table contains all the keywords, reserved words, and symbols      **/
/**   consisting of letters in BSI/VDM-SL.                                   **/
/** Note: In alphabetic order.                                               **/

static struct
   LexReservedWordTable
   {  const char* StringRepr;
      int Token;
   }
   LexReservedWord [] =
    {
@#if VDMPP
      {"#act",        LEX_ACT},
      {"#active",     LEX_ACTIVE}, 
      {"#fin",        LEX_FIN}, 
      {"#req",        LEX_REQ}, 
      {"#waiting",    LEX_WAITING},
@#endif //VDMPP 
      {"abs",         LEX_ABS},
      {"all",         LEX_ALL},
      {"always",      LEX_ALWAYS},
      {"and",         LEX_AND},
//      {"assert",      LEX_ASSERT},
@#if VICE
      {"async",       LEX_ASYNC},
@#endif //VICE
      {"atomic",      LEX_ATOMIC},
      {"be",          LEX_BE},
      {"bool",        LEX_BOOL},
      {"by",          LEX_BY},
      {"card",        LEX_CARD},
      {"cases",       LEX_CASES},
      {"char",        LEX_CHAR},
@#if VDMPP
      {"class",       LEX_CLASS},
@#endif //VDMPP
      {"comp",        LEX_COMP},
      {"compose",     LEX_COMPOSE},
      {"conc",        LEX_CONC},
@#if VICE
      {"cycles",      LEX_CYCLES},
@#endif //VICE
      {"dcl",         LEX_DCL},
      {"def",         LEX_DEF},
@#if VDMSL
      {"definitions", LEX_DEFINITIONS},
@#endif //VDMSL
      {"dinter",      LEX_DINTER},
      {"div",         LEX_ARITHMETIC_INTEGER_DIVISION},
@#if VDMSL
      {"dlmodule",    LEX_DLMODULE},
@#endif //VDMSL
@#if VDMPP
      {"dlclass",     LEX_DLCLASS},
@#endif //VDMSL
      {"do",          LEX_DO},
      {"dom",         LEX_DOM},
      {"dunion",      LEX_DUNION},
@#if VICE
      {"duration",    LEX_DURATION},
@#endif //VICE
      {"elems",       LEX_ELEMS},
      {"else",        LEX_ELSE},
      {"elseif",      LEX_ELSEIF},
      {"end",         LEX_END},
      {"eq",          LEX_EQ},
      {"error",       LEX_ERROR},
      {"errs",        LEX_ERRS},
      {"exists",      LEX_EXISTS},
      {"exists1",     LEX_EXISTS1},
      {"exit",        LEX_EXIT},
@#if VDMSL
      {"exports",     LEX_EXPORTS},
@#endif //VDMSL
      {"ext",         LEX_EXT},
      {"false",       LEX_bool_false},
      {"floor",       LEX_FLOOR},
      {"for",         LEX_FOR},
      {"forall",      LEX_FORALL},
@#if VDMSL
      {"from",        LEX_FROM},
@#endif //VDMSL
      {"functions",   LEX_FUNCTIONS},
      {"hd",          LEX_HD},
      {"if",          LEX_IF},
@#if VDMSL
      {"imports",     LEX_IMPORTS},
@#endif //VDMSL
      {"in",          LEX_IN},
      {"inds",        LEX_INDS},
@#if VDMSL
      {"init",        LEX_INIT},
@#endif
      {"inmap",       LEX_INMAP},
//      {"inmap1",       LEX_INMAP1},
@#if VDMPP
      {"instance",    LEX_INSTANCE},
@#endif //VDMPP
      {"int",         LEX_INT},
      {"inter",       LEX_SET_INTERSECTION},
      {"inv",         LEX_INV},
      {"inverse",     LEX_INVERSE},
      {"iota",        LEX_IOTA},
      {"is",          LEX_IS},
      {"is_",         LEX_IS_},
@#if VDMPP
      {"isofbaseclass", LEX_ISOFBASECLASS},
      {"isofclass",   LEX_ISOFCLASS},
@#endif //VDMPP
      {"lambda",      LEX_LAMBDA},
      {"len",         LEX_LEN},
      {"let",         LEX_LET},
      {"map",         LEX_MAP},
//      {"map1",         LEX_MAP1},
      {"measure",     LEX_MEASURE},
      {"merge",       LEX_DMERGE},
      {"mk_",         LEX_MK_},
      {"mod",         LEX_MOD},
@#if VDMSL
      {"module",      LEX_MODULE},
@#endif //VDMSL
      {"mu",          LEX_MU},
      {"munion",      LEX_MAP_MERGE},
@#if VDMPP
      {"mutex",       LEX_MUTEX},
@#endif //VDMPP
      {"narrow_",     LEX_NARROW_},
      {"nat",         LEX_NAT},
      {"nat1",        LEX_NATONE},
@#if VDMPP
      {"new",         LEX_NEW},
@#endif //VDMPP
      {"nil",         LEX_NIL},
      {"not",         LEX_NOT},
@#if VDMPP
      {"obj_",        LEX_OBJ_},
@#endif //VDMPP
      {"of",          LEX_OF},
      {"operations",  LEX_OPERATIONS},
      {"or",          LEX_OR},
      {"ord",         LEX_ORD},
      {"others",      LEX_OTHERS},
@#if VDMPP
      {"per",         LEX_PER},
      {"periodic",    LEX_PERIODIC},
@#endif //VDMPP
      {"post",        LEX_POST},
      {"power",       LEX_POWER},
      {"pre",         LEX_PRE},
      {"pre_",        LEX_PRECONDAPPLY},
@#if VDMPP
      {"private",     LEX_PRIVATE},
      {"protected",   LEX_PROTECTED},
@#endif //VDMPP
      {"psubset",     LEX_PROPER_SUBSET},
@#if VDMPP
      {"public",      LEX_PUBLIC},
@#endif //VDMPP
      {"pure",        LEX_PURE},
      {"rat",         LEX_RAT},
      {"rd",          LEX_RD},
      {"real",        LEX_REAL},
      {"rem",         LEX_REM},
@#if VDMSL
      {"renamed",     LEX_RENAMED},
@#endif //VDMSL
@#if VDMPP
      {"responsibility", LEX_RESPONSIBILITY},
@#endif //VDMPP
      {"return",      LEX_RETURN},
      {"reverse",     LEX_REVERSE},
      {"rng",         LEX_RNG},
@#if VDMPP
      {"samebaseclass", LEX_SAMEBASECLASS},
      {"sameclass",   LEX_SAMECLASS},
      {"self",        LEX_SELF},
@#endif //VDMPP
      {"seq",         LEX_SEQ},    
      {"seq1",        LEX_SEQ1},    
      {"set",         LEX_SET},    
      {"set1",        LEX_SET1},    
      {"skip",        LEX_SKIP},
      {"specified",   LEX_SPECIFIED},
@#if VDMPP
      {"sporadic",    LEX_SPORADIC},
@#endif //VDMPP
      {"st",          LEX_ST},
@#if VDMPP
      {"start",       LEX_START},
      {"startlist",   LEX_STARTLIST},
@#endif //VDMPP
@#if VDMSL
      {"state",       LEX_STATE},
@#endif //VDMSL
@#if VDMPP
      {"static",      LEX_STATIC},
      {"stop",        LEX_STOP},
      {"stoplist",    LEX_STOPLIST},
@#endif //VDMPP
@#if VDMSL
      {"struct",      LEX_STRUCT},
@#endif //VDMSL
@#if VDMPP
      {"subclass",    LEX_SUBCLASS},
@#endif //VDMPP
      {"subset",      LEX_SUBSET},
@#if VDMPP
      {"sync",        LEX_SYNC},
@#endif //VDMPP
@#if VICE
      {"system",      LEX_SYSTEM},
@#endif //VICE
      {"then",        LEX_THEN},
@#if VDMPP
      {"thread",      LEX_THREAD},
      {"threadid",    LEX_THREADID},
@#endif //VDMPP
@#if VICE
      {"time",        LEX_CURTIME},
@#endif //VICE
      {"tixe",        LEX_TIXE},
      {"tl",          LEX_TL},
      {"to",          LEX_TO},
      {"token",       LEX_TOKEN},
      {"traces",      LEX_TRACES},
      {"trap",        LEX_TRAP},
      {"true",        LEX_bool_true},
      {"types",       LEX_TYPES},
      {"undefined",   LEX_UNDEFINED}, 
      {"union",       LEX_SET_UNION},
      {"uselib",      LEX_USELIB},
      {"values",      LEX_VALUES},
@#if VDMPP
      {"variables",   LEX_VARIABLES},
@#endif //VDMPP
      {"while",       LEX_WHILE},
      {"with",        LEX_WITH},
      {"wr",          LEX_WR},
      {"yet",         LEX_YET},
      {NULL,          0} // end of table
   };   

std::map<string, int> SCANNER::ReservedWordMap1;
std::map<int, const char *> SCANNER::ReservedWordMap2;

class SetReservedWord {
public:
  SetReservedWord() {
//    const char error_out_of_order[] = "ReservedWord array is out of order: ";
//    size_t maxreservedword = 0;
//    while(LexReservedWord[maxreservedword].StringRepr != NULL) maxreservedword++;
//    maxreservedword--;
//    for (size_t i = 0; i < maxreservedword - 1; i++) {
//      if (strcmp(LexReservedWord[i].StringRepr, LexReservedWord[i+1].StringRepr) >= 0) {
//        write(2, error_out_of_order, strlen(error_out_of_order));
//        write(2, LexReservedWord[i].StringRepr, strlen(LexReservedWord[i].StringRepr));
//        write(2, "\n", 1);
//        exit(1);
//      }
//    }
    for (size_t idx = 0; LexReservedWord[idx].StringRepr != NULL; idx++) {
     SCANNER::SetReservedWord(LexReservedWord[idx].StringRepr, LexReservedWord[idx].Token);
    }
  }
  ~SetReservedWord() { SCANNER::ClearReservedWord(); }
}; 

static class SetReservedWord dummy_var; 

/** SearchReservedWord ********************************************************/
/** The function performs a binary search in the reserved word table to      **/
/** verify whether a certain string is a keyword (reserved word) or not. If  **/
/** it is found, the code to which the keyword corresponds is returned. If   **/
/** it is not found, then apparently it is an identifier and -1 is returned. **/
int SCANNER::SearchReservedWord (const char* Candidate)
{
  std::map<string, int>::const_iterator itr (ReservedWordMap1.find(string(Candidate)));
  if (itr != ReservedWordMap1.end()) {
    return itr->second;
  }
  else {
    return -1;
  }
}

/////////////////////////////////////
// SearchReservedName
// Try to translate a token number to the corresponding 
// string return NULL if not found 
// Used by the RTF PP
/////////////////////////////////////
const char * SCANNER::SearchReservedName (int tk_num)
{
  std::map<int, const char *>::const_iterator itr (ReservedWordMap2.find(tk_num));
  if (itr != ReservedWordMap2.end()) {
    return itr->second;
  }
  else {
    return NULL;
  }
}

LXINPUT SCANNER::LEXINPUT;

/** InitCounts ****************************************************************/
/** Resets the internal counters for position informations 		     **/

void SCANNER::InitCounts ()
{
  CurrentPos.init(); 
  NextPos.init(); 
}

void SCANNER::SetNewLine(int sec, int line)
{
  CurrentPos.SetSectionCount(sec);
  CurrentPos.SetSectionLineCount(line - 1); // due to newline
  NextPos.SetSectionCount(sec);
  NextPos.SetSectionLineCount(line - 1); // due to newline
  CurrentPos.RelNewLine();
  NextPos.RelNewLine();
}

/** InitLex *******************************************************************/
void SCANNER::InitLex( void (*f)(const wchar_t *, YYLTYPE&pos) )
{
  InitCounts();
  ReportP = f;
  yyrestart(NULL);
  BEGIN(0);
}
 
/** GetLineCount **************************************************************/
/** Returns the current line number of the input.                            **/
int SCANNER::GetLineCount ()
{
  // cerr << "GetLineCount returning: " << LineCount << "\n" << flush; 
  return (CurrentPos.GetAbsLineCount());
}

/** GetColumnCount ************************************************************/
/** Returns the current column number of the input.                          **/
int SCANNER::GetColumnCount ()
{
  //fprintf(stderr,"GetColumnCount returning: %d\n", ColumnCount); 
  return (CurrentPos.GetColumnCount());
}

// //////////////////////////////////////////////////
// IniPos 

void SCANNER::IniPos(YYLTYPE *pos)
{
//  pos->pos.reset(); 
  pos->pos.Init(Int(-1), Int(-1), Int(-1)); 
  pos->first_line = CurrentPos.GetAbsLineCount(); 
  pos->first_column = CurrentPos.GetColumnCount(); 
  pos->last_line = NextPos.GetAbsLineCount(); 
  pos->last_column = NextPos.GetColumnCount(); 
}

/** AdjustLineColumn **********************************************************/

void SCANNER::AdjustLineColumn(const char* s)
{
  int64_t i = 0;
  CurrentPos = NextPos; 
  while (s[i] != '\0') {
    switch (s[i]) {
      case ' ' : {
        NextPos.IncreaseColumn (SPACE_WIDTH);
        break;
      }
      case '\t': {
        NextPos.IncreaseColumn (TAB_WIDTH);
        break;
      }
      case '\n': {
        NextPos.Newline();
        break;
      }
      case '\r': {
	NextPos.IncreaseColumn (LETTER_WIDTH);
        break; 
      }
      default: {
        NextPos.IncreaseColumn (LETTER_WIDTH);
        break;
      }
    }
    i++;
  }
}

////////////////////////////
// Transfer token to the token list
////////////////////////////

#include "tokenlist.h"

TokenList * SCANNER::global_token_list = NULL;

////////////////////////////////////
// I/F To the Token list
// The token list holds all the tokens that was read by the 
// parser for given specification
///////////////////////////////////
void SCANNER::INSERT_TOKEN2(YYLTYPE *pos, int tok, const char * txt)
{
  yytextstring = (txt != NULL) ? txt : "";
  IniPos(pos); 
  if ( global_token_list != NULL ) {
    TYPE_CI_TokenPos start, end; 
    start.Init(Int(CurrentPos.GetSectionCount()),
               Int(CurrentPos.GetAbsLineCount()),
               Int(CurrentPos.GetRelAbsLineCount()),
               Int(CurrentPos.GetSectionLineCount()),
               Int(CurrentPos.GetColumnCount())); 
    end.Init(Int(NextPos.GetSectionCount()),
             Int(NextPos.GetAbsLineCount()),
             Int(NextPos.GetRelAbsLineCount()),
             Int(NextPos.GetSectionLineCount()),
             Int(NextPos.GetColumnCount())); 

    TokenInfo te;
    te.Init(tok, TBWSTR::string2wstring(yytextstring), start, end);
    Int p (global_token_list->Add(te));

    pos->pos.Init(p, p, p);
  }
}

// lex_unput returns the string p so it is the first chars to be
// scanned. As side-effect yytext is modified by unput (see `man flex').
void SCANNER::lex_unput(const char *p)
{
  string s(p); 
  for (string::size_type i = s.length(); i > 0; i--) {
    unput(s[i-1]); 
  }  
}

/** yywrap ********************************************************************/
/** The function is called when the last token has been read. Returns a code **/
/** to inform the parser of this fact.                                       **/

int yywrap ()
{
   return (DONE);
}

/** End of the lex-specification. ******************************/

