////////////////////////////////////////////////////////////////////////
// $Id: dlclass.h,v 1.4 2006/04/20 07:38:35 vdmtools Exp $
// dlclass.h
// Interface header file for VDM++ dlclass'es. 
//////////////////////////////////////////////////////////////////////

#include "metaiv.h"

#ifndef _dlclass_h_
#define _dlclass_h_

#ifdef _MSC_VER
#define DLLFUN __declspec(dllexport)
#else
#define DLLFUN 
#endif // _MSC_VER

#include <sstream>

class DlClass;

extern "C" {
  /**
   * This method is used to create a class.
   * @param name Name of the class as a String (plain class, no packages!)
   * @returns Class instance if successful, else 0
   */
  DLLFUN DlClass* DlClass_new (const wchar_t* name);

  /**
   * This method is used to delete a class
   * @param c C++ pointer to the class
   * @returns true, if successful, 0 if not 
   * (anyway, we cannot do anything if an error occurs)
   */
  DLLFUN int DlClass_delete (DlClass* c);

  /**
   * Method used to call methods on a class. 
   * @param c Pointer to the class instance
   * @param name name of the method to be called as a wstring.
   * @param params Parameters to the call as MetaIV-value
   * @success reference to int, that indicates if the call was
   * successful (boolean)
   */
  DLLFUN Generic DlClass_call (DlClass* c, const wchar_t * name, const Sequence & params, int & success);
}

/**
* Interface for the DLClass
* We could return void-pointers instead, but if
* all used DLClasses inherit from DlClass, the
* customer can use dynamic_cast
*/
class DlClass
{
public:
  virtual Generic DlMethodCall (const wchar_t * n, const Sequence & p) = 0;

  virtual ~DlClass() {};
};

#endif // _dlclass_h_
