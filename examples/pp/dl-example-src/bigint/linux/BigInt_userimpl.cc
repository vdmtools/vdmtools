#include "bigint_dl.h"

type_ref_BigInt vdm_BigInt::vdm_plus (const type_ref_BigInt &vdm_i) 
{  
  type_ref_BigInt result (new vdm_BigInt);
  vdm_BigInt *vdm_r_ptr = ObjGet_vdm_BigInt(result),
             *vdm_i_ptr = ObjGet_vdm_BigInt(vdm_i);
  BigIntDL *argPtr = (BigIntDL*) vdm_i_ptr->BigInt_dlClassPtr,
           *thisPtr = (BigIntDL*) BigInt_dlClassPtr,
           *resPtr = (BigIntDL*) vdm_r_ptr->BigInt_dlClassPtr;
  resPtr->setVal(thisPtr->getVal() + argPtr->getVal());
  return result;
}

