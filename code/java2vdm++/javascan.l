/*
$Id: javascan.l,v 1.11 2001/06/12 15:04:33 paulm Exp $
*/
/* Java 1.2 language lexer specification */

/* Use together with unicode.flex for Unicode preprocesssing */
/* and java12.cup for a Java 1.2 parser                      */

/* Note that this lexer specification is not tuned for speed.
   It is in fact quite slow on integer and floating point literals,
   because the input is read twice and the methods used to parse
   the numbers are not very fast.
   For a production quality application (e.g. a Java compiler)
   this could be optimized */
%{
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "metaiv.h"
#include "CPP.h"
#include "javascan.h"
#include "javaparse.tab.h"
#include "tbutils.h"

#ifdef __GNUC__
#endif
#ifdef _MSC_VER
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif
#include "tb_wstring.h"

#define YY_NEVER_INTERACTIVE 1

//extern void set_file_and_dir_name (char*);

//JR #define DONE
string JAVASCANNER::pTextString;
bool JAVASCANNER::booleanValue;
SEQ<Char> JAVASCANNER::stringValue;
Char JAVASCANNER::charValue;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int javasLXINPUT::String_getchars (char*p, int maxsize)
{
  if (s_index == (int) st.length()) 
    return 0;
  else {
    int rest = st.length() - s_index; 
    int m = rest < maxsize ? rest : maxsize; 
    memcpy(p, &st[s_index], m);
    s_index += m;
    return m;
  }
}

int javasLXINPUT::Stream_getchars (char*p, int maxsize)
{
  if (! is->fail()) {
    is->read(p, maxsize);
    return is->gcount();
  } else {
    return 0;
  }
}

void javasLXINPUT::ReadFrom(const string & s) 
{
  st = s; 
  s_index = 0; 
  current_getchars = &javasLXINPUT::String_getchars;
  current_reread = &javasLXINPUT::String_reread; 
}

void javasLXINPUT::ReadFrom(istream& s) 
{  
  is = &s;
  yy_switch_to_buffer (yy_create_buffer (0, YY_BUF_SIZE));
  current_getchars = &javasLXINPUT::Stream_getchars; 
  current_reread = &javasLXINPUT::Stream_reread; 
}

javasLXINPUT JAVASCANNER::javasLEXINPUT;

#define DONE                    1
#define TAB_WIDTH               1 
/* Safe assumption as a tab is only one character anyway */
#define SPACE_WIDTH             1
#define LETTER_WIDTH            1
/************** #define PREFIX_SEPARATOR        '-' **********************/
/* #define LEXDEBUG */

javasLexPos JAVASCANNER::javasCurrentPos;
javasLexPos JAVASCANNER::javasNextPos; 

///////////////////////////////////

//string javastextstring;
                                 /* This is assigned by javasINSERT_TOKEN2
                                 * access this instead of using
                                 * yytext.
                                 * 
                                 * Accessing extern char *yytext in
                                 * other files causes problems when
                                 * yytext has been modified by unput.
                                 * */

std::deque<string> JAVASCANNER::javasSaved_Strings;

void JAVASCANNER::javasSave_LexStringP (YYSTYPE* yylval, const char *p)
{
  javasSaved_Strings.push_front(string(p));
  yylval->LexStringP = &javasSaved_Strings[0];
}

int javaplex(YYSTYPE*yylval, YYLTYPE*yylpos) {return javaslex(yylval, yylpos);}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

%}

LineTerminator  [\r]|[\n]|[\r\n]
InputCharacter  [^\r\n]

WhiteSpace  [ \t\f]

CommentContent        ([^*]|\*+[^*/])*
TraditionalComment    "/*"[^*]{CommentContent}\*+"/"
EndOfLineComment      "//"{InputCharacter}*{LineTerminator}?
DocumentationComment  "/**"{CommentContent}\*+"/"
ControllComment      "@"{InputCharacter}*{LineTerminator}?

Identifier  [a-zA-Z][a-zA-Z0-9_]*

