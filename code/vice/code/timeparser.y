/***
*  * WHAT
*  *    bison source file for timefile parser (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/vice/code/timeparser.y,v $
*  * VERSION
*  *    $Revision: 1.8 $
*  * DATE
*  *    $Date: 2005/10/25 05:00:26 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  *
*  * SPECIFICATION VERSION
*  *    implementation of timeparser.vdm version 1.2
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

%{
#include <stdio.h>
#include <iostream>
#include "metaiv.h"
#include "tb_wstring.h"
#include "TIMEPARSER.h"
#include "TIMEMAP.h"

#if !defined(__APPLE_CC__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif // !defined(__APPLE_CC__) && !defined(__FreeBSD__)

#define YYDEBUG 1

extern FILE *tp_in;
extern int tp_error(const char *s);
extern int tp_lex();
extern void tp_reset();
extern wostream* err_stream;
extern int tp_lineno_local; 
extern int tp_debug;

TYPE_TIMEPARSER_ContextInfo SetContext();
void InsertAssemblyEntry(TYPE_TIMEPARSER_AssemblyEntry);
int LookUp(TYPE_TIMEPARSER_AssemblyInstruction);

//wstring InstrMap(const TYPE_TIMEPARSER_Instruction & instr);
//wstring AssemblyInstrMap(const TYPE_TIMEPARSER_AssemblyInstruction & instr);

static Sequence* tf = new Sequence;
Map assemblyTable = Map();
%}

%union {
  wchar_t* str;
  int val;
  Sequence* sequence;
  Record* record;

  TYPE_TIMEPARSER_SimpleEntry* SimpleEntry_p;
  TYPE_TIMEPARSER_TypedEntry* TypedEntry_p;
  TYPE_TIMEPARSER_AssemblyEntry* AssemblyEntry_p;
  TYPE_REP_BasicTypeRep* BasicTypeRep_p; 
  
  TYPE_TIMEPARSER_Not* Not_p;
}



%token LEX_TIME_AddEnv
%token LEX_TIME_Addset
%token LEX_TIME_AllorExists
%token LEX_TIME_Always
%token LEX_TIME_Appendmap
%token LEX_TIME_Appendseq
%token LEX_TIME_Appendtup
%token LEX_TIME_Apply
%token LEX_TIME_BindList
%token LEX_TIME_BlockStmt
%token LEX_TIME_Br
%token LEX_TIME_Call
%token LEX_TIME_Cbr
%token LEX_TIME_Exit
%token LEX_TIME_Fieldsel
%token LEX_TIME_Guard
%token LEX_TIME_History
%token LEX_TIME_Iota
%token LEX_TIME_Isexpr
%token LEX_TIME_Narrowexpr
%token LEX_TIME_Isofbaseclass
%token LEX_TIME_Isofclass
%token LEX_TIME_LambdaExpr
%token LEX_TIME_LetBeST
%token LEX_TIME_Lookup
%token LEX_TIME_Loop
%token LEX_TIME_MapInverse
%token LEX_TIME_MatchPattern
%token LEX_TIME_Newobj
%token LEX_TIME_NonDet
%token LEX_TIME_Pattern
%token LEX_TIME_Polyinst
%token LEX_TIME_RecTrap
%token LEX_TIME_Reccons
%token LEX_TIME_Recmod
%token LEX_TIME_Return
%token LEX_TIME_Samebaseclass
%token LEX_TIME_Sameclass
%token LEX_TIME_Selfexpr
%token LEX_TIME_Seqlen
%token LEX_TIME_Seqmapover
%token LEX_TIME_Setcard
%token LEX_TIME_Setrng
%token LEX_TIME_Start
%token LEX_TIME_Stop
%token LEX_TIME_Subseq
%token LEX_TIME_Threadid
%token LEX_TIME_Trap
%token LEX_TIME_Tupsel
%token LEX_TIME_Update
%token LEX_TIME_Plus
%token LEX_TIME_Minus
%token LEX_TIME_Mult
%token LEX_TIME_Div
%token LEX_TIME_Rem
%token LEX_TIME_Mod
%token LEX_TIME_Intdiv
%token LEX_TIME_And
%token LEX_TIME_Or
%token LEX_TIME_Equal
%token LEX_TIME_GreaterThan
%token LEX_TIME_GreaterThanOrEqual
%token LEX_TIME_Abs
%token LEX_TIME_Floor
%token <Not_p> LEX_TIME_Not

%token LEX_ASSEMBLY_Add
%token LEX_ASSEMBLY_Branch
%token LEX_ASSEMBLY_Call
%token LEX_ASSEMBLY_Compare
%token LEX_ASSEMBLY_Return
%token LEX_ASSEMBLY_Sub
%token LEX_ASSEMBLY_Div
%token LEX_ASSEMBLY_Mul
%token LEX_ASSEMBLY_Neg
%token LEX_ASSEMBLY_Fsqrt
%token LEX_ASSEMBLY_Logic
%token LEX_ASSEMBLY_Cas
%token LEX_ASSEMBLY_Push
%token LEX_ASSEMBLY_Pop




%token <str> NUMERICLITERAL

/* Basic types */
%token <BasicTypeRep_p> LEX_TIME_BOOL LEX_TIME_NAT LEX_TIME_NATONE 
%token <BasicTypeRep_p> LEX_TIME_RAT LEX_TIME_REAL LEX_TIME_CHAR 
%token <BasicTypeRep_p> LEX_TIME_INT LEX_TIME_TOKEN

