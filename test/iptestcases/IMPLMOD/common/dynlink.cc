/***
*  * WHAT
*  *    Dynamic linking extension in interpreter for implementation modules
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/test/iptestcases/IMPLMOD/common/dynlink.cc,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2000/07/24 18:46:17 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    IFAD VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: blackie $
*  * COPYRIGHT
*  *    (C) 1992 IFAD, Denmark
***/
/****************************************************************************/

#include <dlfcn.h>
#include <iostream.h>
#include "metaiv.h"

#define MODE 1    /* mode of dlopen  */


/***************************************************************
*  Load the DL Library for the VDM Module
*  returns an Handler to this module
***************************************************************/
void * LoadDLImplModule(char * filename)
{
  void * retval;

//  cout << "In Open Lib" << filename << "\n";
  if ((retval = dlopen(filename, MODE)) == NULL)
      cerr << "dlopen error " << dlerror() <<  filename;
// else cout << "dlopen okay "<< filename<<"  "<< retval << "\n";
      return(retval);    
}

/***************************************************************
*  Search for the VDN definition in the dynamic link library
*  returns the address of this function
***************************************************************/ 
void * FindDLImplSymbol(void * libhandle, char *symbolname)
{
    void * retval;

//    cout << "Search for: " << symbolname << "\n";
  if ((retval = dlsym(libhandle, symbolname)) == NULL)
//    if ((retval = dlsym(libhandle,"MyCos")) == NULL)
    cerr << "dlsym error" << dlerror()  << symbolname << "\n"; 
//    else
//       cout << "dlsym okay "<< retval << "\n";
    return (retval);
}
/***************************************************************
*  Close the specified dynamic link library
*  returns 0 if successfull, otherwise 1
***************************************************************/
int DlLibClose(void * libhandle)
  {
    int retval;
    if (retval = dlclose(libhandle))
       cerr << "dlclose error" << dlerror();
//    else
//      cout<<  " dlclose okay\n";
    return(retval);
  }

