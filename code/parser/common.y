// -*-Mode: text; -*-
/***
*  * WHAT
*  *   General parser file for VDM-SL Toolbox
*  *   and the VDM++ parser.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/parser/common.y,v $
*  * VERSION
*  *    $Revision: 1.191 $
*  * DATE
*  *    $Date: 2006/05/23 07:30:33 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Michael Andersen. Currently: $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) 2016 Kyushu University
***/

%{

#ifdef __GNUC__
#include <stdlib.h>
#endif // __GNUC__
#if !defined(__APPLE_CC__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif // !defined(__APPLE_CC__) && !defined(__FreeBSD__)

#include <stdio.h>
#include "metaiv.h"  // Basic Meta-IV/VDM data types and operations.
#include "AS.h"
#include "recover.h"
#include "binops.h"
#include "asquotes.h"
#include "tbutils.h"
#include "astaux.h"
#include "backslash.h"
#include "tb_exceptions.h"
#include "tb_wstring.h"

#ifdef VDMPP
#include "MANGLE.h"
#include "statsem.h"
#endif //VDMPP

#include "ProjectTypes.h"
#include "Errors.h"
#include "tokenlist.h"
#include "contextinfo.h"
#include "settings.h"

//
//#define YYDEBUG 1
//#define YYERROR_VERBOSE 1
//

//---------- Declarations used by the error recovery algorithm ----------

//extern int yylex(); // Function which read input from a the sequence 
                      // RepTokens if it is not empty, otherwise it reads 
                      // standard input
//-----------------------------------------------------------------------

// To build a non-Nil ast for post-expression.

#ifdef VDMPP
// Current version of the abstract syntax VDM++
#define ast_version  7
#endif //VDMPP

#include "scanner.h"

#ifdef FULL
#include "vdmpars.h"
Sequence * VDMPARSER::asts = NULL;
#define MYPARSER VDMPARSER
#elif VPPSTMT
#include "vppstmt.h"
TYPE_AS_AssignStmt * VDMSTMTPARSER::stmt = NULL;
#define MYPARSER VDMSTMTPARSER
#else
#include "pexpr.h"
Sequence * VDMEXPRPARSER::exprs = NULL;         // Set to addr of result parameter
#define MYPARSER VDMEXPRPARSER
#endif //FULL

TYPE_CI_FileId MYPARSER::FileId;
vdm_Errors * MYPARSER::Errsp = NULL;
ContextInfo * MYPARSER::pars_context_info_p = NULL;
bool MYPARSER::errors = false;

static void setpos_emptyrule (YYLTYPE *yylsp, const YYLTYPE & yylloc)
{
  yylsp->first_line = yylloc.first_line;
  yylsp->first_column = yylloc.first_column;
  yylsp->pos.set_token_ust(yylloc.pos.get_token_ust());
  yylsp->pos.set_token_uast(yylloc.pos.get_token_uast());
  yylsp->last_line = (yylsp-1)->last_line;
  yylsp->last_column = (yylsp-1)->last_column;
  yylsp->pos.set_token_uend((yylsp-1)->pos.get_token_uend());
  yylsp->text = 0;
}

#ifdef USE_BISON128
static void setpos_nonemptyrule (YYLTYPE *yylsp, int yylen)
{
  yylsp->last_line = (yylsp+yylen-1)->last_line;
  yylsp->last_column = (yylsp+yylen-1)->last_column;
  yylsp->pos.set_token_uend((yylsp+yylen-1)->pos.get_token_uend());
}
#endif // USE_BISON128

// Forward declaration used by recovery code. This function
// is used by the code inserted by Make at the yyerrlab: position.
static VDM_Recover& get_repair();

#ifdef FULL
#define ORDERLY_ON
Sequence VDMPARSER::orderly; // this sequence contains the order in which
                             // constructs are defined. This is only used
                             // in the parser generated for the latex
                             // generator. here, we do not want to change
                             // the order in which constructs are defined
                             // Following tags are defined in the orderly
                             // StateDef:: order : [nat]
                             // SyncDef :: order : nat
                             // ThreadDef :: order : nat
                             // InstVarDef :: order : nat
                             // TypeDef :: content : AS`TypeDef
                             // ValueDef :: content : AS`ValueDef
                             // FnDef :: content : AS`FunctionDefinition
                             // OpDef :: content : AS`OperationDefinition
                             // TexBreak :: order : [nat]

Sequence VDMPARSER::modorder; // this sequence contains the order in which
                              // the modules are defined. 
                              // An element in the sequence is a record:
                              // For VDMSL:
                              // ModOrder :: orderly : Orderly
                              //             tex     : bool    -- trailing tex after
                              //             name    : AS`Name -- Name of module
                              // For VDM++:
                              // ClassOrder :: orderly : Orderly
                              //               tex     : bool    -- trailing tex after
                              //               name    : AS`Name -- Name of Class

                              // The interface is not yet generated in the defining order.
Generic VDMPARSER::currentClassMod = Nil();
#ifdef VDMPP
Set VDMPARSER::mangledOpNames;
Set VDMPARSER::mangledFnNames;
#endif // VDMPP
#else
#define ORDERLY_ON //
#endif // FULL

static void yyerror(const char *);

%}

%pure-parser
// token_table ensures that YYMAXUTOK is output
%token-table

// NOTE: %token LINES SHOULD NOT CONTAIN #ifdef FULL!
// If they did we would get two different sets of token #defines
// and would need to generate two sets of scanners and two sets
// of ReservedWord tables.

%token LEX_OPERATIONS
%token LEX_OPERATION_ARROW
%token LEX_PURE

#ifdef VDMSL
// VDMSL Tokens:
%token LEX_DEFINITIONS
%token LEX_EXPORTS
%token LEX_IMPORTS
%token LEX_MODULE
%token LEX_RENAMED
%token LEX_STATE LEX_STRUCT
%token LEX_DLMODULE
%token LEX_FROM
%token LEX_INIT
#endif //VDMSL

%token LEX_USELIB

#ifdef VDMPP
// VDM++ Tokens:
%token LEX_CLASS LEX_DLCLASS
%token LEX_SELF
%token LEX_SAMECLASS LEX_SAMEBASECLASS
%token LEX_ISOFCLASS LEX_ISOFBASECLASS
%token LEX_SUBCLASS
%token LEX_RESPONSIBILITY
%token LEX_PRIVATE LEX_PROTECTED LEX_PUBLIC %token LEX_STATIC
%token LEX_INSTANCE
%token LEX_NEW
%token LEX_START LEX_STARTLIST LEX_STOP LEX_STOPLIST
%token LEX_VARIABLES
%token LEX_ACT LEX_FIN LEX_ACTIVE LEX_WAITING LEX_REQ LEX_PER LEX_MUTEX LEX_SYNC
%token LEX_THREAD LEX_THREADID LEX_PERIODIC LEX_SPORADIC
#ifdef VICE
%token LEX_DURATION LEX_CURTIME LEX_CYCLES LEX_ASYNC LEX_SYSTEM
#endif //VICE
%token LEX_OBJ_
#endif //VDMPP

//************************************************/
//* These are now valid for both VDMSL and VDMPP */
//************************************************/
%token LEX_LAMBDA
%token LEX_ERROR
%token LEX_ALWAYS
%token LEX_EXIT
%token LEX_TIXE
%token LEX_TRAP
%token LEX_IS
%token LEX_WITH
%token LEX_YET
%token LEX_SPECIFIED
%token LEX_NONDET
%token LEX_ATOMIC
%token LEX_TRACES

%token LEX_ABS LEX_ALL
%token LEX_AND
%token LEX_ARITHMETIC_INTEGER_DIVISION LEX_ARROW LEX_ASSIGN
%token LEX_ASSERT
%token LEX_BAR_ARROW LEX_BE LEX_BOOL LEX_BY
%token LEX_CARD LEX_CASES LEX_CHAR
%token LEX_COMP
%token LEX_COMPOSE
%token LEX_CONC
%token LEX_DCL LEX_DEF
%token LEX_DINTER LEX_DMERGE LEX_DO LEX_DOM
%token LEX_DONTCARE LEX_DOUBLE_COLON LEX_DUNION
%token LEX_ELEMS LEX_ELSE LEX_ELSEIF LEX_END LEX_EQUAL LEX_ERRS LEX_EXISTS LEX_EXISTS1
%token LEX_EXP_OR_ITERATE LEX_EXT
%token LEX_FUNCTIONS
%token LEX_FLOOR LEX_FOR LEX_FORALL
%token LEX_GREATER_THAN LEX_GREATER_THAN_OR_EQUAL
%token LEX_HD LEX_HOOK
%token LEX_IF LEX_IMPLY
%token LEX_IN LEX_INDS
%token LEX_IN_REVERSE
%token LEX_INMAP LEX_INMAP1
%token LEX_INT
%token LEX_INV 
%token LEX_EQ
%token LEX_ORD
%token LEX_INVERSE LEX_IN_SET LEX_IOTA LEX_IN_SEQ
%token LEX_IS_ LEX_IS_DEFINED_AS
%token LEX_LEN LEX_LESS_THAN LEX_LESS_THAN_OR_EQUAL LEX_LET LEX_LOGICAL_EQUIVALENCE
%token LEX_MEASURE
%token LEX_MAP LEX_MAP1 LEX_MAP_DOMAIN_RESTRICT_BY LEX_MAP_DOMAIN_RESTRICT_TO
%token LEX_MAP_MERGE LEX_MAP_RANGE_RESTRICT_BY LEX_MAP_RANGE_RESTRICT_TO
%token LEX_MK_ LEX_MOD LEX_MODIFY_BY
%token LEX_MU
%token LEX_NAT LEX_NATONE
%token LEX_NIL
%token LEX_NARROW_
%token LEX_NOT LEX_NOT_EQUAL LEX_NOT_IN_SET
%token LEX_OF LEX_OR LEX_OTHERS LEX_DOTHASH
%token LEX_TARROW LEX_POST LEX_POWER LEX_PRE LEX_PRECONDAPPLY LEX_PRIME LEX_PROPER_SUBSET
%token LEX_RAISED_DOT LEX_RANGE_OVER LEX_RAT LEX_RD LEX_REAL LEX_REM LEX_RETURN LEX_REVERSE LEX_RNG
%token LEX_SEQUENCE_CONCATENATE LEX_SET_INTERSECTION
%token LEX_SET_MINUS LEX_SET_UNION LEX_SKIP
%token LEX_DUMMY_PRECEDENCE_SEQ1_OF LEX_DUMMY_PRECEDENCE_SEQ_OF LEX_DUMMY_PRECEDENCE_SET_OF LEX_DUMMY_PRECEDENCE_SET1_OF
%token LEX_SET LEX_SET1 LEX_SEQ LEX_SEQ1
%token LEX_ST
%token LEX_SUBSET
%token LEX_THEN LEX_TL LEX_TO LEX_TOKEN
%token LEX_TYPES
%token LEX_UNDEFINED
%token LEX_VALUES
%token LEX_WHILE
%token LEX_WR
%token <LexStringP> LEX_identifier
%token <LexStringP> LEX_dollar_identifier
%token <LexStringP> LEX_char_lit
%token <LexStringP> LEX_bool_true LEX_bool_false
%token <LexStringP> LEX_quote_lit
%token <LexStringP> LEX_text_lit
%token <LexStringP> LEX_real_lit
%token <LexStringP> LEX_int_lit
%token LEX_TEXBREAK LEX_COMMENT
%token LEX_FILE_MACRO LEX_LINE_MACRO LEX_COLUMN_MACRO LEX_CLMOD_MACRO LEX_FNOP_MACRO

%token LEX_LAST_RESULT

#ifdef FULL
%start Document
%type <sequence> Document
#elif VPPSTMT
%start CreateInstance
%type <record> CreateInstance
#else // VDMSL || !(VDMPP && VPPSTMT)
%start Arguments
%type <sequence> Arguments
#endif // !FULL

//************** Stuff related to Class Descriptions ******************/

#ifdef FULL
#ifdef VDMPP
%type <sequence> Classes
%type <Class_p> Class
%type <sequence> OptInheritanceClause
%type <sequence> InstanceVariableDefinitions
%type <sequence> InstanceVariableDefinitionList
%type <record>   InstanceVariableDefinition
%type <record>   InvariantDefinition
%type <record>   InstAssignmentDefinition
#endif //VDMPP

%type <generic>  UseSignature // record  // Used by VDMSL & VDMPP

#ifdef VDMSL
%type <sequence> Modules
%type <record>   Module

%type <record>   DLModule DLInterface DLExportDefinition
%type <map>      DLImportDefinitionList DLListOfImportDefinitions
%type <record>   DLListOfExportModuleSignatures
%type <record>   DLListOfImportModuleSignatures
%type <sequence> DLListOfTypeImports DLImportTypeSignatures
%type <tuple>    DLTypeImport

%type <record>   Interface
%type <generic>  ExportDefinition  // optional - used to be Dummy
%type <generic>  ExportModuleSignature  // optional - used to be Dummy
%type <generic>  ImportModuleSignature  // optional - used to be Dummy
%type <record>   ListOfExportModuleSignatures
%type <record>   ListOfImportModuleSignatures
%type <tuple>    TypeExport
%type <sequence> ImportTypeSignatures
%type <sequence> ListOfTypeImports
%type <tuple>    TypeImport
%type <sequence> ImportValueSignatures
%type <sequence> ListOfValueImports
%type <tuple>    ValueImport
%type <sequence> ImportFunctionSignatures
%type <sequence> ListOfFunctionImports
%type <tuple>    FunctionImport
%type <sequence> ImportOperationSignatures
%type <sequence> ListOfOperationImports
%type <tuple>    OperationImport

%type <map>   ListOfImportDefinitions
%type <map>   ImportDefinitionList
%type <map>   ExportTypeSignatures
%type <map>   ListOfTypeExports
%type <map>   ValueSignatures
%type <map>   ListOfValueDescriptions
%type <map>   FunctionSignatures ExportFunctionSignatures
%type <map>   ListOfFunctionSignatures ListOfFunctionExports
%type <map>   OperationSignatures
%type <map>   ListOfOperationSignatures
#endif // VDMSL
#endif // FULL

//************** Stuff related to DEFINITION(BLOCK)S ******************/
#ifdef FULL
%type <map>           OperationDefinitions
%type <sequence>      OpDomType
%type <record>        OpRngType

#if VDMSL
%type <Definitions_p> Definitions
%type <record>        PureDefinitions
%type <Definitions_p> DefinitionBlock
%type <StateDef_p>    StateDefinition
%type <record>        Initialization
%type <tuple>         InvariantInitialization
#endif // VDMSL
#if VDMPP
%type <Definitions_p> OptDefinitionBlock
%type <record>        AccessValueDefinition
%type <inttype>       Access
%type <tuple>         StaticAccess
%type <tuple>         OperationAccess
#endif // VDMPP

%type <Definitions_p> DefinitionBlockInit
%type <Definitions_p> DefinitionBlockAlternatives

%type <tuple>    FunctionDefinition
%type <tuple>    ExplFunctionDefinition
%type <tuple>    ImplFunctionDefinition

%type <tuple>    ExtExplFunctionDefinition
%type <tuple>    OperationDefinition
%type <tuple>    ImplOperationDefinition
%type <tuple>    ExplOperationDefinition
%type <tuple>    ExtExplOperationDefinition
%type <record>   OperationType

%type <sequence> Externals
%type <sequence> ListOfVarInformation
%type <record>   VarInformation
%type <inttype>  Mode
%type <sequence> NameList
%type <sequence> Exceptions
%type <sequence> ListOfErrors

%type <tuple>    TypeDefinition
%type <tuple>    SimpleTypeDefinition
%type <tuple>    CompositeTypeDefinition
%type <record>   Invariant
%type <record>   Equal
%type <record>   Order
%type <tuple>    EqualOrder
%type <tuple>    InvariantEqualOrder

%type <sequence> ValueDefinitions
%type <sequence> ListOfValueDefinitions
%type <map>      TypeDefinitions

%type <map>      FunctionDefinitions
%type <map>      ListOfFunctionDefinitions
%type <map>      ListOfOperationDefinitions
%type <map>      ListOfTypeDefinitions

#ifdef VDMPP
// VDMPP stuff
%type <sequence> SynchronizationDefinitions  DeclarativeSyncList
%type <record>   DeclarativeSync
%type <record>   PermissionStmt MutexStmt
%type <record>   PermissionExpression
%type <generic>  ThreadDefinition
%type <record>   PeriodicObligation
%type <record>   SporadicObligation
#endif // VDMPP

// Traces stuff
%type <map>      TracesDefinitions
%type <map>      NamedTrace
%type <map>      ListOfNamedTrace
%type <sequence> IdentifierSlashList
%type <sequence> TraceDefinitionList
%type <record>   TraceDefinitionTerm
%type <record>   TraceDefinition
%type <record>   SimpleTraceDefinition
%type <record>   TraceBind
%type <sequence> ListOfTraceBinding
%type <generic>  TraceRepeatPattern
#endif // FULL

//************** Stuff related to TYPES  ***********************/
%type <record>   Type
%type <record>   BracketedType
%type <record>   OptionalType
%type <record>   SetType
%type <record>   Set0Type
%type <record>   Set1Type
%type <record>   SeqType
%type <record>   Seq0Type
%type <record>   Seq1Type
%type <record>   TypeVariable
%type <record>   TypeName
%type <record>   Quote
%type <record>   NumericLiteral
%type <record>   BasicType
%type <record>   ProductType
%type <record>   UnionType
%type <record>   MapType
%type <record>   GeneralMap0Type
%type <record>   GeneralMap1Type
%type <record>   InjectiveMap0Type
%type <record>   InjectiveMap1Type
%type <record>   CompositeType
%type <sequence> NFieldList
%type <sequence> FieldList
%type <record>   Field
%type <record>   FunctionType
%type <record>   TFunctionType
%type <record>   PFunctionType
#if FULL
%type <record>   AllType
#endif // FULL

//************** Stuff related to EXPRESSIONS ***********************/
%type <sequence> ExpressionList
%type <sequence> NonEmptyExpressionList
%type <record>   Expression
%type <record>   BracketedExpression
%type <record>   ComplexExpression
%type <record>   DefExpression
%type <sequence> ListOfDefPatternBinds
%type <tuple>    DefPatternBind
%type <record>   LetOrLetBeExpression
%type <sequence> ListOfLocalDefinitions
%type <record>   IfExpression
%type <sequence> ListOfElsifExpressions
%type <record>   CasesExpression
%type <sequence> CasesExpressionAlternatives
%type <record>   CasesExpressionAlternative
%type <generic>  OthersExpression
%type <record>   UnaryExpression
%type <record>   BinaryExpression
%type <record>   Predication
%type <generic>  OptPredication
%type <record>   QuantifiedExpression
%type <record>   AllOrExistsExpression
%type <record>   ExistsUniqueExpression
%type <record>   IotaExpression
%type <record>   UndefinedExpression
%type <record>   LambdaExpression
%type <sequence> TypeBindList
%type <record>   FunctionTypeInstantiation
%type <sequence> TypeList
%type <record>   SetExpression
%type <record>   SetEnumeration
%type <record>   SetComprehension
%type <record>   SetRangeExpression
%type <record>   SequenceExpression
%type <record>   SequenceEnumeration
%type <record>   SequenceComprehension
%type <record>   SequenceModifier
%type <record>   SubSequence
%type <record>   MapExpression
%type <record>   MapEnumeration
%type <sequence> ListOfMaplets
%type <record>   Maplet
%type <record>   MapComprehension
%type <record>   TupleExpression
%type <record>   TupleConstructor
%type <record>   PreCondApply
%type <record>   RecordExpression
%type <record>   RecordModifier
%type <sequence> ListOfRecordModifications
%type <record>   RecordModification
%type <record>   RecordConstructor
%type <record>   TokenExpression
%type <record>   TokenConstructor
%type <record>   ApplyExpression
%type <record>   Apply
%type <record>   SimpleExpression
%type <record>   FieldSelect

#if VDMPP
%type <record>   SelfExpression
%type <record>   ThreadIdExpression
%type <record>   NewExpression
%type <record>   SameClassExpression
%type <record>   SameBaseClassExpression
%type <record>   IsOfClassExpression
%type <record>   IsOfBaseClassExpression
#ifdef VICE
%type <record>   CurTimeExpression
#endif // VICE
#endif // VDMPP

%type <record>   IsExpression
%type <record>   NarrowExpression
%type <record>   MacroExpression
%type <record>   Name
%type <record>   OldNameOrName
%type <record>   IdentifierPrimeList
%type <record>   Identifier
%type <record>   FnOpNameIdentifier
#ifdef VDMPP
%type <record>   KeywordIdentifier
#endif // VDMPP
%type <record>   SymbolicLiteral

#if FULL && VDMPP
%type <sequence> IdentifierCommaList
#endif //FULL

#if !FULL
%type <record> LastResult
#endif //FULL

//************** Stuff related to STATEMENTS ***********************/
#if FULL
%type <record>   Statement
%type <record>   ReturnStatement
%type <record>   IdentityStatement
%type <record>   AlwaysStatement
%type <record>   TrapStatement
%type <record>   RecursiveTrapStatement
%type <sequence> Traps
%type <record>   ExitStatement
%type <sequence> DclStatement
%type <sequence> ListOfPatternBindStatements
%type <sequence> DclStatementList
%type <sequence> AssignmentDefinitionList
%type <record>   PatternBindStatement
%type <record>   AssignmentDefinition
%type <record>   BlockStatement
%type <record>   AssertStatement
%type <sequence> SequenceOfStatements
%type <record>   ErrorStatement
%type <record>   DefStatement
%type <sequence> ListOfEqualsDefinitions
%type <tuple>    EqualsDefinition
%type <record>   LetOrLetBeStatement
%type <record>   StateDesignator
%type <record>   NameOrNarrowRef
%type <sequence> ListOfStateDesignatorQualifiers
%type <sequence> FieldReference
%type <sequence> MapOrSeqReference
%type <record>   NarrowReference
%type <record>   NondeterministicStatement
%type <sequence> ListOfStatements
%type <record>   CallStatement
%type <record>   AssignStatement
%type <record>   CallOrAssignStatement
%type <sequence> CallOrAssignStatementList
%type <record>   AtomicAssignStatement
%type <record>   SequenceForLoop
%type <record>   SetForLoop
%type <record>   IndexForLoop
%type <record>   WhileLoop
%type <record>   IfStatement
%type <sequence> ListOfElsifStatements
%type <record>   ElseStatement
%type <record>   CasesStatement
%type <sequence> CasesStatementAlternatives
%type <record>   CasesStatementAlternative
%type <generic>   OthersStatement
#ifdef VDMPP
%type <record>   StartListStatement
%type <record>   StartStatement
%type <record>   StopListStatement
%type <record>   StopStatement
#ifdef VICE
%type <record>   DurationStmt
%type <record>   CycleStmt
#endif // VICE
#endif // VDMPP
%type <record>   SpecificationStatement
#endif // FULL

//************** Stuff related to PATTERNS ***********************/
%type <sequence> PatternList
//%type <sequence> PatternCondList
%type <record>   Pattern
%type <record>   PatternIdentifier
%type <record>   MatchValue
%type <record>   SetPattern
%type <record>   SetEnumPattern
%type <record>   SetUnionPattern
%type <record>   SequencePattern
%type <record>   SeqEnumPattern
%type <record>   SeqConcPattern
%type <record>   MapPattern
%type <record>   MapEnumPattern
%type <record>   MapMergePattern
%type <record>   TuplePattern
%type <record>   RecordPattern
%type <sequence> ListOfMapletPatterns
%type <record>   MapletPattern
#ifdef VDMPP
%type <record>   ObjectPattern
%type <sequence> FieldPatterns
%type <sequence> ListOfFieldPattern
#endif // VDMPP

//************** Stuff related to BINDINGS ***********************/
%type <sequence> BindList
%type <record>   Bind
%type <record>   TypeBind
%type <record>   SetBind
%type <record>   SeqBind
%type <record>   SetOrSeqBind
%type <record>   PatternBind
%type <record>   MultipleBind
%type <record>   MultipleTypeBind
%type <record>   MultipleSetBind

//************** Stuff related to LocalDef  ***********************/
%type <record>   LocalDefinition
%type <record>   ValueDefinition
%type <record>   LocalImplFunctionDefinition
//%type <record>   LocalExplFunctionDefinition
//%type <record>   LocalExtExplFunctionDefinition

%type <sequence> TypeVarList
%type <sequence> ParametersList
#ifdef FULL
%type <sequence> IdentifierTypePairList
#endif // FULL
%type <sequence> NonEmptyIdentifierTypePairList

%type <sequence> ListOfTypeVar

#ifdef FULL
%type <record>   OpBody
#endif // FULL
%type <record>   FnBody

%type <sequence> Parameters
%type <sequence> ListOfPatternTypePair
%type <sequence> ParameterTypes
%type <tuple>    PrePost
%type <generic>  Pre
%type <generic>  Post

#if FULL
%type <generic>  Measure
#endif // FULL

//************** Misc *********************************************/
#if FULL
%type <vdm_bool> ModTexBreak
%type <vdm_bool> TexBreak
%type <vdm_bool> OptSemi_TexBreak
#endif // FULL

//** Section in reference document: 9.11 Operator Precedence *******************/
//** Deviations from the standard/Remarks:                                    **/
//** - Precedence rules have been added for a number of symbols which have    **/
//**   not (yet) been given a precedence in the reference document            **/
//** - %left LEVEL0 is introduced to deal with the overloaded '*' in          **/
//**   carthesian product types.                                              **/
//** - The operators LEX_REM, LEX_MOD, LEX_INTEGER_DIVISION and '/' have been given **/
/**   precedence and associativity as well (they are not mentioned in the    **/
//**   reference document.                                                    **/

%nonassoc error
//#ifdef VDMSL
//%right LEX_FROM
//#endif // VDMSL
%right LEX_OF LEX_CASES LEX_DEF LEX_IF LEX_LET LEX_RAISED_DOT LEX_ASSIGN LEX_IN LEX_ARROW LEX_TARROW LEX_BAR_ARROW LEX_THEN LEX_ELSE LEX_ELSEIF LEX_BE LEX_OTHERS
//%right LEX_FROM LEX_OF LEX_CASES LEX_DEF LEX_IF LEX_LET LEX_RAISED_DOT LEX_ASSIGN LEX_IN LEX_ARROW LEX_TARROW LEX_BAR_ARROW LEX_THEN LEX_ELSE LEX_ELSEIF LEX_BE LEX_OTHERS
%left '|'
%left LEX_LOGICAL_EQUIVALENCE // connectives(1)
%right LEX_IMPLY   // connectives(2) %right was %left
%left LEX_OR       // connectives(3)
%left LEX_AND      // connectives(4)
%nonassoc LEX_NOT  // connectives(5)
%nonassoc LEX_EQUAL LEX_NOT_EQUAL LEX_IN_SET LEX_NOT_IN_SET LEX_LESS_THAN LEX_LESS_THAN_OR_EQUAL LEX_GREATER_THAN LEX_GREATER_THAN_OR_EQUAL LEX_SUBSET LEX_PROPER_SUBSET   // relations
%nonassoc DUMMY_RELATION // Used for SetBind LEX_IN_SET
%left '+' '-' LEX_SET_MINUS LEX_SET_UNION LEX_SEQUENCE_CONCATENATE LEX_MAP_MERGE LEX_MODIFY_BY // evaluators(1)
//%right ':'
%left '/' '*' LEX_ARITHMETIC_INTEGER_DIVISION LEX_REM LEX_MOD LEX_SET_INTERSECTION   // evaluators(2)
%right LEX_TO
%left LEX_INVERSE // evaluators(3)
%right LEX_DUMMY_PRECEDENCE_SET_OF LEX_DUMMY_PRECEDENCE_SET1_OF LEX_DUMMY_PRECEDENCE_SEQ_OF LEX_DUMMY_PRECEDENCE_SEQ1_OF
%right LEX_MAP_DOMAIN_RESTRICT_TO LEX_MAP_DOMAIN_RESTRICT_BY  // evaluators(4) %right=NONSTD
%left LEX_MAP_RANGE_RESTRICT_TO LEX_MAP_RANGE_RESTRICT_BY     // evaluators(5)
%nonassoc LEX_ABS LEX_FLOOR LEX_CARD LEX_POWER LEX_DUNION LEX_DINTER LEX_DMERGE LEX_HD LEX_TL LEX_LEN LEX_ELEMS LEX_INDS LEX_CONC LEX_DOM LEX_RNG LEX_REVERSE  // evaluators(6)
%right LEX_COMP             // combinators(1)
%right LEX_EXP_OR_ITERATE   // combinators(2)
%right '(' '.' '{' '['     // applicators
%left LEX_DOTHASH
//#if VDMPP
//%left '!'
//#endif //VDMPP
// Removed ',' and ':'
//%right LEX_COMP             // combinators(1)
//%right LEX_EXP_OR_ITERATE   // combinators(2)

%%

#if FULL
//** Section in reference document: 9.1 Document *******************************/
//** Deviations from the standard/Remarks:                                    **/

#if VDMSL
Document
        : PureDefinitions
        { VDMPARSER::asts->ImpAppend (*$1);
          delete $1;
        }
        | Modules
        {
          *VDMPARSER::asts = *$1;
          delete $1;
        }
        ;
#endif //VDMSL
#if VDMPP
Document
        : Classes
        {
          *VDMPARSER::asts = *$1;
          delete $1;
        }
        ;
#endif //VDMPP
#endif //FULL

#if !FULL && !VPPSTMT
//** Top level non terminal symbol when parsing expressions alone **/

Arguments
        : NonEmptyExpressionList
        {
          *VDMEXPRPARSER::exprs = *$1;
          delete $1;
        }
        ;
#endif //!FULL

#if FULL
#if VDMSL
Modules
        : ModTexBreak Module
        { $$ = new Sequence ();
          $$->ImpAppend (*$2);

ORDERLY_ON MYPARSER::AddModOrder($2->GetField(pos_AS_Module_nm), false);

          delete $1;
          delete $2;
        }
        | Modules ModTexBreak Module
        { $1->ImpAppend (*$3);

ORDERLY_ON MYPARSER::AddModOrder($3->GetField(pos_AS_Module_nm), $2->GetValue());

          delete $2;
          delete $3;
        }
        ;

/** Section in reference document: 9.2 Modules ********************************/
/** Deviations from the standard/Remarks: IPTES                              **/

Module : LEX_MODULE Identifier TexBreak Interface Definitions LEX_END Identifier
        { $$ = new TYPE_AS_Module();
          MYPARSER::SetPos2(*$$, @1, @7);

          VDMPARSER::currentClassMod = Nil();

          $$->SetField (pos_AS_Module_nm,   *$2);
          $$->SetField (pos_AS_Module_intf, *$4);
          $$->SetField (pos_AS_Module_defs, *$5);

          if (! (*$2 == *$7))
            MYPARSER::Report (L"Identifiers in module heading must be identical." , @2, @7);

          delete $2;
          delete $3;
          delete $4;
          delete $5;
          delete $7;
        }
        | DLModule
        ;

DLModule
        : LEX_DLMODULE Identifier DLInterface UseSignature LEX_END Identifier
        { $$ = new TYPE_AS_DLModule();
          MYPARSER::SetPos2(*$$, @1, @6);

          VDMPARSER::currentClassMod = Nil();

          $$->SetField (pos_AS_DLModule_nm,      *$2);
          $$->SetField (pos_AS_DLModule_intf,    *$3);
          $$->SetField (pos_AS_DLModule_useslib, *$4);

          if (! (*$2 == *$6))
            MYPARSER::Report (L"Identifiers in dlmodule heading must be identical.", @2, @6);

          delete $2;
          delete $3;
          delete $4;
          delete $6;
        }
        ;
#endif // VDMSL

UseSignature
        : LEX_USELIB LEX_text_lit
        {
          TYPE_AS_TextLit rec;
          MYPARSER::SetPos2(rec, @1, @2);

          wstring error, res;
          Backslashed::convert_backslashed(TBWSTR::string2wstring(*$2), res, error);
          if (error.length() > 0)
            MYPARSER::Report(error.c_str(), @1);

          rec.SetField (pos_AS_TextLit_val, Sequence(res));
          $$ = new Generic(rec);
        }
        ;

#if VDMSL
DLInterface
        : DLImportDefinitionList DLExportDefinition
        { $$ = new TYPE_AS_DLInterface();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_DLInterface_imp, *$1); /* imp  */
          $$->SetField (pos_AS_DLInterface_exp, *$2); /* exp  */

          delete $1;
          delete $2;
        }
        ;

