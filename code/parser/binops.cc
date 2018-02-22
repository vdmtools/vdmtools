/*
 *  WHAT
 *     Module: Byacc parser recovery.
 *     Interface glue between .y file and recover.cc,h
 *
 *  IDENTIFICATION
 *     $Id: binops.cc,v 1.54 2006/03/15 07:47:45 vdmtools Exp $
 *  AUTHOR
 *     ETN
 *  COPYRIGHT
 *     (C) Kyushu University
 */

#include "binops.h"
#include "scanner.h"
#include "tb_wstring.h"

#ifdef VDMSL
#include "parssl.tab.h"
#endif //VDMSL
#ifdef VICE
#include "parsrt.tab.h"
#else
#ifdef VDMPP
#include "parspp.tab.h"
#endif //VDMPP
#endif //VICE

#include "projectval.h"
#include "tbutils.h"
#include "recover.h"
#include "stdlib.h" // for exit
#include <vector>

static const int TG_None  = 0;
static const int TG_Other = 1<<0;
static const int TG_BinOp = 1<<1;
static const int TG_Type  = 1<<2;
static const int TG_Expr  = 1<<3;
static const int TG_Pat   = 1<<4;
static const int TG_Stmt  = 1<<5;

static const int TG_skip_bin_op     = 0;
static const int TG_skip_expr_unop  = 0;
static const int TG_skip_expr       = 0;
static const int TG_skip_type       = 0;
static const int TG_valid           = 1;

VDM_Recover vdm_rec;

void VDM_Recover::Report(int line, int col, const type_cLL & msgs)
{
  TYPE_ProjectTypes_ErrMsg rc;
  rc.Init(Int(this->FileId), Int(line), Int(col), msgs);
  this->Errs->vdm_AddMsg(rc);
}

