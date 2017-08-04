% This is a nuweb document to be processed by nuweb. -*- mode: text -*-

//%% ID
//%%   $Id: javaparse.w,v 1.23 2007/03/23 14:10:42 vdmtools Exp $
//%% COPYRIGHT
//%%   Kyushu University 2016

\documentclass{article}
\usepackage{toolbox}
\usepackage{latexsym}

\begin{document}

% Comment: at-sign is the command prefix and must be duplicated (@@)
% where is is needed verbatim.

\docdef{Parser converting Java Concrete Syntax to Java Abstract Syntax}
{The VDM-SL Tool Group}
{010205}
{javaparse.w}
{Code documentation}
{In developement}
{Restricted}
{\copyright\ Kyushu University}
{\item[V1.0] First version}
{}

\tableofcontents
\newpage

\section{Introduction}
Explicit dependencies:
javascan.w

Dependencies: (only the most important ones
mod_cppas.vdm
mod_ci.vdm ?

This Bison file parses java source code and issues an (CPP)AST image.
The Java AST is defined using VDM-SL in the file .../spec/mod_cppas.vdm
(dependent of where the CVS module is checked out).

The grammar conforms to the Java grammar described at Sun's homepage in "The Java(TM)
Language Specification. J. Gosling, B. Joy, G. Steele. SECOND EDITION"
We also loked at the (Bison) grammar for Java included in GNU source code package for
gcc: gcc-2.95.2, and at the JavaCup 1.2 specification of a Java grammar.

The following modifications were brought to the original grammar:

Changed rules:

FctBody: added the rule '| CompoundStmt SEMICOLON'
StaticInitializer: added the rule 'STATIC CompoundStmt SEMICOLON'.

Rule 'class_body:' and rule 'interface_body:' SEMICOLON where added as an alternative
to all productions.


Added rule 'ArgumentDeclarationList' to hold 'seq of ArgumentDeclaration' and changed
the productions of rule 'method_declarator:' accordingly

Rule 'for_update:' the production '| statement_expression_list' was changed to
'| argument_list' to satisfy the VDM-SL specification of several other subtools.

A rule like L"something_nsi": 'nsi' should be read 'no_short_if'.

\section{Java Grammar Definition}
\label{sec:VersionDef}

@O javaparse.y
@{@<This file is generated@>
@< Includes    @>
@< Defines     @>
@< Tokens      @>
@< Types       @>
@< Productions @>
@}


@d Includes @{
%{
#include <stdlib.h>
#if (!defined(__APPLE_CC__) && !defined(__FreeBSD__))
#include <malloc.h>
#endif
#include <stdio.h>
#include <string.h>

#include "metaiv.h"      /* Basic Meta-IV/VDM data types and operations */
#include "CPP_anonym.h"
#include "CPP.h"
#include "Errors.h"
#include "contextinfo.h"
#include "CI.h"
#include "tb_exceptions.h"
#include "tbutils.h"
#include "javascan.h"
#include "tb_wstring.h"

@}


@d Defines @{

//#define YY LSP _ NEEDED

static TYPE_CPP_File* javafile;
static Generic dirname = Nil();
static Generic filename = Nil();

//extern int javapparse ();

static void set_file_and_dir_name (const wstring &);
static int ConvRealLit (const wchar_t*, double&);
static SEQ<TYPE_CPP_InitDeclarator> move_SBs_2_InitDeclarator (const TYPE_CPP_ArrayNewDecl &, const Sequence &);
static TYPE_CPP_ArrayDecl move_SBs_2_ArrayDecl (const TYPE_CPP_ArrayNewDecl &, const Record &);
static Generic move_dims_2_ArrayAbsDecl (const TYPE_CPP_ArrayNewDecl &);
static void move_SBs_2_ArraySpecifiers (const TYPE_CPP_ArrayNewDecl &, Sequence &);

static void javaperror (const char *);

//#define YYDEBUG 1
//extern int yydebug;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

static TYPE_CI_FileId javapFileId;
static vdm_Errors * javaErrs;
static ContextInfo * javap_context_info_p;

//////////////////////////////////////////////////
// These two defines are put into the bison generated parser code
// by the 'fixposassign' script.

#define YYLSP_SETPOS_EMPTYRULE javapsetpos_emptyrule(yylsp, yylloc);

static void javapsetpos_emptyrule (YYLTYPE * yylsp, const YYLTYPE & yylloc)
{
  yylsp -> first_line = yylloc.first_line;
  yylsp -> first_column = yylloc.first_column;
  yylsp -> pos.set_token_ust(yylloc.pos.get_token_ust());
  yylsp -> pos.set_token_uast(yylloc.pos.get_token_uast()); // 20090915
  yylsp -> last_line = (yylsp-1) -> last_line;
  yylsp -> last_column = (yylsp-1) -> last_column;
  yylsp -> pos.set_token_uend((yylsp-1) -> pos.get_token_uend());
  yylsp -> text = 0;
}

#define YYLSP_SETPOS_NONEMPTYRULE javapsetpos_nonemptyrule(yylsp, yylen);

#ifdef USE_BISON128
static void javapsetpos_nonemptyrule (YYLTYPE * yylsp, int yylen)
{
  yylsp -> last_line = (yylsp+yylen-1) -> last_line;
  yylsp -> last_column = (yylsp+yylen-1) -> last_column;
  yylsp -> pos.set_token_uend((yylsp+yylen-1) -> pos.get_token_uend());
}
#endif // USE_BISON128

//////////////////////////////////////////////////

static void javapSetPos3 (Record & node, YYLTYPE & yyltp_first, YYLTYPE & yyltp_middle, YYLTYPE & yyltp_last)
{
  if (javap_context_info_p && javapFileId.GetValue() >= 0)
  {
    TYPE_CI_ContextId cid (javap_context_info_p -> Push (javapFileId));

    int st = yyltp_first.pos.get_token_ust();
    int ast = yyltp_middle.pos.get_token_uast();
    int end = yyltp_last.pos.get_token_uend();

    TYPE_CI_TokenSpan ts;
    ts.Init(Int(st), Int(ast), Int(end));

    javap_context_info_p -> SetPos (cid, ts);

    node.SetField (node.Length(), Int (cid));
  }
  else
  {
    node.SetField (node.Length(), NilContextId);
  }
}

static void javapSetPos2 (Record & node, YYLTYPE & yyltp, YYLTYPE & yyltp_last)
{
  javapSetPos3 (node, yyltp, yyltp, yyltp_last);
}

/*
static void javapSetPosNilCid (Record* node)
{
  node -> SetField (node -> Length (), NilContextId);
}

static void javapSetPos (Record* node, Generic pos)
{
  node -> SetField (node -> Length (), Int (pos));
}
*/

static bool errors;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

%}
@}

Definition of union moved to L"javascan.h"


@d Tokens @{

%pure-parser

%token <uRecord>  PLUS           MINUS          MULT
%token <uRecord>  DIV            MOD
%token <uRecord>  LEFTSHIFT             RIGHTSHIFT            ZRS
%token <uRecord>  BITAND            EXCLOR            BITOR
%token <uRecord>  AND       OR
%token <uRecord>  EQ             NEQ            GT
%token <uRecord>  GEQ            LT             LEQ
%token <uRecord>  ASPLUS    ASMINUS   ASMULT
%token <uRecord>  ASDIV     ASMOD
%token <uRecord>  ASLEFTSHIFT      ASRIGHTSHIFT     ZRS_ASSIGN
%token <uRecord>  ASBITWISEAND     ASBITWISEEXOR     ASBITWISEINCLOR
%token <uRecord>  PUBLIC         PRIVATE        PROTECTED
%token <uRecord>  STATIC         FINAL          SYNCHRONIZED
%token <uRecord>  VOLATILE       TRANSIENT      NATIVE
%token <uRecord>  ABSTRACT       STRICTFP
%token <uRecord>  PMINUS           PPLUS
%token <uRecord>  DEFAULT        IF             THROW
%token <uRecord>  BOOLEAN        DO             IMPLEMENTS
%token <uRecord>  THROWS         BREAK          IMPORT
%token <uRecord>  ELSE           INSTANCEOF     RETURN
%token <uRecord>  VOID           CATCH          INTERFACE
%token <uRecord>  CASE           EXTENDS        FINALLY
%token <uRecord>  SUPER          WHILE          CLASS
%token <uRecord>  SWITCH         TRY
%token <uRecord>  FOR            NEW            CONTINUE
%token <uRecord>  PACKAGE        THIS
%token <uRecord>  INT
%token <uRecord>  LONG           CHAR
%token <uRecord>  FLOAT          DOUBLE
%token <uRecord>  REL_QM         COLON         COMPL
%token <uRecord>  NEG
%token <uRecord>  ASEQUAL
%token <uRecord>  ELLIPSIS
%token <uRecord>  OP             CP
%token <uRecord>  OSB            CSB
%token <uRecord>  SEMICOLON             COMMA              DOT
%token <uRecord>  COMMENT
%token <uIntegerLit>  IntegerLit
%token <CharacterLit> CharacterLit
%token <FloatingLit>  FloatingLit
%token <StringLit>    StringLit
%token <BoolLit>      BoolLit
%token <NullLit>      NullLit
%token <uRecord>  TRUE           FALSE
%token <uBool>        BYTE
%token <uShort>       SHORT
%token <uIdentifier>  IDENTIFIER
%token <uSequence>  OCB CCB
@}

@d Types @{

%type <uSequence> ImportDeclarations
                  type_declarations
                  Modifiers
                  class_body_declarations
                  interface_member_declarations
                  class_body
                  interface_body
                  class_type_list
                  interface_type_list
                  interfaces
                  extends_interfaces
                  argument_list
                  variable_declarators
                  variable_initializers
                  block_statements
                  ArgumentDeclarationList
                  baseclass
                  wildcards
                  wildcard_type_list
                  catches
                  throws
                  for_begin
                  for_init
                  statement_expression_list
                  for_update
                  SwitchBlockStmtGrps

//%type <uSimpleTypeName> SimpleTypeName
//%type <uSimpleTypeName> reference_type
//%type <uArrayDecl> array_type

%type <uSimpleTypeName> primitive_type
%type <uSimplePackageName> SimplePackageName
%type <uIdentifier> Identifier
                    label_decl
%type <uPackageDeclaration> PackageDeclaration
%type <uQualifiedPackageName> QualifiedPackageName
%type <uPackageName> PackageName
                     class_type
                     interface_type
%type <uImportDeclaration> ImportDeclaration
%type <uSingleTypeImportDeclaration> SingleTypeImportDeclaration
%type <uTypeImportOnDemandDeclaration> TypeImportOnDemandDeclaration
%type <uTypeSpecifier> class_declaration
                       interface_declaration
                       constant_declaration
                       abstract_method_declaration
%type <uRecord> type_declaration
%type <uDeclarator> method_declarator
%type <uFctDecl> constructor_declarator
%type <uFctBody> FctBody
                 constructor_body
%type <uFunctionDefinition> FunctionDefinition
                            method_header
                            constructor_declaration
                            constructor_header
%type <uModifier> Modifier
%type <uArgumentDeclaration> ArgumentDeclaration
%type <uMemberDeclaration> class_member_declaration
                           interface_member_declaration
%type <uDeclarator> variable_declarator_id
%type <uArrayDecl> ArrayDecl
%type <uIdentDeclaration> field_declaration
%type <uInitDeclarator> variable_declarator

%type <uDeclarationStmt> local_variable_declaration_statement
                         local_variable_declaration

%type <uExpr> argument
              left_hand_side
              assignment_expression
              unary_expression
              primary
              postfix_expression
              primary_no_new_array
              unary_expression_not_plus_minus
              constant_expression
              expression
              switch_expression
              variable_initializer
              dim_expr
              statement_expression

%type <uClassInstanceCreationExpr> ClassInstanceCreationExpr

%type <uArrayCreationExpr> ArrayCreationExpr
%type <uArrayInitializer>  ArrayInitializer

%type <uStmt> block_statement statement statement_nsi statement_without_trailing_substatement

%type <uExpressionStmt> ExpressionStmt explicit_constructor_invocation

%type <uLabelStmt>        LabelStmt labeled_statement_nsi
%type <uCompoundStmt>     CompoundStmt finally
%type <uIfStmt>           if_then_statement if_then_else_statement if_then_else_statement_nsi
%type <uSwitchStmt>         SwitchStmt
%type <uSwitchBlock>        SwitchBlock
%type <uSwitchBlockStmtGrp> SwitchBlockStmtGrp
%type <uSwitchLabels>       SwitchLabels
%type <uStmt>               CaseStmt
%type <uWhileStmt> while_expression while_statement while_statement_nsi
%type <uDoWhileStmt> DoWhileStmt
%type <uForStmt> for_statement for_statement_nsi
%type <uForEachStmt> foreach_statement
%type <uBreak>            Break
%type <uContinue>         Continue
%type <uReturn>           Return
%type <uSynchronizedStmt> SynchronizedStmt
%type <uTryBlock>         TryBlock
%type <uEmptyStmt>        EmptyStmt

%type <uAssignExpr>         assignment
%type <uConditionalExpr>    ConditionalExpr
%type <uLogicalExpr>        conditional_and_expression conditional_or_expression
%type <uBitExpr>            and_expression exclusive_or_expression inclusive_or_expression
%type <uEqualityExpr>   EqualityExpr
%type <uRelationalExpr> RelationalExpr
%type <uTypeCompExpr>   TypeCompExpr
%type <uShiftExpr>      ShiftExpr
%type <uBinExpr> multiplicative_expression additive_expression
%type <uCastExpr> CastExpr
%type <uPreInDecrementExpr> PreInDecrementExpr
%type <uFctCall> FctCall
%type <uArrayApply> ArrayApply
%type <uStaticInitializer> StaticInitializer
%type <uThrowExpression> ThrowExpression
%type <uLiteral> Literal
%type <uAssignOp> AssignOp
%type <uHandler> catch_clause
%type <uExceptionDeclaration> ExceptionDeclaration
%type <uPostFixInDecrementExpr> post_increment_expression post_decrement_expression
%type <uRecord> compilation_unit
%type <uExpr> something_dot_new
%type <uObjectMemberAccess> field_access

%type <uArrayNewDecl> dim_exprs dims osb_csb_s osb_csb_s_1
%%

@}

19.2 Production from 2.3: The Syntactic Grammar

@d Productions @{
goal:
  compilation_unit
    {}
;
@}

19.3 Productions from 3: Lexical structure

  Literal = IntegerLit | CharacterLit | FloatingLit | StringLit | BoolLit | NullLit

  IntegerLit ::
    val : int
    cid :- CI`ContextId;

  CharacterLit ::
    val : wchar_t
    cid :- CI`ContextId;

  FloatingLit ::
    val : real
    cid :- CI`ContextId;

  StringLit ::
    val : seq of wchar_t
    cid :- CI`ContextId;

  BoolLit ::
    val : bool
    cid :- CI`ContextId;

  NullLit ::
    cid :- CI`ContextId;

The 'literal' rule is of type: Expr

@d Productions @{
Literal:
  IntegerLit
    {
      TYPE_CPP_IntegerLit* val = new TYPE_CPP_IntegerLit();
      javapSetPos2 (*val, @@1, @@1);
      double value;
      string str (JAVASCANNER::GetPTextString());
      wstring s (TBWSTR::string2wstring(str.c_str()));
      if (ConvRealLit (s.c_str (), value))
        val -> set_val (Real (value));
      else
      {
        val -> set_val (Real (0.0));
//      ERROR
      }
      $$ = val;
    }
| CharacterLit
    {
      TYPE_CPP_CharacterLit* val = new TYPE_CPP_CharacterLit();
      val -> set_val (JAVASCANNER::GetCharValue());
      javapSetPos2 (*val, @@1, @@1);
      $$ = val;
    }
| FloatingLit
    {
      TYPE_CPP_FloatingLit* val = new TYPE_CPP_FloatingLit();
      javapSetPos2 (*val, @@1, @@1);
      double value;
      string str (JAVASCANNER::GetPTextString());
      wstring s (TBWSTR::string2wstring(str.c_str()));
      if (ConvRealLit (s.c_str (), value))
        val -> set_val (Real (value));
      else
      {
        val -> set_val (Real (0.0));
//      ERROR
      }
      val -> set_val (Real (value));
      $$ = val;
    }
| StringLit
    {
      TYPE_CPP_StringLit* val = new TYPE_CPP_StringLit();
      val -> set_val (JAVASCANNER::GetStringValue());
      javapSetPos2 (*val, @@1, @@1);
      $$ = val;
    }
| BoolLit
    {
      TYPE_CPP_BoolLit* val = new TYPE_CPP_BoolLit();
      val -> set_val (JAVASCANNER::GetBooleanValue());
      javapSetPos2 (*val, @@1, @@1);
      $$ = val;
    }
| NullLit
    {
      TYPE_CPP_NullLit* val = new TYPE_CPP_NullLit();
      javapSetPos2 (*val, @@1, @@1);
      $$ = val;
    }
;
@}

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
19.4 Productions from 4: Types, Values and Variables
  SimpleTypeName = PackageName
                 | Bool | Void | Char | Short | Int | Long | Float | Double;

SimpleTypeName:
  reference_type
    {}
| primitive_type
    {}
;

reference_type:
  PackageName
    {}
This 'sub rule'?'production rule' is NOT part of the CPPAS specification, so if we get one of these,
we have to ... JR

| array_type
    {}
;
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

@d Productions @{

primitive_type:
  BOOLEAN
    {
      TYPE_CPP_Bool* b = new TYPE_CPP_Bool ();
      javapSetPos2 (*b, @@1, @@1);
      $$ = b;
    }
| VOID
    {
      TYPE_CPP_Void* v = new TYPE_CPP_Void ();
      javapSetPos2 (*v, @@1, @@1);
      $$ = v;
    }
| CHAR
    {
      TYPE_CPP_Char* c = new TYPE_CPP_Char ();
      javapSetPos2 (*c, @@1, @@1);
      $$ = c;
    }
| BYTE
    {
      TYPE_CPP_Byte* b = new TYPE_CPP_Byte ();
      javapSetPos2 (*b, @@1, @@1);
      $$ = b;
    }
| SHORT
    {
      TYPE_CPP_Short* s = new TYPE_CPP_Short ();
      javapSetPos2 (*s, @@1, @@1);
      $$ = s;
    }
| INT
    {
      TYPE_CPP_Int* i = new TYPE_CPP_Int ();
      javapSetPos2 (*i, @@1, @@1);
      $$ = i;
    }
| LONG
    {
      TYPE_CPP_Long* l = new TYPE_CPP_Long ();
      javapSetPos2 (*l, @@1, @@1);
      $$ = l;
    }
| FLOAT
    {
      TYPE_CPP_Float* f = new TYPE_CPP_Float ();
      javapSetPos2 (*f, @@1, @@1);
      $$ = f;
    }
| DOUBLE
    {
      TYPE_CPP_Double* d = new TYPE_CPP_Double ();
      javapSetPos2 (*d, @@1, @@1);
      $$ = d;
    }
;


class_type:
  PackageName
    {}
;

interface_type:
  PackageName
    {}
;
@}

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
array_type:
  primitive_type OSB CSB
    {
      $$ = new TYPE_CPP_ArrayDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| PackageName OSB CSB
    {
      $$ = new TYPE_CPP_ArrayDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
/* Not part of 'java12.cup' ? */
| array_type OSB CSB
    {
      $$ = new TYPE_CPP_ArrayDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_expr (Nil());
      javapSetPos2 (*$$, @@1, @@3);
    }
| primitive_type OSB error
    {
      javaperror ("']' expected");
      return -1;
    }
| array_type OSB error
    {
      javaperror ("']' expected");
      return -1;
    }
;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

19.5 Productions from 6: Names

  PackageName = SimplePackageName | QualifiedPackageName

@d Productions @{

PackageName:
  SimplePackageName
    {}
| QualifiedPackageName
    {}
;
@}

  SimplePackageName::
    id : Identifier;

@d Productions @{

SimplePackageName:
  Identifier
    {
      $$ = new TYPE_CPP_SimplePackageName ();
      $$ -> set_id (*$1);
      delete $1;
      javapSetPos2 (*$$, @@1, @@1);
    }
;
@}

  QualifiedPackageName::
    pn: PackageName
    id: Identifier

@d Productions @{

QualifiedPackageName:
  PackageName DOT Identifier
    {
      $$ = new TYPE_CPP_QualifiedPackageName ();
      $$ -> set_pn (*$1);
      delete $1;
      $$ -> set_id (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
;
@}

  Identifier ::
    id : seq1 of wchar_t
    CNTXT;

@d Productions @{

Identifier:
  IDENTIFIER
    {
      $$ = new TYPE_CPP_Identifier ();
      $$ -> set_id ((const SEQ<Char>) Sequence (TBWSTR::string2wstring (JAVASCANNER::GetPTextString())));
      javapSetPos2 (*$$, @@1, @@1);
    }
;
@}

19.6: Production from 7: Packages

  File ::
    dirname  : [FileName]
    filename : FileName
    header   : [Header]
    ast      : CPPAS;
    CNTXT;

  FileName = seq1 of wchar_t;

  Header = PackageAndImportDeclarations

  PackageAndImportDeclarations ::
    pd: [PackageDeclaration]
    ims: ImportDeclarations
    CNTXT;

  CPPAS = seq of (Declaration)  -- ONLY ONE ELEMENT !

  Declaration = IdentDeclaration | FunctionDefinition;

only IdentDeclaration in this place

@d Productions @{

compilation_unit:
    {}
| PackageDeclaration
    {
      TYPE_CPP_PackageAndImportDeclarations header;
      header.set_pd (*$1);
      delete $1;
      header.set_ims ((const SEQ<Record>) Sequence ());
      javapSetPos2 (header, @@1, @@1);
      TYPE_CPP_IdentDeclaration declaration;
      declaration.set_ds ((const SEQ<Record>) Sequence ());
      declaration.set_dl (Nil());
      declaration.set_cid (-1);
      SEQ<Record> ast;
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (header);
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@1);
    }
| ImportDeclarations
    {
      TYPE_CPP_PackageAndImportDeclarations header;
      header.set_pd (Nil ());
      header.set_ims ((const SEQ<Record>) *$1);
      delete $1;
      javapSetPos2 (header, @@1, @@1);
      TYPE_CPP_IdentDeclaration declaration;
      declaration.set_ds ((const SEQ<Record>) Sequence ());
      declaration.set_dl (Nil ());
      declaration.set_cid (-1);
      SEQ<Record> ast;
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (header);
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@1);
    }
| type_declarations
    {
      TYPE_CPP_IdentDeclaration declaration;
      declaration.set_ds ((const SEQ<Record>) *$1);
      delete $1;
      declaration.set_dl (Nil ());
      javapSetPos2 (declaration, @@1, @@1);
      SEQ<Record> ast;
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (Nil ());
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@1);
    }
| PackageDeclaration ImportDeclarations
    {
      TYPE_CPP_PackageAndImportDeclarations header;
      header.set_pd (*$1);
      delete $1;
      header.set_ims ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (header, @@1, @@2);
      TYPE_CPP_IdentDeclaration declaration;
      declaration.set_ds ((const SEQ<Record>) Sequence());
      declaration.set_dl (Nil ());
      declaration.set_cid (-1);
      SEQ<Record> ast;
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (header);
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@2);
    }
| PackageDeclaration type_declarations
    {
      TYPE_CPP_PackageAndImportDeclarations header;
      header.set_pd (*$1);
      delete $1;
      header.set_ims ((const SEQ<Record>) Sequence ());
      javapSetPos2 (header, @@1, @@1);
      TYPE_CPP_IdentDeclaration declaration;
      declaration.set_ds ((const SEQ<Record>) *$2);
      delete $2;
      declaration.set_dl (Nil ());
      javapSetPos2 (declaration, @@2, @@2);
      SEQ<Record> ast;
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (header);
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@2);
    }
| ImportDeclarations type_declarations
    {
      TYPE_CPP_PackageAndImportDeclarations header;
      TYPE_CPP_IdentDeclaration declaration;
      SEQ<Record> ast;
      header.set_pd (Nil());
      header.set_ims ((const SEQ<Record>) *$1);
      delete $1;
      javapSetPos2 (header, @@1, @@1);
      declaration.set_ds ((const SEQ<Record>) *$2);
      delete $2;
      declaration.set_dl (Nil());
      javapSetPos2 (declaration, @@2, @@2);
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (header);
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@2);
    }
