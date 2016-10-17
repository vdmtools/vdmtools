/***
*  * WHAT
*  *   
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/TPGEN_if.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 1998/07/03 05:21:10 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Lars T. Olsen +  $Author: hanne $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef _INCL_TPGEN_IF_H
#define  _INCL_TPGEN_IF_


#ifdef VDMSL
void TPGEN_AddModuleTypes(Record);
//Sequence TPGEN_GenTypeDefsForImportedTypes(Record);
#endif
#ifdef VDMPP
void TPGEN_AddClassTypes(Record);
Sequence TPGEN_GenObjRefClass(Record,Set);
#endif

void init_TPGEN();
//Record TPGEN_AS2RepType(Record);
Sequence TPGEN_GenFctOpDefIfDef(Record,Sequence);
Sequence TPGEN_GenCppTypeDef(Record, Record);
Set TPGEN_GenGlobalFiles(Record);
Sequence TPGEN_GenModuleCCIncludes();
Sequence TPGEN_GenModuleCCPart();
Sequence TPGEN_GenModuleHIncludes();
Sequence TPGEN_GenHIncludes(Set);
Sequence TPGEN_GenModuleHPart();
//Record TPGEN_GenVDMTpName(Record);
void TPGEN_InitState(Generic);
Record TPGEN_GenQName(Record);
Record TPGEN_GenQName2(Record,Record);
Generic TPGEN_GenQComp(Record, Generic);
Generic TPGEN_GenRQComp(Record, Generic);
//Record TPGEN_Rep2AsType(Record);
Generic TPGEN_Rep2CppTypeSpecifier(Record, Generic);
Token TPGEN_ConcatIds(Sequence );
Token TPGEN_Name2Id(Record);
Record TPGEN_PrefixGLOBALDef(Record);
Record TPGEN_GenCompositeTypeTagId(Record);

#endif



