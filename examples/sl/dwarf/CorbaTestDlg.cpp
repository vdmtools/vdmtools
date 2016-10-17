// CorbaTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CorbaTest.h"
#include "CorbaTestDlg.h"
#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCorbaTestDlg dialog

CCorbaTestDlg::CCorbaTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCorbaTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCorbaTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCorbaTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCorbaTestDlg)
	DDX_Control(pDX, ErrorsEdit, m_errorsEdit);
	DDX_Control(pDX, MessageEdit, m_messageEdit);
	DDX_Control(pDX, L3_check, m_L3);
	DDX_Control(pDX, L2_check, m_L2);
	DDX_Control(pDX, L1_check, m_L1);
	DDX_Control(pDX, signal_bitmap, m_signal_bitmap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCorbaTestDlg, CDialog)
	//{{AFX_MSG_MAP(CCorbaTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(bn_stop, Onstop)
	ON_BN_CLICKED(bn_dark, Ondark)
	ON_BN_CLICKED(bn_drive, Ondrive)
	ON_BN_CLICKED(bn_warning, Onwarning)
	ON_BN_CLICKED(L2_check, OncheckL2)
	ON_BN_CLICKED(L1_check, OncheckL1)
	ON_BN_CLICKED(L3_check, OncheckL3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCorbaTestDlg message handlers

BOOL CCorbaTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	// Initialize CORBA:
    // Initialise the ORB and BOA. Consult corba_client.{h,cc} and the
    // omniORB2 user maual for details on how this is done.
    init_corba(0, NULL);
    if(!get_app(app, 0, NULL)){
      AfxMessageBox("Cannot invoke on a nil object reference.");
      exit(0);
	}
	
	fact = app->GetVDMFactory();

	VDMProject_var prj = app->GetProject();

	char *res, prjnm[256];
    if ((res = getcwd (NULL, 256)) != NULL)
		sprintf(prjnm,"%s\\dwarf.prj",res);
	else
		sprintf(prjnm,"dwarf.prj");
	try{
	  prj->Open(prjnm);
	  // Get a handle to the interpreter:
	  interp = app->GetInterpreter();
	  interp->Verbose(true);
	  // Initialise the Toolbox:
	  interp->Init();
	  // Ready to make invocations to the Toolbox Spec.
	}
    catch(APIError &ex){
      cerr << "Caught API error " << (char*)ex.msg << "\n";
	}
    catch(CORBA::COMM_FAILURE &ex) {
      cerr << "Caught system exception COMM_FAILURE, unable to contact server" 
           << endl;
	}
    catch(omniORB::fatalException& ex) {
      cerr << "Caught omniORB2 fatalException" << endl;
	}

	BmpTable[0].LoadBitmap(Dark);
	BmpTable[1].LoadBitmap(L1);
	BmpTable[2].LoadBitmap(L2);
	BmpTable[3].LoadBitmap(Stop);
	BmpTable[4].LoadBitmap(L3);
	BmpTable[5].LoadBitmap(Warning);
	BmpTable[6].LoadBitmap(Drive);
	BmpTable[7].LoadBitmap(AllOn);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCorbaTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCorbaTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCorbaTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCorbaTestDlg::GenLampSet(VDM::Set_var s)
{
	VDM::Sequence_var ls = fact->mkSequence();
	int l[3] = {m_L1.GetCheck(),m_L2.GetCheck(),m_L3.GetCheck()}, i;
	for(i=0;i<3;i++){
		if(l[i]){
			char q[5];
			sprintf(q,"L%d",i+1);
			VDM::Quote_var qq = fact->mkQuote(q);
			s->Insert(qq);
			qq->destroy();
		}
	}
}

void CCorbaTestDlg::Onstop() 
{
	VDM::Quote_var q = fact->mkQuote("stop");
	DoControl(q);
	q->destroy();
}

void CCorbaTestDlg::Ondark() 
{
	VDM::Quote_var q = fact->mkQuote("dark");
	DoControl(q);
	q->destroy();
}


void CCorbaTestDlg::DoLampTransition(VDM::Sequence_var trace)
{
	for(int i=1; i<=trace->Length(); i++){
		VDM::Set_var lamp_s = VDM::Set::_narrow(trace->Index(i));
		VDM::Generic_var g;
		int bmp_index=0;
		// Compute the right index to the bitmap table, i.e. translate the set of
		// LampId's to an index from 0 to 7
		for(int j=lamp_s->First(g); j; j=lamp_s->Next(g)){
			VDM::Quote_var lamp = VDM::Quote::_narrow(g);
			string qval = lamp->GetValue();
			bmp_index += 1 << (qval[1] - '0' - 1); 
			g->destroy();
		}
		m_signal_bitmap.SetBitmap(BmpTable[bmp_index]);	
//		Sleep(500 * (i-trace->Length()) );
		lamp_s->destroy();
	}
}

void CCorbaTestDlg::DoControl(VDM::Quote_var q)
{
	VDM::Set_var s = fact->mkSet();
	GenLampSet(s); // Read the lamp failure settings

	VDM::Sequence_var arg_l = fact->mkSequence();
	
	// Construct the arguments for the call to the Control function of the spec.
	arg_l->ImpAppend(q);
	arg_l->ImpAppend(s);
	

	// First we check the pre condition of Control:
	char expr[512];
	sprintf(expr,"pre_Control(%s, %s, Dwarf)", q->ascii(), s->ascii());
	VDM::Bool_var pre = VDM::Bool::_narrow(interp->EvalExpression(expr));

//	VDM::Bool_var pre = VDM::Bool::_narrow(interp->Apply("pre_Control", arg_l));
	if(pre->GetValue()){

		// Call the interpreter through the Apply function:
		VDM::Generic_var res = interp->Apply("Control", arg_l);

		if(res->IsTuple()){
			VDM::Tuple_var tp = VDM::Tuple::_narrow(res);
			VDM::Sequence_var trace = VDM::Sequence::_narrow(tp->GetField(3));

			m_messageEdit.SetWindowText("");
			m_errorsEdit.SetWindowText("");
			DoLampTransition(trace);
			trace->destroy();

			VDM::Generic_var temp;
			temp = tp->GetField(1);
			m_messageEdit.SetWindowText(temp->ascii());
			temp->destroy();

			temp = tp->GetField(2);
			m_errorsEdit.SetWindowText(temp->ascii());
			temp->destroy();
		}
		res->destroy();
		s->destroy();
		arg_l->destroy();
	}
	else{
		m_messageEdit.SetWindowText("Pre condition of 'Control' violated");
	}
	pre->destroy();
}

void CCorbaTestDlg::Ondrive() 
{
	VDM::Quote_var q = fact->mkQuote("drive");
	DoControl(q);
	q->destroy();
}

void CCorbaTestDlg::Onwarning() 
{
	VDM::Quote_var q = fact->mkQuote("warning");
	DoControl(q);
	q->destroy();
}


void CCorbaTestDlg::OncheckL2() 
{
	// TODO: Add your control notification handler code here
}

void CCorbaTestDlg::OncheckL3() 
{
	// TODO: Add your control notification handler code here

}

void CCorbaTestDlg::OncheckL1() 
{
	// TODO: Add your control notification handler code here

}
