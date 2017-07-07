#ifndef __javascan_h__
#define __javascan_h__

#include <iostream>
#include <fstream>
#include "metaiv.h"
#include "CPP.h" 
#include "CI.h" 
#include "contextinfo.h" 
#include <deque>

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size)\
{\
 int c = JAVASCANNER::javasLEXINPUT.getchars(buf, max_size);\
 result = (c==0) ? YY_NULL : c;\
}

class javasLexPos {
private:
  int  SectionCount, SectionLineCount, RelAbsLineCount, AbsLineCount, ColumnCount; 
public:
  void init() 
         { SectionCount = 1;     // The section number in the file
           AbsLineCount = 1; 
           RelAbsLineCount = 1;
           SectionLineCount = 1; // line count relative to section
           ColumnCount = 1; 
         }
  javasLexPos()  { init(); }
  void IncreaseColumn(int c) { ColumnCount += c; }
  void Newline() { ColumnCount = 1; SectionLineCount++; AbsLineCount++; RelAbsLineCount++;  }
  void RelNewLine() { RelAbsLineCount-= 2; } // This newline should not be counted.
  void SetSectionCount(int c) { SectionCount = c; }
  int GetSectionCount() const { return SectionCount; }
  void SetSectionLineCount(int c) { SectionLineCount = c; }
  int GetSectionLineCount() const { return SectionLineCount; }
  int GetRelAbsLineCount() const { return RelAbsLineCount; }
  int GetAbsLineCount() const { return AbsLineCount; }
  int GetColumnCount() const { return ColumnCount; }
}; 

class javasLXINPUT {
private: 
  string st; 
  int s_index; 
  istream *is; 

  int String_getchars(char*, int);
  int Stream_getchars (char*, int);

  int (javasLXINPUT:: *current_getchars) (char*, int); // Points to currently used getchars function. 
  void (javasLXINPUT:: *current_reread) ();            // Points to currently used reread function. 

  void String_reread () { s_index = 0; }
  void Stream_reread () { is->clear(); is->seekg(0); } 


  int EMPTY_getchars (char*, int) { return 0; }
  void EMPTY_reread() {}

public:
  javasLXINPUT ()  {
    current_getchars = &javasLXINPUT::EMPTY_getchars;
    current_reread = &javasLXINPUT::EMPTY_reread; 
  }
  int getchars (char*p, int maxsz) { return (this ->*current_getchars) (p, maxsz); }
  void reread () { (this->*current_reread)(); } 
  void ReadFrom (const string & s);
  void ReadFrom (istream& s);
}; 

// Remember to reinitialize lex and yacc where needed. 


#define YYSTYPE javasAsTree

union NodeType {
  Sequence* uSequence;
  Record* uRecord;
  string *LexStringP; 

  TYPE_CPP_SimpleTypeName*                uSimpleTypeName;
  TYPE_CPP_Bool*   uBool;
  TYPE_CPP_Short*  uShort;

  TYPE_CPP_SimplePackageName*             uSimplePackageName;
  TYPE_CPP_Identifier*                    uIdentifier;
  TYPE_CPP_QualifiedPackageName*          uQualifiedPackageName;
  TYPE_CPP_PackageName*                   uPackageName;
  TYPE_CPP_PackageDeclaration*            uPackageDeclaration;
  TYPE_CPP_ImportDeclaration*             uImportDeclaration;
  TYPE_CPP_SingleTypeImportDeclaration*   uSingleTypeImportDeclaration;
  TYPE_CPP_TypeImportOnDemandDeclaration* uTypeImportOnDemandDeclaration;

  TYPE_CPP_MemberDeclaration* uMemberDeclaration;
  TYPE_CPP_Modifier*  uModifier;
  TYPE_CPP_TypeSpecifier*  uTypeSpecifier;
  TYPE_CPP_FunctionDefinition* uFunctionDefinition;
  TYPE_CPP_FctDecl* uFctDecl;
  TYPE_CPP_FctBody* uFctBody;
  TYPE_CPP_ArgDecl* uArgumentDeclaration;
  TYPE_CPP_Declarator* uDeclarator;
  TYPE_CPP_ArrayDecl* uArrayDecl;
  TYPE_CPP_IdentDeclaration* uIdentDeclaration;
  TYPE_CPP_InitDeclarator* uInitDeclarator;
  TYPE_CPP_ClassInstanceCreationExpr* uClassInstanceCreationExpr;
  TYPE_CPP_Expr* uExpr;

  TYPE_CPP_Stmt* uStmt;
  TYPE_CPP_LabelStmt* uLabelStmt;
  TYPE_CPP_CaseStmt* uCaseStmt;
  TYPE_CPP_CompoundStmt* uCompoundStmt;
  TYPE_CPP_IfStmt* uIfStmt;
  TYPE_CPP_SwitchStmt* uSwitchStmt;
  TYPE_CPP_WhileStmt* uWhileStmt;
  TYPE_CPP_DoWhileStmt* uDoWhileStmt;
  TYPE_CPP_ForStmt* uForStmt;
  TYPE_CPP_ForEachStmt* uForEachStmt;
  TYPE_CPP_Break*    uBreak;
  TYPE_CPP_Continue* uContinue;
  TYPE_CPP_Return* uReturn;
  TYPE_CPP_SynchronizedStmt* uSynchronizedStmt;
  TYPE_CPP_TryBlock* uTryBlock;
  TYPE_CPP_EmptyStmt* uEmptyStmt;
  TYPE_CPP_DeclarationStmt* uDeclarationStmt;
  TYPE_CPP_ExpressionStmt* uExpressionStmt;