void ParserRecover::InitRepairTokens()
{
  AddToken(0,                  L"<end of file>",       TG_valid,       0,        TTF_NoQuote);
  AddToken('@',                L"@",                   TG_valid,       0,        TTF_None);
  AddToken(';',                L";",                   TG_valid,       0,        TTF_None);
  AddToken(')',                L")",                   TG_valid,       0,        TTF_None);
  AddToken('*',                L"*",                   TG_valid,       TG_BinOp, TTF_None);
  AddToken(',',                L",",                   TG_valid,       0,        TTF_None);
  AddToken(':',                L":",                   TG_valid,       0,        TTF_None);
  AddToken(LEX_DOTHASH,        L".#",                  TG_valid,       0,        TTF_None);
#if VDMPP
  AddToken('!',                L"!",                   TG_valid,       0,        TTF_None);
//  AddToken(LEX_APPROX,         L"=~",                  TG_valid,       TG_BinOp, TTF_None);
  AddToken(LEX_ACT,            L"#act",                TG_valid,       0,        TTF_None);
  AddToken(LEX_ACTIVE,         L"#active",             TG_valid,       0,        TTF_None);
  AddToken(LEX_FIN,            L"#fin",                TG_valid,       0,        TTF_None);
  AddToken(LEX_REQ,            L"#req",                TG_valid,       0,        TTF_None);
  AddToken(LEX_WAITING,        L"#waiting",            TG_valid,       0,        TTF_None);
  AddToken(LEX_ATOMIC,         L"atomic",              TG_valid,       0,        TTF_None);
  AddToken(LEX_CLASS,          L"class",               TG_valid,       0,        TTF_None);
  AddToken(LEX_DLCLASS,        L"dlclass",             TG_valid,       0,        TTF_None);
  AddToken(LEX_INSTANCE,       L"instance",            TG_valid,       0,        TTF_None);
  AddToken(LEX_IS,             L"is",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_ISOFBASECLASS,  L"isofbaseclass",       TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_ISOFCLASS,      L"isofclass",           TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_MUTEX,          L"mutex",               TG_valid,       0,        TTF_None);
  AddToken(LEX_NEW,            L"new",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_PER,            L"per",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_PERIODIC,       L"periodic",            TG_valid,       0,        TTF_None);
  AddToken(LEX_PROTECTED,      L"protected",           TG_valid,       0,        TTF_None);
  AddToken(LEX_PRIVATE,        L"private",             TG_valid,       0,        TTF_None);
  AddToken(LEX_PUBLIC,         L"public",              TG_valid,       0,        TTF_None);
  AddToken(LEX_RESPONSIBILITY, L"responsibility",      TG_valid,       0,        TTF_None);
  AddToken(LEX_SAMEBASECLASS,  L"samebaseclass",       TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_SAMECLASS,      L"sameclass",           TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_SELF,           L"self",                TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_SPECIFIED,      L"specified",           TG_valid,       0,        TTF_None);
  AddToken(LEX_START,          L"start",               TG_valid,       0,        TTF_None);
  AddToken(LEX_STARTLIST,      L"start_list",          TG_valid,       0,        TTF_None);
  AddToken(LEX_STOP,           L"stop",                TG_valid,       0,        TTF_None);
  AddToken(LEX_STOPLIST,       L"stop_list",           TG_valid,       0,        TTF_None);
  AddToken(LEX_STATIC,         L"static",              TG_valid,       0,        TTF_None);
  AddToken(LEX_SUBCLASS,       L"subclass",            TG_valid,       0,        TTF_None);
  AddToken(LEX_SYNC,           L"sync",                TG_valid,       0,        TTF_None);
  AddToken(LEX_THREAD,         L"thread",              TG_valid,       0,        TTF_None);
  AddToken(LEX_THREADID,       L"threadid",            TG_valid,       0,        TTF_None);
#ifdef VICE
  AddToken(LEX_CURTIME,        L"time",                TG_valid,       0,        TTF_None);
#endif // VICE
  AddToken(LEX_VARIABLES,      L"variables",           TG_valid,       0,        TTF_None);
  AddToken(LEX_YET,            L"yet",                 TG_valid,       0,        TTF_None);
#endif // VDMPP
  AddToken('.',                L".",                   TG_valid,       0,        TTF_None);
  AddToken(']',                L"]",                   TG_valid,       0,        TTF_None);
  AddToken('|',                L"|",                   TG_valid,       0,        TTF_None);
  AddToken('}',                L"}",                   TG_valid,       0,        TTF_None);
// Use CONCATENATE below. Do not insert this
  //AddToken('^',                L"^",                   false,          TG_BinOp , TTF_None);
  AddToken('(',                L"(",                   TG_valid,       TG_Expr|TG_Type|TG_Pat|TG_Stmt, TTF_None);
  AddToken('[',                L"[",                   TG_valid,       TG_Expr|TG_Type|TG_Pat,         TTF_None);
  AddToken('{',                L"{",                   TG_valid,       TG_Expr|TG_Pat,                 TTF_None);
  AddToken('+',                L"+",                   TG_valid,       TG_BinOp|TG_Expr,               TTF_None);
  AddToken('-',                L"-",                   TG_valid,       TG_Expr|TG_BinOp|TG_Pat,        TTF_None);
  AddToken('/',                L"/",                   TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_SET_MINUS,      L"\\",                  TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_MAP_DOMAIN_RESTRICT_TO, L"<:",          TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_MAP_DOMAIN_RESTRICT_BY, L"<-:",         TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_MAP_RANGE_RESTRICT_TO, L":>",           TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_MAP_RANGE_RESTRICT_BY, L":->",          TG_skip_bin_op, TG_BinOp, TTF_None);
#if VDMSL
  AddToken(LEX_EXP_OR_ITERATE, L"**",                  TG_skip_bin_op, TG_BinOp, TTF_None);
#endif // VDMSL
#if VDMPP
  AddToken(LEX_EXP_OR_ITERATE, L"**",                  TG_valid,       TG_BinOp, TTF_None);
#endif // VDMPP
  AddToken(LEX_LOGICAL_EQUIVALENCE, L"<=>",            TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_EQUAL,          L"=",                   TG_valid,       TG_BinOp, TTF_None);
  AddToken(LEX_GREATER_THAN_OR_EQUAL, L">=",           TG_skip_bin_op, TG_BinOp, TTF_None);
#if VDMSL
  AddToken(LEX_GREATER_THAN,   L">",                   TG_skip_bin_op, TG_BinOp, TTF_None);
#endif
#if VDMPP
  AddToken(LEX_GREATER_THAN,   L">",                   TG_valid,       TG_BinOp, TTF_None);
#endif
  AddToken(LEX_LESS_THAN_OR_EQUAL, L"<=",              TG_skip_bin_op, TG_BinOp, TTF_None);
#if VDMSL
  AddToken(LEX_LESS_THAN,      L"<",                   TG_skip_bin_op, TG_BinOp, TTF_None);
#endif
#if VDMPP
  AddToken(LEX_LESS_THAN,      L"<",                   TG_valid,       TG_BinOp, TTF_None);
#endif
  AddToken(LEX_NOT_EQUAL,      L"<>",                  TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_NOT_IN_SET,     L"not in set",          TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_ABS,            L"abs",                 TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_ALL,            L"all",                 TG_valid,      0,        TTF_None);
  AddToken(LEX_ALWAYS,         L"always",              TG_valid,      TG_Stmt,  TTF_None);
  AddToken(LEX_AND,            L"and",                 TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_ARROW,          L"->",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_ASSIGN,         L":=",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_BAR_ARROW,      L"|->",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_BE,             L"be",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_BOOL,           L"bool",                TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_BY,             L"by",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_CARD,           L"card",                TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_CASES,          L"cases",               TG_valid,       TG_Expr|TG_Stmt, TTF_None);
  AddToken(LEX_CHAR,           L"wchar_t",             TG_skip_type,   TG_Type, TTF_None);
  AddToken(LEX_COMP,           L"comp",                TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_COMPOSE,        L"compose",             TG_valid,       TG_Type,  TTF_None);
  AddToken(LEX_CONC,           L"conc",                TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_DCL,            L"dcl",                 TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_DEF,            L"def",                 TG_valid,       TG_Expr|TG_Stmt, TTF_None);
#ifdef VDMSL
  AddToken(LEX_DEFINITIONS,    L"definitions",         TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_DINTER,         L"dinter",              TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_ARITHMETIC_INTEGER_DIVISION, L"div",    TG_skip_bin_op,	TG_BinOp , TTF_None);
#ifdef VDMSL
  AddToken(LEX_DLMODULE,       L"dlmodule",            TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_USELIB,         L"uselib",              TG_valid,       0,        TTF_None);
  AddToken(LEX_DO,             L"do",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_DONTCARE,       L":-",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_DOM,            L"dom",                 TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_DOUBLE_COLON,   L"::",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_DUNION,         L"dunion",              TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_ELEMS,          L"elems",               TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_ELSE,           L"else",                TG_valid,       0,        TTF_None);
  AddToken(LEX_ELSEIF,         L"elseif",              TG_valid,       0,        TTF_None);
  AddToken(LEX_END,            L"end",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_EQ,             L"eq",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_ERROR,          L"error",               TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_ERRS,           L"errs",                TG_valid,       0,        TTF_None);
  AddToken(LEX_EXISTS,         L"exists",              TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_EXISTS1,        L"exists1",             TG_skip_expr,   TG_Expr,  TTF_None);
  AddToken(LEX_EXIT,           L"exit",                TG_valid,       TG_Stmt,  TTF_None);
#ifdef VDMSL
  AddToken(LEX_EXPORTS,        L"exports",             TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_EXT,            L"ext",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_FLOOR,          L"floor",               TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_FOR,            L"for",                 TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_FORALL,         L"forall",              TG_valid,       TG_Expr,  TTF_None);
#ifdef VDMSL
  AddToken(LEX_FROM,           L"from",                TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_FUNCTIONS,      L"functions",           TG_valid,       0,        TTF_None);
  AddToken(LEX_HD,             L"hd",                  TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_HOOK,           L"~",                   TG_valid,       0,        TTF_None);
  AddToken(LEX_IF,             L"if",                  TG_valid,       TG_Expr|TG_Stmt, TTF_None);
  AddToken(LEX_IMPLY,          L"=>",                  TG_skip_bin_op, TG_BinOp, TTF_None);
#ifdef VDMSL
  AddToken(LEX_IMPORTS,        L"imports",             TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_IN,             L"in",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_INDS,           L"inds",                TG_skip_expr_unop, TG_Expr, TTF_None);
#ifdef VDMSL
  AddToken(LEX_INIT,           L"init",                TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_INMAP,          L"inmap",               TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_INMAP1,         L"inmap1",               TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_INT,            L"int",                 TG_valid,       TG_Type,  TTF_None);
  AddToken(LEX_INV,            L"inv",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_INVERSE,        L"inverse",             TG_skip_bin_op, TG_Expr,  TTF_None);
  AddToken(LEX_IN_SET,         L"in set",              TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_IN_SEQ,         L"in seq",              TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_IOTA,           L"iota",                TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_IS_,            L"is_",                 TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_IS_DEFINED_AS,  L"==",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_LAMBDA,         L"lambda",              TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_LEN,            L"len",                 TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_LET,            L"let",                 TG_valid,       TG_Expr|TG_Stmt, TTF_None);
  AddToken(LEX_MAP,            L"map",                 TG_valid,       TG_Type,  TTF_None);
  AddToken(LEX_MAP1,           L"map1",                 TG_valid,       TG_Type,  TTF_None);
  AddToken(LEX_MEASURE,        L"mease",               TG_valid,       0,        TTF_None);
  AddToken(LEX_DMERGE,         L"merge",               TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_MAP_MERGE,      L"munion",              TG_skip_bin_op, TG_Pat|TG_BinOp, TTF_None);
  AddToken(LEX_MK_,            L"mk_",                 TG_valid,       TG_Pat|TG_Expr, TTF_None);
  AddToken(LEX_MOD,            L"mod",                 TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_MODIFY_BY,      L"++",                  TG_skip_bin_op, TG_BinOp, TTF_None);
#ifdef VDMSL
  AddToken(LEX_MODULE,         L"module",              TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_MU,             L"mu",                  TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_NARROW_,        L"narrow_",             TG_valid,       TG_Expr,  TTF_None);
  AddToken(LEX_NAT,            L"nat",                 TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_NATONE,         L"nat1",                TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_NIL,            L"nil",                 0,              TG_Pat|TG_Expr, TTF_None);
  AddToken(LEX_NONDET,         L"||",                  TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_NOT,            L"not",                 TG_skip_expr_unop, TG_Expr, TTF_None);
#ifdef VDMPP
  AddToken(LEX_OBJ_,           L"obj_",                TG_valid,       TG_Pat,   TTF_None);
#endif // VDMPP
  AddToken(LEX_OF,             L"of",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_OPERATIONS,     L"operations",          TG_valid,       0,        TTF_None);
  AddToken(LEX_OPERATION_ARROW, L"==>",                TG_valid,       0,        TTF_None);
  AddToken(LEX_OR,             L"or",                  TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_ORD,            L"ord",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_OTHERS,         L"others",              TG_valid,       0,        TTF_None);
  AddToken(LEX_TARROW,         L"+>",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_POST,           L"post",                TG_valid,       0,        TTF_None);
  AddToken(LEX_POWER,          L"power",               TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_PRE,            L"pre",                 TG_valid,       0,        TTF_None);
  AddToken(LEX_PRECONDAPPLY,   L"pre_",                TG_valid,       0,        TTF_None);
  AddToken(LEX_PRIME,          L"`",                   TG_valid,       0,        TTF_None);
  AddToken(LEX_PROPER_SUBSET,  L"psubset",             TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_PURE,           L"pure",                TG_valid,       0,        TTF_None);
  AddToken(LEX_RAISED_DOT,     L"&",                   TG_valid,       0,        TTF_None);
  AddToken(LEX_RANGE_OVER,     L", ... ,",             TG_valid,       0,        TTF_None);
  AddToken(LEX_RAT,            L"rat",                 TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_RD,             L"rd",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_REAL,           L"real",                TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_REM,            L"rem",                 TG_skip_bin_op, TG_BinOp, TTF_None);
#ifdef VDMSL
  AddToken(LEX_RENAMED,        L"renamed",             TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_RETURN,         L"return",              TG_valid,       TG_Stmt,  TTF_None);
  //AddToken(LEX_REVERSE,        L"reverse",             TG_valid,           TG_None,  TTF_None); // UnOp
  AddToken(LEX_REVERSE,        L"reverse",             TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_RNG,            L"rng",                 TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_SEQ1,           L"seq1",                TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_SEQUENCE_CONCATENATE, L"^",             TG_skip_bin_op, TG_Pat|TG_BinOp, TTF_None);
  AddToken(LEX_SEQ,            L"seq",                 TG_valid,       TG_Type,  TTF_None);
  AddToken(LEX_SET_INTERSECTION, L"inter",             TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_SET,            L"set",                 TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_SET1,           L"set1",                 TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_SET_UNION,      L"union",               TG_skip_bin_op, TG_Pat|TG_BinOp, TTF_None);
  AddToken(LEX_SKIP,           L"skip",                TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_ST,             L"st",                  TG_valid,       0,        TTF_None);
#ifdef VDMSL
  AddToken(LEX_STATE,          L"state",               TG_valid,       0,        TTF_None);
  AddToken(LEX_STRUCT,         L"struct",              TG_valid,       0,        TTF_None);
#endif //VDMSL
  AddToken(LEX_SUBSET,         L"subset",              TG_skip_bin_op, TG_BinOp, TTF_None);
  AddToken(LEX_THEN,           L"then",                TG_valid,       0,        TTF_None);
  AddToken(LEX_TIXE,           L"tixe",                TG_valid,       TG_Stmt,  TTF_None);
  AddToken( LEX_TL,            L"tl",                  TG_skip_expr_unop, TG_Expr, TTF_None);
  AddToken(LEX_TO,             L"to",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_TOKEN,          L"token",               TG_skip_type,   TG_Type,  TTF_None);
  AddToken(LEX_TRACES,         L"traces",              TG_valid,       0,        TTF_None);
  AddToken(LEX_TRAP,           L"trap",                TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_TYPES,          L"types",               TG_valid,       0,        TTF_None);
  AddToken(LEX_UNDEFINED,      L"undefined",           0,              TG_Expr,  TTF_None);
  AddToken(LEX_VALUES,         L"values",              TG_valid,       0,        TTF_None);
  AddToken(LEX_WHILE,          L"while",               TG_valid,       TG_Stmt,  TTF_None);
  AddToken(LEX_WITH,           L"with",                TG_valid,       0,        TTF_None);
  AddToken(LEX_WR,             L"wr",                  TG_valid,       0,        TTF_None);
  AddToken(LEX_LAST_RESULT,    L"$$",                  0,              0,        TTF_None);
  AddToken(LEX_identifier ,    L"<name>",              TG_valid,       TG_Expr|TG_Type|TG_Pat|TG_Stmt , TTF_NoQuote);
  AddToken(LEX_dollar_identifier, L"<$name>",          0,              TG_Expr|TG_Type|TG_Pat|TG_Stmt , TTF_NoQuote);
  AddToken(LEX_char_lit,       L"'wchar_t'",           0,              TG_Pat|TG_Expr, TTF_NoQuote);
  AddToken(LEX_bool_true,      L"<true>",              0,              TG_Pat|TG_Expr, TTF_NoQuote);
  AddToken(LEX_bool_false,     L"<false>",             0,              TG_Pat|TG_Expr, TTF_NoQuote);
  AddToken(LEX_quote_lit,      L"<quote>",             TG_valid,       TG_Pat|TG_Expr|TG_Type, TTF_NoQuote);
  AddToken(LEX_text_lit,       L"\"text\"",            0,              TG_Pat|TG_Expr, TTF_NoQuote);
  AddToken(LEX_real_lit,       L"<number>",            0,              TG_Pat|TG_Expr, TTF_NoQuote);
  AddToken(LEX_int_lit,        L"<number>",            0,              TG_Pat|TG_Expr, TTF_NoQuote);
  AddToken(LEX_TEXBREAK,       L"\\begin/end{vdm_al}", 0,              0,        TTF_None);
  AddToken(LEX_FILE_MACRO,     L"__FILE__",            0,              0,        TTF_None);
  AddToken(LEX_LINE_MACRO,     L"__LINE__",            0,              0,        TTF_None);
  AddToken(LEX_COLUMN_MACRO,   L"__COLUMN__",          0,              0,        TTF_None);
  AddToken(LEX_CLMOD_MACRO,    L"__CLMOD__",           0,              0,        TTF_None);
  AddToken(LEX_FNOP_MACRO,     L"__FNOP__",            0,              0,        TTF_None);
  AddToken(LEX_COMMENT,        L"",                    0,              0,        TTF_None);
  // These exist just to prevent that they are output to recover.log
  // when RECOVERDEBUG==DEBUG_CHECK_TOKENLIST
  // They are never tried due to the 3rd 'false' parameter.
  AddToken(DUMMY_RELATION,     L"",                    0,              0,        TTF_None);
  AddToken(LEX_DUMMY_PRECEDENCE_SEQ1_OF, L"",          0,              0,        TTF_None);
  AddToken(LEX_DUMMY_PRECEDENCE_SEQ_OF, L"",           0,              0,        TTF_None);
  AddToken(LEX_DUMMY_PRECEDENCE_SET_OF, L"",           0,              0,        TTF_None);
}

