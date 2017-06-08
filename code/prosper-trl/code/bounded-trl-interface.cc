#include "bounded-trl-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "BOUNDED_uTRL_uMEDIATOR.h"

static BoundedTrlInterface theBoundedTrlInterface;

BoundedTrlInterface& GetBoundedTrlInterface() {
  return theBoundedTrlInterface;
};

void BoundedTrlInterface::Reset () {
    vdm_BOUNDED_uTRL_uMEDIATOR_Reset();
}

void BoundedTrlInterface::TranslateTypeDef(int index) {
    vdm_BOUNDED_uTRL_uMEDIATOR_TranslateTypeDef(Int(index));
}

void BoundedTrlInterface::TranslateFnDef(int index) {
    vdm_BOUNDED_uTRL_uMEDIATOR_TranslateFnDef(Int(index));
}

void BoundedTrlInterface::TranslateValueDef(){
    vdm_BOUNDED_uTRL_uMEDIATOR_TranslateValueDef();
}

void BoundedTrlInterface::ComputeDependencyOrderForTypeDefs()
{
    vdm_BOUNDED_uTRL_uMEDIATOR_ComputeDependencyOrderForTypeDefs();
}

void BoundedTrlInterface::ComputeDependencyOrderForFnDefs()
{
    vdm_BOUNDED_uTRL_uMEDIATOR_ComputeDependencyOrderForFnDefs();
}

string  BoundedTrlInterface::GetTranslation(){
    return Seq2Str(vdm_BOUNDED_uTRL_uMEDIATOR_GetTranslation());
}

string  BoundedTrlInterface::GetBackTranslation(){
    return Seq2Str(vdm_BOUNDED_uTRL_uMEDIATOR_GetBackTranslation());
}

string  BoundedTrlInterface::GetCounterModelPO(int poNumber,int list_len,string* varList ,bool* boolList) {
  type_cLL vdm_varList;
  type_bL vdm_boolList;

    cout << "BoundedTrlInterface::GetCounterModel..." << endl;
  
  for(int i=0; i<list_len; i++){
     vdm_varList.ImpAppend((Generic)(Sequence(varList[i])));
  }

  for(int j=0; j<list_len; j++){
     vdm_boolList.ImpAppend((Bool)boolList[j]);
  }
  /*
  delete varList;
  delete boolList;
  */
  cout << "done!" << endl;

  return Seq2Str(vdm_BOUNDED_uTRL_uMEDIATOR_GetCounterModelPO(poNumber,vdm_varList,vdm_boolList));
} 











