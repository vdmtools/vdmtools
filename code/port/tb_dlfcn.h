#ifndef TB_DLFCN_H
#define TB_DLFCN_H

#ifdef _MSC_VER
#include <windows.h>
//#include <stdafx.h>
#define DLL_HANDLE HINSTANCE
#define PROC_PTR FARPROC
#define DLOPEN( path )       LoadLibraryA ( TBWSTR::wstring2string(path).c_str() ) 
#define DLSYM( dllib, name ) GetProcAddress ( dllib, TBWSTR::wstring2string(name).c_str() )
#define DLCLOSE( dlLib )     FreeLibrary ( dlLib )
//#define DLERROR()            wstring( L"Dynamic Library Error" )
#define DLERROR()            GetLoadErrorStr()
#else
#ifdef DL_MODULE_ENABLED
#include <dlfcn.h>
#define DLL_HANDLE void*
#define PROC_PTR void*
#define DLOPEN( path )       dlopen ( TBWSTR::wstring2string(path).c_str(), RTLD_NOW )
#define DLSYM( dllib, name ) dlsym( dllib, TBWSTR::wstring2string(name).c_str() )
#define DLCLOSE( dlLIb )     dlclose ( dlLib )
#define DLERROR()            TBWSTR::string2wstring( dlerror() )
#else
#define DLL_HANDLE void*
#define PROC_PTR void*
#define DLOPEN( path )       NULL
#define DLSYM( dllib, name ) NULL
#define DLCLOSE( dlLIb )     0
#define DLERROR()            wstring( L"" )
#endif // DL_MODULE_ENABLED
#endif //_MSC_VER

#endif 