| PackageDeclaration ImportDeclarations type_declarations
    {
      TYPE_CPP_PackageAndImportDeclarations header;
      header.set_pd (*$1);
      delete $1;
      header.set_ims ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (header, @@1, @@2);
      TYPE_CPP_IdentDeclaration declaration;
      declaration.set_ds ((const SEQ<Record>) *$3);
      delete $3;
      declaration.set_dl (Nil ());
      javapSetPos2 (declaration, @@3, @@3);
      SEQ<Record> ast;
      ast.ImpAppend (declaration);

      javafile -> set_dirname (dirname);
      javafile -> set_name (filename);
      javafile -> set_h (header);
      javafile -> set_ast (ast);
      javapSetPos2 (*javafile, @@1, @@3);
    }
;
@}

  ImportDeclarations = seq of ImportDeclaration;

@d Productions @{

ImportDeclarations:
  ImportDeclaration
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| ImportDeclarations ImportDeclaration
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;

type_declarations:
  type_declaration
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| type_declarations type_declaration
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;
@}

  PackageDeclaration::
    name: PackageName
    CNTXT;

@d Productions @{
PackageDeclaration:
  PACKAGE PackageName SEMICOLON
    {
      $$ = new TYPE_CPP_PackageDeclaration ();
      $$ -> set_name (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| PACKAGE error
    {
      javaperror ("PACKAGE: Missing name");
      return -1;
    }
| PACKAGE PackageName error
    {
      javaperror ("';' expected (1)");
      return -1;
    }
;
@}

  ImportDeclaration = SingleTypeImportDeclaration | TypeImportOnDemandDeclaration

@d Productions @{
ImportDeclaration:
  SingleTypeImportDeclaration
    {}
| TypeImportOnDemandDeclaration
    {}
;
@}

  SingleTypeImportDeclaration::
    name: PackageName
    CNTXT;

@d Productions @{

SingleTypeImportDeclaration:
  IMPORT PackageName SEMICOLON
    {
      $$ = new TYPE_CPP_SingleTypeImportDeclaration ();
      $$ -> set_name (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| IMPORT error
    {
      javaperror ("IMPORT: Missing name");
      return -1;
    }
| IMPORT PackageName error
    {
      javaperror ("';' expected (2)");
      return -1;
    }
;
@}

  TypeImportOnDemandDeclaration::
    name: PackageName;
    CNTXT;

@d Productions @{

TypeImportOnDemandDeclaration:
  IMPORT PackageName DOT MULT SEMICOLON
    {
      $$ = new TYPE_CPP_TypeImportOnDemandDeclaration ();
      $$ -> set_name (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@5);
    }
| IMPORT PackageName DOT error
    {
      javaperror ("'*' expected");
      return -1;
    }
| IMPORT PackageName DOT MULT error
    {
      javaperror ("';' expected (3)");
      return -1;
    }
;

type_declaration:
  class_declaration
    {}
| interface_declaration
    {}
| SEMICOLON
    {
      TYPE_CPP_EmptyStmt* es = new TYPE_CPP_EmptyStmt ();
      javapSetPos2 (*es, @@1, @@1);
      $$ = es;
    }
| error
    {
      javaperror ("Class or interface declaration expected");
      TYPE_CPP_EmptyStmt* es = new TYPE_CPP_EmptyStmt ();
      es -> set_cid (-1);
      $$ = es;
    }
;
@}

19.7 Modifiers

  Modifiers = seq of Modifier;

@d Productions @{

Modifiers:
  Modifier
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| Modifiers Modifier
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;
@}

  Modifier ::
    ModifierAccess
    CNTXT;

  ModifierAccess = <PUBLIC> | <PROTECTED> | <PRIVATE> | <ABSTRACT> | <FINAL>
                 | <NATIVE> | <SYNCHRONIZED> | <TRANSIENT> | <STATIC> | <VOLATILE>;

@d Productions @{

Modifier:
  PUBLIC
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"PUBLIC"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| PROTECTED
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"PROTECTED"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| PRIVATE
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"PRIVATE"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ABSTRACT
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"ABSTRACT"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| FINAL
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"FINAL"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| NATIVE
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"NATIVE"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| SYNCHRONIZED
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"SYNCHRONIZED"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| TRANSIENT
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"TRANSIENT"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| STATIC
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"STATIC"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| VOLATILE
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"VOLATILE"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| STRICTFP
    {
      $$ = new TYPE_CPP_Modifier ();
      $$ -> set_ma (Quote (L"STRICTFP"));
      javapSetPos2 (*$$, @@1, @@1);
    }
;
@}

19.8.1 Production from $8.1: Class Declaration

  TypeSpecifier ::
    tp :  SimpleTypeName | ClassSpecifier | InterfaceSpecifier;
    CNTXT

  ClassSpecifier ::
    ch : ClassHead
    ml : seq of MemberDeclaration;
    CNTXT;

  ClassHead ::
    m     : Modifiers
    ck    : <CLASS>
    name  : [Identifier]
    bs    : BaseSpec    -- Base Class in Java
    ispec : [InterSpec] -- Interface Classes in Java
    CNTXT;

  BaseSpec = seq of BaseSpecifier

  BaseSpecifier = PackageName

@d Productions @{

class_declaration:
  Modifiers CLASS Identifier wildcards baseclass interfaces class_body semicolon
    {
      TYPE_CPP_ClassHead head;
      head.set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      head.set_ck (Quote (L"CLASS"));
      head.set_name (*$3);
      delete $3;
      head.set_bs ((const SEQ<Record>) *$5);
      delete $5;
      head.set_ispec (*$6);
      delete $6;
      head.set_tpspec (*$4);
      delete $4;
      javapSetPos2 (head, @@1, @@6);
      TYPE_CPP_ClassSpecifier cs;
      cs.set_ch (head);
      cs.set_ml ((const SEQ<Record>) *$7);
      delete $7;
      javapSetPos2 (cs, @@1, @@7);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (cs);
      javapSetPos2 (*$$, @@1, @@7);
    }
| Modifiers CLASS Identifier wildcards baseclass class_body semicolon
    {
      TYPE_CPP_ClassHead head;
      head.set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      head.set_ck (Quote (L"CLASS"));
      head.set_name (*$3);
      delete $3;
      head.set_bs ((const SEQ<Record>) *$5);
      delete $5;
      head.set_ispec (Nil ());
      head.set_tpspec (*$4);
      delete $4;
      javapSetPos2 (head, @@1, @@5);
      TYPE_CPP_ClassSpecifier cs;
      cs.set_ch (head);
      cs.set_ml ((const SEQ<Record>) *$6);
      delete $6;
      javapSetPos2 (cs, @@1, @@6);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (cs);
      javapSetPos2 (*$$, @@1, @@6);
    }
| CLASS Identifier wildcards baseclass interfaces class_body semicolon
    {
      TYPE_CPP_ClassHead head;
      head.set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      head.set_ck (Quote (L"CLASS"));
      head.set_name (*$2);
      delete $2;
      head.set_bs ((const SEQ<Record>) *$4);
      delete $4;
      head.set_ispec (*$5);
      delete $5;
      head.set_tpspec (*$3);
      delete $3;
      javapSetPos2 (head, @@1, @@5);
      TYPE_CPP_ClassSpecifier cs;
      cs.set_ch (head);
      cs.set_ml ((const SEQ<Record>) *$6);
      delete $6;
      javapSetPos2 (cs, @@1, @@6);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (cs);
      javapSetPos2 (*$$, @@1, @@6);
    }
| CLASS Identifier wildcards baseclass class_body semicolon
    {
      TYPE_CPP_ClassHead head;
      head.set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      head.set_ck (Quote (L"CLASS"));
      head.set_name (*$2);
      delete $2;
      head.set_bs ((const SEQ<Record>) *$4);
      delete $4;
      head.set_ispec (Nil());
      head.set_tpspec (*$3);
      delete $3;
      javapSetPos2 (head, @@1, @@4);
      TYPE_CPP_ClassSpecifier cs;
      cs.set_ch (head);
      cs.set_ml ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (cs, @@1, @@5);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (cs);
      javapSetPos2 (*$$, @@1, @@5);
    }
| Modifiers CLASS error
    {
      javaperror ("Modifiers CLASS: Missing class name");
      return -1;
    }
| CLASS error
    {
      javaperror ("CLASS: Missing class name");
      return -1;
    }
| CLASS Identifier error
    {
      javaperror ("CLASS Identifier: '{' expected");
      return -1;
    }
| Modifiers CLASS Identifier error
    {
      javaperror ("Modifiers CLASS Identifier:'{' expected");
      return -1;
    }
;

semicolon:
    {}
| SEMICOLON
    {}
;

wildcards:
    {
      $$ = new Sequence ();
    }
    | LT wildcard_type_list GT
    {
      $$ = $2;
    };

wildcard_type_list:
  Identifier
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| Identifier LT wildcard_type_list GT
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
      delete $3;
    }
| REL_QM EXTENDS Identifier
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$3);
      delete $3;
    }
| REL_QM SUPER Identifier
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$3);
      delete $3;
    }
| REL_QM
    {
    }
| wildcard_type_list COMMA Identifier
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| wildcard_type_list COMMA Identifier LT wildcard_type_list GT
    {
      $1 -> ImpAppend (*$3);
      delete $3;
      delete $5;
    };

baseclass:
    {
      $$ = new Sequence ();
    }
| EXTENDS class_type wildcards
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$2);
      delete $2;
      delete $3;
    }
//| EXTENDS class_type LT class_type_list GT
//    {
//      $$ = new Sequence ();
//      $$ -> ImpAppend (*$2);
//      delete $2;
//      delete $4;
//    }
| EXTENDS class_type error
    {
      javaperror ("EXTENDS class_type: '{' expected");
      return -1;
    }
| EXTENDS error
    {
      javaperror ("EXTENDS: Missing base class name");
      return -1;
    }
;
@}

  InterSpec = seq of PackageName;

@d Productions @{

interfaces:
  IMPLEMENTS interface_type_list
    {
// CI ?
      $$ = $2;
    }
| IMPLEMENTS error
    {
      javaperror ("IMPLEMENTS: Missing interface name");
      return -1;
    }
;

interface_type_list:
  interface_type wildcards
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
      delete $2;
    }
| interface_type_list COMMA interface_type
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| interface_type_list COMMA error
    {
      javaperror ("Missing interface name");
      return -1;
    }
;
@}

