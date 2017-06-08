/***
*  * WHAT
*  *   Wrapper file to replace GNU String with C++ std wstring.
*  * FILE
*  *    $Id: String.h,v 1.6 2005/07/07 06:03:40 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) 1996 The Institute of Applied Computer Science
***/

//#if defined(_MSC_VER) 
//#include "tb_port.h"
//#endif

// Get the wstring class.
#ifdef _MSC_VER
//#include <bwstring.h>
// Rename the erase method to remove. 
#include <string>

#define string_size_type size_t
#define string_npos string::npos
using namespace std; 
//#define tbswprintf(buf,size,str1,str2) swprintf(buf,str1,str2)
#else
#include <string>
#define _GLIBCPP_USE_WCHAR_T 1
#include <bits/stringfwd.h>
#define string_size_type string::size_type
#define string_npos string::npos
using namespace std;
//#define tbswprintf(buf,size,str1,str2) swprintf(buf,size,str1,str2)
#endif

// Make sure we get the real thing.
// #define String wstring