DLImportDefinitionList
        : /* empty */
        {
          $$ = new Map;
        }
        | LEX_IMPORTS DLListOfImportDefinitions
        { $$ = $2;
        }
        ;

DLExportDefinition
        : LEX_EXPORTS DLListOfExportModuleSignatures
        { $$ = $2;
        }
        ;

DLListOfExportModuleSignatures
        : ValueSignatures
        { $$ = new TYPE_AS_DLExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_DLExportSig_val, *$1);
          $$->SetField (pos_AS_DLExportSig_fns, Map ());
          $$->SetField (pos_AS_DLExportSig_ops, Map ());

          delete $1;
        }
        | FunctionSignatures
        { $$ = new TYPE_AS_DLExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_DLExportSig_val, Map ());
          $$->SetField (pos_AS_DLExportSig_fns, *$1);
          $$->SetField (pos_AS_DLExportSig_ops, Map ());

          delete $1;
        }
        | OperationSignatures
        { $$ = new TYPE_AS_DLExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_DLExportSig_val, Map ());
          $$->SetField (pos_AS_DLExportSig_fns, Map ());
          $$->SetField (pos_AS_DLExportSig_ops, *$1);

          delete $1;
        }
        | DLListOfExportModuleSignatures ValueSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_DLExportSig_val));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Value is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_DLExportSig_val, tmp_m);

          delete $2;
        }
        | DLListOfExportModuleSignatures FunctionSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_DLExportSig_fns));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Function is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_DLExportSig_fns, tmp_m);

          delete $2;
        }
        | DLListOfExportModuleSignatures OperationSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_DLExportSig_ops));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Operation is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_DLExportSig_ops, tmp_m);

          delete $2;
        }
        ;

// End of Export
DLListOfImportDefinitions
        : LEX_FROM Identifier DLListOfImportModuleSignatures
        {
          $$ = new Map;
          $$->Insert (*$2, *$3); // was $3->GetField(1)

          delete $2;
          delete $3;
        }
        | DLListOfImportDefinitions ',' LEX_FROM Identifier DLListOfImportModuleSignatures
        {
          if ($1->DomExists (*$4))
            MYPARSER::Report (L"That module is already being imported from.", @4);
          else
            $1->Insert (*$4, *$5); // was $5->GetField(1)

          delete $4;
          delete $5;
        }
        ;

DLListOfImportModuleSignatures
        : DLImportTypeSignatures
        { $$ = new TYPE_AS_DLImportSig();
//          MYPARSER::SetPos2(*$$, @1, @1);

          Map ITS;
          Generic TI;
          for(bool bb = $1->First (TI); bb; bb = $1->Next (TI))
          {
            Generic nmtd (((Tuple) (TI)).GetField (1));

            Record nm;
            Generic tp;
            if (nmtd.IsRecord ())
            { nm = nmtd;
              tp = Nil();
            }
            else /* must be TypeDef */
            { nm = ((Tuple) (nmtd)).GetField (1);
              tp = ((Tuple) (nmtd)).GetField (2);
            }

            if (ITS.DomExists (nm))
              MYPARSER::Report (L"This type name is already imported.", @1); // @1 TBC/etn
            else
              ITS.Insert (nm, tp);
          }
          $$->SetField (pos_AS_DLImportSig_tps, ITS);
          delete $1;
        }
        | DLListOfImportModuleSignatures DLImportTypeSignatures
        {
          Map ITS;
          Generic TI;
          for(bool bb = $2->First (TI); bb; bb = $2->Next (TI))
          {  Generic nmtd (((Tuple) (TI)).GetField (1));

            Record nm;
            Generic tp;

            if (nmtd.IsRecord ())
            { nm = nmtd;
              tp = Nil();
            }
            else /* must be TypeDef */
            { nm = ((Tuple) (nmtd)).GetField (1);
              tp = ((Tuple) (nmtd)).GetField (2);
            }

            if (ITS.DomExists (nm))
              MYPARSER::Report (L"This type name is already imported.", @2);
            else
              ITS.Insert (nm, tp);
          }
          Map tmp_tp ($1->GetMap (pos_AS_DLImportSig_tps));

          if (MYPARSER::DomOverlap (tmp_tp, ITS))
            MYPARSER::Report (L"Type is imported twice", @2);
          else
            tmp_tp.ImpOverride (ITS);

          $1->SetField (pos_AS_DLImportSig_tps, tmp_tp);
          delete $2;
          $$ = $1;
        }

DLImportTypeSignatures
        : LEX_TYPES DLListOfTypeImports OptSemi
        { $$ = $2;
        }
        ;

DLListOfTypeImports
        : DLTypeImport
        { /* MUST be a Sequence for RENAME reasons. MIAN */
          $$ = new Sequence ();
          $$->ImpAppend (*$1);

          delete $1;
        }
        | DLListOfTypeImports ';' DLTypeImport
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

DLTypeImport
        : Name
        { $$ = new Tuple (2);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* renames */

          delete $1;
        }

/** Section in reference document: 9.3 Interfaces *****************************/
/** Deviations from the standard/Remarks: IPTES                              **/

Interface
        : ImportDefinitionList
        { $$ = new TYPE_AS_Interface();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Interface_imp, *$1); /* imp  */
          $$->SetField (pos_AS_Interface_exp,
                 TYPE_AS_ExportSig().Init(Map(),Map(),Map(),Map(),NilContextId)); /* exp  */
          delete $1;
        }
        | ExportDefinition
        { $$ = new TYPE_AS_Interface();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Interface_imp, Map()); /* imp  */
          $$->SetField (pos_AS_Interface_exp, *$1); /* exp  */

          delete $1;
        }
        | ImportDefinitionList ExportDefinition
        { $$ = new TYPE_AS_Interface();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_Interface_imp, *$1); /* imp  */
          $$->SetField (pos_AS_Interface_exp, *$2); /* exp  */

          delete $1;
          delete $2;
        }
        | ExportDefinition ImportDefinitionList
        { $$ = new TYPE_AS_Interface();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_Interface_imp, *$2); /* imp  */
          $$->SetField (pos_AS_Interface_exp, *$1); /* exp  */

          delete $1;
          delete $2;
        }
        ;

ImportDefinitionList
        : LEX_IMPORTS ListOfImportDefinitions
        { $$ = $2;
        }
        ;

ListOfImportDefinitions
        : LEX_FROM Identifier ImportModuleSignature
        {
          $$ = new Map;
          $$->Insert (*$2, *$3);
          delete $2;
          delete $3;
        }
        | ListOfImportDefinitions ',' LEX_FROM Identifier ImportModuleSignature
        {
          if ($1->DomExists (*$4))
            MYPARSER::Report (L"That module is already being imported from.", @4);
          else
            $1->Insert (*$4, *$5);

          delete $4;
          delete $5;
        }
        ;

ImportModuleSignature
        : ListOfImportModuleSignatures
        { $$ = new Generic(*$1);
          delete $1;
        }
        | LEX_ALL
        { $$ = new Generic(Nil ());
        }
        ;

ListOfImportModuleSignatures
        : ImportTypeSignatures
        { $$ = new TYPE_AS_ImportSig();
          MYPARSER::SetPos2(*$$, @1, @1);

          Map ITS, REN;
          Generic TI;
          for(bool bb = $1->First (TI); bb; bb = $1->Next (TI))
          { Generic nmtd (((Tuple) (TI)).GetField (1));
            Generic ren (((Tuple) (TI)).GetField (2));

            Record nm;
            Generic tp;
            if (nmtd.IsRecord ())
            { nm = nmtd;
              tp = Nil();
            }
            else /* must be TypeDef */
            { nm = ((Tuple) (nmtd)).GetField (1);
              tp = ((Tuple) (nmtd)).GetField (2);
            }

            if (ITS.DomExists (nm))
              MYPARSER::Report (L"This type name is already imported.", @1); // TBF
            else
              ITS.Insert (nm, tp);

            if (! ren.IsNil ())
            { Record renamed (ren);

              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two types renamed to the same.", @1);
              else
                REN.Insert (renamed, nm);
            }
          }

          $$->SetField (pos_AS_ImportSig_tps, ITS);
          $$->SetField (pos_AS_ImportSig_val, Map ());
          $$->SetField (pos_AS_ImportSig_fns, Map ());
          $$->SetField (pos_AS_ImportSig_ops, Map ());
          $$->SetField (pos_AS_ImportSig_ren, REN);

          delete $1;
        }
        | ImportValueSignatures
        {  $$ = new TYPE_AS_ImportSig();
           MYPARSER::SetPos2(*$$, @1, @1);

           Map IVS, REN;
           Generic VI;
           for(bool bb = $1->First (VI); bb; bb = $1->Next (VI))
           { Record name (((Tuple) (VI)).GetField (1));
             Generic typ (((Tuple) (VI)).GetField (2));
             Generic ren (((Tuple) (VI)).GetField (3));

             if (IVS.DomExists (name))
               MYPARSER::Report (L"This value name is already imported.", @1);
             else
               IVS.Insert (name, typ);

             if (! ren.IsNil ())
             { Record renamed (ren);
               if (REN.DomExists (renamed))
                 MYPARSER::Report (L"Two values renamed to the same.", @1);
               else
                 REN.Insert (renamed, name);
             }
           }

           $$->SetField (pos_AS_ImportSig_tps, Map ());
           $$->SetField (pos_AS_ImportSig_val, IVS);
           $$->SetField (pos_AS_ImportSig_fns, Map ());
           $$->SetField (pos_AS_ImportSig_ops, Map ());
           $$->SetField (pos_AS_ImportSig_ren, REN);

           delete $1;
        }
        | ImportFunctionSignatures
        { $$ = new TYPE_AS_ImportSig();
          MYPARSER::SetPos2(*$$, @1, @1);

          Map IFS, REN;
          Generic FI;
          for(bool bb = $1->First (FI); bb; bb = $1->Next (FI))
          { Record name (((Tuple) (FI)).GetField (1));
            Generic typ (((Tuple) (FI)).GetField (2));
            Generic ren (((Tuple) (FI)).GetField (3));

            if (IFS.DomExists (name))
              MYPARSER::Report (L"This function name is already imported.", @1);
            else
              IFS.Insert (name, typ);

            if (! ren.IsNil ())
            { Record renamed (ren);
              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two functions renamed to the same.", @1);
              else
                REN.Insert (renamed, name);
            }
          }

          $$->SetField (pos_AS_ImportSig_tps, Map ());
          $$->SetField (pos_AS_ImportSig_val, Map ());
          $$->SetField (pos_AS_ImportSig_fns, IFS);
          $$->SetField (pos_AS_ImportSig_ops, Map ());
          $$->SetField (pos_AS_ImportSig_ren, REN);

          delete $1;
        }
        | ImportOperationSignatures
        { $$ = new TYPE_AS_ImportSig();
          MYPARSER::SetPos2(*$$, @1, @1);

          Map IOS, REN;
          Generic OI;
          for(bool bb = $1->First (OI); bb; bb = $1->Next (OI))
          { Record name (((Tuple) (OI)).GetField (1));
            Generic typ (((Tuple) (OI)).GetField (2));
            Generic ren (((Tuple) (OI)).GetField (3));

            if (IOS.DomExists (name))
              MYPARSER::Report (L"This operation name is already imported.", @1);
            else
              IOS.Insert (name, typ);

            if (! ren.IsNil ())
            { Record renamed (ren);
              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two operations renamed to the same.", @1);
              else
                REN.Insert (renamed, name);
            }
          }

          $$->SetField (pos_AS_ImportSig_tps, Map ());
          $$->SetField (pos_AS_ImportSig_val, Map ());
          $$->SetField (pos_AS_ImportSig_fns, Map ());
          $$->SetField (pos_AS_ImportSig_ops, IOS);
          $$->SetField (pos_AS_ImportSig_ren, REN);

          delete $1;
        }
        | ListOfImportModuleSignatures ImportTypeSignatures
        {
          Map ITS, REN;
          Generic TI;
          for(bool bb = $2->First (TI); bb; bb = $2->Next (TI))
          { Generic nmtd (((Tuple) (TI)).GetField (1));
            Generic ren (((Tuple) (TI)).GetField (2));

            Record nm;
            Generic tp;

            if (nmtd.IsRecord ())
            { nm = nmtd;
              tp = Nil();
            }
            else /* must be TypeDef */
            { nm = ((Tuple) (nmtd)).GetField (1);
              tp = ((Tuple) (nmtd)).GetField (2);
            }

            if (ITS.DomExists (nm))
              MYPARSER::Report (L"This type name is already imported.", @2);
            else
              ITS.Insert (nm, tp);

            if (! ren.IsNil ())
            { Record renamed (ren);
              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two types renamed to the same.", @2);
              else
                REN.Insert (renamed, nm);
            }
          }

          Map tmp_tp ($1->GetMap (pos_AS_ImportSig_tps));
          Map tmp_ren ($1->GetMap (pos_AS_ImportSig_ren));

          if (MYPARSER::DomOverlap (tmp_tp, ITS))
            MYPARSER::Report (L"Type is imported twice", @2);
          else
            tmp_tp.ImpOverride (ITS);

          if (MYPARSER::DomOverlap (tmp_ren, REN))
            MYPARSER::Report (L"More imports are renamed to the same.", @2);
          else
            tmp_ren.ImpOverride (REN);

          $1->SetField (pos_AS_ImportSig_tps, tmp_tp);
          $1->SetField (pos_AS_ImportSig_ren, tmp_ren);

          delete $2;
        }
        | ListOfImportModuleSignatures ImportValueSignatures
        {
          Map IVS, REN;
          Generic VI;
          for(bool bb = $2->First (VI); bb; bb = $2->Next (VI))
          { Record name (((Tuple) (VI)).GetField (1));
            Generic typ (((Tuple) (VI)).GetField (2));
            Generic ren (((Tuple) (VI)).GetField (3));

            if (IVS.DomExists (name))
              MYPARSER::Report (L"This value name is already imported.", @2);
            else
              IVS.Insert (name, typ);

            if (! ren.IsNil ())
            { Record renamed (ren);
              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two values renamed to the same.", @2);
              else
                REN.Insert (renamed, name);
            }
          }

          Map tmp_val ($1->GetMap (pos_AS_ImportSig_val));
          Map tmp_ren ($1->GetMap (pos_AS_ImportSig_ren));

          if (MYPARSER::DomOverlap (tmp_val, IVS))
            MYPARSER::Report (L"Value is imported twice", @2);
          else
            tmp_val.ImpOverride (IVS);

          if (MYPARSER::DomOverlap (tmp_ren, REN))
            MYPARSER::Report (L"More imports are renamed to the same.", @2);
          else
            tmp_ren.ImpOverride (REN);

          $1->SetField (pos_AS_ImportSig_val, tmp_val);
          $1->SetField (pos_AS_ImportSig_ren, tmp_ren);

          delete $2;
        }
        | ListOfImportModuleSignatures ImportFunctionSignatures
        {
          Map IFS, REN;
          Generic FI;
          for(bool bb = $2->First (FI); bb; bb = $2->Next (FI))
          { Record name (((Tuple) (FI)).GetField (1));
            Generic typ (((Tuple) (FI)).GetField (2));
            Generic ren (((Tuple) (FI)).GetField (3));

            if (IFS.DomExists (name))
              MYPARSER::Report (L"This function name is already imported.", @2);
            else
              IFS.Insert (name, typ);

            if (! ren.IsNil ())
            { Record renamed (ren);
              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two functions renamed to the same.", @2);
              else
                REN.Insert (renamed, name);
            }
          }

          Map tmp_fns ($1->GetMap (pos_AS_ImportSig_fns));
          Map tmp_ren ($1->GetMap (pos_AS_ImportSig_ren));

          if (MYPARSER::DomOverlap (tmp_fns, IFS))
            MYPARSER::Report (L"Function is imported twice", @2);
          else
            tmp_fns.ImpOverride (IFS);

          if (MYPARSER::DomOverlap (tmp_ren, REN))
            MYPARSER::Report (L"More imports are renamed to the same.", @2);
          else
            tmp_ren.ImpOverride (REN);

          $1->SetField (pos_AS_ImportSig_fns, tmp_fns);
          $1->SetField (pos_AS_ImportSig_ren, tmp_ren);

          delete $2;
        }
        | ListOfImportModuleSignatures ImportOperationSignatures
        {
          Map IOS, REN;
          Generic OI;
          for(bool bb = $2->First (OI); bb; bb = $2->Next (OI))
          { Tuple t (OI);
            const TYPE_AS_Name & name (t.GetRecord (1));
            const Generic & typ (t.GetField (2));
            const Generic & ren (t.GetField (3));

            if (IOS.DomExists (name))
              MYPARSER::Report (L"This operation name is already imported.", @2);
            else
              IOS.Insert (name, typ);

            if (! ren.IsNil ())
            { TYPE_AS_Name renamed (ren);
              //renamed.SetField (1, Record (ren).GetField (1));

              if (REN.DomExists (renamed))
                MYPARSER::Report (L"Two operations renamed to the same.", @2);
              else
                REN.Insert (renamed, name);
            }
          }

          Map tmp_ops ($1->GetMap (pos_AS_ImportSig_ops));
          Map tmp_ren ($1->GetMap (pos_AS_ImportSig_ren));

          if (MYPARSER::DomOverlap (tmp_ops, IOS))
            MYPARSER::Report (L"Operation is imported twice", @2);
          else
            tmp_ops.ImpOverride (IOS);

          if (MYPARSER::DomOverlap (tmp_ren, REN))
            MYPARSER::Report (L"More imports are renamed to the same.", @2);
          else
            tmp_ren.ImpOverride (REN);

          $1->SetField (pos_AS_ImportSig_ops, tmp_ops);
          $1->SetField (pos_AS_ImportSig_ren, tmp_ren);

          delete $2;
        }
        ;

ImportTypeSignatures
        : LEX_TYPES ListOfTypeImports OptSemi
        {  $$ = $2;
        }
        ;

ListOfTypeImports
        : TypeImport
        { /* MUST be a Sequence for RENAME reasons. MIAN */
          $$ = new Sequence ();
          $$->ImpAppend (*$1);

          delete $1;
        }
        | ListOfTypeImports ';' TypeImport
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

TypeImport
        : Name
        { $$ = new Tuple (2);
          $$->SetField (1, *$1); /* name | typedef */
          $$->SetField (2, Nil ()); /* renames */
          delete $1;
        }
        | Name LEX_RENAMED Name
        { $$ = new Tuple (2);
          $$->SetField (1, *$1); /* name | typedef*/
          $$->SetField (2, *$3); /* renames */
          delete $1;
          delete $3;
        }
        | TypeDefinition
        { $$ = new Tuple (2);
          $$->SetField (1, *$1); /* name | typedef */
          $$->SetField (2, Nil ()); /* renames */
          delete $1;
        }
        | TypeDefinition LEX_RENAMED Name
        { $$ = new Tuple (2);
          $$->SetField (1, *$1); /* name | typedef */
          $$->SetField (2, *$3); /* renames */
          delete $1;
          delete $3;
        }
        ;

ImportValueSignatures
        : LEX_VALUES ListOfValueImports OptSemi
        { $$ = $2;
        }
        ;

ListOfValueImports
        : ValueImport
        {  /* MUST be a Sequence for RENAME reasons. MIAN */
          $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfValueImports ';' ValueImport
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

ValueImport
        : Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* type */
          $$->SetField (3, Nil ()); /* renames */
          delete $1;
        }
        | Name ':' Type
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, *$3); /* type */
          $$->SetField (3, Nil ()); /* renames */
          delete $1;
          delete $3;
        }
        | Name LEX_RENAMED Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* type */
          $$->SetField (3, *$3); /* renames */
          delete $1;
          delete $3;
        }
        | Name ':' Type LEX_RENAMED Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, *$3); /* type */
          $$->SetField (3, *$5); /* renames */
          delete $1;
          delete $3;
          delete $5;
        }
        ;

ImportFunctionSignatures
        : LEX_FUNCTIONS ListOfFunctionImports OptSemi
        { $$ = $2;
        }
        ;

ListOfFunctionImports
        : FunctionImport
        { /* MUST be a Sequence for RENAME reasons. MIAN */
          $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfFunctionImports ';' FunctionImport
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

FunctionImport
        : Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* type */
          $$->SetField (3, Nil ()); /* renames */
          delete $1;
        }
        | Name TypeVarList ':' FunctionType
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, mk_(*$2,*$4)); /* type */
          $$->SetField (3, Nil ()); /* renames */
          delete $1;
          delete $2;
          delete $4;
        }
        | Name LEX_RENAMED Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* type */
          $$->SetField (3, *$3); /* renames */
          delete $1;
          delete $3;
        }
        | Name TypeVarList ':' FunctionType LEX_RENAMED Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, mk_(*$2,*$4)); /* type */
          $$->SetField (3, *$6); /* renames */
          delete $1;
          delete $2;
          delete $4;
          delete $6;
        }
        ;

ImportOperationSignatures
        : LEX_OPERATIONS ListOfOperationImports OptSemi
        { $$ = $2;
        }
        ;

ListOfOperationImports
        : OperationImport
        {  /* MUST be a Sequence for RENAME reasons. MIAN */
          $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfOperationImports ';' OperationImport
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

OperationImport
        : Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* type */
          $$->SetField (3, Nil ()); /* renames */
          delete $1;
        }
        | Name ':' OperationType
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, *$3); /* type */
          $$->SetField (3, Nil ()); /* renames */
          delete $1;
          delete $3;
        }
        | Name LEX_RENAMED Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, Nil ()); /* type */
          $$->SetField (3, *$3); /* renames */
          delete $1;
          delete $3;
        }
        | Name ':' OperationType LEX_RENAMED Name
        { $$ = new Tuple (3);
          $$->SetField (1, *$1); /* name */
          $$->SetField (2, *$3); /* type */
          $$->SetField (3, *$5); /* renames */
          delete $1;
          delete $3;
          delete $5;
        }
        ;

ExportDefinition
        : LEX_EXPORTS ExportModuleSignature
        { $$ = $2;  // $2 is already a Generic*
        }
        ;

ExportModuleSignature
        : ListOfExportModuleSignatures
        { $$ = new Generic(*$1);
          delete $1;
        }
        | LEX_ALL
        { $$ = new Generic(Nil ());
        }
        ;

ListOfExportModuleSignatures
        : ExportTypeSignatures
        { $$ = new TYPE_AS_ExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_ExportSig_tps, *$1);
          $$->SetField (pos_AS_ExportSig_val, Map ());
          $$->SetField (pos_AS_ExportSig_fns, Map ());
          $$->SetField (pos_AS_ExportSig_ops, Map ());
          delete $1;
        }
        | ValueSignatures
        { $$ = new TYPE_AS_ExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_ExportSig_tps, Map ());
          $$->SetField (pos_AS_ExportSig_val, *$1);
          $$->SetField (pos_AS_ExportSig_fns, Map ());
          $$->SetField (pos_AS_ExportSig_ops, Map ());
          delete $1;
        }
        | ExportFunctionSignatures
        { $$ = new TYPE_AS_ExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_ExportSig_tps, Map ());
          $$->SetField (pos_AS_ExportSig_val, Map ());
          $$->SetField (pos_AS_ExportSig_fns, *$1);
          $$->SetField (pos_AS_ExportSig_ops, Map ());
          delete $1;
        }
        | OperationSignatures
        { $$ = new TYPE_AS_ExportSig();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_ExportSig_tps, Map ());
          $$->SetField (pos_AS_ExportSig_val, Map ());
          $$->SetField (pos_AS_ExportSig_fns, Map ());
          $$->SetField (pos_AS_ExportSig_ops, *$1);
          delete $1;
        }
        | ListOfExportModuleSignatures ExportTypeSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_ExportSig_tps));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Type is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_ExportSig_tps, tmp_m);

          delete $2;
        }
        | ListOfExportModuleSignatures ValueSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_ExportSig_val));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Value is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_ExportSig_val, tmp_m);

          delete $2;
        }
        | ListOfExportModuleSignatures ExportFunctionSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_ExportSig_fns));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Function is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_ExportSig_fns, tmp_m);

          delete $2;
        }
        | ListOfExportModuleSignatures OperationSignatures
        {
          Map tmp_m ($1->GetMap (pos_AS_ExportSig_ops));
          if (MYPARSER::DomOverlap (tmp_m, *$2))
            MYPARSER::Report (L"Operation is declared twice", @2);
          else
            tmp_m.ImpOverride (*$2);
          $1->SetField (pos_AS_ExportSig_ops, tmp_m);

          delete $2;
        }
        ;

ExportTypeSignatures
        : LEX_TYPES ListOfTypeExports OptSemi
        { $$ = $2;
        }
        ;

ListOfTypeExports
        : TypeExport
        { $$ = new Map();
          $$->Insert ($1->GetField (1), $1->GetField (2));

          delete $1;
        }
        | ListOfTypeExports ';' TypeExport
        { if ($1->DomExists ($3->GetField (1)))
            MYPARSER::Report (L"Type already exported.", @3);
          else
            $1->Insert ($3->GetField (1), $3->GetField (2));

          delete $3;
        }
        ;

TypeExport
        : Name
        { $$ = new Tuple (2);
          $$->SetField (1, *$1);
          $$->SetField (2, Nil ());
          delete $1;
        }
        | LEX_STRUCT Name
        { $$ = new Tuple (2);
          $$->SetField (1, *$2);
          $$->SetField (2, (Int)STRUCT);
          delete $2;
        }
        ;

ValueSignatures
        : LEX_VALUES ListOfValueDescriptions OptSemi
        { $$ = $2;
        }
        ;

ListOfValueDescriptions
        : NameList ':' Type
        {
          $$ = new Map;
          Generic name;
          for(bool bb = $1->First (name); bb; bb = $1->Next (name))
          { if ($$->DomExists (name))
              MYPARSER::Report (L"Value already described", @2);
            else
              $$->Insert (name, *$3);
          }
          delete $1;
          delete $3;
        }
        | ListOfValueDescriptions ';' NameList ':' Type
        { Generic name;
          for(bool bb = $3->First (name); bb; bb = $3->Next (name))
          { if ($1->DomExists (name))
              MYPARSER::Report (L"Value already described", @4);
            else
              $1->Insert (name, *$5);
          }
          delete $3;
          delete $5;
        }
        ;

FunctionSignatures
        : LEX_FUNCTIONS ListOfFunctionSignatures OptSemi
        { $$ = $2;
        }
        ;

ExportFunctionSignatures
        : LEX_FUNCTIONS ListOfFunctionExports OptSemi
        { $$ = $2;
        }
        ;

ListOfFunctionSignatures
        : NameList ':' FunctionType
        { $$ = new Map;
          Generic name;
          for(bool bb = $1->First (name); bb; bb = $1->Next (name))
          { if ($$->DomExists (name))
              MYPARSER::Report (L"Function already described", @2);
            else
              $$->Insert (name, *$3);
          }
          delete $1;
          delete $3;
        }
        | ListOfFunctionSignatures ';' NameList ':' FunctionType
        { Generic name;
          for(bool bb = $3->First (name); bb; bb = $3->Next (name))
          { if ($1->DomExists (name))
              MYPARSER::Report (L"Function already described", @4);
            else
              $1->Insert (name, *$5);
          }
          delete $3;
          delete $5;
        }
        ;

ListOfFunctionExports
        : NameList TypeVarList ':' FunctionType
        { $$ = new Map;
          Generic name;
          for(bool bb = $1->First (name); bb; bb = $1->Next (name))
          { if ($$->DomExists (name))
              MYPARSER::Report (L"Function already described", @2);
            else
              $$->Insert (name, mk_(*$2,*$4));
          }
          delete $1;
          delete $2;
          delete $4;
        }
        | ListOfFunctionExports ';' NameList TypeVarList ':' FunctionType
        { Generic name;
          for(bool bb = $3->First (name); bb; bb = $3->Next (name))
          { if ($1->DomExists (name))
              MYPARSER::Report (L"Function already described", @4);
            else
              $1->Insert (name, mk_(*$4,*$6));
          }
          delete $3;
          delete $4;
          delete $6;
        }
        ;

OperationSignatures
        : LEX_OPERATIONS ListOfOperationSignatures OptSemi
        { $$ = $2;
        }
        ;

ListOfOperationSignatures
        : NameList ':' OperationType
        { $$ = new Map;
          Generic name;
          for(bool bb = $1->First (name); bb; bb = $1->Next (name))
          { if ($$->DomExists (name))
              MYPARSER::Report (L"Operation already described", @2);
            else
              $$->Insert (name, *$3);
          }
          delete $1;
          delete $3;
        }
        | ListOfOperationSignatures ';' NameList ':' OperationType
        { Generic name;
          for(bool bb = $3->First (name); bb; bb = $3->Next (name))
          { if ($1->DomExists (name))
              MYPARSER::Report (L"Operation already described", @4);
            else
              $1->Insert (name, *$5);
          }
          delete $3;
          delete $5;
        }
        ;

/** Section in reference document: 9.4 Definitions ****************************/
/** Deviations from the standard/Remarks: IPTES                              **/

PureDefinitions
        : TexBreak DefinitionBlock
        { $$ = $2;
          delete $1;
        }
        ;

Definitions
        : LEX_DEFINITIONS TexBreak DefinitionBlock
        {
          $$ = $3;
          delete $2;
        }
        | /* empty */
        {
          $$ = new TYPE_AS_Definitions();
          $$->SetField (pos_AS_Definitions_typem,  Map ());
          $$->SetField (pos_AS_Definitions_valuem, Sequence ());
          $$->SetField (pos_AS_Definitions_fnm,    Map ());
          $$->SetField (pos_AS_Definitions_opm,    Map ());
          $$->SetField (pos_AS_Definitions_State,  Nil ());
          $$->SetField (pos_AS_Definitions_tracem, Map() );
          $$->SetField (pos_AS_Definitions_cid,    NilContextId);
        }
        ;
#endif // VDMSL

#if VDMPP
// Compared to DefinitionBlock the OptDefinitionBlock may be empty.
OptDefinitionBlock
        : DefinitionBlockInit
        {
          $$ = $1;   // Not really needed (default action).
        }
        | OptDefinitionBlock DefinitionBlockAlternatives
        {
           $$ = $1;   // Not really needed (default action).
        }
#endif // VDMPP

#if VDMSL
DefinitionBlock
        : DefinitionBlockInit DefinitionBlockAlternatives
        {
          $$ = $1;   // Not really needed (default action).
        }
        | DefinitionBlock DefinitionBlockAlternatives
        {
          $$ = $1;   // Not really needed (default action).
        }
#endif // VDMSL

DefinitionBlockInit
        : /*empty*/
        {
          $$ = new TYPE_AS_Definitions();
          $$->SetField (pos_AS_Definitions_typem,     Map());
          $$->SetField (pos_AS_Definitions_valuem,    Sequence());
          $$->SetField (pos_AS_Definitions_fnm,       Map());
          $$->SetField (pos_AS_Definitions_opm,       Map());
#if VDMSL
          $$->SetField (pos_AS_Definitions_State,     Nil());
#endif // VDMSL
#if VDMPP
          $$->SetField (pos_AS_Definitions_instvars,  Sequence());
          $$->SetField (pos_AS_Definitions_syncs,     Sequence() );
          $$->SetField (pos_AS_Definitions_threaddef, Nil());
#endif // VDMPP
          $$->SetField (pos_AS_Definitions_tracem,    Map());
          $$->SetField (pos_AS_Definitions_cid,       NilContextId);
        }

DefinitionBlockAlternatives
        : TypeDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Map tmp_m (defs->GetMap(pos_AS_Definitions_typem));

          if (MYPARSER::DomOverlap (tmp_m, *$1))
            MYPARSER::Report (L"Type is defined twice", @1);

          Tuple noid (MYPARSER::NameOverlapInDefs(*defs, *$1, wstring(L"type"), wstring(L"types")));
          if (noid.GetBool(1))
            MYPARSER::Report(noid.GetSequence(2).GetString().c_str(), @1);
          else
            tmp_m.ImpOverride (*$1);

          defs->SetField (pos_AS_Definitions_typem, tmp_m);

          delete $1;
        }
#if VDMSL
        | StateDefinition
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;

          Tuple noisd (MYPARSER::NameOverlapInStateDef(*defs, *$1));
          if (noisd.GetBool(1))
            MYPARSER::Report(noisd.GetSequence(2).GetString().c_str(), @1);
          else if (defs->GetField(pos_AS_Definitions_State).IsNil ())
            defs->SetField (pos_AS_Definitions_State, *$1);
          else
            MYPARSER::Report (L"Only one state definition allowed.", @1);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_StateDef, Nil ());

          delete $1;
        }