@d Productions @{

class_body:
  OCB class_body_declarations CCB
    {
// CI ?
      $$ = $2;
    }

/* JR moved to ?
JR : added SEMICOLON
| OCB class_body_declarations CCB SEMICOLON
    {
// CI ?
      $$ = $2;
    }
*/

| OCB CCB
    {
// CI ?
      $$ = new Sequence ();
    }

/* JR moved to ?
JR : added SEMICOLON
| OCB CCB SEMICOLON
    {
// CI ?
      $$ = new Sequence ();
    }
*/

;

class_body_declarations:
  class_member_declaration
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| class_body_declarations class_member_declaration
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;
@}

MemberDeclaration = MemberSpecifier | IdentDeclaration | FunctionDefinition | StaticInitializer

@d Productions @{

class_member_declaration:
  field_declaration
    {}
/*
// JR already SEMICOLON here, look at rule 'field_declaration:'
| field_declaration SEMICOLON
    {}
*/
| FunctionDefinition
    {}
| class_declaration
    {
      SEQ<Record> s;
      s.ImpAppend (*$1);
      delete $1;
      TYPE_CPP_IdentDeclaration* id = new TYPE_CPP_IdentDeclaration ();
      id -> set_ds (s);
      id -> set_dl (Nil ());
      javapSetPos2 (*id, @@1, @@1);
      $$ = id;
    }
| interface_declaration
    {
      SEQ<Record> s;
      s.ImpAppend (*$1);
      delete $1;
      TYPE_CPP_IdentDeclaration* id = new TYPE_CPP_IdentDeclaration ();
      id -> set_ds (s);
      id -> set_dl (Nil ());
      javapSetPos2 (*id, @@1, @@1);
      $$ = id;
    }
| constructor_declaration
    {
//JR PreProcStmt ????
    }
| StaticInitializer
    {}
| CompoundStmt
    {}
;
@}

19.8.2 Productions from 8.3: Field Declarations

  IdentDeclaration ::
    ds : seq of DeclSpecifier
    dl : [DeclaratorList]
    cid :- CI`ContextId;

  DeclSpecifier = StorageClassSpecifier | TypeSpecifier | ArraySpecifier | Modifier

  DeclaratorList = seq of InitDeclarator;

  InitDeclarator ::
    decl : Declarator
    i    : [Initializer]
    cid :- CI`ContextId;

  Declarator = Name | FctDecl | ArrayDecl

@d Productions @{

field_declaration:
  primitive_type osb_csb_s variable_declarators SEMICOLON
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_IdentDeclaration ();
      $$ -> set_ds (s);
      if ($2 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$2, *$3));
        delete $2;
        $$ -> set_dl (v_d);
      }
      else
        $$ -> set_dl ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
| PackageName osb_csb_s variable_declarators SEMICOLON
// FAUP
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_IdentDeclaration ();
      $$ -> set_ds (s);
      if ($2 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$2, *$3));
        delete $2;
        $$ -> set_dl (v_d);
      }
      else
        $$ -> set_dl ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
| Modifiers primitive_type osb_csb_s variable_declarators SEMICOLON
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      $1 -> ImpAppend (ts);
      $$ = new TYPE_CPP_IdentDeclaration ();
      $$ -> set_ds ((const SEQ<Record>) *$1);
      delete $1;
      if ($3 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$3, *$4));
        delete $3;
        $$ -> set_dl (v_d);
      }
      else
        $$ -> set_dl ((const SEQ<Record>) *$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@5);
    }
| Modifiers PackageName osb_csb_s variable_declarators SEMICOLON
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      $1 -> ImpAppend (ts);
      $$ = new TYPE_CPP_IdentDeclaration ();
      $$ -> set_ds ((const SEQ<Record>) *$1);
      delete $1;
      if ($3 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$3, *$4));
        delete $3;
        $$ -> set_dl (v_d);
      }
      else
        $$ -> set_dl ((const SEQ<Record>) *$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@5);
    }
;

variable_declarators:
  variable_declarator
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| variable_declarators COMMA variable_declarator
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| variable_declarators COMMA error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  InitDeclarator ::
    decl : Declarator
    i    : [Initializer]
    cid :- CI`ContextId;

  Declarator = Name | FctDecl | ArrayDecl

  Name = Identifier | Destructor | QualifiedName | PackageName

  Initializer = AsgnInit

  AsgnInit ::
    expr : Expr
    cid :- CI`ContextId;

  ObjectInit ::
   exprs : Exprs
   cid :- CI`ContextId;

@d Productions @{

variable_declarator:
  variable_declarator_id
    {
      $$ = new TYPE_CPP_InitDeclarator ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_i (Nil ());
      javapSetPos2 (*$$, @@1, @@1);
    }
| variable_declarator_id ASEQUAL variable_initializer
    {
      TYPE_CPP_AsgnInit ai;
      ai.set_expr (*$3);
      delete $3;
      javapSetPos2 (ai, @@3, @@3);
      $$ = new TYPE_CPP_InitDeclarator ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_i (ai);
      javapSetPos2 (*$$, @@1, @@3);
    }
| variable_declarator_id ASEQUAL error
    {
      javaperror ("Missing variable initializer");
      return -1;
    }
| variable_declarator_id ASEQUAL variable_initializer error
    {
      javaperror ("';' expected (4)");
      return -1;
    }
;

variable_declarator_id:
  ArrayDecl
    {}
| Identifier
    {}
| Identifier error
    {
      javaperror ("Invalid declaration");
      return -1;
    }
;
@}

  ArrayDecl ::
    decl : Declarator
    expr : [Expr]
    CNTXT;

  Declarator = Name | FctDecl | ArrayDecl
here ArrayDecl or Name

  Name = Identifier | Destructor | QualifiedName | PackageName

@d Productions @{

ArrayDecl:
  Identifier OSB CSB
    {
      $$ = new TYPE_CPP_ArrayDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| ArrayDecl OSB CSB
    {
      $$ = new TYPE_CPP_ArrayDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| ArrayDecl OSB error
    {
      javaperror ("']' expected");
      return -1;
    }
| ArrayDecl CSB error
    {
      javaperror ("Unbalanced ']'");
      return -1;
    }
;

variable_initializer:
  expression
    {}
| ArrayInitializer
    {}
;
@}

19.8.3 Productions from 8.4: Method Declarations

  FunctionDefinition ::
    m: Modifiers
    ds  : seq of DeclSpecifier
    decl: Declarator              (= FctDecl )
    ci  : nil
    throw : [seq of SimpleTypeName]
    fb  : [FctBody]
    CNTXT

  DeclSpecifier = StorageClassSpecifier | TypeSpecifier | ArraySpecifier | Modifier

  Declarator = Name | FctDecl | ArrayDecl

@d Productions @{

FunctionDefinition:
  method_header FctBody
    {
      if ($2 == NULL) {
        $1 -> set_fb (Nil ());
        javapSetPos2 (*$1, @@1, @@1);
      }
      else {
        $1 -> set_fb (*$2);
        javapSetPos2 (*$1, @@1, @@2);
      }
    }
| method_header OCB CCB SEMICOLON
    {
      $1 -> set_fb (Nil ());
      javapSetPos2 (*$1, @@1, @@4);
    }
| method_header OCB CCB
    {
      $1 -> set_fb (Nil ());
      javapSetPos2 (*$1, @@1, @@3);
    }
| method_header SEMICOLON
    {
// JR also ruled as: abstract_method_declaration
      $1 -> set_fb (Nil ());
      javapSetPos2 (*$1, @@1, @@2);
    }
| method_header error
    {
      javaperror ("'{' expected");
      return -1;
    }
;

method_header:
  primitive_type osb_csb_s method_declarator throws
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_FunctionDefinition ();
      $$ -> set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      if ($2 != NULL)
      {
        move_SBs_2_ArraySpecifiers (*$2, s);
        delete $2;
      }
      $$ -> set_ds (s);
      $$ -> set_decl (*$3);
      delete $3;
      $$ -> set_ci (Nil ());
      $$ -> set_throw (*$4);
      delete $4;
//      javapSetPos2 (*$$, @@1, @@4);
    }
| PackageName osb_csb_s method_declarator throws
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_FunctionDefinition ();
      $$ -> set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      if ($2 != NULL)
      {
        move_SBs_2_ArraySpecifiers (*$2, s);
        delete $2;
      }
      $$ -> set_ds (s);
      $$ -> set_decl (*$3);
      delete $3;
      $$ -> set_ci (Nil ());
      $$ -> set_throw (*$4);
      delete $4;
//      javapSetPos2 (*$$, @@1, @@4);
    }
| Modifiers primitive_type osb_csb_s method_declarator throws
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_FunctionDefinition ();
      $$ -> set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      if ($3 != NULL)
      {
        move_SBs_2_ArraySpecifiers (*$3, s);
        delete $3;
      }
      $$ -> set_ds (s);
      $$ -> set_decl (*$4);
      delete $4;
      $$ -> set_ci (Nil ());
      $$ -> set_throw (*$5);
      delete $5;
//      javapSetPos2 (*$$, @@1, @@5);
    }
| Modifiers PackageName osb_csb_s method_declarator throws
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_FunctionDefinition ();
      $$ -> set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      if ($3 != NULL)
      {
        move_SBs_2_ArraySpecifiers (*$3, s);
        delete $3;
      }
      $$ -> set_ds (s);
      $$ -> set_decl (*$4);
      delete $4;
      $$ -> set_ci (Nil ());
      $$ -> set_throw (*$5);
      delete $5;
//      javapSetPos2 (*$$, @@1, @@5);
    }
| primitive_type error
    {
      javaperror ("method_declarator ERROR primitive type ? ");
      return -1;
    }
| PackageName error
    {
      javaperror ("method declarator ERROR ? identifier");
      return -1;
    }
| Modifiers primitive_type error
    {
      javaperror ("method_declarator ERROR ? primitive type");
      return -1;
    }
| Modifiers PackageName error
    {
      javaperror ("method_declarator ERROR ? identifier");
      return -1;
    }
| Modifiers error
    {
      javaperror ("Invalid method declaration, return type required");
      return -1;
    }
;
@}

  FctDecl ::
    decl : Declarator
    arg : [ArgumentDeclarationList]
    cvlist : nil
    CNTXT

  Declarator = Name | FctDecl | ArrayDecl
only Name

@d Productions @{

method_declarator:
  Identifier OP CP
    {
      TYPE_CPP_FctDecl* fd = new TYPE_CPP_FctDecl ();
      fd -> set_decl (*$1);
      delete $1;
      fd -> set_arg (Nil ());
      fd -> set_cvlist (Nil ());
      javapSetPos2 (*fd, @@1, @@3);
      $$ = fd;
    }
| Identifier OP ArgumentDeclarationList CP
    {
      TYPE_CPP_ArgumentDeclarationList adl;
      adl.set_ellipsis (Nil ());
      adl.set_arg (*$3);
      delete $3;
      javapSetPos2 (adl, @@2, @@4);
      TYPE_CPP_FctDecl* fd = new TYPE_CPP_FctDecl ();
      fd -> set_decl (*$1);
      delete $1;
      fd -> set_arg (adl);
      fd -> set_cvlist (Nil ());
      javapSetPos2 (*fd, @@1, @@4);
      $$ = fd;
    }
| method_declarator OSB CSB
    {
// ( JR ) Discouraged form of returned type specification
      TYPE_CPP_ArrayDecl* ad = new TYPE_CPP_ArrayDecl ();
      ad -> set_decl (*$1);
      delete $1;
      ad -> set_expr (Nil ());
      javapSetPos2 (*ad, @@1, @@3);
      $$ = (TYPE_CPP_Declarator*) ad;
    }
| Identifier OP error
    {
      javaperror ("')' expected");
      return -1;
    }
| method_declarator OSB error
    {
      javaperror ("']' expected");
      return -1;
    }
;
@}

JR : added 'ArgumentDeclarationList' to hold Sequence(..)

  ArgumentDeclarationList ::
    arg      : [seq of ArgumentDeclaration]
    ellipsis : [<ELLIPSIS>]
    CNTXT;

@d Productions @{

ArgumentDeclarationList:
  ArgumentDeclaration
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| ArgumentDeclarationList COMMA ArgumentDeclaration
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| ArgumentDeclarationList COMMA error
    {
      javaperror ("Missing formal parameter term");
      return -1;
    }
;
@}

  ArgumentDeclaration = ArgDecl | DefaultArgDecl | ArgAbsDecl | DefaultArgAbsDecl;

  ArgDecl ::
    ds  : seq of DeclSpecifier
    decl: Declarator
    CNTXT;

  DeclSpecifier = StorageClassSpecifier | TypeSpecifier | ArraySpecifier | Modifier

  TypeSpecifier ::
    tp :  SimpleTypeName | ClassSpecifier | InterfaceSpecifier;
    CNTXT

  Declarator = Name | FctDecl | ArrayDecl
only ArrayDecl

@d Productions @{

ArgumentDeclaration:
  primitive_type osb_csb_s variable_declarator_id
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_ArgDecl ();
      $$ -> set_ds (s);
      if ($2 != NULL)
      {
        TYPE_CPP_ArrayDecl ad (move_SBs_2_ArrayDecl (*$2, *$3));
        delete $2;
        $$ -> set_decl (ad);
      }
      else
        $$ -> set_decl (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| PackageName osb_csb_s variable_declarator_id
    {
// FAUP
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_ArgDecl ();
      $$ -> set_ds (s);
      if ($2 != NULL)
      {
        TYPE_CPP_ArrayDecl ad (move_SBs_2_ArrayDecl (*$2, *$3));
        delete $2;
        $$ -> set_decl (ad);
      }
      else
        $$ -> set_decl (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| primitive_type ELLIPSIS variable_declarator_id
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      TYPE_CPP_ArraySpecifier as;
      javapSetPos2 (as, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (ts);
      s.ImpAppend (as);
      $$ = new TYPE_CPP_ArgDecl ();
      $$ -> set_ds (s);
      $$ -> set_decl (*$3);
      javapSetPos2 (*$$, @@1, @@3);
      delete $3;
    }
| PackageName ELLIPSIS variable_declarator_id
    {
// FAUP
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      TYPE_CPP_ArraySpecifier as;
      javapSetPos2 (as, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (ts);
      s.ImpAppend (as);
      $$ = new TYPE_CPP_ArgDecl ();
      $$ -> set_ds (s);
      $$ -> set_decl (*$3);
      javapSetPos2 (*$$, @@1, @@3);
      delete $3;
    }
| FINAL primitive_type osb_csb_s variable_declarator_id
    {
      TYPE_CPP_Modifier m;
      m.set_ma (Quote (L"FINAL"));
      javapSetPos2 (m, @@1, @@1);
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (m);
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_ArgDecl ();
      $$ -> set_ds (s);
      if ($3 != NULL)
      {
        TYPE_CPP_ArrayDecl ad (move_SBs_2_ArrayDecl (*$3, *$4));
        delete $3;
        $$ -> set_decl (ad);
      }
      else
        $$ -> set_decl (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| FINAL PackageName osb_csb_s variable_declarator_id
    {
// FAUP
      TYPE_CPP_Modifier m;
      m.set_ma (Quote (L"FINAL"));
      javapSetPos2 (m, @@1, @@1);
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (m);
      s.ImpAppend (ts);
      $$ = new TYPE_CPP_ArgDecl ();
      $$ -> set_ds (s);
      if ($3 != NULL)
      {
        TYPE_CPP_ArrayDecl ad (move_SBs_2_ArrayDecl (*$3, *$4));
        delete $3;
        $$ -> set_decl (ad);
      }
      else
        $$ -> set_decl (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| primitive_type error
    {
      javaperror ("Missing primitive type");
      return -1;
    }
| PackageName error
    {
      javaperror ("Missing Identifier");
      return -1;
    }
| Modifiers primitive_type error
    {
      javaperror ("Missing primitive type");
      return -1;
    }
| Modifiers PackageName error
    {
      javaperror ("Missing Identifier");
      return -1;
    }
;

throws:
    {
      $$ = new Sequence ();
    }
| THROWS class_type_list
    {
      $$ = $2;
    }
| THROWS error
    {
      javaperror ("Missing class type term");
      return -1;
    }
;

class_type_list:
  class_type
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| class_type_list COMMA class_type
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| class_type_list COMMA error
    {
      javaperror ("class_type_list COMMA: Missing class type term");
      return -1;
    }
;
@}

  FctBody ::
    stmt : CompoundStmt
    CNTXT;

@d Productions @{

FctBody:
  OCB block_statements CCB
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB block_statements CCB SEMICOLON
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@4);
    }
;
@}

19.8.4 Productions from 8.5: Static Initializers

  StaticInitializer ::
    stmt : CompoundStmt
    CNTXT;

@d Productions @{

StaticInitializer:
  STATIC CompoundStmt
    {
      $$ = new TYPE_CPP_StaticInitializer ();
      $$ -> set_stmt (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
;
@}

19.8.5 Productions from 8.6: Constructor Declarations

@d Productions @{

constructor_declaration:
  constructor_header constructor_body
    {
      $1 -> set_fb (*$2);
    }
;

constructor_header:
  constructor_declarator throws
    {
      SEQ<Record> s;
      $$ = new TYPE_CPP_FunctionDefinition ();
      $$ -> set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      $$ -> set_ds (s);
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_ci (Nil());
      $$ -> set_throw (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
| Modifiers constructor_declarator throws
    {
      SEQ<Record> s;
      $$ = new TYPE_CPP_FunctionDefinition ();
      $$ -> set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      $$ -> set_ds (s);
      $$ -> set_decl (*$2);
      delete $2;
      $$ -> set_ci (Nil ());
      $$ -> set_throw (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
;

constructor_declarator:
  SimplePackageName OP CP
    {
      $$ = new TYPE_CPP_FctDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_arg (Nil ());
      $$ -> set_cvlist (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| SimplePackageName OP ArgumentDeclarationList CP
    {
      TYPE_CPP_ArgumentDeclarationList adl;
      adl.set_ellipsis (Nil ());
      adl.set_arg (*$3);
      delete $3;
      javapSetPos2 (adl, @@3, @@3);
      $$ = new TYPE_CPP_FctDecl ();
      $$ -> set_decl (*$1);
      delete $1;
      $$ -> set_arg (adl);
      $$ -> set_cvlist (Nil ());
      javapSetPos2 (*$$, @@1, @@4);
    }
;

constructor_body:
  OCB CCB
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) Sequence());
      cs.set_cid (-1);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@2);
    }
| OCB CCB SEMICOLON
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) Sequence());
      cs.set_cid (-1);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB explicit_constructor_invocation CCB
    {
      SEQ<Record> s;
      s.ImpAppend (*$2);
      delete $2;
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms (s);
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB explicit_constructor_invocation CCB SEMICOLON
    {
      SEQ<Record> s;
      s.ImpAppend (*$2);
      delete $2;
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms (s);
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@4);
    }
| OCB block_statements CCB
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB block_statements CCB SEMICOLON
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@4);
    }
| OCB explicit_constructor_invocation block_statements CCB
    {
      $3 -> ImpPrepend (*$2);
      delete $2;
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (cs, @@2, @@3);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@4);
    }
| OCB explicit_constructor_invocation block_statements CCB SEMICOLON
    {
      $3 -> ImpPrepend (*$2);
      delete $2;
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (cs, @@2, @@3);
      $$ = new TYPE_CPP_FctBody ();
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@5);
    }
;

explicit_constructor_invocation:
  THIS OP CP SEMICOLON
    {
// JR ??? this
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"this"));
      javapSetPos2 (i, @@1, @@1);
      TYPE_CPP_SimplePackageName spn;
      spn.set_id (i);
      javapSetPos2 (spn, @@1, @@1);
      TYPE_CPP_FctCall fc;
      fc.set_fct (spn);
      fc.set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (fc, @@1, @@4);
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (fc);
      javapSetPos2 (*$$, @@1, @@4);
    }
| SUPER OP CP SEMICOLON
    {
// JR ??? super
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@1, @@1);
      TYPE_CPP_SimplePackageName spn;
      spn.set_id (i);
      javapSetPos2 (spn, @@1, @@1);
      TYPE_CPP_FctCall fc;
      fc.set_fct (spn);
      fc.set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (fc, @@1, @@4);
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (fc);
      javapSetPos2 (*$$, @@1, @@4);
    }
| THIS OP argument_list CP SEMICOLON
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"this"));
      javapSetPos2 (i, @@1, @@1);
      TYPE_CPP_SimplePackageName spn;
      spn.set_id (i);
      javapSetPos2 (spn, @@1, @@1);
      TYPE_CPP_FctCall fc;
      fc.set_fct (spn);
      fc.set_arg ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (fc, @@1, @@5);
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (fc);
      javapSetPos2 (*$$, @@1, @@5);
    }
| SUPER OP argument_list CP SEMICOLON
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@1, @@1);
      TYPE_CPP_SimplePackageName spn;
      spn.set_id (i);
      javapSetPos2 (spn, @@1, @@1);
      TYPE_CPP_FctCall fc;
      fc.set_fct (spn);
      fc.set_arg ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (fc, @@1, @@5);
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (fc);
      javapSetPos2 (*$$, @@1, @@5);
    }
//| PackageName DOT THIS OP CP SEMICOLON
// JR error, not part of Java Language Specification
//    {
//      TYPE_CPP_Identifier i;
//      i.set_id ((const SEQ<Char>) Sequence (L"this"));
//      javapSetPos2 (i, @@3, @@3);
//      TYPE_CPP_QualifiedPackageName qpn;
//      qpn.set_id (i);
//      qpn.set_pn (*$1);
//      delete $1;
//      javapSetPos2 (qpn, @@1, @@3);
//      TYPE_CPP_FctCall fc;
//      fc.set_fct (qpn);
//      fc.set_arg ((const SEQ<Record>) Sequence ());
//      javapSetPos2 (fc, @@1, @@6);
//      $$ = new TYPE_CPP_ExpressionStmt ();
//      $$ -> set_expr (fc);
//      javapSetPos2 (*$$, @@1, @@6);
//    }
//| PackageName DOT SUPER OP CP SEMICOLON
// JR error, is: | primary DOT SUPER OP CP SEMICOLON
//    {
//      TYPE_CPP_Identifier i;
//      i -> set_id ((const SEQ<Char>) Sequence (L"super"));
//      javapSetPos2 (i, @@3, @@3);
//      TYPE_CPP_QualifiedPackageName qpn;
//      qpn.set_id (i);
//      qpn.set_pn (*$1);
//      delete $1;
//      javapSetPos2 (qpn, @@1, @@3);
//      TYPE_CPP_FctCall fc;
//      fc.set_fct (qpn);
//      fc.set_arg ((const SEQ<Record>) Sequence ());
//      javapSetPos2 (fc, @@1, @@6);
//      $$ = new TYPE_CPP_ExpressionStmt ();
//      $$ -> set_expr (fc);
//      javapSetPos2 (*$$, @@1, @@6);
//    }
| primary DOT SUPER OP CP SEMICOLON
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@3, @@3);
      TYPE_CPP_QualifiedName qn;
      qn.set_qcn (*$1);
      delete $1;
      qn.set_id (i);
      javapSetPos2 (qn, @@1, @@3);
      TYPE_CPP_FctCall fc;
      fc.set_fct (qn);
      fc.set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (fc, @@1, @@6);
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (fc);
      javapSetPos2 (*$$, @@1, @@6);
    }
//| PackageName DOT THIS OP argument_list CP SEMICOLON
// JR error, not part of Java Language Specification
//    {
//      TYPE_CPP_Identifier i;
//      i.set_id ((const SEQ<Char>) Sequence (L"this"));
//      javapSetPos2 (i, @@3, @@3);
//      TYPE_CPP_QualifiedPackageName qpn;
//      qpn.set_id (i);
//      qpn.set_pn (*$1);
//      delete $1;
//      javapSetPos2 (qpn, @@1, @@3);
//      TYPE_CPP_FctCall fc;
//      fc.set_fct (qpn);
//      fc.set_arg ((const SEQ<Record>) *$5);
//      delete $5;
//      javapSetPos2 (fc, @@1, @@7);
//      $$ = new TYPE_CPP_ExpressionStmt ();
//      $$ -> set_expr (fc);
//      javapSetPos2 (*$$, @@1, @@7);
//    }
//| PackageName DOT SUPER OP argument_list CP SEMICOLON
// JR error, is : | primary DOT SUPER OP argument_list CP SEMICOLON
//    {
//      TYPE_CPP_Identifier i;
//      i.set_id ((const SEQ<Char>) Sequence (L"super"));
//      javapSetPos2 (i, @@3, @@3);
//      TYPE_CPP_QualifiedPackageName qpn;
//      qpn.set_id (i);
//      qpn.set_pn (*$1);
//      delete $1;
//      javapSetPos2 (qpn, @@1, @@3);
//      TYPE_CPP_FctCall fc;
//      fc.set_fct (qpn);
//      fc.set_arg ((const SEQ<Record>) *$5);
//      delete $5;
//      javapSetPos2 (fc, @@1, @@7);
//      $$ = new TYPE_CPP_ExpressionStmt ();
//      $$ -> set_expr (fc);
//      javapSetPos2 (*$$, @@1, @@7);
//    }
| primary DOT SUPER OP argument_list CP SEMICOLON
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@3, @@3);
      TYPE_CPP_QualifiedName qn;
      qn.set_qcn (*$1);
      delete $1;
      qn.set_id (i);
      javapSetPos2 (qn, @@1, @@3);
      TYPE_CPP_FctCall fc;
      fc.set_fct (qn);
      fc.set_arg ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (fc, @@1, @@7);
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (fc);
      javapSetPos2 (*$$, @@1, @@7);
    }
;
@}

19.9   Productions from 9: Interfaces
19.9.1 Productions from 9.1: Interfaces Declarations

  InterfaceSpecifier::
    ih : InterfaceHead
    ml : InterfaceBody
    cid :- CI`ContextId;

  InterfaceHead ::
    m    : Modifiers
    name : Identifier
    bs   : InterSpec
    cid :- CI`ContextId;

  InterfaceBody = seq of MemberDeclaration;

@d Productions @{

interface_declaration:
  Modifiers INTERFACE Identifier extends_interfaces interface_body
    {
      TYPE_CPP_InterfaceHead ih;
      ih.set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      ih.set_name (*$3);
      delete $3;
      ih.set_bs ((const SEQ<TYPE_CPP_PackageName>) *$4);
      delete $4;
      ih.set_tpspec(Nil());
      javapSetPos2 (ih, @@1, @@4);
      TYPE_CPP_InterfaceSpecifier is;
      is.set_ih (ih);
      is.set_ml ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (is, @@1, @@5);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (is);
      javapSetPos2 (*$$, @@1, @@5);
    }
| Modifiers INTERFACE Identifier interface_body
    {
      TYPE_CPP_InterfaceHead ih;
      ih.set_m ((const SEQ<TYPE_CPP_Modifier>) *$1);
      delete $1;
      ih.set_name (*$3);
      delete $3;
      ih.set_bs ((const SEQ<TYPE_CPP_PackageName>) Sequence ());
      ih.set_tpspec(Nil());
      javapSetPos2 (ih, @@1, @@3);
      TYPE_CPP_InterfaceSpecifier is;
      is.set_ih (ih);
      is.set_ml ((const SEQ<Record>) *$4);
      delete $4;
      javapSetPos2 (is, @@1, @@4);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (is);
      javapSetPos2 (*$$, @@1, @@4);
    }
| INTERFACE Identifier extends_interfaces interface_body
    {
      TYPE_CPP_InterfaceHead ih;
      ih.set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      ih.set_name (*$2);
      delete $2;
      ih.set_bs ((const SEQ<TYPE_CPP_PackageName>) *$3);
      delete $3;
      ih.set_tpspec(Nil());
      javapSetPos2 (ih, @@1, @@3);
      TYPE_CPP_InterfaceSpecifier is;
      is.set_ih (ih);
      is.set_ml ((const SEQ<Record>) *$4);
      delete $4;
      javapSetPos2 (is, @@1, @@4);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (is);
      javapSetPos2 (*$$, @@1, @@4);
    }
| INTERFACE Identifier interface_body
    {
      TYPE_CPP_InterfaceHead ih;
      ih.set_m ((const SEQ<TYPE_CPP_Modifier>) Sequence ());
      ih.set_name (*$2);
      delete $2;
      ih.set_bs ((const SEQ<TYPE_CPP_PackageName>) Sequence ());
      ih.set_tpspec(Nil());
      javapSetPos2 (ih, @@1, @@2);
      TYPE_CPP_InterfaceSpecifier is;
      is.set_ih (ih);
      is.set_ml ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (is, @@1, @@3);
      $$ = new TYPE_CPP_TypeSpecifier ();
      $$ -> set_tp (is);
      javapSetPos2 (*$$, @@1, @@3);
    }
| INTERFACE Identifier error
    {
      javaperror ("'{' expected");
      return -1;
    }
| Modifiers INTERFACE Identifier error
    {
      javaperror ("'{' expected");
      return -1;
    }
;

extends_interfaces:
  EXTENDS interface_type
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$2);
      delete $2;
    }
| extends_interfaces COMMA interface_type
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| EXTENDS error
    {
      javaperror ("Invalid interface type");
      return -1;
    }
| extends_interfaces COMMA error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

JR: added SEMICOLON

@d Productions @{

interface_body:
  OCB CCB
    {
      $$ = new Sequence ();
    }
| OCB CCB SEMICOLON
    {
      $$ = new Sequence ();
    }
| OCB interface_member_declarations CCB
    {
      $$ = $2;
    }
| OCB interface_member_declarations CCB SEMICOLON
    {
      $$ = $2;
    }
;

interface_member_declarations:
  interface_member_declaration
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| interface_member_declarations interface_member_declaration
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;

interface_member_declaration:
  constant_declaration
    {}
| abstract_method_declaration
    {}
| class_declaration
    {
      SEQ<Record> s;
      s.ImpAppend (*$1);
      delete $1;
      TYPE_CPP_IdentDeclaration* id = new TYPE_CPP_IdentDeclaration ();
      id -> set_ds (s);
      id -> set_dl (Nil ());
      javapSetPos2 (*id, @@1, @@1);
      $$ = id;
    }
| interface_declaration
    {
      SEQ<Record> s;
      s.ImpAppend (*$1);
      delete $1;
      TYPE_CPP_IdentDeclaration* id = new TYPE_CPP_IdentDeclaration ();
      id -> set_ds (s);
      id -> set_dl (Nil ());
      javapSetPos2 (*id, @@1, @@1);
      $$ = id;
    }
;

constant_declaration:
  field_declaration
    {}
;

abstract_method_declaration:
  method_header SEMICOLON
    {
      $1 -> set_fb (Nil ());
      javapSetPos2 (*$1, @@1, @@2);
    }
| method_header error
    {
      javaperror ("';' expected (5)");
      return -1;
    }
;
@}

19.10 Productions from 10: Arrays

  ArrayInitializer ::
    vi  : [seq of (Expr | ArrayInitializer)]
    cid :- CI`ContextId;