%type <BasicTypeRep_p> basictype
%type <SimpleEntry_p> simpleentry
%type <TypedEntry_p> typedentry
%type <AssemblyEntry_p> assemblyentry
%type <record> instruction binaryop unaryop timeentry 
%type <record> assemblyinstruction timebinaryop
%type <sequence> timeentries
%type <sequence> timefile
%type <val> timeexpr timebinaryexpr timebracketedexpr



%%

timefile:        
                         { }
                     | timeentries
                         { *tf = *$1; }
                 
                     ;

timeentries:           timeentry
                         { $$ = new Sequence;
                           if ($1){
                             $$ -> ImpAppend(*$1);
                             delete $1;
                           }
                         }
                     | timeentries ';' timeentry
                         { 
                           if ($3){
                             $1->ImpAppend(*$3);
                             delete $3;
                           }
                         }
                     ;

timeentry:             simpleentry
                        { $$ = $1;}
                     | typedentry
                        { $$ = (Record*) $1;}
                     | assemblyentry
                        { $$ = (Record*) 0; }
                     ;

simpleentry:           instruction timeexpr
                         { $$ = new TYPE_TIMEPARSER_SimpleEntry();
                           $$->SetField(1, *$1); 
                           $$->SetField(2, Int($2)); /* cid?? */
                           $$->SetField(3, SetContext());
                           delete $1; 
                         }

timeexpr:              assemblyinstruction 
                         { $$ = LookUp(*$1); 
                           delete $1;
                         }
                     | NUMERICLITERAL 
                         { $$ = atoi(TBWSTR::wstring2string($1).c_str());}
                     | timebinaryexpr
                     | timebracketedexpr;

timebinaryexpr:        timeexpr timebinaryop timeexpr
                         { int tag = (*$2).GetTag();
                           switch(tag){
                           case TAG_TYPE_TIMEPARSER_TimePlus:
                             $$ = $1 + $3; break;
                           case TAG_TYPE_TIMEPARSER_TimeMultiply:
                             $$ = $1 * $3; break;
                           }
                           delete $2;
                         }

timebinaryop:          LEX_TIME_Plus 
                         { $$ = new TYPE_TIMEPARSER_TimePlus();}
                     | LEX_TIME_Mult
                         { $$ = new TYPE_TIMEPARSER_TimeMultiply();}
                     ;

timebracketedexpr:     '(' timeexpr ')'
                         { $$ = $2;}

