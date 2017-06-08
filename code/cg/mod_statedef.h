/***
*  * WHAT
*  *    Implementation of mod_statedef.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_statedef.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 1998/03/30 13:49:56 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERS
*  * AUTHOR
*  *    Henrik Voss + $Author: erik $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_statedef_h__
#define __mod_statedef_h__

Tuple GenStateDef(const TYPE_AS_StateDef &);
SEQ<TYPE_CPP_Stmt> GenInitialization(const TYPE_AS_CompositeType &, const TYPE_AS_StateInit &);
SEQ<TYPE_CPP_Stmt> InitStateVar(const TYPE_AS_Field &, const TYPE_AS_Expr &);
TYPE_CPP_Stmt GenUserDefInit();

#endif // __mod_statedef_h__