DecIntegerLiteral  0|[1-9][0-9]*
DecLongLiteral     {DecIntegerLiteral}[lL]
HexDigit           [0-9a-fA-F]
HexIntegerLiteral  0[xX]0*{HexDigit}{1,8}
HexLongLiteral     0[xX]0*{HexDigit}{1,16}[lL]
OctDigit           [0-7]
OctIntegerLiteral  0+[1-3]?{OctDigit}{1,15}
OctLongLiteral     0+1?{OctDigit}{1,21}[lL]
OctDigits          \\[0-3]?{OctDigit}?{OctDigit}
Exponent           [eE][+\-]?[0-9]+
FLit1              [0-9]+\.[0-9]*{Exponent}?
FLit2              \.[0-9]+{Exponent}?
FLit3              [0-9]+{Exponent}
FLit4              [0-9]+{Exponent}?
FloatLiteral       ({FLit1}|{FLit2}|{FLit3}|{FLit4})[fF]
DoubleLiteral      {FLit1}|{FLit2}|{FLit3}|{FLit4}
StringCharacter    [^\r\n\"\\]
SingleCharacter    [^\r\n\'\\]

%x strings chars

%%

"abstract"       { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ABSTRACT, yytext); 
                   return ABSTRACT;     
                 }
"boolean"        { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BOOLEAN, yytext); 
                   return BOOLEAN;      
                 }
"break"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BREAK, yytext); 
                   return BREAK;        
                 }
"byte"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BYTE, yytext); 
                   return BYTE;         
                 }
"case"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CASE, yytext); 
                   return CASE;         
                 }
"catch"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CATCH, yytext); 
                   return CATCH;        
                 }
"char"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CHAR, yytext); 
                   return CHAR;         
                 }
"class"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CLASS, yytext); 
                   return CLASS;        
                 }
"const"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CONST, yytext); 
                   return CONST;        
                 }
"continue"       { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CONTINUE, yytext); 
                   return CONTINUE;     
                 }
"do"             { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, DO, yytext); 
                   return DO;           
                 }
"double"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, DOUBLE, yytext); 
                   return DOUBLE;       
                 }
"else"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ELSE, yytext); 
                   return ELSE;         
                 }
"extends"        { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, EXTENDS, yytext); 
                   return EXTENDS;      
                 }
"final"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FINAL, yytext); 
                   return FINAL;        
                 }
"finally"        { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FINALLY, yytext); 
                   return FINALLY;      
                 }
"float"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FLOAT, yytext); 
                   return FLOAT;        
                 }
"for"            { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FOR, yytext); 
                   return FOR;          
                 }
"default"        { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, DEFAULT, yytext); 
                   return DEFAULT;      
                 }
"implements"     { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IMPLEMENTS, yytext); 
                   return IMPLEMENTS;   
                 }
"import"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IMPORT, yytext); 
                   return IMPORT;       
                 }
"instanceof"     { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, INSTANCEOF, yytext); 
                   return INSTANCEOF;   
                 }
"int"            { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, INT, yytext); 
                   return INT;          
                 }
"interface"      { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, INTERFACE, yytext); 
                   return INTERFACE;    
                 }
"long"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, LONG, yytext); 
                   return LONG;         
                 }
"native"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, NATIVE, yytext); 
                   return NATIVE;       
                 }
"new"            { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, NEW, yytext); 
                   return NEW;          
                 }
"goto"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, GOTO, yytext); 
                   return GOTO;         
                 }
"if"             { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IF, yytext); 
                   return IF;           
                 }
"public"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PUBLIC, yytext); 
                   return PUBLIC;       
                 }
"short"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, SHORT, yytext); 
                   return SHORT;        
                 }
"super"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, SUPER, yytext); 
                   return SUPER;        
                 }
"switch"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, SWITCH, yytext); 
                   return SWITCH;       
                 }
"synchronized"   { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, SYNCHRONIZED, yytext); 
                   return SYNCHRONIZED; 
                 }
"package"        { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PACKAGE, yytext); 
                   return PACKAGE;      
                 }
"private"        { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PRIVATE, yytext); 
                   return PRIVATE;      
                 }
"protected"      { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PROTECTED, yytext); 
                   return PROTECTED;    
                 }
"transient"      { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, TRANSIENT, yytext); 
                   return TRANSIENT;    
                 }
"return"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, RETURN, yytext); 
                   return RETURN;       
                 }
"void"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, VOID, yytext); 
                   return VOID;         
                 }
"static"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, STATIC, yytext); 
                   return STATIC;       
                 }
"while"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, WHILE, yytext); 
                   return WHILE;        
                 }
"this"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, THIS, yytext); 
                   return THIS;         
                 }
"throw"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, THROW, yytext); 
                   return THROW;        
                 }
"throws"         { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, THROWS, yytext); 
                   return THROWS;       
                 }
"try"            { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, TRY, yytext); 
                   return TRY;          
                 }
"volatile"       { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, VOLATILE, yytext); 
                   return VOLATILE;     
                 }
"strictfp"       { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, STRICTFP, yytext); 
                   return STRICTFP;     
                 }
"true"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BoolLit, yytext); 
                   JAVASCANNER::SetBooleanValue(true);
                   return BoolLit; 
                 }
