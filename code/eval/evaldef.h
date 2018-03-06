/***
*  * WHAT
*  *    Header for translation functions.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/evaldef.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2006/09/23 13:57:18 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __evaldef_h__
#define __evaldef_h__

#include "metaiv.h"
#include "AS.h"
#include "GLOBAL.h"
#include "semvalues.h"

class DEF
{
public:
  enum accessType {ACC_OP, ACC_FN, ACC_VAL, ACC_TP, ACC_INST };

public:
#ifdef VDMSL
  static TYPE_GLOBAL_SigmaDEFS TranslateDefinitions (const TYPE_AS_Name & mod_id,
                                                     const TYPE_AS_Definitions & defs);
  static TYPE_GLOBAL_SigmaEXP TranslateDLExportSig(const TYPE_AS_Name & mod_id,
                                                   const TYPE_AS_DLExportSig & dlexportsig,
                                                   const Generic & useslib);
  static TYPE_GLOBAL_SigmaMO EmptySigmaMO();
#endif // VDMSL
#ifdef VDMPP
  static MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> ReadClasses(const TYPE_AS_Class & cs);
  static TYPE_GLOBAL_SigmaClass EmptySigmaClass();
  static Map UpdateConstructors(const TYPE_AS_Name & cls,
                                const SEQ<TYPE_AS_Name> & supercls,
                                const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm,
                                const SEQ<TYPE_AS_InstAssignDef> & instvars);
#endif // VDMPP
  static TYPE_AS_Access RealAccess(const TYPE_AS_Access &access, accessType kind);

  static TYPE_SEM_VAL TransFN (const TYPE_AS_Name & mod_id, const TYPE_AS_FnDef & fn_def);
  static Map CreatePrePostFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_FnDef & fn_def,
                               const TYPE_AS_Name & overloadnm);
  static Map CreatePolyPrePostFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_FnDef & fn_def,
                                   const TYPE_AS_Name & overloadnm);
  static Map CreateMeasureFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_FnDef & fn_def,
                               const TYPE_AS_Name & overloadnm);
  static Map CreateExplMeasureFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_FnDef & fn_def,
                                   const TYPE_AS_Name & overloadnm);
  static Map CreateExtExplMeasureFns (const TYPE_AS_Name & mod_id,
                                      const TYPE_AS_FnDef & fn_def,
                                      const TYPE_AS_Name & overloadnm);
  static Map CreatePolyMeasureFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_FnDef & fn_def,
                                   const TYPE_AS_Name & overloadnm);
  static Map CreateExplPolyMeasureFns (const TYPE_AS_Name & mod_id,
                                       const TYPE_AS_FnDef & fn_def,
                                       const TYPE_AS_Name & overloadnm);
  static Map CreateExtExplPolyMeasureFns (const TYPE_AS_Name & mod_id,
                                          const TYPE_AS_FnDef & fn_def,
                                          const TYPE_AS_Name & overloadnm);
  static Tuple ImplicitTypeParams (const SEQ<TYPE_AS_PatTypePair> & partps);
  static TYPE_AS_Type ImplicitResType (const SEQ<TYPE_AS_NameType> & resnmtps);
#ifdef VDMPP
  static Tuple TransInstVars(const SEQ<TYPE_AS_InstanceVarDef> & instvardef_l);
#endif // VDMPP

private:
#ifdef VDMPP
  static Map MergeFnsOpsPolys(const TYPE_GLOBAL_SigmaClass & sigmacl);
  static MAP<TYPE_AS_Name,TYPE_AS_TypeDef> UpdateTypeDefs(
                             const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & typem,
                             const TYPE_AS_Name & curcls);
  static TYPE_AS_Type UpdateType(const TYPE_AS_Type & type, const TYPE_AS_Name & curcls);
#endif // VDMPP
#ifdef VDMSL
  static MAP<TYPE_AS_Name,TYPE_AS_TypeDef> GenStateTypeDef (const TYPE_AS_StateDef & sd);
  static Generic StateInvariant (const Generic & e);
#endif // VDMSL
#ifdef VDMPP
  static SET<TYPE_AS_Name> TransInheritance(const SEQ<TYPE_AS_Name> & inh_l);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> ExtractStaticMembers(
                                  const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm,
                                  const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm,
                                  const TYPE_GLOBAL_SigmaClass & sigma);
  static TYPE_SEM_VAL LookUpLocalFn(const TYPE_AS_Name & fnnm, const TYPE_GLOBAL_SigmaClass & sigma);
  static Map TransLocalRecSel(const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & tpdefs);
#endif // VDMPP
  static Tuple TransFnMap (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & FnMap);
  static TYPE_SEM_VAL TransExplFN(const TYPE_AS_Name & mod_id, const TYPE_AS_ExplFnDef& fn_def);
  static TYPE_SEM_VAL TransImplFN(const TYPE_AS_Name & mod_id, const TYPE_AS_ImplFnDef& fn_def);
  static TYPE_SEM_VAL TransExtExplFN(const TYPE_AS_Name & mod_id, const TYPE_AS_ExtExplFnDef& fn_def);
  static Map CreateExplPrePostFns (const TYPE_AS_Name & mod_id,
                                     const TYPE_AS_ExplFnDef & fn_def,
                                     const TYPE_AS_Name & overloadnm);
  static Map CreateExtExplPrePostFns (const TYPE_AS_Name & mod_id,
                                        const TYPE_AS_ExtExplFnDef & fn_def,
                                        const TYPE_AS_Name & overloadnm);
  static Map CreateImplPrePostFns (const TYPE_AS_Name & mod_id,
                                     const TYPE_AS_ImplFnDef & fn_def,
                                     const TYPE_AS_Name & overloadnm);
  static SEQ<TYPE_AS_Pattern> CreatePostParms(const SEQ<TYPE_AS_Name> & resnms);
  static Map CreateExplPolyPrePostFns (const TYPE_AS_Name & mod_id,
                                         const TYPE_AS_ExplFnDef & fn_def,
                                         const TYPE_AS_Name & overloadnm);
  static Map CreateExtExplPolyPrePostFns (const TYPE_AS_Name & mod_id,
                                            const TYPE_AS_ExtExplFnDef & fn_def,
                                            const TYPE_AS_Name & overloadnm);
  static Map CreateImplPolyPrePostFns (const TYPE_AS_Name & mod_id,
                                         const TYPE_AS_ImplFnDef & fn_def,
                                         const TYPE_AS_Name & overloadnm);
  static Tuple ImplicitResNameTypes (const SEQ<TYPE_AS_NameType> & resnmtps);
  static TYPE_AS_Type CreateFunctionPreType (
                                 const SEQ<TYPE_AS_Type> & tpdom,
                                 const TYPE_AS_Type & tprng,
                                 const SEQ<TYPE_AS_Parameters> & parms);
  static TYPE_AS_Type CreateFunctionPostType (
                                 const SEQ<TYPE_AS_Type> & tpdom,
                                 const SEQ<TYPE_AS_Type> & tprng,
                                 const SEQ<TYPE_AS_Parameters> & parms);
  static TYPE_AS_Type CreateFunctionMeasureType (
                                 const SEQ<TYPE_AS_Type> & tpdom,
                                 const TYPE_AS_Type & tprng,
                                 const SEQ<TYPE_AS_Parameters> & parms);
  static TYPE_AS_Type CreateOperationPreType (const SEQ<TYPE_AS_Type> & tpdom, const Generic & st_id);
  static TYPE_AS_Type CreateOperationPostType (
                                 const SEQ<TYPE_AS_Type> & tpdom,
                                 const SEQ<TYPE_AS_Type> & tprng,
                                 const Generic & st_id);
#ifdef VDMPP
  static MAP<TYPE_AS_Name,TYPE_GLOBAL_Overloaded> TransOverloaded(
                                const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm,
                                const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm);
#endif // VDMPP
  static Map TransOpMap (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & OpMap);
  static TYPE_SEM_OP TransOP (const TYPE_AS_Name & mod_id, const TYPE_AS_OpDef & Op);
  static TYPE_SEM_OP TransExplOP (const TYPE_AS_Name & mod_id, const TYPE_AS_ExplOpDef & Op);
  static TYPE_SEM_OP TransImplOP (const TYPE_AS_Name & mod_id, const TYPE_AS_ImplOpDef & Op);
  static TYPE_SEM_OP TransExtExplOP (const TYPE_AS_Name & mod_id, const TYPE_AS_ExtExplOpDef & Op);
  static Map CreateOperationPrePostFns (const TYPE_AS_Name & mod_id,
                                        const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm,
                                        const Generic & gst);
  static SEQ<TYPE_AS_Parameters> CreateOperationPreParms (const SEQ<TYPE_AS_Pattern> & parm, const Generic & gst);
  static SEQ<TYPE_AS_Parameters> CreateExplOperationPostParms (
                                   const SEQ<TYPE_AS_Pattern> & parm,
                                   const SEQ<TYPE_AS_Type> & resnmtps,
                                   const Generic & gst);
  static SEQ<TYPE_AS_Parameters> CreateExtExplOperationPostParms (
                                         const SEQ<TYPE_AS_Pattern> & parm,
                                         const SEQ<TYPE_AS_Name> & resnms,
                                         const Generic & gst);
  static SEQ<TYPE_AS_Parameters> CreateImplOperationPostParms (
                                         const SEQ<TYPE_AS_Pattern> & parm,
                                         const SEQ<TYPE_AS_Name> & resnms,
                                         const Generic & gst);
  static TYPE_AS_RecordPattern CreateSigmaPattern (const TYPE_AS_CompositeType & comp_t, bool old_names);
  static Map CreateExplOpFns (const TYPE_AS_Name & mod_id,
                                const TYPE_AS_ExplOpDef & op_def,
                                const Generic & gst);
  static Map CreateExtExplOpFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_ExtExplOpDef & op_def,
                                   const Generic & gst);
  static Map CreateImplOpFns (const TYPE_AS_Name & mod_id,
                                const TYPE_AS_ImplOpDef & op_def,
                                const Generic & gst);
  static Map TransTpDefMap (const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & TpMap);
  static Map TransTP(const Set & typeset);
  static Tuple TCompT(const TYPE_AS_Name & id, const SEQ<TYPE_AS_Field> & fields, const TYPE_AS_Access &access);
#ifdef VDMSL
  static Map CreateStateInvAndInit(const TYPE_AS_Name & mod_id, const TYPE_AS_StateDef & statedef);
#endif // VDMSL
  static Map CreateInvs (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tm);
};

#endif // __evaldef_h__
