%{


#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include "metaiv.h"
#include "AS.h"
#include "MANGLE.h"
#include "asquotes.h"
#include "astaux.h"
#include "contextinfo.h"
#include "tb_wstring.h"

#if !defined(__APPLE_CC__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif // !defined(__APPLE_CC__) && !defined(__FreeBSD__)

int unmangle_lex();
int unmangle_error(const char *);

string UNMANGLE::inputStr;
int UNMANGLE::inputLen;
int UNMANGLE::currentPos;
string UNMANGLE::textLit;
SEQ<TYPE_AS_Type> UNMANGLE::unmangledType;
const int UNMANGLE::sepalator = '-';
bool UNMANGLE::maybetypename = false;

%}

%union {
  SEQ<TYPE_AS_Type>* seq;
  TYPE_AS_Type* rec;
}

%type <seq> MangledTypeSeq;
%type <rec> MangledType;

%token LEX_text_lit LEX_lparen LEX_rparen LEX_sep

%%

TopLevel : MangledTypeSeq
           {
             UNMANGLE::unmangledType= *$1;
             delete $1;
           }

MangledTypeSeq : MangledType
                 { 
                   $$ = new SEQ<TYPE_AS_Type>();
                   $$->ImpAppend(*$1);
                   delete $1;
                 }
               | MangledTypeSeq LEX_sep MangledType
                 {
                   $$ = $1;
                   $$->ImpAppend(*$3);
                   delete $3;
                 }
               ;

