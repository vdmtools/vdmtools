/***
*  * WHAT
*  *    Time stamp mechanism.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/diverse/date.h,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:05 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Michael Andersen + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#ifndef TIMESTAMP
#define TIMESTAMP 1

#ifdef __cplusplus
#include <string>
extern std::wstring date;
#else
extern wchar_t date[];
#endif

#endif // TIMESTAMP
