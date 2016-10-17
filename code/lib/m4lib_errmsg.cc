/***
*  * WHAT
*  *    Integer Record Tag definitions and error messages for
*  *    metaiv libraries
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/m4lib_errmsg.cc,v $
*  * VERSION
*  *    $Revision: 1.18 $
*  * DATE
*  *    $Date: 2006/04/20 07:38:36 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "m4lib_errmsg.h"
#include "metaiv.h"
#include <stdlib.h>

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

/**************************************************************************
*  All errors detected in the VDM C++ library causes a call to
*  ReportError. A short description of the error is recorded
*  in m4err, and the exception ELIB is raised.
**************************************************************************/
bool M4LibError::guimode = true;
//static bool guimode = true;
void M4LibError::SetGUIMode(bool mode)
{
  M4LibError::guimode = mode;
}

void M4LibError::ReportError(int ErrorNumber, const wstring & st)
{
  wchar_t cern[20];
  swprintf (cern, 19, L"ML_ERROR %d: ", ErrorNumber);
  wstring message = cern + st;
  wstring title (L"Internal error in VDM Library"); //+ tb_version.GiveStartMenuToolTitle ();
  M4LibError::Complain(title, message);
  VDMErrorHandle(VDM_ERR_LIB, 1); 
}

void M4LibError::Complain(const wstring & title, const wstring & message)
{
#ifdef _MSC_VER
  const char * c = getenv("VDMERRMSG"); 
  if ( (c != NULL) || !guimode )
    wcerr << title << endl << message << endl << flush;
  else
    MessageBoxW (NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
#else 
  wcerr << title << endl << message << endl << flush;
#endif // _MSC_VER
}
