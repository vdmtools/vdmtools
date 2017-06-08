/***
*  * WHAT
*  *    Implementation of preliminary methods in class Dependency
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/Dependency_userimpl.cc,v $
*  * VERSION
*  *    $Revision:
*  * DATE
*  *    $Date: 2001/06/12 15:04:56 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    IFAD-VDM28
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "intconvquotes.h"

#include "AS.h"
#ifdef VDMPP
#include "DEP.h"
#endif //VDMPP
#include "StateStore.h"

#include "Dependency.h"
#include "projectval.h"
#include "tbutils.h"

void vdm_Dependency::vdm_Init (const type_ref_Repository & r)
{
#ifdef VDMPP
  init_DEP();
#endif //VDMPP
  vdm_rep = r;
}

void vdm_Dependency::vdm_Depend()
{
#ifdef VDMPP
  vdm_DEP_Update();
#endif // VDMPP
}

type_23ProjectTypes_ModuleNameCS vdm_Dependency::vdm_GetAllSubClasses(const type_23ProjectTypes_ModuleNameCS & nm_s)
{
#ifdef VDMPP
  SET<TYPE_ProjectTypes_ModuleName> nm_s_q (nm_s);
  SET<TYPE_AS_Name> subclass_s;
  Generic g;
  for (bool bb = nm_s_q.First (g); bb; bb = nm_s_q.Next(g)) {
    subclass_s.ImpUnion (vdm_DEP_AllSubClasses (PTAUX::ModuleName2ASName (g)));
  }
  SET<TYPE_ProjectTypes_ModuleName> returnset_s (PTAUX::ASNameSet2ModuleNameSet (subclass_s));
  return returnset_s;
#else
  return SET<TYPE_ProjectTypes_ModuleName>();
#endif //VDMPP
}

type_23ProjectTypes_ModuleNameCS  vdm_Dependency::vdm_GetAllUsedBy (const type_23ProjectTypes_ModuleNameCS & nm_s)
{
#ifdef VDMPP
  SET<TYPE_AS_Name> usedby_s;
  SET<TYPE_ProjectTypes_ModuleName> nm_s_q (nm_s);
  Generic g;
  for (bool bb = nm_s_q.First (g); bb; bb = nm_s_q.Next(g)) {
    usedby_s.ImpUnion (vdm_DEP_AllClients (PTAUX::ModuleName2ASName (g)));
  }
  SET<TYPE_ProjectTypes_ModuleName> returnset_s (PTAUX::ASNameSet2ModuleNameSet (usedby_s));
  return returnset_s;
#else
  return SET<TYPE_ProjectTypes_ModuleName> ();
#endif //VDMPP
}

void vdm_Dependency::vdm_UpdateGraph (const type_19ProjectTypes_ModuleCL & nm_l)
{
#ifdef VDMPP
  // Sequence of Modules
  SEQ<TYPE_AS_Class> depupd;
  size_t len_nm_l = nm_l.Length();
  for (size_t idx = 1; idx <= len_nm_l; idx++)
  {
    const TYPE_ProjectTypes_Module & mod (nm_l[idx]);
    Generic ast (PTAUX::ExtractModuleAst (mod));
    depupd.ImpAppend (INT2Q::h2gAS (ast));
  }
  vdm_DEP_UpdateSC(depupd);
#endif //VDMPP
}

void vdm_Dependency::vdm_Remove (const type_23ProjectTypes_ModuleNameCS & nm_s)
{
#ifdef VDMPP
  SET<TYPE_ProjectTypes_ModuleName> nm_s_q (nm_s);
  SET<TYPE_AS_Name> asnm_s;
  Generic g;
  for (bool bb = nm_s_q.First (g); bb; bb = nm_s_q.Next (g))
    asnm_s.Insert (PTAUX::ModuleName2ASName (g));
  vdm_DEP_Remove (asnm_s);
#endif //VDMPP
}

type_23ProjectTypes_ModuleNameCS vdm_Dependency::vdm_GetSub (const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMPP
  SET<TYPE_AS_Name> res (vdm_DEP_GetSub (PTAUX::ModuleName2ASName (nm)));
  return PTAUX::ASNameSet2ModuleNameSet (res);
#else
  return SET<TYPE_ProjectTypes_ModuleName>();
#endif //VDMPP
}

type_23ProjectTypes_ModuleNameCS vdm_Dependency::vdm_GetSuper (const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMPP
  SET<TYPE_AS_Name> res (vdm_DEP_GetSuper (PTAUX::ModuleName2ASName (nm)));
  return PTAUX::ASNameSet2ModuleNameSet(res);
#else
  return SET<TYPE_ProjectTypes_ModuleName>();
#endif //VDMPP
}

type_23ProjectTypes_ModuleNameCS vdm_Dependency::vdm_GetUsedBy (const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMPP
  SET<TYPE_AS_Name> res (vdm_DEP_GetClients (PTAUX::ModuleName2ASName (nm)));
  return PTAUX::ASNameSet2ModuleNameSet(res);
#else
  return SET<TYPE_ProjectTypes_ModuleName>();
#endif //VDMPP
}

type_23ProjectTypes_ModuleNameCS vdm_Dependency::vdm_GetUses (const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMPP
  SET<TYPE_AS_Name> res (vdm_DEP_GetServers (PTAUX::ModuleName2ASName (nm)));
  return PTAUX::ASNameSet2ModuleNameSet(res);
#else
  return SET<TYPE_ProjectTypes_ModuleName>();
#endif //VDMPP
}

Bool vdm_Dependency::vdm_Save (const type_ref_StateStore & stor)
{
#ifdef VDMPP
  TYPE_DEP_DependencyGraph gr (vdm_DEP_ExportDepGraph());
  TYPE_ProjectTypes_DepGraph dep (PTAUX::mk_DepGraph (gr));
  Bool ok (ObjGet_vdm_StateStore (stor)->vdm_WriteDep (dep));//cast LTO ???
  return ok;
#else
  return Bool (true);
#endif //VDMPP
}

Bool vdm_Dependency::vdm_Load (const type_ref_StateStore & store)
{
#ifdef VDMPP
  Generic dep (ObjGet_vdm_StateStore (store)->vdm_ReadDep ());
  if (dep.IsRecord ()) {
    TYPE_DEP_DependencyGraph gr (PTAUX::ExtractDepGraph (dep));
    vdm_DEP_ImportDepGraph (gr);
    return Bool (true);
  }
  return Bool (false);
#else
  return Bool (true);
#endif //VDMPP
}

Bool vdm_Dependency::vdm_IsCyclic (const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMPP
  Bool cyc (vdm_DEP_IsCyclic (PTAUX::ModuleName2ASName (nm)));
  return cyc;
#else
  return Bool (false);
#endif // VDMPP
}

type_23ProjectTypes_ModuleNameCSL vdm_Dependency::vdm_OrderOfProcess(const TYPE_ProjectTypes_ModuleName & nm)
{
#ifdef VDMPP
  type_7AS_NameCSL asnm_l (vdm_DEP_OrderOfProcess(PTAUX::ModuleName2ASName (nm)));
  type_23ProjectTypes_ModuleNameCSL modnm_l;
  size_t len_asnm_l = asnm_l.Length();
  for (size_t idx = 1; idx <= len_asnm_l; idx++)
    modnm_l.ImpAppend (PTAUX::ASNameSet2ModuleNameSet (asnm_l[idx]));
  return modnm_l;  // seq of set of ProjectTypes`ModuleName
#else
  return type_23ProjectTypes_ModuleNameCSL();
#endif //VDMPP
}
  