"false"          { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BoolLit, yytext); 
                   JAVASCANNER::SetBooleanValue(false);
                   return BoolLit; 
                 }
"null"           { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                   JAVASCANNER::javasINSERT_TOKEN2 (yylpos, NullLit, yytext); 
                   return NullLit;      
                 }

"..."      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ELLIPSIS, yytext); 
           return ELLIPSIS;             
         }
"("      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, OP, yytext); 
           return OP;             
         }
")"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CP, yytext); 
           return CP;             
         }
"{"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, OCB, yytext); 
           return OCB;            
         }
"}"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CCB, yytext); 
           return CCB;            
         }
"["      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, OSB, yytext); 
           return OSB;            
         }
"]"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CSB, yytext); 
           return CSB;            
         }
";"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, SEMICOLON, yytext); 
           return SEMICOLON;      
         }
","      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COMMA, yytext); 
           return COMMA;          
         }
"."      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, DOT, yytext); 
           return DOT;            
         }
"="      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASEQUAL, yytext); 
           return ASEQUAL;        
         }
">"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, GT, yytext); 
           return GT;             
         }
"<"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, LT, yytext); 
           return LT;             
         }
"!"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, NEG, yytext); 
           return NEG;            
         }
"~"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COMPL, yytext); 
           return COMPL;          
         }
"?"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, REL_QM, yytext); 
           return REL_QM;         
         }
":"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COLON, yytext); 
           return COLON;          
         }
"=="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, EQ, yytext); 
           return EQ;             
         }
"<="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, LEQ, yytext); 
           return LEQ;            
         }
">="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, GEQ, yytext); 
           return GEQ;            
         }
"!="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, NEQ, yytext); 
           return NEQ;            
         }
"&&"     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, AND, yytext); 
           return AND;            
         }
"||"     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, OR, yytext); 
           return OR;             
         }
"++"     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PPLUS, yytext); 
           return PPLUS;          
         }
"--"     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PMINUS, yytext); 
           return PMINUS;         
         }
"+"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, PLUS, yytext); 
           return PLUS;           
         }
"-"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, MINUS, yytext); 
           return MINUS;          
         }
"*"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, MULT, yytext); 
           return MULT;           
         }
"/"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, DIV, yytext); 
           return DIV;            
         }
"&"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BITAND, yytext); 
           return BITAND;         
         }
"|"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, BITOR, yytext); 
           return BITOR;          
         }
"^"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, EXCLOR, yytext); 
           return EXCLOR;         
         }
"%"      { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, MOD, yytext); 
           return MOD;            
         }
"<<"     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, LEFTSHIFT, yytext); 
           return LEFTSHIFT;      
         }
">>"     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, RIGHTSHIFT, yytext); 
           return RIGHTSHIFT;     
         }
">>>"    { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ZRS, yytext); 
           return ZRS;            
         }
"+="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASPLUS, yytext); 
           return ASPLUS;         
         }
"-="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASMINUS, yytext); 
           return ASMINUS;        
         }
"*="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASMULT, yytext); 
           return ASMULT;         
         }
"/="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASDIV, yytext); 
           return ASDIV;          
         }
"&="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASBITWISEAND, yytext); 
           return ASBITWISEAND;   
         }
"|="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASBITWISEINCLOR, yytext); 
           return ASBITWISEINCLOR;
         }
"^="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASBITWISEEXOR, yytext); 
           return ASBITWISEEXOR;  
         }
"%="     { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASMOD, yytext); 
           return ASMOD;          
         }
"<<="    { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASLEFTSHIFT, yytext); 
           return ASLEFTSHIFT;    
         }
">>="    { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ASRIGHTSHIFT, yytext); 
           return ASRIGHTSHIFT;   
         }
