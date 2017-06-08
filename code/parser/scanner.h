/***
*  * WHAT
*  *   $Locker:  $ $RCSfile: scanner.h,v $
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/scanner.h,v $
*  * VERSION
*  *    $Revision: 1.20 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * FORMAT
*  *    $State: Exp $
*  * AUTHOR
*  *    Author: Erik T. Nielsen, Michael Andersen + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) 1991-93 The Institute of Applied Computer Science
***/

#ifndef __scanner_h__
#define __scanner_h__

#include "metaiv.h"
#include "AS.h" 

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>

// Draft for reading from different sources in lex.

// Lex defines macro input() as follows.
// # define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)

// Insert appropriate place
//#undef input
//# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):LEXINPUT.getch())==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size)\
{\
 int c = SCANNER::getchars(buf, max_size);\
 result = (c==0) ? YY_NULL : c;\
}

class LXINPUT {
public:
  LXINPUT() : current_getchars(&LXINPUT::EMPTY_getchars), current_reread(&LXINPUT::EMPTY_reread) {}

  int getchars(char*p,int maxsz) { return (this->*current_getchars)(p,maxsz); }
  void reread () { (this->*current_reread)(); } 

  void ReadFrom(const std::string & s);
  void ReadFrom(istream & s);

private: 
  int (LXINPUT::*current_getchars)(char*,int); // Points to currently used getchars function. 
  void (LXINPUT::*current_reread)(); // Points to currently used reread function. 

  void String_reread () { s_index = 0; } 
  void Stream_reread () { is->clear(); is->seekg(0); }

  int String_getchars(char*,int);
  int Stream_getchars(char*,int);

  int EMPTY_getchars(char*,int) { return 0; }
  void EMPTY_reread() {}

  std::string st; 
  int s_index; 
  istream *is; 
}; 

//extern LXINPUT LEXINPUT;
    
// Remember to reinitialize lex and yacc where needed. 


#define YYSTYPE AsTree

union NodeType {
   Record* record;
   Sequence* sequence;
   Set* set;
   Map* map;
   Char* chartype;
   Bool* vdm_bool;
   Quote* quote;
   Real* real;
   Int* inttype;
   Text* text;
   Tuple* tuple;
   Token* token;
   Nil* nil;
   Generic *generic; 
   std::string *LexStringP; 
  // AST types. 
  TYPE_AS_Definitions * Definitions_p; 
  TYPE_AS_SpecificationStmt * SpecificationStatement_p; 
  TYPE_AS_OpBody * OpBody_p; 
  TYPE_AS_FnBody * FnBody_p; 
#if VDMSL
  TYPE_AS_StateDef * StateDef_p;
#endif // VDMSL
#if VDMPP
  TYPE_AS_StartListStmt * StartListStatement_p; 
  TYPE_AS_StartStmt * StartStatement_p; 
  TYPE_AS_Class * Class_p; 
  TYPE_AS_InstanceInv * InstanceInv_p;
  TYPE_AS_InstAssignDef * InstAssignDef_p;
#endif // VDMPP
};

typedef union NodeType YYSTYPE;

class LexPos {
private:
  int64_t  SectionCount, SectionLineCount, RelAbsLineCount, AbsLineCount, ColumnCount;
public:
  void init()
  { SectionCount = 1;     // The section number in the file
    AbsLineCount = 1;
    RelAbsLineCount = 1;
    SectionLineCount = 1; // line count relative to section
    ColumnCount = 1;
  }
  LexPos() : SectionCount(1), SectionLineCount(1), RelAbsLineCount(1), AbsLineCount(1), ColumnCount(1) {}
  void IncreaseColumn(int64_t c) { ColumnCount += c; }
  void Newline() { ColumnCount = 1; SectionLineCount++; AbsLineCount++; RelAbsLineCount++;  }
  void RelNewLine() { RelAbsLineCount-= 2; } // This newline should not be counted.
  void SetSectionCount(int64_t c) { SectionCount = c; }
  int64_t GetSectionCount() const { return SectionCount; }
  void SetSectionLineCount(int c) { SectionLineCount = c; }
  int64_t GetSectionLineCount() const { return SectionLineCount; }
  int64_t GetRelAbsLineCount() const { return RelAbsLineCount; }
  int64_t GetAbsLineCount() const { return AbsLineCount; }
  int64_t GetColumnCount() const { return ColumnCount; }
};