#endif //VDMSL
        | ValueDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Sequence tmp_l ((*defs).GetSequence(pos_AS_Definitions_valuem));
          tmp_l.ImpConc (*$1);
          defs->SetField (pos_AS_Definitions_valuem, tmp_l);

          delete $1;
        }
        | FunctionDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Map tmp_m (defs->GetMap(pos_AS_Definitions_fnm));

          if (MYPARSER::DomOverlap (tmp_m, *$1))
            MYPARSER::Report (L"Function is defined twice", @1);

          Tuple noid (MYPARSER::NameOverlapInDefs(*defs, *$1, wstring(L"function"), wstring(L"functions")));
          if (noid.GetBool(1))
            MYPARSER::Report(noid.GetSequence(2).GetString().c_str());
          else
#ifdef VDMSL
            tmp_m.ImpOverride (*$1);
#endif // VDMSL
#ifdef VDMPP
            tmp_m = MYPARSER::MergeOpFnDef(tmp_m, *$1);
#endif // VDMPP

          defs->SetField (pos_AS_Definitions_fnm, tmp_m);

          delete $1;
        }
        | OperationDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Map tmp_m (defs->GetMap(pos_AS_Definitions_opm));

          if (MYPARSER::DomOverlap (tmp_m, *$1))
            MYPARSER::Report (L"Operation is defined twice", @1);

          Tuple noid (MYPARSER::NameOverlapInDefs(*defs, *$1, wstring(L"operation"), wstring(L"operations")));
          if (noid.GetBool(1))
            MYPARSER::Report(noid.GetSequence(2).GetString().c_str(), @1);
          else
#ifdef VDMSL
            tmp_m.ImpOverride (*$1);
#endif // VDMSL
#ifdef VDMPP
            tmp_m = MYPARSER::MergeOpFnDef(tmp_m, *$1);
#endif // VDMPP

          defs->SetField (pos_AS_Definitions_opm, tmp_m);

          delete $1;
        }
#if VDMPP
        | InstanceVariableDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Sequence tmp_s ((*defs).GetSequence(pos_AS_Definitions_instvars));
          tmp_s.ImpConc (*$1);

          Tuple ivdo (MYPARSER::InstVarDomOverlap (tmp_s));
          if (ivdo.GetBool(1))
            MYPARSER::Report (ivdo.GetSequence(2).GetString().c_str(), @1);

          defs->SetField (pos_AS_Definitions_instvars, tmp_s);

          delete $1;
        }
        | SynchronizationDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Sequence tmp_s (defs->GetSequence(pos_AS_Definitions_syncs));

          // if (MYPARSER::DomOverlap (tmp_s, *$1))
          //   MYPARSER::Report (L"Operation is defined twice", @1);
          // Tuple noid (MYPARSER::NameOverlapInDefs(*defs, *$1, wstring(L"synchronization"), wstring(L"synchronizations")))
          // if (noid.GetBool(1))
          //   MYPARSER::Report(noid.GetSequence(2).GetString().c_str(), @1);
          // else

             tmp_s.ImpConc (*$1);
          defs->SetField (pos_AS_Definitions_syncs, tmp_s);

          delete $1;
        }
        | ThreadDefinition
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          if (!defs->GetField(pos_AS_Definitions_threaddef).IsNil())
            MYPARSER::Report (L"Thread already defined", @1);

          defs->SetField (pos_AS_Definitions_threaddef, *$1);
          delete $1;
        }
#endif // VDMPP
        | TracesDefinitions
        {
          TYPE_AS_Definitions * defs = $<Definitions_p>0;
          Map tmp_m (defs->GetMap(pos_AS_Definitions_tracem));
          if (!tmp_m.IsCompatible(*$1))
            MYPARSER::Report (L"Traces already defined", @1);

          tmp_m.ImpOverride (*$1);
          defs->SetField (pos_AS_Definitions_tracem, tmp_m);
          delete $1;
        }
        ;
#endif // FULL

/*************************************************************************/
/** Class Definition                                                    **/
/**                                                                     **/
/** Reference: Section A.1 in L"afro/ifad/pbl/feum/v1".                 **/
/*************************************************************************/

#if VDMPP
#if VPPSTMT
CreateInstance
        : Identifier LEX_ASSIGN Expression
        {
          *VDMSTMTPARSER::stmt = TYPE_AS_AssignStmt();
          VDMSTMTPARSER::SetPos2(*VDMSTMTPARSER::stmt, @1, @3);
          VDMSTMTPARSER::stmt->SetField(pos_AS_AssignStmt_lhs, *$1);
          VDMSTMTPARSER::stmt->SetField(pos_AS_AssignStmt_rhs, *$3);
          delete $1;
          delete $3;
        }
        ;
#endif // VPPSTMT
#endif // VDMPP

#if FULL
#if VDMPP
Classes
        : ModTexBreak Class
        {
          $$ = new Sequence();
          $$->ImpAppend (*$2);

ORDERLY_ON MYPARSER::AddModOrder($2->GetField(pos_AS_Class_nm), false);

          delete $1;
          delete $2;
        }
        | Classes ModTexBreak Class
        {
          $1->ImpAppend (*$3);

ORDERLY_ON MYPARSER::AddModOrder($3->GetField(pos_AS_Class_nm), $2->GetValue());

          delete $2;
          delete $3;
        }
        ;

Class   : LEX_CLASS Name OptInheritanceClause TexBreak OptDefinitionBlock LEX_END Name
        {
          $$ = new TYPE_AS_Class();
          MYPARSER::SetPos2(*$$, @1, @7);

          VDMPARSER::mangledOpNames.Clear();
          VDMPARSER::mangledFnNames.Clear();
          VDMPARSER::currentClassMod = Nil();

          if (!($2->GetField (pos_AS_Name_ids) == $7->GetField (pos_AS_Name_ids)))
            MYPARSER::Report (L"Class identifiers must be identical.", @2, @7);
          if ($3->Elems().InSet(*$2))
            MYPARSER::Report (L"Class name must not be in super classes.", @2);

          $$->SetField (pos_AS_Class_nm,       *$2);
          $$->SetField (pos_AS_Class_sys,      Bool(false));
          $$->SetField (pos_AS_Class_supercls, *$3);
          $$->SetField (pos_AS_Class_defs,     *$5);
          $$->SetField (pos_AS_Class_useslib,  Nil());

          delete $2;
          delete $3;
          delete $4;
          delete $5;
          delete $7;
        }
        | LEX_DLCLASS Name OptInheritanceClause TexBreak UseSignature TexBreak OptDefinitionBlock LEX_END Name
        {
          $$ = new TYPE_AS_Class();
          MYPARSER::SetPos2(*$$, @1, @9);

          VDMPARSER::mangledOpNames.Clear();
          VDMPARSER::mangledFnNames.Clear();
          VDMPARSER::currentClassMod = Nil();

          if (!($2->GetField (pos_AS_Name_ids) == $9->GetField (pos_AS_Name_ids)))
             MYPARSER::Report (L"Class identifiers must be identical.", @2,@9);

          $$->SetField (pos_AS_Class_nm,       *$2);
          $$->SetField (pos_AS_Class_sys,      Bool(false));
          $$->SetField (pos_AS_Class_supercls, *$3);
          $$->SetField (pos_AS_Class_defs,     *$7);
          $$->SetField (pos_AS_Class_useslib,  *$5);

          delete $2;
          delete $3;
          delete $5;
          delete $7;
          delete $9;
        }
#ifdef VICE
        | LEX_SYSTEM Name OptInheritanceClause TexBreak OptDefinitionBlock LEX_END Name
        {
          $$ = new TYPE_AS_Class();
          MYPARSER::SetPos2(*$$, @1, @7);

          VDMPARSER::mangledOpNames.Clear();
          VDMPARSER::mangledFnNames.Clear();
          VDMPARSER::currentClassMod = Nil();

          if (!($2->GetField (pos_AS_Name_ids) == $7->GetField (pos_AS_Name_ids)))
             MYPARSER::Report (L"System identifiers must be identical.", @2,@7);

          $$->SetField (pos_AS_Class_nm,       *$2);
          $$->SetField (pos_AS_Class_sys,      Bool(true));
          $$->SetField (pos_AS_Class_supercls, *$3);
          $$->SetField (pos_AS_Class_defs,     *$5);
          $$->SetField (pos_AS_Class_useslib,  Nil());

          delete $2;
          delete $3;
          delete $5;
          delete $7;
        }
#endif //VICE
        ;

/*************************************************************************/
/** Synchronization                                                     **/
/**                                                                     **/
/** Reference: Chapter 8 in  L"afro/cg/ed/lrm/v8".                      **/
/*************************************************************************/

SynchronizationDefinitions
        : LEX_SYNC DeclarativeSyncList OptSemi_TexBreak
        { $$ = $2;
          delete $3;
        }
        | LEX_SYNC TexBreak
        { $$ = new Sequence ();
          delete $2;
        }
        ;

DeclarativeSyncList
        : DeclarativeSync
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_SyncDef, Int($$->Length()));
                                         // The offset into the Sync sequence
          delete $1;
        }
        | DeclarativeSyncList ';' TexBreak DeclarativeSync
        { $$ = $1;
          $1->ImpAppend (*$4);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_SyncDef, Int($1->Length()));
                               // The offset into the thread sequence
          delete $3;
          delete $4;
        }
        ;

DeclarativeSync
        : PermissionStmt
        | MutexStmt
        ;

PermissionStmt
        : LEX_PER Name LEX_IMPLY Expression
        { $$ = new TYPE_AS_Permission();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_Permission_spec, *$2);
          $$->SetField (pos_AS_Permission_guard, *$4);

          delete $2;
          delete $4;
        }
        ;

MutexStmt
        : LEX_MUTEX '(' LEX_ALL ')'
        { $$ = new TYPE_AS_Mutex ();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_Mutex_ops, Nil());
        }
        | LEX_MUTEX '(' NameList ')'
        { $$ = new TYPE_AS_Mutex ();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_Mutex_ops, *$3);
          delete $3;
        }
        ;

//////////////////////////
// Sync
//////////////////////////

/*************************************************************************/
/** Thread                                                              **/
/**                                                                     **/
/** Reference: Chapter 9 in  L"afro/cg/ed/lrm/v8".                      **/
/*************************************************************************/

ThreadDefinition
        : LEX_THREAD PeriodicObligation OptSemi_TexBreak
        { $$ = new Generic(*$2);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_ThreadDef, Int(1));

          delete $2;
          delete $3;
        }
        | LEX_THREAD SporadicObligation OptSemi_TexBreak
        { $$ = new Generic(*$2);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_ThreadDef, Int(1));

          delete $2;
          delete $3;
        }
        | LEX_THREAD Statement OptSemi_TexBreak
        { $$ = new Generic(*$2);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_ThreadDef, Int(1));

          delete $2;
          delete $3;
        }
        | LEX_THREAD TexBreak
        { $$ = new Generic(Nil());
          delete $2;
        }
        ;

#ifdef VICE
PeriodicObligation
        : LEX_PERIODIC '(' Expression ',' Expression ',' Expression ',' Expression ')' '(' Name ')'
        { $$ = new TYPE_AS_PerObl();
          MYPARSER::SetPos2(*$$, @1, @13);
          $$->SetField (pos_AS_PerObl_period, *$3);
          $$->SetField (pos_AS_PerObl_jitter, *$5);
          $$->SetField (pos_AS_PerObl_delay,  *$7);
          $$->SetField (pos_AS_PerObl_offset, *$9);
          $$->SetField (pos_AS_PerObl_mtd,    *$12);

          delete $3;
          delete $5;
          delete $7;
          delete $9;
          delete $12;
        }
        ;
#else
PeriodicObligation
        : LEX_PERIODIC '(' NumericLiteral ')' '(' Name ')'
        { $$ = new TYPE_AS_PerObl();
          MYPARSER::SetPos2(*$$, @1, @7);
          TYPE_AS_RealLit dummy_lit;
          dummy_lit.Init(Real(0), NilContextId);
          $$->SetField (pos_AS_PerObl_period, *$3);
          $$->SetField (pos_AS_PerObl_jitter, dummy_lit);
          $$->SetField (pos_AS_PerObl_delay,  dummy_lit);
          $$->SetField (pos_AS_PerObl_offset, dummy_lit);
          $$->SetField (pos_AS_PerObl_mtd,    *$6);

          delete $3;
          delete $6;
        }
        ;
#endif // VICE
SporadicObligation
        : LEX_SPORADIC '(' Expression ',' Expression ',' Expression ')' '(' Name ')'
        { $$ = new TYPE_AS_SpoObl();
          MYPARSER::SetPos2(*$$, @1, @11);
          $$->SetField (pos_AS_SpoObl_delay,  *$3);
          $$->SetField (pos_AS_SpoObl_bound,  *$5);
          $$->SetField (pos_AS_SpoObl_offset, *$7);
          $$->SetField (pos_AS_SpoObl_mtd,    *$10);

          delete $3;
          delete $5;
          delete $7;
          delete $10;
        }
        ;
#endif // VDMPP

/*************************************************************************/
/** Traces                                                              **/
/*************************************************************************/
TracesDefinitions
        : LEX_TRACES ListOfNamedTrace
        { $$ = $2;
        }
        | LEX_TRACES
        { $$ = new Map();
        }
        ;

ListOfNamedTrace
        : NamedTrace
        { $$ = $1;
        }
        | ListOfNamedTrace NamedTrace
        { $$ = $1;
          $$->ImpOverride(*$2);
          delete $2;
        }
        ;

NamedTrace
        : IdentifierSlashList ':' TraceDefinitionList OptSemi
        { $$ = new Map();
          $$->ImpModify (*$1, *$3);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_Traces, *$1);

          delete $1;
          delete $3;
        }
        ;

TraceDefinitionList
        : TraceDefinitionTerm
        { $$ = new Sequence();
          $$->ImpAppend(*$1);
          delete $1;
        }
        | TraceDefinitionList ';' TraceDefinitionTerm
        { $$ = $1;
          $$->ImpAppend(*$3);
          delete $3;
        }
        ;

TraceDefinitionTerm
        : TraceDefinition                             // TraceDef
        { $$ = $1;
        }
        | TraceDefinitionTerm '|' TraceDefinition     // TraceDefAltn
        { $$ = new TYPE_AS_TraceDefAltn();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField(pos_AS_TraceDefAltn_term, *$1);
          $$->SetField(pos_AS_TraceDefAltn_tdef, *$3);
          delete $1;
          delete $3;
        }
        ;

TraceDefinition
        : ListOfTraceBinding SimpleTraceDefinition TraceRepeatPattern
        { $$ = new TYPE_AS_QualifiedRepeatTrace();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField(pos_AS_QualifiedRepeatTrace_binds, *$1);
          $$->SetField(pos_AS_QualifiedRepeatTrace_tdef, *$2);
          $$->SetField(pos_AS_QualifiedRepeatTrace_pat, *$3);
          delete $1;
          delete $2;
          delete $3;
        }
        | ListOfTraceBinding SimpleTraceDefinition
        { $$ = new TYPE_AS_QualifiedTrace();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField(pos_AS_QualifiedTrace_binds, *$1);
          $$->SetField(pos_AS_QualifiedTrace_tdef, *$2);
          delete $1;
          delete $2;
        }
        | SimpleTraceDefinition TraceRepeatPattern
        { $$ = new TYPE_AS_RepeatTrace();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField(pos_AS_RepeatTrace_tdef, *$1);
          $$->SetField(pos_AS_RepeatTrace_pat, *$2);
          delete $1;
          delete $2;
        }
        | SimpleTraceDefinition
        { $$ = $1;
        }
        ;

SimpleTraceDefinition
        : '(' TraceDefinitionList ')'
        { $$ = new TYPE_AS_TraceBracketedExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField(1, *$2);
          delete $2;
        }
        | LEX_NONDET '(' TraceDefinitionList ')'
        { $$ = new TYPE_AS_TracePermuteExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(1, *$3);
          delete $3;
        }
#ifdef VDMSL
        | Name '(' ExpressionList ')'
        { $$ = new TYPE_AS_TraceApplyExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_TraceApplyExpr_op, *$1);
          $$->SetField(pos_AS_TraceApplyExpr_args, *$3);
          delete $1;
          delete $3;
        }
#endif // VDMSL
#ifdef VDMPP
        | Identifier '.' Identifier '(' ExpressionList ')'
        { $$ = new TYPE_AS_TraceApplyExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField(pos_AS_TraceApplyExpr_obj, *$1);
          $$->SetField(pos_AS_TraceApplyExpr_op, *$3);
          $$->SetField(pos_AS_TraceApplyExpr_args, *$5);
          delete $1;
          delete $3;
          delete $5;
        }
        | Name '(' ExpressionList ')'
        { $$ = new TYPE_AS_TraceApplyExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_TraceApplyExpr_obj, Nil());
          $$->SetField(pos_AS_TraceApplyExpr_op, *$1);
          $$->SetField(pos_AS_TraceApplyExpr_args, *$3);
          delete $1;
          delete $3;
        }
#endif // VDMPP
        ;

ListOfTraceBinding
        : ListOfTraceBinding TraceBind
        {  $$ = $1;
           $$->ImpAppend (*$2);
           delete $2;
        }
        | TraceBind
        {  $$ = new Sequence ();
           $$->ImpAppend (*$1);
           delete $1;
        }
        ;

TraceBind
        : LEX_LET ListOfLocalDefinitions LEX_IN
        {
          $$ = new TYPE_AS_LocalTraceBind();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_LocalTraceBind_localdefs, *$2);
          delete $2;
        }
        | LEX_LET Bind LEX_IN
        {
          $$ = new TYPE_AS_LetTraceBind();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_LetTraceBind_bind, ASTAUX::BindToBindList(*$2));
          delete $2;
        }
        | LEX_LET BindList LEX_IN
        {
          $$ = new TYPE_AS_LetTraceBind();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_LetTraceBind_bind, *$2);
          delete $2;
        }
        | LEX_LET Bind LEX_BE LEX_ST Expression LEX_IN
        {
          $$ = new TYPE_AS_LetBeTraceBind();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_LetBeTraceBind_bind,   ASTAUX::BindToBindList(*$2));
          $$->SetField (pos_AS_LetBeTraceBind_stexpr, *$5);
          delete $2;
          delete $5;
        }
        | LEX_LET BindList LEX_BE LEX_ST Expression LEX_IN
        {
          $$ = new TYPE_AS_LetBeTraceBind();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_LetBeTraceBind_bind,   *$2);
          $$->SetField (pos_AS_LetBeTraceBind_stexpr, *$5);
          delete $2;
          delete $5;
        }
        ;

TraceRepeatPattern
        : '*'
        { $$ = new Generic(Int(ANY));
        }
        | '+'
        { $$ = new Generic(Int(ATLEASTONE));
        }
        | '?'
        { $$ = new Generic(Int(POSSIBLY));
        }
        | '{' NumericLiteral ',' NumericLiteral '}'
        { TYPE_AS_RepeatInterval ri;
          MYPARSER::SetPos2(ri, @1, @5);
          ri.SetField (pos_AS_RepeatInterval_low,   *$2);
          ri.SetField (pos_AS_RepeatInterval_high,  *$4);
          $$ = new Generic(ri);
          delete $2;
          delete $4;
        }
        | '{' NumericLiteral '}'
        { TYPE_AS_RepeatFixed rf;
          MYPARSER::SetPos2(rf, @1, @3);
          rf.SetField (pos_AS_RepeatFixed_num,   *$2);
          $$ = new Generic(rf);
          delete $2;
        }
        ;

IdentifierSlashList
        : Identifier
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | IdentifierSlashList '/' Identifier
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;


/*************************************************************************/
/** Inheritance                                                         **/
/**                                                                     **/
/** Reference: Section A.2 i L"afro/ifad/pbl/feum/v1".                  **/
/*************************************************************************/
#ifdef VDMPP
OptInheritanceClause
        : /* empty */
        { $$ = new SEQ<TYPE_AS_Name>();
        }
        | LEX_IS LEX_SUBCLASS LEX_OF IdentifierCommaList
        { $$ = $4;
        }
        ;

/*************************************************************************/
/** Instance Variables                                                  **/
/**                                                                     **/
/** Reference: Section A.3 in L"afro/ifad/pbl/feum/v1".                 **/
/*************************************************************************/
InstanceVariableDefinitions
        : LEX_INSTANCE LEX_VARIABLES TexBreak InstanceVariableDefinitionList OptSemi_TexBreak
        { $$ = $4;
          delete $3;
          delete $5;
        }
        | LEX_INSTANCE LEX_VARIABLES TexBreak
        { $$ = new Sequence;
          delete $3;
        }
        ;

InstanceVariableDefinitionList
        : InstanceVariableDefinition
        {
          $$ = new Sequence();
          $$->ImpAppend(*$1);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_InstVarDef, Int($$->Length()));

          delete $1;
        }
        | InstanceVariableDefinitionList ';' TexBreak InstanceVariableDefinition
        {
          $$ = $1;
          $$->ImpAppend(*$4);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_InstVarDef, Int($$->Length()));

          delete $3;
          delete $4;
        }
        ;

InstanceVariableDefinition
        : InvariantDefinition
        | InstAssignmentDefinition
        ;

InvariantDefinition
        : LEX_INV Expression
        {
          $$ = new TYPE_AS_InstanceInv();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField(pos_AS_InstanceInv_expr, *$2);
          $$->SetField(pos_AS_InstanceInv_access,   Int (DEFAULT_AS));
          delete $2;
        }
        ;

InstAssignmentDefinition
        : AssignmentDefinition
        { $$ = new TYPE_AS_InstAssignDef ();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField(pos_AS_InstAssignDef_ad,     *$1);
          $$->SetField(pos_AS_InstAssignDef_access, Int (DEFAULT_AS));
          $$->SetField(pos_AS_InstAssignDef_stat,   Bool(false));
          delete $1;
        }
        | StaticAccess AssignmentDefinition
        { $$ = new TYPE_AS_InstAssignDef ();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField(pos_AS_InstAssignDef_ad, *$2);
          $$->SetField(pos_AS_InstAssignDef_access, $1->GetField(1));
          $$->SetField(pos_AS_InstAssignDef_stat,   $1->GetField(2));
          delete $1;
          delete $2;
        }
        ;

////////////////////////////////
// VPP - InstanceVardef
///////////////////////////////

//////////////////////////////
// VPP - InstanceInv
//////////////////////////////

/////////////////////////
// VPP - InstanceInit
/////////////////////////

/*************************************************************************/
/** Methods                                                             **/
/**                                                                     **/
/** Reference: Section A.4 in L"afro/ifad/pbl/feum/v1".                 **/
/*************************************************************************/
#endif //VDMPP

TypeDefinitions
        : LEX_TYPES TexBreak ListOfTypeDefinitions OptSemi_TexBreak
        {
          $$ = $3;
          delete $2;
          delete $4;
        }
        | LEX_TYPES TexBreak
        {
          $$ = new Map();
          delete $2;
        }
        ;

ListOfTypeDefinitions
        : TypeDefinition
        {
          $$ = new Map;
          $$->Insert ($1->GetRecord (1), $1->GetRecord (2));

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_TypeDef, $1->GetRecord (1));

          delete $1;
        }
        | ListOfTypeDefinitions ';' TexBreak TypeDefinition
        {
          if ($1->DomExists ($4->GetRecord (1))) {
            MYPARSER::Report (L"Type already defined", @4);
          }
          else {
            $1->Insert ($4->GetRecord (1), $4->GetRecord (2));
          }

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_TypeDef, $4->GetRecord (1));

          delete $3;
          delete $4;
        }
        ;

TypeDefinition
        : SimpleTypeDefinition
        | CompositeTypeDefinition
#ifdef VDMPP
        | Access SimpleTypeDefinition
        {
          $$ = $2;
          TYPE_AS_TypeDef td ($2->GetRecord (2));
          td.SetField(pos_AS_TypeDef_access, *$1);
          $$->SetField(2, td);
          delete $1;
        }
        | Access CompositeTypeDefinition
        {
          $$ = $2;
          TYPE_AS_TypeDef td ($2->GetRecord (2));
          td.SetField(pos_AS_TypeDef_access, *$1);
          $$->SetField(2, td);
          delete $1;
        }
#endif // VDMPP
        ;

SimpleTypeDefinition
        : Identifier LEX_EQUAL Type
        {
          $$ = new Tuple (2); // (AS`Name * AS`TypeDef)

          TYPE_AS_TypeDef td;
          MYPARSER::SetPos2(td, @1, @3);
          td.SetField (pos_AS_TypeDef_nm,     *$1);
          td.SetField (pos_AS_TypeDef_shape,  *$3);
          td.SetField (pos_AS_TypeDef_Inv,    Nil ());
          td.SetField (pos_AS_TypeDef_Eq,     Nil ());
          td.SetField (pos_AS_TypeDef_Ord,    Nil ());
          td.SetField (pos_AS_TypeDef_access, Int (DEFAULT_AS));

          $$->SetField (1, *$1);
          $$->SetField (2, td);

          delete $1;
          delete $3;
        }
        | Identifier LEX_EQUAL Type InvariantEqualOrder
        {
          $$ = new Tuple (2); // (AS`Name * AS`TypeDef)

          TYPE_AS_TypeDef td;
          MYPARSER::SetPos2(td, @1, @4);
          td.SetField (pos_AS_TypeDef_nm,     *$1);
          td.SetField (pos_AS_TypeDef_shape,  *$3);
          td.SetField (pos_AS_TypeDef_Inv,    $4->GetField(1));
          td.SetField (pos_AS_TypeDef_Eq,     $4->GetField(2));
          td.SetField (pos_AS_TypeDef_Ord,    $4->GetField(3));
          td.SetField (pos_AS_TypeDef_access, Int (DEFAULT_AS));

          $$->SetField (1, *$1);
          $$->SetField (2, td);

          delete $1;
          delete $3;
          delete $4;
        }
        ;

CompositeTypeDefinition
        : Identifier LEX_DOUBLE_COLON FieldList
        {
          $$ = new Tuple (2); // (AS`Name * AS`TypeDef)

          TYPE_AS_CompositeType co;
          MYPARSER::SetPos2(co, @1, @3);
          co.SetField (pos_AS_CompositeType_name,   *$1);
          co.SetField (pos_AS_CompositeType_fields, *$3);

          TYPE_AS_TypeDef td;
          MYPARSER::SetPos2(td, @1, @3);
          td.SetField (pos_AS_TypeDef_nm,     *$1);
          td.SetField (pos_AS_TypeDef_shape,  co);
          td.SetField (pos_AS_TypeDef_Inv,    Nil ());
          td.SetField (pos_AS_TypeDef_Eq,     Nil ());
          td.SetField (pos_AS_TypeDef_Ord,    Nil ());
          td.SetField (pos_AS_TypeDef_access, Int (DEFAULT_AS));

          $$->SetField (1, *$1);
          $$->SetField (2, td);

          delete $1;
          delete $3;
        }
        | Identifier LEX_DOUBLE_COLON FieldList InvariantEqualOrder
        {
          $$ = new Tuple (2); // (AS`Name * AS`TypeDef)

          TYPE_AS_CompositeType co;
          MYPARSER::SetPos2(co, @1, @4);
          co.SetField (pos_AS_CompositeType_name,   *$1);
          co.SetField (pos_AS_CompositeType_fields, *$3);

          TYPE_AS_TypeDef td;
          MYPARSER::SetPos2(td, @1, @4);
          td.SetField (pos_AS_TypeDef_nm,     *$1);
          td.SetField (pos_AS_TypeDef_shape,  co);
          td.SetField (pos_AS_TypeDef_Inv,    $4->GetField(1));
          td.SetField (pos_AS_TypeDef_Eq,     $4->GetField(2));
          td.SetField (pos_AS_TypeDef_Ord,    $4->GetField(3));
          td.SetField (pos_AS_TypeDef_access, Int (DEFAULT_AS));

          $$->SetField (1, *$1);
          $$->SetField (2, td);

          delete $1;
          delete $3;
          delete $4;
        }
        ;
#endif //FULL

Type    : BracketedType
        | BasicType
        | Quote
        | CompositeType
        | UnionType
        | ProductType
        | OptionalType
        | SetType
        | SeqType
        | MapType
        | PFunctionType
        | TFunctionType
        | TypeName
        | TypeVariable
#if FULL
//        | OperationType
        | AllType
#endif //FULL
        ;

#if FULL
// for VDM10
AllType
        : '?'
        {
          if (!Settings.IsAllTypeAllowed(VDMPARSER::currentClassMod))
            MYPARSER::Report (L"Unknown type : ?", @1);
          $$ = new TYPE_AS_AllType();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;
#endif //FULL

BracketedType
        : '(' Type ')'
        {
          $$ = new TYPE_AS_BracketedType();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField(pos_AS_BracketedType_tp, *$2);
          delete $2;
        }
        ;

BasicType
        : LEX_BOOL
        {
          $$ = new TYPE_AS_BooleanType();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        | LEX_NAT
        {
          $$ = new TYPE_AS_NumericType();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_NumericType_qtp, (Int)NAT);
        }
        | LEX_NATONE
        {
          $$ = new TYPE_AS_NumericType();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_NumericType_qtp, (Int)NATONE);
        }
        | LEX_INT
        {
          $$ = new TYPE_AS_NumericType();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_NumericType_qtp, (Int)INTEGER);
        }
        | LEX_RAT
        {
          $$ = new TYPE_AS_NumericType();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_NumericType_qtp, (Int)RAT);
        }
        | LEX_REAL
        {
          $$ = new TYPE_AS_NumericType();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_NumericType_qtp, (Int)REAL);
        }
        | LEX_CHAR
        {
          $$ = new TYPE_AS_CharType();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        | LEX_TOKEN
        {
          $$ = new TYPE_AS_TokenType();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;

Quote :
        LEX_quote_lit
        {
          $$ = new TYPE_AS_QuoteType();
          MYPARSER::SetPos2(*$$, @1, @1);
          TYPE_AS_QuoteLit rp;
          MYPARSER::SetPos2(rp, @1, @1);
          rp.SetField (pos_AS_QuoteLit_val, Sequence(TBWSTR::string2wstring(*$1)));
          $$->SetField (pos_AS_QuoteType_lit, rp);
        }
        ;

CompositeType
        : LEX_COMPOSE Identifier LEX_OF FieldList LEX_END
        {
          $$ = new TYPE_AS_CompositeType();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_CompositeType_name, *$2);
          $$->SetField (pos_AS_CompositeType_fields, *$4);
          delete $2;
          delete $4;
        }
        ;

FieldList
        : /* empty */
        { $$ = new Sequence ();
        }
        | NFieldList
        ;

NFieldList
        : Field
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | NFieldList Field
        { $1->ImpAppend (*$2);
          delete $2;
        }
        ;

Field
        : Identifier ':' Type
        {
          $$ = new TYPE_AS_Field();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Field_sel,  *$1);
          $$->SetField (pos_AS_Field_type, *$3);
          $$->SetField (pos_AS_Field_dc,   Bool(false));
          delete $1;
          delete $3;
        }
        | Identifier LEX_DONTCARE Type
        {
          $$ = new TYPE_AS_Field();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Field_sel,  *$1);
          $$->SetField (pos_AS_Field_type, *$3);
          $$->SetField (pos_AS_Field_dc,   Bool(true));
          delete $1;
          delete $3;
        }
        | Type
        {
          $$ = new TYPE_AS_Field();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_Field_sel,  Nil ());
          $$->SetField (pos_AS_Field_type, *$1);
          $$->SetField (pos_AS_Field_dc,   Bool(false));
          delete $1;
        }
        ;

UnionType
        : Type '|' Type
        {
          $$ = new TYPE_AS_UnionType();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          SEQ<TYPE_AS_Type> Types ;
          if ($1->Is (TAG_TYPE_AS_UnionType))
            Types.ImpConc($1->GetSequence (pos_AS_UnionType_tps));
          else
            Types.ImpAppend (*$1);
          Types.ImpAppend (*$3);
          $$->SetField(pos_AS_UnionType_tps, Types);
          delete $1;
          delete $3;
        }
        ;

OptionalType
        : '[' Type ']'
        {
          $$ = new TYPE_AS_OptionalType();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_OptionalType_tp, *$2);
          delete $2;
        }
        ;

