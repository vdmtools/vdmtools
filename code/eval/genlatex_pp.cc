/*** 
*  * WHAT
*  *   Pretty printer function for VDM++ AS for the
*  *   VDM++ Toolbox
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/genlatex_pp.cc,v $
*  * VERSION
*  *    $Revision: 1.48 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:04 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    .
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Lars T. Olsen: $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include "AS.h"
#include "genlatex.h"
#include <fstream.h> 


/////////////
// PRETTY PRINTER FUNCTIONS
////////////

void TYPE_AS_Sync_pp(Sequence sync, Options o) {
  int i = 1;
  Generic g; 
  for(int cont=sync.First(g);cont; cont = sync.Next(g),i++) {
    Record elem(g);
    if (i > 1) OUT << L", ";
    switch (elem.GetTag()){
    case TAG_TYPE_AS_Permission:
      ((TYPE_AS_PermissionStmt)elem).pp(o);
      break;
    case Trace:
      ((TYPE_AS_Trace)elem).pp(o);
      break;
    default:
      my_latex.ErrMsg1(elem,L"Unknown Construct in Sync");
    }
  }
};

void TYPE_AS_Term_pp(Record t, Options o) {
  switch (t.GetTag()){
  case CTerm:
    ((TYPE_AS_CTerm)t).pp(o);
    break;
  case CTerm0:
    ((TYPE_AS_CTerm0)t).pp(o);
    break;
  case CTerm1:
    ((TYPE_AS_CTerm1)t).pp(o);
    break;
  default:
    my_latex.ErrMsg1(t,L"Unknown Term");
  }
};

void TYPE_AS_CTerm::pp(Options o) {
  if (this->GetTag() == TraceExpr) {
    OUT << L"(";
    ((TYPE_AS_TraceExpr) (*this)).pp(o);
    OUT << L")";
  } 
  else {
    ((TYPE_AS_Name) (*this)).pp(o);
  }
};

void TYPE_AS_Error::pp(Options o) {
  OUT << L"\n\\err{";
  get_nm().pp(o);
  OUT << L"}{";
  TYPE_AS_Expr_pp(get_cond(),o);
  OUT << L" \\To ";
  TYPE_AS_Expr_pp(get_action(),o);
  OUT << L"}";
};

void TYPE_AS_Trace::pp(Options o) {

  if (INDEX) my_latex.GenIndex(get_name(),methodocc_index); //FIXME: was get_id(). 

  OUT << L"\n\\subtrace{";
  get_name().pp(o);
  OUT << L"}{";
  TYPE_AS_TraceStructure_pp(get_strct(),o);
  OUT << L"}";
};

void TYPE_AS_CTerm0::pp(Options o) {
  OUT << L"\\cterm{";
  get_1().pp(o);
  OUT << L"}";
};

void TYPE_AS_CTerm1::pp(Options o) {
  OUT << L"\\ctermone{";
  get_1().pp(o);
  OUT << L"}";
};

void TYPE_AS_Effect::pp(Options o) {

  OUT << L"\n\\effect{";
  get_nms().pp(o);
  OUT << L"}{";
  TYPE_AS_Expr_pp(get_expr(),o);
  OUT << L"}";
};

void TYPE_AS_ExtInf::pp(Options o) {
  TYPE_AS_Mode_pp(get_mode(),o);
  get_vars().pp(o);
  OUT << L" : ";
  if (! get_tp().IsNil() ) {
    TYPE_AS_Type_pp(get_tp(),o);
  }
};

void TYPE_AS_PerObl::pp(Options o) {

  if (INDEX) my_latex.GenIndex(get_mtd(),methodocc_index);

  OUT << L"\n\\periodic{";
  TYPE_AS_Expr_pp(get_dur(),o);
  OUT << L"}{";
  get_mtd().pp(o);
  OUT << L"}";
};

void TYPE_AS_Thread_pp(Sequence s, Options o) {
  for (size_t i = 1; i <= s.Length(); i++ ) {
    Record elem(s[i]);
    if (i > 1) OUT << SLF;
    if (elem.GetTag() == PerObl) ((TYPE_AS_PerObl)elem).pp(o);
    else  TYPE_AS_Stmt_pp(elem,o);
  }
};


void TYPE_AS_Thread::pp(Options o) {
  type_dL s (this->get_1());
  for (size_t i = 1; i <= s.Length(); i++) {
    Record elem(s[i]);
    if (i > 1) OUT << SLF;
    if (elem.GetTag() == PerObl)
      ((TYPE_AS_PerObl)elem).pp(o);
    else
      TYPE_AS_Stmt_pp(elem,o);
  }
}

void TYPE_AS_DelTrace::pp(Options o) {
  OUT << L"\\del{";
  get_m1().pp(o);
  OUT << L"}{";
  get_m2().pp(o);
  OUT << L"}";
};

void TYPE_AS_TraceSet_pp(Record ts, Options o) {
  switch (ts.GetTag()) {
  case AlphabetExpr:
    ((TYPE_AS_AlphabetExpr) ts).pp(o);
    break;
  case TraceExpr:
    ((TYPE_AS_TraceExpr) ts).pp(o);
    break;
  default:
    my_latex.ErrMsg1(ts,L"Unknown construct in Trace Set");
  }
};

void TYPE_AS_PrefTrace::pp(Options o) {
  OUT << L"\\pref{";
  get_1().pp(o);
  OUT << L"}";
};

void TYPE_AS_ProjTrace::pp(Options o) {
  TYPE_AS_TraceStructure_pp(get_strct(),o);
  OUT << L"$**$";  // ???
  get_alpha().pp(o);
};


void TYPE_AS_SyncTrace::pp(Options o) {
  OUT << L"\\slack{";
  get_expr1().pp(o);
  OUT << L"}{";
  get_expr2().pp(o);
  OUT << L"}{";
  get_m1().pp(o);
  OUT << L"}{";
  get_m2().pp(o);
  OUT << L"}";
};

void TYPE_AS_TraceExpr::pp(Options o) {
  for (size_t i = 1; i < get_1().Length(); i++){
    TYPE_AS_Term_pp(get_1()[i],o);
  }
};

void TYPE_AS_AlphaTrace::pp(Options o) {
  OUT << L"\n\\tracestructure{";
  TYPE_AS_TraceSet_pp(get_trace(),o);
  OUT << L"}{";
  get_alpha().pp(o);
  OUT << L"}";
};

void TYPE_AS_Assumption::pp(Options o) {

  OUT << L"\n\\assumption{";
  get_nms().pp(o);
  OUT << L"}{";
  TYPE_AS_Expr_pp(get_expr(),o);
  OUT << L"}";
};

void TYPE_AS_QsyncTrace::pp(Options o) {
  OUT << L"\\gslack{";
  get_expr1().pp(o);
  OUT << L"}{";
  get_expr2().pp(o);
  OUT << L"}{";
  get_m1().pp(o);
  OUT << L"}{";
  get_m2().pp(o);
  OUT << L"}{";
  get_m3().pp(o);
  OUT << L"}{";
  get_m4().pp(o);
  OUT << L"}";
};

void TYPE_AS_WeaveTrace::pp(Options o) {
  OUT << L"\\weave{";
  get_struct1().pp(o);
  OUT << L"}{";
  get_struct2().pp(o);
  OUT << L"}";
};

void TYPE_AS_InstanceInv::pp(Options o) {

//FIXME-LANGADJ//   OUT << L"\n\\instinv{";
//FIXME-LANGADJ//   if (get_nms().IsSequence()) {
//FIXME-LANGADJ//     PP_SEQ(type_7AS_NameCL,get_nms,L",");
//FIXME-LANGADJ//   } else 
//FIXME-LANGADJ//     OUT << L"\\kObjectstate ";
//FIXME-LANGADJ//   OUT << L"}{";
//FIXME-LANGADJ//   TYPE_AS_Expr_pp(get_expr(),o);
//FIXME-LANGADJ//   OUT << L"\n}";
};

void TYPE_AS_TimeVarDecl::pp(Options o) {
  
  if (INDEX) 
    for (size_t i =1 ; i <= get_nms().Length(); i++) 
      my_latex.GenIndex(get_nms()[i],timevar_index);

  OUT << L"\n\\timevardcl{";
  if (! get_i().IsNil() ) OUT << L"\\kInput ";
  get_nms().pp(o);
  OUT << L"}{";
  TYPE_AS_Type_pp(get_tp(),o);
  OUT << L"}";
};

void TYPE_AS_AlphabetExpr::pp(Options o) {
  OUT << L"\\alphabet{";
  if (get_1().IsSequence()) {
    PP_SEQ_VAL(type_7AS_NameCL,get_1(),L", ");
     }  
  else {
    TYPE_AS_TraceStructure_pp(get_1(),o);
  };
  OUT << L"}";
};

/////////////////////////////////
// The new PP for Class taking into account Texbreaks and
// the order of the item in the source text
/////////////////////////////////
 
//FIXME-LANGADJ//void TYPE_AS_InstanceVarDef::pp(Options o) {
//FIXME-LANGADJ//  if (INDEX) my_latex.GenIndex(get_nm(),instvar_index);
//FIXME-LANGADJ//
//FIXME-LANGADJ//  OUT << L"\n\\vardcl{";
//FIXME-LANGADJ//  get_nm().pp(o);
//FIXME-LANGADJ//  OUT << L"}{";
//FIXME-LANGADJ//   TYPE_AS_Type_pp(get_tp(),o);
//FIXME-LANGADJ//  OUT << L"}";
//FIXME-LANGADJ//};

//FIXME-LANGADJ//void type_17AS_InstanceVarDefCL::pp(Options o) {
//FIXME-LANGADJ//      PP_SEQ_this;
//FIXME-LANGADJ//};

void TYPE_AS_ParameterTypes::pp(Options o) {
  PP_SEQ1_this(L", ");
};

void TYPE_AS_PermissionStmt::pp(Options o) { // TYPE_AS_Permission

  if (INDEX) my_latex.GenIndex(get_spec(),methodocc_index);

  OUT << L"\n\\per{";
  get_spec().pp(o);
  OUT << L"}{\n";
  TYPE_AS_Expr_pp(get_guard(),o);
  OUT << L"\n}";
};

void TYPE_AS_TraceStructure_pp(Record ts, Options o) {
  switch (ts.GetTag()) {
  case Name:
    ((TYPE_AS_Name) ts).pp(o);
    break;
  case AlphaTrace:
    ((TYPE_AS_AlphaTrace) ts).pp(o);
    break;
  case ProjTrace:
    ((TYPE_AS_ProjTrace) ts).pp(o);
    break;
  case PrefTrace:
    ((TYPE_AS_PrefTrace) ts).pp(o);
    break;
  case DelTrace:
    ((TYPE_AS_DelTrace) ts).pp(o);
    break;
  case WeaveTrace:
    ((TYPE_AS_WeaveTrace) ts).pp(o);
    break;
  case SyncTrace:
    ((TYPE_AS_SyncTrace) ts).pp(o);
    break;
  case QsyncTrace:
    ((TYPE_AS_QsyncTrace) ts).pp(o);
    break;
  default:
    my_latex.ErrMsg1(ts,L"Unknown Trace Structure");
  }
};



