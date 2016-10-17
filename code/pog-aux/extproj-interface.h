#ifndef __extproj_interface_h__
#define __extproj_interface_h__

#include <string>
     
class ExtProjectInterface
{
 public:
    void setup();
    int getNumberOfTypeDefs();
    int getNumberOfFnDefs();
    std::wstring getTypeDefName(int index);
    std::wstring getFnDefName(int index);    
};
     
ExtProjectInterface& GetExtProjectInterface();

#endif // __extproj_interface_h__
