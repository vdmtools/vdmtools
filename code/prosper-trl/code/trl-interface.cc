#include "trl-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "TRL_uMEDIATOR.h"

static TrlInterface theTrlInterface;

TrlInterface& GetTrlInterface() {
  return theTrlInterface;
};

void TrlInterface::Reset () {
    vdm_TRL_uMEDIATOR_Reset();
}

void TrlInterface::TranslateTypeDef(int index) {
    vdm_TRL_uMEDIATOR_TranslateTypeDef(Int(index));
}

void TrlInterface::TranslateFnDef(int index) {
    vdm_TRL_uMEDIATOR_TranslateFnDef(Int(index));
}

void TrlInterface::TranslateValueDef(){
    vdm_TRL_uMEDIATOR_TranslateValueDef();
}

void TrlInterface::ComputeDependencyOrderForTypeDefs()
{
    vdm_TRL_uMEDIATOR_ComputeDependencyOrderForTypeDefs();
}

void TrlInterface::ComputeDependencyOrderForFnDefs()
{
    vdm_TRL_uMEDIATOR_ComputeDependencyOrderForFnDefs();
}

string TrlInterface::GetTranslation(){
    return Seq2Str(vdm_TRL_uMEDIATOR_GetTranslation());
}

string TrlInterface::translation() {
    return Seq2Str(vdm_TRL_uMEDIATOR_Translation());
}