void ParserRecover::AddToken(int token, const wchar_t * str, int vr, long grp, long flag)
{
  pair<TokenTableType::iterator, bool> p (
    this->repair_tokens.insert(TokenTableType::value_type(token, TokenTableEntry(str, vr, grp, flag))));
  if (!p.second)
  {
     wcout << L"Duplicate entry [" << token << L"] in ReservedWords: " << str << endl;
     exit(1);
  }
}

void VDM_Recover::InitTokenGroupArray()
{
  AddTokenGroup(TG_None,    L"",               TG_None);
  AddTokenGroup(TG_Other,   L"<<other>>",      TG_None);
  AddTokenGroup(TG_BinOp,   L"<<binary op>>",  TG_None);
  AddTokenGroup(TG_Type,    L"<<type>>",       TG_None);
  AddTokenGroup(TG_Expr,    L"<<expr>>",       TG_Pat);
  AddTokenGroup(TG_Pat,     L"<<pattern>>",    TG_None);
  AddTokenGroup(TG_Stmt,    L"<<stmt>>",       TG_None);
}

void VDM_Recover::AddTokenGroup(long grp, const wchar_t * name, long over)
{
  this->tokenGroupArray.push_back(TokenGroup(grp, name, over));
}

wstring VDM_Recover::GroupExpected(const TokenSeq & expected, TokenTableType & RepToks)
{
  // Count how many in each group.
  for (TokenGroupArray::iterator itr(this->tokenGroupArray.begin()); itr != this->tokenGroupArray.end(); itr++)
  {
    itr->count = 0;
    itr->actCount = 0;
  }

  for (TokenTableType::const_iterator iter(this->repair_tokens.begin()); iter != this->repair_tokens.end(); iter++)
  {
    if (!iter->second.validRepair)
      continue;
    long f = iter->second.lexTokenGroups;
    for (TokenGroupArray::iterator itr(this->tokenGroupArray.begin());
         itr != this->tokenGroupArray.end() && f != 0; itr++)
    {
      if (f & itr->tGroup)
      {
        itr->count++;
        f &= ~ itr->tGroup;
      } 
    }
  }

//  for (j = 0; LexTokenGroups[j].tGroup >= 0; j++) {
//    LexTokenGroups[j].actCount = 0;
//  }

  long doGroupFlag = 0;
  for (TokenSeq::const_iterator titr = expected.begin(); titr != expected.end(); titr++)
  {
    //int tok = titr->token;
    //TokenTableType::iterator mitr (this->repair_tokens.find(tok));
    TokenTableType::iterator mitr (this->repair_tokens.find(titr->token));
    int flag = mitr->second.lexTokenGroups;
    if (flag != 0)
    {
      for (TokenGroupArray::iterator itr(this->tokenGroupArray.begin()); itr != this->tokenGroupArray.end(); itr++)
      {
        if (itr->tGroup & flag)
        {
          if (++(itr->actCount) == itr->count)
            doGroupFlag |= itr->tGroup;
        }
      }
    }
  }

  for (TokenGroupArray::const_iterator itr(this->tokenGroupArray.begin()); itr != this->tokenGroupArray.end(); itr++)
  {
    if (itr->tGroup & doGroupFlag && itr->override)
      doGroupFlag &= ~ itr->override;
  }

  long outFlag = 0;
  vector<wstring> slist;
  TokenSeq rest;
  for (TokenSeq::const_iterator titr = expected.begin(); titr != expected.end(); titr++)
  {
    //int tok = titr->token;
    //TokenTableType::iterator mitr (this->repair_tokens.find(tok));
    TokenTableType::iterator mitr (this->repair_tokens.find(titr->token));
    int flag = mitr->second.lexTokenGroups;
    if ((flag & doGroupFlag) == 0) {
      rest.push_back(*titr);
    } else {
      flag &= doGroupFlag & ~outFlag;
      outFlag |= flag;
      for (TokenGroupArray::iterator itr(this->tokenGroupArray.begin()); itr != this->tokenGroupArray.end(); itr++)
      {
        if (itr->tGroup & flag) {
          slist.push_back(itr->groupName);
          flag &= ~ itr->tGroup;
        }
      }
    }
  }

  //for (size_t i6 = 0; i6 < rest.size(); i6++)
  //  slist.push_back(TextifyToken(rest[i6], true));
  for (TokenSeq::const_iterator titr = rest.begin(); titr != rest.end(); titr++)
    slist.push_back(TextifyToken(*titr, true));

  wstring Res;
  int slist_size = slist.size();
  for (int i7 = 0; i7 < slist_size; i7++) {
    if ((i7 > 0) && (i7 + 1 == slist_size)) {
      Res += L" or ";
    } else if (i7 > 0) {
      Res += L", ";
    }
    Res += slist[i7];
  }
  return Res;
}

