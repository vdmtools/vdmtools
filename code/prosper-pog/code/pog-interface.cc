#include "pog-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "POG_uMEDIATOR.h"

static PogInterface thePogInterface;

PogInterface& GetPogInterface() {
  return thePogInterface;
};


void  PogInterface::setup() {
    vdm_POG_uMEDIATOR_setupPOG();
}

void  PogInterface::genPOForTypeDef (int index){
    vdm_POG_uMEDIATOR_GenPOForTypeDef(Int(index));
}

void PogInterface::genPOForFnDef (int index){
    vdm_POG_uMEDIATOR_GenPOForFnDef(Int(index));
}

void PogInterface::genPOForValueDef (){
    vdm_POG_uMEDIATOR_GenPOForValueDef();
}



int PogInterface::getNumberOfPO() {
    return vdm_POG_uMEDIATOR_GetNumberOfPO().GetValue();  
}

string PogInterface::getPO(int index) {
    return Seq2Str(vdm_POG_uMEDIATOR_GetPO(Int(index)));
}
string PogInterface::getPOStatus(int index) {
    return Seq2Str(vdm_POG_uMEDIATOR_GetPOStatus(Int(index)));
}
string PogInterface::getPOName(int index) {
    return Seq2Str(vdm_POG_uMEDIATOR_GetPOName(Int(index)));
}
string PogInterface::getPOLocation(int index) {
    return Seq2Str(vdm_POG_uMEDIATOR_GetPOLocation(Int(index)));
}
int PogInterface::getPOLine(int index){
    return vdm_POG_uMEDIATOR_GetPOLine(Int(index)).GetValue();
}

int PogInterface::getPOCol(int index){
    return vdm_POG_uMEDIATOR_GetPOCol(Int(index)).GetValue();
}
string PogInterface::getPODueTo(int index){
    /*
      cout << "PogInterface::getPODueTo" << endl;
 cout << index << endl;
 cout << Int(index) << endl;
   Int cast_index(index);
  TYPE_POG_uMEDIATOR_string seq;
  string str;
  cout << index << endl;
  cout << cast_index.GetValue() << endl;
  seq = vdm_POG_uMEDIATOR_GetPODueTo(cast_index);
  cout << "so far" << endl;
  str = Seq2Str(seq);
  cout << str << endl;
  return str;
  */
  return Seq2Str(vdm_POG_uMEDIATOR_GetPODueTo(Int(index)));
}
string PogInterface::getPOClassification(int index){
  return Seq2Str(vdm_POG_uMEDIATOR_GetPOClassification(Int(index)));
}

void PogInterface::genPO (){
  //    cout << "PogInterface::genPO" << endl;
    vdm_POG_uMEDIATOR_GenPO ();
    //  cout << "PogInterface::genPO" << endl;
}

string PogInterface::getPrettyPO(){
    return Seq2Str (vdm_POG_uMEDIATOR_GetPrettyPO());
}

string PogInterface::getSpecWithPO(){
    return Seq2Str (vdm_POG_uMEDIATOR_GlueSpecWithPoLst());
}