MangledType: 'B'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)AS_BOOLEAN);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_BooleanType();
               $$->SetField (pos_AS_BooleanType_cid, NilContextId);
             }
           | 'M'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)NATONE);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_NumericType();
               $$->SetField (pos_AS_NumericType_qtp, (Int)NATONE);
               $$->SetField (pos_AS_NumericType_cid, NilContextId);
             }
           | 'N'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)NAT);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_NumericType();
               $$->SetField (pos_AS_NumericType_qtp, (Int)NAT);
               $$->SetField (pos_AS_NumericType_cid, NilContextId);
             }
           | 'I'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)INTEGER);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_NumericType();
               $$->SetField (pos_AS_NumericType_qtp, (Int)INTEGER);
               $$->SetField (pos_AS_NumericType_cid, NilContextId);
             }
           | 'Q'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)RAT);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_NumericType();
               $$->SetField (pos_AS_NumericType_qtp, (Int)RAT);
               $$->SetField (pos_AS_NumericType_cid, NilContextId);
             }
           | 'R'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)REAL);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_NumericType();
               $$->SetField (pos_AS_NumericType_qtp, (Int)REAL);
               $$->SetField (pos_AS_NumericType_cid, NilContextId);
             }
           | 'T'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)AS_TOKEN);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_TokenType();
               $$->SetField (pos_AS_TokenType_cid, NilContextId);
             }
           | 'C'
             { 
               //$$ = new TYPE_AS_BasicType();
               //$$->SetField (pos_AS_BasicType_qtp, (Int)AS_CHAR);
               //$$->SetField (pos_AS_BasicType_cid, NilContextId);
               $$ = new TYPE_AS_CharType();
               $$->SetField (pos_AS_CharType_cid, NilContextId);
             }
           | 'q' LEX_sep LEX_text_lit
             {
               $$ = new TYPE_AS_QuoteType();
               TYPE_AS_QuoteLit rp;
               rp.SetField(pos_AS_QuoteLit_val, Sequence(UNMANGLE::bslashUs2wstring(UNMANGLE::textLit)));
               rp.SetField(pos_AS_QuoteLit_cid, NilContextId);
               $$->SetField (pos_AS_QuoteType_lit, rp);
               $$->SetField (pos_AS_QuoteType_cid, NilContextId);
             }
           | 'u' LEX_lparen MangledTypeSeq LEX_rparen
             {
               $$ = new TYPE_AS_UnionType();
               $$->SetField (pos_AS_UnionType_tps, *$3);
               $$->SetField (pos_AS_UnionType_cid, NilContextId);
               delete $3;
             }
           | 'p' LEX_lparen MangledTypeSeq LEX_rparen
             {
               $$ = new TYPE_AS_ProductType();
               $$->SetField (pos_AS_ProductType_tps, *$3);
               $$->SetField (pos_AS_ProductType_cid, NilContextId);
               delete $3;
             }
           | 'o' LEX_sep MangledType
             {
               $$ = new TYPE_AS_OptionalType();
               $$->SetField (pos_AS_OptionalType_tp, *$3);
               $$->SetField (pos_AS_OptionalType_cid, NilContextId);
               delete $3;
             }
           | 't' LEX_sep MangledType
             {
               $$ = new TYPE_AS_SetType();
               $$->SetField (pos_AS_SetType_elemtp, *$3);
               $$->SetField (pos_AS_SetType_cid, NilContextId);
               delete $3;
             }
           | 's' LEX_sep MangledType
             {
               $$ = new TYPE_AS_Seq0Type();
               $$->SetField (pos_AS_Seq0Type_elemtp, *$3);
               $$->SetField (pos_AS_Seq0Type_cid, NilContextId);
               delete $3;
             }
           | 'm' LEX_sep MangledType LEX_sep MangledType
             {
               $$ = new TYPE_AS_GeneralMapType();
               $$->SetField (pos_AS_GeneralMapType_mapdom, *$3);
               $$->SetField (pos_AS_GeneralMapType_maprng, *$5);
               $$->SetField (pos_AS_GeneralMapType_cid, NilContextId);
               delete $3;
               delete $5;
             }
           | 'n' LEX_sep LEX_text_lit
             {
               $$ = new TYPE_AS_TypeName();
               $$->SetField (pos_AS_TypeName_name, ASTAUX::MkNameFromStr(TBWSTR::string2wstring(UNMANGLE::textLit), NilContextId));
               $$->SetField (pos_AS_TypeName_cid, NilContextId);
             }
           | 'v' LEX_sep LEX_text_lit
             {
               $$ = new TYPE_AS_TypeVar();
               $$->SetField (pos_AS_TypeVar_name, ASTAUX::MkNameFromStr(TBWSTR::string2wstring(UNMANGLE::textLit), NilContextId));
               $$->SetField (pos_AS_TypeVar_cid, NilContextId);
             }
           | 'f' LEX_sep 'p' LEX_lparen MangledTypeSeq LEX_rparen LEX_sep MangledType
             {
               $$ = new TYPE_AS_PartialFnType();
               $$->SetField (pos_AS_PartialFnType_fndom, *$5);
               $$->SetField (pos_AS_PartialFnType_fnrng, *$8);
               $$->SetField (pos_AS_PartialFnType_cid, NilContextId);
                delete $5;
                delete $8;
             }
           | 'b' LEX_sep MangledType
             {
               $$ = new TYPE_AS_BracketedType();
               $$->SetField (pos_AS_BracketedType_tp, *$3);
               $$->SetField (pos_AS_BracketedType_cid, NilContextId);
                delete $3;
             }
           | 'a'
             {
               $$ = new TYPE_AS_AllType();
               $$->SetField (pos_AS_AllType_cid, NilContextId);
             }
           ;

%%

int unmangle_error(const char *)
{
  return 1;
}

int unmangle_lex()
{
  return UNMANGLE::unmangleLex();
}

int UNMANGLE::unmangleLex()
{
  if (currentPos == inputLen)
    return -1;

  switch (inputStr[currentPos]) {
    case sepalator: { // '-'
      currentPos++;
      return LEX_sep;
    }
    case '(': {
      currentPos++;
      return LEX_lparen;
    }
    case ')': {
      currentPos++;
      return LEX_rparen;
    }
    default: {
      int i;
      for (i = currentPos; i < inputLen && !is_sep(inputStr[i]); i++)
        ;

      if (i - currentPos > 1) {
        textLit = inputStr.substr(currentPos, i - currentPos);
        //      cout << "textLit is " << textLit << endl;
        currentPos = i;
        return LEX_text_lit;
      }
      else if ((i == inputLen) && (i - currentPos > 0) && maybetypename) {
        textLit = inputStr.substr(currentPos, i - currentPos);
        //      cout << "textLit is " << textLit << endl;
        currentPos = i;
        maybetypename = false;
        return LEX_text_lit;
      }
      else {
// 20090303 -->
        if (!maybetypename)
        {
          if ((currentPos > 1) &&
              (inputStr[currentPos - 1] == sepalator) &&
              (inputStr[currentPos - 2] == 'n'))
          {
            if ((currentPos == 2) ||
                ((currentPos > 2 ) && (inputStr[currentPos - 3] == sepalator)))
            {
              textLit = inputStr.substr(currentPos, i - currentPos);
              currentPos = i;
              return LEX_text_lit;
            }
          }
        }
// <-- 20090303
        char c = inputStr[currentPos];
        currentPos = i;
        maybetypename = (c == 'v');
        return c;
      }
    }
  }
}

