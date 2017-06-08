#include "trf-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "TRF_uMEDIATOR.h"

static TrfInterface theTrfInterface;

TrfInterface& GetTrfInterface() {
  return theTrfInterface;
};

void TrfInterface::Reset () {
    vdm_TRF_uMEDIATOR_Reset();
}

void TrfInterface::TransformTypeDef(int index) {
    vdm_TRF_uMEDIATOR_TransformTypeDef(Int(index));
}

void TrfInterface::TransformFnDef(int index) {
    vdm_TRF_uMEDIATOR_TransformFnDef(Int(index));
}

void TrfInterface::TransformValueDef(){
    vdm_TRF_uMEDIATOR_TransformValueDef();
}

string TrfInterface::GetTransformation(){
    return Seq2Str(vdm_TRF_uMEDIATOR_GetTransformation());
}

string TrfInterface::transform() {
    /*    cout << "TrfInterface::transform" << endl;
	  string str = Seq2Str(vdm_TRF_uMEDIATOR_Transform());
	  cout << str << endl;
	  return str;
    */
    return Seq2Str(vdm_TRF_uMEDIATOR_Transform());
}

     










