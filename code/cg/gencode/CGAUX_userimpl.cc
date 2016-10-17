/***
*  * WHAT
*  *   
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/CGAUX_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.7 $
*  * DATE
*  *    $Date: 2006/03/17 08:29:12 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Lars T. Olsen +  $Author: vdmtools $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "AS.h"
#include "codegen_tools.h"
#include "intconvquotes.h"

////////////////////////////////////////////
// OPTIMIZED INTERNAL FUNCTIONS
////////////////////////////////////////////
// GiveLastName
// name : ( AS`Name | AS`OldName )
// -> AS`Id
TYPE_AS_Id vdm_CGAUX_GiveLastName(const Record & name)
{
  return GetVDMCG().GiveLastName(name);
}

// GenFileExt
// fnm : CPP`FileName
// -> CPP`FileName
TYPE_CPP_FileName vdm_CGAUX_GenFileExt(const TYPE_CPP_FileName & fnm)
{
  return GetVDMCG().GenFileExt(fnm);
}

// CleanIdentifier
// id : AS`Id
// ==> AS`Id
TYPE_AS_Id vdm_CGAUX_CleanIdentifier(const TYPE_AS_Id & id)
{
  return GetVDMCG().CleanIdentifier(id);
}

// StringNumber
// n : nat
// -> seq of char
type_cL vdm_CGAUX_StringNumber(const Int & n)
{
  return GetVDMCG().StringNumber(n);
}

// FindScope
// nm : AS`Name
// ==> int
Int vdm_CGAUX_FindScope(const TYPE_AS_Name & nm)
{
  return GetVDMCG().FindScope(nm);
}

#ifdef VDMPP
// IncludeClass
// nm : AS`Name
// ==> ()
void vdm_CGAUX_IncludeClass(const TYPE_AS_Name & nm)
{
  GetVDMCG().IncludeClass(nm);
}

// ConvAsAccess2CPP
// a : AS`Access
// -> [<PRIVATE>|<PROTECTED>|<PUBLIC>]
Generic vdm_CGAUX_ConvAsAccess2CPP(const TYPE_AS_Access & a)
{
  //return GetVDMCG().ConvAsAccess2CPP(a);
  return INT2Q::h2gAS(GetVDMCG().ConvAsAccess2CPP(INT2Q::g2hAS(a)));
}

// IsInstVar
// nm : AS`Name
// ==> bool
Bool vdm_CGAUX_IsInstVar(const TYPE_AS_Name & nm)
{
  return GetVDMCG().IsInstVar(nm);
}
#endif //VDMPP

#ifdef VDMSL
// GiveCurCName
// ==> AS`Id
TYPE_AS_Id vdm_CGAUX_GiveCurCName()
{
  return vdm_CGAUX_GiveLastName(GetVDMCG().GiveCurCASName());
}
#endif //VDMSL
