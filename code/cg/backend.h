/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/backend.h,v $
*  * VERSION
*  *    $Revision: 1.10 $
*  * DATE
*  *    $Date: 2005/06/28 04:16:59 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERS/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __backend_h__
#define __backend_h__

#include "metaiv.h"
#include "CPP.h"
#include <fstream>

class CGBackEnd
{
public:
  enum cg_backend_kind{JAVA, CPP};

public:
  void GenFiles(const Generic & ast, enum cg_backend_kind kind);
  CGBackEnd() : level(0), indent(0) {};

private:
  enum contextType { NF, INCLUDE, IMPORT, MACRO, FDEF, FDECL, BE_DECL, CLASS, CSTMT, BLOCK, FOR, SWITCH, ELSE };
  contextType last;

  // level - nesting level, since we require a flat structure for keep
  // tags i.e.  they can not be nested
  int level;

  int indent;

  // generatedEntities - the set of entities corresponding to the Java
  // file that is current being generated
  Set generatedEntities;

  // existingEntitiesMap - map from tag names to CGTYPES`KeepCode
  // i.e. the code read from the existing file that has "yes" keep tags
  Map existingEntitiesMap;

  // existingEntitiesOrder - sequence of tag name, corresponds to the
  // order in which the "yes" keep tags appear in the existing file
  Sequence existingEntitiesOrder;

  enum cg_backend_kind asttype;

#ifdef TESTSPEC
  ofstream m4;
#endif // TESTSPEC

  ofstream m4code;

  // The following strings are used when outputting Java,
  // to retain hand editted code. They define the "keep tags" which
  // appear before and after code

  static const string startString;
  static const string endString;
  static const string nameString;
  static const string keepString;

  // String used for error messages when reading keep tags in a Java file
  static const wstring illFormed;

private:
  bool isCPP() const { return (this->asttype == CPP); };
  bool isJAVA() const { return (this->asttype == JAVA); };
  void NewScope() { this->indent = 0; };
  void OpenScope() { this->indent += 2; };
  void CloseScope() { this->indent -= 2; };
  void IncLevel() { this->level++; };
  void DecLevel() { this->level--; };
  int GetLevel() { return this->level; };
  void IND(ostream & o);
  void SetContext(contextType t) { this->last = t; };
  contextType GetContext() { return this->last; };
  wstring PrintContext();

  wstring GetName(const Generic & decl);
  wstring GetMangleName(const Generic & decl);
  string StartKeepTag(const Generic & decl, bool override = false, bool keep = false);
  string EndKeepTag(const Generic & decl, bool override = false);
  Set GetHeadEntities(const TYPE_CPP_PackageAndImportDeclarations & head);
  Set GetASTEntities(const Generic & ast, bool inClass);

#ifdef TESTSPEC
  static wstring GiveToolVersionDate ();
#endif // TESTSPEC
  string CheckExisting(wstring entityName, bool override = false);
  wstring GetKeepTagName(const string & text,
                         int searchPos, int &endNamePos,
                         int linenum, const wstring& filename,
                         bool expectSpace);
  Tuple ReadJavaFile(ifstream* javaFile, const wstring & filename);
  void GenNewLine(ostream & o);
  void OpenBlock();
  void CloseBlock();
  void GenFile(const TYPE_CPP_File & rc);
  void GenTypeSpecifier(const TYPE_CPP_TypeSpecifier & rc);
  void GenPackageAndImportDeclarations(const TYPE_CPP_PackageAndImportDeclarations & rc);
  void GenPackageDeclaration(const TYPE_CPP_PackageDeclaration & rc);
  void GenTypeImportOnDemandDeclaration(const TYPE_CPP_TypeImportOnDemandDeclaration & rc);
  void GenSingleTypeImportDeclaration(const TYPE_CPP_SingleTypeImportDeclaration &rc);
  void GenIdMacroDef(const TYPE_CPP_IdMacroDef & rc);
  void GenFctMacroDef(const TYPE_CPP_FctMacroDef & rc);
  void GenSquareIncl(const TYPE_CPP_SquareIncl& rc);
  void GenQuoteIncl(const TYPE_CPP_QuoteIncl & rc);
  void GenIdentDeclaration(const TYPE_CPP_IdentDeclaration & rc);
  void GenResScopeQualifiedClassName(const TYPE_CPP_ResScopeQualifiedClassName & rc);
  void GenEnumSpecifier(const TYPE_CPP_EnumSpecifier & rc);
  void GenEnumDefaultAssgn(const TYPE_CPP_EnumDefaultAssgn & rc);
  void GenElaboratedTypeSpecifier(const TYPE_CPP_ElaboratedTypeSpecifier &rc);
  void GenFunctionDefinition(const TYPE_CPP_FunctionDefinition & rc);
  void GenMemInitializer(const TYPE_CPP_MemInitializer & rc);
  void GenFctBody(const TYPE_CPP_FctBody & rc);
  void GenInitDeclarator(const TYPE_CPP_InitDeclarator & rc);
  void GenAsgnInit(const TYPE_CPP_AsgnInit & rc);
  void GenEnumInit(const TYPE_CPP_EnumInit & rc);
  void GenObjectInit(const TYPE_CPP_ObjectInit & rc);
  void GenInitializerList(const TYPE_CPP_InitializerList & rc);
  void GenTypeDefName(const TYPE_CPP_TypeDefName & rc);
  void GenCode(const Generic &);
  void GenLogInfo (ostream & o);
  void GenStmt(const TYPE_CPP_Stmt &, bool = false);
  void GenClassSpecifier(const TYPE_CPP_ClassSpecifier & rc);
  void GenClassHead(const TYPE_CPP_ClassHead & rc);
  void GenMemberList(const TYPE_CPP_MemberList & rc);
  void GenMemberSpecifier(const TYPE_CPP_MemberSpecifier & rc);
  void GenInterfaceSpecifier(const TYPE_CPP_InterfaceSpecifier & rc);
  void GenInterfaceHead(const TYPE_CPP_InterfaceHead & rc);
  void GenIfStmt(const TYPE_CPP_IfStmt & rc);
  void GenExpressionStmt(const TYPE_CPP_ExpressionStmt & rc);
  void GenCompoundStmt(const TYPE_CPP_CompoundStmt & rc);

  int Quote2Int(const Quote & op);
  string Quote2String(const Quote &);
  int OpPrec(const Generic &);
  void GenCodeSeq(const Sequence &, const string &);
  void OutputErr(const wstring &);
  void OutputLog(const wstring & msg);
};

#endif // __backend_h__
