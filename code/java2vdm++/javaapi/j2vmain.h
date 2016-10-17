#ifndef __j2vmain_h__
#define __j2vmain_h__

#include "metaiv.h"
//#include <varargs.h>

//extern string wstring2string(wstring ws);
//extern wstring string2wstring(const string& s);
extern void Log (const char* format, ...);

#ifdef J2VLIB_LOG        
extern FILE* pLog;
#define LOG ::Log
#else
#define LOG 1 ? (void)0 : ::Log
#endif // J2VLIB_LOG 

#endif // __j2vmain_h__
