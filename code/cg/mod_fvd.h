/***
*  * WHAT
*  *    Implementation of mod_fvd.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_fvd.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2001/12/17 09:43:47 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_fvd_h__
#define __mod_fvd_h__

private:
  SEQ<TYPE_CPP_Stmt> vi;                    // seq of CPP`Stmt
  SEQ<TYPE_CPP_IdentDeclaration> v_idecl_l; // seq of CPP`IdentDeclaration
  SEQ<TYPE_CPP_Stmt> uncaught;              // seq of CPP`Stmt

private:
  void InitState_FVD();
//Sequence GenFctDecls(Sequence);
//Sequence GenFctDefs(Sequence);
  Map GenValues(const TYPE_AS_Name&, const SEQ<TYPE_AS_ValueDef> &);
  SEQ<TYPE_CPP_Stmt> GetValInit() const;
  SEQ<TYPE_CPP_IdentDeclaration> GetIdeclL() const;
  void AddValInit(const SEQ<TYPE_CPP_Stmt> &);
  void AddUncaughtValInit(const SEQ<TYPE_CPP_Stmt> & p_stmts);
  SEQ<TYPE_CPP_Stmt> GetUncaught() const;
//  void ClearUncaught();

#endif // __mod_fvd_h__