ProductType
        : Type '*' Type
        {
          $$ = new TYPE_AS_ProductType();
          MYPARSER::SetPos3(*$$, @1, @2, @3);

          SEQ<TYPE_AS_Type> Types;
          if ($1->Is (TAG_TYPE_AS_ProductType))
            Types.ImpConc($1->GetSequence (pos_AS_ProductType_tps));
          else
            Types.ImpAppend (*$1);
          Types.ImpAppend (*$3);

          $$->SetField (pos_AS_ProductType_tps, Types);
          delete $1;
          delete $3;
        }
        ;

SetType
        : Set0Type
        | Set1Type
        ;

Set0Type
        : LEX_SET LEX_OF Type  %prec LEX_DUMMY_PRECEDENCE_SET_OF
        {
          $$ = new TYPE_AS_Set0Type();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Set0Type_elemtp, *$3);
          delete $3;
        }
        ;

Set1Type
        : LEX_SET1 LEX_OF Type  %prec LEX_DUMMY_PRECEDENCE_SET1_OF
        {
          $$ = new TYPE_AS_Set1Type();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Set1Type_elemtp, *$3);
          delete $3;
        }
        ;

SeqType
        : Seq0Type
        | Seq1Type
        ;

Seq0Type
        : LEX_SEQ LEX_OF Type %prec LEX_DUMMY_PRECEDENCE_SEQ_OF
        {
          $$ = new TYPE_AS_Seq0Type();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Seq0Type_elemtp, *$3);
          delete $3;
        }
        ;

Seq1Type
        : LEX_SEQ1 LEX_OF Type %prec LEX_DUMMY_PRECEDENCE_SEQ1_OF
        {
          $$ = new TYPE_AS_Seq1Type();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Seq1Type_elemtp, *$3);
          delete $3;
        }
        ;

MapType
        : GeneralMap0Type
        | GeneralMap1Type
        | InjectiveMap0Type
        | InjectiveMap1Type
        ;

GeneralMap0Type
        : LEX_MAP Type LEX_TO Type
        {
          $$ = new TYPE_AS_GeneralMap0Type();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_GeneralMap0Type_mapdom, *$2);
          $$->SetField (pos_AS_GeneralMap0Type_maprng, *$4);
          delete $2;
          delete $4;
        }
        ;

GeneralMap1Type
        : LEX_MAP1 Type LEX_TO Type
        {
          $$ = new TYPE_AS_GeneralMap1Type();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_GeneralMap1Type_mapdom, *$2);
          $$->SetField (pos_AS_GeneralMap1Type_maprng, *$4);
          delete $2;
          delete $4;
        }
        ;

InjectiveMap0Type
        : LEX_INMAP Type LEX_TO Type
        {
          $$ = new TYPE_AS_InjectiveMap0Type();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_InjectiveMap0Type_mapdom, *$2);
          $$->SetField (pos_AS_InjectiveMap0Type_maprng, *$4);
          delete $2;
          delete $4;
        }
        ;

InjectiveMap1Type
        : LEX_INMAP1 Type LEX_TO Type
        {
          $$ = new TYPE_AS_InjectiveMap1Type();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_InjectiveMap1Type_mapdom, *$2);
          $$->SetField (pos_AS_InjectiveMap1Type_maprng, *$4);
          delete $2;
          delete $4;
        }
        ;

TypeName
        : Name
        {
          $$ = new TYPE_AS_TypeName();
          MYPARSER::SetPos2(*$$, @1, @1); // $1->GetField (2)
          $$->SetField (pos_AS_TypeName_name, *$1);
          delete $1;
        }
        ;

TypeVariable
        : '@' Identifier
        {
          $$ = new TYPE_AS_TypeVar();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_TypeVar_name, *$2);
          delete $2;
        }
        ;

FunctionType
        : PFunctionType
        | TFunctionType
        ;

TFunctionType
        : Type LEX_TARROW Type
        {
          $$ = new TYPE_AS_TotalFnType();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          if ($1->Is(TAG_TYPE_AS_ProductType))
            $$->SetField (pos_AS_TotalFnType_fndom, $1->GetSequence(pos_AS_ProductType_tps));
          else
            $$->SetField (pos_AS_TotalFnType_fndom, mk_sequence(*$1));
          $$->SetField (pos_AS_TotalFnType_fnrng, *$3);
          delete $1;
          delete $3;
        }
        | '(' ')' LEX_TARROW Type
        {
          $$ = new TYPE_AS_TotalFnType();
          MYPARSER::SetPos3(*$$, @1, @2, @4);
          $$->SetField (pos_AS_TotalFnType_fndom, SEQ<TYPE_AS_Type>());
          $$->SetField (pos_AS_TotalFnType_fnrng, *$4);
          delete $4;
        }
        ;

PFunctionType
        : Type LEX_ARROW Type
        {
          $$ = new TYPE_AS_PartialFnType();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          if ($1->Is(TAG_TYPE_AS_ProductType))
            $$->SetField (pos_AS_PartialFnType_fndom, $1->GetSequence(pos_AS_ProductType_tps));
          else
            $$->SetField (pos_AS_PartialFnType_fndom, mk_sequence(*$1));
          $$->SetField (pos_AS_PartialFnType_fnrng, *$3);
          delete $1;
          delete $3;
        }
        | '(' ')' LEX_ARROW Type
        {
          $$ = new TYPE_AS_PartialFnType();
          MYPARSER::SetPos3(*$$, @1, @3, @4);
          $$->SetField (pos_AS_PartialFnType_fndom, SEQ<TYPE_AS_Type>());
          $$->SetField (pos_AS_PartialFnType_fnrng, *$4);
          delete $4;
        }
        ;

#if FULL
/** Section in reference document: 9.4.3 Value Definitions ********************/
/** Deviations from the standard/Remarks: IPTES                              **/

#ifdef VDMSL
ValueDefinitions
        : LEX_VALUES TexBreak ListOfValueDefinitions OptSemi_TexBreak
        {
          $$ = $3;
          delete $2;
          delete $4;
        }
        | LEX_VALUES TexBreak
        {
          $$ = new Sequence;
          delete $2;
        }
        ;

ListOfValueDefinitions
        : ValueDefinition
        { $$ = new Sequence;
          $$->ImpAppend (*$1);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_ValueDef, $1->GetRecord (pos_AS_ValueDef_pat));

          delete $1;
        }
        | ListOfValueDefinitions ';' TexBreak ValueDefinition
        { $1->ImpAppend (*$4);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_ValueDef, $4->GetRecord (pos_AS_ValueDef_pat));

          delete $3;
          delete $4;
        }
        ;
#endif // VDMSL

#ifdef VDMPP
ValueDefinitions
        : LEX_VALUES TexBreak ListOfValueDefinitions OptSemi_TexBreak
        {
          $$ = $3;
          delete $2;
          delete $4;
        }
        | LEX_VALUES TexBreak
        {
          $$ = new Sequence;
          delete $2;
        }
        ;

ListOfValueDefinitions
        : AccessValueDefinition
        { $$ = new Sequence;
          $$->ImpAppend (*$1);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_ValueDef, $1->GetField (pos_AS_ValueDef_pat));

          delete $1;
        }
        | ListOfValueDefinitions ';' TexBreak AccessValueDefinition
        { $1->ImpAppend (*$4);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_AS_ValueDef, $4->GetField (pos_AS_ValueDef_pat));

          delete $3;
          delete $4;
        }
        ;

AccessValueDefinition
        : ValueDefinition
        {
          $$->SetField (pos_AS_ValueDef_access, Int (DEFAULT_AS));
        }
        | StaticAccess ValueDefinition
        { $$ = $2;
          $$->SetField (pos_AS_ValueDef_access, $1->GetField(1));
          $$->SetField (pos_AS_ValueDef_stat,   $1->GetField(2));
          delete $1;
        }
        ;

OperationAccess
        : StaticAccess
        | LEX_PURE
        { $$ = new Tuple(4);
          $$->SetField(1, Int (DEFAULT_AS));
          $$->SetField(2, Bool(false)); // static
          $$->SetField(3, Bool(true));  // sync
          $$->SetField(4, Bool(true));  // pure
        }
        | LEX_PURE StaticAccess
        {
          $$ = $2;
          $$->SetField(4, Bool(true)); // pure
        }
#ifdef VICE
        | LEX_ASYNC
        { $$ = new Tuple(4);
          $$->SetField(1, Int (DEFAULT_AS));
          $$->SetField(2, Bool(false)); // static
          $$->SetField(3, Bool(false)); // sync
          $$->SetField(4, Bool(false)); // pure
        }
        | LEX_ASYNC StaticAccess
        { 
          $$ = $2;
          $$->SetField(3, Bool(false)); // sync
        }
#endif // VICE
        ;

StaticAccess
        : LEX_STATIC
        { $$ = new Tuple(4);
          $$->SetField(1, Int (DEFAULT_AS));
          $$->SetField(2, Bool(true));  // static
          $$->SetField(3, Bool(true));  // sync
          $$->SetField(4, Bool(false)); // pure
        }
        | Access
        { $$ = new Tuple(4);
          $$->SetField(1, *$1);
          $$->SetField(2, Bool(false)); // static
          $$->SetField(3, Bool(true));  // sync
          $$->SetField(4, Bool(false)); // pure
          delete $1;
        }
        | LEX_STATIC Access
        { $$ = new Tuple(4);
          $$->SetField(1, *$2);
          $$->SetField(2, Bool(true));  // static
          $$->SetField(3, Bool(true));  // sync
          $$->SetField(4, Bool(false)); // pure
          delete $2;
        }
        | Access LEX_STATIC
        { $$ = new Tuple(4);
          $$->SetField(1, *$1);
          $$->SetField(2, Bool(true));  // static
          $$->SetField(3, Bool(true));  // sync
          $$->SetField(4, Bool(false)); // pure
          delete $1;
        }
        ;

Access
        : LEX_PRIVATE
        { $$ = new Int (PRIVATE_AS);
        }
        | LEX_PROTECTED
        { $$ = new Int (PROTECTED_AS);
        }
        | LEX_PUBLIC
        { $$ = new Int (PUBLIC_AS);
        }
        ;
#endif // VDMPP

/** Section in reference document: 9.4.4 Function Definitions *****************/
/** Deviations from the standard/Remarks: IPTES                              **/

FunctionDefinitions
        : LEX_FUNCTIONS TexBreak ListOfFunctionDefinitions OptSemi_TexBreak
        {
          $$ = $3;
          delete $2;
          delete $4;
        }
        | LEX_FUNCTIONS TexBreak
        {
          $$ = new Map;
          delete $2;
        }
        ;

#if VDMSL
ListOfFunctionDefinitions
        : FunctionDefinition
        {
          $$ = new Map;
          $$->Insert ($1->GetRecord (1), $1->GetRecord (2));

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_FnDef, $1->GetRecord (1));

          delete $1;
        }
        | ListOfFunctionDefinitions ';' TexBreak FunctionDefinition
        {
          if ($1->DomExists ($4->GetRecord (1))) {
            MYPARSER::Report (L"Function already defined", @4);
          }
          else {
            $1->Insert ($4->GetRecord (1), $4->GetRecord (2));
          }

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_FnDef, $4->GetRecord (1));

          delete $3;
          delete $4;
        }
        ;
#endif // VDMSL
#if VDMPP
ListOfFunctionDefinitions
        : FunctionDefinition
        {
          $$ = new Map;
          $$->Insert ($1->GetRecord(1), $1->GetRecord(2));

          TYPE_AS_Name mangledNm (MYPARSER::GetMangleName($1->GetRecord(2)));

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_FnDef, mangledNm);

          delete $1;
        }
        | ListOfFunctionDefinitions ';' TexBreak FunctionDefinition
        {
          const TYPE_AS_Name & orgName ($4->GetRecord(1));
          TYPE_AS_FnDef fnd ($4->GetRecord(2));
          TYPE_AS_Name mangledNewName (MYPARSER::GetMangleName(fnd));
          if ($1->DomExists (orgName)) {
            TYPE_AS_FnDef fn ($1->operator[](orgName));
            TYPE_AS_Name mangledOrgName (MYPARSER::GetMangleName(fn));

            if (mangledNewName == mangledOrgName) {
              MYPARSER::Report (L"Function already defined", @4);
            }
            else {
              // TODO: possibly memory leak here
              // Pull out existing fn, mangle its name, reinsert
              $1->RemElem(orgName);
              $1->Insert(mangledOrgName, fn);

              // Mangle this name, insert
              $1->Insert(mangledNewName, fnd);
              VDMPARSER::mangledFnNames.Insert(orgName);
            }
          }
          else if ($1->DomExists (mangledNewName)) {
            MYPARSER::Report (L"Operation already defined", @4);
          }
          else if (VDMPARSER::mangledFnNames.InSet(orgName)) {
            // This name has already been mangled once
            $1->Insert(mangledNewName, fnd);
            VDMPARSER::mangledFnNames.Insert(orgName);
          }
          else {
            $1->Insert (orgName, fnd);
          }

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_FnDef, mangledNewName);

          delete $3;
          delete $4;
        }
        ;
#endif // VDMPP

FunctionDefinition
        : ExplFunctionDefinition
        | ImplFunctionDefinition
        | ExtExplFunctionDefinition
#ifdef VDMPP
        | StaticAccess ExplFunctionDefinition
        {
          $$ = $2;
          TYPE_AS_ExplFnDef fndef ($2->GetRecord(2));
          fndef.SetField (pos_AS_ExplFnDef_access, $1->GetField(1));
          fndef.SetField (pos_AS_ExplFnDef_stat,   $1->GetBool(2));
          $$->SetField(2, fndef);
          delete $1;
        }
        | StaticAccess ImplFunctionDefinition
        {
          $$ = $2;
          TYPE_AS_ImplFnDef fndef ($2->GetRecord(2));
          fndef.SetField (pos_AS_ImplFnDef_access, $1->GetField(1));
          fndef.SetField (pos_AS_ImplFnDef_stat,   $1->GetBool(2));
          $$->SetField(2, fndef);
          delete $1;
        }
        | StaticAccess ExtExplFunctionDefinition
        {
          $$ = $2;
          TYPE_AS_FnDef fndef ($2->GetRecord(2));
          fndef.SetField (pos_AS_ExtExplFnDef_access, $1->GetField(1));
          fndef.SetField (pos_AS_ExtExplFnDef_stat,   $1->GetBool(2));
          $$->SetField(2, fndef);
          delete $1;
        }
#endif // VDMPP
        ;

ImplFunctionDefinition
        : FnOpNameIdentifier TypeVarList ParameterTypes NonEmptyIdentifierTypePairList Pre Post
        { $$ = new Tuple (2); // (AS`Name * AS`ImplFnDef)
          TYPE_AS_ImplFnDef ifd;
          MYPARSER::SetPos2(ifd, @1, @6);
          ifd.SetField (pos_AS_ImplFnDef_nm,       *$1);
          ifd.SetField (pos_AS_ImplFnDef_params,   *$2);
          ifd.SetField (pos_AS_ImplFnDef_partps,   *$3);
          ifd.SetField (pos_AS_ImplFnDef_resnmtps, *$4);
          ifd.SetField (pos_AS_ImplFnDef_fnpre,    *$5);
          ifd.SetField (pos_AS_ImplFnDef_fnpost,   *$6);
          ifd.SetField (pos_AS_ImplFnDef_access,   Int (DEFAULT_AS));
          ifd.SetField (pos_AS_ImplFnDef_stat,     Bool(false));

          $$->SetField (1, *$1);
          $$->SetField (2, ifd);

          delete $1;
          delete $2;
          delete $3;
          delete $4;
          delete $5;
          delete $6;
        }
        ;

ExplFunctionDefinition
        : FnOpNameIdentifier TypeVarList ':' FunctionType FnOpNameIdentifier ParametersList LEX_IS_DEFINED_AS FnBody PrePost Measure
        { $$ = new Tuple (2); // (AS`Name * AS`ExplFnDef)

          TYPE_AS_ExplFnDef efd;
          MYPARSER::SetPos2(efd, @1, @9);
          if (! (*$1 == *$5))
            MYPARSER::Report (L"Identifiers in function heading should be identical.", @1, @5);
          efd.SetField (pos_AS_ExplFnDef_nm,      *$1);
          efd.SetField (pos_AS_ExplFnDef_tpparms, *$2);
          efd.SetField (pos_AS_ExplFnDef_tp,      *$4);
          efd.SetField (pos_AS_ExplFnDef_parms,   *$6);
          efd.SetField (pos_AS_ExplFnDef_body,    *$8);
          efd.SetField (pos_AS_ExplFnDef_fnpre,   $9->GetField (1));
          efd.SetField (pos_AS_ExplFnDef_fnpost,  $9->GetField (2));
          efd.SetField (pos_AS_ExplFnDef_access,  Int (DEFAULT_AS));
          efd.SetField (pos_AS_ExplFnDef_stat,    Bool(false));
          efd.SetField (pos_AS_ExplFnDef_measu,   *$10);

          $$->SetField (1, *$1);
          $$->SetField (2, efd);

          delete $1;
          delete $2;
          delete $4;
          delete $5;
          delete $6;
          delete $8;
          delete $9;
          delete $10;
        }
        ;

ExtExplFunctionDefinition
        : FnOpNameIdentifier TypeVarList ParameterTypes NonEmptyIdentifierTypePairList LEX_IS_DEFINED_AS FnBody PrePost Measure
        { $$ = new Tuple (2); // (AS`name * AS`ExtExplFnDef)

          TYPE_AS_ExtExplFnDef eefd;
          MYPARSER::SetPos2(eefd, @1, @8);
          eefd.SetField (pos_AS_ExtExplFnDef_nm,       *$1);
          eefd.SetField (pos_AS_ExtExplFnDef_params,   *$2);
          eefd.SetField (pos_AS_ExtExplFnDef_partps,   *$3);
          eefd.SetField (pos_AS_ExtExplFnDef_resnmtps, *$4);
          eefd.SetField (pos_AS_ExtExplFnDef_body,     *$6);
          eefd.SetField (pos_AS_ExtExplFnDef_fnpre,    $7->GetField (1));
          eefd.SetField (pos_AS_ExtExplFnDef_fnpost,   $7->GetField (2));
          eefd.SetField (pos_AS_ExtExplFnDef_access,   Int (DEFAULT_AS));
          eefd.SetField (pos_AS_ExtExplFnDef_stat,     Bool(false));
          eefd.SetField (pos_AS_ExtExplFnDef_measu,    *$8);

          $$->SetField (1, *$1);
          $$->SetField (2, eefd);

          delete $1;
          delete $2;
          delete $3;
          delete $4;
          delete $6;
          delete $7;
          delete $8;
        }
        ;

/** Section in reference document: 9.4.5 Operation Definitions ****************/
/** Deviations from the standard/Remarks: IPTES                              **/

OperationDefinitions
        : LEX_OPERATIONS TexBreak ListOfOperationDefinitions OptSemi_TexBreak
        {
          $$ = $3;
          delete $2;
          delete $4;
        }
        | LEX_OPERATIONS TexBreak
        {
          $$ = new Map;
          delete $2;
        }
        ;

#if VDMSL
ListOfOperationDefinitions
        : OperationDefinition
        {
          $$ = new Map();
          $$->Insert ($1->GetRecord(1), $1->GetRecord(2));

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_OpDef, $1->GetRecord (1));

          delete $1;
        }
        | ListOfOperationDefinitions ';' TexBreak OperationDefinition
        {
          if ($1->DomExists ($4->GetRecord (1))) {
            MYPARSER::Report (L"Operation already defined", @4);
          }
          else {
            $$->Insert ($4->GetRecord(1), $4->GetRecord(2));
          }

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_OpDef, $4->GetRecord (1));

          delete $3;
          delete $4;
        }
        ;
#endif // VDMSL
#if VDMPP
ListOfOperationDefinitions
        : OperationDefinition
        {
          $$ = new Map();
          $$->Insert ($1->GetRecord(1), $1->GetRecord(2));

          TYPE_AS_Name mangledNm (MYPARSER::GetMangleName($1->GetRecord(2)));
ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_OpDef, mangledNm);

          delete $1;
        }
        | ListOfOperationDefinitions ';' TexBreak OperationDefinition
        {
          const TYPE_AS_Name & orgName ($4->GetRecord(1));
          TYPE_AS_OpDef opd ($4->GetRecord(2));
          TYPE_AS_Name mangledNewName (MYPARSER::GetMangleName(opd));
          if ($1->DomExists (orgName)) {
            TYPE_AS_OpDef op ($1->operator[](orgName)); 
            TYPE_AS_Name mangledOrgName (MYPARSER::GetMangleName(op));
            if (mangledNewName == mangledOrgName) {
              MYPARSER::Report (L"Operation already defined", @4);
            }
            else {
              // TODO: possibly memory leak here
              // Pull out existing op, mangle its name, reinsert
              $1->RemElem(orgName);
              $1->Insert(mangledOrgName, op);

              // Mangle this name, insert
              $1->Insert(mangledNewName, opd);
              VDMPARSER::mangledOpNames.Insert(orgName);
            }
          }
          else if ($1->DomExists (mangledNewName)) {
            MYPARSER::Report (L"Operation already defined", @4);
          }
          else if (VDMPARSER::mangledOpNames.InSet(orgName)) {
            // This name has already been mangled once
            $1->Insert(mangledNewName, opd);
            VDMPARSER::mangledOpNames.Insert(orgName);
          }
          else {
            $1->Insert (orgName, opd);
          }

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_OpDef, mangledNewName);

          delete $3;
          delete $4;
        }
        ;
#endif // VDMPP

OperationDefinition
        : ImplOperationDefinition
        | ExplOperationDefinition
        | ExtExplOperationDefinition
#ifdef VDMSL
        | LEX_PURE ImplOperationDefinition
        {
          $$ = $2;
          TYPE_AS_ImplOpDef opdef ($2->GetRecord (2));
          opdef.SetField(pos_AS_ImplOpDef_oppure, Bool(true));
          $$->SetField(2, opdef);
        }
        | LEX_PURE ExplOperationDefinition
        {
          $$ = $2;
          TYPE_AS_ExplOpDef opdef ($2->GetRecord (2));
          opdef.SetField(pos_AS_ExplOpDef_oppure, Bool(true));
          $$->SetField(2, opdef);
        }
        | LEX_PURE ExtExplOperationDefinition
        {
          $$ = $2;
          TYPE_AS_ExtExplOpDef opdef ($2->GetRecord (2));
          opdef.SetField(pos_AS_ExtExplOpDef_oppure, Bool(true));
          $$->SetField(2, opdef);
        }
#endif // VDMSL
#ifdef VDMPP
        | OperationAccess ImplOperationDefinition
        {
          $$ = $2;
          TYPE_AS_ImplOpDef opdef ($2->GetRecord(2));
          opdef.SetField (pos_AS_ImplOpDef_access, $1->GetField(1));
          opdef.SetField (pos_AS_ImplOpDef_stat,   $1->GetBool(2));
          opdef.SetField (pos_AS_ImplOpDef_opsync, $1->GetBool(3));
          opdef.SetField (pos_AS_ImplOpDef_oppure, $1->GetBool(4));
          $$->SetField(2, opdef);
          delete $1;
        }
        | OperationAccess ExplOperationDefinition
        {
          $$ = $2;
          TYPE_AS_ExplOpDef opdef ($2->GetRecord(2));
          opdef.SetField (pos_AS_ExplOpDef_access, $1->GetField(1));
          opdef.SetField (pos_AS_ExplOpDef_stat,   $1->GetBool(2));
          opdef.SetField (pos_AS_ExplOpDef_opsync, $1->GetBool(3));
          opdef.SetField (pos_AS_ExplOpDef_oppure, $1->GetBool(4));
          $$->SetField(2, opdef);
          delete $1;
        }
        | OperationAccess ExtExplOperationDefinition
        {
          $$ = $2;
          TYPE_AS_ExtExplOpDef opdef ($2->GetRecord(2));
          opdef.SetField (pos_AS_ExtExplOpDef_access, $1->GetField(1));
          opdef.SetField (pos_AS_ExtExplOpDef_stat,   $1->GetBool(2));
          opdef.SetField (pos_AS_ExtExplOpDef_opsync, $1->GetBool(3));
          opdef.SetField (pos_AS_ExtExplOpDef_oppure, $1->GetBool(4));
          $$->SetField(2, opdef);
          delete $1;
        }
#endif // VDMPP
        ;

ImplOperationDefinition
        : FnOpNameIdentifier ParameterTypes IdentifierTypePairList Externals Pre Post Exceptions
        { $$ = new Tuple (2); // (AS`Name * AS`ImplOpDef)
          TYPE_AS_ImplOpDef iod;
          MYPARSER::SetPos2(iod, @1, @7);
          iod.SetField (pos_AS_ImplOpDef_nm,       *$1);
          iod.SetField (pos_AS_ImplOpDef_oppure,   Bool(false));
          iod.SetField (pos_AS_ImplOpDef_opsync,   Bool(true));
          iod.SetField (pos_AS_ImplOpDef_partps,   *$2);
          iod.SetField (pos_AS_ImplOpDef_resnmtps, *$3);
          iod.SetField (pos_AS_ImplOpDef_opext,    *$4);
          iod.SetField (pos_AS_ImplOpDef_oppre,    *$5);
          iod.SetField (pos_AS_ImplOpDef_oppost,   *$6);
          iod.SetField (pos_AS_ImplOpDef_excps,    *$7);
          iod.SetField (pos_AS_ImplOpDef_access,   Int (DEFAULT_AS));
          iod.SetField (pos_AS_ImplOpDef_stat,     Bool(false));
#if VDMPP
          if (!VDMPARSER::currentClassMod.IsNil() && (*$1 == VDMPARSER::currentClassMod))
            iod.SetField (pos_AS_ImplOpDef_constr, Bool(true));
          else
#endif //VDMPP
            iod.SetField (pos_AS_ImplOpDef_constr, Bool(false));

          $$->SetField (1, *$1);
          $$->SetField (2, iod);

          delete $1;
          delete $2;
          delete $3;
          delete $4;
          delete $5;
          delete $6;
          delete $7;
        }
        ;

ExplOperationDefinition
        : FnOpNameIdentifier ':' OperationType FnOpNameIdentifier Parameters LEX_IS_DEFINED_AS OpBody Externals PrePost Exceptions
        { $$ = new Tuple (2); // (AS`Name * AS`ExplOpDef)

          if (! (*$1 == *$4))
            MYPARSER::Report (L"Identifiers in operation heading should be identical.", @1, @4);

          TYPE_AS_ExplOpDef eod;
          MYPARSER::SetPos2(eod, @1, @8);
          eod.SetField (pos_AS_ExplOpDef_nm,     *$1);
          eod.SetField (pos_AS_ExplOpDef_oppure, Bool(false));
          eod.SetField (pos_AS_ExplOpDef_opsync, Bool(true));
          eod.SetField (pos_AS_ExplOpDef_tp,     *$3);
          eod.SetField (pos_AS_ExplOpDef_parms,  *$5);
          eod.SetField (pos_AS_ExplOpDef_body,   *$7);
          //eod.SetField (pos_AS_ExplOpDef_opext, *$8); // not yet
          eod.SetField (pos_AS_ExplOpDef_oppre,  $9->GetField (1));
          eod.SetField (pos_AS_ExplOpDef_oppost, $9->GetField (2));
          //eod.SetField (pos_AS_ExplOpDef_excps, *$10); // not yet
          eod.SetField (pos_AS_ExplOpDef_access, Int (DEFAULT_AS));
          eod.SetField (pos_AS_ExplOpDef_stat,   Bool(false));
#if VDMPP
          if (!VDMPARSER::currentClassMod.IsNil() && (*$1 == VDMPARSER::currentClassMod))
            eod.SetField (pos_AS_ExplOpDef_constr, Bool(true));
          else
#endif //VDMPP
            eod.SetField (pos_AS_ExplOpDef_constr, Bool(false));

          $$->SetField (1, *$1);
          $$->SetField (2, eod);

          delete $1;
          delete $3;
          delete $4;
          delete $5;
          delete $7;
          delete $8;
          delete $9;
          delete $10;
        }
        ;

ExtExplOperationDefinition
        : FnOpNameIdentifier ParameterTypes IdentifierTypePairList LEX_IS_DEFINED_AS OpBody Externals PrePost Exceptions
        { $$ = new Tuple (2); // (AS`Name * AS`ExtExplOpDef)

          TYPE_AS_ExtExplOpDef eeod;
          MYPARSER::SetPos2(eeod, @1, @8);
          eeod.SetField (pos_AS_ExtExplOpDef_nm,       *$1);
          eeod.SetField (pos_AS_ExtExplOpDef_oppure,   Bool(false));
          eeod.SetField (pos_AS_ExtExplOpDef_opsync,   Bool(true));
          eeod.SetField (pos_AS_ExtExplOpDef_partps,   *$2);
          eeod.SetField (pos_AS_ExtExplOpDef_resnmtps, *$3);
          eeod.SetField (pos_AS_ExtExplOpDef_body,     *$5);
          eeod.SetField (pos_AS_ExtExplOpDef_opext,    *$6);
          eeod.SetField (pos_AS_ExtExplOpDef_oppre,    $7->GetField (1));
          eeod.SetField (pos_AS_ExtExplOpDef_oppost,   $7->GetField (2));
          eeod.SetField (pos_AS_ExtExplOpDef_excps,    *$8);
          eeod.SetField (pos_AS_ExtExplOpDef_access,   Int (DEFAULT_AS));
          eeod.SetField (pos_AS_ExtExplOpDef_stat,     Bool(false));
#if VDMPP
          if (!VDMPARSER::currentClassMod.IsNil() && (*$1 == VDMPARSER::currentClassMod))
            eeod.SetField (pos_AS_ExtExplOpDef_constr, Bool(true));
          else
#endif //VDMPP
            eeod.SetField (pos_AS_ExtExplOpDef_constr, Bool(false));

          $$->SetField (1, *$1);
          $$->SetField (2, eeod);

          delete $1;
          delete $2;
          delete $3;
          delete $5;
          delete $6;
          delete $7;
          delete $8;
        }
        ;

OpRngType
        : Type
        | '(' ')'
        {
          $$ = new TYPE_AS_VoidType();
          MYPARSER::SetPos2(*$$, @1, @2);
        }
        ;

OpDomType
        : Type
        { $$ = new Sequence ();
          if ($1->Is(TAG_TYPE_AS_ProductType))
            $$->ImpConc($1->GetSequence(pos_AS_ProductType_tps));
          else
            $$->ImpAppend (*$1);

          delete $1;
        }
        | '(' ')'
        { $$ = new Sequence ();
        }
        ;

OperationType
        : OpDomType LEX_OPERATION_ARROW OpRngType
        {
          $$ = new TYPE_AS_OpType();
          MYPARSER::SetPos3(*$$, @1, @2, @3);

          $$->SetField (pos_AS_OpType_opdom, *$1);
          $$->SetField (pos_AS_OpType_oprng, *$3);

          delete $1;
          delete $3;
        }
        ;

Externals
        : /* empty */
        { $$ = new Sequence ();
        }
        | LEX_EXT ListOfVarInformation
        { $$ = $2;
        }
        ;

ListOfVarInformation
        : VarInformation
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfVarInformation VarInformation
        { $1->ImpAppend (*$2);
          delete $2;
        }
        ;

VarInformation
        : Mode NameList
        { $$ = new TYPE_AS_ExtInf();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_ExtInf_mode, *$1);      /* mode */
          $$->SetField (pos_AS_ExtInf_vars, *$2);      /* vars */
          $$->SetField (pos_AS_ExtInf_tp,   Nil ());   /* type */
          delete $1;
          delete $2;
        }
        | Mode NameList ':' Type
        { $$ = new TYPE_AS_ExtInf();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_ExtInf_mode, *$1);      /* mode */
          $$->SetField (pos_AS_ExtInf_vars, *$2);      /* vars */
          $$->SetField (pos_AS_ExtInf_tp,   *$4);      /* type */
          delete $1;
          delete $2;
          delete $4;
        }
        ;

Mode
        : LEX_RD
        { $$ = new Int (READ);
        }
        | LEX_WR
        { $$ = new Int (READWRITE);
        }
        ;

/** Section in reference document: 9.6.14 Names *******************************/
/** Deviations from the standard/Remarks: IPTES                              **/

NameList
        : Name
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | NameList ',' Name
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

/** Section in reference document: 9.2.5 Exceptions ***************************/
/** Deviations from the standard/Remarks: NONE                               **/
Exceptions
        : /* empty */
        { $$ = new Sequence ();
        }
        | LEX_ERRS ListOfErrors
        { $$ = $2;
        }
        ;