">>>="   { 
           JAVASCANNER::javasAdjustLineColumn (yytext); 
           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, ZRS_ASSIGN, yytext); 
           return ZRS_ASSIGN;     
         }
{DecIntegerLiteral} { JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IntegerLit, yytext);
                      return IntegerLit; 
                    }
{DecLongLiteral}    { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IntegerLit, yytext);
                      return IntegerLit; 
                    }
{HexIntegerLiteral} { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IntegerLit, yytext);
                      return IntegerLit; 
                    }
{HexLongLiteral}    { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IntegerLit, yytext);
                      return IntegerLit; 
                    }
{OctIntegerLiteral} { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IntegerLit, yytext);
                      return IntegerLit; 
                    }
{OctLongLiteral}    { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IntegerLit, yytext);
                      return IntegerLit; 
                    }
{FloatLiteral}      { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FloatingLit, yytext);
                      return FloatingLit; 
                    }
{DoubleLiteral}     { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FloatingLit, yytext);
                      return FloatingLit; 
                    }
{DoubleLiteral}[dD] { 
                      JAVASCANNER::SetPTextString(yytext); 
                      JAVASCANNER::javasAdjustLineColumn (yytext); 
                      JAVASCANNER::javasINSERT_TOKEN2 (yylpos, FloatingLit, yytext);
                      return FloatingLit; 
                    }
{TraditionalComment}   { 
                         JAVASCANNER::javasAdjustLineColumn (yytext); 
                         JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COMMENT, yytext); 
                       }
{EndOfLineComment}     { 
                         JAVASCANNER::javasAdjustLineColumn (yytext); 
                         JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COMMENT, yytext); 
                       }
{DocumentationComment} { 
                         JAVASCANNER::javasAdjustLineColumn (yytext); 
                         JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COMMENT, yytext); 
                       }
{ControllComment}     { 
                         JAVASCANNER::javasAdjustLineColumn (yytext); 
                         JAVASCANNER::javasINSERT_TOKEN2 (yylpos, COMMENT, yytext); 
                       }
{WhiteSpace}     { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                 }
{LineTerminator} { 
                   JAVASCANNER::javasAdjustLineColumn (yytext); 
                 }
{Identifier} { 
               JAVASCANNER::SetPTextString(yytext); 
               JAVASCANNER::javasAdjustLineColumn (yytext); 
               JAVASCANNER::javasSave_LexStringP (yylval, yytext);
               JAVASCANNER::javasINSERT_TOKEN2 (yylpos, IDENTIFIER, yytext);
               return IDENTIFIER; 
             }
\"                          { 
                              JAVASCANNER::ClearStringValue(); 
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                              BEGIN(strings); 
                            }
<strings>\"                 { 
                              BEGIN(INITIAL); 
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                              string s = TBWSTR::wstring2string(JAVASCANNER::GetStringValue().ascii ());
                              char* sp = &(s[1]);
                              int i;
                              for (i = 1; (s[i] != '\0'); i++);
                              s[i-1] = '\0';
                              JAVASCANNER::javasINSERT_TOKEN2 (yylpos, StringLit, sp);
                              return StringLit; 
                            }