typedentry:            instruction ':' basictype timeexpr
                         { $$ = new TYPE_TIMEPARSER_TypedEntry();
                           ((Record*)$$)->SetField(1, *$1);
                           ((Record*)$$)->SetField(2, Int($4)); /* cid?? */ 
                           ((Record*)$$)->SetField(3, *$3);
                           ((Record*)$$)->SetField(4, SetContext());
                           delete $1; 
                         }

assemblyentry:         assemblyinstruction NUMERICLITERAL
                         { $$ = new TYPE_TIMEPARSER_AssemblyEntry();
                           $$->SetField(1, *$1); 
                           $$->SetField(2, Int(atoi(TBWSTR::wstring2string($2).c_str()))); /* cid?? */
                           $$->SetField(3, SetContext());
                           InsertAssemblyEntry(*$$);
                           delete $1;
                         }             

basictype:             LEX_TIME_BOOL 
                         {
                           $$ = new TYPE_REP_BooleanTypeRep();
                          }
                     | LEX_TIME_NAT 
                         {
                           $$ = new TYPE_REP_NumericTypeRep();
                           $$->SetField(1, Int(TAG_quote_NAT));
                         }
                     | LEX_TIME_NATONE 
                         {
                           $$ = new TYPE_REP_NumericTypeRep();
                           $$->SetField(1, Int(TAG_quote_NATONE));
                         }
                     | LEX_TIME_INT 
                         {
                           $$ = new TYPE_REP_NumericTypeRep();
                           $$->SetField(1, Int(TAG_quote_INTEGER));
                         }
                     | LEX_TIME_RAT 
                         {
                           $$ = new TYPE_REP_NumericTypeRep();
                           $$->SetField(1, Int(TAG_quote_RAT));
                         }
                     | LEX_TIME_REAL 
                         {
                           $$ = new TYPE_REP_NumericTypeRep();
                           $$->SetField(1, Int(TAG_quote_REAL));
                         }
                     | LEX_TIME_CHAR 
                         {
                           $$ = new TYPE_REP_CharTypeRep();
                         }
                     | LEX_TIME_TOKEN
                         {
                           $$ = new TYPE_REP_TokenTypeRep();
                          }
                     ;