ListOfErrors
        : Identifier ':' Expression LEX_ARROW Expression
        { $$ = new Sequence ();

          TYPE_AS_Error e;
          MYPARSER::SetPos2(e, @1, @5);
          e.SetField (pos_AS_Error_nm,     *$1);
          e.SetField (pos_AS_Error_cond,   *$3);
          e.SetField (pos_AS_Error_action, *$5);
          $$->ImpAppend (e);
          delete $1;
          delete $3;
          delete $5;
        }
        | ListOfErrors Identifier ':' Expression LEX_ARROW Expression
        { TYPE_AS_Error e;
          MYPARSER::SetPos2(e, @2, @6);
          e.SetField (pos_AS_Error_nm,     *$2);
          e.SetField (pos_AS_Error_cond,   *$4);
          e.SetField (pos_AS_Error_action, *$6);
          $1->ImpAppend (e);
          delete $2;
          delete $4;
          delete $6;
        }
        ;

DclStatementList
        : /* empty */
        { $$ = new Sequence ();
        }
        | DclStatementList DclStatement
        { $1->ImpConc (*$2);
          delete $2;
        }
        ;

AssignmentDefinitionList
        : AssignmentDefinition
        { $$ = new Sequence();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | AssignmentDefinitionList ',' AssignmentDefinition
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

AssignmentDefinition
        : Identifier ':' Type
        { $$ = new TYPE_AS_AssignDef();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_AssignDef_var,     *$1);
          $$->SetField (pos_AS_AssignDef_tp,      *$3);
          $$->SetField (pos_AS_AssignDef_dclinit, Nil ());
          delete $1;
          delete $3;
        }
        | Identifier ':' Type LEX_ASSIGN Expression
        { $$ = new TYPE_AS_AssignDef();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_AssignDef_var,     *$1);
          $$->SetField (pos_AS_AssignDef_tp,      *$3);
          $$->SetField (pos_AS_AssignDef_dclinit, *$5);
          delete $1;
          delete $3;
          delete $5;
        }
        ;

DclStatement
        : LEX_DCL AssignmentDefinitionList ';'
        { $$ = $2;
        }
        ;

#if VDMSL
/** Section in reference document: 9.4.2 State Definition *********************/
/** Deviations from the standard/Remarks: IPTES                              **/

StateDefinition
        : LEX_STATE Identifier LEX_OF FieldList LEX_END TexBreak
        { $$ = new TYPE_AS_StateDef();
          MYPARSER::SetPos2(*$$, @1, @6);

          TYPE_AS_CompositeType Co;
          MYPARSER::SetPos2(Co, @1, @5);
          Co.SetField (pos_AS_CompositeType_name,   *$2);
          Co.SetField (pos_AS_CompositeType_fields, *$4);

          $$->SetField (pos_AS_StateDef_tp,   Co);
          $$->SetField (pos_AS_StateDef_Inv,  Nil ());
          $$->SetField (pos_AS_StateDef_Init, Nil ());

          delete $2;
          delete $4;
          delete $6;
        }
        | LEX_STATE Identifier LEX_OF FieldList InvariantInitialization LEX_END TexBreak
        { $$ = new TYPE_AS_StateDef();
          MYPARSER::SetPos2(*$$, @1, @7);

          TYPE_AS_CompositeType Co;
          MYPARSER::SetPos2(Co, @1, @6);
          Co.SetField (pos_AS_CompositeType_name,   *$2);
          Co.SetField (pos_AS_CompositeType_fields, *$4);

          $$->SetField (pos_AS_StateDef_tp,   Co);
          $$->SetField (pos_AS_StateDef_Inv,  $5->GetField(1));
          $$->SetField (pos_AS_StateDef_Init, $5->GetField(2));

          delete $2;
          delete $4;
          delete $5;
          delete $7;
        }
        ;

InvariantInitialization
        : Invariant
        { $$ = new Tuple(2);
          $$->SetField(1, *$1);
          $$->SetField(2, Nil());
          delete $1;
        } 
        | Initialization
        { $$ = new Tuple(2);
          $$->SetField(1, Nil());
          $$->SetField(2, *$1);
          delete $1;
        } 
        | Invariant Initialization
        { $$ = new Tuple(2);
          $$->SetField(1, *$1);
          $$->SetField(2, *$2);
          delete $1;
          delete $2;
        } 
        | Initialization Invariant
        { $$ = new Tuple(2);
          $$->SetField(1, *$2);
          $$->SetField(2, *$1);
          delete $1;
          delete $2;
        } 
        ;

Initialization
        : LEX_INIT Pattern LEX_IS_DEFINED_AS Expression
        {
          $$ = new TYPE_AS_StateInit();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_StateInit_pat,  *$2);
          $$->SetField (pos_AS_StateInit_expr, *$4);
          delete $2;
          delete $4;
        }
        ;
#endif // VDMSL

InvariantEqualOrder
        : Invariant
        { $$ = new Tuple(3);
          $$->SetField(1, *$1);
          $$->SetField(2, Nil());
          $$->SetField(3, Nil());
          delete $1;
        } 
        | EqualOrder
        { $$ = new Tuple(3);
          $$->SetField(1, Nil());
          $$->SetField(2, $1->GetField(1));
          $$->SetField(3, $1->GetField(2));
          delete $1;
        } 
        | Invariant EqualOrder
        { $$ = new Tuple(3);
          $$->SetField(1, *$1);
          $$->SetField(2, $2->GetField(1));
          $$->SetField(3, $2->GetField(2));
          delete $1;
          delete $2;
        } 
        | EqualOrder Invariant
        { $$ = new Tuple(3);
          $$->SetField(1, *$2);
          $$->SetField(2, $1->GetField(1));
          $$->SetField(3, $1->GetField(2));
          delete $1;
          delete $2;
        }
        ;

EqualOrder
        : Equal
        { $$ = new Tuple(2);
          $$->SetField(1, *$1);
          $$->SetField(2, Nil());
          delete $1;
        } 
        | Order
        { $$ = new Tuple(2);
          $$->SetField(1, Nil());
          $$->SetField(2, *$1);
          delete $1;
        } 
        | Equal Order
        { $$ = new Tuple(2);
          $$->SetField(1, *$1);
          $$->SetField(2, *$2);
          delete $1;
          delete $2;
        } 
        | Order Equal
        { $$ = new Tuple(2);
          $$->SetField(1, *$2);
          $$->SetField(2, *$1);
          delete $1;
          delete $2;
        } 
        ;

Invariant
        : LEX_INV Pattern LEX_IS_DEFINED_AS Expression
        { $$ = new TYPE_AS_Invariant();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_Invariant_pat,  *$2);
          $$->SetField (pos_AS_Invariant_expr, *$4);
          delete $2;
          delete $4;
        }
        ;

Equal
        : LEX_EQ Pattern LEX_EQUAL Pattern LEX_IS_DEFINED_AS Expression
        { $$ = new TYPE_AS_Equal();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_Order_lhs,  *$2);
          $$->SetField (pos_AS_Order_rhs,  *$4);
          $$->SetField (pos_AS_Order_expr, *$6);
          delete $2;
          delete $4;
          delete $6;
        } 
        ;

Order
        : LEX_ORD Pattern LEX_LESS_THAN Pattern LEX_IS_DEFINED_AS Expression
        { $$ = new TYPE_AS_Order();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_Order_lhs,  *$2);
          $$->SetField (pos_AS_Order_rhs,  *$4);
          $$->SetField (pos_AS_Order_expr, *$6);
          delete $2;
          delete $4;
          delete $6;
        } 
        ;

/** Section in reference document: 9.5 Statements *****************************/
/** Deviations from the standard/Remarks:                                    **/

Statement
        : BlockStatement
        | AlwaysStatement
        | TrapStatement
        | RecursiveTrapStatement
        | AssignStatement
        | CallStatement
        | AtomicAssignStatement
        | ErrorStatement
        | NondeterministicStatement
        | ExitStatement
        | IdentityStatement
        | DefStatement
        | LetOrLetBeStatement
        | SequenceForLoop
        | SetForLoop
        | IndexForLoop
        | WhileLoop
        | IfStatement
        | CasesStatement
        | ReturnStatement
        | SpecificationStatement
        | AssertStatement
#if VDMPP
        | StartStatement
        | StartListStatement
        | StopStatement
        | StopListStatement
#if VICE
        | DurationStmt
        | CycleStmt
#endif //VICE
#endif //VDMPP
        ;

CallOrAssignStatement
        : AssignStatement
        | CallStatement
        ;

CallOrAssignStatementList
        : CallOrAssignStatement
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | CallOrAssignStatementList ';' CallOrAssignStatement
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

CallStatement
        : StateDesignator
        {
          if($1->Is(TAG_TYPE_AS_MapOrSeqRef))
          {
            // Revert State Designator to a call statement.
            $$ = new TYPE_AS_CallStmt();
            MYPARSER::SetPos2(*$$, @1, @1);

            TYPE_AS_MapOrSeqRef mosr (*$1);
            const TYPE_AS_StateDesignator & var (mosr.GetRecord(pos_AS_MapOrSeqRef_var));

            // The expression list
            // HACK: see MapOrSeqReference
            // mosr.GetField(pos_AS_MapOrSeqRef_var) corresponds to mosr.get_arg()
            // however, this cannot be used because of hack using
            // StateDesignator to parse both call statement and assign
            // statement.
            const Sequence & expr_l (mosr.GetSequence(pos_AS_MapOrSeqRef_arg));

            $$->SetField(pos_AS_CallStmt_args, expr_l);

            switch(var.GetTag()) {
              case TAG_TYPE_AS_Name: {
                $$->SetField(pos_AS_CallStmt_oprt, var);
                $$->SetField(pos_AS_CallStmt_obj, Nil());
                break;
              }
              case TAG_TYPE_AS_FieldRef: {
                const TYPE_AS_Name & selector (var.GetField(pos_AS_FieldRef_sel));
#ifdef VDMPP
                const TYPE_AS_StateDesignator & sd (var.GetField(pos_AS_FieldRef_var));
#endif // VDMPP
                $$->SetField(pos_AS_CallStmt_oprt, selector);
#ifdef VDMSL
                $$->SetField(pos_AS_CallStmt_obj,  Nil());
#endif // VDMSL
#ifdef VDMPP
                $$->SetField(pos_AS_CallStmt_obj,  MYPARSER::StateDesignator2Expr(sd));
#endif // VDMPP
                break;
              }
              default: {
                MYPARSER::Report(L"Call statement must call an operation");
                break;
              }
            }
            delete $1;
          }
          else
            MYPARSER::Report(L"Call Statement must have an argument list", @1);
        }
        ;

AssignStatement
        : StateDesignator LEX_ASSIGN Expression
        { // Check that the state designator does not have a new expression.
          // and that the expression list has length < 2
          Tuple psdias (MYPARSER::ProperSDInAsgnStmt(*$1));
          if (psdias.GetBoolValue(1)) {
            $$ = new TYPE_AS_AssignStmt();
            MYPARSER::SetPos2(*$$, @1, @3);
            $$->SetField (pos_AS_AssignStmt_lhs, psdias.GetRecord(3));
            $$->SetField (pos_AS_AssignStmt_rhs, *$3);
            delete $1;
            delete $3;
          }
          else {
            MYPARSER::Report(psdias.GetSequence(2).GetString().c_str(), @1);
          }
        }
        ;

AtomicAssignStatement
        : LEX_ATOMIC '(' CallOrAssignStatementList OptSemi ')'
        { $$ = new TYPE_AS_AtomicAssignStmt ();
          MYPARSER::SetPos2 (*$$, @1, @5);
          if ($3->Length() < 2)
            MYPARSER::Report (L"Atomic Assign statement must have at least two statements.", @3);
          $$->SetField (pos_AS_AtomicAssignStmt_atm, *$3);
          delete $3;
        }
        ;

AlwaysStatement
        : LEX_ALWAYS Statement LEX_IN Statement
        {
          $$ = new TYPE_AS_AlwaysStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_AlwaysStmt_Post, *$2);
          $$->SetField (pos_AS_AlwaysStmt_body, *$4);
          delete $2;
          delete $4;
        }
        ;

TrapStatement
        : LEX_TRAP PatternBind LEX_WITH Statement LEX_IN Statement
        {
          $$ = new TYPE_AS_TrapStmt();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_TrapStmt_pat, *$2);
          $$->SetField (pos_AS_TrapStmt_Post, *$4);
          $$->SetField (pos_AS_TrapStmt_body, *$6);
          delete $2;
          delete $4;
          delete $6;
        }
        ;

RecursiveTrapStatement
        : LEX_TIXE Traps LEX_IN Statement
        {
          $$ = new TYPE_AS_RecTrapStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_RecTrapStmt_traps, *$2);
          $$->SetField (pos_AS_RecTrapStmt_body, *$4);
          delete $2;
          delete $4;
        }
        ;

Traps
        : '{' ListOfPatternBindStatements '}'
        { $$ = $2;
        }
        ;

ListOfPatternBindStatements
        : PatternBindStatement
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfPatternBindStatements ',' PatternBindStatement
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

PatternBindStatement
        : PatternBind LEX_BAR_ARROW Statement
        { $$ = new TYPE_AS_Trap();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Trap_match,    *$1);
          $$->SetField (pos_AS_Trap_trappost, *$3);
          delete $1;
          delete $3;
        }
        ;

BlockStatement
        : '(' DclStatementList SequenceOfStatements OptSemi ')'
        {  /* Should be () parenthesised. MIAN */
          $$ = new TYPE_AS_BlockStmt();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_BlockStmt_dcls,  *$2);
          $$->SetField (pos_AS_BlockStmt_stmts, *$3);
          delete $2;
          delete $3;
        }
        | '(' DclStatementList OptSemi ')'
        {
          $$ = new TYPE_AS_BlockStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_BlockStmt_dcls,  *$2);
          $$->SetField (pos_AS_BlockStmt_stmts, Sequence());
          delete $2;
        }
        ;

SequenceOfStatements
        : Statement
        { $$ = new Sequence();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | SequenceOfStatements ';' Statement
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

DefStatement
        : LEX_DEF ListOfEqualsDefinitions OptSemi LEX_IN Statement
        {
          $$ = new TYPE_AS_DefStmt();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_DefStmt_value, *$2);
          $$->SetField (pos_AS_DefStmt_In,    *$5);
          delete $2;
          delete $5;
        }
        ;

ListOfEqualsDefinitions
        : EqualsDefinition
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfEqualsDefinitions ';' EqualsDefinition
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

EqualsDefinition
        : PatternBind LEX_EQUAL Expression
        { $$ = new Tuple (2);
          $$->SetField (1, *$1);
          $$->SetField (2, *$3);
          /* TBD: Routine `TypeCheck' must convert        */
          /* the expression in a call statement (when it  */
          /* should be a call statement). NP              */
          delete $1;
          delete $3;
        }
        ;

LetOrLetBeStatement
        : LEX_LET ListOfLocalDefinitions LEX_IN Statement
        {
          $$ = new TYPE_AS_LetStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_LetStmt_localdef, *$2);
          $$->SetField (pos_AS_LetStmt_In,       *$4);
          delete $2;
          delete $4;
        }
        | LEX_LET TypeBind LEX_IN Statement
        {
          $$ = new TYPE_AS_LetBeSTStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_LetBeSTStmt_bind, ASTAUX::BindToBindList(*$2));
          $$->SetField (pos_AS_LetBeSTStmt_St,   Nil());
          $$->SetField (pos_AS_LetBeSTStmt_In,   *$4);
          delete $2;
          delete $4;
        }
        | LEX_LET TypeBind LEX_BE LEX_ST Expression LEX_IN Statement
        {
          $$ = new TYPE_AS_LetBeSTStmt();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_LetBeSTStmt_bind, ASTAUX::BindToBindList(*$2));
          $$->SetField (pos_AS_LetBeSTStmt_St,   *$5);
          $$->SetField (pos_AS_LetBeSTStmt_In,   *$7);
          delete $2;
          delete $5;
          delete $7;
        }
        | LEX_LET BindList LEX_IN Statement
        {
          $$ = new TYPE_AS_LetBeSTStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_LetBeSTStmt_bind, *$2);
          $$->SetField (pos_AS_LetBeSTStmt_St,   Nil());
          $$->SetField (pos_AS_LetBeSTStmt_In,   *$4);
          delete $2;
          delete $4;
        }
        | LEX_LET BindList LEX_BE LEX_ST Expression LEX_IN Statement
        {
          $$ = new TYPE_AS_LetBeSTStmt();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_LetBeSTStmt_bind, *$2);
          $$->SetField (pos_AS_LetBeSTStmt_St,   *$5);
          $$->SetField (pos_AS_LetBeSTStmt_In,   *$7);
          delete $2;
          delete $5;
          delete $7;
        }
        ;

NondeterministicStatement
        : LEX_NONDET '(' ListOfStatements ')'
        {
          $$ = new TYPE_AS_NonDetStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_NonDetStmt_stmts, *$3);
          delete $3;
        }
        ;

ListOfStatements
        : Statement
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfStatements ',' Statement
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

SequenceForLoop
        : LEX_FOR PatternBind LEX_IN Expression LEX_DO Statement
        {
          $$ = new TYPE_AS_SeqForLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_SeqForLoopStmt_cv,   *$2);
          $$->SetField (pos_AS_SeqForLoopStmt_fseq, *$4);
          $$->SetField (pos_AS_SeqForLoopStmt_body, *$6);
          delete $2;
          delete $4;
          delete $6;
        }
        | LEX_FOR PatternBind LEX_IN_REVERSE Expression LEX_DO Statement
        {
          TYPE_AS_PrefixExpr pe;
          MYPARSER::SetPos2(pe, @3, @4);
          pe.SetField (pos_AS_PrefixExpr_opr, Int(SEQREVERSE));
          pe.SetField (pos_AS_PrefixExpr_arg, *$4);

          $$ = new TYPE_AS_SeqForLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_SeqForLoopStmt_cv,   *$2);
          $$->SetField (pos_AS_SeqForLoopStmt_fseq, pe);
          $$->SetField (pos_AS_SeqForLoopStmt_body, *$6);
          delete $2;
          delete $4;
          delete $6;
        }
/*
        | LEX_FOR PatternBind LEX_IN LEX_REVERSE Expression LEX_DO Statement
        {
          TYPE_AS_PrefixExpr pe;
          MYPARSER::SetPos2(pe, @4, @5);
          pe.SetField (pos_AS_PrefixExpr_opr, Int(SEQREVERSE));
          pe.SetField (pos_AS_PrefixExpr_arg, *$5);

          $$ = new TYPE_AS_SeqForLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_SeqForLoopStmt_cv,   *$2);
          $$->SetField (pos_AS_SeqForLoopStmt_fseq, pe);
          $$->SetField (pos_AS_SeqForLoopStmt_body, *$7);
          delete $2;
          delete $5;
          delete $7;
        }
*/
        ;

SetForLoop
        : LEX_FOR LEX_ALL Pattern LEX_IN_SET Expression LEX_DO Statement
        {
          $$ = new TYPE_AS_SetForLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_SetForLoopStmt_cv,   *$3);
          $$->SetField (pos_AS_SetForLoopStmt_fset, *$5);
          $$->SetField (pos_AS_SetForLoopStmt_body, *$7);
          delete $3;
          delete $5;
          delete $7;
        }
        ;

IndexForLoop
        : LEX_FOR Identifier LEX_EQUAL Expression LEX_TO Expression LEX_DO Statement
        {
          $$ = new TYPE_AS_IndexForLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @8);
          $$->SetField (pos_AS_IndexForLoopStmt_cv,   *$2);
          $$->SetField (pos_AS_IndexForLoopStmt_lb,   *$4);
          $$->SetField (pos_AS_IndexForLoopStmt_ub,   *$6);
          $$->SetField (pos_AS_IndexForLoopStmt_By,   Nil());
          $$->SetField (pos_AS_IndexForLoopStmt_body, *$8);
          delete $2;
          delete $4;
          delete $6;
          delete $8;
        }
        | LEX_FOR Identifier LEX_EQUAL Expression LEX_TO Expression LEX_BY Expression LEX_DO  Statement
        {
          $$ = new TYPE_AS_IndexForLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @10);
          $$->SetField (pos_AS_IndexForLoopStmt_cv,   *$2);
          $$->SetField (pos_AS_IndexForLoopStmt_lb,   *$4);
          $$->SetField (pos_AS_IndexForLoopStmt_ub,   *$6);
          $$->SetField (pos_AS_IndexForLoopStmt_By,   *$8);
          $$->SetField (pos_AS_IndexForLoopStmt_body, *$10);
          delete $2;
          delete $4;
          delete $6;
          delete $8;
          delete $10;
        }
        ;

WhileLoop
        : LEX_WHILE Expression LEX_DO Statement
        {
          $$ = new TYPE_AS_WhileLoopStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_WhileLoopStmt_test, *$2);
          $$->SetField (pos_AS_WhileLoopStmt_body, *$4);
          delete $2;
          delete $4;
        }
        ;

IfStatement
        : LEX_IF Expression LEX_THEN Statement
        {
          $$ = new TYPE_AS_IfStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_IfStmt_test,  *$2);
          $$->SetField (pos_AS_IfStmt_cons,  *$4);
          $$->SetField (pos_AS_IfStmt_elsif, Sequence());
          $$->SetField (pos_AS_IfStmt_altn,  Nil());
          delete $2;
          delete $4;
        }
        | LEX_IF Expression LEX_THEN Statement ElseStatement
        {
          $$ = new TYPE_AS_IfStmt();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_IfStmt_test,  *$2);
          $$->SetField (pos_AS_IfStmt_cons,  *$4);
          $$->SetField (pos_AS_IfStmt_elsif, Sequence());
          $$->SetField (pos_AS_IfStmt_altn,  *$5);
          delete $2;
          delete $4;
          delete $5;
        }
        | LEX_IF Expression LEX_THEN Statement ListOfElsifStatements
        {
          $$ = new TYPE_AS_IfStmt();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_IfStmt_test,  *$2);
          $$->SetField (pos_AS_IfStmt_cons,  *$4);
          $$->SetField (pos_AS_IfStmt_elsif, *$5);
          $$->SetField (pos_AS_IfStmt_altn,  Nil());
          delete $2;
          delete $4;
          delete $5;
        }
        | LEX_IF Expression LEX_THEN Statement ListOfElsifStatements ElseStatement
        {
          $$ = new TYPE_AS_IfStmt();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_IfStmt_test,  *$2);
          $$->SetField (pos_AS_IfStmt_cons,  *$4);
          $$->SetField (pos_AS_IfStmt_elsif, *$5);
          $$->SetField (pos_AS_IfStmt_altn,  *$6);
          delete $2;
          delete $4;
          delete $5;
          delete $6;
        }
        ;

ListOfElsifStatements
        : LEX_ELSEIF Expression LEX_THEN Statement
        {
          $$ = new Sequence ();
          TYPE_AS_ElseifStmt EIS;
          MYPARSER::SetPos3(EIS, @1, @2, @4);
          EIS.SetField (pos_AS_ElseifStmt_test, *$2);
          EIS.SetField (pos_AS_ElseifStmt_cons, *$4);
          $$->ImpAppend (EIS);
          delete $2;
          delete $4;
        }
        | ListOfElsifStatements LEX_ELSEIF Expression LEX_THEN Statement
        {
          TYPE_AS_ElseifStmt EIS;
          MYPARSER::SetPos3(EIS, @2, @2, @5);
          EIS.SetField (pos_AS_ElseifStmt_test, *$3);
          EIS.SetField (pos_AS_ElseifStmt_cons, *$5);
          $1->ImpAppend (EIS);
          delete $3;
          delete $5;
        }
        ;

ElseStatement
        : LEX_ELSE Statement
        {
          $$ = $2;
        }
        ;

CasesStatement
        : LEX_CASES Expression ':' CasesStatementAlternatives OthersStatement LEX_END
        {
           $$ = new TYPE_AS_CasesStmt();
           MYPARSER::SetPos2(*$$, @1, @6);
           $$->SetField (pos_AS_CasesStmt_sel,    *$2);
           $$->SetField (pos_AS_CasesStmt_altns,  *$4);
           $$->SetField (pos_AS_CasesStmt_Others, *$5);
           delete $2;
           delete $4;
           delete $5;
        }
        ;

CasesStatementAlternatives
        : CasesStatementAlternative
        {
          $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | CasesStatementAlternatives ',' CasesStatementAlternative
        {
          $1->ImpAppend (*$3);
          delete $3;
        }
        ;

CasesStatementAlternative
        : PatternList LEX_ARROW Statement
//        : PatternCondList LEX_ARROW Statement
        {
          $$ = new TYPE_AS_CasesStmtAltn();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_CasesStmtAltn_match, *$1);
          $$->SetField (pos_AS_CasesStmtAltn_body,  *$3);
          delete $1;
          delete $3;
        }
        ;

OthersStatement
        :
        { $$ = new Generic(Nil());
        }
        | ',' LEX_OTHERS LEX_ARROW Statement
        {
          $$ = new Generic(*$4);
          delete $4;
        }
        ;

ReturnStatement
        : LEX_RETURN Expression
        {
          $$ = new TYPE_AS_ReturnStmt();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_ReturnStmt_val, *$2);
          delete $2;
        }
        | LEX_RETURN
        {
          $$ = new TYPE_AS_ReturnStmt();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_ReturnStmt_val, Nil ());
        }
        ;

ErrorStatement
        : LEX_ERROR
        {
          $$ = new TYPE_AS_ErrorStmt();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
        }
        ;

ExitStatement
        : LEX_EXIT
        {
          $$ = new TYPE_AS_ExitStmt();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_ExitStmt_expr, Nil());
        }
        | LEX_EXIT Expression
        {
          $$ = new TYPE_AS_ExitStmt();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_ExitStmt_expr, *$2);
          delete $2;
        }
        ;

IdentityStatement
        : LEX_SKIP
        {
          $$ = new TYPE_AS_IdentStmt();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;

#if VDMPP
StartStatement:
        LEX_START '(' Expression ')'
        {
          $$ = new TYPE_AS_StartStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_StartStmt_expr, *$3);
          delete $3;
        }
        ;

StartListStatement:
        LEX_STARTLIST '(' Expression ')'
        {
          $$ = new TYPE_AS_StartListStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_StartListStmt_expr, *$3);
          delete $3;
        }
        ;

StopStatement:
        LEX_STOP '(' Expression ')'
        {
          $$ = new TYPE_AS_StopStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_StopStmt_expr, *$3);
          delete $3;
        }
        ;

StopListStatement:
        LEX_STOPLIST '(' Expression ')'
        {
          $$ = new TYPE_AS_StopListStmt();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_StopListStmt_expr, *$3);
          delete $3;
        }
        ;

#if VICE
DurationStmt:
       LEX_DURATION '(' Expression ')' Statement
       {
          $$ = new TYPE_AS_DurationStmt();
          MYPARSER::SetPos2(*$$,@1,@5);
          $$->SetField(pos_AS_DurationStmt_num,  *$3);
          $$->SetField(pos_AS_DurationStmt_stmt, *$5);
          delete $3;
          delete $5;
       }
       ;

CycleStmt:
       LEX_CYCLES '(' Expression ')' Statement
       {
          $$ = new TYPE_AS_CycleStmt();
          MYPARSER::SetPos2(*$$,@1,@5);
          $$->SetField(pos_AS_CycleStmt_num,  *$3);
          $$->SetField(pos_AS_CycleStmt_stmt, *$5);
          delete $3;
          delete $5;
       }
       ;
#endif // VICE
#endif // VDMPP

SpecificationStatement
        : '[' Externals Pre Post Exceptions ']'
        {
          $$ = new TYPE_AS_SpecificationStmt();
          MYPARSER::SetPos2(*$$, @1, @6);

          $$->SetField (pos_AS_SpecificationStmt_opext,  *$2);
          $$->SetField (pos_AS_SpecificationStmt_oppre,  *$3);
          $$->SetField (pos_AS_SpecificationStmt_oppost, *$4);
          $$->SetField (pos_AS_SpecificationStmt_excps,  *$5);
          delete $2;
          delete $3;
          delete $4;
          delete $5;
        }
        ;

AssertStatement
       : LEX_ASSERT '(' Expression ')'
       {
          $$ = new TYPE_AS_AssertStmt();
          MYPARSER::SetPos2(*$$,@1,@4);
          $$->SetField(pos_AS_AssertStmt_expr, *$3);
          delete $3;
       }
       ;

/** Section in reference document: 9.7 State Designators **********************/
/** Deviations from the standard/Remarks: IPTES                              **/

StateDesignator
        : NameOrNarrowRef ListOfStateDesignatorQualifiers
        {
          TYPE_AS_StateDesignator sd (*$1); //
          size_t len_sd = $2->Length();
          for(size_t idx = 1; idx <= len_sd; idx++)
          {
            TYPE_AS_StateDesignator frmose ($2->Index(idx));
            switch (frmose.GetTag()) {
              case TAG_TYPE_AS_FieldRef: {
                frmose.SetField(pos_AS_FieldRef_var, sd);
                sd = frmose;
                break;
              }
              case TAG_TYPE_AS_MapOrSeqRef: {
                frmose.SetField(pos_AS_MapOrSeqRef_var, sd);
                sd = frmose;
                break;
              }
              default: {
                MYPARSER::Report (L"Unknown state designator component.", @2);
                break;
              }
            }
          }
          $$ = new TYPE_AS_StateDesignator (sd);
          delete $1;
          delete $2;
        }
#ifdef VDMPP
        | SelfExpression FieldReference
        {
          Generic sd = *$1;
          size_t len_sd = $2->Length();
          for(size_t idx = 1; idx <= len_sd; idx++)
          {
            TYPE_AS_StateDesignator frmose ($2->Index(idx));
            switch (frmose.GetTag()) {
              case TAG_TYPE_AS_FieldRef: {
                frmose.SetField(pos_AS_FieldRef_var, sd);
                sd = frmose;
                break;
              }
              case TAG_TYPE_AS_MapOrSeqRef: {
                frmose.SetField(pos_AS_MapOrSeqRef_var, sd);
                sd = frmose;
                break;
              }
              default: {
                MYPARSER::Report (L"Unknown state designator component.", @2);
                break;
              }
            }
          }
          $$ = new TYPE_AS_StateDesignator (sd);
          delete $1;
          delete $2;
        }
#endif // VDMPP
        ;

NameOrNarrowRef
        : Name
        | NarrowReference
#ifdef VDMPP
        | NewExpression
#endif // VDMPP
        | Name '[' TypeList ']'  // 20111012
        {
          $$ = new TYPE_AS_FctTypeInstExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_FctTypeInstExpr_polyfct, *$1);
          $$->SetField (pos_AS_FctTypeInstExpr_inst,    *$3);
          delete $1;
          delete $3;
        }
        ;

ListOfStateDesignatorQualifiers
        : /* empty */
        { $$ = new Sequence;
        }
        | FieldReference
        | MapOrSeqReference
        ;

FieldReference
        : '.' Name ListOfStateDesignatorQualifiers
        {
          TYPE_AS_FieldRef fr ;
          MYPARSER::SetPos2(fr, @1, @3);

          TYPE_AS_Ids id_l ($2->GetField (pos_AS_Name_ids));
#ifdef VDMSL
          if (id_l.Length () != 1)
            MYPARSER::Report (L"Name references must be simple identifiers.", @3);
#endif //VDMSL
          fr.SetField (pos_AS_FieldRef_sel, *$2);
          $3->ImpPrepend (fr);
          $$ = $3;
          delete $2;
        }
        ;

MapOrSeqReference
        : '(' ExpressionList ')' ListOfStateDesignatorQualifiers
        {
          TYPE_AS_MapOrSeqRef mosr;
          MYPARSER::SetPos2(mosr, @1, @4);
          // HACK: see CallOrAssignStatement
          // Remark here we insert a sequence of expressions.
          // However, this is not type correct, since a
          // MapOrSequenceRef takes only one expression as
          // argument.
          mosr.SetField (pos_AS_MapOrSeqRef_arg, *$2);
          $4->ImpPrepend (mosr);
          $$ = $4;
          delete $2;
        }
        ;

NarrowReference
        //: LEX_NARROW_ '(' Expression ',' Type ')'
        : LEX_NARROW_ '(' StateDesignator ',' Type ')'
        {
          $$ = new TYPE_AS_NarrowRef();
          MYPARSER::SetPos2(*$$, @1, @6);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_NarrowRef_var, *$3);
          $$->SetField (pos_AS_NarrowRef_type, *$5);
          delete $3;
          delete $5;
        }
        ;