  TYPE_CPP_StaticInitializer* uStaticInitializer;

  TYPE_CPP_Literal*      uLiteral;
  TYPE_CPP_IntegerLit*   uIntegerLit;
  TYPE_CPP_CharacterLit* uCharacterLit;
  TYPE_CPP_FloatingLit*  uFloatingLit;
  TYPE_CPP_StringLit*    uStringLit;
  TYPE_CPP_BoolLit*      uBoolLit;
  TYPE_CPP_NullLit*      uNullLit;

  TYPE_CPP_ThrowExpression* uThrowExpression;
  TYPE_CPP_AssignExpr* uAssignExpr;
  TYPE_CPP_AssignOp*   uAssignOp;
  TYPE_CPP_PreInDecrementExpr* uPreInDecrementExpr;
  TYPE_CPP_CastExpr* uCastExpr;
  TYPE_CPP_BinExpr* uBinExpr;
  TYPE_CPP_ShiftExpr* uShiftExpr;
  TYPE_CPP_RelationalExpr* uRelationalExpr;
  TYPE_CPP_TypeCompExpr* uTypeCompExpr;
  TYPE_CPP_EqualityExpr* uEqualityExpr;
  TYPE_CPP_LogicalExpr* uLogicalExpr;
  TYPE_CPP_ObjectMemberAccess* uObjectMemberAccess;

  TYPE_CPP_Handler* uHandler;
  TYPE_CPP_ExceptionDeclaration* uExceptionDeclaration;
  TYPE_CPP_BitExpr* uBitExpr;
  TYPE_CPP_PostFixInDecrementExpr* uPostFixInDecrementExpr;
  TYPE_CPP_ConditionalExpr* uConditionalExpr;
  TYPE_CPP_FctCall* uFctCall;
  TYPE_CPP_ArrayApply* uArrayApply;

  TYPE_CPP_ArrayCreationExpr* uArrayCreationExpr;
  TYPE_CPP_ArrayNewDecl* uArrayNewDecl;
  TYPE_CPP_ArrayInitializer* uArrayInitializer;

  TYPE_CPP_SwitchBlock* uSwitchBlock;
  TYPE_CPP_SwitchBlockStmtGrp* uSwitchBlockStmtGrp;
  TYPE_CPP_SwitchLabels* uSwitchLabels;
};

typedef union NodeType YYSTYPE;

struct yyltype 
{
// Used by bison: 
  int first_line, first_column, last_line, last_column; 
// Used by us. 
  TYPE_CI_TokenSpan pos; 
  char *text; 
  yyltype() :
//      pos(), 
      first_line(-1), first_column(-1), 
      last_line(-1), last_column(-1)
  {
    pos.Init(Int(-1), Int(-1), Int(-1));
  } 
}; 

#define YYLTYPE yyltype

int javaplex(YYSTYPE*yylval, YYLTYPE*yylpos);
#define YY_DECL int javaslex(YYSTYPE*yylval, YYLTYPE*yylpos)
YY_DECL; 

class JAVASCANNER
{
public:
  static int javasGetLineCount ();
  static int javasGetColumnCount ();
  static void javasInitCounts ();
  static void javasInitLex ();

  static void javasSave_LexStringP (YYSTYPE* , const char *);

  static void javasIniPos (YYLTYPE *pos);
  static void javasAdjustLineColumn (char* s);

  static void javasSetTokenList (TokenList * tkl);
  static void javasINSERT_TOKEN2 (YYLTYPE *pos, int tok, char * txt);

  static void javasFreeLexStrings () { javasSaved_Strings.clear(); } // Call this function after parse is done.

  static string javasTextString;

  static javasLexPos javasCurrentPos, javasNextPos;

  static javasLXINPUT javasLEXINPUT;

  static std::deque<string> javasSaved_Strings;

  static void SetPTextString(const string & s) { pTextString = s; }
  static string GetPTextString() { return pTextString; }
  static void SetBooleanValue(bool b) { booleanValue = b; }
  static Bool GetBooleanValue() { return Bool(booleanValue); }
  static void ClearStringValue() { stringValue.Clear(); }
  static void AppendChar(const Char & ch) { stringValue.ImpAppend(ch); }
  static SEQ<Char> GetStringValue() { return stringValue; }
  static void SetCharValue(const Char & ch) { charValue = ch; }
  static Char GetCharValue() { return charValue; }
  
private:
  static TokenList * javas_global_token_list;

  static string pTextString;
  static bool booleanValue;
  static SEQ<Char> stringValue;
  static Char charValue;
};

#endif // __javascan_h__