instruction:           binaryop 
                     | unaryop 
                     | LEX_TIME_AddEnv
                         { $$ = new TYPE_TIMEPARSER_AddEnv() ; }
                     | LEX_TIME_Addset
                         { $$ = new TYPE_TIMEPARSER_Addset() ; }
                     | LEX_TIME_AllorExists
                         { $$ = new TYPE_TIMEPARSER_AllorExists() ; }
                     | LEX_TIME_Always
                         { $$ = new TYPE_TIMEPARSER_Always() ; }
                     | LEX_TIME_Appendmap
                         { $$ = new TYPE_TIMEPARSER_Appendmap() ; }
                     | LEX_TIME_Appendseq
                         { $$ = new TYPE_TIMEPARSER_Appendseq() ; }
                     | LEX_TIME_Appendtup
                         { $$ = new TYPE_TIMEPARSER_Appendtup() ; }
                     | LEX_TIME_Apply
                         { $$ = new TYPE_TIMEPARSER_Apply() ; }
                     | LEX_TIME_BindList
                         { $$ = new TYPE_TIMEPARSER_BindList() ; }
                     | LEX_TIME_BlockStmt
                         { $$ = new TYPE_TIMEPARSER_BlockStmt() ; }
                     | LEX_TIME_Br
                         { $$ = new TYPE_TIMEPARSER_Br() ; }
                     | LEX_TIME_Call
                         { $$ = new TYPE_TIMEPARSER_Call() ; }
                     | LEX_TIME_Cbr
                         { $$ = new TYPE_TIMEPARSER_Cbr() ; }
                     | LEX_TIME_Exit
                         { $$ = new TYPE_TIMEPARSER_Exit() ; }
                     | LEX_TIME_Fieldsel
                         { $$ = new TYPE_TIMEPARSER_Fieldsel() ; }
                     | LEX_TIME_Guard
                         { $$ = new TYPE_TIMEPARSER_Guard() ; }
                     | LEX_TIME_History
                         { $$ = new TYPE_TIMEPARSER_History() ; }
                     | LEX_TIME_Iota
                         { $$ = new TYPE_TIMEPARSER_Iota() ; }
                     | LEX_TIME_Isexpr
                         { $$ = new TYPE_TIMEPARSER_Isexpr() ; }
                     | LEX_TIME_Narrowexpr
                         { $$ = new TYPE_TIMEPARSER_Narrowexpr() ; }
                     | LEX_TIME_Isofbaseclass
                         { $$ = new TYPE_TIMEPARSER_Isofbaseclass() ; }
                     | LEX_TIME_Isofclass
                         { $$ = new TYPE_TIMEPARSER_Isofclass() ; }
                     | LEX_TIME_LambdaExpr
                         { $$ = new TYPE_TIMEPARSER_LambdaExpr() ; }
                     | LEX_TIME_LetBeST
                         { $$ = new TYPE_TIMEPARSER_LetBeST() ; }
                     | LEX_TIME_Lookup
                         { $$ = new TYPE_TIMEPARSER_Lookup() ; }
                     | LEX_TIME_Loop
                         { $$ = new TYPE_TIMEPARSER_Loop() ; }
                     | LEX_TIME_MapInverse
                         { $$ = new TYPE_TIMEPARSER_MapInverse() ; }
                     | LEX_TIME_MatchPattern
                         { $$ = new TYPE_TIMEPARSER_MatchPattern() ; }
                     | LEX_TIME_Newobj
                         { $$ = new TYPE_TIMEPARSER_Newobj() ; }
                     | LEX_TIME_NonDet
                         { $$ = new TYPE_TIMEPARSER_NonDet() ; }
                     | LEX_TIME_Pattern
                         { $$ = new TYPE_TIMEPARSER_Pattern() ; }
                     | LEX_TIME_Polyinst
                         { $$ = new TYPE_TIMEPARSER_Polyinst() ; }
                     | LEX_TIME_Reccons
                         { $$ = new TYPE_TIMEPARSER_Reccons() ; }
                     | LEX_TIME_Recmod
                         { $$ = new TYPE_TIMEPARSER_Recmod() ; }
                     | LEX_TIME_RecTrap
                         { $$ = new TYPE_TIMEPARSER_RecTrap() ; }
                     | LEX_TIME_Return
                         { $$ = new TYPE_TIMEPARSER_Return() ; }
                     | LEX_TIME_Samebaseclass
                         { $$ = new TYPE_TIMEPARSER_Samebaseclass() ; }
                     | LEX_TIME_Sameclass
                         { $$ = new TYPE_TIMEPARSER_Sameclass() ; }
                     | LEX_TIME_Selfexpr
                         { $$ = new TYPE_TIMEPARSER_Selfexpr() ; }
                     | LEX_TIME_Seqlen
                         { $$ = new TYPE_TIMEPARSER_Seqlen() ; }
                     | LEX_TIME_Seqmapover
                         { $$ = new TYPE_TIMEPARSER_Seqmapover() ; }
                     | LEX_TIME_Setcard
                         { $$ = new TYPE_TIMEPARSER_Setcard() ; }
                     | LEX_TIME_Setrng
                         { $$ = new TYPE_TIMEPARSER_Setrng() ; }
                     | LEX_TIME_Start
                         { $$ = new TYPE_TIMEPARSER_Start() ; }
                     | LEX_TIME_Stop
                         { $$ = new TYPE_TIMEPARSER_Stop() ; }
                     | LEX_TIME_Subseq
                         { $$ = new TYPE_TIMEPARSER_Subseq() ; }
                     | LEX_TIME_Threadid
                         { $$ = new TYPE_TIMEPARSER_Threadid() ; }
                     | LEX_TIME_Trap
                         { $$ = new TYPE_TIMEPARSER_Trap() ; }
                     | LEX_TIME_Tupsel
                         { $$ = new TYPE_TIMEPARSER_Tupsel() ; }
                     | LEX_TIME_Update
                         { $$ = new TYPE_TIMEPARSER_Update() ; }
                     ;

