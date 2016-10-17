/***
*  * WHAT
*  *    Header for translation functions.
*  * FILE
*  *    $Source:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __evalmod_h__
#define __evalmod_h__

#ifdef VDMSL
#include "metaiv.h"
#include "AS.h"
#include "GLOBAL.h"

class MOD
{
public:
  static TYPE_GLOBAL_SigmaMO TranslateModule(const TYPE_AS_Module & mod_sig);
  static TYPE_GLOBAL_StateMap InitGS (const Generic & e);
  static TYPE_GLOBAL_ValueMap InitGV(const SEQ<TYPE_AS_ValueDef> & val_l, const TYPE_AS_Name & nm);
  static TYPE_GLOBAL_SigmaIMO TranslateDLModule(const TYPE_AS_DLModule & dlmod);
  static Map TransStaticRef(const Map & mods);

private:
  static MAP<TYPE_AS_Name,TYPE_AS_Name> BuildRenameMap (const Map & imp_m);
  static MAP<TYPE_AS_Name,TYPE_AS_Name> MakeRenamings (const TYPE_AS_Name & mod_nm,
                                                const MAP<TYPE_AS_Name,TYPE_AS_Name> & ren_m);
  static Generic BuildExportSig (const Generic & exp_sig,
                          const Map & efns, const Map & epfns, const MAP<TYPE_AS_Name,TYPE_AS_Name> & ren);
  static bool HasFnPrePostCond (const TYPE_AS_Name & name, const Map & efns, const Map & epfns);
  static Map GetPrePostFns (const TYPE_AS_Name & name, const Map & efns, const Map & epfns);
  static bool HasOpPrePostCond (const TYPE_AS_Name & name, const Map & efns);
  static Map GetPrePostOps (const TYPE_AS_Name & name, const Map & efns);
  static bool HasTpInvariant (const TYPE_AS_Name & name, const Map & efns);
  static Map GetTpInvariant (const TYPE_AS_Name & name, const Map & efns);
  static bool IsAllPatternName (const TYPE_AS_PatternBind & pat_p);
};
#endif // VDMSL
#endif //__evalmod_h__