@d Productions @{

ArrayInitializer:
  OCB CCB
    {
      $$ = new TYPE_CPP_ArrayInitializer ();
      $$ -> set_vi (Nil ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| OCB COMMA CCB
    {
// JR comma ?
      $$ = new TYPE_CPP_ArrayInitializer ();
      $$ -> set_vi (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB variable_initializers CCB
    {
      $$ = new TYPE_CPP_ArrayInitializer ();
      $$ -> set_vi ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB variable_initializers COMMA CCB
    {
// JR comma ?
      $$ = new TYPE_CPP_ArrayInitializer ();
      $$ -> set_vi ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@4);
    }
;

variable_initializers:
  variable_initializer
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| variable_initializers COMMA variable_initializer
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| variable_initializers COMMA error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

19.11 Production from 14: Blocks and Statements

  CompoundStmt ::
    stms : seq of Stmt
    cid :- CI`ContextId;

@d Productions @{

CompoundStmt:
  OCB CCB
    {
      $$ = new TYPE_CPP_CompoundStmt ();
      $$ -> set_stms ((const SEQ<Record>) Sequence ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| OCB block_statements CCB
    {
      $$ = new TYPE_CPP_CompoundStmt ();
      $$ -> set_stms ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
;

block_statements:
  block_statement
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| block_statements block_statement
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;

block_statement:
  local_variable_declaration_statement
    {}
| statement
    {}
| class_declaration
    {
      SEQ<Record> s;
      s.ImpAppend (*$1);
      delete $1;
      TYPE_CPP_IdentDeclaration id;
      id.set_ds (s);
      id.set_dl (Nil ());
      javapSetPos2 (id, @@1, @@1);
      TYPE_CPP_DeclarationStmt* ds = new TYPE_CPP_DeclarationStmt ();
      ds -> set_decl (id);
      javapSetPos2 (*ds, @@1, @@1);
      $$ = ds;
    }
/* Part of 'java12.cup' - NOT of 'javac'
| interface_declaration
*/
;

local_variable_declaration_statement:
  local_variable_declaration SEMICOLON       /* Can't catch missing ';' here */
    {}
;
@}

  DeclarationStmt ::
    decl : Declaration
    CNTXT;

  Declaration = IdentDeclaration | FunctionDefinition;
only IdentDeclaration here

  IdentDeclaration ::
    ds : seq of DeclSpecifier
    dl : [DeclaratorList]
    CNTXT;

 DeclSpecifier = StorageClassSpecifier | TypeSpecifier | ArraySpecifier | Modifier

  TypeSpecifier ::
    tp :  SimpleTypeName | ClassSpecifier | InterfaceSpecifier;
    CNTXT

  DeclaratorList = seq of InitDeclarator;

@d Productions @{

local_variable_declaration:
  primitive_type osb_csb_s variable_declarators
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      TYPE_CPP_IdentDeclaration id;
      id.set_ds (s);
      if ($2 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$2, *$3));
        delete $2;
        id.set_dl (v_d);
      }
      else
        id.set_dl ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (id, @@3, @@3);
      $$ = new TYPE_CPP_DeclarationStmt ();
      $$ -> set_decl (id);
      javapSetPos2 (*$$, @@1, @@3);
    }
| PackageName osb_csb_s variable_declarators
    {
// FAUP
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$1);
      delete $1;
      javapSetPos2 (ts, @@1, @@1);
      SEQ<Record> s;
      s.ImpAppend (ts);
      TYPE_CPP_IdentDeclaration id;
      id.set_ds (s);
      if ($2 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$2, *$3));
        delete $2;
        id.set_dl (v_d);
        javapSetPos2 (id, @@2, @@3);
      }
      else {
        id.set_dl ((const SEQ<Record>) *$3);
        javapSetPos2 (id, @@3, @@3);
      }
      delete $3;
      $$ = new TYPE_CPP_DeclarationStmt ();
      $$ -> set_decl (id);
      javapSetPos2 (*$$, @@1, @@3);
    }
| FINAL primitive_type osb_csb_s variable_declarators
    {
      TYPE_CPP_Modifier m;
      m.set_ma (Quote (L"FINAL"));
      javapSetPos2 (m, @@1, @@1);
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (m);
      s.ImpAppend (ts);
      TYPE_CPP_IdentDeclaration id;
      id.set_ds (s);
      if ($3 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$3, *$4));
        delete $3;
        id.set_dl (v_d);
        javapSetPos2 (id, @@3, @@4);
      }
      else {
        id.set_dl ((const SEQ<Record>) *$4);
        javapSetPos2 (id, @@4, @@4);
      }
      delete $4;
      $$ = new TYPE_CPP_DeclarationStmt ();
      $$ -> set_decl (id);
      javapSetPos2 (*$$, @@1, @@4);
    }
| FINAL PackageName osb_csb_s variable_declarators
    {
      TYPE_CPP_Modifier m;
      m.set_ma (Quote (L"FINAL"));
      javapSetPos2 (m, @@1, @@1);
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<Record> s;
      s.ImpAppend (m);
      s.ImpAppend (ts);
      TYPE_CPP_IdentDeclaration id;
      id.set_ds (s);
      if ($3 != NULL)
      {
        SEQ<TYPE_CPP_InitDeclarator> v_d (move_SBs_2_InitDeclarator (*$3, *$4));
        delete $3;
        id.set_dl (v_d);
        javapSetPos2 (id, @@3, @@4);
      }
      else {
        id.set_dl ((const SEQ<Record>) *$4);
        javapSetPos2 (id, @@4, @@4);
      }
      delete $4;
      $$ = new TYPE_CPP_DeclarationStmt ();
      $$ -> set_decl (id);
      javapSetPos2 (*$$, @@1, @@4);
    }
;

statement:
  statement_without_trailing_substatement
    {}
| LabelStmt
    {}
| if_then_statement
    {}
| if_then_else_statement
    {}
| while_statement
    {}
| for_statement
    {}
| foreach_statement
    {}
;

statement_nsi:
  statement_without_trailing_substatement
    {}
| labeled_statement_nsi
    {}
| if_then_else_statement_nsi
    {}
| while_statement_nsi
    {}
| for_statement_nsi
    {}
;

statement_without_trailing_substatement:
  CompoundStmt
    {}
| EmptyStmt
    {}
| ExpressionStmt
    {}
| SwitchStmt
    {}
| DoWhileStmt
    {}
| Break
    {}
| Continue
    {}
| Return
    {}
| SynchronizedStmt
    {}
| ThrowExpression
    {
      TYPE_CPP_ExpressionStmt* te = new TYPE_CPP_ExpressionStmt ();
      te -> set_expr (*$1);
      javapSetPos2 (*te, @@1, @@1);
      $$ = te;
      delete $1;
    }
| TryBlock
    {}
;

EmptyStmt:
  SEMICOLON
    {
      $$ = new TYPE_CPP_EmptyStmt ();
      javapSetPos2 (*$$, @@1, @@1);
    }
;
@}

  LabelStmt ::
    id   : Identifier
    stmt : Stmt
    cid :- CI`ContextId;

@d Productions @{

LabelStmt:
  label_decl statement
    {
      $$ = new TYPE_CPP_LabelStmt ();
      $$ -> set_id (*$1);
      delete $1;
      $$ -> set_stmt (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
| Identifier error
    {
      javaperror ("':' expected");
      return -1;
    }
;

label_decl:
  Identifier COLON
    {}
;

labeled_statement_nsi:
  label_decl statement_nsi
    {
      $$ = new TYPE_CPP_LabelStmt ();
      $$ -> set_id (*$1);
      delete $1;
      $$ -> set_stmt (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
;
@}

We concentrate here a bunch of error handling rules that we couldn't write
earlier, because ExpressionStmt catches a missing ';'.

 ExpressionStmt ::
     expr : [Expr]
     cid :- CI`ContextId;

@d Productions @{

ExpressionStmt:
  statement_expression SEMICOLON
    {
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (*$1);
      delete $1;
      javapSetPos2 (*$$, @@1, @@2);
    }
/* JR : moved to 'statement_expression'
| error SEMICOLON
    {
      javaperror ("Invalid expression statement (1)");
      return -1;
    }
| error OCB
    {
      javaperror ("Invalid expression statement (2)");
      return -1;
    }
| error CCB
    {
      javaperror ("Invalid expression statement (3)");
      return -1;
    }
*/
| THIS OP error
    {
      javaperror ("')' expected");
      return -1;
    }
| SUPER OP error
    {
      javaperror ("')' expected");
      return -1;
    }
| THIS OP CP error
    {
      javaperror ("Constructor invocation must be first thing in a constructor");
      return -1;
    }
| SUPER OP CP error
    {
      javaperror ("Constructor invocation must be first thing in a constructor");
      return -1;
    }
| THIS OP argument_list error
    {
      javaperror ("')' expected");
      return -1;
    }
| SUPER OP argument_list error
    {
      javaperror ("')' expected");
      return -1;
    }
| THIS OP argument_list CP error
    {
      javaperror ("Constructor invocation must be first thing in a constructor");
      return -1;
    }
| SUPER OP argument_list CP error
    {
      javaperror ("Constructor invocation must be first thing in a constructor");
      return -1;
    }
// JR | PackageName DOT SUPER error
| primary DOT SUPER error
    {
      javaperror ("'(' expected");
      return -1;
    }
// JR | PackageName DOT SUPER OP error
| primary DOT SUPER OP error
    {
      javaperror ("')' expected");
      return -1;
    }
// JR | PackageName DOT SUPER OP argument_list error
| primary DOT SUPER OP argument_list error
    {
      javaperror ("')' expected");
      return -1;
    }
// JR | PackageName DOT SUPER OP argument_list CP error
| primary DOT SUPER OP argument_list CP error
    {
      javaperror ("';' expected (6)");
      return -1;
    }
// JR | PackageName DOT SUPER OP CP error
| primary DOT SUPER OP CP error
    {
      javaperror ("';' expected (7)");
      return -1;
    }
;

statement_expression:
  assignment
    {}
| PreInDecrementExpr
    {}
| post_increment_expression
    {}
| post_decrement_expression
    {}
| FctCall
    {}
| ClassInstanceCreationExpr
    {}
| error SEMICOLON
    {
      javaperror ("Invalid expression statement (1)");
      return -1;
    }
| error OCB
    {
      javaperror ("Invalid expression statement (2)");
      return -1;
    }
| error CCB
    {
      javaperror ("Invalid expression statement (3)");
      return -1;
    }
;
@}

  IfStmt ::
    expr : Expr
    alt1 : Stmt
    alt2 : [ Stmt ]
    cid :- CI`ContextId;

@d Productions @{

if_then_statement:
  IF OP expression CP statement
    {
      $$ = new TYPE_CPP_IfStmt ();
      $$ -> set_expr (*$3);
      delete $3;
      $$ -> set_alt1 (*$5);
      delete $5;
      $$ -> set_alt2 (Nil ());
      javapSetPos2 (*$$, @@1, @@5);
    }
| IF error
    {
      javaperror ("'(' expected");
      return -1;
    }
| IF OP error
    {
      javaperror ("Missing term");
      return -1;
    }
| IF OP expression error
    {
      javaperror ("')' expected");
      return -1;
    }
;

if_then_else_statement:
  IF OP expression CP statement_nsi ELSE statement
    {
      $$ = new TYPE_CPP_IfStmt ();
      $$ -> set_expr (*$3);
      delete $3;
      $$ -> set_alt1 (*$5);
      delete $5;
      $$ -> set_alt2 (*$7);
      delete $7;
      javapSetPos2 (*$$, @@1, @@7);
    }
;

if_then_else_statement_nsi:
  IF OP expression CP statement_nsi ELSE statement_nsi
    {
      $$ = new TYPE_CPP_IfStmt ();
      $$ -> set_expr (*$3);
      delete $3;
      $$ -> set_alt1 (*$5);
      delete $5;
      $$ -> set_alt2 (*$7);
      delete $7;
      javapSetPos2 (*$$, @@1, @@7);
    }
;
@}

  SwitchStmt ::
    expr : Expr
    block : SwitchBlock
    cid :- CI`ContextId;

@d Productions @{

SwitchStmt:
  switch_expression SwitchBlock
    {
      $$ = new TYPE_CPP_SwitchStmt ();
      $$ -> set_expr (*$1);
      delete $1;
      $$ -> set_block (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
;

switch_expression:
  SWITCH OP expression CP
    {
/*
      $$ = new TYPE_CPP_ExpressionStmt ();
      $$ -> set_expr (*$3);
      delete $3;
      $$ -> set_cid (-1);
*/
      $$ = $3;
    }
| SWITCH error
    {
      javaperror ("'(' expected");
      return -1;
    }
| SWITCH OP error
    {
      javaperror ("Missing term or ')'");
      return -1;
    }
| SWITCH OP expression CP error
    {
      javaperror ("'{' expected");
      return -1;
    }
;
@}

  SwitchBlock ::
    groups : seq of SwitchBlockStmtGrp
    labels : [SwitchLabels]
    cid :- CI`ContextId;

@d Productions @{

SwitchBlock:
  OCB CCB
    {
      $$ = new TYPE_CPP_SwitchBlock ();
      $$ -> set_groups ((const SEQ<TYPE_CPP_SwitchBlockStmtGrp>) Sequence ());
      $$ -> set_labels (Nil ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| OCB SwitchLabels CCB
    {
      $$ = new TYPE_CPP_SwitchBlock ();
      $$ -> set_groups ((const SEQ<TYPE_CPP_SwitchBlockStmtGrp>) Sequence ());
      $$ -> set_labels (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB SwitchBlockStmtGrps CCB
    {
      $$ = new TYPE_CPP_SwitchBlock ();
      $$ -> set_groups ((const SEQ<TYPE_CPP_SwitchBlockStmtGrp>) *$2);
      delete $2;
      $$ -> set_labels (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| OCB SwitchBlockStmtGrps SwitchLabels CCB
    {
      $$ = new TYPE_CPP_SwitchBlock ();
      $$ -> set_groups ((const SEQ<TYPE_CPP_SwitchBlockStmtGrp>) *$2);
      delete $2;
      $$ -> set_labels (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
;

SwitchBlockStmtGrps:
  SwitchBlockStmtGrp
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| SwitchBlockStmtGrps SwitchBlockStmtGrp
    {
      $1 -> ImpAppend (*$2);
      delete $2;
    }
;
@}

  SwitchBlockStmtGrp ::
    labels : [SwitchLabels]
    stmt  : CompoundStmt
    cid :- CI`ContextId;

@d Productions @{

SwitchBlockStmtGrp:
  SwitchLabels block_statements
    {
      TYPE_CPP_CompoundStmt cs;
      cs.set_stms ((const SEQ<Record>) *$2);
      delete $2;
      javapSetPos2 (cs, @@2, @@2);
      $$ = new TYPE_CPP_SwitchBlockStmtGrp ();
      $$ -> set_labels (*$1);
      delete $1;
      $$ -> set_stmt (cs);
      javapSetPos2 (*$$, @@1, @@2);
    }
;
@}

  SwitchLabels ::
    stmt   : CaseStmt | DefaultStmt
    labels : [SwitchLabels]
    cid :- CI`ContextId;

@d Productions @{

SwitchLabels:
  CaseStmt
    {
      $$ = new TYPE_CPP_SwitchLabels ();
      $$ -> set_stmt (*$1);
      delete $1;
      $$ -> set_labels (Nil ());
      javapSetPos2 (*$$, @@1, @@1);
    }
| SwitchLabels CaseStmt
    {
      $$ = new TYPE_CPP_SwitchLabels ();
      $$ -> set_stmt (*$2);
      delete $2;
      $$ -> set_labels (*$1);
      delete $1;
      javapSetPos2 (*$$, @@1, @@2);
    }
;
@}

  CaseStmt ::
    cexpr : Expr
    stmt  : nil
    cid :- CI`ContextId;

  DefaultStmt ::
    stmt : nil
    cid :- CI`ContextId;

@d Productions @{

CaseStmt:
  CASE constant_expression COLON
    {
      TYPE_CPP_CaseStmt* cs = new TYPE_CPP_CaseStmt ();
      cs -> set_cexpr (*$2);
      delete $2;
      cs -> set_stmt (Nil ());
      javapSetPos2 (*cs, @@1, @@3);
      $$ = cs;
    }
| DEFAULT COLON
    {
      TYPE_CPP_DefaultStmt* ds = new TYPE_CPP_DefaultStmt ();
      ds -> set_stmt (Nil ());
      javapSetPos2 (*ds, @@1, @@2);
      $$ = ds;
    }
| CASE error
    {
      javaperror ("Missing or invalid constant expression");
      return -1;
    }
| CASE constant_expression error
    {
      javaperror ("':' expected");
      return -1;
    }
| DEFAULT error
    {
      javaperror ("':' expected");
      return -1;
    }
;

while_expression:
  WHILE OP expression CP
    {
      $$ = new TYPE_CPP_WhileStmt ();
      $$ -> set_expr (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
;
@}

  WhileStmt ::
    expr : Expr
    stmt : Stmt
    cid :- CI`ContextId;

@d Productions @{

while_statement:
  while_expression statement
    {
      $1 -> set_stmt (*$2);
      delete $2;
    }
| WHILE error
    {
      javaperror ("'(' expected");
      return -1;
    }
| WHILE OP error
    {
      javaperror ("Missing term and ')' expected");
      return -1;
    }
| WHILE OP expression error
    {
      javaperror ("')' expected");
      return -1;
    }
;

while_statement_nsi:
  while_expression statement_nsi
    {
      $1 -> set_stmt (*$2);
      delete $2;
    }
;
@}

  DoWhileStmt ::
    stmt : Stmt
    expr : Expr
    cid :- CI`ContextId;

@d Productions @{

DoWhileStmt:
  DO statement WHILE OP expression CP SEMICOLON
    {
      $$ = new TYPE_CPP_DoWhileStmt ();
      $$ -> set_stmt (*$2);
      delete $2;
      $$ -> set_expr (*$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@7);
    }
;
@}

  ForStmt ::
    i     : seq of (ExpressionStmt | DeclarationStmt)
    expr1 : [ Expr ]
    expr2 : seq of Expr
    stmt  : Stmt
    cid :- CI`ContextId
 inv forstmt == isCPP() => len forstmt.expr2 = 1;

@d Productions @{

for_statement:
  for_begin SEMICOLON expression SEMICOLON for_update CP statement
    {
      $$ = new TYPE_CPP_ForStmt ();
      $$ -> set_i ((const SEQ<Record>) *$1);
      delete $1;
      $$ -> set_expr1 (*$3);
      delete $3;
      $$ -> set_expr2 ((const SEQ<Record>) *$5);
      delete $5;
      $$ -> set_stmt (*$7);
      delete $7;
      javapSetPos2 (*$$, @@1, @@7);
    }
| for_begin SEMICOLON SEMICOLON for_update CP statement
    {
      $$ = new TYPE_CPP_ForStmt ();
      $$ -> set_i ((const SEQ<Record>) *$1);
      delete $1;
      $$ -> set_expr1 (Nil ());
      $$ -> set_expr2 ((const SEQ<Record>) *$4);
      delete $4;
      $$ -> set_stmt (*$6);
      delete $6;
      javapSetPos2 (*$$, @@1, @@6);
    }
| for_begin SEMICOLON error
    {
      javaperror ("Invalid control expression");
      return -1;
    }
| for_begin SEMICOLON expression SEMICOLON error
    {
      javaperror ("Invalid update expression");
      return -1;
    }
| for_begin SEMICOLON SEMICOLON error
    {
      javaperror ("Invalid update expression");
      return -1;
    }
;

for_statement_nsi:
  for_begin SEMICOLON expression SEMICOLON for_update CP statement_nsi
    {
      $$ = new TYPE_CPP_ForStmt ();
      $$ -> set_i ((const SEQ<Record>) *$1);
      delete $1;
      $$ -> set_expr1 (*$3);
      delete $3;
      $$ -> set_expr2 ((const SEQ<Record>) *$5);
      delete $5;
      $$ -> set_stmt (*$7);
      delete $7;
      javapSetPos2 (*$$, @@1, @@7);
    }
| for_begin SEMICOLON SEMICOLON for_update CP statement_nsi
    {
      $$ = new TYPE_CPP_ForStmt ();
      $$ -> set_i ((const SEQ<Record>) *$1);
      delete $1;
      $$ -> set_expr1 (Nil ());
      $$ -> set_expr2 ((const SEQ<Record>) *$4);
      delete $4;
      $$ -> set_stmt (*$6);
      delete $6;
      javapSetPos2 (*$$, @@1, @@6);
    }
;

for_header:
  FOR OP
    {}
| FOR error
    {
      javaperror ("'(' expected");
      return -1;
    }
| FOR OP error
    {
      javaperror ("Invalid init statement");
      return -1;
    }
;

for_begin:
  for_header for_init
    {
      $$ = $2;
    }
;

for_init:
    {
      $$ = new Sequence ();
    }
| statement_expression_list
    {}
| local_variable_declaration
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| statement_expression_list error
    {
      javaperror ("';' expected (8)");
      return -1;
    }
;
@}

 ForEachStmt ::
    e    : DeclarationStmt
    expr : Expr 
    stmt : Stmt
    CNTXT
 ;

@d Productions @{

foreach_statement:
  for_begin COLON expression CP statement
    {
      $$ = new TYPE_CPP_ForEachStmt ();
      SEQ<Record> s ((const SEQ<Record>) *$1);
      $$ -> set_e (s[1]);
      $$ -> set_expr (*$3);
      $$ -> set_stmt (*$5);
      delete $1;
      delete $3;
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
;
@}

Change to rule 'for_update':
The production alternative was changed from:
| statement_expression_list
to :
| argument_list
- just a sequence of expressions without the TYPE_CPP_ExpressionStmt-wrapper
as the original alternative has.
This change demands that the '*.java' file is correctly compiled with 'javac'
before handing over to this 'parser'.

@d Productions @{

for_update:
    {
      $$ = new Sequence ();
    }
| argument_list    // JR | statement_expression_list
    {}
;

statement_expression_list:
  statement_expression
    {
      TYPE_CPP_ExpressionStmt es;
      es.set_expr (*$1);
      delete $1;
      javapSetPos2 (es, @@1, @@1);
      $$ = new Sequence ();
      $$ -> ImpAppend (es);
    }
| statement_expression_list COMMA statement_expression
    {
      TYPE_CPP_ExpressionStmt es;
      es.set_expr (*$3);
      delete $3;
      javapSetPos2 (es, @@3, @@3);
      $1 -> ImpAppend (es);
    }
| statement_expression_list COMMA error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  Break ::
    id  : [Identifier]
    cid :- CI`ContextId;

@d Productions @{

Break:
  BREAK SEMICOLON
    {
      $$ = new TYPE_CPP_Break ();
      $$ -> set_id (Nil ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| BREAK Identifier SEMICOLON
    {
      $$ = new TYPE_CPP_Break ();
      $$ -> set_id (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| BREAK error
    {
      javaperror ("Missing term");
      return -1;
    }
| BREAK Identifier error
    {
      javaperror ("';' expected (9)");
      return -1;
    }
;
@}

  Continue ::
    id  : [Identifier]
    cid :- CI`ContextId;

@d Productions @{

Continue:
  CONTINUE SEMICOLON
    {
      $$ = new TYPE_CPP_Continue ();
      $$ -> set_id (Nil ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| CONTINUE Identifier SEMICOLON
    {
      $$ = new TYPE_CPP_Continue ();
      $$ -> set_id (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| CONTINUE error
    {
      javaperror ("Missing term");
      return -1;
    }
| CONTINUE Identifier error
    {
      javaperror ("';' expected (10)");
      return -1;
    }
;
@}

  Return ::
    expr : [ Expr ]
    cid :- CI`ContextId;

@d Productions @{

Return:
  RETURN SEMICOLON
    {
      $$ = new TYPE_CPP_Return ();
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| RETURN expression SEMICOLON
    {
      $$ = new TYPE_CPP_Return ();
      $$ -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| RETURN error
    {
      javaperror ("Missing term");
      return -1;
    }
| RETURN expression error
    {
      javaperror ("';' expected (11)");
      return -1;
    }
;
@}

  ThrowExpression ::
    expr : [ Expr ]
    cid :- CI`ContextId;

@d Productions @{

ThrowExpression:
  THROW expression SEMICOLON
    {
      $$ = new TYPE_CPP_ThrowExpression ();
      $$ -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@3);
    }
| THROW error
    {
      javaperror ("Missing term");
      return -1;
    }
| THROW expression error
    {
      javaperror ("';' expected (12)");
      return -1;
    }
;
@}

  SynchronizedStmt ::
    expr : Expr
    block: CompoundStmt
    cid :- CI`ContextId;

@d Productions @{

SynchronizedStmt:
  SYNCHRONIZED OP expression CP CompoundStmt
    {
      $$ = new TYPE_CPP_SynchronizedStmt ();
      $$ -> set_expr (*$3);
      delete $3;
      $$ -> set_block (*$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| SYNCHRONIZED OP expression CP error
    {
      javaperror ("'{' expected");
      return -1;
    }
| SYNCHRONIZED error
    {
      javaperror ("'(' expected");
      return -1;
    }
| SYNCHRONIZED OP error CP
    {
      javaperror ("Missing term");
      return -1;
    }
| SYNCHRONIZED OP error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  TryBlock ::
    stmt : CompoundStmt
    hdl  : seq of Handler
    fin : [CompoundStmt]
    cid :- CI`ContextId;

@d Productions @{

TryBlock:
  TRY CompoundStmt catches
    {
      $$ = new TYPE_CPP_TryBlock ();
      $$ -> set_stmt (*$2);
      delete $2;
      $$ -> set_hdl ((const SEQ<TYPE_CPP_Handler>) *$3);
      delete $3;
      $$ -> set_fin (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| TRY CompoundStmt finally
    {
      $$ = new TYPE_CPP_TryBlock ();
      $$ -> set_stmt (*$2);
      delete $2;
      $$ -> set_hdl ((const SEQ<TYPE_CPP_Handler>) Sequence ());
      $$ -> set_fin (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| TRY CompoundStmt catches finally
    {
      $$ = new TYPE_CPP_TryBlock ();
      $$ -> set_stmt (*$2);
      delete $2;
      $$ -> set_hdl ((const SEQ<TYPE_CPP_Handler>) *$3);
      delete $3;
      $$ -> set_fin (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| TRY error
    {
      javaperror ("'{' expected");
      return -1;
    }
;

catches:
  catch_clause
    {
       $$ = new Sequence ();
       $$ -> ImpAppend (*$1);
       delete $1;
    }
| catches catch_clause
    {
       $1 -> ImpAppend (*$2);
       delete $2;
    }
;
@}

  Handler ::
    exepdecl : All | ExceptionDeclaration
    stmt     : Stmt
    cid :- CI`ContextId;

  All :: cid :- CI`ContextId;

@d Productions @{

catch_clause:
  ExceptionDeclaration CompoundStmt
    {
      $$ = new TYPE_CPP_Handler ();
      $$ -> set_exepdecl (*$1);
      delete $1;
      $$ -> set_stmt (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
;
@}

  ExceptionDeclaration ::
    tsl  : seq of TypeSpecifier
    decl : [ Declarator | AbstractDeclarator ]
    cid :- CI`ContextId;

  Declarator = Name | FctDecl | ArrayDecl

  Name = Identifier | Destructor | QualifiedName | PackageName

JR ?
1. tsl : seq of DeclSpecifier
2. AbstractDeclarator never used ! b.c.o. ArgumentDeclaration

ExceptionDeclaration: CATCH OP ArgumentDeclaration CP | ...

  ArgumentDeclaration = ArgDecl | DefaultArgDecl | ArgAbsDecl | DefaultArgAbsDecl;

  ArgDecl ::
    ds  : seq of DeclSpecifier
    decl: Declarator
    CNTXT;

 DeclSpecifier = StorageClassSpecifier | TypeSpecifier | ArraySpecifier | Modifier

  TypeSpecifier ::
    tp :  SimpleTypeName | ClassSpecifier | InterfaceSpecifier;
    CNTXT

@d Productions @{

ExceptionDeclaration:
  CATCH OP ArgumentDeclaration CP
    {
      $$ = new TYPE_CPP_ExceptionDeclaration ();
      $$ -> set_tsl ((const SEQ<TYPE_CPP_TypeSpecifier>) ((TYPE_CPP_ArgDecl) *$3).get_ds ());
      $$ -> set_decl (((TYPE_CPP_ArgDecl) *$3).get_decl ());
      $$ -> set_cid (((TYPE_CPP_ArgDecl) *$3).get_cid ());
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
| CATCH error
    {
      javaperror ("'(' expected");
      return -1;
    }
| CATCH OP error
    {
      javaperror ("Missing term or ')' expected");
      return -1;
    }
| CATCH OP error CP /* That's for () */
    {
      javaperror ("')' expected");
      return -1;
    }
;

finally:
  FINALLY CompoundStmt
    {
       $$ = $2;
    }
| FINALLY error
    {
      javaperror ("'{' expected");
      return -1;
    }
;
@}

19.12 Production from 15: Expressions

@d Productions @{

primary:
  primary_no_new_array
    {}
| ArrayCreationExpr
    {}
;
@}

  BracketedExpr ::
    expr : Expr
    cid :- CI`ContextId;

@d Productions @{

primary_no_new_array:
  Literal
    {}
| THIS
    {
      TYPE_CPP_This* t = new TYPE_CPP_This ();
      javapSetPos2 (*t, @@1, @@1);
      $$ = t;
    }
| OP expression CP
    {
      TYPE_CPP_BracketedExpr* be = new TYPE_CPP_BracketedExpr();
      be -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*be, @@1, @@3);
      $$ = be;
    }
| ClassInstanceCreationExpr
    {}
| field_access
    {}
| FctCall
    {}
| ArrayApply
    {}
| PackageName DOT CLASS    /* Added, JDK1.1 class literals */
    {
      TYPE_CPP_ClassExpr* ce = new TYPE_CPP_ClassExpr ();
      ce -> set_operand (*$1);
      delete $1;
      javapSetPos2 (*ce, @@1, @@3);
      $$ = ce;
    }
| primitive_type DOT CLASS /* Added, JDK1.1 class literals */
    {
      TYPE_CPP_ClassExpr* ce = new TYPE_CPP_ClassExpr ();
      ce -> set_operand (*$1);
      delete $1;
      javapSetPos2 (*ce, @@1, @@3);
      $$ = ce;
    }
| PackageName DOT THIS
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"this"));
      javapSetPos2 (i, @@3, @@3);
      TYPE_CPP_QualifiedPackageName* qpn = new TYPE_CPP_QualifiedPackageName ();
      qpn -> set_id (i);
      qpn -> set_pn (*$1);
      delete $1;
      javapSetPos2 (*qpn, @@1, @@3);
      $$ = qpn;
    }
| OP expression error
    {
      javaperror ("')' expected");
      return -1;
    }
| PackageName DOT error
    {
      javaperror ("'class' or 'this' expected" );
      return -1;
    }
| primitive_type DOT error
    {
      javaperror ("'class' expected" );
      return -1;
    }
;
@}

  ClassInstanceCreationExpr ::
    classtype       : CompleteClassName | PackageName
    somethingDotNew : [Expr]
    arg             : seq of Expr
    ml              : [seq of MemberDeclaration]
    cid             :- CI`ContextId;

@d Productions @{

ClassInstanceCreationExpr:
  NEW class_type OP argument_list CP class_body
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (Nil ());
      $$ -> set_arg ((const SEQ<Record>) *$4);
      delete $4;
      $$ -> set_ml ((const SEQ<Record>) *$6);
      delete $6;
      javapSetPos2 (*$$, @@1, @@6);
    }
| NEW class_type OP CP class_body
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (Nil());
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      $$ -> set_ml ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| NEW class_type OP argument_list CP
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (Nil ());
      $$ -> set_arg ((const SEQ<Record>) *$4);
      delete $4;
      $$ -> set_ml (Nil());
      javapSetPos2 (*$$, @@1, @@5);
    }
| NEW class_type OP CP
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (Nil ());
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      $$ -> set_ml (Nil ());
      javapSetPos2 (*$$, @@1, @@4);
    }
| something_dot_new Identifier OP argument_list CP class_body
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (*$1);
      delete $1;
      $$ -> set_arg ((const SEQ<Record>) *$4);
      delete $4;
      $$ -> set_ml ((const SEQ<Record>) *$6);
      delete $6;
      javapSetPos2 (*$$, @@1, @@6);
    }
| something_dot_new Identifier OP CP class_body
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (*$1);
      delete $1;
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      $$ -> set_ml ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| something_dot_new Identifier OP argument_list CP
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (*$1);
      delete $1;
      $$ -> set_arg ((const SEQ<Record>) *$4);
      delete $4;
      $$ -> set_ml (Nil ());
      javapSetPos2 (*$$, @@1, @@5);
    }
| something_dot_new Identifier OP CP
    {
      $$ = new TYPE_CPP_ClassInstanceCreationExpr ();
      $$ -> set_classtype (*$2);
      delete $2;
      $$ -> set_somethingDotNew (*$1);
      delete $1;
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      $$ -> set_ml (Nil ());
      javapSetPos2 (*$$, @@1, @@4);
    }
| NEW error SEMICOLON
    {
      javaperror ("'(' expected");
      return -1;
    }
| NEW class_type error
    {
      javaperror ("'(' expected");
      return -1;
    }
| NEW class_type OP error
    {
      javaperror ("')' or term expected");
      return -1;
    }
| NEW class_type OP argument_list error
    {
      javaperror ("')' expected");
      return -1;
    }
| something_dot_new error
    {
      javaperror ("Identifier expected");
      return -1;
    }
| something_dot_new Identifier error
    {
      javaperror ("'(' expected");
      return -1;
    }
;

something_dot_new:    /* Added, not part of the specs. */
  PackageName DOT NEW
    {}
| primary DOT NEW
    {}
;

argument_list:
  argument
    {
      $$ = new Sequence ();
      $$ -> ImpAppend (*$1);
      delete $1;
    }
| argument_list COMMA argument
    {
      $1 -> ImpAppend (*$3);
      delete $3;
    }
| argument_list COMMA error
    {
      javaperror ("Missing term");
      return -1;
    }
;

argument:
  expression
    {}
;
@}

  ArrayCreationExpr ::
    type      : SimpleTypeName
    dimExpr   : [ArrayNewDecl]
    dim       : [ArrayNewDecl]
    arrayInit : [ArrayInitializer]
    cid :- CI`ContextId;

@d Productions @{

ArrayCreationExpr:
  NEW primitive_type dim_exprs
    {
      $$ = new TYPE_CPP_ArrayCreationExpr ();
      $$ -> set_type (*$2);
      delete $2;
      $$ -> set_dimExpr (*$3);
      delete $3;
      $$ -> set_dim (Nil ());
      $$ -> set_arrayInit (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| NEW primitive_type dim_exprs dims
    {
      $$ = new TYPE_CPP_ArrayCreationExpr ();
      $$ -> set_type (*$2);
      delete $2;
      $$ -> set_dimExpr (*$3);
      delete $3;
      $$ -> set_dim (*$4);
      delete $4;
      $$ -> set_arrayInit (Nil ());
      javapSetPos2 (*$$, @@1, @@4);
    }
| NEW primitive_type dims ArrayInitializer
    {
      $$ = new TYPE_CPP_ArrayCreationExpr ();
      $$ -> set_type (*$2);
      delete $2;
      $$ -> set_dimExpr (Nil ());
      $$ -> set_dim (*$3);
      delete $3;
      $$ -> set_arrayInit (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| NEW PackageName dim_exprs
    {
      $$ = new TYPE_CPP_ArrayCreationExpr ();
      $$ -> set_type (*$2);
      delete $2;
      $$ -> set_dimExpr (*$3);
      delete $3;
      $$ -> set_dim (Nil ());
      $$ -> set_arrayInit (Nil ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| NEW PackageName dim_exprs dims
    {
      $$ = new TYPE_CPP_ArrayCreationExpr ();
      $$ -> set_type (*$2);
      delete $2;
      $$ -> set_dimExpr (*$3);
      delete $3;
      $$ -> set_dim (*$4);
      delete $4;
      $$ -> set_arrayInit (Nil ());
      javapSetPos2 (*$$, @@1, @@4);
    }
| NEW PackageName dims ArrayInitializer
    {
      $$ = new TYPE_CPP_ArrayCreationExpr ();
      $$ -> set_type (*$2);
      delete $2;
      $$ -> set_dimExpr (Nil ());
      $$ -> set_dim (*$3);
      delete $3;
      $$ -> set_arrayInit (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| NEW error CSB
    {
      javaperror ("'[' expected");
      return -1;
    }
| NEW error OSB
    {
      javaperror ("']' expected");
      return -1;
    }
;
@}

  ArrayNewDecl ::
    newdecl : [NewDeclarator]
    expr    : [Expr]            -- added optional for Java
    cid :- CI`ContextId;

  NewDeclarator = CVNewDeclarator | ClassNewDecl | ArrayNewDecl;

@d Productions @{

dim_exprs:
  dim_expr
    {
      $$ = new TYPE_CPP_ArrayNewDecl ();
      $$ -> set_newdecl (Nil ());
      $$ -> set_expr (*$1);
      delete $1;
      javapSetPos2 (*$$, @@1, @@1);
    }
| dim_exprs dim_expr
    {
      $$ = new TYPE_CPP_ArrayNewDecl ();
      $$ -> set_newdecl (*$1);
      delete $1;
      $$ -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
;

dim_expr:
  OSB expression CSB
    {
      $$ = $2;
    }
| OSB expression error
    {
      javaperror ("']' expected");
      return -1;
    }
| OSB error
    {
      javaperror ("Missing term, ']' expected");
      return -1;
    }
;
@}

  ObjectMemberAccess ::
    object : Expr
    name   : Expr
    cid :- CI`ContextId;

@d Productions @{

field_access:
  primary DOT Identifier
    {
      $$ = new TYPE_CPP_ObjectMemberAccess ();
      $$ -> set_object (*$1);
      delete $1;
      $$ -> set_name (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| SUPER DOT Identifier
    {
      TYPE_CPP_Super s;
      javapSetPos2 (s, @@1, @@1);
      $$ = new TYPE_CPP_ObjectMemberAccess ();
      $$ -> set_object (s);
      $$ -> set_name (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
/* Part of 'java12.cup' */
| PackageName DOT SUPER DOT Identifier
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@3, @@3);
//JR linked list of ObjectMemberAccess ????
      TYPE_CPP_QualifiedPackageName qpn;
      qpn.set_id (i);
      qpn.set_pn (*$1);
      delete $1;
      javapSetPos2 (qpn, @@1, @@3);
      $$ = new TYPE_CPP_ObjectMemberAccess ();
      $$ -> set_object (qpn);
      $$ -> set_name (*$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| SUPER error
    {
      javaperror ("Field expected");
      return -1;
    }
;
@}

  FctCall ::
    fct : Expr
    arg : Exprs
    cid :- CI`ContextId;

  QualifiedName ::
    qcn : QualifiedClassName | Expr
    id  : Name
    CNTXT;

@d Productions @{

FctCall:
  PackageName OP CP
    {
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (*$1);
      delete $1;
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (*$$, @@1, @@3);
    }
| PackageName OP argument_list CP
    {
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (*$1);
      delete $1;
      $$ -> set_arg ((const SEQ<Record>) *$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
| primary DOT Identifier OP CP
    {
      TYPE_CPP_QualifiedName qn;
      qn.set_qcn (*$1);
      delete $1;
      qn.set_id (*$3);
      delete $3;
      javapSetPos2 (qn, @@1, @@3);
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (qn);
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (*$$, @@1, @@5);
    }
| primary DOT Identifier OP argument_list CP
    {
      TYPE_CPP_QualifiedName qn;
      qn.set_qcn (*$1);
      delete $1;
      qn.set_id (*$3);
      delete $3;
      javapSetPos2 (qn, @@1, @@3);
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (qn);
      $$ -> set_arg ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@6);
    }
| SUPER DOT Identifier OP CP
    {
// JR ? use QualifiedName instead ?
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@1, @@1);
      TYPE_CPP_SimplePackageName spn;
      spn.set_id (i);
      javapSetPos2 (spn, @@1, @@1);
      TYPE_CPP_QualifiedPackageName qpn;
      qpn.set_pn (spn);
      qpn.set_id (*$3);
      delete $3;
      javapSetPos2 (qpn, @@1, @@3);
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (qpn);
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (*$$, @@1, @@5);
    }
| SUPER DOT Identifier OP argument_list CP
// JR ? use QualifiedName instead ?
    {
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@1, @@1);
      TYPE_CPP_SimplePackageName spn;
      spn.set_id (i);
      javapSetPos2 (spn, @@1, @@1);
      TYPE_CPP_QualifiedPackageName qpn;
      qpn.set_pn (spn);
      qpn.set_id (*$3);
      delete $3;
      javapSetPos2 (qpn, @@1, @@3);
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (qpn);
      $$ -> set_arg ((const SEQ<Record>) *$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@6);
    }
/* Part of 'java12.cup' */
| PackageName DOT SUPER DOT Identifier OP CP
    {
// JR order: pn super id ?
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@3, @@3);
      TYPE_CPP_QualifiedPackageName qpn1;
      qpn1.set_id (i);
      qpn1.set_pn (*$1);
      delete $1;
      javapSetPos2 (qpn1, @@1, @@3);
      TYPE_CPP_QualifiedPackageName qpn2;
      qpn2.set_pn (qpn1);
      qpn2.set_id (*$5);
      delete $5;
      javapSetPos2 (qpn2, @@1, @@5);
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (qpn2);
      $$ -> set_arg ((const SEQ<Record>) Sequence ());
      javapSetPos2 (*$$, @@1, @@7);
    }
/* Part of 'java12.cup' */
| PackageName DOT SUPER DOT Identifier OP argument_list CP
    {
// JR order: pn super id ?
      TYPE_CPP_Identifier i;
      i.set_id ((const SEQ<Char>) Sequence (L"super"));
      javapSetPos2 (i, @@3, @@3);
      TYPE_CPP_QualifiedPackageName qpn1;
      qpn1.set_id (i);
      qpn1.set_pn (*$1);
      delete $1;
      javapSetPos2 (qpn1, @@1, @@3);
      TYPE_CPP_QualifiedPackageName qpn2;
      qpn2.set_pn (qpn1);
      qpn2.set_id (*$5);
      delete $5;
      javapSetPos2 (qpn2, @@1, @@5);
      $$ = new TYPE_CPP_FctCall ();
      $$ -> set_fct (qpn2);
      $$ -> set_arg ((const SEQ<Record>) *$7);
      delete $7;
      javapSetPos2 (*$$, @@1, @@8);
    }
| primary DOT error
    {
      javaperror ("'(' expected");
      return -1;
    }
| SUPER DOT error CP
    {
      javaperror ("'(' expected");
      return -1;
    }
| SUPER DOT error DOT
    {
      javaperror ("'(' expected");
      return -1;
    }
;
@}

  ArrayApply ::
    array : Expr
    arg   : Expr
    cid :- CI`ContextId;

@d Productions @{

ArrayApply:
  PackageName OSB expression CSB
    {
      $$ = new TYPE_CPP_ArrayApply ();
      $$ -> set_array (*$1);
      delete $1;
      $$ -> set_arg (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
| primary_no_new_array OSB expression CSB
    {
      $$ = new TYPE_CPP_ArrayApply ();
      $$ -> set_array (*$1);
      delete $1;
      $$ -> set_arg (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@4);
    }
| PackageName OSB error
    {
      javaperror ("Missing term and ']' expected");
      return -1;
    }
| PackageName OSB expression error
    {
      javaperror ("']' expected");
      return -1;
    }
| primary_no_new_array OSB error
    {
      javaperror ("Missing term and ']' expected");
      return -1;
    }
| primary_no_new_array OSB expression error
    {
      javaperror ("']' expected");
      return -1;
    }
;

postfix_expression:
  primary
    {}
| PackageName
    {}
| post_increment_expression
    {}
| post_decrement_expression
    {}
;
@}

  PostFixInDecrementExpr ::
    op   : <PPLUS> | <PMINUS>
    expr : Expr
    cid  :- CI`ContextId;

@d Productions @{

post_increment_expression:
  postfix_expression PPLUS
    {
      $$ = new TYPE_CPP_PostFixInDecrementExpr ();
      $$ -> set_op (Quote (L"PPLUS"));
      $$ -> set_expr (*$1);
      delete $1;
      javapSetPos2 (*$$, @@1, @@2);
    }
;

post_decrement_expression:
  postfix_expression PMINUS
    {
      $$ = new TYPE_CPP_PostFixInDecrementExpr ();
      $$ -> set_op (Quote (L"PMINUS"));
      $$ -> set_expr (*$1);
      delete $1;
      javapSetPos2 (*$$, @@1, @@2);
    }
;
@}

  UnaryOpExpr ::
    op   : UnaryOp
    expr : Expr
    cid :- CI`ContextId;

  UnaryOp ::
    val : <PLUS> | <MINUS> | <NEG> | <COMPL>
    cid :- CI`ContextId

@d Productions @{

unary_expression:
  PreInDecrementExpr
    {}
| PLUS unary_expression
    {
      TYPE_CPP_UnaryOp uo;
      uo.set_val (Quote (L"PLUS"));
      javapSetPos2 (uo, @@1, @@1);
      TYPE_CPP_UnaryOpExpr* uoe = new TYPE_CPP_UnaryOpExpr ();
      uoe -> set_op (uo);
      uoe -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*uoe, @@1, @@2);
      $$ = uoe;
    }
| MINUS unary_expression
    {
      TYPE_CPP_UnaryOp uo;
      uo.set_val (Quote (L"MINUS"));
      javapSetPos2 (uo, @@1, @@1);
      TYPE_CPP_UnaryOpExpr* uoe = new TYPE_CPP_UnaryOpExpr ();
      uoe -> set_op (uo);
      uoe -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*uoe, @@1, @@2);
      $$ = uoe;
    }
| unary_expression_not_plus_minus
    {}
| PLUS error
    {
      javaperror ("Missing term");
      return -1;
    }
| MINUS error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  PreInDecrementExpr ::
    op   : <PPLUS> | <PMINUS>
    expr : Expr
    cid :- CI`ContextId;

@d Productions @{

PreInDecrementExpr:
  PPLUS unary_expression
    {
      $$ = new TYPE_CPP_PreInDecrementExpr ();
      $$ -> set_op (Quote (L"PPLUS"));
      $$ -> set_expr(*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
| PMINUS unary_expression
    {
      $$ = new TYPE_CPP_PreInDecrementExpr ();
      $$ -> set_op (Quote (L"PMINUS"));
      $$ -> set_expr(*$2);
      delete $2;
      javapSetPos2 (*$$, @@1, @@2);
    }
| PPLUS error
    {
      javaperror ("Missing term");
      return -1;
    }
| PMINUS error
    {
      javaperror ("Missing term");
      return -1;
    }
;

unary_expression_not_plus_minus:
  postfix_expression
    {}
| COMPL unary_expression
    {
      TYPE_CPP_UnaryOp uo;
      uo.set_val (Quote (L"COMPL"));
      javapSetPos2 (uo, @@1, @@1);
      TYPE_CPP_UnaryOpExpr* uoe = new TYPE_CPP_UnaryOpExpr ();
      uoe -> set_op (uo);
      uoe -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*uoe, @@1, @@2);
      $$ = uoe;
    }
| NEG unary_expression
    {
      TYPE_CPP_UnaryOp uo;
      uo.set_val (Quote (L"NEG"));
      javapSetPos2 (uo, @@1, @@1);
      TYPE_CPP_UnaryOpExpr* uoe = new TYPE_CPP_UnaryOpExpr ();
      uoe -> set_op (uo);
      uoe -> set_expr (*$2);
      delete $2;
      javapSetPos2 (*uoe, @@1, @@2);
      $$ = uoe;
    }
| CastExpr
    {}
| COMPL error
    {
      javaperror ("Missing term");
      return -1;
    }
| NEG error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  CastExpr ::
    typenm : TypeName | Expr
    expr   : Expr
    cid :- CI`ContextId;

  TypeName ::
    tsp : seq of TypeSpecifier
    ad  : [AbstractDeclarator]
    cid :- CI`ContextId;

  TypeSpecifier ::
    tp :  SimpleTypeName | ClassSpecifier | InterfaceSpecifier;
    CNTXT

  AbstractDeclarator = IndirectionAbsDecl | RefAbsDecl | PointerToMemberAbs
                     | FctAbsDecl | ArrayAbsDecl | BracketedAbsDecl;

  ArrayAbsDecl ::
    ad  : [AbstractDeclarator]
    ce  : [Expr]
    cid :- CI`ContextId;

Error handling here is potentially weak

@d Productions @{

CastExpr:
  OP primitive_type dims CP unary_expression
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<TYPE_CPP_TypeSpecifier> s;
      s.ImpAppend (ts);
      TYPE_CPP_TypeName tn;
      tn.set_tsp (s);
      Generic ad (move_dims_2_ArrayAbsDecl (*$3));
      delete $3;
      tn.set_ad (ad);
      javapSetPos2 (tn, @@2, @@3);
      $$ = new TYPE_CPP_CastExpr ();
      $$ -> set_typenm (tn);
      $$ -> set_expr (*$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| OP primitive_type CP unary_expression
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<TYPE_CPP_TypeSpecifier> s;
      s.ImpAppend (ts);
      TYPE_CPP_TypeName tn;
      tn.set_tsp (s);
      tn.set_ad (Nil ());
      javapSetPos2 (tn, @@2, @@2);
      $$ = new TYPE_CPP_CastExpr ();
      $$ -> set_typenm (tn);
      $$ -> set_expr (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| OP expression CP unary_expression_not_plus_minus
    {
      $$ = new TYPE_CPP_CastExpr ();
      $$ -> set_typenm (*$2);
      delete $2;
      $$ -> set_expr (*$4);
      delete $4;
      javapSetPos2 (*$$, @@1, @@4);
    }
| OP PackageName dims CP unary_expression_not_plus_minus
    {
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$2);
      delete $2;
      javapSetPos2 (ts, @@2, @@2);
      SEQ<TYPE_CPP_TypeSpecifier> s;
      s.ImpAppend (ts);
      TYPE_CPP_TypeName tn;
      tn.set_tsp (s);
      Generic ad (move_dims_2_ArrayAbsDecl (*$3));
      delete $3;
      tn.set_ad (ad);
      javapSetPos2 (tn, @@2, @@3);
      $$ = new TYPE_CPP_CastExpr ();
      $$ -> set_typenm (tn);
      $$ -> set_expr (*$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| OP primitive_type OSB error
    {
      javaperror ("']' expected, invalid type expression");
      return -1;
    }
| OP error
    {
      javaperror ("Invalid type expression");
      return -1;
    }
| OP primitive_type dims CP error
    {
      javaperror ("Missing term");
      return -1;
    }
| OP primitive_type CP error
    {
      javaperror ("Missing term");
      return -1;
    }
| OP PackageName dims CP error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  BinExpr ::
    right : Expr
    binop : BinOp
    left  : Expr
    cid :- CI`ContextId;

  BinOp ::
    op  : <PLUS> | <MINUS> | <MULT> | <DIV> | <MOD>
    cid :- CI`ContextId;

@d Productions @{

multiplicative_expression:
  unary_expression
    {}
| multiplicative_expression MULT unary_expression
    {
      TYPE_CPP_BinOp bo;
      bo.set_op (Quote (L"MULT"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BinExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_binop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| multiplicative_expression DIV unary_expression
    {
      TYPE_CPP_BinOp bo;
      bo.set_op (Quote (L"DIV"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BinExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_binop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| multiplicative_expression MOD unary_expression
    {
      TYPE_CPP_BinOp bo;
      bo.set_op (Quote (L"MOD"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BinExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_binop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| multiplicative_expression MULT error
    {
      javaperror ("Missing term");
      return -1;
    }
| multiplicative_expression DIV error
    {
      javaperror ("Missing term");
      return -1;
    }
| multiplicative_expression MOD error
    {
      javaperror ("Missing term");
      return -1;
    }
;

additive_expression:
  multiplicative_expression
    {}
| additive_expression PLUS multiplicative_expression
    {
      TYPE_CPP_BinOp bo;
      bo.set_op (Quote (L"PLUS"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BinExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_binop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| additive_expression MINUS multiplicative_expression
    {
      TYPE_CPP_BinOp bo;
      bo.set_op (Quote (L"MINUS"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BinExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_binop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| additive_expression PLUS error
    {
      javaperror ("Missing term");
      return -1;
    }
| additive_expression MINUS error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  ShiftExpr ::
    right : Expr
    shop  : ShOp
    left  : Expr
    cid :- CI`ContextId;

  ShOp ::
    op  : <LEFTSHIFT> | <RIGHTSHIFT>
    cid :- CI`ContextId;

@d Productions @{

ShiftExpr:
  additive_expression
    {}
| ShiftExpr LEFTSHIFT additive_expression
    {
      TYPE_CPP_ShOp so;
      so.set_op (Quote (L"LEFTSHIFT"));
      javapSetPos2 (so, @@2, @@2);
      $$ = new TYPE_CPP_ShiftExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_shop (so);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| ShiftExpr RIGHTSHIFT additive_expression
    {
      TYPE_CPP_ShOp so;
      so.set_op (Quote (L"RIGHTSHIFT"));
      javapSetPos2 (so, @@2, @@2);
      $$ = new TYPE_CPP_ShiftExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_shop (so);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| ShiftExpr ZRS additive_expression
    {
      TYPE_CPP_ShOp so;
      so.set_op (Quote (L"UNSIGNEDRIGHTSHIFT"));
      javapSetPos2 (so, @@2, @@2);
      $$ = new TYPE_CPP_ShiftExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_shop (so);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| ShiftExpr LEFTSHIFT error
    {
      javaperror ("Missing term");
      return -1;
    }
| ShiftExpr RIGHTSHIFT error
    {
      javaperror ("Missing term");
      return -1;
    }
| ShiftExpr ZRS error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  RelationalExpr ::
    right : Expr
    relop : RelOp
    left  : Expr
    cid :- CI`ContextId;

  RelOp ::
    op  : <LT> | <GT> | <LEQ> | <GEQ>
    cid :- CI`ContextId;

@d Productions @{

RelationalExpr:
  ShiftExpr
    {}
| TypeCompExpr
    {}
| RelationalExpr LT ShiftExpr
    {
      TYPE_CPP_RelOp ro;
      ro.set_op (Quote (L"LT"));
      javapSetPos2 (ro, @@2, @@2);
      $$ = new TYPE_CPP_RelationalExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_relop (ro);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| RelationalExpr GT ShiftExpr
    {
      TYPE_CPP_RelOp ro;
      ro.set_op (Quote (L"GT"));
      javapSetPos2 (ro, @@2, @@2);
      $$ = new TYPE_CPP_RelationalExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_relop (ro);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| RelationalExpr LEQ ShiftExpr
    {
      TYPE_CPP_RelOp ro;
      ro.set_op (Quote (L"LEQ"));
      javapSetPos2 (ro, @@2, @@2);
      $$ = new TYPE_CPP_RelationalExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_relop (ro);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| RelationalExpr GEQ ShiftExpr
    {
      TYPE_CPP_RelOp ro;
      ro.set_op (Quote (L"GEQ"));
      javapSetPos2 (ro, @@2, @@2);
      $$ = new TYPE_CPP_RelationalExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_relop (ro);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| RelationalExpr LT error
    {
      javaperror ("Missing term");
      return -1;
    }
| RelationalExpr GT error
    {
      javaperror ("Missing term");
      return -1;
    }
| RelationalExpr LEQ error
    {
      javaperror ("Missing term");
      return -1;
    }
| RelationalExpr GEQ error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  TypeCompExpr::
    right : Expr | TypeName
    op    : TcOp
    left  : Expr
    cid :- CI`ContextId;

  TcOp ::
    op  : <INSTANCEOF>
    cid :- CI`ContextId;

  TypeName ::
    tsp : seq of TypeSpecifier
    ad  : [AbstractDeclarator]
    cid :- CI`ContextId;

@d Productions @{

TypeCompExpr:
 RelationalExpr INSTANCEOF primitive_type osb_csb_s
    {
      TYPE_CPP_TcOp to;
      to.set_op (Quote (L"ISINSTANCEOF"));
      javapSetPos2 (to, @@2, @@2);
      $$ = new TYPE_CPP_TypeCompExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_op (to);
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$3);
      delete $3;
      javapSetPos2 (*$$, @@3, @@3);
      SEQ<TYPE_CPP_TypeSpecifier> s;
      s.ImpAppend (ts);
      TYPE_CPP_TypeName tn;
      tn.set_tsp (s);
      if ($4 != NULL)
      {
        Generic ad (move_dims_2_ArrayAbsDecl (*$4));
        delete $4;
        tn.set_ad (ad);
        javapSetPos2 (tn, @@3, @@4);
        javapSetPos2 (*$$, @@1, @@4);
      }
      else {
        tn.set_ad (Nil ());
        javapSetPos2 (tn, @@3, @@3);
        javapSetPos2 (*$$, @@1, @@3);
      }
      $$ -> set_right (tn);
    }
| RelationalExpr INSTANCEOF PackageName osb_csb_s
    {
      TYPE_CPP_TcOp to;
      to.set_op (Quote (L"ISINSTANCEOF"));
      javapSetPos2 (to, @@2, @@2);
      $$ = new TYPE_CPP_TypeCompExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_op (to);
      TYPE_CPP_TypeSpecifier ts;
      ts.set_tp (*$3);
      delete $3;
      javapSetPos2 (ts, @@3, @@3);
      SEQ<TYPE_CPP_TypeSpecifier> s;
      s.ImpAppend (ts);
      TYPE_CPP_TypeName tn;
      tn.set_tsp (s);
      if ($4 != NULL)
      {
        Generic ad (move_dims_2_ArrayAbsDecl (*$4));
        tn.set_ad (ad);
        javapSetPos2 (tn, @@3, @@4);
        javapSetPos2 (*$$, @@1, @@4);
      }
      else {
        tn.set_ad (Nil ());
        javapSetPos2 (tn, @@3, @@3);
        javapSetPos2 (*$$, @@1, @@3);
      }
      $$ -> set_right (tn);
    }
| RelationalExpr INSTANCEOF error
    {
      javaperror ("Invalid reference type");
      return -1;
    }
;
@}

  EqualityExpr ::
    right : Expr
    eqop  : EqOp
    left  : Expr
    cid :- CI`ContextId;

  EqOp ::
    op  : <EQ> | <NEQ>
    cid :- CI`ContextId;

@d Productions @{

EqualityExpr:
  RelationalExpr
    {}
| EqualityExpr EQ RelationalExpr
    {
      TYPE_CPP_EqOp eo;
      eo.set_op (Quote (L"EQ"));
      javapSetPos2 (eo, @@2, @@2);
      $$ = new TYPE_CPP_EqualityExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_eqop (eo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| EqualityExpr NEQ RelationalExpr
    {
      TYPE_CPP_EqOp eo;
      eo.set_op (Quote (L"NEQ"));
      javapSetPos2 (eo, @@2, @@2);
      $$ = new TYPE_CPP_EqualityExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_eqop (eo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| EqualityExpr EQ error
    {
      javaperror ("Missing term");
      return -1;
    }
| EqualityExpr NEQ error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  BitExpr ::
    right : Expr
    bitop : BitOp
    left  : Expr
    cid :- CI`ContextId;

  BitOp ::
    op  : <EXCLOR> | <BITOR> | <BITAND>
    cid :- CI`ContextId;

@d Productions @{

and_expression:
  EqualityExpr
    {}
| and_expression BITAND EqualityExpr
    {
      TYPE_CPP_BitOp bo;
      bo.set_op (Quote (L"BITAND"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BitExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_bitop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| and_expression BITAND error
    {
      javaperror ("Missing term");
      return -1;
    }
;

exclusive_or_expression:
  and_expression
    {}
| exclusive_or_expression EXCLOR and_expression
    {
      TYPE_CPP_BitOp bo;
      bo.set_op (Quote (L"EXCLOR"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BitExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_bitop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| exclusive_or_expression EXCLOR error
    {
      javaperror ("Missing term");
      return -1;
    }
;

inclusive_or_expression:
  exclusive_or_expression
    {}
| inclusive_or_expression BITOR exclusive_or_expression
    {
      TYPE_CPP_BitOp bo;
      bo.set_op (Quote (L"BITOR"));
      javapSetPos2 (bo, @@2, @@2);
      $$ = new TYPE_CPP_BitExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_bitop (bo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| inclusive_or_expression BITOR error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  LogicalExpr ::
    right : Expr
    logop : LogOp
    left  : Expr
    cid :- CI`ContextId;

  LogOp ::
    op  : <OR> | <AND>
    cid :- CI`ContextId;

@d Productions @{

conditional_and_expression:
  inclusive_or_expression
    {}
| conditional_and_expression AND inclusive_or_expression
    {
      TYPE_CPP_LogOp lo;
      lo.set_op (Quote (L"AND"));
      javapSetPos2 (lo, @@2, @@2);
      $$ = new TYPE_CPP_LogicalExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_logop (lo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| conditional_and_expression AND error
    {
      javaperror ("Missing term");
      return -1;
    }
;

conditional_or_expression:
  conditional_and_expression
    {}
| conditional_or_expression OR conditional_and_expression
    {
      TYPE_CPP_LogOp lo;
      lo.set_op (Quote (L"OR"));
      javapSetPos2 (lo, @@2, @@2);
      $$ = new TYPE_CPP_LogicalExpr ();
      $$ -> set_left (*$1);
      delete $1;
      $$ -> set_logop (lo);
      $$ -> set_right (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| conditional_or_expression OR error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

  ConditionalExpr ::
    cond  : Expr
    expr1 : Expr
    expr2 : Expr
    cid :- CI`ContextId;

Error handling here is weak

@d Productions @{

ConditionalExpr:
  conditional_or_expression
    {}
| conditional_or_expression REL_QM expression COLON ConditionalExpr
    {
      $$ = new TYPE_CPP_ConditionalExpr ();
      $$ -> set_cond (*$1);
      delete $1;
      $$ -> set_expr1 (*$3);
      delete $3;
      $$ -> set_expr2 (*$5);
      delete $5;
      javapSetPos2 (*$$, @@1, @@5);
    }
| conditional_or_expression REL_QM COLON error
    {
      javaperror ("Missing term");
      return -1;
    }
| conditional_or_expression REL_QM error
    {
      javaperror ("Missing term");
      return -1;
    }
| conditional_or_expression REL_QM expression COLON error
    {
      javaperror ("Missing term");
      return -1;
    }
;

assignment_expression:
  ConditionalExpr
    {}
| assignment
    {}
;
@}

  AssignExpr ::
    unary      : Expr
    assignop   : AssignOp
    assignexpr : Expr
    cid :- CI`ContextId;

@d Productions @{

assignment:
  left_hand_side AssignOp assignment_expression
    {
      $$ = new TYPE_CPP_AssignExpr ();
      $$ -> set_unary (*$1);
      delete $1;
      $$ -> set_assignop (*$2);
      delete $2;
      $$ -> set_assignexpr (*$3);
      delete $3;
      javapSetPos2 (*$$, @@1, @@3);
    }
| left_hand_side AssignOp error
    {
      javaperror ("Missing term");
      return -1;
    }
;
@}

JR return ObjectMemberAccess

@d Productions @{

left_hand_side:
  PackageName
    {
//JR      ObjectMemberAccess* r = new ObjectMemberAccess ();
    }
| field_access
    {}
| ArrayApply
    {}
;
@}

  AssignOp ::
    op  : <ASEQUAL> | <ASMULT> | <ASDIV> | <ASMOD> | <ASPLUS> | <ASMINUS>
        | <ASRIGHTSHIFT> | <ASLEFTSHIFT>
        | <ASBITWISEAND> | <ASBITWISEEXOR> | <ASBITWISEINCLOR>
    cid :- CI`ContextId;

@d Productions @{

AssignOp:
// FAUP on all ?
  ASEQUAL
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASEQUAL"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASMULT
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASMULT"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASDIV
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASDIV"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASMOD
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASMOD"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASPLUS
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASPLUS"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASMINUS
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASMINUS"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASLEFTSHIFT
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASLEFTSHIFT"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASRIGHTSHIFT
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASRIGHTSHIFT"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ZRS_ASSIGN
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASUNSIGNEDRIGHTSHIFT"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASBITWISEAND
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASBITWISEAND"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASBITWISEEXOR
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASBITWISEEXOR"));
      javapSetPos2 (*$$, @@1, @@1);
    }
| ASBITWISEINCLOR
    {
      $$ = new TYPE_CPP_AssignOp ();
      $$ -> set_op (Quote (L"ASBITWISEINCLOR"));
      javapSetPos2 (*$$, @@1, @@1);
    }
;

expression:
  assignment_expression
    {}
;

constant_expression:
  expression
    {}
;

dims:
  osb_csb_s_1
    {}
;

osb_csb_s:
    {
      $$ = NULL;
    }
| osb_csb_s_1
    {}
;

osb_csb_s_1:
OSB CSB
    {
      $$ = new TYPE_CPP_ArrayNewDecl ();
      $$ -> set_newdecl (Nil ());
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@1, @@2);
    }
| osb_csb_s_1 OSB CSB
    {
      $$ = new TYPE_CPP_ArrayNewDecl ();
      $$ -> set_newdecl (*$1);
      delete $1;
      $$ -> set_expr (Nil ());
      javapSetPos2 (*$$, @@2, @@3);
  }
| osb_csb_s_1 OSB error
    {
      javaperror ("']' expected");
      return -1;
    }
;
%%
@}

/*
TYPE_CPP_PackageName* revertPN2PN (TYPE_CPP_PackageName* pn)
{
  TYPE_CPP_PackageName* rpn;
  if (pn -> Is (TAG_TYPE_CPP_SimplePackageName))
    return pn;

  TYPE_CPP_SimplePackageName spn;
  spn.set_id (((TYPE_CPP_QualifiedPackageName*) pn) -> get_id ());
  spn.set_cid (((TYPE_CPP_QualifiedPackageName*) pn) -> get_cid ());
  TYPE_CPP_PackageName last (spn);
  rpn = ((TYPE_CPP_QualifiedPackageName*) pn) -> get_pn ();
  while (rpn -> Is (TAG_TYPE_CPP_QualifiedPackageName))
  {
    TYPE_CPP_QualifiedPackageName* tmp = new TYPE_CPP_QualifiedPackageName ();
    tmp -> set_id (((TYPE_CPP_QualifiedPackageName*) rpn) -> get_id ());
    tmp -> set_cid (((TYPE_CPP_QualifiedPackageName*) rpn) -> get_cid ());
    tmp -> set_pn (*last);
    last = tmp;
    rpn = rpn -> get_pn ();
  }
  if (rpn -> Is (TAG_TYPE_CPP_SimplePackageName))
  {
    TYPE_CPP_QualifiedPackageName* tmp = new TYPE_CPP_QualifiedPackageName ();
    tmp -> set_id (((TYPE_CPP_SimplePackageName*) rpn) -> get_id ());
    tmp -> set_cid (((TYPE_CPP_SimplePackageName*) rpn) -> get_cid ());
    tmp -> set_pn (*last);
    last = tmp;
  }
  return last;
}
*/



Inherited from 'valueparser.y' :

@d Productions @{

static int ConvRealLit (const wchar_t *text, double &res)
{
  wchar_t* ptr;
  res = wcstod(text, &ptr);
  return (text != ptr && *ptr=='\0');
}
@}

Correction to rule: 'field_declaration' and 'local_variable_declaration'
Append a sequence of '[]' on the type specifier to all the variable declarators
Example:
  int[] i[] -> int i[][]

@d Productions @{

static SEQ<TYPE_CPP_InitDeclarator> move_SBs_2_InitDeclarator (
                         const TYPE_CPP_ArrayNewDecl & p_sbs,
                         const Sequence & p_s)
{
  SEQ<TYPE_CPP_InitDeclarator> l_vd;
  Generic newdecl = p_sbs;
  size_t len_p_s = p_s.Length();
  for (size_t idx = 1; idx <= len_p_s; idx++)
  {
    Generic l_g (p_s[idx]);
    if (l_g.Is(TAG_TYPE_CPP_InitDeclarator))
    {
      TYPE_CPP_InitDeclarator idecl (l_g);
      TYPE_CPP_Declarator l_decl (idecl.get_decl ());

      while (!newdecl.IsNil())
      {
        TYPE_CPP_ArrayNewDecl ndecl (newdecl);
        TYPE_CPP_ArrayDecl l_ad;
        l_ad.set_decl (l_decl);
        l_ad.set_expr (Nil ());
        l_ad.set_cid (ndecl.get_cid ());
        l_decl = l_ad;

        newdecl = ndecl.get_newdecl();
      }

      TYPE_CPP_InitDeclarator l_id;
      l_id.set_decl (l_decl);
      l_id.set_i (idecl.get_i ());
      l_id.set_cid (idecl.get_cid ());

      l_vd.ImpAppend (l_id);
    }
  }
  return l_vd;
}
@}

Correction to rule: ArgumentDeclaration

Append a sequence of '[]' on the type specifier to all the variable declarators
Example:
  (int[] i[] -> int i[][]

@d Productions @{

static
TYPE_CPP_ArrayDecl move_SBs_2_ArrayDecl (const TYPE_CPP_ArrayNewDecl & p_sbs,
                                         const Record & p_vdi)
{
  Generic l_decl = p_vdi;
  Generic newdecl = p_sbs;
  while (!newdecl.IsNil())
  {
    TYPE_CPP_ArrayNewDecl ndecl (newdecl);
    TYPE_CPP_ArrayDecl l_ad;
    l_ad.set_decl (l_decl);
    l_ad.set_expr (Nil ());
    l_ad.set_cid (ndecl.get_cid ());
    l_decl = l_ad;

    newdecl = ndecl.get_newdecl();
  }
  return l_decl;
}

static
Generic move_dims_2_ArrayAbsDecl (const TYPE_CPP_ArrayNewDecl & p_sbs)
{
  Generic l_decl = Nil();
  Generic newdecl = p_sbs;
  while (!newdecl.IsNil())
  {
    TYPE_CPP_ArrayNewDecl ndecl (newdecl);
    TYPE_CPP_ArrayAbsDecl l_aad;
    l_aad.set_ad (l_decl);
    l_aad.set_ce (Nil ());
    l_aad.set_cid (ndecl.get_cid ());
    l_decl = l_aad;
    newdecl = ndecl.get_newdecl();
  }
  return l_decl;
}

static void move_SBs_2_ArraySpecifiers (const TYPE_CPP_ArrayNewDecl & p_sbs, Sequence & p_s)
{
  Generic newdecl = p_sbs;
  while (!newdecl.IsNil())
  {
    TYPE_CPP_ArrayNewDecl ndecl (newdecl);
    TYPE_CPP_ArraySpecifier l_as;
    l_as.set_cid (ndecl.get_cid ());
    p_s.ImpAppend (l_as);
    newdecl = ndecl.get_newdecl();
  }
}

static void set_file_and_dir_name (const wstring & fname)
{
//JR split file from directory
  filename = Sequence (fname);
  dirname = Nil();
}

static void Report(const char* msg)
{
  errors = true;

  Int fid (javapFileId);
  Int line (JAVASCANNER::javasGetLineCount());
  Int col (JAVASCANNER::javasGetColumnCount());

  Sequence s;
  s.ImpAppend(SEQ<Char>(TBWSTR::string2wstring(msg)));
  
  TYPE_ProjectTypes_ErrMsg rc;
  rc.Init(fid, line, col, s);
  javaErrs->vdm_AddMsg(rc);
}

void javaperror (const char * msg)
{
  Report(msg);
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int yyparse();

//extern void javasSetTokenList (TokenList * );

void SetInput(ifstream& inp)
{
  JAVASCANNER::javasLEXINPUT.ReadFrom(inp);
}

bool java_parse (wchar_t* fname,
                 bool POS_WANTED,           // include position information
                 vdm_Errors* _errs,         // List of errors generated
                 TYPE_CPP_File& res,             // the resulting AST
                 ContextInfo& ci,
                 bool reset_ci,
                 const TYPE_CI_FileId & fid)     // the file id
{
   set_file_and_dir_name (wstring(fname));
   javapFileId  = fid;
   javap_context_info_p = &ci;

   ci.UseContextTab (javapFileId, reset_ci);

   res = TYPE_CPP_File();
   javafile = &res;
#if YYDEBUG
   yydebug = 1;
#endif

   javaErrs = _errs;
   errors = false;
   JAVASCANNER::javasInitCounts ();
   JAVASCANNER::javasInitLex ();
   JAVASCANNER::javasSetTokenList (NULL);  // set the token list
   int result = 0;

   if (result == 0)
   {
     JAVASCANNER::javasSetTokenList (&(ci.GetTokenList(javapFileId)));  // set the token list
     JAVASCANNER::javasInitLex ();
     JAVASCANNER::javasInitCounts ();
     try
     {
       JAVASCANNER::javasLEXINPUT.reread ();
       result = javapparse ();
     }
     catch (TB_Exception & e)
     {
       switch (e.GetExType())
       {
         default:
          // It must not happen
          vdm_err << L"Internal Error in javaparse\n";
          break;
       }
     }
   }
   JAVASCANNER::javasSetTokenList (NULL); // reset the tokenlist
   JAVASCANNER::javasFreeLexStrings ();
   return (result || errors);
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

@}

@d This file is generated @{
/* This file is generated from javaparse.w. Do not modify! */
@}
