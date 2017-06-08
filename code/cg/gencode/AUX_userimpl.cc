/***
*  * WHAT
*  *   
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/AUX_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 1998/09/18 15:19:34 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Lars T. Olsen +  $Author: jeppe $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "AS.h"

#if 0
Record vdmcg::AUX_CleanFlatType(Record r) {
  return g2hAS(vdm_AUX_CleanFlatType(h2gAS(r)));
}

Record vdmcg::AUX_FromAS2RepType(Generic r) {
  return g2hAS(vdm_AUX_FromAS2RepType(h2gAS(r)));
}

#endif

////////////////////////////////////////////
// OPTIMIZED INTERNAL FUNCTIONS
////////////////////////////////////////////

extern vdmcg * CGi_ObjPtr;

Record vdm_CGAUX_CleanFlatType(const Record & tp) {
  return h2gAS(CGi_ObjPtr->CleanFlatType(g2hAS(tp)));
}

Record vdm_CGAUX_FromAS2RepType(const Generic & tp) {
  return h2gAS(CGi_ObjPtr->FromAS2RepType(g2hAS(tp)));
}