binaryop:              LEX_TIME_Plus 
                         { $$ = new TYPE_TIMEPARSER_Plus(); }
                     | LEX_TIME_Minus 
                         { $$ = new TYPE_TIMEPARSER_Minus(); }
                     | LEX_TIME_Mult 
                         { $$ = new TYPE_TIMEPARSER_Mult(); }
                     | LEX_TIME_Div 
                         { $$ = new TYPE_TIMEPARSER_Div(); }
                     | LEX_TIME_Rem 
                         { $$ = new TYPE_TIMEPARSER_Rem(); }
                     | LEX_TIME_Mod 
                         { $$ = new TYPE_TIMEPARSER_Mod(); }
                     | LEX_TIME_Intdiv 
                         { $$ = new TYPE_TIMEPARSER_Intdiv(); }
                     | LEX_TIME_And 
                         { $$ = new TYPE_TIMEPARSER_And(); }
                     | LEX_TIME_Or 
                         { $$ = new TYPE_TIMEPARSER_Or(); }
                     | LEX_TIME_Equal 
                         { $$ = new TYPE_TIMEPARSER_Equal(); }
                     | LEX_TIME_GreaterThan 
                         { $$ = new TYPE_TIMEPARSER_GreaterThan(); }
                     | LEX_TIME_GreaterThanOrEqual 
                         { $$ = new TYPE_TIMEPARSER_GreaterThanOrEqual(); };

unaryop:               LEX_TIME_Abs 
                         { $$ = new TYPE_TIMEPARSER_Abs(); }
                     | LEX_TIME_Floor 
                         { $$ = new TYPE_TIMEPARSER_Floor(); }
                     | LEX_TIME_Not 
                         { $$ = new TYPE_TIMEPARSER_Not(); };

assemblyinstruction:   LEX_ASSEMBLY_Add
                         { $$ = new TYPE_TIMEPARSER_Assembly_uAdd();}
                     | LEX_ASSEMBLY_Branch
                         { $$ = new TYPE_TIMEPARSER_Assembly_uBranch();}
                     | LEX_ASSEMBLY_Call
                         { $$ = new TYPE_TIMEPARSER_Assembly_uCall();}
                     | LEX_ASSEMBLY_Compare
                         { $$ = new TYPE_TIMEPARSER_Assembly_uCompare();}
                     | LEX_ASSEMBLY_Return
                         { $$ = new TYPE_TIMEPARSER_Assembly_uReturn();}
                     | LEX_ASSEMBLY_Sub
                         { $$ = new TYPE_TIMEPARSER_Assembly_uSub();}
                     | LEX_ASSEMBLY_Div
                         { $$ = new TYPE_TIMEPARSER_Assembly_uDiv();}
                     | LEX_ASSEMBLY_Mul
                         { $$ = new TYPE_TIMEPARSER_Assembly_uMul();}
                     | LEX_ASSEMBLY_Neg
                         { $$ = new TYPE_TIMEPARSER_Assembly_uNeg();}
                     | LEX_ASSEMBLY_Fsqrt
                         { $$ = new TYPE_TIMEPARSER_Assembly_uFsqrt();}
                     | LEX_ASSEMBLY_Logic
                         { $$ = new TYPE_TIMEPARSER_Assembly_uLogic();}
                     | LEX_ASSEMBLY_Cas
                         { $$ = new TYPE_TIMEPARSER_Assembly_uCas();}
                     | LEX_ASSEMBLY_Push
                         { $$ = new TYPE_TIMEPARSER_Assembly_uPush();}
                     | LEX_ASSEMBLY_Pop
                         { $$ = new TYPE_TIMEPARSER_Assembly_uPop();}
                     ;

%%

int yyparse();


TYPE_TIMEPARSER_ContextInfo  SetContext()
{
  TYPE_TIMEPARSER_ContextInfo* ci = new TYPE_TIMEPARSER_ContextInfo();
  ci->set_line(Int(tp_lineno_local));
  return *ci;
}

