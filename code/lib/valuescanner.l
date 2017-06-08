/*  * ID
 *  *    $Id: valuescanner.l,v 1.6 2001/06/01 08:28:46 mikhail Exp $
*/

/* This is the scanner matching the grammar defined in valueparser.y
   It is largely a subset of the lexical rules used for the main VDM parser,
   with the exception that numeric literals may have a leading "-" here;
   in the main parser -2 would be parsed as an application of the unary 
   operator "-". 
*/

%{
#include <stdio.h>
#include <string.h>
#include "metaiv.h"

#include "valuescanner.h"
#include "valueparser.tab.h"

#ifdef _MSC_VER
//define isatty
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif // _MSC_VER

#define YY_NEVER_INTERACTIVE 1

#define DONE

VLLXINPUT VLLEXINPUT;
%}

hexdigit                [0-9ABCDEF]|[0-9abcdef]
hexquad                 {hexdigit}{hexdigit}{hexdigit}{hexdigit}

universalcharactername  (\\u{hexquad})|(\\U{hexquad})

letter                  ([A-Za-z]|#[A-Za-z]|{universalcharactername})

digit                   [0-9]
identifierorkeyword    {letter}([0-9\'_]|{letter})*
numericliteral         "-"?{digit}+
realliteral            "-"?[0-9]+(("."[0-9]+)|([Ee]("+"|"-")?[0-9]+)|("."[0-9]+[Ee]("+"|"-")?[0-9]+))

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

characterliteral       "'"([\040-\133\135-\176]|{universalcharactername}|{embeddedctrlchar}|{backslashed}|{deletechar}|{highbitchar})"'"

doublequote            \"\"
textliteral            \"([\040-\041\043-\133\135-\176]|{universalcharactername}|{embeddedctrlchar}|{backslashed}|{deletechar}|{highbitchar})*\"

quoteliteral           \<{identifierorkeyword}\>

%%

{characterliteral}	{  register int i;
                           for (i = 0; (valueptext[i] = valueptext[i+1]) != '\0'; i++);
                           valueptext[i-1] = '\0';
                           VALUEPARSER::valueptextstring = valueptext;
                           return (LEX_char_lit);
                        }
{quoteliteral}		{  register int i;
                           for (i = 0; (valueptext[i] = valueptext[i+1]) != '\0'; i++);
                           valueptext[i-1] = '\0';
                           VALUEPARSER::valueptextstring = valueptext;
                           return(LEX_quote_lit);
                        }
{textliteral}           {  register int i;
                           for (i = 0; (valueptext[i] = valueptext[i+1]) != '\0'; i++);
                           valueptext[i-1] = '\0';
                           VALUEPARSER::valueptextstring = valueptext;
                           return(LEX_text_lit);
                        }
{realliteral}           {  VALUEPARSER::valueptextstring = valueptext;
                           return (LEX_real_lit);
                        }
{numericliteral}        {  VALUEPARSER::valueptextstring = valueptext;
                           return (LEX_real_lit);
                        }
"token"                 { return (LEX_TOKEN);
                        }
"nil"                   { return (LEX_NIL);
                        }
"true"                  { return (LEX_bool_true);
                        }
"false"                 { return (LEX_bool_false);
                        }
"mk_"                   {  return (LEX_MK_);
                        }

"mk_"/{identifierorkeyword}  {
                           VALUEPARSER::valueptextstring = valueptext;
			   return (LEX_MK_);
                        }
{identifierorkeyword}	{  VALUEPARSER::valueptextstring = valueptext;
                           return (LEX_identifier);
                        }

%{
%}
"|->"                     {  return (LEX_BAR_ARROW);
                        }
","                     {  return (',');
                        }
")"                     {  return (')');
                        }
"("                     {  return ('(');
                        }
"["                     {  return ('[');
                        }
"]"                     {  return (']');
                        }
"{"                     {  return ('{');
                        }
"}"                     {  return ('}');
                        }
"`"                     {  return LEX_PRIME;
                        }
">"                     {  return ('>');
                        }
"<"                     {  return ('<');
                        }
" "                     {  VALUEPARSER::valueptextstring = "";
                        }
"\n"                    {  VALUEPARSER::valueptextstring = "";
                        }
"\t"                    {  VALUEPARSER::valueptextstring = "";
                        }
"\r"                    {  VALUEPARSER::valueptextstring = "";
                        }
<<EOF>>                 {  VALUEPARSER::valueptextstring = "";
			   return 0; // EOF
                        }
.                       {  cout << "Unrecognized character: " << valueptext << endl;}
			
%%
int valuepwrap()
{
  return 1;
}