#endif //FULL

ExpressionList
        : /* empty */
        { $$ = new Sequence ();
        }
        | NonEmptyExpressionList
        ;

NonEmptyExpressionList
        : Expression
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | NonEmptyExpressionList ',' Expression
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

Expression
        : BracketedExpression
        | ComplexExpression
        | UnaryExpression
        | BinaryExpression
        | QuantifiedExpression
        | IotaExpression
        | TokenExpression
        | LambdaExpression
        | FunctionTypeInstantiation
        | SetExpression
        | SequenceExpression
        | MapExpression
        | TupleExpression
        | RecordExpression
        | ApplyExpression
        | PreCondApply
        | SimpleExpression
        | UndefinedExpression
        | SymbolicLiteral
        | OldNameOrName
#if VDMPP
        | SelfExpression
        | ThreadIdExpression
#if VICE
        | CurTimeExpression
#endif // VICE
        | NewExpression
#if FULL
        | PermissionExpression
#endif //FULL
#endif //VDMPP
#if !FULL
        | LastResult
#endif //FULL
        | NarrowExpression
        | MacroExpression
        ;

#if !FULL
LastResult
        : LEX_LAST_RESULT
        {
          $$ = new TYPE_AS_LastRes();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;
#endif //FULL

#if VDMPP
SelfExpression
        : LEX_SELF
        { $$ = new TYPE_AS_SelfExpr();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;

ThreadIdExpression
        : LEX_THREADID
        { $$ = new TYPE_AS_ThreadIdExpr();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;

#ifdef VICE
CurTimeExpression
        : LEX_CURTIME
        { $$ = new TYPE_AS_CurTimeExpr();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;
#endif // VICE

NewExpression
        : LEX_NEW Name '(' ExpressionList ')'
        {
          $$ = new TYPE_AS_NewExpr();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField(pos_AS_NewExpr_cls,  *$2);
          $$->SetField(pos_AS_NewExpr_args, *$4);
          delete $2;
          delete $4;
        }
        ;
#endif //VDMPP
#if FULL && VDMPP
PermissionExpression
        : LEX_ACT '(' NameList ')'
        {
          $$ = new TYPE_AS_ActExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_ActExpr_mthd, *$3);
          delete $3;
        }
        | LEX_FIN '(' NameList ')'
        {
          $$ = new TYPE_AS_FinExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_FinExpr_mthd, *$3);
          delete $3;
        }
        | LEX_ACTIVE '(' NameList ')'
        {
          $$ = new TYPE_AS_ActiveExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_ActiveExpr_mthd, *$3);
          delete $3;
        }
        | LEX_WAITING '(' NameList ')'
        {
          $$ = new TYPE_AS_WaitingExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_WaitingExpr_mthd, *$3);
          delete $3;
        }
        | LEX_REQ '(' NameList ')'
        {
          $$ = new TYPE_AS_ReqExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField(pos_AS_ReqExpr_mthd, *$3);
          delete $3;
        }
        ;
#endif //FULL && VDMPP

BracketedExpression
        : '(' Expression ')'
        {
          $$ = new TYPE_AS_BracketedExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField(pos_AS_BracketedExpr_expr, *$2);
          delete $2;
        }
        ;

/** Section in reference document: 9.6.1 Complex Expressions ******************/
/** Deviations from the standard/Remarks: IPTES                              **/

ComplexExpression
        : LetOrLetBeExpression
        | DefExpression
        | IfExpression
        | CasesExpression
        ;

DefExpression
        : LEX_DEF ListOfDefPatternBinds OptSemi LEX_IN Expression
        {
          $$ = new TYPE_AS_DefExpr();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_DefExpr_Def, *$2);
          $$->SetField (pos_AS_DefExpr_In,  *$5);
          delete $2;
          delete $5;
        }
        ;

ListOfDefPatternBinds
        : DefPatternBind
        { $$ = new Sequence ();
          $$->ImpAppend(*$1);
          delete $1;
        }
        | ListOfDefPatternBinds ';' DefPatternBind
        { $1->ImpAppend(*$3);
          delete $3;
        }
        ;

DefPatternBind
        : PatternBind LEX_EQUAL Expression
        { $$ = new Tuple (2);
          $$->SetField(1, *$1);
          $$->SetField(2, *$3);
          delete $1;
          delete $3;
        }
        ;

LetOrLetBeExpression
        : LEX_LET ListOfLocalDefinitions LEX_IN Expression
        {
          $$ = new TYPE_AS_LetExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_LetExpr_localdef, *$2);
          $$->SetField (pos_AS_LetExpr_body,     *$4);
          delete $2;
          delete $4;
        }
        | LEX_LET TypeBind LEX_IN Expression
        {
          $$ = new TYPE_AS_LetBeSTExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_LetBeSTExpr_lhs, ASTAUX::BindToBindList(*$2));
          $$->SetField (pos_AS_LetBeSTExpr_St,  Nil());
          $$->SetField (pos_AS_LetBeSTExpr_In,  *$4);
          delete $2;
          delete $4;
        }
        | LEX_LET TypeBind LEX_BE LEX_ST Expression LEX_IN Expression
        {
          $$ = new TYPE_AS_LetBeSTExpr();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_LetBeSTExpr_lhs, ASTAUX::BindToBindList(*$2));
          $$->SetField (pos_AS_LetBeSTExpr_St,  *$5);
          $$->SetField (pos_AS_LetBeSTExpr_In,  *$7);
          delete $2;
          delete $5;
          delete $7;
        }
        | LEX_LET BindList LEX_IN Expression
        {
          $$ = new TYPE_AS_LetBeSTExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_LetBeSTExpr_lhs, *$2);
          $$->SetField (pos_AS_LetBeSTExpr_St,  Nil());
          $$->SetField (pos_AS_LetBeSTExpr_In,  *$4);
          delete $2;
          delete $4;
        }
        | LEX_LET BindList LEX_BE LEX_ST Expression LEX_IN Expression
        {
          $$ = new TYPE_AS_LetBeSTExpr();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_LetBeSTExpr_lhs, *$2);
          $$->SetField (pos_AS_LetBeSTExpr_St,  *$5);
          $$->SetField (pos_AS_LetBeSTExpr_In,  *$7);
          delete $2;
          delete $5;
          delete $7;
        }
        ;

ListOfLocalDefinitions
        : LocalDefinition
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfLocalDefinitions ',' LocalDefinition
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

LocalDefinition
//        : LocalExtExplFunctionDefinition
//        | LocalImplFunctionDefinition
        : LocalImplFunctionDefinition
//        | LocalExplFunctionDefinition
        | ValueDefinition
        ;

ValueDefinition
        : Pattern LEX_EQUAL Expression
        { $$ = new TYPE_AS_ValueDef();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_ValueDef_pat,    *$1);
          $$->SetField (pos_AS_ValueDef_tp,     Nil());
          $$->SetField (pos_AS_ValueDef_val,    *$3);
          $$->SetField (pos_AS_ValueDef_access, Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ValueDef_stat,   Bool(true));
          delete $1;
          delete $3;
        }
        | Pattern ':' Type LEX_EQUAL Expression
        { $$ = new TYPE_AS_ValueDef();
          MYPARSER::SetPos2(*$$, @1, @5);
          if ($1->Is(TAG_TYPE_AS_PatternName)) {
            $1->SetField(pos_AS_PatternName_tp, *$3);
          }
          $$->SetField (pos_AS_ValueDef_pat,    *$1);
          $$->SetField (pos_AS_ValueDef_tp,     *$3);
          $$->SetField (pos_AS_ValueDef_val,    *$5);
          $$->SetField (pos_AS_ValueDef_access, Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ValueDef_stat,   Bool(true));
          delete $1;
          delete $3;
          delete $5;
        }
        | Pattern ':' FunctionType Identifier ParametersList LEX_IS_DEFINED_AS FnBody PrePost
        { $$ = new TYPE_AS_ExplFnDef();
          MYPARSER::SetPos2(*$$, @1, @8);

          if ($1->Is (TAG_TYPE_AS_PatternName))
          { if (! ($1->GetField (pos_AS_PatternName_nm) == *$4))
              MYPARSER::Report (L"Identifiers in function heading should be identical.",@1,@4);
            $$->SetField (pos_AS_ExplFnDef_nm, $1->GetField (pos_AS_PatternName_nm));
          }
          else
          { MYPARSER::Report (L"Local function identifiers must be simple identifiers.", @1);
            $$->SetField (pos_AS_ExplFnDef_nm, Record());
          }

          $$->SetField (pos_AS_ExplFnDef_tpparms, Sequence());
          $$->SetField (pos_AS_ExplFnDef_tp,      *$3);
          $$->SetField (pos_AS_ExplFnDef_parms,   *$5);
          $$->SetField (pos_AS_ExplFnDef_body,    *$7);
          $$->SetField (pos_AS_ExplFnDef_fnpre,   $8->GetField (1));
          $$->SetField (pos_AS_ExplFnDef_fnpost,  $8->GetField (2));
          $$->SetField (pos_AS_ExplFnDef_access,  Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ExplFnDef_stat,    Bool (false));
          $$->SetField (pos_AS_ExplFnDef_measu,   Nil ());

          delete $1;
          delete $3;
          delete $4;
          delete $5;
          delete $7;
          delete $8;
        }
        | Pattern '[' ListOfTypeVar ']' ':' FunctionType Identifier ParametersList LEX_IS_DEFINED_AS FnBody PrePost
        { $$ = new TYPE_AS_ExplFnDef();
          MYPARSER::SetPos2(*$$, @1, @11);

          if ($1->Is (TAG_TYPE_AS_PatternName))
          { if (! ($1->GetField (pos_AS_PatternName_nm) == *$7))
              MYPARSER::Report (L"Identifiers in function heading should be identical.",@1,@7);
            $$->SetField (pos_AS_ExplFnDef_nm, $1->GetField (pos_AS_PatternName_nm));
          }
          else
          { MYPARSER::Report (L"Local function identifiers must be simple identifiers.", @1);
            $$->SetField (pos_AS_ExplFnDef_nm, Record());
          }

          $$->SetField (pos_AS_ExplFnDef_tpparms, *$3);
          $$->SetField (pos_AS_ExplFnDef_tp,      *$6);
          $$->SetField (pos_AS_ExplFnDef_parms,   *$8);
          $$->SetField (pos_AS_ExplFnDef_body,    *$10);
          $$->SetField (pos_AS_ExplFnDef_fnpre,   $11->GetField (1));
          $$->SetField (pos_AS_ExplFnDef_fnpost,  $11->GetField (2));
          $$->SetField (pos_AS_ExplFnDef_access,  Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ExplFnDef_stat,    Bool (false));
          $$->SetField (pos_AS_ExplFnDef_measu,   Nil ());

          delete $1;
          delete $3;
          delete $6;
          delete $7;
          delete $8;
          delete $10;
          delete $11;
        }
        ;

LocalImplFunctionDefinition
        : Pattern TypeVarList ParameterTypes NonEmptyIdentifierTypePairList Pre Post
        { $$ = new TYPE_AS_ImplFnDef();
          MYPARSER::SetPos2(*$$, @1, @6);
          if ($1->Is (TAG_TYPE_AS_PatternName))
          { $$->SetField (pos_AS_ImplFnDef_nm, $1->GetField (pos_AS_PatternName_nm));
          }
          else
          { MYPARSER::Report (L"Local function identifiers must be simple identifiers.", @1);
            $$->SetField (pos_AS_ImplFnDef_nm, Record());
          }

          $$->SetField (pos_AS_ImplFnDef_params,   *$2);
          $$->SetField (pos_AS_ImplFnDef_partps,   *$3);
          $$->SetField (pos_AS_ImplFnDef_resnmtps, *$4);
          $$->SetField (pos_AS_ImplFnDef_fnpre,    *$5);
          $$->SetField (pos_AS_ImplFnDef_fnpost,   *$6);
          $$->SetField (pos_AS_ImplFnDef_access,   Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ImplFnDef_stat,     Bool(false));

          delete $1;
          delete $2;
          delete $3;
          delete $4;
          delete $5;
          delete $6;
        }
        | Pattern TypeVarList ParameterTypes NonEmptyIdentifierTypePairList LEX_IS_DEFINED_AS FnBody PrePost
        { $$ = new TYPE_AS_ExtExplFnDef();
          MYPARSER::SetPos2(*$$, @1, @7);

          if ($1->Is (TAG_TYPE_AS_PatternName))
          { $$->SetField (pos_AS_ExtExplFnDef_nm, $1->GetField (pos_AS_PatternName_nm));
          }
          else
          { MYPARSER::Report (L"Local function identifiers must be simple identifiers.", @1);
            $$->SetField (pos_AS_ExtExplFnDef_nm, Record());
          }

          $$->SetField (pos_AS_ExtExplFnDef_params,   *$2);
          $$->SetField (pos_AS_ExtExplFnDef_partps,   *$3);
          $$->SetField (pos_AS_ExtExplFnDef_resnmtps, *$4);
          $$->SetField (pos_AS_ExtExplFnDef_body,     *$6);
          $$->SetField (pos_AS_ExtExplFnDef_fnpre,    $7->GetField (1));
          $$->SetField (pos_AS_ExtExplFnDef_fnpost,   $7->GetField (2));
          $$->SetField (pos_AS_ExtExplFnDef_access,   Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ExtExplFnDef_stat,     Bool (false));
          $$->SetField (pos_AS_ExtExplFnDef_measu,    Nil());

          delete $1;
          delete $2;
          delete $3;
          delete $4;
          delete $6;
          delete $7;
        }
        ;

/*
LocalExplFunctionDefinition
        : Pattern TypeVarList ':' FunctionType Identifier ParametersList LEX_IS_DEFINED_AS FnBody PrePost
        { $$ = new TYPE_AS_ExplFnDef();
          MYPARSER::SetPos2(*$$, @1, @9);

          if ($1->Is (TAG_TYPE_AS_PatternName))
          { if (! ($1->GetField (pos_AS_PatternName_nm) == *$5))
              MYPARSER::Report (L"Identifiers in function heading should be identical.",@1,@5);
            $$->SetField (pos_AS_ExplFnDef_nm, $1->GetField (pos_AS_PatternName_nm));
          }
          else
          { MYPARSER::Report (L"Local function identifiers must be simple identifiers.", @1);
            $$->SetField (pos_AS_ExplFnDef_nm, Record());
          }

          $$->SetField (pos_AS_ExplFnDef_tpparms, *$2);
          $$->SetField (pos_AS_ExplFnDef_tp,      *$4);
          $$->SetField (pos_AS_ExplFnDef_parms,   *$6);
          $$->SetField (pos_AS_ExplFnDef_body,    *$8);
          $$->SetField (pos_AS_ExplFnDef_fnpre,   $9->GetField (1));
          $$->SetField (pos_AS_ExplFnDef_fnpost,  $9->GetField (2));
          $$->SetField (pos_AS_ExplFnDef_access,  Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ExplFnDef_stat,    Bool (false));
          $$->SetField (pos_AS_ExplFnDef_measu,   Nil ());

          delete $1;
          delete $2;
          delete $4;
          delete $5;
          delete $6;
          delete $8;
          delete $9;
        }
        ;
*/

/*
LocalExtExplFunctionDefinition
        : Pattern TypeVarList ParameterTypes NonEmptyIdentifierTypePairList LEX_IS_DEFINED_AS FnBody PrePost
        { $$ = new TYPE_AS_ExtExplFnDef();
          MYPARSER::SetPos2(*$$, @1, @7);

          if ($1->Is (TAG_TYPE_AS_PatternName))
          { $$->SetField (pos_AS_ExtExplFnDef_nm, $1->GetField (pos_AS_PatternName_nm));
          }
          else
          { MYPARSER::Report (L"Local function identifiers must be simple identifiers.", @1);
            $$->SetField (pos_AS_ExtExplFnDef_nm, Record());
          }

          $$->SetField (pos_AS_ExtExplFnDef_params,   *$2);
          $$->SetField (pos_AS_ExtExplFnDef_partps,   *$3);
          $$->SetField (pos_AS_ExtExplFnDef_resnmtps, *$4);
          $$->SetField (pos_AS_ExtExplFnDef_body,     *$6);
          $$->SetField (pos_AS_ExtExplFnDef_fnpre,    $7->GetField (1));
          $$->SetField (pos_AS_ExtExplFnDef_fnpost,   $7->GetField (2));
          $$->SetField (pos_AS_ExtExplFnDef_access,   Int (NOT_INITIALISED_AS));
          $$->SetField (pos_AS_ExtExplFnDef_stat,     Bool (false));
          $$->SetField (pos_AS_ExtExplFnDef_measu,    Nil());

          delete $1;
          delete $2;
          delete $3;
          delete $4;
          delete $6;
          delete $7;
        }
        ;
*/

ParameterTypes
        : '(' ListOfPatternTypePair ')'
        { $$ = $2;
        }
        | '(' ')'
        { $$ = new Sequence ();
        }
        ;

ListOfPatternTypePair
        : PatternList ':' Type
        { $$ = new Sequence ();
          TYPE_AS_PatTypePair PTP;
          MYPARSER::SetPos2(PTP, @1, @3);
          PTP.SetField (pos_AS_PatTypePair_pats, *$1);
          PTP.SetField (pos_AS_PatTypePair_tp,   *$3);
          $$->ImpAppend (PTP);
          delete $1;
          delete $3;
        }
        | ListOfPatternTypePair ',' PatternList ':' Type
        { TYPE_AS_PatTypePair PTP;
          MYPARSER::SetPos2(PTP, @3, @5);
          PTP.SetField (pos_AS_PatTypePair_pats, *$3);
          PTP.SetField (pos_AS_PatTypePair_tp,   *$5);
          $1->ImpAppend (PTP);
          delete $3;
          delete $5;
        }
        ;

Pre     : /* empty */
        { $$ = new Generic (Nil());
        }
        | LEX_PRE Expression
        { $$ = new Generic(*$2);
          delete $2;
        }
        ;

Post    : LEX_POST Expression
        { $$ = new Generic(*$2);
          delete $2;
        }
        ;

PrePost : /* empty */
        { $$ = new Tuple (2); // ([AS`Expr] * [AS`Expr])
          $$->SetField (1, Nil ());
          $$->SetField (2, Nil ());
        }
        | LEX_PRE Expression
        { $$ = new Tuple (2); // ([AS`Expr] * [AS`Expr])
          $$->SetField (1, *$2);
          $$->SetField (2, Nil ());

          delete $2;
        }
        | LEX_POST Expression
        { $$ = new Tuple (2); // ([AS`Expr] * [AS`Expr])
          $$->SetField (1, Nil ());
          $$->SetField (2, *$2);

          delete $2;
        }
        | LEX_PRE Expression LEX_POST Expression
        { $$ = new Tuple (2); // ([AS`Expr] * [AS`Expr])
          $$->SetField (1, *$2);
          $$->SetField (2, *$4);

          delete $2;
          delete $4;
        }
        ;

#if FULL
Measure
        : /* empty */
        { $$ = new Generic (Nil());
        }
        | LEX_MEASURE Name
        { $$ = new Generic (*$2);
          delete $2;
        }
        ;
IdentifierTypePairList
        : /* empty */
        { $$ = new Sequence ();
        }
        | NonEmptyIdentifierTypePairList
        ;
#endif // FULL

NonEmptyIdentifierTypePairList
        : Identifier ':' Type
        { $$ = new Sequence ();
          TYPE_AS_NameType idtppair;
          MYPARSER::SetPos2(idtppair, @1, @3);
          idtppair.SetField (pos_AS_NameType_nm, *$1);
          idtppair.SetField (pos_AS_NameType_tp, *$3);
          $$->ImpAppend (idtppair);
          delete $1;
          delete $3;
        }
        | NonEmptyIdentifierTypePairList ',' Identifier ':' Type
        { TYPE_AS_NameType idtppair;
          MYPARSER::SetPos2(idtppair, @3, @5);
          idtppair.SetField (pos_AS_NameType_nm, *$3);
          idtppair.SetField (pos_AS_NameType_tp, *$5);
          $$->ImpAppend (idtppair);
          delete $3;
          delete $5;
        }
        ;

TypeVarList
        : /* empty */
        {  $$ = new Sequence;
        }
        | '[' ListOfTypeVar ']'
        {  $$ = $2;
        }
        ;

ListOfTypeVar
        : TypeVariable
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfTypeVar ',' TypeVariable
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

ParametersList
        : Parameters
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ParametersList Parameters
        { (*$1).ImpAppend (*$2);
          delete $2;
        }
        ;

Parameters
        : '(' ')'
        { $$ = new Sequence;
        }
        | '(' PatternList ')'
        { $$ = new Sequence (*$2);
          delete $2;
        }
        ;

IfExpression
        : LEX_IF Expression LEX_THEN Expression ListOfElsifExpressions LEX_ELSE Expression
        {
          $$ = new TYPE_AS_IfExpr();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_IfExpr_test,  *$2);
          $$->SetField (pos_AS_IfExpr_cons,  *$4);
          $$->SetField (pos_AS_IfExpr_elsif, *$5);
          $$->SetField (pos_AS_IfExpr_altn,  *$7);
          delete $2;
          delete $4;
          delete $5;
          delete $7;
        }
/*
        | '(' Expression '?' Expression ':' Expression ')'
        {
          $$ = new TYPE_AS_IfExpr();
          MYPARSER::SetPos2(*$$, @1, @7);
          $$->SetField (pos_AS_IfExpr_test,  *$2);
          $$->SetField (pos_AS_IfExpr_cons,  *$4);
          $$->SetField (pos_AS_IfExpr_elsif, SEQ<TYPE_AS_ElseifExpr>());
          $$->SetField (pos_AS_IfExpr_altn,  *$6);
          delete $2;
          delete $4;
          delete $6;
        }
*/
        ;


ListOfElsifExpressions
        : /* empty */
        { $$ = new Sequence ();
        }
        | ListOfElsifExpressions LEX_ELSEIF Expression LEX_THEN Expression
        { TYPE_AS_ElseifExpr EIE;
          MYPARSER::SetPos2(EIE, @2, @5);
          EIE.SetField (pos_AS_ElseifExpr_test, *$3);
          EIE.SetField (pos_AS_ElseifExpr_cons, *$5);
          $1->ImpAppend (EIE);
          delete $3;
          delete $5;
        }
        ;

CasesExpression
        : LEX_CASES Expression ':' CasesExpressionAlternatives OthersExpression LEX_END
        {
          $$ = new TYPE_AS_CasesExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_CasesExpr_sel,    *$2);
          $$->SetField (pos_AS_CasesExpr_altns,  *$4);
          $$->SetField (pos_AS_CasesExpr_Others, *$5);
          delete $2;
          delete $4;
          delete $5;
        }
        ;

CasesExpressionAlternatives
        : CasesExpressionAlternative
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | CasesExpressionAlternatives ',' CasesExpressionAlternative
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

CasesExpressionAlternative
        : PatternList LEX_ARROW Expression
//        : PatternCondList LEX_ARROW Expression
        { $$ = new TYPE_AS_CaseAltn();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_CaseAltn_match, *$1);
          $$->SetField (pos_AS_CaseAltn_body,  *$3);
          delete $1;
          delete $3;
        }
        ;

OthersExpression
        :
        { $$ = new Generic(Nil());
          /* The Nil() value should result in 'error' from
             the interpreter if it encounters it. The user
             may have supplied full coverage of the domain
             of the expression.
          */
        }
        | ',' LEX_OTHERS LEX_ARROW Expression
        { $$ = new Generic(*$4);
          delete $4;
        }
        ;

/** Section in reference document: 9.6.1 Unary Expressions ********************/
/** Deviations from the standard/Remarks: IPTES                              **/

UnaryExpression
        : '+' Expression %prec LEX_ABS
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(NUMPLUS));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | '-' Expression %prec LEX_ABS
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(NUMMINUS));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_ABS Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(NUMABS));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_FLOOR Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(FLOOR));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_NOT Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(NOT));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_CARD Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SETCARD));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_POWER Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SETPOWER));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_DUNION Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SETDISTRUNION));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_DINTER Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SETDISTRINTERSECT));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_DMERGE Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(MAPDISTRMERGE));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_HD Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQHEAD));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_TL Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQTAIL));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_LEN Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQLEN));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_ELEMS Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQELEMS));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_INDS Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQINDICES));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_CONC Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQDISTRCONC));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
// for VDM 10
        | LEX_REVERSE Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(SEQREVERSE));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_DOM Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(MAPDOM));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_RNG Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(MAPRNG));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        | LEX_INVERSE Expression
        {
          $$ = new TYPE_AS_PrefixExpr();
          MYPARSER::SetPos2(*$$, @1, @2);
          $$->SetField (pos_AS_PrefixExpr_opr, Int(MAPINVERSE));
          $$->SetField (pos_AS_PrefixExpr_arg, *$2);
          delete $2;
        }
        ;

/** Section in reference document: 9.6.3 Binary Expressions *******************/
/** Deviations from the standard/Remarks: IPTES                              **/

BinaryExpression
        : Expression LEX_DOTHASH NumericLiteral
        {
          $$ = new TYPE_AS_TupleSelectExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_TupleSelectExpr_tuple, *$1);
          $$->SetField (pos_AS_TupleSelectExpr_no,    *$3);
          delete $1;
          delete $3;
        }
        | Expression '+' Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMPLUS));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression '-' Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMMINUS));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression '*' Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMMULT));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression '/' Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMDIV));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_REM Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMREM));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_MOD Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMMOD));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_ARITHMETIC_INTEGER_DIVISION Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(INTDIV));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_SET_UNION Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(SETUNION));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_SET_MINUS Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(SETMINUS));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_SET_INTERSECTION Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(SETINTERSECT));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_SUBSET Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(SUBSET));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_PROPER_SUBSET Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(PROPERSUBSET));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_IN_SET Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(INSET));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_NOT_IN_SET Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NOTINSET));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_SEQUENCE_CONCATENATE Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(SEQCONC));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_MAP_MERGE Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(MAPMERGE));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_MAP_DOMAIN_RESTRICT_TO Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(MAPDOMRESTTO));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_MAP_DOMAIN_RESTRICT_BY Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(MAPDOMRESTBY));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_MAP_RANGE_RESTRICT_TO Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(MAPRNGRESTTO));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_MAP_RANGE_RESTRICT_BY Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(MAPRNGRESTBY));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_EXP_OR_ITERATE Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMEXP));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_COMP Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(COMPOSE));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_AND Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(AND));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_OR Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(OR));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_IMPLY Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(IMPLY));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_LOGICAL_EQUIVALENCE Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(EQUIV));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_EQUAL Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(EQ));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
// 20060105
//#if VDMPP
//        | Expression LEX_APPROX  Expression
//        {
//          $$ = new TYPE_AS_BinaryExpr();
//          MYPARSER::SetPos3(*$$, @1, @2, @3);
//          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
//          $$->SetField (pos_AS_BinaryExpr_opr,   Int(APPROX));
//          $$->SetField (pos_AS_BinaryExpr_right, *$3);
//          delete $1;
//          delete $3;
//        }
//#endif //VDMPP
        | Expression LEX_NOT_EQUAL Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NE));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_LESS_THAN Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMLT));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_LESS_THAN_OR_EQUAL Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMLE));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_GREATER_THAN Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMGT));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        | Expression LEX_GREATER_THAN_OR_EQUAL Expression
        {
          $$ = new TYPE_AS_BinaryExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_BinaryExpr_left,  *$1);
          $$->SetField (pos_AS_BinaryExpr_opr,   Int(NUMGE));
          $$->SetField (pos_AS_BinaryExpr_right, *$3);
          delete $1;
          delete $3;
        }
        ;

/** Section in reference document: 9.6.4 Quantified Expressions ***************/
/** Deviations from the standard/Remarks: IPTES                              **/

Predication
        : LEX_RAISED_DOT Expression
        {
          $$ = $2;
        }
        ;

OptPredication
        :
        {
          $$ = new Generic(Nil());
        }
        | Predication
        {
          $$ = new Generic(*$1);
          delete $1;
        }
        ;

QuantifiedExpression
        : AllOrExistsExpression
        | ExistsUniqueExpression
        ;

AllOrExistsExpression
        : LEX_FORALL BindList Predication
        {
          $$ = new TYPE_AS_AllOrExistsExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_AllOrExistsExpr_quant, Int (ALL));
          $$->SetField (pos_AS_AllOrExistsExpr_bind,  *$2);
          $$->SetField (pos_AS_AllOrExistsExpr_pred,  *$3);
          delete $2;
          delete $3;
        }
        | LEX_EXISTS BindList Predication
        {
          $$ = new TYPE_AS_AllOrExistsExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_AllOrExistsExpr_quant, Int (EXISTS));
          $$->SetField (pos_AS_AllOrExistsExpr_bind,  *$2);
          $$->SetField (pos_AS_AllOrExistsExpr_pred,  *$3);
          delete $2;
          delete $3;
        }
        ;

/** Section in reference document: 9.6.5 Iota Expression **********************/
/** Deviations from the standard/Remarks: none                               **/

IotaExpression
        : LEX_IOTA Bind Predication
        {
          $$ = new TYPE_AS_IotaExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_IotaExpr_bind, *$2);
          $$->SetField (pos_AS_IotaExpr_pred, *$3);
          delete $2;
          delete $3;
        }
        ;

LambdaExpression
        : LEX_LAMBDA TypeBindList Predication
        {
          $$ = new TYPE_AS_LambdaExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_LambdaExpr_parm, *$2);
          $$->SetField (pos_AS_LambdaExpr_body, *$3);
          $$->SetField (pos_AS_LambdaExpr_type, Nil());
          delete $2;
          delete $3;
        }
//        | LEX_LAMBDA TypeBindList Predication ':' Type
//        {
//          $$ = new TYPE_AS_LambdaExpr();
//          MYPARSER::SetPos2(*$$, @1, @5);
//          $$->SetField (pos_AS_LambdaExpr_parm, *$2);
//          $$->SetField (pos_AS_LambdaExpr_body, *$3);
//          $$->SetField (pos_AS_LambdaExpr_type, *$5);
//          delete $2;
//          delete $3;
//          delete $5;
//        }
        ;

TypeBindList
        : TypeBind
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | TypeBindList ',' TypeBind
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

BindList
        : MultipleBind
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | BindList ',' MultipleBind
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

ExistsUniqueExpression
        : LEX_EXISTS1 Bind Predication
        {
          $$ = new TYPE_AS_ExistsUniqueExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_ExistsUniqueExpr_bind, *$2);
          $$->SetField (pos_AS_ExistsUniqueExpr_pred, *$3);
          delete $2;
          delete $3;
        }
        ;

/** Section in reference document: 9.6.6 Set Expressions **********************/
/** Deviations from the standard/Remarks: IPTES                              **/

SetExpression
        : SetEnumeration
        | SetComprehension
        | SetRangeExpression
        ;

SetEnumeration
        : '{' ExpressionList '}'
        {
          $$ = new TYPE_AS_SetEnumerationExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_SetEnumerationExpr_els, *$2);
          delete $2;
        }
        ;

SetComprehension
        : '{' Expression '|' BindList OptPredication '}'
        {
          $$ = new TYPE_AS_SetComprehensionExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_SetComprehensionExpr_elem, *$2);
          $$->SetField (pos_AS_SetComprehensionExpr_bind, *$4);
          $$->SetField (pos_AS_SetComprehensionExpr_pred, *$5);

          delete $2;
          delete $4;
          delete $5;
        }
        ;

SetRangeExpression
        : '{'  Expression LEX_RANGE_OVER Expression '}'
        {
          $$ = new TYPE_AS_SetRangeExpr();
          MYPARSER::SetPos2(*$$, @1, @5);
          $$->SetField (pos_AS_SetRangeExpr_lb, *$2);
          $$->SetField (pos_AS_SetRangeExpr_ub, *$4);
          delete $2;
          delete $4;
        }
        ;

/** Section in reference document: 9.6.7 Sequence Expressions *****************/
/** Deviations from the standard/Remarks: IPTES                              **/

SequenceExpression
        : SequenceEnumeration
        | SequenceComprehension
        | SubSequence
        | SequenceModifier
        ;

SequenceEnumeration
        : '['  ExpressionList ']'
        {
          $$ = new TYPE_AS_SeqEnumerationExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_SeqEnumerationExpr_els, *$2);
          delete $2;
        }
        ;

SetOrSeqBind
        : SetBind
        | SeqBind
        ;
            