bool UNMANGLE::is_sep(wchar_t c)
{
  return (c == '(' || c == ')' || c == sepalator);
}

SEQ<TYPE_AS_Type> UNMANGLE::unmangle_main(const wstring & nm)
{
  SEQ<TYPE_AS_Type> res;
  if (!nm.empty())
  {
    UNMANGLE::textLit = "";
    UNMANGLE::unmangledType.Clear();
    UNMANGLE::inputStr = TBWSTR::ConvertToHexquad(nm);
    UNMANGLE::inputLen = inputStr.length();
    UNMANGLE::currentPos = 0;
    UNMANGLE::maybetypename = false;

    int result = unmangle_parse();
    if (!result)
      res.ImpConc(UNMANGLE::unmangledType);

    // clear variables
    UNMANGLE::textLit = "";
    UNMANGLE::unmangledType.Clear();
  }
  return res;
}


#define BSLASH '\\'
#define WBSLASH L'\\'
#define UPPERCASE_U 'U'
#define LOWERCASE_U 'u'

wstring UNMANGLE::bslashUs2wstring(const string& str)
{
  wstring res;
  int len = str.length();

  int prefix = 0;
  int maxNumOfhexDigits = 0;
  int hexDigitIdx = 0;

  wchar_t code = 0;

  for(int i = 0; i < len; i++)
  {
    char c = str[i];
    switch(prefix) {
      case 0: {
        if(c == BSLASH)
          prefix++;
        else
          res += btowc(c);
        break;
      } 
      case 1: {
        if((c == LOWERCASE_U) || (c == UPPERCASE_U)) {
          maxNumOfhexDigits = (c == LOWERCASE_U ? 4:8);
          prefix++;
          code = 0;
        } else {
          res += WBSLASH;
          prefix = 0;
        }
        break;
      }
      case 2: {
        if(hexDigitIdx < maxNumOfhexDigits) {
          code = ((code << 4) | hexDigit2int(c));
        }
        if(hexDigitIdx == maxNumOfhexDigits - 1) {
          res += code;
          prefix = 0;
        } else {
          hexDigitIdx++;
        }
        break;
      }
      default: {
        res += btowc(c);
        break;
      }
    }
  }

  return res;
}

int UNMANGLE::hexDigit2int(char c)
{
  if(c<='9') return c-'0';
  return 10+(c-'A');
}

/*
string wstring2Us2(const wstring& line)
{
  string str;
  const wchar_t* pwc = line.c_str();

  char converted[7];
  while (*pwc!=L'\0') {
    char* pc = converted;
    if(*pwc < 0x007F) {
      *pc++ = (char)*pwc;
    } else {
      *pc++ = '\\';
      *pc++ = 'u';
      TBWSTR::hexquad(*pwc, &pc);
    }
    *pc++ = '\0';
    str += string(converted);
    pwc++;
  }

  return str;
}
*/

#if 0
main(int argc, char** argv)
{
  init_AS();
  argc--;
  ++argv;
  //  unmangle_debug = 1;
  if (argc == 0)
    return -1;
  else {
    inputLen = strlen(argv[0]);
    inputStr = wstring (argv[0]);
    int result = unmangle_parse();
    if (!result)
      cout << L"Success:" << unmangledType.ascii() << endl;
    else
      cout << L"Failure" << endl;
    return result;
  }
}

#endif



