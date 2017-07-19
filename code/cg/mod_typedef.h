/***
*  * WHAT
*  *    Implementation of mod_typedef.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_typedef.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 1999/04/07 08:34:52 $
*  * STATUS
*  *    Under development $State: Exp $
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: jojo $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_typedef_h__
#define __mod_typedef_h__

Sequence GenInvEqOrdDef(const SEQ<TYPE_AS_TypeDef> &);

#ifdef VDMSL
TYPE_CPP_CPPAS GenInvEqOrdDecl(const SEQ<TYPE_AS_TypeDef> &);
#endif //VDMSL

#ifdef VDMPP
SEQ<TYPE_CPP_MemberDeclaration> GenInvEqOrdDecl(const TYPE_CPP_Name &, const SEQ<TYPE_AS_TypeDef> &);
#endif //VDMPP

#ifdef VDMSL
// TYPE_CPP_FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> GenVDMLibInit(
                                  const TYPE_AS_Name &,
                                  const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> &,
                                  const Generic &);
#endif //VDMSL

#endif // __mod_typedef_h__
