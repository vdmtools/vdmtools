// CorbaTestDlg.h : header file
//

#if !defined(AFX_CORBATESTDLG_H__CC9C1E6A_EB17_11D1_9BA0_006097D383E7__INCLUDED_)
#define AFX_CORBATESTDLG_H__CC9C1E6A_EB17_11D1_9BA0_006097D383E7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCorbaTestDlg dialog

class CCorbaTestDlg : public CDialog
{
// Construction
public:
	CCorbaTestDlg(CWnd* pParent = NULL);	// standard constructor

	// The CORBA handle to the VDM Toolbox
    VDMApplication_var app;
	VDMInterpreter_var interp;
	VDMFactory_var fact;

	void GenLampSet(VDM::Set_var);
	void DoControl(VDM::Quote_var);
	void DoLampTransition(VDM::Sequence_var);
	CBitmap BmpTable[8];

// Dialog Data
	//{{AFX_DATA(CCorbaTestDlg)
	enum { IDD = IDD_CORBATEST_DIALOG };
	CEdit	m_errorsEdit;
	CEdit	m_messageEdit;
	CButton	m_L3;
	CButton	m_L2;
	CButton	m_L1;
	CStatic	m_signal_bitmap;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCorbaTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCorbaTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void Onprimes();
	afx_msg void Onstop();
	afx_msg void Ondark();
	afx_msg void Ondrive();
	afx_msg void Onwarning();
	afx_msg void OncheckL2();
	afx_msg void OncheckL1();
	afx_msg void OncheckL3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORBATESTDLG_H__CC9C1E6A_EB17_11D1_9BA0_006097D383E7__INCLUDED_)
