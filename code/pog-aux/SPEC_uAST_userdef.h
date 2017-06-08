#ifndef __SPEC_uAST_userdef_h__
#define __SPEC_uAST_userdef_h__

#include "main_tag.h"
#include "metaiv.h"
#include "AS.h"

#define DEF_SPEC_uAST_USERIMPL

#define DEF_SPEC_uAST_GetModuleName 1
#define DEF_SPEC_uAST_GetDefs 1
#define DEF_SPEC_uAST_GetTypeDefMap 1
#define DEF_SPEC_uAST_GetTypeDefMap_q 1
#define DEF_SPEC_uAST_GetTypeDef 1
#ifdef VDMSL
#define DEF_SPEC_uAST_GetStateDef 1
#define DEF_SPEC_uAST_GetStateDef_q 1
#endif // VDMSL
#define DEF_SPEC_uAST_GetFnDefMap 1
#define DEF_SPEC_uAST_GetFnDef 1
#define DEF_SPEC_uAST_GetOpDefMap 1
#define DEF_SPEC_uAST_GetOpDef 1
#define DEF_SPEC_uAST_GetValueDefSeq 1

#define DEF_SPEC_uAST_GetTypeDefNames 1
#define DEF_SPEC_uAST_GetFnDefNames 1
#ifdef VDMPP
#define DEF_SPEC_uAST_GetInstVars 1
#endif // VDMPP
#define DEF_SPEC_uAST_GetASTs 1

#ifdef DEF_SPEC_uAST_USERIMPL
class SPECAST
{
public:
  static Generic GetModuleName ();
  static SEQ<TYPE_AS_Definitions> GetDefs (const Generic & nm);
  static MAP<TYPE_AS_Name, TYPE_AS_TypeDef> GetTypeDefMap ();
  static MAP<TYPE_AS_Name, TYPE_AS_TypeDef> GetTypeDefMap_q (const TYPE_AS_Name & mnm);
#ifdef VDMSL
  static Record GetTypeDef (const TYPE_AS_Name & nm);
#endif // VDMSL
#ifdef VDMPP
  static TYPE_AS_TypeDef GetTypeDef (const TYPE_AS_Name & nm);
  static SEQ<TYPE_AS_InstanceVarDef> GetInstVars (const TYPE_AS_Name & modnm);
#endif // VDMPP
#ifdef VDMSL
  static Generic GetStateDef ();
  static Generic GetStateDef_q (const TYPE_AS_Name & mnm);
#endif // VDMSL
  static MAP<TYPE_AS_Name, TYPE_AS_FnDef> GetFnDefMap (const TYPE_AS_Name & modnm);
  static TYPE_AS_FnDef GetFnDef (const TYPE_AS_Name & nm);
  static MAP<TYPE_AS_Name, TYPE_AS_OpDef> GetOpDefMap (const TYPE_AS_Name & modnm);
  static TYPE_AS_OpDef GetOpDef (const TYPE_AS_Name & nm);
  static SEQ<TYPE_AS_ValueDef> GetValueDefSeq ();

  static Tuple SplitName (const TYPE_AS_Name & nm);
  static Generic GetASTs ();
  static SEQ<TYPE_AS_Name> GetTypeDefNames (const Generic & mnm);
  static SEQ<TYPE_AS_Name> GetFnDefNames (const Generic & mnm);

private:
  static Sequence MapToSeq (const Map & m);
  static Generic GetModule (const TYPE_AS_Name & nm, const Sequence & mods);
#ifdef VICE
  static TYPE_AS_ExplOpDef GetCPUConstructor(const TYPE_AS_Name & opnm);
  static TYPE_AS_ExplOpDef GetDeployOp(const TYPE_AS_Name & opnm);
  static TYPE_AS_ExplOpDef GetSetPriorityOp(const TYPE_AS_Name & opnm);
  static MAP<TYPE_AS_Name, TYPE_AS_OpDef> GetCPUOpDef();
  static TYPE_AS_Definitions GetCPUDefinitions();
  static TYPE_AS_Class GetCPUClassDef();
  static TYPE_AS_ExplOpDef GetBUSConstructor(const TYPE_AS_Name & opnm);
  static MAP<TYPE_AS_Name, TYPE_AS_OpDef> GetBUSOpDef();
  static TYPE_AS_Definitions GetBUSDefinitions();
  static TYPE_AS_Class GetBUSClassDef();
#endif // VICE
};
#endif // DEF_SPEC_uAST_USERIMPL

#endif // __SPEC_uAST_userdef_h__
