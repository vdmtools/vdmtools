/***
*  * WHAT
*  *    Standard include file for MFC applications.
*  *    
*  *    
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/stdafx.h,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2006/04/18 07:45:42 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined( __STD_AFX_H__ )
#define __STD_AFX_H__

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#endif // VC_EXTRALEAN

#ifndef WINVER
//#define WINVER 0x0400
#define WINVER 0x0500
#endif // WINVER

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif // _WIN32_WINNT

#ifdef _MSC_VER
#if (_MSC_VER >= 1400 )
#define VC_WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#define _AFXDLL 1
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#else // (_MSC_VER < 1400 )
#define _AFXDLL 1
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#endif // 
#endif // _MSC_VER

#endif // __STD_AFX_H__