int VDM_Recover::Lex()
{
  YYSTYPE yylval;
  return yylex(&yylval, &this->pos);
}

wstring VDM_Recover::GetString(int token)
{
  wstring res;
  switch (token) {
    case 0: {                     // End of File/Line/Expression.
      if (!this->EofText.empty())
        return this->EofText;
      else
        return ParserRecover::GetString(token);
    }
    default:
      return ParserRecover::GetString(token);
  }
}

wstring VDM_Recover::GetLexText(int token)
{
  return BINOPS::getLexText(token, TBWSTR::string2wstring(SCANNER::yytextstring));
}

wstring BINOPS::getLexText( int id, const wstring & text )
{
  wstring res;
  switch (id) {
    case LEX_text_lit:
      res = L"\"";
      res += text;
      res += L"\"";
      break;
    case LEX_char_lit:
      res = L"'";
      res += text;
      res += L"'";
      break;
    case LEX_dollar_identifier:
      res = text;
      break;
    case LEX_TEXBREAK:
      res = L"\\begin/end{vdm_al}";
      break;
    default:
      res = text;
      break;
  }
  return res;
}

wstring BINOPS::id2str( int id, const wstring & text )
{
  wstring res;
  if( id < 256 )
  {
    wchar_t a = id;
    res = a;
  }
  else
    res = text;

  if (res.empty())
    res = vdm_rec.GetString(id);

  return res;
}