// Insert an assembly entry in the 
void InsertAssemblyEntry(TYPE_TIMEPARSER_AssemblyEntry ae)
{
  TYPE_TIMEPARSER_AssemblyInstruction instr = ae.get_instr();
  if (assemblyTable.DomExists(instr)){
    // Get the name of the instruction
    Sequence instrname = vdm_TIMEPARSER_AssemblyInstrMap(instr);
    wstring entry_name;
    instrname.GetString(entry_name);
//    wstring entry_name (AssemblyInstrMap(instr));
    Int ln(((TYPE_TIMEPARSER_ContextInfo)ae.GetField(ae.Length())).get_line());
    (*err_stream) << L"Warning: attempt to redefine " << entry_name
                  << L" at line " << ln << L" - ignored\n";
  }
  else {
    assemblyTable.Insert(instr, ae.get_time());
  }
}

int LookUp(TYPE_TIMEPARSER_AssemblyInstruction instr)
{
  if (assemblyTable.DomExists(instr)){
    return ((Int) assemblyTable[instr]).GetValue();
  }
  else {
    Sequence instr_seq = vdm_TIMEPARSER_AssemblyInstrMap(instr);
    wstring instr_string;
    instr_seq.GetString(instr_string);
//    wstring instr_string (AssemblyInstrMap(instr));
    (*err_stream) << L"Warning: no value yet defined for "
                  << instr_string << L" at line number " << tp_lineno_local
                  << L" ... using default value\n";
    return vdm_TIMEMAP_defaultDuration.GetValue();
  }
}
              

// Output an error message to an output stream for a particular error object
void tp_OutputError(wostream &out, TYPE_TIMEMAP_Error e)
{
  Record entry = e.get_entry();

  // Extract the entry instruction from the error object
  TYPE_TIMEPARSER_Instruction instr;
  if(entry.GetTag() == TAG_TYPE_TIMEPARSER_SimpleEntry)
    instr = ((TYPE_TIMEPARSER_SimpleEntry) entry).get_instr();
  else
    instr = ((TYPE_TIMEPARSER_TypedEntry) entry).get_instr();

  // Get the name of the instruction
  Sequence instrname = vdm_TIMEPARSER_InstrMap(instr);
  wstring entry_name;
  instrname.GetString(entry_name);
//  wstring entry_name (InstrMap(instr));

  // Output error message header
  out << L"*Error* entry \"" << entry_name << L"\" at line " 
      << ((TYPE_TIMEPARSER_ContextInfo)entry.GetField(entry.Length())).get_line()
      << L" is repeated at ";

  // Step through each repeated entry, listing the line at which the
  // repetition occurs
  Generic g;
  Set reps = e.get_reps();
  int i = 0;
  for(int jj = reps.First(g); jj; jj = reps.Next(g) ) 
  {
    Record r = (Record) g;
    Generic ci_field = r.GetField(r.Length());
    TYPE_TIMEPARSER_ContextInfo ci(ci_field);
    if (i != 0)
      out << L" and ";
    out << L"line " 
        << ci.get_line();
    i++;
  }
  out << endl;
}


// Output an error message to an output stream for all the error objects
// generated by a time file
void tp_OutputErrors(wostream &o, Set errs)
{
  Generic g;
  for(int ii = errs.First(g); ii; ii = errs.Next(g))
  {
    tp_OutputError(o, (TYPE_TIMEMAP_Error) g);
  }
}

// It is assumed that fname exists
bool tp_main(wostream *o, const wchar_t *fname, Set &errs, 
             TYPE_TIMEMAP_Timemap &timemap)
{
  tp_in = fopen(TBWSTR::wstring2fsstr(wstring(fname)).c_str(),"r");
  tp_lineno_local = 1;
  err_stream = o;

  //tp_debug = 1;
  int parse_result = tp_parse();
  fclose(tp_in);
  tp_in = (FILE*) 0;
  tp_reset();
  assemblyTable = Map();

  if(parse_result != 0)
    return false;

  errs = vdm_TIMEMAP_wf_uTimefile((SEQ<Record>) *tf);
  if (!errs.IsEmpty())
    return false;
  else {
    timemap = vdm_TIMEMAP_MkTimeMap((SEQ<Record>) *tf);
    return true;
  }

}