SequenceComprehension
        : '['  Expression '|' SetOrSeqBind OptPredication ']'
        {
          $$ = new TYPE_AS_SeqComprehensionExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_SeqComprehensionExpr_elem, *$2);
          $$->SetField (pos_AS_SeqComprehensionExpr_bind, *$4);
          $$->SetField (pos_AS_SeqComprehensionExpr_pred, *$5);

          delete $2;
          delete $4;
          delete $5;
        }
        ;

SubSequence
        : Expression '(' Expression LEX_RANGE_OVER Expression ')'
        {
          $$ = new TYPE_AS_SubSequenceExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @6); // ??ETN
          $$->SetField (pos_AS_SubSequenceExpr_sequence, *$1);
          $$->SetField (pos_AS_SubSequenceExpr_frompos,  *$3);
          $$->SetField (pos_AS_SubSequenceExpr_topos,    *$5);
          delete $1;
          delete $3;
          delete $5;
        }
        ;

SequenceModifier
        : Expression LEX_MODIFY_BY Expression
        {
          $$ = new TYPE_AS_SeqModifyMapOverrideExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_SeqModifyMapOverrideExpr_seqmap, *$1);
          $$->SetField (pos_AS_SeqModifyMapOverrideExpr_mapexp, *$3);
          delete $1;
          delete $3;
        }
        ;


/** Section in reference document: 9.6.8 Map Expressions **********************/
/** Deviations from the standard/Remarks: IPTES                              **/

MapExpression : MapEnumeration
              | MapComprehension
              ;

MapEnumeration
        : '{' LEX_BAR_ARROW '}'
        {
          $$ = new TYPE_AS_MapEnumerationExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_MapEnumerationExpr_els, Sequence ());
        }
        | '{' ListOfMaplets '}'
        {
          $$ = new TYPE_AS_MapEnumerationExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_MapEnumerationExpr_els, *$2);
          delete $2;
        }
        ;

ListOfMaplets
        : Maplet
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfMaplets ',' Maplet
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

Maplet
        : Expression LEX_BAR_ARROW Expression
        { $$ = new TYPE_AS_Maplet();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_Maplet_mapdom, *$1);
          $$->SetField (pos_AS_Maplet_maprng, *$3);
          delete $1;
          delete $3;
        }
        ;

MapComprehension
        : '{' Maplet '|' BindList OptPredication '}'
        {
          $$ = new TYPE_AS_MapComprehensionExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_MapComprehensionExpr_elem, *$2);
          $$->SetField (pos_AS_MapComprehensionExpr_bind, *$4);
          $$->SetField (pos_AS_MapComprehensionExpr_pred, *$5);

          delete $2;
          delete $4;
          delete $5;
        }
        ;

/** Section in reference document: 9.6.9 Tuple Expression *********************/
/** Deviations from the standard/Remarks: IPTES                              **/

TupleExpression
        : TupleConstructor
        ;

TupleConstructor
        : LEX_MK_ '(' ExpressionList ')'
        {
          $$ = new TYPE_AS_TupleConstructorExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          if ($3->Length() <= 1)
            MYPARSER::Report (L"Tuples must have at least two elements.", @3);
          else
            $$->SetField (pos_AS_TupleConstructorExpr_fields, *$3);
          delete $3;
        }
        ;

PreCondApply
        : LEX_PRECONDAPPLY '(' Expression ')'
        { $$ = new TYPE_AS_PreConditionApplyExpr ();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_PreConditionApplyExpr_fct, *$3);
          $$->SetField (pos_AS_PreConditionApplyExpr_arg, Sequence ());
          delete $3;
        }
        | LEX_PRECONDAPPLY '(' Expression ',' ExpressionList ')'
        { $$ = new TYPE_AS_PreConditionApplyExpr ();
          MYPARSER::SetPos2 (*$$, @1, @6);
          $$->SetField (pos_AS_PreConditionApplyExpr_fct, *$3);
          $$->SetField (pos_AS_PreConditionApplyExpr_arg, *$5);
          delete $3;
          delete $5;
        }
        ;

/** Section in reference document: 9.6.10 Record Expressions ******************/
/** Deviations from the standard/Remarks: IPTES                              **/

RecordExpression
        : RecordConstructor
        | RecordModifier
        ;

RecordConstructor
        : LEX_MK_ Name '(' ExpressionList ')'
        {
          $$ = new TYPE_AS_RecordConstructorExpr();
          MYPARSER::SetPos2(*$$, @1, @5);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_RecordConstructorExpr_tag,    *$2);
          $$->SetField (pos_AS_RecordConstructorExpr_fields, *$4);
          delete $2;
          delete $4;
        }
        ;

RecordModifier
        : LEX_MU '(' Expression ',' ListOfRecordModifications ')'
        {
          $$ = new TYPE_AS_RecordModifierExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_RecordModifierExpr_rec,       *$3);
          $$->SetField (pos_AS_RecordModifierExpr_modifiers, *$5);
          delete $3;
          delete $5;
        }
        ;
ListOfRecordModifications
        : RecordModification
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfRecordModifications ',' RecordModification
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

RecordModification
        : Identifier LEX_BAR_ARROW Expression
        { $$ = new TYPE_AS_RecordModification();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_RecordModification_field, *$1);
          $$->SetField (pos_AS_RecordModification_newexpr,   *$3);
          delete $1;
          delete $3;
        }
        ;

TokenExpression
        : TokenConstructor
        ;

TokenConstructor
        : LEX_MK_ LEX_TOKEN '(' Expression ')'
        {
          $$ = new TYPE_AS_TokenConstructorExpr();
          MYPARSER::SetPos3(*$$, @1, @4, @5); //??
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_TokenConstructorExpr_field, *$4);
          delete $4;
        }
        ;

/** Section in reference document: 9.6.11 Apply Expressions *******************/
/** Deviations from the standard/Remarks:                                    **/
/** - `Apply' has been adjusted so that it will recognize a call statement   **/
/**   as well in certain contexts.                                           **/

ApplyExpression
        : Apply
        | FieldSelect
        ;

Apply
        : Expression '(' ExpressionList ')'
        {
          $$ = new TYPE_AS_ApplyExpr();
          MYPARSER::SetPos3(*$$, @1, @2, @4); // ??
          $$->SetField (pos_AS_ApplyExpr_fct, *$1);
          $$->SetField (pos_AS_ApplyExpr_arg, *$3);
          delete $1;
          delete $3;
        }
        ;

FieldSelect
#if VDMSL
        : Expression '.' Identifier
#endif // VDMSL
#if VDMPP
        : Expression '.' Name
#endif // VDMPP
        {
          $$ = new TYPE_AS_FieldSelectExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_FieldSelectExpr_rec, *$1);
          $$->SetField (pos_AS_FieldSelectExpr_nm,  *$3);
          delete $1;
          delete $3;
        }
#if VDMPP
        | Expression '.' FunctionTypeInstantiation
        {
          $$ = new TYPE_AS_FieldSelectExpr();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_FieldSelectExpr_rec, *$1);
          $$->SetField (pos_AS_FieldSelectExpr_nm,  *$3);
          delete $1;
          delete $3;
        }
#endif // VDMPP
        ;

FunctionTypeInstantiation
        : OldNameOrName '[' TypeList ']' // Was Name; created conflict
        {
          $$ = new TYPE_AS_FctTypeInstExpr();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_FctTypeInstExpr_polyfct, *$1);
          $$->SetField (pos_AS_FctTypeInstExpr_inst,    *$3);
          delete $1;
          delete $3;
        }
        ;

TypeList
        : Type
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | TypeList ',' Type
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

/** Section in reference document: 9.6.12 Simple Expressions ******************/
/** Deviations from the standard/Remarks: IPTES                              **/

SimpleExpression
        : IsExpression
#if VDMPP
        | IsOfClassExpression
        | IsOfBaseClassExpression
        | SameClassExpression
        | SameBaseClassExpression
#endif //VDMPP
        ;

IsExpression
        : LEX_IS_ Name '(' Expression ')'
        {
          $$ = new TYPE_AS_IsExpr();
          MYPARSER::SetPos2(*$$, @1, @5);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_IsExpr_type, *$2);
          $$->SetField (pos_AS_IsExpr_arg,  *$4);
          delete $2;
          delete $4;
        }
        | LEX_IS_ BasicType '(' Expression ')'
        {
          $$ = new TYPE_AS_IsExpr();
          MYPARSER::SetPos2(*$$, @1, @5);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_IsExpr_type, *$2);
          $$->SetField (pos_AS_IsExpr_arg,  *$4);
          delete $2;
          delete $4;
        }
        | LEX_IS_ '(' Expression ',' Type ')'
        {
          $$ = new TYPE_AS_TypeJudgementExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_TypeJudgementExpr_expr, *$3);
          $$->SetField (pos_AS_TypeJudgementExpr_type, *$5);
          delete $3;
          delete $5;
        }
        ;

NarrowExpression
        : LEX_NARROW_ '(' Expression ',' Type ')'
        {
          $$ = new TYPE_AS_NarrowExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_NarrowExpr_expr, *$3);
          $$->SetField (pos_AS_NarrowExpr_type, *$5);
          delete $3;
          delete $5;
        }
        ;

/** Section in reference document: 9.6.13 Undefined Expression ****************/
/** Deviations from the standard/Remarks: IPTES                              **/

UndefinedExpression
        : LEX_UNDEFINED
        {
          $$ = new TYPE_AS_UndefinedExpr();
          MYPARSER::SetPos2(*$$, @1, @1);
        }
        ;

#if VDMPP
/** Special Afrodite functions                  *******************************/
SameClassExpression
        : LEX_SAMECLASS '(' Expression ',' Expression ')'
        {
          $$ = new TYPE_AS_SameClassExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_SameClassExpr_expr1, *$3);
          $$->SetField (pos_AS_SameClassExpr_expr2, *$5);
          delete $3;
          delete $5;
        }
        ;

SameBaseClassExpression
        : LEX_SAMEBASECLASS '(' Expression ',' Expression ')'
        {
          $$ = new TYPE_AS_SameBaseClassExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_SameBaseClassExpr_expr1, *$3);
          $$->SetField (pos_AS_SameBaseClassExpr_expr2, *$5);
          delete $3;
          delete $5;
        }
        ;

IsOfClassExpression
        : LEX_ISOFCLASS '(' Name ',' Expression ')'
        {
          $$ = new TYPE_AS_IsOfClassExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_IsOfClassExpr_cls, *$3);
          $$->SetField (pos_AS_IsOfClassExpr_arg,   *$5);
          delete $3;
          delete $5;
        }
        ;

IsOfBaseClassExpression
        : LEX_ISOFBASECLASS '(' Name ',' Expression ')'
        {
          $$ = new TYPE_AS_IsOfBaseClassExpr();
          MYPARSER::SetPos2(*$$, @1, @6);
          $$->SetField (pos_AS_IsOfBaseClassExpr_cls, *$3);
          $$->SetField (pos_AS_IsOfBaseClassExpr_arg,   *$5);
          delete $3;
          delete $5;
        }
        ;
#endif //VDMPP

/** Section in reference document: 9.6.14 Names *******************************/
/** Deviations from the standard/Remarks: IPTES                              **/

Name    : IdentifierPrimeList
        {
          $$ = $1;
        }
        ;

OldNameOrName
        : Identifier LEX_HOOK
        {
          $$ = new TYPE_AS_OldName();
          MYPARSER::SetPos2(*$$, @1, @2); // @?
          Sequence id_l;
          id_l.ImpConc($1->GetSequence (pos_AS_Name_ids));
          $$->SetField (pos_AS_OldName_ids, id_l);
          delete $1;
        }
        | Name
        ;

IdentifierPrimeList
        : FnOpNameIdentifier
        | Identifier LEX_PRIME Identifier
        {
          $$ = new TYPE_AS_Name();
          MYPARSER::SetPos2(*$$, @1, @3);
          TYPE_AS_Ids ids ($1->GetSequence (pos_AS_Name_ids));
          ids.ImpConc ($3->GetSequence (pos_AS_Name_ids));
          $$->SetField (pos_AS_Name_ids, ids);
          delete $1;
          delete $3;
        }
        ;

#if FULL && VDMPP
IdentifierCommaList
        : Identifier
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | IdentifierCommaList ',' Identifier
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;
#endif // FULL && VDMPP

/** Because Field = Type = TypeName = Name = IdentifierPrimeList and
    some error productions, we cannot trap wrong PRIME as eg. L"'".
**/

FnOpNameIdentifier
        : Identifier
#if VDMPP
        | KeywordIdentifier
#endif // VDMPP
        ;

Identifier
        : LEX_identifier
        {
          $$ = new TYPE_AS_Name();

          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_Name_ids, MYPARSER::GetIdentifier(*$1));
#if FULL
          if (SCANNER::getLastWasLEX_CLASS_MOD()) {
            VDMPARSER::currentClassMod = *$$;
            SCANNER::setLastWasLEX_CLASS_MOD (false);
          }
#endif // FULL
        }
        | LEX_dollar_identifier
        {
          $$ = new TYPE_AS_Name();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_Name_ids, MYPARSER::GetIdentifier(*$1));
        }
        ;

#if VDMPP
// for java code generation support
KeywordIdentifier
        : LEX_START
        {
          if (!Settings.GetJCGHackParser())
            MYPARSER::Report (L"'start' is vdm keyword.", @1);

          $$ = new TYPE_AS_Name();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Name_ids, TYPE_AS_Ids().ImpAppend(ASTAUX::MkId(L"start")));
        }
        | LEX_SET
        {
          if (!Settings.GetJCGHackParser())
            MYPARSER::Report (L"'set' is vdm keyword.", @1);

          $$ = new TYPE_AS_Name();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Name_ids, TYPE_AS_Ids().ImpAppend(ASTAUX::MkId(L"set")));
        }
        | LEX_EXISTS
        {
          if (!Settings.GetJCGHackParser())
            MYPARSER::Report (L"'exists' is vdm keyword.", @1);

          $$ = new TYPE_AS_Name();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Name_ids, TYPE_AS_Ids().ImpAppend(ASTAUX::MkId(L"exists")));
        }
        | LEX_ERROR
        {
          if (!Settings.GetJCGHackParser())
            MYPARSER::Report (L"'error' is vdm keyword.", @1);

          $$ = new TYPE_AS_Name();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Name_ids, TYPE_AS_Ids().ImpAppend(ASTAUX::MkId(L"error")));
        }
        ;
#endif //VDMPP

MacroExpression
        : LEX_FILE_MACRO
        {
          $$ = new TYPE_AS_Macro();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Macro_tp, Int(MACRO_FILE));
        }
        | LEX_LINE_MACRO
        {
          $$ = new TYPE_AS_Macro();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Macro_tp, Int(MACRO_LINE));
        }
        | LEX_COLUMN_MACRO
        {
          $$ = new TYPE_AS_Macro();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Macro_tp, Int(MACRO_COLUMN));
        }
        | LEX_CLMOD_MACRO
        {
          $$ = new TYPE_AS_Macro();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Macro_tp, Int(MACRO_CLMOD));
        }
        | LEX_FNOP_MACRO
        {
          $$ = new TYPE_AS_Macro();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_Macro_tp, Int(MACRO_FNOP));
        }
        ;

/** Section in reference document: 11.3 Literals ******************************/

NumericLiteral
        : LEX_real_lit
        {
          Tuple t (TBUTILS::ConvRealLit ($1->c_str()));
          if (t.GetBoolValue(1))
          {
            $$ = new TYPE_AS_RealLit();
            MYPARSER::SetPos2(*$$, @1, @1); // @?
            $$->SetField (pos_AS_RealLit_val, t.GetReal(2));
          }
          else
            MYPARSER::Report (L"NUM_INCONSISTENCY", @1);
        }
        | LEX_int_lit
        {
          Tuple t (TBUTILS::ConvRealLit ($1->c_str()));
          if (t.GetBoolValue(1))
          {
            //$$ = new TYPE_AS_RealLit();
            //MYPARSER::SetPos2(*$$, @1, @1); // @?
            //$$->SetField (pos_AS_RealLit_val, t.GetReal(2));
            $$ = new TYPE_AS_NumLit();
            MYPARSER::SetPos2(*$$, @1, @1); // @?
            $$->SetField (pos_AS_NumLit_val, t.GetReal(2));
          }
          else
            MYPARSER::Report (L"NUM_INCONSISTENCY", @1);
        }
        ;

SymbolicLiteral
        : LEX_char_lit
        {
          $$ = new TYPE_AS_CharLit();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          wstring c (TBWSTR::string2wstring(*$1));
          if ((c.size() == 1) && (c[0] == L'\\'))
          {
            $$->SetField (pos_AS_CharLit_val, Char (c[0]));
          }
          else
          {
            wstring error, res;
            Backslashed::convert_backslashed(TBWSTR::string2wstring(*$1), res, error);
            if (error.length() > 0) {
              MYPARSER::Report(error.c_str(), @1);
            }
            else if (res.size() > 1) {
              MYPARSER::Report(L"Wrong universal character.", @1);
            }

            $$->SetField (pos_AS_CharLit_val, Char (res[0]));
          }
        }
        | LEX_bool_true
        {
          $$ = new TYPE_AS_BoolLit();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_BoolLit_val, Bool (true));
        }
        | LEX_bool_false
        {
          $$ = new TYPE_AS_BoolLit();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_BoolLit_val, Bool (false));
        }
        | LEX_quote_lit
        {
          $$ = new TYPE_AS_QuoteLit();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          Sequence val (TBWSTR::string2wstring(*$1));
          $$->SetField (pos_AS_QuoteLit_val, val);
        }
        | LEX_text_lit
        {
          $$ = new TYPE_AS_TextLit();
          MYPARSER::SetPos2(*$$, @1, @1); // @?

          wstring error, res;
          Backslashed::convert_backslashed(TBWSTR::string2wstring(*$1), res, error);
          if (error.length() > 0)
            MYPARSER::Report(error.c_str(), @1);

          $$->SetField (pos_AS_TextLit_val, Sequence(res));
        }
        | NumericLiteral
        | LEX_NIL
        {
          $$ = new TYPE_AS_NilLit();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
        }
        ;

/** Section in reference document: 9.8 Patterns *******************************/
/** Deviations from the standard/Remarks: IPTES                              **/

PatternList
        : Pattern
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | PatternList ',' Pattern
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

/*
PatternCondList
        : Pattern
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
        }
        | Pattern LEX_RAISED_DOT Expression
        { $$ = new Sequence ();
          $$->ImpAppend (*$1);
          delete $1;
          delete $3;
        }
        | PatternCondList ',' Pattern
        { $1->ImpAppend (*$3);
          delete $3;
        }
        | PatternCondList ',' Pattern LEX_RAISED_DOT Expression
        { $1->ImpAppend (*$3);
          delete $3;
          delete $5;
        }
        ;
*/

Pattern
        : PatternIdentifier
        | MatchValue
        | SetPattern
        | SequencePattern
        | MapPattern
        | TuplePattern
        | RecordPattern
#ifdef VDMPP
        | ObjectPattern
#endif // VDMPP
        ;

PatternIdentifier
        : Identifier
        {
          $$ = new TYPE_AS_PatternName();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_PatternName_nm, *$1);
          $$->SetField (pos_AS_PatternName_tp, Nil());
          delete $1;
        }
        | '-'
        {
          $$ = new TYPE_AS_PatternName();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_PatternName_nm, Nil ());
          $$->SetField (pos_AS_PatternName_tp, Nil());
        }
        ;

MatchValue
        : '(' Expression ')'
        {
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos3(*$$, @1, @2, @3); // @?
          $$->SetField (pos_AS_MatchVal_val, *$2);
          delete $2;
        }
        | '+' NumericLiteral
        {
          TYPE_AS_PrefixExpr expr;
          MYPARSER::SetPos2(expr, @1, @2);
          expr.SetField (pos_AS_PrefixExpr_opr, Int(NUMPLUS));
          expr.SetField (pos_AS_PrefixExpr_arg, *$2);
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos2(*$$, @1, @2); // @?
          $$->SetField (pos_AS_MatchVal_val, expr);
          delete $2;
        }
        | '-' NumericLiteral
        {
          TYPE_AS_PrefixExpr expr;
          MYPARSER::SetPos2(expr, @1, @2);
          expr.SetField (pos_AS_PrefixExpr_opr, Int(NUMMINUS));
          expr.SetField (pos_AS_PrefixExpr_arg, *$2);
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos2(*$$, @1, @2); // @?
          $$->SetField (pos_AS_MatchVal_val, expr);
          delete $2;
        }
        | SymbolicLiteral
        {
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos2(*$$, @1, @1); // @?
          $$->SetField (pos_AS_MatchVal_val, *$1);
          delete $1;
        }
        ;

SetPattern
        : SetEnumPattern
        | SetUnionPattern
        ;

SetEnumPattern
        : '{' PatternList '}'
        {
          $$ = new TYPE_AS_SetEnumPattern();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_SetEnumPattern_Elems, *$2);
          delete $2;
        }
        | '{' '}'
        {
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos2(*$$, @1, @2);
          TYPE_AS_SetEnumerationExpr EmptySet;
          MYPARSER::SetPos2(EmptySet, @1, @2);
          EmptySet.SetField (pos_AS_SetEnumerationExpr_els, Sequence());
          $$->SetField (pos_AS_SetEnumPattern_Elems, EmptySet);
        }
        ;

SetUnionPattern
        : Pattern LEX_SET_UNION  Pattern
        {
          $$ = new TYPE_AS_SetUnionPattern();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_SetUnionPattern_lp, *$1);
          $$->SetField (pos_AS_SetUnionPattern_rp, *$3);
          delete $1;
          delete $3;
        }
        ;

SequencePattern
        : SeqEnumPattern
        | SeqConcPattern
        ;

SeqEnumPattern
        : '[' PatternList ']'
        {
          $$ = new TYPE_AS_SeqEnumPattern();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_SeqEnumPattern_els, *$2);
          delete $2;
        }
        | '[' ']'
        {
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos2(*$$, @1, @2);
          TYPE_AS_SeqEnumerationExpr EmptySeq;
          MYPARSER::SetPos2(EmptySeq, @1, @2);
          EmptySeq.SetField (pos_AS_SeqEnumerationExpr_els, Sequence());
          $$->SetField (pos_AS_MatchVal_val, EmptySeq);
        }
        ;

SeqConcPattern
        : Pattern LEX_SEQUENCE_CONCATENATE Pattern
        {
          $$ = new TYPE_AS_SeqConcPattern();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_SeqConcPattern_lp, *$1);
          $$->SetField (pos_AS_SeqConcPattern_rp, *$3);
          delete $1;
          delete $3;
        }
        ;

MapPattern
        : MapEnumPattern
        | MapMergePattern
        ;

ListOfMapletPatterns
        : MapletPattern
        { $$ = new Sequence;
          $$->ImpAppend (*$1);
          delete $1;
        }
        | ListOfMapletPatterns ',' MapletPattern
        { $1->ImpAppend (*$3);
          delete $3;
        }
        ;

MapletPattern
        : Pattern LEX_BAR_ARROW Pattern
        { $$ = new TYPE_AS_MapletPattern();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_MapletPattern_dp, *$1);
          $$->SetField (pos_AS_MapletPattern_rp, *$3);
          delete $1;
          delete $3;
        }
        ;

MapEnumPattern
        : '{' ListOfMapletPatterns '}'
        {
          $$ = new TYPE_AS_MapEnumPattern();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_MapEnumPattern_mls, *$2);
          delete $2;
        }
        | '{' LEX_BAR_ARROW '}'
        {
          $$ = new TYPE_AS_MatchVal();
          MYPARSER::SetPos2(*$$, @1, @3);
          TYPE_AS_MapEnumerationExpr EmptyMap;
          MYPARSER::SetPos2(EmptyMap, @1, @3);
          EmptyMap.SetField (pos_AS_MapEnumerationExpr_els, Sequence());
          $$->SetField (pos_AS_MatchVal_val, EmptyMap);
        }
        ;

MapMergePattern
        : Pattern LEX_MAP_MERGE Pattern
        {
          $$ = new TYPE_AS_MapMergePattern();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_MapMergePattern_lp, *$1);
          $$->SetField (pos_AS_MapMergePattern_rp, *$3);
          delete $1;
          delete $3;
        }
        ;

TuplePattern
        : LEX_MK_ '(' PatternList ')'
        {
          $$ = new TYPE_AS_TuplePattern();
          MYPARSER::SetPos2(*$$, @1, @4);
          if ($3->Length() <= 1)
            MYPARSER::Report (L"Tuples must have at least two elements.", @3);
          else
            $$->SetField (pos_AS_TuplePattern_fields, *$3);
          delete $3;
        }
        ;

RecordPattern
        : LEX_MK_ Name '(' PatternList ')'
        {
          $$ = new TYPE_AS_RecordPattern();
          MYPARSER::SetPos2(*$$, @1, @5);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_RecordPattern_nm,     *$2);
          $$->SetField (pos_AS_RecordPattern_fields, *$4);
          delete $2;
          delete $4;
        }
        | LEX_MK_ Name '(' ')'
        {
          $$ = new TYPE_AS_RecordPattern();
          MYPARSER::SetPos2(*$$, @1, @4);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_RecordPattern_nm,     *$2);
          $$->SetField (pos_AS_RecordPattern_fields, Sequence ());
          delete $2;
        }
        ;

#ifdef VDMPP
ObjectPattern
        : LEX_OBJ_ Identifier FieldPatterns
        {
          $$ = new TYPE_AS_ObjectPattern();
          MYPARSER::SetPos2(*$$, @1, @3);
          MYPARSER::CheckNoDelimiter(@1);
          $$->SetField (pos_AS_ObjectPattern_cls,    *$2);
          $$->SetField (pos_AS_ObjectPattern_fields, *$3);
          delete $2;
          delete $3;
        }
        ;

FieldPatterns
        : '(' ListOfFieldPattern ')'
        { $$ = $2;
        }
        | '(' ')'
        { $$ = new Sequence ();
        }
        ;

ListOfFieldPattern
        : Identifier LEX_BAR_ARROW Pattern
        { $$ = new Sequence ();
          TYPE_AS_FieldPattern fp;
          MYPARSER::SetPos2(fp, @1, @3);
          fp.SetField (pos_AS_FieldPattern_nm, *$1);
          fp.SetField (pos_AS_FieldPattern_pat, *$3);
          $$->ImpAppend(fp);
          delete $1;
          delete $3;
        }
        | ListOfFieldPattern ',' Identifier LEX_BAR_ARROW Pattern
        { $$ = $1;
          TYPE_AS_FieldPattern fp;
          MYPARSER::SetPos2(fp, @1, @5);
          fp.SetField (pos_AS_FieldPattern_nm, *$3);
          fp.SetField (pos_AS_FieldPattern_pat, *$5);
          $$->ImpAppend(fp);
          delete $3;
          delete $5;
        }
        ;
#endif // VDMPP

PatternBind
        : Pattern
        | Bind
        ;

/** Section in reference document: 9.9 Bindings *******************************/
/** Deviations from the standard/Remarks: IPTES                              **/

Bind
        : SetBind
        | TypeBind
        ;

SetBind
        : Pattern LEX_IN_SET Expression %prec DUMMY_RELATION
        { $$ = new TYPE_AS_SetBind();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_SetBind_pat, *$1);
          $$->SetField (pos_AS_SetBind_Set, *$3);
          delete $1;
          delete $3;
        }
        ;

TypeBind
        : Pattern ':' Type
        {
          $$ = new TYPE_AS_TypeBind();
          MYPARSER::SetPos2(*$$, @1, @3);
          if ($1->Is(TAG_TYPE_AS_PatternName))
            $1->SetField(pos_AS_PatternName_tp, *$3);
          $$->SetField (pos_AS_TypeBind_pat, *$1);
          $$->SetField (pos_AS_TypeBind_tp,  *$3);
          delete $1;
          delete $3;
        }
        ;

SeqBind
        : Pattern LEX_IN_SEQ Expression %prec DUMMY_RELATION
        { $$ = new TYPE_AS_SeqBind();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_SeqBind_pat, *$1);
          $$->SetField (pos_AS_SeqBind_Seq, *$3);
          delete $1;
          delete $3;
        }
        ;

MultipleBind
        : MultipleSetBind
        | MultipleTypeBind
        ;

MultipleSetBind
        : PatternList LEX_IN_SET Expression
        {
          $$ = new TYPE_AS_MultSetBind();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          $$->SetField (pos_AS_MultSetBind_pat, *$1);
          $$->SetField (pos_AS_MultSetBind_Set, *$3);
          delete $1;
          delete $3;
        }
        ;
/** Adding 'error' for LEX_IN_SET collides with 'LocalDefinitions'. MA **/

MultipleTypeBind
        : PatternList ':' Type
        {
          $$ = new TYPE_AS_MultTypeBind();
          MYPARSER::SetPos3(*$$, @1, @2, @3);
          SEQ<TYPE_AS_Pattern> pat_l(*$1), n_pat_l;
          size_t len_pat_l = pat_l.Length();
          for (size_t idx = 1; idx <= len_pat_l; idx++)
          {
            TYPE_AS_Pattern p (pat_l[idx]);
            if (p.Is(TAG_TYPE_AS_PatternName))
              p.SetField(pos_AS_PatternName_tp, *$3);
            n_pat_l.ImpAppend(p);
          }
          $$->SetField (pos_AS_MultTypeBind_pat, n_pat_l);
          $$->SetField (pos_AS_MultTypeBind_tp, *$3);
          delete $1;
          delete $3;
        }
        ;

#if FULL

OptSemi_TexBreak
        : TexBreak
        { $$ = $1;
        }
        | ';' TexBreak
        { $$ = $2;
        }
        ;

/** Production TexBreak inserts break markers in orderly **/

TexBreak
        : LEX_TEXBREAK
        { $$ = new Bool(true);

ORDERLY_ON MYPARSER::AddOrderly(TAG_TYPE_PRETTY_PRINTER_Texbreak, Nil ());

        }
        |
        { $$ = new Bool(false);
        }
        ;

ModTexBreak
        : LEX_TEXBREAK
        { $$ = new Bool(true);
        }
        |
        { $$ = new Bool(false);
        }
        ;

#endif //FULL

OptSemi :
         /*empty*/
        {
        }
        | ';'
        {
        }
        ;

FnBody  : Expression
        {
          $$ = new TYPE_AS_FnBody();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_FnBody_body, *$1);
          delete $1;
        }
        | LEX_IS LEX_NOT LEX_YET LEX_SPECIFIED
        { $$ = new TYPE_AS_FnBody ();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_FnBody_body, Int(NOTYETSPEC));
        }
#if VDMPP
        | LEX_IS LEX_SUBCLASS LEX_RESPONSIBILITY
        { $$ = new TYPE_AS_FnBody ();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_FnBody_body, Int(SUBRESP));
        }
        ;
#endif //VDMPP

#if FULL
OpBody  : Statement
        {
          $$ = new TYPE_AS_OpBody();
          MYPARSER::SetPos2(*$$, @1, @1);
          $$->SetField (pos_AS_OpBody_body, *$1);
          delete $1;
        }
        | LEX_IS LEX_NOT LEX_YET LEX_SPECIFIED
        { $$ = new TYPE_AS_OpBody ();
          MYPARSER::SetPos2(*$$, @1, @4);
          $$->SetField (pos_AS_OpBody_body, Int(NOTYETSPEC));
        }
#if VDMPP
        | LEX_IS LEX_SUBCLASS LEX_RESPONSIBILITY
        { $$ = new TYPE_AS_OpBody ();
          MYPARSER::SetPos2(*$$, @1, @3);
          $$->SetField (pos_AS_OpBody_body, Int(SUBRESP));
        }
#endif //VDMPP
        ;
#endif //FULL

%%

static void yyerror(const char * ErrorMessage)
{
  MYPARSER::Report (TBWSTR::string2wstring(ErrorMessage).c_str());
}

TYPE_CI_ContextId MYPARSER::SetPos(YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last)
{
  if (yyltp_middle.pos.get_token_uast().GetValue() == -1)
  {
    return NilContextId;
  }

  if (MYPARSER::FileId.GetValue() >= 0) {
    TYPE_CI_ContextId cid (MYPARSER::pars_context_info_p->Push(MYPARSER::FileId));

    int st = yyltp_first.pos.get_token_ust();
    int ast = yyltp_middle.pos.get_token_uast();
    int end = yyltp_last.pos.get_token_uend();

  if (st > end)
  {
    int new_st = end + 1;
    int new_end = st - 1;
    int new_ast = ast;
    if (new_end < new_st)
      new_end = new_st;

    if (ast == st)
      new_ast = new_st;
    if (new_ast < new_st)
      new_ast = new_st;
    else if(new_ast > new_end)
      new_ast = new_end;

    TYPE_CI_TokenSpan ts;
    ts.Init(Int(new_st), Int(new_ast), Int(new_end));

    MYPARSER::pars_context_info_p->SetPos(cid, ts);

    return TYPE_CI_ContextId(cid);
  }
//
    TYPE_CI_TokenSpan ts;
    ts.Init(Int(st), Int(ast), Int(end));

    MYPARSER::pars_context_info_p->SetPos(cid, ts);

    return TYPE_CI_ContextId(cid);
  }
  else {
    return NilContextId;
  }
}