#include "contextinfo.h" 

//////////////////////////////////////////////////
// These two defines are put into the bison generated parser code
// when it has been filtered by the fix_posassign script. 
// This code really belongs in common.y put it can't
// be put there since common.y is preprocessed when creating the
// various parsers. 
//////////////////////////////////////////////////

#define YYLSP_SETPOS_EMPTYRULE setpos_emptyrule(yylsp, yylloc); 
#define YYLSP_SETPOS_NONEMPTYRULE setpos_nonemptyrule(yylsp, yylen); 

//////////////////////////////////////////////////
      
struct yyltype 
{
  // Used by bison: 
  int first_line, first_column, last_line, last_column; 
  // Used by us. 
  TYPE_CI_TokenSpan pos; 
  char *text; 

  yyltype() : first_line(-1), first_column(-1), last_line(-1), last_column(-1)
  {
    this->pos.Init(TYPE_CI_TokenRef(-1), TYPE_CI_TokenRef(-1), TYPE_CI_TokenRef(-1));
    this->text = NULL;
  } 
}; 

#define YYLTYPE yyltype
#define YY_DECL int yylex(YYSTYPE *yylval, YYLTYPE*yylpos)
YY_DECL; 

class SCANNER
{
public:
  static int getchars(char*p,int maxsz) { return LEXINPUT.getchars(p, maxsz); }

  static int GetLineCount();
  static int GetColumnCount();
  static void InitCounts ();
  static void SetNewLine(int sec, int line);

  static void IniPos(YYLTYPE *pos); 
  static void AdjustLineColumn(const char* s);
  static int SearchReservedWord (const char* Candidate);
  static const char * SearchReservedName (int tk_num);

  static void InitLex ( void (*f)(const wchar_t *, YYLTYPE&pos) );

  static void SetTokenList(TokenList * tkl) { global_token_list = tkl; }
  static TokenList * GetTokenList() { return global_token_list; }

//#ifdef VDMPP
// These functions are used to mark if the last token was LEX_CLASS
// If so, then the identifier following the token is the class name,
// which is used later to decide if an operation is a constructor or not.
  static void  setLastWasLEX_CLASS_MOD(bool b) { lastWasLEX_CLASS_MOD = b; }
  static bool getLastWasLEX_CLASS_MOD() { return lastWasLEX_CLASS_MOD; }
//#endif //VDMPP

  static void INSERT_TOKEN2(YYLTYPE*, int ,const char * );

  static void Save_LexStringP(YYSTYPE*yylval, const char *p);
  static void FreeLexStrings() { Saved_Strings.clear(); } // Call this function after parse is done.

  static void (*ReportP) (const wchar_t*, YYLTYPE & pos);

  static const char * skipwsafter(const char*s, const char *after);
  static void lex_unput(const char *p);

  static void ReadFrom(const string & s) { LEXINPUT.ReadFrom(s); }
  static void ReadFrom(istream & s) { LEXINPUT.ReadFrom(s); }
  static void reread() { LEXINPUT.reread(); }

  static void SetReservedWord(const char * stringRepr, int token)
  {
    ReservedWordMap1.insert(std::map<string, int>::value_type(stringRepr, token));
    ReservedWordMap2.insert(std::map<int, const char *>::value_type(token, stringRepr));
  }
  static void ClearReservedWord()
  {
    ReservedWordMap1.clear();
    ReservedWordMap2.clear();
  }

  static std::string yytextstring;
private:
  static LexPos CurrentPos, NextPos;
  static LXINPUT LEXINPUT;
  static TokenList * global_token_list;
  static bool lastWasLEX_CLASS_MOD;

  static std::deque<string> Saved_Strings;
  static std::map<string, int> ReservedWordMap1;
  static std::map<int, const char *> ReservedWordMap2;
};

#endif // __scanner_h__
