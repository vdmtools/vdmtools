
#ifndef _pog_interface_h
#define _pog_interface_h

#include "tb_string.h"
     
class PogInterface {
 public:
    void setup();

    void genPOForTypeDef (int index);
    void genPOForFnDef (int index);
    void genPOForValueDef ();
    
    int getNumberOfPO();
    string getPO(int index);
    string getPOStatus(int index);
    string getPOName(int index);
    string getPOLocation(int index);
    int getPOLine(int index);
    int getPOCol(int index);
    string getPODueTo(int index);
    string getPOClassification(int index);
    //string getPOLocClassification(int index);
    void genPO ();
    string getPrettyPO();
    string getSpecWithPO();
};
     
PogInterface& GetPogInterface();

#endif
