// CorbaTest.h : main header file for the CORBATEST application
//

#if !defined(AFX_CORBATEST_H__CC9C1E68_EB17_11D1_9BA0_006097D383E7__INCLUDED_)
#define AFX_CORBATEST_H__CC9C1E68_EB17_11D1_9BA0_006097D383E7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "corba_client.h"


/////////////////////////////////////////////////////////////////////////////
// CCorbaTestApp:
// See CorbaTest.cpp for the implementation of this class
//

class CCorbaTestApp : public CWinApp
{
public:
	CCorbaTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCorbaTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCorbaTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORBATEST_H__CC9C1E68_EB17_11D1_9BA0_006097D383E7__INCLUDED_)
