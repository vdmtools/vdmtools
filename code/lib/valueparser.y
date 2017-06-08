// -*-Mode: text; -*- 
/*  * ID
 *  *    $Id: valueparser.y,v 1.6 2005/01/19 05:01:28 vdmtools Exp $
*/

// This file represents the grammar for a parser of metaiv values. 
// It is intended for reading values written to files using the metaiv
// ascii method. Values that do not match this format will not parse 
// correctly, if at all.

// Most of the parser is fairly standard. The only point of interest is
// the way in which records are treated. A map is used to map textual
// record names to numeric tags used in the generated code. This map is
// established in the constructors/init functions in the generated code,
// via the AddRecordTag function defined below. 

// Note that object references are not supported since they are not 
// supported within the Toolbox interpreter. Thus, the IO library should 
// not be considered as a means of object serialization and deserialization. 

%{


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "metaiv.h"	/* Basic Meta-IV/VDM data types and operations.      */
#include "tb_wstring.h"
#include "valuescanner.h"
#include "cg.h"

#if !defined(__darwin__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif // !defined(__darwin__) && !defined(__FreeBSD__)

string VALUEPARSER::valueptextstring;
Generic * VALUEPARSER::exprs = NULL;
Map * VALUEPARSER::RecordNameToTag = NULL;

extern FILE *valuepin;
static void valueperror(const char*);

#define YYDEBUG 1
extern int valuepdebug;
extern int valueplex();

%}

%union {
  Sequence *seq;
  Tuple *tup;
  Map *map;
  Set *set;
  Real *real;
  Char *cha;
  Quote *quote;
  Nil *nil;
  Record *rec;
  Token *tok;
  Bool *boo;
  Generic *val;
  string* str;
}


%type <val> Value File
%type <seq> ValueList NonEmptyValueList SequenceVal StringVal 
%type <real> NumericVal 
%type <tup> TupleVal 
%type <tok> TokenVal
%type <set> SetVal 
%type <map> MapVal ListOfMaplets Maplet
%type <rec> RecordVal 
%type <quote> QuoteVal 
%type <nil> NilVal 
%type <boo> BoolVal 
%type <cha> CharVal
%type <str> Identifier 
%token <str> LEX_identifier LEX_real_lit LEX_char_lit LEX_quote_lit LEX_text_lit 
%token LEX_MK_ LEX_BAR_ARROW LEX_PRIME LEX_TOKEN LEX_bool_true LEX_bool_false LEX_NIL LEX_MINUS



%%

File : Value
      { 
        *VALUEPARSER::exprs = *$1;
        delete $1;
      };

Value : SetVal { $$ = new Generic(*$1); delete $1;} 
      | SequenceVal { $$ = new Generic(*$1); delete $1;}
      | MapVal { $$ = new Generic(*$1); delete $1;}
      | NumericVal { $$ = new Generic(*$1); delete $1;}
      | TokenVal { $$ = new Generic(*$1); delete $1;}
      | TupleVal { $$ = new Generic(*$1); delete $1;}
      | StringVal { $$ = new Generic(*$1); delete $1;}
      | RecordVal { $$ = new Generic(*$1); delete $1;}
      | QuoteVal { $$ = new Generic(*$1); delete $1;}
      | NilVal { $$ = new Generic(*$1); delete $1;}
      | BoolVal { $$ = new Generic(*$1); delete $1;}
      | CharVal { $$ = new Generic(*$1); delete $1;}
      ;

ValueList
        : /* empty */
        {  $$ = new Sequence ();
        }
        | NonEmptyValueList
        ;

NonEmptyValueList
        : Value
        {  $$ = new Sequence ();
           $$->ImpAppend (*$1);
           delete $1;
        }
        | NonEmptyValueList ',' Value
        {  $1->ImpAppend (*$3);
           delete $3;
        }
        ;

TupleVal
        : LEX_MK_ '(' ValueList ')'
        {
          size_t len = $3->Length();
          $$ = new Tuple(len);
          for (size_t i = 1; i <= len; i++)
            $$->SetField(i, $3->Index(i));
          delete $3;
        }
        ;

MapVal
        : '{' LEX_BAR_ARROW '}'
        {  
           $$ = new Map();
        }
        | '{' ListOfMaplets '}'
        {  
           $$ = $2;
        }
        ;

ListOfMaplets
        : Maplet 
        {  $$ = $1;
        }
        | ListOfMaplets ',' Maplet
        {  $1->ImpOverride (*$3);
           delete $3;
        }
        ;

