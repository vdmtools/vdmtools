/***
*  * WHAT
*  *   
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/TPGEN_if.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:01 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Lars T. Olsen +  $Author: paulm $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "AS.h"


static void dummy() {
  type_cL a,b;
  SEQ<Char> c,d;
  SEQ<TYPE_REP_FieldRep> e,f;
  e = f;
  c = d;
  a = b;
}

#ifdef VDMSL

void vdmcg::TPGEN_AddModuleTypes(Record ) {
  // LOT is used ! so this is just a dummy function doing nothing
  //  vdm_TPGEN_AddModuleTypes(h2gAS(r));
}
/*
Sequence vdmcg::TPGEN_GenTypeDefsForImportedTypes(Record r) {
return g2hAS(vdm_TPGEN_GenTypeDefsForImportedTypes(h2gAS(r)));
}
*/
#endif

#ifdef VDMPP
void vdmcg::TPGEN_AddClassTypes(Record ) {
  // LOT IS being used -- A handcoded version should be used !!! 
  // vdm_TPGEN_AddClassTypes(h2gAS(c));
}

Sequence vdmcg::TPGEN_GenObjRefClass(Record classnm, Set inh) {
return g2hAS(vdm_TPGEN_GenObjRefClass(h2gAS(classnm),h2gAS(inh)));;
}
#endif

void vdmcg::init_TPGEN() {
  init_TPGEN();
}

/*
Record vdmcg::TPGEN_AS2RepType(Record r) {
  return g2hAS(vdm_TPGEN_AS2RepType(h2gAS(r)));
}
*/
Sequence vdmcg::TPGEN_GenCppTypeDef(Record r1, Record r2){
return g2hAS(vdm_TPGEN_GenCppTypeDef(h2gAS(r1),h2gAS(r2)));
}

Set vdmcg::TPGEN_GenGlobalFiles(Record r) {
  return g2hAS(vdm_TPGEN_GenGlobalFiles(h2gAS(r)));
}

Sequence vdmcg::TPGEN_GenModuleCCIncludes() {
  return g2hAS(vdm_TPGEN_GenModuleCCIncludes());
}
Sequence vdmcg::TPGEN_GenModuleCCPart() {
  return g2hAS(vdm_TPGEN_GenModuleCCPart());
}
Sequence vdmcg::TPGEN_GenModuleHIncludes() {
  return g2hAS(vdm_TPGEN_GenModuleHIncludes());
}

Sequence vdmcg::TPGEN_GenHIncludes(Set names) {
  return g2hAS(vdm_TPGEN_GenHIncludes(h2gAS(names)));  
}


Sequence vdmcg::TPGEN_GenModuleHPart() {
  return g2hAS(vdm_TPGEN_GenModuleHPart());
}

/*
Record vdmcg::TPGEN_GenVDMTpName(Record r) {
return g2hAS(vdm_TPGEN_GenVDMTpName(h2gAS(r)));
}
*/

void vdmcg::TPGEN_InitState(Generic g) {
   vdm_TPGEN_InitState(h2gAS(g));
}
/*
Record vdmcg::TPGEN_Rep2AsType(Record r) {
return g2hAS(vdm_TPGEN_Rep2AsType(h2gAS(r)));
}
*/
Generic vdmcg::TPGEN_Rep2CppTypeSpecifier(Record r, Generic atp) {
return g2hAS(vdm_TPGEN_Rep2CppTypeSpecifier(h2gAS(r), h2gAS(atp)));
}

Record vdmcg::TPGEN_GenCompositeTypeTagId(Record nm) {
 return g2hAS(vdm_TPGEN_GenCompositeTypeTagId(h2gAS(nm)));
}

////////////////////////////////////////////
// OPTIMIZED EXTERNAL FUNCTIONS
// These functions are handcoded versions of functions
// in the module
////////////////////////////////////////////

Token vdmcg::TPGEN_ConcatIds(Sequence il) {
  wstring res = L"";
  switch (il.Length()) {
  case 0 : break;
  case 1 : res = CleanIdentifier((Token) il.Hd());
           break;
  default:
           res = CleanIdentifier((Token) il.Hd()) + L"_" + TPGEN_ConcatIds(il.Tl()).GetValue();
           break;
  }
  return Token(res);
}

Token vdmcg::TPGEN_Name2Id(Record nm) {
  return TPGEN_ConcatIds(nm.GetField(1));
}

Record vdmcg::TPGEN_GenQName(Record n) {
  return g2hAS(vdm_TPGEN_GenQName(h2gAS(n)));
}

Record vdmcg::TPGEN_GenQName2(Record modn, Record n) {
  return g2hAS(vdm_TPGEN_GenQName2(h2gAS(modn),h2gAS(n)));
}

Generic vdmcg::TPGEN_GenQComp(Record clnm, Generic type) {
  return g2hAS(vdm_TPGEN_GenQComp(h2gAS(clnm), h2gAS(type)));
}
  
Generic vdmcg::TPGEN_GenRQComp(Record clnm, Generic type) {
  return g2hAS(vdm_TPGEN_GenRQComp(h2gAS(clnm), h2gAS(type)));
}

Record vdmcg::TPGEN_PrefixGLOBALDef(Record id) {
 wstring sid(((Token) id.GetField(1)).GetValue()); 
 return GenIdentifier(L"DEF_" + sid);
}

//extern Generic vdm_TPGEN_modnm;

Sequence vdmcg::TPGEN_GenFctOpDefIfDef(Record nm, Sequence cpp) {
  Record id;
  id = GenIdentifier( Id2String(GiveCurCName()) + L"_" + 
                      Id2String(GiveLastName(nm)));
  //  id = GenIdentifier(TPGEN_Name2Id(TPGEN_GenQName2(g2hAS(vdm_TPGEN_modnm),nm)).GetValue());
  Sequence res;
  res.ImpAppend(GenPreIfNotDef(TPGEN_PrefixGLOBALDef(id)));
  res.ImpConc(cpp);
  res.ImpAppend(GenPreEndIf());
  return res;
  //return g2hAS(vdm_TPGEN_GenFctOpDefIfDef( h2gAS(nm),h2gAS(cpp)));
}
