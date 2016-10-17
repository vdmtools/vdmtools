/***
*  * WHAT
*  *    Header for translation functions.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/evalclass.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2006/09/23 10:59:01 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __evalclass_h__
#define __evalclass_h__

#include "metaiv.h"
#include "semvalues.h"
#include "AS.h"

class CLASS
{
#ifdef VDMSL
public:
  static TYPE_GLOBAL_ValueMap InitGV(const SEQ<TYPE_AS_ValueDef> & val_l, const TYPE_AS_Name & nm);
  static Map ExpandStaticRef(const Map & sref, const SET<TYPE_AS_Name> & done1);
#endif // VDMSL

#ifdef VDMPP
public:
  static SEQ<TYPE_AS_Type> ExtOpDom(const TYPE_AS_OpDef & opdef);
  static void GenInsMap(const TYPE_AS_Name & nm);
  static TYPE_GLOBAL_ValueMap InitGV(const SEQ<TYPE_AS_ValueDef> & val_l, const TYPE_AS_Name & nm);
  static void TransHierarchy();
  static void EvalInhStrct();
  static Map CreateConstructor(const TYPE_AS_Name& curcls,
                               const SEQ<TYPE_AS_InstAssignDef> & instvars,
                               const SEQ<TYPE_AS_Name> & supercls,
                               const  MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm);
  static void TransSyncs(const TYPE_AS_Document & cs);
  static Map TransLocalHchy(const TYPE_AS_Name & nm);

  static Map TransStaticRef(const MAP<TYPE_AS_Name, TYPE_GLOBAL_SigmaClass> & classes);
  static Map ExpandStaticRef(const Map & sref, const SET<TYPE_AS_Name> & done1);
private:
  static Map ExpandHierarchy(const Map & hchy1_p, const SET<TYPE_AS_Name> & done1);
  static SET<TYPE_AS_Name> ExpandNextLevel(const SET<TYPE_AS_Name> & to_exp,
                                           const Map & hchy,
                                           const SET<TYPE_AS_Name> & in_hchy);
  static SET<TYPE_AS_Name> ExpCl( const TYPE_AS_Name & nm, const Map & hchy_p, const SET<TYPE_AS_Name> & in_hchy);
  static SET<TYPE_AS_Name> NewlyDone(const Map & hchy, const SET<TYPE_AS_Name> & done);
  static bool OkHierarchy(const Map & clhchy);
  static Sequence OrderOfProcess(const TYPE_AS_Name & nm);
  static Sequence OrderOfProcess_Aux(const Sequence & order, const SET<TYPE_AS_Name> & to_process);
  static SEQ<TYPE_AS_Name> GetPermissionOrder(const TYPE_AS_Document & cs);
  static SEQ<TYPE_AS_Name> set2seq(const SET<TYPE_AS_Name> & elms);
  static void TransSyncsForOneClass(const TYPE_AS_Name & clnm, const Generic & defs_g);
  static Generic GetInhThread (const TYPE_AS_Name & clnm);
  static void TransThreadDef(const TYPE_AS_Name & clnm, const Generic & Def);
  static void TransSyncDef(const TYPE_AS_Name & nm,
                           const TYPE_AS_SyncDef & Def,
                           const SET<TYPE_AS_Name> & opnm_s,
                           const SET<TYPE_AS_Name> & supers);
  static void TransPermission(const TYPE_AS_Name & clsnm,
                              const TYPE_AS_Permission & perm,
                              const SET<TYPE_AS_Name> & opnm_s,
                              const SET<TYPE_AS_Name> & super_ops);
  static void InstallPermission(const TYPE_AS_Name & clnm,
                           const TYPE_AS_Name & opnm,
                           const TYPE_STKM_SubProgram & code,
                           const SET<TYPE_AS_Name> & local_ops);
  static TYPE_STKM_SubProgram MergePermission(const TYPE_STKM_SubProgram & prog1, const TYPE_STKM_SubProgram & prog2);
  static TYPE_STKM_SubProgram MergePermissionWithSupers(const TYPE_AS_Name & clnm,
                                                        const TYPE_AS_Name & opnm,
                                                        const TYPE_STKM_SubProgram & prog);
  static void TransMutex(const TYPE_AS_Name & clnm, const TYPE_AS_Mutex & mutex, const SET<TYPE_AS_Name> & local_ops);

#endif // VDMPP
};

#endif //__evalclass_h__
