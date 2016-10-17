#include "pog-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "SPEC_uAST.h"

static ExtProjectInterface theExtProjectInterface;

ExtProjectInterface& GetExtProjectInterface() {
  return theExtProjectInterface;
};


void  ExtProjectInterface::setup() {
    vdm_SPEC_uAST_setup();
}
int  ExtProjectInterface::getNumberOfTypeDefs(){
    return vdm_SPEC_uAST_GetNumberOfTypeDefs().GetValue();
}

int  ExtProjectInterface::getNumberOfFnDefs(){
    return vdm_SPEC_uAST_GetNumberOfFnDefs().GetValue();
}

string  ExtProjectInterface::getTypeDefName(int index){
    return Seq2Str(vdm_SPEC_uAST_GetTypeDefName(Int(index)));
}

string  ExtProjectInterface::getFnDefName(int index){
    return Seq2Str(vdm_SPEC_uAST_GetFnDefName(Int(index)));
}











