
#ifndef _bounded_trl_interface_h
#define _bounded_trl_interface_h

#include "tb_string.h"
     
class BoundedTrlInterface {
 public:
    void Reset ();
    void TranslateTypeDef(int index);
    void TranslateFnDef(int index);
    void TranslateValueDef();
    void ComputeDependencyOrderForTypeDefs();
    void ComputeDependencyOrderForFnDefs();
    string GetTranslation();
    string GetBackTranslation();
    string GetCounterModelPO(int poNumber,int list_len,string* varList ,bool* boolList);
};
     
BoundedTrlInterface& GetBoundedTrlInterface();

#endif







