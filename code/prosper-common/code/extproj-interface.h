
#ifndef _extproj_interface_h
#define _extproj_interface_h

#include "tb_string.h"
     
class ExtProjectInterface {
 public:
    void setup();
    int getNumberOfTypeDefs();
    int getNumberOfFnDefs();
    string getTypeDefName(int index);
    string getFnDefName(int index);    
};
     
ExtProjectInterface& GetExtProjectInterface();

#endif
