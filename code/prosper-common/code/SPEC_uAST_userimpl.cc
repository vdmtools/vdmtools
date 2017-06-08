/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/prosper-common/code/SPEC_uAST_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.1.1.1 $
*  * DATE
*  *    $Date: 2000/04/18 14:07:18 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    IDERS-
*  * PROJECT
*  *    IDERS Project EP8593
*  * AUTHOR
*  *    $Author: sunesen $
*  * LOG
*  *    $Log: SPEC_uAST_userimpl.cc,v $
*  *    Revision 1.1.1.1  2000/04/18 14:07:18  sunesen
*  *    directory prosper-common
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "tools.h"
#include "intconvquotes.h"

#include "AS.h"
#include "libman.h" // SetToSeq

TYPE_AS_Module vdm_SPEC_uAST_GetAst() {
  return h2gAS(GetASTs().Hd());
};

TYPE_AS_Name vdm_SPEC_uAST_GetModuleName() {
    TYPE_AS_Module modl      = (TYPE_AS_Module) GetASTs().Hd();
    TYPE_AS_Name   name      = modl.get_nm();

    return h2gAS(name);
}
    
TYPE_AS_TypeDef vdm_SPEC_uAST_GetTypeDef(const TYPE_AS_Name &name) {
    TYPE_AS_Module modl      = (TYPE_AS_Module) GetASTs().Hd();
    TYPE_AS_Definitions defs = (TYPE_AS_Definitions) modl.get_defs();	
    type_7AS_NameC10AS_TypeDefCM typem = defs.get_typem();
	
    return h2gAS(typem[ g2hAS(name)]);
};

Record vdm_SPEC_uAST_GetFnDef(const TYPE_AS_Name &name) {
    TYPE_AS_Module modl      = (TYPE_AS_Module) GetASTs().Hd();
    TYPE_AS_Definitions defs = (TYPE_AS_Definitions) modl.get_defs();	
    type_7AS_NameCdM fnm     = defs.get_fnm();
	
    return h2gAS(fnm[ g2hAS(name)]);
};


type_11AS_ValueDefCL vdm_SPEC_uAST_GetValueDefSeq() {
    TYPE_AS_Module modl         = (TYPE_AS_Module) GetASTs().Hd();
    TYPE_AS_Definitions defs    = (TYPE_AS_Definitions) modl.get_defs();	
    type_11AS_ValueDefCL valuem = defs.get_valuem();
    
    return h2gAS(valuem);
};


Sequence
MapToSeq (Map m)
{
  Sequence res_l;
  Generic e;
  for (int bb = m.First (e); bb; bb = m.Next (e))
    res_l.ImpAppend (e);

  return res_l;
}



type_7AS_NameCL vdm_SPEC_uAST_GetTypeDefNames() {
    TYPE_AS_Module modl      = (TYPE_AS_Module) GetASTs().Hd();
    TYPE_AS_Definitions defs = (TYPE_AS_Definitions) modl.get_defs();	
    type_7AS_NameC10AS_TypeDefCM typem = defs.get_typem();
    Sequence tpSeq = MapToSeq(typem);
	
    //    return tpSeq;
    //    Sequence tpSeq = SetToSeq(typem.Dom());
	
    return  (type_7AS_NameCL) h2gAS(tpSeq);
};

type_7AS_NameCL vdm_SPEC_uAST_GetFnDefNames() {  
    TYPE_AS_Module modl      = (TYPE_AS_Module) GetASTs().Hd();
    TYPE_AS_Definitions defs = (TYPE_AS_Definitions) modl.get_defs();	
    type_7AS_NameCdM fnm     = defs.get_fnm();
    Sequence fnSeq = MapToSeq(fnm);

    //return fnSeq;

    //    Sequence fnSeq = SetToSeq(fnm.Dom());

    return  (type_7AS_NameCL) h2gAS(fnSeq);
};