TYPE_CI_ContextId MYPARSER::SetPos3(Record & node, YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last)
{
  TYPE_CI_ContextId cid (SetPos(yyltp_first, yyltp_middle, yyltp_last));
  node.SetField(node.Length(), cid);
  return cid;
}

TYPE_CI_ContextId MYPARSER::SetPos2(Record & node, YYLTYPE & yyltp, YYLTYPE & yyltp_last)
{
  return SetPos3(node, yyltp, yyltp, yyltp_last);
}

TYPE_AS_BoolLit MYPARSER::GetAstTrue()
{
  TYPE_AS_BoolLit b;
  b.Init(Bool (true), NilContextId);
  return b;
}

#ifdef FULL
void MYPARSER::AddModOrder(const TYPE_AS_Name & nm, bool trailingtex)
{
#ifdef VDMSL
  Record modinfo (TAG_TYPE_PRETTY_PRINTER_ModOrder, 3);  // create a new record
  modinfo.SetField (1, MYPARSER::orderly);     // order of definitions
  modinfo.SetField (2, Bool(trailingtex));      // trailing tex
  modinfo.SetField (3, nm);                     // Module name
  MYPARSER::modorder.ImpAppend (modinfo);
#endif // VDMSL
#ifdef VDMPP
  Record classinfo (TAG_TYPE_PRETTY_PRINTER_ClassOrder, 3);
  classinfo.SetField (1, MYPARSER::orderly);   // order of definitions
  classinfo.SetField (2, Bool(trailingtex));    // trailing tex
  classinfo.SetField (3, nm);                   // Class name
  MYPARSER::modorder.ImpAppend (classinfo);
#endif // VDMPP

  MYPARSER::orderly.Clear();
}
#endif // FULL

TYPE_AS_Ids MYPARSER::GetIdentifier(const string & idname)
{
  return TYPE_AS_Ids().ImpAppend(SEQ<Char>(TBWSTR::string2wstring(idname)));
}

//** Report *****************************************************************/
//** Is called whenever an error occurred because of lack of a keyword.    **/
//** What is done is simply a call to Error.Store with the proper args.    **/
//** Should it be a macro ?????????????????????????????????????????????    **/
void MYPARSER::Report (const wchar_t * WhatIsWrong)
{
  YYLTYPE pos;
  // FIXME: temporary solution to get real line and column output
  pos.first_line = SCANNER::GetLineCount();
  pos.first_column = SCANNER::GetColumnCount();
  Report(WhatIsWrong, pos);
}

void MYPARSER::Report (const wchar_t * WhatIsWrong, YYLTYPE& pos)
{
//  Report(WhatIsWrong, &pos);
  errors = true;

  Sequence s;
  s.ImpAppend(SEQ<Char>(WhatIsWrong));

  TYPE_ProjectTypes_ErrMsg rc;
  rc.Init(MYPARSER::FileId, Int(pos.first_line), Int(pos.first_column), s);
  Errsp->vdm_AddMsg(rc);
}

void MYPARSER::Report (const wchar_t * WhatIsWrong, YYLTYPE& pos, YYLTYPE&pos2)
{
  Report(WhatIsWrong, pos);
  Report(L"Here is the other", pos2);
}

void MYPARSER::CheckNoDelimiter(YYLTYPE& pos)
{
  TokenList * tl = SCANNER::GetTokenList();
  if (tl != NULL)
  {
    int p = pos.pos.get_token_ust();
    const TokenInfo & ti1 = tl->Get(p);
    const TokenInfo & ti2 = tl->Get(p+1);

    if ((ti1.get_abs_line_end() != ti2.get_abs_line_st()) || (ti1.get_col_end() != ti2.get_col_st()))
      Report(L"no delimiter is allowed", pos);
  }
}

#ifndef NO_RECOVERY
static int recover_yytranslate(int yyc) { return YYTRANSLATE(yyc); }

static VDM_Recover vdm_repair(YYLAST,
                              YYFINAL,
#ifdef USE_BISON128
                              YYFLAG,
#else
                              YYPACT_NINF,
#endif // USE_BISON128
                              YYNTOKENS,
                              recover_yytranslate,
                              yyr1,
                              yyr2,
                              yydefact,
                              yydefgoto,
                              yypact,
                              yypgoto,
                              yytable,
                              yycheck,
                              YYINITDEPTH,
                              YYMAXUTOK);

static VDM_Recover& get_repair() { return vdm_repair; }

#ifdef FULL
wstring MYPARSER::GetTokenText(int token) { return vdm_repair.GetString(token); }
#endif // FULL
#endif // RECOVERY

//////////////////////////////
/// Name: StateDesignator2Expr
/// Desc: This functions take an StateDesignator
///       node and transforms it to an expression.
///       The operation is used by the rule CallOrAssignStmt.
///       Basically, the operation tranvers the AST replaces
///       all MapOrSeqRef node with a ApplyExpr node, and
///       all FieldRef node with a FieldSelectExpr node.
///
//////////////////////////////
TYPE_AS_Expr MYPARSER::StateDesignator2Expr(const TYPE_AS_StateDesignator & sd)
{
  switch(sd.GetTag()) {
    case TAG_TYPE_AS_Name:
    case TAG_TYPE_AS_NarrowRef:
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
    case TAG_TYPE_AS_NewExpr:
#endif // VDMPP
    case TAG_TYPE_AS_FctTypeInstExpr: {
      return sd;
    }
    case TAG_TYPE_AS_FieldRef: {
      TYPE_AS_FieldSelectExpr fse;
      fse.Init(StateDesignator2Expr(sd.GetField(pos_AS_FieldRef_var)),
               sd.GetField(pos_AS_FieldRef_sel),
               sd.GetField(pos_AS_FieldRef_cid));
      return fse;
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      TYPE_AS_ApplyExpr ae;
      ae.Init(StateDesignator2Expr(sd.GetField(pos_AS_MapOrSeqRef_var)),
              sd.GetField(pos_AS_MapOrSeqRef_arg), //
              sd.GetField(pos_AS_MapOrSeqRef_cid));
      return ae;
    }
    default:
      return TYPE_AS_Expr();
  }
}

////////////////////////////////////
/// Name: ProperSDInAsgnStmt
/// Description: ProperSDInAsgnStmt checks if the
///              the state designator structure (sd)
///              that is given as input has a proper
///              structure to be a state designator
///              in a assignstatement.
///              Proper means here: the first element
///              must not be a NexExpr and the expression
///              list in MapOrSeqRef must have length zero or 1.
///              If sd is a proper state designator the
///              function will return the state designator in
///              L"newsd" such that all MapOrSeqRef now has
///              a record and not a sequence in its argument list (field
////////////////////////////////////
// ProperSDInAsgnStmt
// sd : AS`StateDesignator
// ==> bool * [seq of char] * [AS`StateDesignator]
Tuple MYPARSER::ProperSDInAsgnStmt(const TYPE_AS_StateDesignator & sd)
{
  switch (sd.GetTag()) {
    case TAG_TYPE_AS_Name: {
      return mk_(Bool(true), Nil(), sd);
    }
    case TAG_TYPE_AS_FieldRef: {
      const TYPE_AS_StateDesignator & var (sd.GetRecord(pos_AS_FieldRef_var));
#ifdef VDMPP
      if (var.Is(TAG_TYPE_AS_SelfExpr))
        return mk_(Bool(true), Nil(), sd);
#endif // VDMPP
      Tuple psdias (ProperSDInAsgnStmt(var));
      if (psdias.GetBoolValue(1)) {
        TYPE_AS_StateDesignator newsd (sd);
        newsd.SetField(pos_AS_FieldRef_var, psdias.GetRecord(3));
        return mk_(Bool(true), Nil(), newsd);
      }
      else
        return mk_(Bool(false), psdias.GetSequence(2), Nil());
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      Sequence arg (sd.GetSequence(pos_AS_MapOrSeqRef_arg));
      switch(arg.Length()) {
        case 0: {
          return mk_(Bool(false), Sequence(L"State designator has no argument"), Nil());
        }
        case 1: {
          Tuple psdias (ProperSDInAsgnStmt(sd.GetField(pos_AS_MapOrSeqRef_var)));
          if (psdias.GetBoolValue(1)) {
            TYPE_AS_StateDesignator newsd (sd);
            newsd.SetField(pos_AS_MapOrSeqRef_var, psdias.GetRecord(3));
            newsd.SetField(pos_AS_MapOrSeqRef_arg, arg.Hd()); //
            return mk_(Bool(true), Nil(), newsd);
          }
          else {
            return mk_(Bool(false), psdias.GetSequence(2), Nil());
          }
        }
        default: {
          return mk_(Bool(false), Sequence(L"State designator has too many arguments"), Nil());
        }
      }
    }
    case TAG_TYPE_AS_NarrowRef: {
// 20151023 -->
      //Tuple psdias (ProperSDInAsgnStmt(sd.GetRecord(pos_AS_NarrowExpr_expr)));
//      TYPE_AS_StateDesignator tsd (sd.GetRecord(pos_AS_NarrowExpr_expr));
//       if (tsd.Is(TAG_TYPE_AS_ApplyExpr)) {
//         TYPE_AS_MapOrSeqRef msr;
//         msr.Init(tsd.GetRecord(pos_AS_MapOrSeqRef_var),
//                  tsd.GetRecord(pos_AS_MapOrSeqRef_arg),
//                  tsd.GetInt(pos_AS_MapOrSeqRef_cid));
//         tsd = msr;
//       }
//      Tuple psdias (ProperSDInAsgnStmt(tsd));
      Tuple psdias (ProperSDInAsgnStmt(sd.GetRecord(pos_AS_NarrowRef_var)));
// <-- 20151023
      if (psdias.GetBoolValue(1)) {
        TYPE_AS_StateDesignator newsd (sd);
        newsd.SetField(pos_AS_NarrowExpr_expr, psdias.GetRecord(3));
        return mk_(Bool(true), Nil(), newsd);
      }
      else
        return mk_(Bool(false), psdias.GetSequence(2), Nil());
    }
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr: {
      return mk_(Bool(false), Sequence(L"New expression in state designator is not legal"), Nil());
    }
    case TAG_TYPE_AS_SelfExpr: {
      return mk_(Bool(false), Sequence(L"Self expression in state designator is not legal"), Nil());
    }
#endif // VDMPP
    default: {
wcout << sd << endl;
      return mk_(Bool(false), Sequence(L"Internal error"), Nil());
    }
  }
}

#ifdef FULL
bool MYPARSER::DomOverlap(const Map & m1, const Map & m2)
{
  Set dom_m1 (m1.Dom());
  Set dom_m2 (m2.Dom());
  Generic g1, g2;
  for(bool b1 = dom_m1.First (g1); b1; b1 = dom_m1.Next (g1))
  {
    Generic h1 (m1[g1]);
    for(bool b2 = dom_m2.First (g2); b2; b2 = dom_m2.Next (g2))
    {
      Generic h2 (m2[g2]);
#ifdef VDMPP
      if (h1.IsRecord() && h2.IsRecord())
      {
        switch(Record(h1).GetTag()) {
          case TAG_TYPE_AS_ExplOpDef:
          case TAG_TYPE_AS_ImplOpDef:
          case TAG_TYPE_AS_ExtExplOpDef:
          case TAG_TYPE_AS_ExplFnDef:
          case TAG_TYPE_AS_ImplFnDef:
          case TAG_TYPE_AS_ExtExplFnDef:
          {
            TYPE_AS_Name mangledName1 (g1);
            TYPE_AS_Name mangledName2 (g2);
            if(!MANGLE::IsMangled(g1))
              mangledName1 = MANGLE::Mangle(g1, MANGLE::MethType2Seq(GetMethType(h1)));
            if(!MANGLE::IsMangled(g2))
              mangledName2 = MANGLE::Mangle(g2, MANGLE::MethType2Seq(GetMethType(h2)));
            if (mangledName1 == mangledName2)
              return true;
            else
              continue;
          }
        }
      }
#endif // VDMPP
      if (g1 == g2)
      {
        return true;
      }
    }
  }
  return false;
}

Sequence MYPARSER::ReportNameOverlapInDefs(const Set & nms, const wstring & single, const wstring & plural)
{
  switch(nms.Card()) {
    case 0: {
      return Sequence();
    }
    case 1: {
      wstring str(L"The name of ");
      str += single;
      str += L" `";
      str += ASTAUX::ASName2String(nms.GetElem());
      str += L"' is multiple defined";
      return Sequence(str);
    }
    default: {
      wstring str(L"The names of ");
      str += plural;
      str += L": ";
      Set nms_q (nms);
      Generic nm_g;
      for (bool bb = nms_q.First(nm_g); bb; bb=nms_q.Next(nm_g) ) {
        str += ASTAUX::ASName2String(nm_g);
        str += L" ";
      }
      str += L"are multiple defined";
      return Sequence(str);
    }
  }
}

Tuple MYPARSER::NameOverlapInDefs(const TYPE_AS_Definitions & defblock1,
                                  const Map & def_map,
                                  const wstring & single,
                                  const wstring & plural)
{
  Set names;
  names.ImpUnion(defblock1.GetMap(pos_AS_Definitions_typem).Dom()); //--typem

#ifdef VDMPP
  Set names2;
  names2.ImpUnion(defblock1.GetMap(pos_AS_Definitions_typem).Dom()); //--typem
#endif // VDMPP

#if VDMSL
  if (! (defblock1.GetField(pos_AS_Definitions_State)).IsNil()) { //-- StateDef
    const TYPE_AS_StateDef & state (defblock1.GetField(pos_AS_Definitions_State));
    const TYPE_AS_CompositeType & tp (state.GetRecord(pos_AS_StateDef_tp));  //-- tp: CompositeType
    const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));//-- fields: seq of Field
    size_t len_fields = fields.Length();
    for (size_t idx = 1; idx <= len_fields; idx++) {
      names.Insert(fields[idx].GetField(pos_AS_Field_sel));
    }
  }
#endif // VDMSL
  // Note this function does not check for value names.

#ifdef VDMSL
  names.ImpUnion(defblock1.GetMap(pos_AS_Definitions_fnm).Dom()); // functions
  names.ImpUnion(defblock1.GetMap(pos_AS_Definitions_opm).Dom()); // operations
#endif // VDMSL
#ifdef VDMPP
  const Map & fnm_m (defblock1.GetMap(pos_AS_Definitions_fnm));
  const Map & opm_m (defblock1.GetMap(pos_AS_Definitions_opm));
  Set dom_fnm_m (fnm_m.Dom());
  Generic g1;
  for(bool bb = dom_fnm_m.First(g1); bb; bb = dom_fnm_m.Next(g1))
  {
    if(MANGLE::IsMangled(g1))
      names.Insert(g1);
    else
      names.Insert(MANGLE::Mangle(g1, MANGLE::MethType2Seq(GetMethType(fnm_m[g1]))));

    if (single == L"operation")
    {
      if(MANGLE::IsMangled(g1))
        names2.Insert(MANGLE::GetUnmangledName(g1));
      else
        names2.Insert(g1);
    }
  }
  Set dom_opm_m (opm_m.Dom());
  Generic g2;
  for(bool cc = dom_opm_m.First(g2); cc; cc = dom_opm_m.Next(g2))
  {
    if(MANGLE::IsMangled(g2))
      names.Insert(g2);
    else
      names.Insert(MANGLE::Mangle(g2, MANGLE::MethType2Seq(GetMethType(opm_m[g2]))));

    if (single == L"function")
    {
      if(MANGLE::IsMangled(g2))
        names2.Insert(MANGLE::GetUnmangledName(g2));
      else
        names2.Insert(g2);
    }
  }
#endif // VDMPP

#ifdef VDMSL
  Set nms2 (def_map.Dom()); //checking types
#endif // VDMSL
#ifdef VDMPP
  Set dom_def_map (def_map.Dom());
  Set nms2, nms3; //checking types
  Generic g3;
  for(bool dd = dom_def_map.First(g3); dd; dd = dom_def_map.Next(g3))
  {
    Generic h3 (def_map[g3]);
    if (h3.IsRecord())
    {
      switch(Record(h3).GetTag()) {
        case TAG_TYPE_AS_ExplOpDef:
        case TAG_TYPE_AS_ImplOpDef:
        case TAG_TYPE_AS_ExtExplOpDef:
        case TAG_TYPE_AS_ExplFnDef:
        case TAG_TYPE_AS_ImplFnDef:
        case TAG_TYPE_AS_ExtExplFnDef:
        {
          if(MANGLE::IsMangled(g3))
          {
            nms2.Insert(g3);
            nms3.Insert(MANGLE::GetUnmangledName(g3));
          }
          else
          {
            nms2.Insert(MANGLE::Mangle(g3, MANGLE::MethType2Seq(GetMethType(h3))));
            nms3.Insert(g3);
          }
          break;
        }
        default: {
          nms2.Insert(g3);
          break;
        }
      }
    }
    else
      nms2.Insert(g3);
  }
#endif // VDMPP

  Set res_nms (nms2.ImpIntersect(names));
  if (! res_nms.IsEmpty())
    return mk_(Bool(true), ReportNameOverlapInDefs(res_nms, single, plural));

#ifdef VDMPP
  if ((single == L"function") || (single == L"operation"))
  {
    Set res_nms2 (nms3.ImpIntersect(names2));
    if (! res_nms2.IsEmpty())
      return mk_(Bool(true), ReportNameOverlapInDefs(res_nms2, single, plural));
  }
#endif // VDMPP

  return mk_(Bool(false), Nil());
}

#ifdef VDMPP
// GetMethType
// meth : AS`FnDef | AS`OpDef
// +> [AS`Type | AS`ParameterTypes]
Generic MYPARSER::GetMethType(const Record & meth)
{
  switch (meth.GetTag()){
    case TAG_TYPE_AS_ExplFnDef: {
      return meth.GetField(pos_AS_ExplFnDef_tp);
      break;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return meth.GetField(pos_AS_ExtExplFnDef_partps);
      break;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return meth.GetField(pos_AS_ImplFnDef_partps);
      break;
    }
    case TAG_TYPE_AS_ExplOpDef: {
      return meth.GetField(pos_AS_ExplOpDef_tp);
      break;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      return meth.GetField(pos_AS_ImplOpDef_partps);
      break;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      return meth.GetField(pos_AS_ExtExplOpDef_partps);
      break;
    }
    default:
      return Nil();
  }
}

// This function checks if the sequence of InstanceVarDef has duplicated
// instance variables.
Tuple MYPARSER::InstVarDomOverlap(const SEQ<TYPE_AS_InstanceVarDef> & instvar_defs)
{
  Set nms;
  size_t len_instvar_defs = instvar_defs.Length();
  for (size_t idx = 1; idx <= len_instvar_defs; idx++)
  {
    const TYPE_AS_InstanceVarDef & ivd (instvar_defs[idx]);
    if (ivd.Is(TAG_TYPE_AS_InstAssignDef) ) {
      const TYPE_AS_AssignDef & ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
      const TYPE_AS_Name & nm (ad.GetRecord(pos_AS_AssignDef_var));
      if (nms.InSet(nm) ) {
        Set rep_nms;
        rep_nms.Insert(nm);
        return mk_(Bool(true),
                   ReportNameOverlapInDefs(rep_nms, wstring(L"instance variable"), wstring(L"instance variables")));
      }
      else {
        nms.Insert(nm);
      }
    }
  }
  return mk_(Bool(false), Nil());
}

// MergeOpFnDef
// m1 : map AS`Name to AS`FnDef | map AS`Name to AS`OpDef
// m2 : map AS`Name to AS`FnDef | map AS`Name to AS`OpDef
// ==> map AS`Name to AS`FnDef | map AS`Name to AS`OpDef
Map MYPARSER::MergeOpFnDef(const Map & m1, const Map & m2)
{
  if(m1.IsEmpty()) return m2;
  if(m2.IsEmpty()) return m1;

  Set dom_m1 (m1.Dom());
  Set names;
  Generic g1;
  for(bool bb = dom_m1.First(g1); bb; bb = dom_m1.Next(g1))
  {
    names.Insert(Record(m1[g1]).GetField(1)); // nm
  }

  Set dom_m2 (m2.Dom());
  Set names2;
  Generic g2;
  for(bool cc = dom_m2.First(g2); cc; cc = dom_m2.Next(g2))
  {
    names2.Insert(Record(m2[g2]).GetField(1)); // nm
  }
  names.ImpIntersect(names2);
  if(names.IsEmpty ())
  {
    Map res (m1);
    res.ImpOverride(m2);
    return res;
  }
  else
  {
    Set dom_m1 (m1.Dom());
    Map res;
    Generic g3;
    for(bool dd = dom_m1.First(g3); dd; dd = dom_m1.Next(g3))
    {
      Generic h3 (m1[g3]);
      TYPE_AS_Name nm (Record(h3).GetField(1));
      if(!names.InSet(nm) || MANGLE::IsMangled(g3))
        res.ImpModify(g3, h3);
      else
      {
        res.ImpModify(MANGLE::Mangle(g3, MANGLE::MethType2Seq(GetMethType(h3))), h3);
      }
    }
    Set dom_m2 (m2.Dom());
    Generic g4;
    for(bool ee = dom_m2.First(g4); ee; ee = dom_m2.Next(g4))
    {
      Generic h4 (m2[g4]);
      TYPE_AS_Name nm (Record(h4).GetField(1));
      if(!names.InSet(nm) || MANGLE::IsMangled(g4))
        res.ImpModify(g4, h4);
      else
      {
        res.ImpModify(MANGLE::Mangle(g4, MANGLE::MethType2Seq(GetMethType(h4))), h4);
      }
    }
    return res;
  }
}

TYPE_AS_Name MYPARSER::GetMangleName(const Record & def)
{
  switch (def.GetTag()) {
    case TAG_TYPE_AS_ImplFnDef: {
      return MANGLE::Mangle(def.GetRecord(pos_AS_ImplFnDef_nm), ASTAUX::GetFnParms(def));
    }
    case TAG_TYPE_AS_ExplFnDef: {
      return MANGLE::Mangle(def.GetRecord(pos_AS_ExplFnDef_nm), ASTAUX::GetFnParms(def));
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return MANGLE::Mangle(def.GetRecord(pos_AS_ExtExplFnDef_nm), ASTAUX::GetFnParms(def));
    }
    case TAG_TYPE_AS_ImplOpDef: {
      return MANGLE::Mangle(def.GetRecord(pos_AS_ImplOpDef_nm), ASTAUX::GetOpParms(def));
    }
    case TAG_TYPE_AS_ExplOpDef: {
      return MANGLE::Mangle(def.GetRecord(pos_AS_ExplOpDef_nm), ASTAUX::GetOpParms(def));
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      return MANGLE::Mangle(def.GetRecord(pos_AS_ExtExplOpDef_nm), ASTAUX::GetOpParms(def));
    }
  }
  return Record(); // dummy
}
#endif // VDMPP

#ifdef VDMSL
Tuple MYPARSER::NameOverlapInStateDef(const TYPE_AS_Definitions & defblock1, const TYPE_AS_StateDef & statedef)
{
  Set names;

  names.ImpUnion(defblock1.GetMap(pos_AS_Definitions_typem).Dom()); //--typem

  if (! (defblock1.get_State ()).IsNil())
  { //-- StateDef
    const TYPE_AS_StateDef & state (defblock1.GetRecord(pos_AS_Definitions_State));
    const TYPE_AS_CompositeType & tp (state.GetRecord(pos_AS_StateDef_tp));  //-- tp: CompositeType
    const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));//-- fields: seq of Field
    size_t len_fields = fields.Length();
    for (size_t idx = 1; idx <= len_fields; idx++)
    {
      const Generic & nm (fields[idx].GetField(pos_AS_Field_sel));
      if (!nm.IsNil()) {
        names.Insert(nm);
      }
    }
  }

  // Note this function does not check for value names.

  names.ImpUnion(defblock1.GetMap(pos_AS_Definitions_fnm).Dom()); // functions
  names.ImpUnion(defblock1.GetMap(pos_AS_Definitions_opm).Dom()); // operations

  Set res_nms;
  const TYPE_AS_CompositeType & tp (statedef.GetRecord(pos_AS_StateDef_tp));
  const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
  size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++)
  {
    const Generic & sel (fields[idx].GetField(pos_AS_Field_sel));
    if (!sel.IsNil() && names.InSet(sel))
      res_nms.Insert(sel);
  }
  if (!res_nms.IsEmpty())
    return mk_(Bool(true), ReportNameOverlapInDefs(res_nms, wstring(L"state"), wstring(L"states")));
  else
    return mk_(Bool(false), Nil());
}
#endif // VDMSL
#endif // FULL

int yyparse();

#if FULL
bool VDMPARSER::pars_vdm_string(const string & str,
                                bool POS_WANTED,           // include position information
                                vdm_Errors * _errs,        // List of errors generated
                                Sequence & res,            // the resulting AST
                                ContextInfo & ci,
                                bool reset_ci,
                                const TYPE_CI_FileId & fid // the file id
                                )
{
  VDMPARSER::orderly.Clear();
  VDMPARSER::modorder.Clear();

  SCANNER::ReadFrom(str);
  return pars_vdm(POS_WANTED, _errs, res, ci, reset_ci, fid);
}

bool VDMPARSER::pars_vdm_stream(istream & in,
                                bool POS_WANTED,           // include position information
                                vdm_Errors * _errs,        // List of errors generated
                                Sequence & res,            // the resulting AST
                                ContextInfo & ci,
                                bool reset_ci,
                                const TYPE_CI_FileId & fid // the file id
                                )
{
  VDMPARSER::orderly.Clear();
  VDMPARSER::modorder.Clear();

  SCANNER::ReadFrom(in);
  return pars_vdm(POS_WANTED, _errs, res, ci, reset_ci, fid);
}

bool VDMPARSER::pars_vdm(bool POS_WANTED,           // include position information
                         vdm_Errors * _errs,        // List of errors generated
                         Sequence & res,            // the resulting AST
                         ContextInfo & ci,
                         bool reset_ci,
                         const TYPE_CI_FileId & fid // the file id
                         )
{
  VDMPARSER::FileId = fid;
  VDMPARSER::pars_context_info_p = &ci;

  ci.UseContextTab(fid, reset_ci);

#if YYDEBUG
  yydebug = 1;
#endif // YYDEBUG

  res.Clear();
  VDMPARSER::asts = &res;

  VDMPARSER::Errsp = _errs;
  VDMPARSER::errors = false;
  SCANNER::InitLex (&VDMPARSER::Report);

#ifndef NO_RECOVERY
  vdm_repair.SetFileId(VDMPARSER::FileId); // What a load of garbage in the two parser.
  vdm_repair.SetErrs(VDMPARSER::Errsp);
  vdm_repair.SetEofText(wstring(L"<end of file>"));
#endif // NO_RECOVERY

  SCANNER::SetTokenList(NULL);  // set the token list
  int result = vdm_repair.PreParse();
  if (result == 0) {
    SCANNER::SetTokenList(&(ci.GetTokenList(VDMPARSER::FileId)));
    SCANNER::InitLex(&VDMPARSER::Report);
    try {
      SCANNER::reread();
      result = yyparse();
    }
    catch (TB_Exception & e) {
      switch (e.GetExType()) {
      default:
         // It must not happen
         vdm_err << L"Internal Error in pars_vdm" << endl;
         break;
      }
    }
  }
  SCANNER::SetTokenList(NULL); // reset the tokenlist
  SCANNER::FreeLexStrings();
  return ((result > 0) || VDMPARSER::errors);
}
#endif //FULL

#if !FULL
#if !VPPSTMT
// EXPR.FUN
     // yacc interface

int yyparse();

bool VDMEXPRPARSER::pars_vdm_expr_string(const string & str,
                                         bool POS_WANTED,
                                         vdm_Errors * _errs,
                                         Sequence & res,
                                         ContextInfo & ci,
                                         bool reset_ci,
                                         const TYPE_CI_FileId & fid)
{
  SCANNER::ReadFrom (str);
  return pars_vdm_expr(POS_WANTED, _errs, res, ci, reset_ci, fid);
}

bool VDMEXPRPARSER::pars_vdm_expr_stream(istream & in,
                                         bool POS_WANTED,
                                         vdm_Errors * _errs,
                                         Sequence & res,
                                         ContextInfo & ci,
                                         bool reset_ci,
                                         const TYPE_CI_FileId & fid)
{
  SCANNER::ReadFrom (in);
  return pars_vdm_expr(POS_WANTED, _errs, res, ci, reset_ci, fid);
}

bool VDMEXPRPARSER::pars_vdm_expr(bool POS_WANTED,
                                  vdm_Errors * _errs,
                                  Sequence & res,
                                  ContextInfo & ci,
                                  bool reset_ci,
                                  const TYPE_CI_FileId & fid)
{
  VDMEXPRPARSER::FileId = fid;
  VDMEXPRPARSER::pars_context_info_p = &ci;

  if (fid != TYPE_CI_FileId(CI_FID_NO_CI))
    ci.UseContextTab(fid, reset_ci);

#ifdef YYDEBUG
  yydebug = 1;
#endif // YYDEBUG

  res.Clear();
  VDMEXPRPARSER::exprs = &res;

  VDMEXPRPARSER::Errsp = _errs;
  VDMEXPRPARSER::errors = false;
  SCANNER::InitLex (&VDMEXPRPARSER::Report);

#ifndef NO_RECOVERY
  vdm_repair.SetFileId(VDMEXPRPARSER::FileId); // What a load of garbage in the two parser.
  vdm_repair.SetErrs(VDMEXPRPARSER::Errsp);
  vdm_repair.SetEofText(wstring(L"<end of expr>"));
#endif // NO_RECOVERY

  SCANNER::SetTokenList(NULL);  // set the token list
  int result = vdm_repair.PreParse();
  if (result == 0) {
    if (VDMEXPRPARSER::FileId != TYPE_CI_FileId(CI_FID_NO_CI))
      SCANNER::SetTokenList(&(ci.GetTokenList(TYPE_CI_FileId(VDMEXPRPARSER::FileId))));
    SCANNER::InitLex(&VDMEXPRPARSER::Report);
    try {
      SCANNER::reread();
      result = yyparse();
    }
    catch (TB_Exception & e) {
      switch (e.GetExType()) {
      default:
        // It must not happen
        vdm_err << L"Internal Error in pars_vdm_expr" << endl;
        break;
      }
    }
  }
  SCANNER::SetTokenList(NULL);  // set the token list
  SCANNER::FreeLexStrings();
  return ((result > 0) || VDMEXPRPARSER::errors);
}
#endif //!VPPSTMT

#ifdef VPPSTMT
bool VDMSTMTPARSER::pars_vpp_asgn_invoke_stmt(const string & str,
                                              bool POS_WANTED,
                                              vdm_Errors * _errs,
                                              TYPE_AS_AssignStmt & res)
{
  VDMSTMTPARSER::FileId = TYPE_CI_FileId(-10);

  res = TYPE_AS_AssignStmt();
  VDMSTMTPARSER::stmt = &res;

  VDMSTMTPARSER::Errsp = _errs;
  VDMSTMTPARSER::errors = false;

  SCANNER::ReadFrom (str);
  SCANNER::InitLex (&VDMSTMTPARSER::Report);

#ifndef NO_RECOVERY
  vdm_repair.SetFileId(VDMSTMTPARSER::FileId);    // What a load of garbage in the two parser.
  vdm_repair.SetErrs(VDMSTMTPARSER::Errsp);
  vdm_repair.SetEofText(wstring(L"<end of expr>"));
#endif // NO_RECOVERY

  int result = 0;
  try {
    result = yyparse();
  }
  catch (TB_Exception & e) {
    switch (e.GetExType()) {
    default:
      // It must not happen
      vdm_err << L"Internal Error in pars_vpp_asgn" << endl;
      break;
    }
  }
  SCANNER::FreeLexStrings();
  return ((result > 0) || VDMSTMTPARSER::errors);
}
#endif //VPPSTMT
#endif //!FULL
