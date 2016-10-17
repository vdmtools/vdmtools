/***
 *  * WHAT
 *  *    
 *  * FILE
 *  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/stringext.h,v $
 *  * VERSION
 *  *    $Revision: 1.9 $
 *  * DATE
 *  *    $Date: 2006/06/08 07:23:13 $
 *  * STATUS
 *  *    $State: Exp $
 *  * REFERENCES
 *  *    
 *  * PROJECT
 *  *    IDERS/AFRODITE
 *  * AUTHOR
 *  *    Erik Toubro Nielsen + $Author: vdmtools $
 *  * COPYRIGHT
 *  *    (C) Kyushu University
 ***/

#ifndef __stringext_h__
#define __stringext_h__

#include "metaiv.h"
#include <string>

int STR_split(const std::wstring &,
              std::wstring *,
              int str_arr_size,
              const wchar_t * splitset); 
Sequence STR_split(const std::wstring &,
                   const wchar_t* splitset); 

extern const wchar_t * STR_RXwhite, * STR_RXwhite_and_comma;

#endif // __stringext_h__