<strings>{StringCharacter}+ {  
                              register int i;
                              for (i = 0; (yytext[i] != '\0'); i++)
                                JAVASCANNER::AppendChar(Char (yytext[i]));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>"\\\\"             { 
                              JAVASCANNER::AppendChar(Char (L'\\'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>"\\r"              { 
                              JAVASCANNER::AppendChar(Char (L'\r'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>"\\n"              { 
                              JAVASCANNER::AppendChar(Char (L'\n'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>\\t                { 
                              JAVASCANNER::AppendChar(Char (L'\t'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>"\\f"              { 
                              JAVASCANNER::AppendChar(Char (L'\f'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>\\\"               { 
                              JAVASCANNER::AppendChar(Char (L'\"'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>"\\'"              { 
                              JAVASCANNER::AppendChar(Char (L'\''));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>"\\b"              { 
                              JAVASCANNER::AppendChar(Char (L'\b'));
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>{OctDigits}        { 
                              // JR no action yet 
                            }
<strings>{LineTerminator}   { 
                              //vdm_err << "Unterminated strings at end of line" << endl << flush;
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
<strings>\\.                { 
                              //vdm_err << "Illegal escape sequence \"" << yytext << "\"" << endl << flush; 
                              JAVASCANNER::javasAdjustLineColumn (yytext); 
                            }
"'"                      { 
                           BEGIN(chars); 
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                         }
<chars>"'"               { 
                           BEGIN(INITIAL); 
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           return CharacterLit; 
                         }
<chars>{SingleCharacter} { 
                           JAVASCANNER::SetCharValue(Char (yytext[0])); 
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\\\              { 
                           JAVASCANNER::SetCharValue(Char (L'\\'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\r               { 
                           JAVASCANNER::SetCharValue(Char (L'\r'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\n               { 
                           JAVASCANNER::SetCharValue(Char (L'\n'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\t               { 
                           JAVASCANNER::SetCharValue(Char (L'\t'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\f               { 
                           JAVASCANNER::SetCharValue(Char (L'\f'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\\"              { 
                           JAVASCANNER::SetCharValue(Char (L'\"'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\'               { 
                           JAVASCANNER::SetCharValue(Char (L'\''));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>{OctDigits}       { 
// JR !
                           JAVASCANNER::SetCharValue(Char (L' '));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\b               { 
                           JAVASCANNER::SetCharValue(Char (L'\b'));
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                           JAVASCANNER::javasINSERT_TOKEN2 (yylpos, CharacterLit, yytext);
                         }
<chars>\\.               { 
                           //vdm_err << "Illegal character escape sequence \"" << yytext << "\"" << endl << flush; 
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                         }
<chars>{LineTerminator}  { 
                           //vdm_err << "Unterminated character literal at end of line" << endl << flush; 
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                         }
<chars>.                 { 
                           //vdm_err << "Unrecognized character: " << yytext << endl << flush; 
                           JAVASCANNER::javasAdjustLineColumn (yytext); 
                         }
%%


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/** InitCounts ****************************************************************/
/** Resets the internal counters for position informations 		     **/

void JAVASCANNER::javasInitCounts ()
{  
  javasCurrentPos.init(); 
  javasNextPos.init(); 
}

/** InitLex *******************************************************************/

//JR void javasInitLex ( void (*f) (const char *, YYLTYPE&pos) )
void JAVASCANNER::javasInitLex ()
{
//  ReportP = f;
  yyrestart (NULL);
  BEGIN (0);
}
 
/** GetLineCount **************************************************************/
/** Returns the current line number of the input.                            **/

int JAVASCANNER::javasGetLineCount ()
{
  // cerr << "GetLineCount returning: " << LineCount << "\n" << flush; 
  return (javasCurrentPos.GetAbsLineCount());
}

/** GetColumnCount ************************************************************/
/** Returns the current column number of the input.                          **/

int JAVASCANNER::javasGetColumnCount ()
{
  //fprintf(stderr,"GetColumnCount returning: %d\n", ColumnCount); 
  return (javasCurrentPos.GetColumnCount());
}

void JAVASCANNER::javasIniPos (YYLTYPE *pos)
{
//  pos->pos.reset(); 
  pos->pos.Init(Int(-1), Int(-1), Int(-1)); 
  pos->first_line   = javasCurrentPos.GetAbsLineCount(); 
  pos->first_column = javasCurrentPos.GetColumnCount(); 
  pos->last_line    = javasNextPos.GetAbsLineCount(); 
  pos->last_column  = javasNextPos.GetColumnCount(); 
}

void JAVASCANNER::javasAdjustLineColumn (register char* s)
{  
  register int i = 0;
  javasCurrentPos = javasNextPos; 
  while (s[i] != '\0')
  {  
    switch (s[i])
    {  
      case ' ' :
        javasNextPos.IncreaseColumn (SPACE_WIDTH);
        break;
      case '\t':
        javasNextPos.IncreaseColumn (TAB_WIDTH);
        break;
      case '\n':
        javasNextPos.Newline ();
        break;
      case '\r':
        javasNextPos.IncreaseColumn (LETTER_WIDTH);
        break; 
      default  :
	javasNextPos.IncreaseColumn (LETTER_WIDTH);
        break;
    }
    i++;
  }
}

#include "tokenlist.h"

TokenList * JAVASCANNER::javas_global_token_list = NULL;

void JAVASCANNER::javasSetTokenList (TokenList * tkl) {
  JAVASCANNER::javas_global_token_list = tkl;
}

////////////////////////////////////
// I/F To the Token list
// The token list holds all the tokens that was read by the 
// parser for given specification
///////////////////////////////////
void JAVASCANNER::javasINSERT_TOKEN2 (YYLTYPE *pos, int tok, char * txt)
{
  string javastextstring ((txt != NULL) ? txt : "");
  javasIniPos (pos); 
  if (javas_global_token_list != NULL)
  {
    TYPE_CI_TokenPos start, end; 
    start.Init (Int(javasCurrentPos.GetSectionCount()), 
                Int(javasCurrentPos.GetAbsLineCount()),
                Int(javasCurrentPos.GetRelAbsLineCount()),
                Int(javasCurrentPos.GetSectionLineCount()),
                Int(javasCurrentPos.GetColumnCount())); 
    end.Init (Int(javasNextPos.GetSectionCount()), 
              Int(javasNextPos.GetAbsLineCount()),
              Int(javasNextPos.GetRelAbsLineCount()),
              Int(javasNextPos.GetSectionLineCount()),
              Int(javasNextPos.GetColumnCount())); 

    TokenInfo te;
    te.Init (tok, TBWSTR::string2wstring(javastextstring), start, end);
    Int p (javas_global_token_list -> Add (te));

    pos->pos.Init(p, p, p); 
  }
}

int javaswrap()
{
  return 1;
}
