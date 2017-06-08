
#ifndef _trf_interface_h
#define _trf_interface_h

#include "tb_string.h"
     
class TrfInterface {
 public:
    void Reset ();
    void TransformTypeDef(int index);
    void TransformFnDef(int index);
    void TransformValueDef();
    string GetTransformation();
   string transform();
};
     
TrfInterface& GetTrfInterface();

#endif
