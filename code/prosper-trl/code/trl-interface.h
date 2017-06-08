
#ifndef _trl_interface_h
#define _trl_interface_h

#include "tb_string.h"
     
class TrlInterface {
 public:
    void Reset ();
    void TranslateTypeDef(int index);
    void TranslateFnDef(int index);
    void TranslateValueDef();
    void ComputeDependencyOrderForTypeDefs();
    void ComputeDependencyOrderForFnDefs();
    string GetTranslation();
    string translation();
};
     
TrlInterface& GetTrlInterface();

#endif