Maplet
        : Value LEX_BAR_ARROW Value
        {  
           $$ = new Map();
           $$->Insert(*$1, *$3);
           delete $1;
           delete $3;
        }
        ;

SetVal
        : '{'  ValueList '}'
        {  
          $$ = new Set();
          Generic g;
          for (bool bb = $2->First(g); bb; bb = $2->Next(g))
             $$->Insert(g);
          delete $2;
        }
        ;

SequenceVal
        : '['  ValueList ']'
        {  
          $$ = $2;
        }
        ;

RecordVal
        : LEX_MK_ Identifier LEX_PRIME Identifier '(' ValueList ')'
        {  
          string strName (*$2 + "`" + *$4);
          SEQ<Char> recordName(TBWSTR::string2wstring(strName));
          Generic g;
          Int tag(1);
          if (VALUEPARSER::RecordNameToTag != NULL) {
            if (VALUEPARSER::RecordNameToTag->DomExists(recordName,g))
               tag = Int(g);
          }
          $$ = new Record(tag.GetValue(), $6->Length());
          $$->SetFields(*$6);
          delete $2;
          delete $4;
          delete $6;
        }
        ;

Identifier
        : LEX_identifier
        {  
          $$ = new string(VALUEPARSER::valueptextstring);
        }
TokenVal: LEX_MK_ LEX_TOKEN '(' Value ')'
        {  
          $$ = new Token();
          *$$ = mk_token(*$4);
          delete $4;
        }
//        | LEX_MK_ LEX_TOKEN '(' StringVal ')'
//        {  
//          wstring s;
//          $4->GetString(s);
//          $$ = new Token(s);
//          delete $4;
//        }
        ;

NumericVal
        : LEX_real_lit
        { double Result;
          string s = VALUEPARSER::valueptextstring;
          int ok = VALUEPARSER::ConvRealLit ( TBWSTR::string2wstring(s).c_str(), Result);
          $$ = new Real(Result);
        }
        ;

CharVal : LEX_char_lit
        { string res = VALUEPARSER::valueptextstring;
          $$ = new Char (res[0]);
        }
        ;

BoolVal : LEX_bool_true
        {  
          $$ = new Bool(true);
        }
        | LEX_bool_false
        {  
          $$ = new Bool(false);
        }
        ;

QuoteVal: LEX_quote_lit
        {  
          string s = VALUEPARSER::valueptextstring;
          $$ = new Quote(TBWSTR::string2wstring(s));
        }
        ;

StringVal
        : LEX_text_lit
        {  
          string res = VALUEPARSER::valueptextstring; 
          $$ = new Sequence(TBWSTR::string2wstring(res));
        }
        ;

NilVal 
        : LEX_NIL
        {  
          $$ = new Nil();
        }
        ;

%%

static void valueperror(const char *ErrorMessage)
{ 
}

int valuepparse();

int VALUEPARSER::ConvRealLit(const wchar_t *text, double &res)
{  wchar_t* ptr;
   res = wcstod(text, &ptr);
   return (text != ptr && *ptr=='\0');
}

void CGUTIL::AddRecordTag(const wstring& nm, const int& tag)
{
//wcerr << L"AddRecordTag: " << nm << tag << endl;
  if (VALUEPARSER::RecordNameToTag == NULL)
    VALUEPARSER::RecordNameToTag = new Map();
  VALUEPARSER::RecordNameToTag->Insert(SEQ<Char>(nm), Int(tag));
}

bool CGUTIL::ParseVDMValue(const wstring & filename, Generic & res)
{
//wcerr << L"ParseVDMValue: " << filename << endl;
//   valuepdebug = 1;
  ifstream ifstr (TBWSTR::wstring2fsstr(filename).c_str());
  if (ifstr.good())
  {
    VALUEPARSER::exprs = &res;
    VLLEXINPUT.ReadFrom(ifstr);
    VLLEXINPUT.reread();
    int result = !valuepparse();
    ifstr.close();
    return result;
  }
  return false;
}

bool CGUTIL::ParseVDMValueString(const wstring & str, Generic & res)
{
//wcerr << L"ParseVDMValueString: " << str << endl;
  VALUEPARSER::exprs = &res;
  string instr (TBWSTR::ConvertToHexquad(str));
  VLLEXINPUT.ReadFrom(instr);
  VLLEXINPUT.reread();
  int result = !valuepparse();
  return result;
}
