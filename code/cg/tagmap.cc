/***
*  * WHAT
*  *   implementation of mod_cppast.vdm r1.24
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/tagmap.cc,v $
*  * VERSION
*  *    $Revision: 1.25 $
*  * DATE
*  *    $Date: 2006/03/14 07:15:37 $
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

#include "tagmap.h"

// FILES CONTAINING TAG VALUES

#include "cgtag.h"
#include "asquotes.h"

///////////////////////
  // Record tags for abstract syntax for C++

  // Files and preprocessors

static struct InitTagsTable {
  const wchar_t * Tag;
  int TagNum;
}

Tags[] = {
  {L"AccCompleteClassSpecifier_CPP", AccCompleteClassSpecifier_CPP},
  {L"AllocationNewTypeExpr_CPP", AllocationNewTypeExpr_CPP},
  {L"AllocationTypeExpr_CPP", AllocationTypeExpr_CPP},
  {L"ArgAbsDecl_CPP", ArgAbsDecl_CPP},
  {L"ArgDecl_CPP", ArgDecl_CPP},
  {L"ArgumentDeclarationList_CPP", ArgumentDeclarationList_CPP},
  {L"ArrayAbsDecl_CPP", ArrayAbsDecl_CPP},
  {L"ArrayApply_CPP", ArrayApply_CPP},
  {L"ArrayDecl_CPP", ArrayDecl_CPP},
  {L"ArrayNewDecl_CPP", ArrayNewDecl_CPP},
  {L"AsgnInit_CPP", AsgnInit_CPP},
  {L"AssignExpr_CPP", AssignExpr_CPP},
  {L"AssignOp_CPP", AssignOp_CPP},
  {L"BinExpr_CPP", BinExpr_CPP},
  {L"BinOp_CPP", BinOp_CPP},
  {L"BitExpr_CPP", BitExpr_CPP},
  {L"BitOp_CPP", BitOp_CPP},
  {L"Bool_CPP", Bool_CPP},
  {L"BoolLit_CPP", BoolLit_CPP},
  {L"BracketedAbsDecl_CPP", BracketedAbsDecl_CPP},
  {L"BracketedDecl_CPP", BracketedDecl_CPP},
  {L"BracketedExpr_CPP", BracketedExpr_CPP},
  {L"Break_CPP", Break_CPP},
  {L"CaseStmt_CPP", CaseStmt_CPP},
  {L"CastExpr_CPP", CastExpr_CPP},
  {L"CharacterLit_CPP", CharacterLit_CPP},
  {L"Char_CPP", Char_CPP},
  {L"ClassHead_CPP", ClassHead_CPP},
#ifdef VDMPP
  {L"ClassInstanceCreationExpr_CPP", ClassInstanceCreationExpr_CPP},
#endif // VDMPP
  {L"ClassNewDecl_CPP", ClassNewDecl_CPP},
//  {L"ClassResScopeQualifiedClassName_CPP", ClassResScopeQualifiedClassName_CPP},
  {L"ClassSpecifier_CPP", ClassSpecifier_CPP},
  {L"CompoundStmt_CPP", CompoundStmt_CPP},
  {L"ConditionalExpr_CPP", ConditionalExpr_CPP},
  {L"Continue_CPP", Continue_CPP},
//  {L"ConversionFunctionName_CPP", ConversionFunctionName_CPP},
//  {L"ConversionTypeName_CPP", ConversionTypeName_CPP},
  {L"CVNewDeclarator_CPP", CVNewDeclarator_CPP},
  {L"DeallocationArrayExpr_CPP", DeallocationArrayExpr_CPP},
  {L"DeallocationExpr_CPP", DeallocationExpr_CPP},
  {L"DeclarationStmt_CPP", DeclarationStmt_CPP},
  {L"DeclPureSpec_CPP", DeclPureSpec_CPP},
  {L"DefaultArgAbsDecl_CPP", DefaultArgAbsDecl_CPP},
  {L"DefaultArgDecl_CPP", DefaultArgDecl_CPP},
  {L"DefaultStmt_CPP", DefaultStmt_CPP},
  {L"Destructor_CPP", Destructor_CPP},
  {L"Double_CPP", Double_CPP},
  {L"DoWhileStmt_CPP", DoWhileStmt_CPP},
  {L"ElaboratedTypeSpecifier_CPP", ElaboratedTypeSpecifier_CPP},
  {L"EmptyStmt_CPP", EmptyStmt_CPP},
  {L"EnumDefaultAssgn_CPP", EnumDefaultAssgn_CPP},
  {L"EnumInit_CPP", EnumInit_CPP},
  {L"EnumName_CPP", EnumName_CPP},
  {L"EnumSpecifier_CPP", EnumSpecifier_CPP},
  {L"EqualityExpr_CPP", EqualityExpr_CPP},
  {L"EqOp_CPP", EqOp_CPP},
  {L"ExplTypeConv_CPP", ExplTypeConv_CPP},
  {L"ExpressionStmt_CPP", ExpressionStmt_CPP},
  {L"FctAbsDecl_CPP", FctAbsDecl_CPP},
  {L"FctBody_CPP", FctBody_CPP},
  {L"FctCall_CPP", FctCall_CPP},
  {L"FctDecl_CPP", FctDecl_CPP},
  {L"FctMacroDef_CPP", FctMacroDef_CPP},
  {L"FctSpecifier_CPP", FctSpecifier_CPP},
  {L"File_CPP", File_CPP},
  {L"Float_CPP", Float_CPP},
  {L"FloatingLit_CPP", FloatingLit_CPP},
  {L"ForStmt_CPP", ForStmt_CPP},
  {L"Friend_CPP", Friend_CPP},
  {L"FunctionDefinition_CPP", FunctionDefinition_CPP},
  {L"Goto_CPP", Goto_CPP},
  {L"IdentDeclaration_CPP", IdentDeclaration_CPP},
  {L"Identifier_CPP", Identifier_CPP},
  {L"IdMacroDef_CPP", IdMacroDef_CPP},
  {L"IfStmt_CPP", IfStmt_CPP},
  {L"IncludeStmt_CPP", IncludeStmt_CPP},
  {L"IndirectionAbsDecl_CPP", IndirectionAbsDecl_CPP},
  {L"IndirectionDecl_CPP", IndirectionDecl_CPP},
  {L"InitDeclarator_CPP", InitDeclarator_CPP},
  {L"InitializerList_CPP", InitializerList_CPP},
  {L"Int_CPP", Int_CPP},
  {L"IntegerLit_CPP", IntegerLit_CPP},
#ifdef VDMPP
  {L"InterfaceHead_CPP", InterfaceHead_CPP},
  {L"InterfaceSpecifier_CPP", InterfaceSpecifier_CPP},
#endif // VDMPP
  {L"LabelStmt_CPP", LabelStmt_CPP},
  {L"LogicalExpr_CPP", LogicalExpr_CPP},
  {L"LogOp_CPP", LogOp_CPP},
  {L"Long_CPP", Long_CPP},
  {L"MemberList_CPP", MemberList_CPP},
  {L"MemberSpecifier_CPP", MemberSpecifier_CPP},
  {L"MemInitializer_CPP", MemInitializer_CPP},
  {L"Modifier_CPP", Modifier_CPP},
  {L"NewInitializer_CPP", NewInitializer_CPP},
  {L"NewTypeName_CPP", NewTypeName_CPP},
  {L"NullLit_CPP", NullLit_CPP},
  {L"ObjectInit_CPP", ObjectInit_CPP},
  {L"ObjectMemberAccess_CPP", ObjectMemberAccess_CPP},
//  {L"Operator_CPP", Operator_CPP},
//  {L"OperatorFunctionName_CPP", OperatorFunctionName_CPP},
#ifdef VDMPP
  {L"PackageAndImportDeclarations_CPP", PackageAndImportDeclarations_CPP},
  {L"PackageDeclaration_CPP", PackageDeclaration_CPP},
#endif // VDMPP
  {L"PmExpr_CPP", PmExpr_CPP},
  {L"PmOp_CPP", PmOp_CPP},
//  {L"PointerDecl_CPP", PointerDecl_CPP},
  {L"PointerToMemberAbs_CPP", PointerToMemberAbs_CPP},
  {L"PointerToMember_CPP", PointerToMember_CPP},
//  {L"PointerToMemberDecl_CPP", PointerToMemberDecl_CPP},
  {L"PointerToObjectMemberAccess_CPP", PointerToObjectMemberAccess_CPP},
  {L"PostFixInDecrementExpr_CPP", PostFixInDecrementExpr_CPP},
  {L"PreDefine_CPP", PreDefine_CPP},
  {L"PreElse_CPP", PreElse_CPP},
  {L"PreEndIf_CPP", PreEndIf_CPP},
  {L"PreIf_CPP", PreIf_CPP},
  {L"PreIfDef_CPP", PreIfDef_CPP},
  {L"PreIfNotDef_CPP", PreIfNotDef_CPP},
  {L"PreInDecrementExpr_CPP", PreInDecrementExpr_CPP},
  {L"PreMacro_CPP", PreMacro_CPP},
  {L"QualifiedName_CPP", QualifiedName_CPP},
#ifdef VDMPP
  {L"QualifiedPackageName_CPP", QualifiedPackageName_CPP},
#endif // VDMPP
  {L"QuoteIncl_CPP", QuoteIncl_CPP},
  {L"RefAbsDecl_CPP", RefAbsDecl_CPP},
  {L"RefDecl_CPP", RefDecl_CPP},
//  {L"RefTypeDecl_CPP", RefTypeDecl_CPP},
  {L"RelationalExpr_CPP", RelationalExpr_CPP},
  {L"RelOp_CPP", RelOp_CPP},
  {L"ResScopeQualifiedClassName_CPP", ResScopeQualifiedClassName_CPP},
  {L"Return_CPP", Return_CPP},
//  {L"ScopeResIdentifier_CPP", ScopeResIdentifier_CPP},
//  {L"ScopeResOpFctName_CPP", ScopeResOpFctName_CPP},
//  {L"ScopeResQualifiedName_CPP", ScopeResQualifiedName_CPP},
  {L"ShiftExpr_CPP", ShiftExpr_CPP},
  {L"ShOp_CPP", ShOp_CPP},
  {L"Short_CPP", Short_CPP},
  {L"Signed_CPP", Signed_CPP},
#ifdef VDMPP
  {L"SimplePackageName_CPP", SimplePackageName_CPP},
  {L"SingleTypeImportDeclaration_CPP", SingleTypeImportDeclaration_CPP},
#endif // VDMPP
  {L"SizeOfExpr_CPP", SizeOfExpr_CPP},
  {L"SizeOfType_CPP", SizeOfType_CPP},
  {L"SquareIncl_CPP", SquareIncl_CPP},
  {L"StaticInitializer_CPP", StaticInitializer_CPP},
  {L"StorageClassSpecifier_CPP", StorageClassSpecifier_CPP},
  {L"StringLit_CPP", StringLit_CPP},
  {L"SynchronizedStmt_CPP", SynchronizedStmt_CPP},
  {L"SwitchStmt_CPP", SwitchStmt_CPP},
  {L"TcOp_CPP", TcOp_CPP},
  {L"TemplateClassName_CPP", TemplateClassName_CPP},
  {L"TemplateName_CPP", TemplateName_CPP},
  {L"This_CPP", This_CPP},
  {L"TypeCompExpr_CPP", TypeCompExpr_CPP},
  {L"TypeDef_CPP", TypeDef_CPP},
  {L"TypeDefName_CPP", TypeDefName_CPP},
#ifdef VDMPP
  {L"TypeImportOnDemandDeclaration_CPP", TypeImportOnDemandDeclaration_CPP},
#endif // VDMPP
  {L"TypeName_CPP", TypeName_CPP},
  {L"TypeSpecifier_CPP", TypeSpecifier_CPP},
  {L"UnaryOpExpr_CPP", UnaryOpExpr_CPP},
  {L"UnaryOp_CPP", UnaryOp_CPP},
  {L"Unsigned_CPP", Unsigned_CPP},
  {L"VirCompleteClassSpecifier_CPP", VirCompleteClassSpecifier_CPP},
  {L"Void_CPP", Void_CPP},
  {L"WhileStmt_CPP", WhileStmt_CPP},
  {L"BlockComments_CPP", BlockComments_CPP},
  {L"SingleLineComments_CPP", SingleLineComments_CPP},
  {L"TrailingComments_CPP", TrailingComments_CPP},
  {L"EndOfLineComments_CPP", EndOfLineComments_CPP},
  {L"DocComments_CPP", DocComments_CPP},


// Exception Handling - not supported

  {L"All_CPP", All_CPP},
  {L"ExceptionDeclaration_CPP", ExceptionDeclaration_CPP},
  {L"Handler_CPP", Handler_CPP},
  {L"ThrowExpression_CPP", ThrowExpression_CPP},
  {L"TryBlock_CPP", TryBlock_CPP},

  // mk_(Quote definitions

  // AssignOp
  {L"ASEQUAL", ASEQUAL_CPP},
  {L"ASMULT", ASMULT_CPP},
  {L"ASDIV", ASDIV_CPP},
  {L"ASMOD", ASMOD_CPP},
  {L"ASPLUS", ASPLUS_CPP},
  {L"ASMINUS", ASMINUS_CPP},
  {L"ASRIGHTSHIFT", ASRIGHTSHIFT_CPP},
  {L"ASLEFTSHIFT", ASLEFTSHIFT_CPP},
  {L"ASBITWISEAND", ASBITWISEAND_CPP},
  {L"ASBITWISEEXOR", ASBITWISEEXOR_CPP},
  {L"ASBITWISEINCLOR", ASBITWISEINCLOR_CPP},

  // LogOp
  {L"OR", OR_CPP},
  {L"AND", AND_CPP},

  // BitOp
  {L"EXCLOR", EXCLOR_CPP},
  {L"BITAND", BITAND_CPP},
  {L"BITOR", BITOR_CPP},

  // EqOp
  {L"EQ", EQ_CPP},
  {L"NEQ", NEQ_CPP},

  // RelOp
  {L"LT", LT_CPP},
  {L"GT", GT_CPP},
  {L"LEQ", LEQ_CPP},
  {L"GEQ", GEQ_CPP},

  //Type Comparison in Java
  {L"ISINSTANCEOF", ISINSTANCEOF_CPP},

  // ShOp
  {L"LEFTSHIFT", LEFTSHIFT_CPP},
  {L"RIGHTSHIFT", RIGHTSHIFT_CPP},

  // BinOp
  {L"MULT", MULT_CPP},
  {L"MOD", MOD_CPP},

  // PmOp
  {L"DOTSTAR", DOTSTAR_CPP},

  // IncDecOp
  {L"PPLUS", PPLUS_CPP},
  {L"PMINUS", PMINUS_CPP},

  // UnaryOp
  {L"INDIRECTION", INDIRECTION_CPP},
  {L"ADDROP", ADDROP_CPP},
  {L"NEG", NEG_CPP},
  {L"COMPL", COMPL_CPP},
  {L"REFERENCE", REFERENCE_CPP},

  // Operator
//  {L"NEW", NEW_CPP},
//  {L"DELETE", DELETE_CPP},
//  {L"PLUS", PLUS_CPP},
//  {L"MINUS", MINUS_CPP},
//  {L"STAR", STAR_CPP},
//  {L"DIV", DIV_CPP},
//  {L"PRECENTAGEMARK", PRECENTAGEMARK_CPP},
//  {L"CIRCUMFEX", CIRCUMFEX_CPP},
//  {L"AMPERSAND", AMPERSAND_CPP},
//  {L"VERTICALLINE", VERTICALLINE_CPP},
//  {L"TILDE", TILDE_CPP},
//  {L"EXCLAMATIONMARK", EXCLAMATIONMARK_CPP},
//  {L"OPEQUAL", OPEQUAL_CPP},
//  {L"LESS", LESS_CPP},
//  {L"GREATER", GREATER_CPP},
//  {L"PLUSEQUAL", PLUSEQUAL_CPP},
//  {L"MINUSEQUAL", MINUSEQUAL_CPP},
//  {L"STAREQUAL", STAREQUAL_CPP},
//  {L"BACKSLASHEQUAL", BACKSLASHEQUAL_CPP},
//  {L"PERCENTAGEMARKEQUAL", PERCENTAGEMARKEQUAL_CPP},
//  {L"CIRCUMFLEXEQUAL", CIRCUMFLEXEQUAL_CPP},
//  {L"ANDEQUAL", ANDEQUAL_CPP},
//  {L"VERTICALLINEEQUAL", VERTICALLINEEQUAL_CPP},
//  {L"DOUBLELESS", DOUBLELESS_CPP},
//  {L"DOUBLEGREATER", DOUBLEGREATER_CPP},
//  {L"DOUBLELESSEQUAL", DOUBLELESSEQUAL_CPP},
//  {L"DOUBLEGREATEREQUAL", DOUBLEGREATEREQUAL_CPP},
//  {L"DOUBLEEQUAL", DOUBLEEQUAL_CPP},
//  {L"EXCLAMATIONMARKEQUAL", EXCLAMATIONMARKEQUAL_CPP},
//  {L"LESSEQUAL", LESSEQUAL_CPP},
//  {L"GREATEREQUAL", GREATEREQUAL_CPP},
//  {L"DOUBLEAND", DOUBLEAND_CPP},
//  {L"DOUBLEVERTICALLINE", DOUBLEVERTICALLINE_CPP},
//  {L"DOUBLEPLUS", DOUBLEPLUS_CPP},
//  {L"DOUBLEMINUS", DOUBLEMINUS_CPP},
//  {L"COMMA", COMMA_CPP},
//  {L"ARROWSTAR", ARROWSTAR_CPP},
//  {L"ARROW", ARROW_CPP},
//  {L"BRACKETS", BRACKETS_CPP},
//  {L"SQUAREBRACKETS", SQUAREBRACKETS_CPP},

  {L"JAVA", JAVA_CPP},
  {L"CPP", CPP_CPP},

  // cv-qualitier
  {L"CONST", CONST_CPP},
  {L"VOLATILE", VOLATILE_CPP},

  {L"CLASS", CLASS_CPP},
  {L"STRUCT", STRUCT_CPP},
  {L"UNION", UNION_CPP},
  {L"PRIVATE", PRIVATE_CPP},
  {L"PROTECTED", PROTECTED_CPP},
  {L"PUBLIC", PUBLIC_CPP},
  {L"ABSTRACT", ABSTRACT_CPP},
  {L"FINAL", FINAL_CPP},
  {L"INLINE", INLINE_CPP},
  {L"VIRTUAL", VIRTUAL_CPP},
  {L"AUTO", AUTO_CPP},
  {L"REGISTER", REGISTER_CPP},
  {L"STATIC", STATIC_CPP},
  {L"EXTERN", EXTERN_CPP},

  // Tags from module AS

  {L"NOTYETSPEC", NOTYETSPEC},
#ifdef VDMPP
  {L"SUBRESP", SUBRESP},
  {L"ANY", ANY},
  {L"ATLEASTONE", ATLEASTONE},
  {L"POSSIBLY", POSSIBLY}, 
  {L"OBJECT", OBJECT},
  {L"LOCAL", LOCAL},
  {L"GLOBAL", GLOBAL},
#endif //VDMPP
  {L"DEFAULT_AS", DEFAULT_AS},
  {L"PRIVATE_AS", PRIVATE_AS},
  {L"PROTECTED_AS", PROTECTED_AS},
  {L"PUBLIC_AS", PUBLIC_AS},
  {L"NOT_INITIALISED_AS", NOT_INITIALISED_AS},
  {L"ALL", ALL},
  {L"STRUCT", STRUCT},
  {L"NATONE", NATONE},
  {L"NAT", NAT},
  {L"INTEGER", INTEGER},
  {L"RAT", RAT},
  {L"REAL", REAL},
  {L"READ", READ},
  {L"READWRITE", READWRITE},
  {L"READ_ONLY", READ_ONLY},
  {L"READ_WRITE", READ_WRITE},
  {L"NUMPLUS", NUMPLUS},
  {L"NUMMINUS", NUMMINUS},
  {L"NUMABS", NUMABS},
  {L"FLOOR", FLOOR},
  {L"NOT", NOT},
  {L"SETCARD", SETCARD},
  {L"SETDISTRUNION", SETDISTRUNION},
  {L"SETDISTRINTERSECT", SETDISTRINTERSECT},
  {L"SETPOWER", SETPOWER},
  {L"SEQLEN", SEQLEN},
  {L"SEQDISTRCONC", SEQDISTRCONC},
  {L"SEQELEMS", SEQELEMS},
  {L"SEQINDICES", SEQINDICES},
  {L"SEQTAIL", SEQTAIL},
  {L"SEQHEAD", SEQHEAD},
  {L"SEQREVERSE", SEQREVERSE},
  {L"MAPDOM", MAPDOM},
  {L"MAPRNG", MAPRNG},
  {L"MAPDISTRMERGE", MAPDISTRMERGE},
  {L"MAPINVERSE", MAPINVERSE},
  {L"NUMMULT", NUMMULT},
  {L"NUMDIV", NUMDIV},
  {L"NUMREM", NUMREM},
  {L"NUMMOD", NUMMOD},
  {L"NUMEXP", NUMEXP},
  {L"INTDIV", INTDIV},
  {L"NUMLT", NUMLT},
  {L"NUMLE", NUMLE},
  {L"NUMGT", NUMGT},
  {L"NUMGE", NUMGE},
  {L"AND", AND},
  {L"OR", OR},
  {L"IMPLY", IMPLY},
  {L"EQUIV", EQUIV},
  {L"EQ", EQ},
  {L"NE", NE},
  {L"SETUNION", SETUNION},
  {L"SETINTERSECT", SETINTERSECT},
  {L"SETMINUS", SETMINUS},
  {L"SUBSET", SUBSET},
  {L"PROPERSUBSET", PROPERSUBSET},
  {L"INSET", INSET},
  {L"NOTINSET", NOTINSET},
  {L"SEQCONC", SEQCONC},
  {L"MAPMERGE", MAPMERGE},
  {L"MAPDOMRESTTO", MAPDOMRESTTO},
  {L"MAPDOMRESTBY", MAPDOMRESTBY},
  {L"MAPRNGRESTTO", MAPRNGRESTTO},
  {L"MAPRNGRESTBY", MAPRNGRESTBY},
  {L"COMPOSE", COMPOSE},
  {L"EXISTS", EXISTS},
  {L"INPUT", AS_INPUT},
  {L"PURE", AS_PURE},
  {L"PUREOP", PUREOP},
  {L"PTHREAD", PTHREAD},

  {L"POS", POS},
  {L"DEF", DEF},

  {L"MACRO_FILE", MACRO_FILE},
  {L"MACRO_LINE", MACRO_LINE},
  {L"MACRO_COLUMN", MACRO_COLUMN},
  {L"MACRO_CLMOD", MACRO_CLMOD},
  {L"MACRO_FNOP", MACRO_FNOP},

  {0, 0}
};

TagsTable::TagsTable()
{
  for (int i = 0; Tags[i].Tag != 0; i++)
    Insert(Quote(Tags[i].Tag), Int(Tags[i].TagNum));
}

TagsTable::~TagsTable()
{
  this->q2imap.Clear();
  this->i2qmap.Clear();
}

void TagsTable::Insert(const Quote & key, const Int & tag)
{
  this->q2imap.Insert(key, tag);
  this->i2qmap.Insert(tag, key);
}

Quote TagsTable::GetKey(const Int & tag) const
{
  if (this->i2qmap.DomExists(tag))
    return this->i2qmap[tag];
  else
    return Quote(L"");
}

Int TagsTable::Compute(const Quote & key) const
{
  if (this->q2imap.DomExists(key))
    return this->q2imap[key];
  else
    return Int(0);
}