Token_Type BINOPS::id2gtype( int id )
{
  Token_Type type = TT_DEFAULT;
  switch( id )
  {
    case LEX_COMMENT:
      type = TT_COMMENT;
      break;

#ifdef VDMSL
    case LEX_DEFINITIONS:
#endif // VDMSL
#ifdef VDMPP
    case LEX_CLASS:
    case LEX_DLCLASS:
#ifdef VICE
    case LEX_SYSTEM:
#endif // VICE
#endif // VDMPP
#ifdef VDMSL
    case LEX_MODULE:
    case LEX_DLMODULE:
#endif // VDMSL
    case LEX_USELIB:
    case LEX_TYPES:
    case LEX_VALUES:
    case LEX_FUNCTIONS:
    case LEX_OPERATIONS:
#ifdef VDMPP
    case LEX_INSTANCE:
    case LEX_VARIABLES:
    case LEX_SYNC:
    case LEX_THREAD:
#endif // VDMPP
    case LEX_TRACES:
      type = TT_DEF;
      break;

    case LEX_IS:
    case LEX_YET:
    case LEX_SPECIFIED:
#ifdef VDMPP
    case LEX_ACT:
    case LEX_ACTIVE:
    case LEX_FIN:
    case LEX_REQ:
    case LEX_WAITING:
    case LEX_ATOMIC:
    case LEX_ISOFBASECLASS:
    case LEX_ISOFCLASS:
    case LEX_MUTEX:
    case LEX_NEW:
    case LEX_PER:
    case LEX_PERIODIC:
    case LEX_PROTECTED:
    case LEX_PRIVATE:
    case LEX_PUBLIC:
    case LEX_RESPONSIBILITY:
    case LEX_SAMEBASECLASS:
    case LEX_SAMECLASS:
    case LEX_SELF:
    case LEX_START:
    case LEX_STARTLIST:
    case LEX_STATIC:
    case LEX_SUBCLASS:
    case LEX_THREADID:
#ifdef VICE
    case LEX_ASYNC:
    case LEX_DURATION:
    case LEX_CURTIME:
    case LEX_CYCLES:
#endif // VICE
#endif // VDMPP
    case LEX_NOT_IN_SET:
    case LEX_ABS:
    case LEX_ALL:
    case LEX_ALWAYS:
    case LEX_AND:
    case LEX_BE:
    case LEX_BOOL:
    case LEX_BY:
    case LEX_CARD:
    case LEX_CASES:
    case LEX_CHAR:
    case LEX_COMP:
    case LEX_COMPOSE:
    case LEX_CONC:
    case LEX_DCL:
    case LEX_DEF:
    case LEX_DINTER:
    case LEX_ARITHMETIC_INTEGER_DIVISION:
    case LEX_DO:
    case LEX_DOM:
    case LEX_DUNION:
    case LEX_ELEMS:
    case LEX_ELSE:
    case LEX_ELSEIF:
    case LEX_END:
    case LEX_ERROR:
    case LEX_ERRS:
    case LEX_EXISTS:
    case LEX_EXISTS1:
    case LEX_EXIT:
#ifdef VDMSL
    case LEX_EXPORTS:
#endif // VDMSL
    case LEX_EXT:
    case LEX_FLOOR:
    case LEX_FOR:
    case LEX_FORALL:
#ifdef VDMSL
    case LEX_FROM:
#endif // VDMSL
    case LEX_HD:
    case LEX_IF:
#ifdef VDMSL
    case LEX_IMPORTS:
#endif // VDMSL
    case LEX_IN:
    case LEX_INDS:
#ifdef VDMSL
    case LEX_INIT:
#endif // VDMSL
    case LEX_INMAP:
    case LEX_INMAP1:
    case LEX_INT:
    case LEX_INV:
    case LEX_INVERSE:
    case LEX_IN_SET:
    case LEX_IN_SEQ:
    case LEX_IOTA:
    case LEX_IS_:
    case LEX_LAMBDA:
    case LEX_LEN:
    case LEX_LET:
    case LEX_MAP:
    case LEX_MAP1:
    case LEX_DMERGE:
    case LEX_MAP_MERGE:
    case LEX_MEASURE:
    case LEX_MK_:
    case LEX_MOD:
    case LEX_MU:
    case LEX_NARROW_:
    case LEX_NAT:
    case LEX_NATONE:
    case LEX_NIL:
    case LEX_NOT:
#ifdef VDMPP
    case LEX_OBJ_:
#endif // VDMPP
    case LEX_OF:
    case LEX_OR:
    case LEX_OTHERS:
    case LEX_TARROW:
    case LEX_POST:
    case LEX_POWER:
    case LEX_PRE:
    case LEX_PRECONDAPPLY:
    case LEX_PROPER_SUBSET:
    case LEX_PURE:
    case LEX_RAT:
    case LEX_RD:
    case LEX_REAL:
    case LEX_REM:
#ifdef VDMSL
    case LEX_RENAMED:
#endif // VDMSL
    case LEX_RETURN:
    case LEX_REVERSE:
    case LEX_RNG:
    case LEX_SEQ1:
    case LEX_SEQ:
    case LEX_SET_INTERSECTION:
    case LEX_SET:
    case LEX_SET1:
    case LEX_SET_UNION:
    case LEX_SKIP:
    case LEX_ST:
#ifdef VDMSL
    case LEX_STATE:
    case LEX_STRUCT:
#endif // VDMSL
    case LEX_SUBSET:
    case LEX_THEN:
    case LEX_TIXE:
    case LEX_TL:
    case LEX_TO:
    case LEX_TOKEN:
    case LEX_TRAP:
    case LEX_UNDEFINED:
    case LEX_WHILE:
    case LEX_WITH:
    case LEX_WR:
      type = TT_RESERVE;
      break;

    case ';':
    case '*':
    case ',':
    case ':':
    case LEX_DOTHASH:
#ifdef VDMPP
    case '!':
#endif // VDMPP
    case '.':
    case '|':
    case '^':
    case '+':
    case '-':
    case '/':
    case LEX_SET_MINUS:
    case LEX_MAP_DOMAIN_RESTRICT_TO:
    case LEX_MAP_DOMAIN_RESTRICT_BY:
    case LEX_MAP_RANGE_RESTRICT_TO:
    case LEX_MAP_RANGE_RESTRICT_BY:
    case LEX_EXP_OR_ITERATE:
    case LEX_LOGICAL_EQUIVALENCE:
    case LEX_EQUAL:
    case LEX_GREATER_THAN_OR_EQUAL:
    case LEX_GREATER_THAN:
    case LEX_LESS_THAN_OR_EQUAL:
    case LEX_LESS_THAN:
    case LEX_NOT_EQUAL:
    case LEX_ARROW:
    case LEX_ASSIGN:
    case LEX_BAR_ARROW:
    case LEX_DONTCARE:
    case LEX_DOUBLE_COLON:
    case LEX_HOOK:
    case LEX_IMPLY:
    case LEX_IS_DEFINED_AS:
    case LEX_MODIFY_BY:
    case LEX_NONDET:
    case LEX_OPERATION_ARROW:
    case LEX_PRIME:
    case LEX_RAISED_DOT:
    case LEX_RANGE_OVER:
    case LEX_SEQUENCE_CONCATENATE:
    case LEX_LAST_RESULT:
      type = TT_OPERATOR;
      break;

    case ')':
    case ']':
    case '}':
    case '(':
    case '[':
    case '{':
      type = TT_PARENTHESIS;
      break;

    case '@':
      type = TT_POLY;
      break;

    case LEX_dollar_identifier:
    case LEX_bool_true:
    case LEX_bool_false:
    case LEX_real_lit:
    case LEX_int_lit:
      type = TT_LITERAL;
      break;

    case LEX_char_lit:
    case LEX_quote_lit:
    case LEX_text_lit:
      type = TT_CHARLITERAL;
      break;

    case LEX_TEXBREAK:
      type = TT_TEXBREAK;
      break;

    case LEX_identifier:
      type = TT_IDENT;
      break;

    default:
      type = TT_DEFAULT;
      break;
  }
  return type;
}

bool BINOPS::isIdentifier( int id )
{
  return (id == LEX_identifier);
}

bool BINOPS::isClassOrModule( int id )
{
#ifdef VDMSL
  return ((id == LEX_MODULE) || (id == LEX_DLMODULE));
#endif // VDMSL
#ifdef VDMPP
  return (id == LEX_CLASS);
#endif // VDMPP
}