/*
wstring InstrMap(const TYPE_TIMEPARSER_Instruction & instr)
{
  switch(instr.GetTag())
  {
    case TAG_TYPE_TIMEPARSER_AddEnv:
      return L"AddEnv";
    case TAG_TYPE_TIMEPARSER_Addset:
      return L"Addset ";
    case TAG_TYPE_TIMEPARSER_AllorExists:
      return L"AllorExists";
    case TAG_TYPE_TIMEPARSER_Always:
      return L"Always";
    case TAG_TYPE_TIMEPARSER_Appendmap:
      return L"Appendmap ";
    case TAG_TYPE_TIMEPARSER_Appendseq:
      return L"Appendseq ";
    case TAG_TYPE_TIMEPARSER_Appendtup:
      return L"Appendtup ";
    case TAG_TYPE_TIMEPARSER_Apply:
      return L"Apply ";
    case TAG_TYPE_TIMEPARSER_BindList:
      return L"BindList";
    case TAG_TYPE_TIMEPARSER_BlockStmt:
      return L"BlockStmt";
    case TAG_TYPE_TIMEPARSER_Br:
      return L"Br ";
    case TAG_TYPE_TIMEPARSER_Call:
      return L"Call";
    case TAG_TYPE_TIMEPARSER_Cbr:
      return L"Cbr ";
    case TAG_TYPE_TIMEPARSER_Exit:
      return L"Exit";
    case TAG_TYPE_TIMEPARSER_Fieldsel:
      return L"Fieldsel ";
    case TAG_TYPE_TIMEPARSER_Guard:
      return L"Guard";
    case TAG_TYPE_TIMEPARSER_History:
      return L"History ";
    case TAG_TYPE_TIMEPARSER_Iota:
      return L"Iota";
    case TAG_TYPE_TIMEPARSER_Narrowexpr:
      return L"Narrowexpr ";
    case TAG_TYPE_TIMEPARSER_Isofbaseclass:
      return L"Isofbaseclass ";
    case TAG_TYPE_TIMEPARSER_Isofclass:
      return L"Isofclass ";
    case TAG_TYPE_TIMEPARSER_LambdaExpr:
      return L"LambdaExpr";
    case TAG_TYPE_TIMEPARSER_LetBeST:
      return L"LetBeST";
    case TAG_TYPE_TIMEPARSER_Lookup:
      return L"Lookup ";
    case TAG_TYPE_TIMEPARSER_Loop:
      return L"Loop";
    case TAG_TYPE_TIMEPARSER_MapInverse:
      return L"MapInverse";
    case TAG_TYPE_TIMEPARSER_MatchPattern:
      return L"MatchPattern";
    case TAG_TYPE_TIMEPARSER_Newobj:
      return L"Newobj ";
    case TAG_TYPE_TIMEPARSER_NonDet:
      return L"NonDet";
    case TAG_TYPE_TIMEPARSER_Pattern:
      return L"Pattern";
    case TAG_TYPE_TIMEPARSER_Polyinst:
      return L"Polyinst ";
    case TAG_TYPE_TIMEPARSER_RecTrap:
      return L"RecTrap";
    case TAG_TYPE_TIMEPARSER_Reccons:
      return L"Reccons ";
    case TAG_TYPE_TIMEPARSER_Recmod:
      return L"Recmod ";
    case TAG_TYPE_TIMEPARSER_Return:
      return L"Return";
    case TAG_TYPE_TIMEPARSER_Samebaseclass:
      return L"Samebaseclass ";
    case TAG_TYPE_TIMEPARSER_Sameclass:
      return L"Sameclass ";
    case TAG_TYPE_TIMEPARSER_Selfexpr:
      return L"Selfexpr ";
    case TAG_TYPE_TIMEPARSER_Seqlen:
      return L"Seqlen ";
    case TAG_TYPE_TIMEPARSER_Seqmapover:
      return L"Seqmapover ";
    case TAG_TYPE_TIMEPARSER_Setcard:
      return L"Setcard ";
    case TAG_TYPE_TIMEPARSER_Setrng:
      return L"Setrng ";
    case TAG_TYPE_TIMEPARSER_Start:
      return L"Start";
    case TAG_TYPE_TIMEPARSER_Stop:
      return L"Stop";
    case TAG_TYPE_TIMEPARSER_Subseq:
      return L"Subseq ";
    case TAG_TYPE_TIMEPARSER_Threadid:
      return L"Threadid ";
    case TAG_TYPE_TIMEPARSER_Trap:
      return L"Trap";
    case TAG_TYPE_TIMEPARSER_Tupsel:
      return L"Tupsel ";
    case TAG_TYPE_TIMEPARSER_Update:
      return L"Update";
    case TAG_TYPE_TIMEPARSER_Plus:
      return L"Plus";
    case TAG_TYPE_TIMEPARSER_Minus:
      return L"Minus";
    case TAG_TYPE_TIMEPARSER_Mult:
      return L"Mult";
    case TAG_TYPE_TIMEPARSER_Div:
      return L"Div";
    case TAG_TYPE_TIMEPARSER_Rem:
      return L"Rem";
    case TAG_TYPE_TIMEPARSER_Mod:
      return L"Mod";
    case TAG_TYPE_TIMEPARSER_Intdiv:
      return L"Intdiv";
    case TAG_TYPE_TIMEPARSER_And:
      return L"And";
    case TAG_TYPE_TIMEPARSER_Or:
      return L"Or";
    case TAG_TYPE_TIMEPARSER_Equal:
      return L"Equal";
    case TAG_TYPE_TIMEPARSER_GreaterThan:
      return L"GreaterThan";
    case TAG_TYPE_TIMEPARSER_GreaterThanOrEqual:
      return L"GreaterThanOrEqual";
    case TAG_TYPE_TIMEPARSER_Abs:
      return L"Abs";
    case TAG_TYPE_TIMEPARSER_Floor:
      return L"Floor";
    case TAG_TYPE_TIMEPARSER_Not:
      return L"Not";
    default:
      return L"Unknown";
  }
}

wstring AssemblyInstrMap(const TYPE_TIMEPARSER_AssemblyInstruction & instr)
{
  switch(instr.GetTag())
  {
    case TAG_TYPE_TIMEPARSER_Assembly_uAdd:
      return L"add";
    case TAG_TYPE_TIMEPARSER_Assembly_uBranch:
      return L"branch";
    case TAG_TYPE_TIMEPARSER_Assembly_uCall:
      return L"call";
    case TAG_TYPE_TIMEPARSER_Assembly_uCompare:
      return L"compare";
    case TAG_TYPE_TIMEPARSER_Assembly_uReturn:
      return L"return";
    case TAG_TYPE_TIMEPARSER_Assembly_uSub:
      return L"sub";
    case TAG_TYPE_TIMEPARSER_Assembly_uDiv:
      return L"div";
    case TAG_TYPE_TIMEPARSER_Assembly_uMul:
      return L"mul";
    case TAG_TYPE_TIMEPARSER_Assembly_uNeg:
      return L"neg";
    case TAG_TYPE_TIMEPARSER_Assembly_uFsqrt:
      return L"fsqrt";
    case TAG_TYPE_TIMEPARSER_Assembly_uLogic:
      return L"logic";
    case TAG_TYPE_TIMEPARSER_Assembly_uCas:
      return L"cas";
    case TAG_TYPE_TIMEPARSER_Assembly_uPush:
      return L"push";
    case TAG_TYPE_TIMEPARSER_Assembly_uPop:
      return L"pop";
    default:
      return L"unknown";
  }
}
*/
