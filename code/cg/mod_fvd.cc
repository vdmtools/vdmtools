/***
*  * WHAT
*  *    mod_fvd.cc: Implementation of mod_fvd.vdm 1.20
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_fvd.cc,v $
*  * VERSION
*  *    $Revision: 1.13 $
*  * DATE
*  *    $Date: 2001/12/17 09:43:47 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $ + jojo
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"

// FVD functions and operations

void vdmcg::InitState_FVD()
{
  this->vi.Clear();
  this->v_idecl_l.Clear();
  this->uncaught.Clear();
}

// GenValues
// classname : AS`Name
// valdef_l : seq of AS`ValueDef
// ==> map AS`Access to seq of CPP`MemberDeclaration
Map vdmcg::GenValues(const TYPE_AS_Name& classname, const SEQ<TYPE_AS_ValueDef> & valdef_l)
{
  SetValState();
  // ==> (map AS`Access to seq of CPP`MemberDeclaration) * seq of CPP`Stmt * seq of CPP`IdentDeclaration
  Tuple tmp (GenValDef(classname, valdef_l));
  UnsetValState();

  Map mem_decl_l (tmp.GetMap(1));  // map AS`Access to seq of CPP`MemberDeclaration
  SEQ<TYPE_CPP_Stmt> stmt_l (tmp.GetSequence(2)); // seq of CPP`Stmt 
  SEQ<TYPE_CPP_IdentDeclaration> idecl_l (tmp.GetSequence(3)); // seq of CPP`IdentDeclaration
  
  this->vi = stmt_l;
  this->v_idecl_l = idecl_l;
  return mem_decl_l;
}

// GetValInit
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GetValInit() const
{
  return this->vi;
}

// GetIdeclL
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GetIdeclL() const
{
  return this->v_idecl_l;
}

// AddValInit
// p_stmts : seq of CPP`Stmt
// ==> ()
void vdmcg::AddValInit(const SEQ<TYPE_CPP_Stmt> & p_stmts)
{
  this->vi.ImpConc(p_stmts);
}

// AddUncaughtValInit
// p_stmts : seq of CPP`Stmt
// ==> ()
void vdmcg::AddUncaughtValInit(const SEQ<TYPE_CPP_Stmt> & p_stmts)
{
  this->uncaught.ImpConc(p_stmts);
}

// GetUncaught
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GetUncaught() const
{
  return this->uncaught;
}

// 20060710
// ClearUncaught
//void vdmcg::ClearUncaught()
//{
//  this->uncaught.Clear();
//}
