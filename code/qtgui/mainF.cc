/***
*  * WHAT
*  * FILE
*  *    $Source: /home/dmtools/cvsroot/toolbox/code/qtgui/mainF.cc,v $
*  * VERSION
*  *    $Revision: 1.175 $
*  *    $Date: 2006/06/30 06:48:33 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#if defined(_MSC_VER)
#define NOMINMAX
#include <windows.h>
#undef PURE  // Somewhere in MFC PURE is defined. This conflicts with
             // definitions in asquotes.h
#undef ASSERT
#undef State
#undef Name
#endif // defined(_MSC_VER)

#include "mainF.h"
#include "customevents.h"
#include "localeutil.h"

#ifndef USE_IMAGE_FILE
#include "imagedata.h"
#endif // USE_IMAGE_FILE

#ifdef VDMPP
#define USE_ROSE 1
#include "roseF.h"
#endif //VDMPP

#include "tooltips.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER
#include "tb_wstring.h"

#ifdef __linux__
#include <stdlib.h>
#endif // __linux__

#if QT_VERSION < 0x040000
#include <qregexp.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <qcursor.h>
#include <qworkspace.h>
#include <qeventloop.h>
#include <qinputdialog.h>
#endif // QT_VERSION < 0x040000

#if __cplusplus > 199711L
#include <thread>
#endif

#include "qtport.h"

#ifdef _MSC_VER
#define HOME_ENV_NAME "USERPROFILE"
#else
#define HOME_ENV_NAME "HOME"
#endif // _MSC_VER

#ifdef VDMSL
#define PROJECT_HISTORY_FILE ".vdmprj" 
#endif // VDMSL
#ifdef VDMPP
#define PROJECT_HISTORY_FILE ".vppprj" 
#endif // VDMPP

///////////////////////////////////////////////////////////////// 
// Global definitions
/////////////////////////////////////////////////////////////////
#ifdef VDMSL
char const * VDMROOT = "VDMSLROOT";
#endif //VDMSL
#ifdef VDMPP
char const * VDMROOT = "VDMPPROOT";
#endif //VDMPP

#ifdef _MSC_VER
// Functions for manipulating the Windows registry
wstring WinFormatMessage(long err);
bool FindRegEntry(wstring&, wstring&);
#endif //_MSC_VER

//extern void complain(const QString & title, const QString & message);
const int hinttime = 1500; //time to use in hints for non action items

///////////////////////////////////////////////////////////////// 
// Class definitions
/////////////////////////////////////////////////////////////////

//
// mainW::mainW
//
// main constructor
#if QT_VERSION >= 0x040000
mainW::mainW(QApplication &app) : QMainWindow( 0 )
#else
mainW::mainW(QApplication &app) : QMainWindow( 0, Qt2TB::GiveToolTitleI() )
#endif // QT_VERSION >= 0x040000
{
  this->toolRunning = true;
  //
  this->ow = NULL;
  this->browserw = NULL;
  this->codew = NULL;
  this->errorw = NULL;
  this->interpreterw = NULL;
  this->logw = NULL;
  this->tw = NULL;
  this->pogw = NULL;

  this->stopEnabled = false;

  //
  this->setApp(&app);

#if QT_VERSION >= 0x040000
  this->setWindowTitle(Qt2TB::GiveToolTitleI());
  this->setWindowIcon(QIcon(this->getImage("vdm_small.png", false)));
//  this->setWindowIcon(QIcon(this->getImage("vdm.png", false)));
//  this->setUnifiedTitleAndToolBarOnMac(true);
#else
  this->setCaption(Qt2TB::GiveToolTitleI());
  this->setIcon(this->getImage("vdm_small.png", false));
//  this->setIcon(this->getImage("vdm.png", false));
#endif // QT_VERSION >= 0x040000

  // Initialize "normal" instance variables
  this->tbThread = new ToolboxThread();

#ifdef VDMPP
  this->rosew = NULL;
#endif // VDMPP

  this->cleaningUp = false;
  if (!this->checkEnvVars(app)){
    this->envVarsOk = false;
    return;
  }
  else
    this->envVarsOk = true;

  this->projectName = "New Project";

  this->setLastDir("");

  //resize main window & set icon
#if QT_VERSION >= 0x040000
  const QRect rect = QApplication::desktop()->screenGeometry();
#if defined(  _MSC_VER )
  // for Windows 7
  QRect qr (rect.left() + 6,rect.top() + 30, rect.width() -12, rect.height() - 66);
#elif defined( __darwin__ )
  QRect qr (rect.left(),rect.top(), rect.width(), rect.height());
#elif defined( __linux__ ) || defined( __SunOS__ ) || defined( __Cygwin__ )
  QRect qr (rect.left(),rect.top(), rect.width() - 1, rect.height() - 1);
#endif
#else
#if defined(  _MSC_VER )
// 20080207
//  this->setGeometry(4,30,900,580);
  RECT rect;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
  Qrect qr(rect.left + 4, rect.top + 30, 900, 580);
  // for Windows 7 full
  //Qrect qr(rect.left + 6, rect.top + 30, rect.right - 12, rect.bottom - 36);
#elif defined( __darwin__ )
#if QT_VERSION >= 0x040600
  QRect qr(0, 0, 900, 600);
#else
  QRect qr(0, 44, 900, 604);
#endif // QT_VERSION >= 0x040600
#elif defined( __linux__ ) || defined( __SunOS__ )
  QRect qr(0, 0, 900, 580);
#endif
#endif // QT_VERSION >= 0x040000
  this->setGeometry(qr);

  // Create Status Bar
  QStatusBar* sb = QMainWindow::statusBar();

  this->statusBar = new QStatusBar(sb);
  this->statusBar->setSizeGripEnabled(false);
  sb->addWidget(statusBar, 4);

  this->statusLabel2 = new QLabel(sb);
  sb->addWidget(statusLabel2, 1);

  this->pb = new QProgressBar(sb);
  sb->addWidget(pb, 1);

#if QT_VERSION >= 0x040600
  this->menuBar()->setNativeMenuBar(false);
#endif // QT_VERSION >= 0x040600

  // Create menus
  this->createProjectMenu();
  this->createFileMenu();
  this->createWindowsMenu();
  this->createActionsMenu();
  this->createInterpreterMenu();
  this->createHelpMenu();

  // Create Doc Window
  this->createDocWindow();

  //create workspace
#if QT_VERSION >= 0x040000
  this->ws = new QMdiArea( this );
  this->setCentralWidget( this->ws );
#else
  QVBox* vb = new QVBox( this );
  vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  this->ws = new QWorkspace( vb );
  this->setCentralWidget( vb );
#endif // QT_VERSION >= 0x040000

  //create initial visible windows
  this->browserw = this->createBrowserWindow(this->ws);
  this->codew = this->createCodeWindow(this->ws);
  this->errorw = this->createErrorWindow(this->ws);
  this->interpreterw = this->createInterpreterWindow(this->ws);
  this->logw = this->createLogWindow(this->ws);
  this->pogw = this->createPogWindow(this->ws);
  this->searchw = this->createSearchWindow(this->ws);

  this->ow = this->createOptionsWindow( this );
  this->tw = this->createToolOptionsDialog( this );

  //ready to go
  this->statusLabel2->setText("Ready");
  this->toolRunning = false;

  this->browserw->setImageDir( this->imgDir );
  this->tw->setFontAndCodec();
  this->tw->setEditor();
#ifdef VDMPP
  this->tw->setUMLOptions();
#endif // VDMPP

  Qt2TB::setBlock( true );

  this->useCorba = false;

  this->repaint();
//  this->update();
  this->closing = false;

  this->isModified = false;
  this->mayBeModified = false;

//#ifdef _MSC_VER
//  this->logWrite(QString("rect.left: %1").arg(rect.left));
//  this->logWrite(QString("rect.top: %1").arg(rect.top));
//  this->logWrite(QString("rect.right: %1").arg(rect.right));
//  this->logWrite(QString("rect.bottom: %1").arg(rect.bottom));
//#endif // _MSC_VER
//  const QRect rec = QApplication::desktop()->screenGeometry();
//  this->logWrite(QString("rect.left: %1").arg(rec.left()));
//  this->logWrite(QString("rect.top: %1").arg(rec.top()));
//  this->logWrite(QString("rect.width: %1").arg(rec.width()));
//  this->logWrite(QString("rect.height: %1").arg(rec.height()));
}

void mainW::periodicAction()
{
}

void mainW::createDocWindow()
{
  if( this->projectTools == NULL || this->fileTools == NULL ||
      this->windowTools == NULL || this->actionTools == NULL ||
      this->interpreterTools == NULL || this->helpTools == NULL ) return;

#if QT_VERSION >= 0x040000
  this->insertToolBarBreak( this->actionTools );
#else
  this->addDockWindow( this->projectTools, tr("Project Operations") , Top, true);
  this->addDockWindow( this->actionTools, tr("Actions") , Top, false );
  this->addDockWindow( this->windowTools, tr("Window Operations") , Top, false );
  this->addDockWindow( this->fileTools, tr("File Operations") , Top, false );
  this->addDockWindow( this->interpreterTools, tr("Interpreter") , Top, false );
  this->addDockWindow( this->helpTools, tr("Help") , Top, false );
#endif // QT_VERSION >= 0x040000

  this->helpTools->hide();
}

QAction* mainW::addAction(const actionSpec & as, QMENU* menu, QToolBar* toolbar)
{
  if( as.accel == -1 ) // for separator
  {
#if QT_VERSION >= 0x040000
    menu->addSeparator();
#else
    menu->insertSeparator();
#endif // QT_VERSION >= 0x040000
    if( as.tool )
      toolbar->addSeparator();
    return NULL;
  }
  else if( as.accel == -2 )
    return NULL;

#if QT_VERSION >= 0x040000
  QIcon iconSet;
#else
  QIconSet iconSet;
#endif // QT_VERSION >= 0x040000
  if( as.imgfile != NULL )
  {
    QPixmap pm (this->getImage(as.imgfile, false));
    if( !pm.isNull() )
#if QT_VERSION >= 0x040000
      iconSet.addPixmap( pm, QIcon::Normal);
#else
      iconSet.setPixmap( pm, QIconSet::Automatic );
#endif // QT_VERSION >= 0x040000

    QString greyFileName (as.imgfile);
    greyFileName.replace(QRegExp(".png"), "_grey.png");
    QPixmap gpm (this->getImage(greyFileName, true));
    if( !gpm.isNull() )
#if QT_VERSION >= 0x040000
      iconSet.addPixmap( gpm, QIcon::Disabled);
#else
      iconSet.setPixmap( gpm, QIconSet::Automatic, QIconSet::Disabled);
#endif // QT_VERSION >= 0x040000
    else
#if QT_VERSION >= 0x040000
      iconSet.addPixmap( pm, QIcon::Disabled);
#else
      iconSet.setPixmap( pm, QIconSet::Automatic, QIconSet::Disabled);
#endif // QT_VERSION >= 0x040000
  }

  // create new action
  QAction *newAction = new QAction(this);
  if (!iconSet.isNull())

#if QT_VERSION >= 0x040000
    newAction->setIcon(iconSet);

#ifdef __darwin__
  QString str (as.text);
  if ((str == "&About") || (str == "About &Qt"))
    newAction->setText( str );
  else
    newAction->setText( mf(tr(as.text)) );
#else
  newAction->setText( mf(tr(as.text)) );
#endif // __darwin__

  newAction->setShortcut(as.accel);
  newAction->setStatusTip( tr(as.tooltip) );
#else
  newAction->setIconSet(iconSet);
  newAction->setMenuText( mf(tr(as.text)) );
  newAction->setAccel(as.accel);
#endif // QT_VERSION >= 0x040000

  newAction->setToolTip( tr(as.tooltip) );
  newAction->setWhatsThis( tr(as.tooltip) );

  // add new action to menubar
#if QT_VERSION >= 0x040000
  menu->addAction(newAction);
#else
  newAction->addTo(menu);
#endif // QT_VERSION >= 0x040000

  // add new action to toolbar
  if(as.tool)
#if QT_VERSION >= 0x040000
    toolbar->addAction(newAction);
#else
    newAction->addTo(toolbar);
#endif // QT_VERSION >= 0x040000

  return newAction;
}

QString mainW::mf( const QString & str )
{
#ifdef __darwin__
  QString ret ( str );
//#if QT_VERSION >= 0x040000
//  QRegExp rx2 ("&");
//#else
  QRegExp rx ("\\(&.\\)");
//#endif // QT_VERSION >= 0x040000
  ret.replace( rx, "" );
#if QT_VERSION >= 0x040000
  QRegExp rx2 ("&");
  ret.replace( rx2, "" );
#endif // QT_VERSION >= 0x040000
  return ret;
#else
  return str;
#endif // __darwin__
}

void mainW::createProjectMenu()
{
  ///////////////////////////////////////////////////////////////// 
  // Create project menu
  /////////////////////////////////////////////////////////////////
#if QT_VERSION >= 0x040000
  this->projectMenu = this->menuBar()->addMenu( mf(tr("&Project")) );
  this->projectTools = this->addToolBar( tr("Project Operations") );
  this->projectTools->setIconSize(QSize(24,24));
#else
  this->projectMenu = new QMENU( this );
  QMainWindow::menuBar()->insertItem( mf(tr("&Project")), this->projectMenu );
  this->projectTools = new QToolBar( NULL, tr("Project Operations") );
#endif // QT_VERSION >= 0x040000

  this->newProjectA = addAction(projectActions[0], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->newProjectA, SIGNAL(triggered(bool)), this, SLOT(newProject_qt4(bool)));
#else
  QObject::connect(this->newProjectA, SIGNAL(activated()), this, SLOT(newProject_qt3()));
#endif // QT_VERSION >= 0x040000

  this->loadProjectA = addAction(projectActions[1], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->loadProjectA, SIGNAL(triggered(bool)), this, SLOT(loadProject_qt4(bool)));
#else
  QObject::connect(this->loadProjectA, SIGNAL(activated()), this, SLOT(loadProject_qt3()));
#endif // QT_VERSION >= 0x040000

  this->saveProjectA = addAction(projectActions[2], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->saveProjectA, SIGNAL(triggered(bool)), this, SLOT(saveProject_qt4(bool)));
#else
  QObject::connect(this->saveProjectA, SIGNAL(activated()), this, SLOT(saveProject_qt3()));
#endif // QT_VERSION >= 0x040000

  this->saveAsProjectA = addAction(projectActions[3], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->saveAsProjectA,SIGNAL(triggered(bool)),this, SLOT(saveAsProject_qt4(bool)));
#else
  QObject::connect(this->saveAsProjectA,SIGNAL(activated()),this, SLOT(saveAsProject_qt3()));
#endif // QT_VERSION >= 0x040000

  addAction(projectActions[4], this->projectMenu, this->projectTools);

  this->addProjectA = addAction(projectActions[5], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->addProjectA, SIGNAL(triggered(bool)), this, SLOT(addProject_qt4(bool)));
#else
  QObject::connect(this->addProjectA, SIGNAL(activated()), this, SLOT(addProject_qt3()));
#endif // QT_VERSION >= 0x040000

  this->removeProjectA = addAction(projectActions[6], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->removeProjectA, SIGNAL(triggered(bool)), this, SLOT(removeProject_qt4(bool)));
#else
  QObject::connect(this->removeProjectA, SIGNAL(activated()), this, SLOT(removeProject_qt3()));
#endif // QT_VERSION >= 0x040000

  addAction(projectActions[7], this->projectMenu, this->projectTools);

  this->projectOptionsA = addAction(projectActions[8], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->projectOptionsA, SIGNAL(triggered(bool)), this, SLOT(projectOptions_qt4(bool)));
#else
  QObject::connect(this->projectOptionsA, SIGNAL(activated()), this, SLOT(projectOptions_qt3()));
#endif // QT_VERSION >= 0x040000

  this->toolOptionsA = addAction(projectActions[9], this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->toolOptionsA, SIGNAL(triggered(bool)), this, SLOT(toolOptions_qt4(bool)));
#else
  QObject::connect(this->toolOptionsA, SIGNAL(activated()), this, SLOT(toolOptions_qt3()));
#endif // QT_VERSION >= 0x040000

#if QT_VERSION >= 0x040000
  this->projectMenu->addSeparator();
#else
  this->projectMenu->insertSeparator();
#endif // QT_VERSION >= 0x040000

  this->createProjectHistoryMenu( this->projectMenu );

#ifndef __darwin__
#if QT_VERSION >= 0x040000
  this->projectMenu->addSeparator();
#else
  this->projectMenu->insertSeparator();
#endif // QT_VERSION >= 0x040000

  QAction * exitA = addAction(exitAction, this->projectMenu, this->projectTools);
#if QT_VERSION >= 0x040000
  QObject::connect(exitA, SIGNAL(triggered(bool)), this->appRef, SLOT(closeAllWindows()));
#else
  QObject::connect(exitA, SIGNAL(activated()), this->appRef, SLOT(closeAllWindows()));
#endif // QT_VERSION >= 0x040000
#endif // __darwin__

//  QObject::connect(menu, SIGNAL(highlighted(int)),this, SLOT(statusHint(int)));
}

void mainW::createProjectHistoryMenu(QMENU* menu)
{
#if QT_VERSION >= 0x040000
  QMenu * h = menu->addMenu( mf(tr( "Recent Pro&jects" )) );
  QObject::connect(h, SIGNAL(triggered(QAction *)), this, SLOT(projectHistory_qt4(QAction *)));
  QObject::connect(h, SIGNAL(hovered(QAction *)), this, SLOT(projectPath_qt4(QAction *)));
#else
  QPopupMenu * h = new QPopupMenu(this);
  menu->insertItem( mf(tr( "Recent Pro&jects" )), h );
  QObject::connect(h, SIGNAL(activated(int)), this, SLOT(projectHistory_qt3(int)));
  QObject::connect(h, SIGNAL(highlighted(int)), this, SLOT(projectPath_qt3(int)));
#endif // QT_VERSION >= 0x040000

  this->history = h;
}

void mainW::createFileMenu()
{
  ///////////////////////////////////////////////////////////////// 
  // Create file menu
  /////////////////////////////////////////////////////////////////
#if QT_VERSION >= 0x040000
  this->fileMenu = this->menuBar()->addMenu( mf(tr("&File")) );
  this->fileTools = this->addToolBar( tr("File Operations") );
  this->fileTools->setIconSize(QSize(24,24));
#else
  this->fileMenu = new QPopupMenu( this );
  QMainWindow::menuBar()->insertItem( mf(tr("&File")), this->fileMenu);
  this->fileTools = new QToolBar( NULL, tr("File Operations") );
#endif // QT_VERSION >= 0x040000

  this->newFileA = addAction(fileActions[0], this->fileMenu, this->fileTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->newFileA, SIGNAL(triggered(bool)), this, SLOT(newFile_qt4(bool)));
#else
  QObject::connect(this->newFileA, SIGNAL(activated()), this, SLOT(newFile_qt3()));
#endif // QT_VERSION >= 0x040000

  this->editA = addAction(fileActions[1], this->fileMenu, this->fileTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->editA, SIGNAL(triggered(bool)), this, SLOT(externalEditor_qt4(bool)));
#else
  QObject::connect(this->editA, SIGNAL(activated()), this, SLOT(externalEditor_qt3()));
#endif // QT_VERSION >= 0x040000

  this->closeA = addAction(fileActions[2], this->fileMenu, this->fileTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->closeA, SIGNAL(triggered(bool)), this, SLOT(fileClose_qt4(bool)));
#else
  QObject::connect(this->closeA, SIGNAL(activated()), this, SLOT(fileClose_qt3()));
#endif // QT_VERSION >= 0x040000

  this->closeAllA = addAction(fileActions[3], this->fileMenu, this->fileTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->closeAllA, SIGNAL(triggered(bool)), this, SLOT(fileCloseAll_qt4(bool)));
#else
  QObject::connect(this->closeAllA, SIGNAL(activated()), this, SLOT(fileCloseAll_qt3()));
#endif // QT_VERSION >= 0x040000

  addAction(fileActions[4], this->fileMenu, this->fileTools);

  this->selectAllA = addAction(fileActions[5], this->fileMenu, this->fileTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->selectAllA, SIGNAL(triggered(bool)), this, SLOT(selectAll_qt4(bool)));
#else
  QObject::connect(this->selectAllA, SIGNAL(activated()), this, SLOT(selectAll_qt3()));
#endif // QT_VERSION >= 0x040000
}

void mainW::createWindowsMenu()
{
  ///////////////////////////////////////////////////////////////// 
  // Create windows menu
  /////////////////////////////////////////////////////////////////
#if QT_VERSION >= 0x040000
  this->windowsMenu = this->menuBar()->addMenu( mf(tr("&Windows")) );
  this->windowTools = this->addToolBar( tr("Window Operations") );
  this->windowTools->setIconSize(QSize(24,24));
#else
  this->windowsMenu = new QPopupMenu( this );
  QMainWindow::menuBar()->insertItem( mf(tr("&Windows")), this->windowsMenu );
  this->windowTools = new QToolBar( NULL, tr("Window Operations") );
#endif // QT_VERSION >= 0x040000

  this->browserA = addAction(windowActions[0], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->browserA, SIGNAL(triggered(bool)), this, SLOT(browser_qt4(bool)));
#else
  QObject::connect(this->browserA, SIGNAL(activated()), this, SLOT(browser_qt3()));
#endif // QT_VERSION >= 0x040000

  this->codeInspectorA = addAction(windowActions[1], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->codeInspectorA, SIGNAL(triggered(bool)), this, SLOT(codeInspector_qt4(bool)));
#else
  QObject::connect(this->codeInspectorA, SIGNAL(activated()), this, SLOT(codeInspector_qt3()));
#endif // QT_VERSION >= 0x040000

  this->logA = addAction(windowActions[2], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->logA, SIGNAL(triggered(bool)), this, SLOT(log_qt4(bool)));
#else
  QObject::connect(this->logA, SIGNAL(activated()), this, SLOT(log_qt3()));
#endif // QT_VERSION >= 0x040000

  this->interpreterA = addAction(windowActions[3], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->interpreterA, SIGNAL(triggered(bool)), this, SLOT(interpreterWindow_qt4(bool)));
#else
  QObject::connect(this->interpreterA, SIGNAL(activated()), this, SLOT(interpreterWindow_qt3()));
#endif // QT_VERSION >= 0x040000

  this->errorListA = addAction(windowActions[4], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->errorListA, SIGNAL(triggered(bool)), this, SLOT(errorlist_qt4(bool)));
#else
  QObject::connect(this->errorListA, SIGNAL(activated()), this, SLOT(errorlist_qt3()));
#endif // QT_VERSION >= 0x040000

  this->pogListA = addAction(windowActions[5], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->pogListA, SIGNAL(triggered(bool)), this, SLOT(pog_qt4(bool)));
#else
  QObject::connect(this->pogListA, SIGNAL(activated()), this, SLOT(pog_qt3()));
#endif // QT_VERSION >= 0x040000

  this->searchListA = addAction(windowActions[6], this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->searchListA, SIGNAL(triggered(bool)), this, SLOT(search_qt4(bool)));
#else
  QObject::connect(this->searchListA, SIGNAL(activated()), this, SLOT(search_qt3()));
#endif // QT_VERSION >= 0x040000

#ifdef VDMPP
#if defined(_MSC_VER) || defined(USE_ROSE)
  this->roseA = addAction(roseAction, this->windowsMenu, this->windowTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->roseA, SIGNAL(triggered(bool)), this, SLOT(rose_qt4(bool)));
#else
  QObject::connect(this->roseA, SIGNAL(activated()), this, SLOT(rose_qt3()));
#endif // QT_VERSION >= 0x040000
#endif // _MSC_VER
#endif // VDMPP
}

void mainW::createActionsMenu()
{
  ///////////////////////////////////////////////////////////////// 
  // Create actions menu
  /////////////////////////////////////////////////////////////////
#if QT_VERSION >= 0x040000
  this->actionsMenu = this->menuBar()->addMenu( mf(tr("&Actions")) );
  this->actionTools = this->addToolBar( tr("Actions") );
  this->actionTools->setIconSize(QSize(24,24));
#else
  this->actionsMenu = new QPopupMenu( this );
  QMainWindow::menuBar()->insertItem( mf(tr("&Actions")), this->actionsMenu );
  this->actionTools = new QToolBar( NULL, tr("Actions") );
#endif // QT_VERSION >= 0x040000

  this->syntaxA = addAction(actionActions[0], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->syntaxA, SIGNAL(triggered(bool)), this, SLOT(syntax_qt4(bool)));
#else
  QObject::connect(this->syntaxA, SIGNAL(activated()), this, SLOT(syntax_qt3()));
#endif // QT_VERSION >= 0x040000

  this->typeA = addAction(actionActions[1], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->typeA, SIGNAL(triggered(bool)), this, SLOT(type_qt4(bool)));
#else
  QObject::connect(this->typeA, SIGNAL(activated()), this, SLOT(type_qt3()));
#endif // QT_VERSION >= 0x040000

  this->pogA = addAction(actionActions[2], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->pogA, SIGNAL(triggered(bool)), this, SLOT(genPOG_qt4(bool)));
#else
  QObject::connect(this->pogA, SIGNAL(activated()), this, SLOT(genPOG_qt3()));
#endif // QT_VERSION >= 0x040000

  this->cppA = addAction(actionActions[3], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->cppA, SIGNAL(triggered(bool)), this, SLOT(cpp_qt4(bool)));
#else
  QObject::connect(this->cppA, SIGNAL(activated()), this, SLOT(cpp_qt3()));
#endif // QT_VERSION >= 0x040000

  //this->cppA->setEnabled( true );

#ifdef VDMPP    
  this->javaA = addAction(actionActions[4], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->javaA, SIGNAL(triggered(bool)), this, SLOT(java_qt4(bool)));
#else
  QObject::connect(this->javaA, SIGNAL(activated()), this, SLOT(java_qt3()));
#endif // QT_VERSION >= 0x040000
  //this->javaA->setEnabled( true);
#endif //VDMPP

  this->prettyA = addAction(actionActions[5], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->prettyA, SIGNAL(triggered(bool)), this, SLOT(pretty_qt4(bool)));
#else
  QObject::connect(this->prettyA, SIGNAL(activated()), this, SLOT(pretty_qt3()));
#endif // QT_VERSION >= 0x040000

#ifdef VDMPP
  this->java2vdmA = addAction(actionActions[6], this->actionsMenu, this->actionTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->java2vdmA, SIGNAL(triggered(bool)), this, SLOT(java2vdm_qt4(bool)));
#else
  QObject::connect(this->java2vdmA, SIGNAL(activated()), this, SLOT(java2vdm_qt3()));
#endif // QT_VERSION >= 0x040000
  //this->java2vdmA->setEnabled( false );
#endif //VDMPP
}

void mainW::createInterpreterMenu()
{
  ///////////////////////////////////////////////////////////////// 
  // Create interpreter menu
  /////////////////////////////////////////////////////////////////
#if QT_VERSION >= 0x040000
  this->interpreterMenu = this->menuBar()->addMenu( mf(tr("&Interpreter")) );
  this->interpreterTools = this->addToolBar( tr("Interpreter") );
  this->interpreterTools->setIconSize(QSize(24,24));
#else
  this->interpreterMenu = new QPopupMenu( this );
  QMainWindow::menuBar()->insertItem( mf(tr("&Interpreter")), this->interpreterMenu );
  this->interpreterTools = new QToolBar( NULL, tr("Interpreter") );
#endif // QT_VERSION >= 0x040000

  this->initA = addAction(interpreterActions[0], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->initA, SIGNAL(triggered(bool)), this, SLOT(ipInit_qt4(bool)));
#else
  QObject::connect(this->initA, SIGNAL(activated()), this, SLOT(ipInit_qt3()));
#endif // QT_VERSION >= 0x040000

  this->stepA = addAction(interpreterActions[1], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->stepA, SIGNAL(triggered(bool)), this, SLOT(ipStep_qt4(bool)));
#else
  QObject::connect(this->stepA, SIGNAL(activated()), this, SLOT(ipStep_qt3()));
#endif // QT_VERSION >= 0x040000

  this->stepinA = addAction(interpreterActions[2], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->stepinA, SIGNAL(triggered(bool)), this, SLOT(ipStepin_qt4(bool)));
#else
  QObject::connect(this->stepinA, SIGNAL(activated()), this, SLOT(ipStepin_qt3()));
#endif // QT_VERSION >= 0x040000

  this->singleA = addAction(interpreterActions[3], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->singleA, SIGNAL(triggered(bool)), this, SLOT(ipSingle_qt4(bool)));
#else
  QObject::connect(this->singleA, SIGNAL(activated()), this, SLOT(ipSingle_qt3()));
#endif // QT_VERSION >= 0x040000

  this->continueA = addAction(interpreterActions[4], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->continueA, SIGNAL(triggered(bool)), this, SLOT(ipContinue_qt4(bool)));
#else
  QObject::connect(this->continueA, SIGNAL(activated()), this, SLOT(ipContinue_qt3()));
#endif // QT_VERSION >= 0x040000

  this->stopA = addAction(interpreterActions[5], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->stopA, SIGNAL(triggered(bool)), this, SLOT(ipStop_qt4(bool)));
#else
  QObject::connect(this->stopA, SIGNAL(activated()), this, SLOT(ipStop_qt3()));
#endif // QT_VERSION >= 0x040000

  this->upA = addAction(interpreterActions[6], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->upA, SIGNAL(triggered(bool)), this, SLOT(ipUp_qt4(bool)));
#else
  QObject::connect(this->upA, SIGNAL(activated()), this, SLOT(ipUp_qt3()));
#endif // QT_VERSION >= 0x040000

  this->downA = addAction(interpreterActions[7], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->downA, SIGNAL(triggered(bool)), this, SLOT(ipDown_qt4(bool)));
#else
  QObject::connect(this->downA, SIGNAL(activated()), this, SLOT(ipDown_qt3()));
#endif // QT_VERSION >= 0x040000

  this->finishA = addAction(interpreterActions[8], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->finishA, SIGNAL(triggered(bool)), this, SLOT(ipFinish_qt4(bool)));
#else
  QObject::connect(this->finishA, SIGNAL(activated()), this, SLOT(ipFinish_qt3()));
#endif // QT_VERSION >= 0x040000

  this->runA = addAction(interpreterActions[9], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->runA, SIGNAL(triggered(bool)), this, SLOT(run_qt4(bool)));
#else
  QObject::connect(this->runA, SIGNAL(activated()), this, SLOT(run_qt3()));
#endif // QT_VERSION >= 0x040000

  this->callLogA = addAction(interpreterActions[10], this->interpreterMenu, this->interpreterTools);
#if QT_VERSION >= 0x040000
  QObject::connect(this->callLogA, SIGNAL(triggered(bool)), this, SLOT(callLog_qt4(bool)));
#else
  QObject::connect(this->callLogA, SIGNAL(activated()), this, SLOT(callLog_qt3()));
#endif // QT_VERSION >= 0x040000

//  QObject::connect(this, SIGNAL(refreshButtons(bool)),
//                   list.at(8), SLOT(setEnabled(bool)));
}

void mainW::createHelpMenu()
{
  ///////////////////////////////////////////////////////////////// 
  // Create help menu
  /////////////////////////////////////////////////////////////////
#if QT_VERSION >= 0x040000
#ifdef __darwin__
  this->helpMenu = this->menuBar()->addMenu( tr("Help") );
#else
  this->helpMenu = this->menuBar()->addMenu( mf(tr("&Help")) );
#endif // __darwin__
  this->helpTools = this->addToolBar( tr("Help") );
  this->helpTools->setIconSize(QSize(24,24));
#else
  this->helpMenu = new QPopupMenu( this );
  QMainWindow::menuBar()->insertItem( mf(tr("&Help")), this->helpMenu );
  this->helpTools = new QToolBar( NULL, tr("Help") );
#endif // QT_VERSION >= 0x040000

  QAction * aboutA = addAction(helpActions[0], this->helpMenu, this->helpTools);
#if QT_VERSION >= 0x040000
  QObject::connect(aboutA, SIGNAL(triggered(bool)), this, SLOT(about_qt4(bool)));
#else
  QObject::connect(aboutA, SIGNAL(activated()), this, SLOT(about_qt3()));
#endif // QT_VERSION >= 0x040000

  QAction * aboutQtA = addAction(helpActions[1], this->helpMenu, this->helpTools);
#if QT_VERSION >= 0x040000
  QObject::connect(aboutQtA, SIGNAL(triggered(bool)), this, SLOT(aboutqt_qt4(bool)));
#else
  QObject::connect(aboutQtA, SIGNAL(activated()), this, SLOT(aboutqt_qt3()));
#endif // QT_VERSION >= 0x040000

/*
  int id=menu->insertItem( tr( "What's &This" ), this, SLOT(whatsThis()), SHIFT+Key_F1 );
  menu->setWhatsThis(id, tr( whatsthisText ) );
//  hintdict.insert(id, new QString( tr( whatsthisText ) ) );

  (void)QWhatsThis::whatsThisButton( helpTools );

  //needed because whatsThis isn't an action
  QObject::connect(menu,SIGNAL(highlighted(int)),this,SLOT(statusHint(int)));
*/
}

browserW * mainW::createBrowserWindow(QWidget * parent)
{
  browserW * w = new browserW(parent, "browserW", 0);

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage("browser.png", false)));
  QWidget * sw = ((QMdiArea *)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(0, 0, 320, 400);
#else
  w->setIcon(this->getImage("browser.png", false));
  w->setGeometry(0, 0, 320, 400);
#endif // QT_VERSION >= 0x040000

  QObject::connect(w, SIGNAL(br_selectAll(bool)), this, SLOT(selectAll_qt4(bool)));
  QObject::connect(w, SIGNAL(br_syntax(bool)), this, SLOT(syntax_qt4(bool)));
  QObject::connect(w, SIGNAL(br_typecheck(bool)), this, SLOT(type_qt4(bool)));
  QObject::connect(w, SIGNAL(br_integrity(bool)), this, SLOT(genPOG_qt4(bool)));
  QObject::connect(w, SIGNAL(br_cpp(bool)), this, SLOT(cpp_qt4(bool)));
  QObject::connect(w, SIGNAL(br_java(bool)), this, SLOT(java_qt4(bool)));
  QObject::connect(w, SIGNAL(br_j2v(bool)), this, SLOT(java2vdm_qt4(bool)));
  QObject::connect(w, SIGNAL(br_pretty(bool)), this, SLOT(pretty_qt4(bool)));
  QObject::connect(w, SIGNAL(br_option(bool)), this, SLOT(projectOptions_qt4(bool)));
  QObject::connect(w, SIGNAL(resetFilePosition(QString, QString, int, int, int)),
                   this, SLOT(resetFilePosition(QString, QString, int, int, int)));
  QObject::connect(w, SIGNAL(setMainWindowTitle(QString)), this, SLOT(setWindowTitle(QString)));
  QObject::connect(w, SIGNAL(logWrite(QString)), this, SLOT(logWrite(QString)));
  QObject::connect(w, SIGNAL(modified(QStringList)), this, SLOT(modified(QStringList)));
  return w;
}

codeW * mainW::createCodeWindow(QWidget * parent)
{
  codeW * w = new codeW( parent,"codeW",0);

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage( "source.png", false )));
  QWidget * sw = ((QMdiArea*)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(200, 30, 600, 400);
  sw->hide();
#else
  w->setIcon(this->getImage( "source.png", false ));
  w->setGeometry(200, 30, 600, 400);
  w->hide();
#endif // QT_VERSION >= 0x040000

  QObject::connect(w, SIGNAL(executeCommand(QString)), this, SLOT(executeCommand(QString)));
  QObject::connect(w, SIGNAL(ip_init(bool)), this, SLOT(ipInit_qt4(bool)));
  QObject::connect(w, SIGNAL(ip_cont(bool)), this, SLOT(ipContinue_qt4(bool)));
  QObject::connect(w, SIGNAL(ip_step(bool)), this, SLOT(ipStep_qt4(bool)));
  QObject::connect(w, SIGNAL(ip_stop(bool)), this, SLOT(ipStop_qt4(bool)));
  QObject::connect(w, SIGNAL(cd_editor(bool)), this, SLOT(externalEditor_qt4(bool)));
  QObject::connect(w, SIGNAL(logWrite(QString)), this, SLOT(logWrite(QString)));
  QObject::connect(w, SIGNAL(resetFilePosition(QString, QString, int, int, int)),
                   this, SLOT(resetFilePosition(QString, QString, int, int, int)));
  return w;
}

errorW * mainW:: createErrorWindow(QWidget * parent)
{
  errorW * w = new errorW( parent, "errorW", 0 );

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage( "error.png", false )));
  QWidget * sw = ((QMdiArea*)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(350, 250, 500, 200);
  sw->hide();
#else
  w->setIcon(this->getImage( "error.png", false ));
  w->setGeometry(350, 250, 500, 200);
  w->hide();
#endif // QT_VERSION >= 0x040000

  QObject::connect(w, SIGNAL(resetFilePosition(QString, QString, int, int, int)),
                   this, SLOT(resetFilePosition(QString, QString, int, int, int)));
  return w;
}

interpreterW * mainW:: createInterpreterWindow(QWidget * parent)
{
  interpreterW * w = new interpreterW(parent,"interpreterW",0);

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage( "interpreter.png", false )));
  QWidget * sw = ((QMdiArea*)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(50,50,600,400);
  sw->hide();
#else
  w->setIcon(this->getImage( "interpreter.png", false ));
  w->setGeometry(50,50,600,400);
  w->hide();
#endif // QT_VERSION >= 0x040000

  QObject::connect(w, SIGNAL(updateButtons()), this, SLOT(checkButtons()));
  QObject::connect(w, SIGNAL(resetFilePosition(QString, int, int)), this, SLOT(resetFilePosition(QString, int, int)));
  QObject::connect(w, SIGNAL(executeCommand(QString)), this, SLOT(executeCommand(QString)));
  QObject::connect(w, SIGNAL(logWrite(QString)), this, SLOT(logWrite(QString)));
  QObject::connect(w, SIGNAL(enableBr(int)), this, SLOT(enableBr(int)));
  QObject::connect(w, SIGNAL(disableBr(int)), this, SLOT(disableBr(int)));
  QObject::connect(w, SIGNAL(deleteBr(int)), this, SLOT(deleteBr(int)));
  QObject::connect(w, SIGNAL(saveInterpreterLog()), this, SLOT(saveInterpreterLog()));
  return w;
}

logW * mainW:: createLogWindow(QWidget * parent)
{
  logW * w = new logW(parent, "logW", 0);

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage( "log.png", false )));
  QWidget * sw = ((QMdiArea*)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(300,300,500,150);
  sw->hide();
#else
  w->setIcon(this->getImage( "log.png", false ));
  w->setGeometry(300,300,500,150);
  w->hide();
#endif // QT_VERSION >= 0x040000

  return w;
}

pogW * mainW:: createPogWindow(QWidget * parent)
{
  pogW * w = new pogW( this, parent, "pogW", 0);

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage( "integrity.png", false )));
  QWidget * sw = ((QMdiArea*)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(80,80,600,420);
  sw->hide();
#else
  w->setIcon(this->getImage( "integrity.png", false ));
  w->setGeometry(80,80,600,420);
  w->hide();
#endif // QT_VERSION >= 0x040000

  QObject::connect(w,    SIGNAL(resetFilePosition(QString, QString, int, int, int)),
                   this, SLOT(resetFilePosition(QString, QString, int, int, int)));
  QObject::connect(w, SIGNAL(logWrite(QString)), this, SLOT(logWrite(QString)));
  return w;
}

searchW * mainW:: createSearchWindow(QWidget * parent)
{
  searchW * w = new searchW( parent, "searchW", 0 );

#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage( "syntaxcheckdone.png", false )));
  QWidget * sw = ((QMdiArea*)parent)->addSubWindow(w);
  sw->setAttribute(Qt::WA_DeleteOnClose, false);
  sw->setGeometry(350, 0, 500, 300);
  sw->hide();
#else
  w->setIcon(this->getImage( "syntaxcheckdone.png", false ));
  w->setGeometry(350, 0, 500, 300);
  w->hide();
#endif // QT_VERSION >= 0x040000

  QObject::connect(w, SIGNAL(searchId(QString, bool, bool)), this, SLOT(searchId(QString, bool, bool)));
  QObject::connect(w,    SIGNAL(resetFilePosition(QString, QString, int, int, int)),
                   this, SLOT(resetFilePosition(QString, QString, int, int, int)));
//  QObject::connect(w,    SIGNAL(openFileWithEditor(QString, int, int)),
//                   this, SLOT(openEditor(QString, int, int)));
  return w;
}

optionsW * mainW::createOptionsWindow(QWidget * parent)
{
  optionsW * w = new optionsW(parent, "optionsW", true, 0);
#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage("projectoptions.png", false)));
#else
  w->setIcon(this->getImage("projectoptions.png", false));
#endif // QT_VERSION >= 0x040000
  return w;
}

toolOptionsDialog * mainW::createToolOptionsDialog(QWidget * parent)
{
  toolOptionsDialog * w = new toolOptionsDialog(parent, "toolOptinsDialog", true, 0);
#if QT_VERSION >= 0x040000
  w->setWindowIcon(QIcon(this->getImage("tooloptions.png", false)));  
#else
  w->setIcon(this->getImage("tooloptions.png", false));  
#endif // QT_VERSION >= 0x040000

  QObject::connect(w, SIGNAL(logWrite(QString)), this, SLOT(logWrite(QString)));
  QObject::connect(w, SIGNAL(setFont(QFont)), this, SLOT(setFont(QFont)));
  QObject::connect(w, SIGNAL(setTextCodec(QString)), this, SLOT(setTextCodec(QString)));
  QObject::connect(w, SIGNAL(enableEditor(bool)), this, SLOT(enableEditor(bool)));
  QObject::connect(w, SIGNAL(setMaxLogLines(int)), this, SLOT(setMaxLogLines(int)));
  QObject::connect(w, SIGNAL(setMaxInterpreterLogLines(int)), this, SLOT(setMaxInterpreterLogLines(int)));
  return w;
}

void mainW::disableGUI()
{
  this->stopTimer();
  //  logWrite("disableGUI");
//  this->setCursor(QCursor(Qt::waitCursor));
  if( NULL != this->browserw )
    this->browserw->setBusy(true);
  if( NULL != this->codew )
    this->codew->setBusy(true);
  if( NULL != this->logw )
    this->logw->setBusy(true);
  if( NULL != this->errorw )
    this->errorw->setBusy(true);
  if( NULL != this->pogw )
    this->pogw->setBusy(true);
  if( NULL != this->interpreterw )
    this->interpreterw->setBusy(true);
  //  setDisabled(true);

  this->newProjectA->setEnabled(false);
  this->loadProjectA->setEnabled(false);
  this->saveProjectA->setEnabled(false);
  this->saveAsProjectA->setEnabled(false);
  this->addProjectA->setEnabled(false);
  this->removeProjectA->setEnabled(false);
  this->projectOptionsA->setEnabled(false);
  this->toolOptionsA->setEnabled(false);

  this->history->setEnabled(false);

  this->syntaxA->setEnabled(false);
  this->typeA->setEnabled(false);
  this->pogA->setEnabled(false);
  this->cppA->setEnabled(false);
#ifdef VDMPP    
  this->javaA->setEnabled(false);
#endif //VDMPP
  this->prettyA->setEnabled(false);
#ifdef VDMPP
  this->java2vdmA->setEnabled(false);
  if (this->roseA != NULL)
    this->roseA->setEnabled(false);
#endif // VDMPP

  this->initA->setEnabled(false);
  this->stepA->setEnabled(false);
  this->stepinA->setEnabled(false);
  this->singleA->setEnabled(false);
  this->continueA->setEnabled(false);
  this->stopA->setEnabled(this->stopEnabled);
  this->upA->setEnabled(false);
  this->downA->setEnabled(false);
  this->finishA->setEnabled(false);

  this->statusLabel2->setText("Running...");
  this->toolRunning = true;
}

void mainW::enableGUI()
{
  //  setDisabled(false);
//  this->setCursor(QCursor(Qt::arrowCursor));
  if( NULL != this->browserw )
    this->browserw->setBusy(false);
  if( NULL != this->codew )
    this->codew->setBusy(false);
  if( NULL != this->logw )
    this->logw->setBusy(false);
  if( NULL != this->errorw )
    this->errorw->setBusy(false);
  if( NULL != this->pogw )
    this->pogw->setBusy(false);
  if( NULL != this->interpreterw )
    this->interpreterw->setBusy(false);

  this->newProjectA->setEnabled(true);
  this->loadProjectA->setEnabled(true);
  this->saveProjectA->setEnabled(true);
  this->saveAsProjectA->setEnabled(true);
  this->addProjectA->setEnabled(true);
  this->removeProjectA->setEnabled(true);
  this->projectOptionsA->setEnabled(true);
  this->toolOptionsA->setEnabled(true);

  this->history->setEnabled(true);

  this->syntaxA->setEnabled(true);
  this->typeA->setEnabled(true);
  this->pogA->setEnabled(true);
  this->cppA->setEnabled(true);
#ifdef VDMPP    
  this->javaA->setEnabled(true);
#endif //VDMPP
  this->prettyA->setEnabled(true);
#ifdef VDMPP
  this->java2vdmA->setEnabled(true);
  if (this->roseA != NULL)
    this->roseA->setEnabled(true);
#endif // VDMPP

  this->initA->setEnabled(true);
  this->stepA->setEnabled(true);
  this->stepinA->setEnabled(true);
  this->singleA->setEnabled(true);
  this->continueA->setEnabled(true);
  this->stopA->setEnabled(true);
  this->upA->setEnabled(true);
  this->downA->setEnabled(true);
  this->finishA->setEnabled(true);

  this->statusLabel2->setText("Ready");
#if QT_VERSION >= 0x040000
  this->statusBar->showMessage("");
#else
  this->statusBar->message("");
#endif // QT_VERSION >= 0x040000
  this->toolRunning = false;

  this->startTimer();
}

//
// mainW::~mainW
//actionActionRefs
// Class destructor
mainW::~mainW()
{
}

//
// mainW::getImage
//
// Auxiliary function 
QPixmap mainW::getImage(const QString& fname, bool optional)
{
#ifdef USE_IMAGE_FILE
  QPixmap img;
  if( !img.load(imgDir + DIRSEP + fname) && !optional )
    this->logWrite(QString("Warning: unable to read image file: ") + imgDir + DIRSEP + fname);
  return img;
#else
  QPixmap img (getImageData( fname ));
  if( img.isNull() && !optional )
    this->logWrite(QString("Warning: unable to read image data: ") + fname);
  return img;
#endif // USE_IMAGE_FILE
}

///////////////////// Slots ///////////////////// 

//project slots
//
// mainW::newProject()
//
#if QT_VERSION >= 0x040000
void mainW::newProject_qt4(bool)
#else
void mainW::newProject_qt4(bool)
{
}
void mainW::newProject_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->checkProjectIsSaved(); // 20051226
  this->sendCommand(new NewUnnamedProjectCMD());

//  this->clearWindows(true);

//  this->repaint();
  this->update();
}

//
// mainW::loadProject()
//
#if QT_VERSION >= 0x040000
void mainW::loadProject_qt4(bool)
#else
void mainW::loadProject_qt4(bool)
{
}
void mainW::loadProject_qt3()
#endif // QT_VERSION >= 0x040000
{ 
  this->checkProjectIsSaved(); // 20051226
  
  QString filter ("Project files (*.prj);; all (*.*)");
  QString files (QtPort::QtGetOpenFileName(this, tr("Open Project File..."), this->lastDir, filter));
  if (! files.isEmpty()) //at least one item 
  {
    this->loadProject( files );
  }
}

void mainW::loadProject(const QString & filename)
{ 
  this->clearWindows( false );

  this->browserw->newProject();

  this->loadWindowsGeometry(filename);
  this->openProject(filename);
  this->addProjectHistory( filename );

  if (this->browserw->isVisible())
  {
    this->browserw->parentWidget()->raise();
    this->browserw->setFocus();
  }

  if (Qt2TB::CallLog())
  {
    Qt2TB::ToggleCallLog();
    this->setCallLogMenu();
  }
}

void mainW::openProject(const QString & file)
{
  this->setLastDir(file);

  this->clearAllMessages();
  this->loading = true;
  // TODO: set codec
  QString cnm (this->ow->getCodecName(file));
  if (!cnm.isEmpty())
  {
    this->setEncoding(cnm);
    this->tw->setCurrentCodec(cnm);
  }
  //
  this->sendCommand(new LoadProjectCMD(file));
}

//
// mainW::saveProject()
//
#if QT_VERSION >= 0x040000
void mainW::saveProject_qt4(bool)
#else
void mainW::saveProject_qt4(bool)
{
}
void mainW::saveProject_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (Qt2TB::getProjectNameI().isNull())
  {
#if QT_VERSION >= 0x040000
    this->saveAsProject_qt4(true);
#else
    this->saveAsProject_qt3();
#endif // QT_VERSION >= 0x040000
  }
  else {
    Qt2TB::setBlock( false );
    int result = Qt2TB::saveProjectI();
    Qt2TB::setBlock( true );
    if (result == INT_ERROR) {
      QString nm (Qt2TB::getProjectNameI());
      QMessageBox::warning( this, "VDM++ Toolbox", QString( tr( saveErrorText ) ) + " " + nm);
    }
    this->saveWindowsGeometry(Qt2TB::getProjectNameI());
  }
}

//
// mainW::saveAsProject()
//
#if QT_VERSION >= 0x040000
void mainW::saveAsProject_qt4(bool)
#else
void mainW::saveAsProject_qt4(bool)
{
  saveAsProject_qt3();
}
void mainW::saveAsProject_qt3()
#endif // QT_VERSION >= 0x040000
{
// 20111108 -->
//  QStringList files(Qt2TB::getProjectFilesI());
//  if (!files.isEmpty())
// <-- 20111108
    this->saveProjectAs();
}

bool mainW::saveProjectAs()
{
  QString filter ("Project files (*.prj)");
  QString fileNm (QtPort::QtGetSaveFileName(this, tr("Save Project As..."), this->lastDir, filter));
  if (fileNm.isEmpty()) return false;

  // Add ".prj" suffix if the user hasn't provided it. FIXME: Make
  // this a user definable option in the future
#if QT_VERSION >= 0x040000
  bool appended = false;
#endif // QT_VERSION >= 0x040000
  if (fileNm.right(4) != ".prj") {
    fileNm.append(".prj");
#if QT_VERSION >= 0x040000
    appended = true;
#endif // QT_VERSION >= 0x040000
  }

#if QT_VERSION >= 0x040000
  if (appended) {
    QFile file (fileNm);
    QString confText (tr( confirmFileOverwriteText ) + " " + fileNm);
    if (file.open(QIODevice::ReadOnly)) {
      file.close();
      // File already exists
      switch( QMessageBox::information( 
                this, 
                Qt2TB::GiveToolTitleI(),
                confText,
                mf(tr( okText )), mf(tr( cancelText )),
                0,      
                1 )) {   
        case 1: {
          this->saveProjectAs();
          return false;
        }
      }
    }
  }
#else
  QFile file (fileNm);
  QString confText (tr( confirmFileOverwriteText ) + " " + fileNm);
  if (file.open(IO_ReadOnly)) {
    file.close();
    // File already exists
    switch( QMessageBox::information( 
              this, 
              Qt2TB::GiveToolTitleI(),
              confText,
              mf(tr( okText )), mf(tr( cancelText )),
              0,      
              1 )) {   
      case 1: {
        this->saveProjectAs();
        return false;
      }
    }    
  }
#endif // QT_VERSION >= 0x040000

  if (this->closing) {
    Qt2TB::setBlock( false );
    Qt2TB::saveProjectI(fileNm);
    Qt2TB::setBlock( true );
  }
  else {
    this->sendCommand(new SaveProjectAsCMD(fileNm));
  }

  this->setLastDir( fileNm );
  this->addProjectHistory( fileNm );
  this->saveWindowsGeometry( fileNm );
  if (this->ow != NULL) {
    this->ow->saveOptions();
  }
  return true;
}

void mainW::saveNewFileAs(const QString & clmodnm)
{
#ifdef VDMSL
  QString name ((clmodnm.isEmpty() ? QString("untitled") : clmodnm) + ".vdmsl");
#endif //VDMSL
#ifdef VDMPP
  QString name (clmodnm + ".vdmpp");
#endif // VDMPP
  QString prjnm (Qt2TB::getProjectNameI());
  QFileInfo fInfo (prjnm);
#if QT_VERSION >= 0x040000
  QString dirPath (fInfo.path() + "/" + name);
#else
  QString dirPath (fInfo.dirPath() + "/" + name);
#endif // QT_VERSION >= 0x040000
#ifdef VDMSL
  QString filter ("VDM-SL files (*.vdmsl *.vdm)");
#endif //VDMSL
#ifdef VDMPP
  QString filter ("VDM++ files (*.vdmpp *.vpp *.vdm)");
#endif // VDMPP
  QString filenm (QtPort::QtGetSaveFileName(this, tr("Save File As..."), dirPath, filter));
  if (filenm.isEmpty()) return;

#if QT_VERSION >= 0x040000
  bool appended = false;
#endif // QT_VERSION >= 0x040000
#ifdef VDMSL
  if ((filenm.right(4) != ".vdm") && (filenm.right(6) != ".vdmsl"))
  {
    filenm.append(".vdmsl");
#if QT_VERSION >= 0x040000
    appended = true;
#endif // QT_VERSION >= 0x040000
  }
#endif //VDMSL
#ifdef VDMPP
  if ((filenm.right(4) != ".vpp") && (filenm.right(6) != ".vdmpp"))
  {
    filenm.append(".vdmpp");
#if QT_VERSION >= 0x040000
    appended = true;
#endif // QT_VERSION >= 0x040000
  }
#endif // VDMPP

  QFile clmodFile(filenm);
  QString confText (tr( confirmFileOverwriteText ) + " " + filenm);
#if QT_VERSION >= 0x040000
  if (appended)
  {
    if (clmodFile.open(QIODevice::ReadOnly)) {
      clmodFile.close();
      // File already exists
      switch( QMessageBox::information( 
                this, 
                Qt2TB::GiveToolTitleI(),
                confText,
                mf(tr( okText )), mf(tr( cancelText )),
                0,      
                1 )) {   
        case 1: {
          this->saveNewFileAs(clmodnm);
          return;
        }
      }    
    }
  }
#else
  if (clmodFile.open(IO_ReadOnly)) {
    clmodFile.close();
    // File already exists
    switch( QMessageBox::information( 
              this, 
              Qt2TB::GiveToolTitleI(),
              confText,
              mf(tr( okText )), mf(tr( cancelText )),
              0,      
              1 )) {   
      case 1: {
        this->saveNewFileAs(clmodnm);
        return;
      }
    }    
  }
#endif // QT_VERSION >= 0x040000

  // create file
  if (this->createVDMFile(filenm, clmodnm)) {
    // add and syntax check new file
    QStringList files;
    files.append(filenm);
    this->sendCommand(new AddFilesCMD(files));
    this->sendSyntaxCommand(files);
  }
}

bool mainW::createVDMFile(const QString & filenm, const QString & clmodnm)
{
  QFile clmodFile(filenm);
#if QT_VERSION >= 0x040000
  if ( !clmodFile.open(QIODevice::WriteOnly) ) return false;
#else
  if ( !clmodFile.open(IO_WriteOnly) ) return false;
#endif // QT_VERSION >= 0x040000

  QTextStream clmodStream(&clmodFile);

  QString codecName (QTLOCALE::MenuNameToCodecName(this->ow->getCurrentCodecName()));

#if QT_VERSION >= 0x040000
#if QT_VERSION >= 0x050000
  QTextCodec* codec = QTextCodec::codecForName (codecName.toLatin1());
#else
  QTextCodec* codec = QTextCodec::codecForName (codecName.toAscii());
#endif // QT_VERSION >= 0x050000
#else
  QTextCodec* codec = QTextCodec::codecForName (codecName);
#endif // QT_VERSION >= 0x040000
  if (codec == NULL) return false;

  clmodStream.setCodec(codec);

  //bool tex = true;
  bool tex = false;

  if (tex) {
    if (!clmodnm.isEmpty()) {
      clmodStream << "\\subsection{" << clmodnm << "}" << endl;
    }
    else {
      clmodStream << "\\subsection{" << filenm << "}" << endl;
    }
    clmodStream << endl;
    clmodStream << "\\subsubsection{Specification in VDM}" << endl;
    clmodStream << endl;
    clmodStream << "\\begin{vdm_al}" << endl;
  }

#ifdef VDMSL
  if (!clmodnm.isEmpty()) {
    clmodStream << "--" << endl;
    clmodStream << "-- Module: " << clmodnm << endl;
    clmodStream << "--" << endl;
    clmodStream << "-- Description: " << endl;
    clmodStream << "--" << endl;
    clmodStream << "-- Version: " << endl;
    clmodStream << "-- Author: " << endl;
    clmodStream << "--" << endl;
    clmodStream << endl;
    clmodStream << "module " << clmodnm << endl;
    clmodStream << endl;
    clmodStream << "--imports" << endl;
    clmodStream << "--  from ModuleA all," << endl;
    clmodStream << "--  from ModuleB all" << endl;
    clmodStream << endl;
    clmodStream << "exports all" << endl;
    clmodStream << endl;
    clmodStream << "definitions" << endl;
    clmodStream << endl;
    clmodStream << "--state Sigma of" << endl;
    clmodStream << "--  a : nat" << endl;
    clmodStream << "--  b : nat" << endl;
    clmodStream << "--  inv mk_Sigma(x,y) == x >= y" << endl;
    clmodStream << "--  init s == s = mk_Sigma(0,0)" << endl;
    clmodStream << "--end" << endl;
    clmodStream << endl;
  }
#endif // VDMSL
#ifdef VDMPP
  clmodStream << "/**" << endl;
  clmodStream << "  " << clmodnm << "." << endl;
  clmodStream << endl;
  clmodStream << "  <p>" << endl;
  clmodStream << "  description" << endl;
  clmodStream << "  </p>" << endl;
  clmodStream << "" << endl;
  clmodStream << "  @version" << endl;
  clmodStream << "  @author" << endl;
  clmodStream << "*/" << endl;
  clmodStream << endl;
  clmodStream << "class " << clmodnm << endl;
  clmodStream << endl;
#endif // VDMPP
  clmodStream << "types" << endl;
  clmodStream << endl;
  clmodStream << "values" << endl;
  clmodStream << endl;
#ifdef VDMPP
  clmodStream << "instance variables" << endl;
  clmodStream << endl;
#endif // VDMPP
  clmodStream << "operations" << endl;
  clmodStream << endl;
#ifdef VDMPP
  clmodStream << "-- constructor for " << clmodnm << endl;
  clmodStream << "public " << clmodnm << ": () ==> " << clmodnm << endl;
  clmodStream << clmodnm << "() == ();" << endl;
  clmodStream << endl;
#endif // VDMPP
  clmodStream << "functions" << endl;
  clmodStream << endl;

#ifdef VDMSL
  if (!clmodnm.isEmpty()) {
    clmodStream << "end " << clmodnm << endl;
  }
#endif // VDMSL
#ifdef VDMPP
  clmodStream << "end " << clmodnm << endl;
#endif // VDMPP

  if (tex) {
    clmodStream << "\\end{vdm_al}" << endl;
    if (!clmodnm.isEmpty()) {
      clmodStream << endl;
      clmodStream << "\\subsubsection{Test Coverage}" << endl;
      clmodStream << endl;
      clmodStream << "\\begin{rtinfo}" << endl;
      clmodStream << "  [" << clmodnm << "]{vdm.tc}[" << clmodnm << "]" << endl;
      clmodStream << "\\end{rtinfo}" << endl;
    }
  }

  clmodFile.close();

  return true;
}

// project options is a modal temporary dialog
//
// mainW::projectOptions()
//
#if QT_VERSION >= 0x040000
void mainW::projectOptions_qt4(bool)
#else
void mainW::projectOptions_qt4(bool)
{
  projectOptions_qt3();
}
void mainW::projectOptions_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->ow->show();
}

QString mainW::getDirectory()
{
  QString res (QtPort::QtGetExistingDirectory(this, tr("Choose a working directory"), this->lastDir));
  if( !res.isNull() )
    this->lastDir = res;
  return res;
}

QString mainW::getXMIFile()
{
  QString filter ("XMI files (*.xml);; all (*.*)");
  QString file (QtPort::QtGetOpenFileName(this, tr("Open XMI File..."), this->lastDir, filter));
  if (!file.isEmpty()) //at least one item 
  {
    this->setLastDir(file);
  }
  return file;
}

//
// mainW::toolOptions()
//
#if QT_VERSION >= 0x040000
void mainW::toolOptions_qt4(bool)
#else
void mainW::toolOptions_qt4(bool)
{ // dummy
}
void mainW::toolOptions_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->tw->show();
}

void mainW::projectHistory_qt4( QAction * a )
{
#if QT_VERSION >= 0x040000
  this->checkProjectIsSaved();  // 20051226

  QString filename (a->whatsThis());

  QFileInfo fi( filename );
  if( !fi.exists() )
  {
    QString errmsg ( tr("Unnable to find the project:\n") );
    errmsg += filename;
    QMessageBox::warning( this, "Project", errmsg );
    {
      removeProjectHistory(filename);
    }
    return;
  }
  this->loadProject( filename );
#endif // QT_VERSION >= 0x040000
}

#if QT_VERSION < 0x040000
void mainW::projectHistory_qt3( int id )
{
  this->checkProjectIsSaved();  // 20051226

  int index = this->history->indexOf(id);
  if ( index == -1 ) return;
  QString filename (this->histAList.at(index)->whatsThis());

  QFileInfo fi( filename );
  if( !fi.exists() )
  {
    QString errmsg ( tr("Unnable to find the project:\n") );
    errmsg += filename;
    QMessageBox::warning( this, "Project", errmsg );
    {
      removeProjectHistory(filename);
    }
    return;
  }
  this->loadProject( filename );
}
#endif // QT_VERSION < 0x040000

void mainW::projectPath_qt4( QAction * a )
{
#if QT_VERSION >= 0x040000
  QString filename (a->whatsThis());
  this->statusBar->showMessage( filename, hinttime );
#endif // QT_VERSION >= 0x040000
}

#if QT_VERSION < 0x040000
void mainW::projectPath_qt3( int id )
{
  int index = this->history->indexOf(id);
  if ( index == -1 ) return;
  QString filename (this->histAList.at(index)->whatsThis());
  this->statusBar->message( filename, hinttime );
}
#endif // QT_VERSION < 0x040000

QString mainW::getFilter()
{
  QString filter;

  if (this->lastPostfix == "tex")
    filter += "Tex files (*.tex);;" ;
#ifdef VDMPP
  else if (this->lastPostfix == "java")
    filter += "Java files (*.java);;";
#endif // VDMPP
  else if (this->lastPostfix == "all")
    filter += "All Files (*);;";

#ifdef VDMSL
  filter += "VDM files (*.vdm *.vdmsl *.rtf *.debug *.opt nulloptions);;";
#endif //VDMSL
#ifdef VDMPP
#ifdef VICE
  filter += "VDM files (*.vpp *.vdmpp *.vice *.vdmrt *.rtf);;";
#else
  filter += "VDM files (*.vpp *.vdmpp *.rtf);;";
#endif // VICE
  if (this->lastPostfix != "java")
    filter += "Java files (*.java);;";
#endif //VDMPP

  if (this->lastPostfix != "tex")
    filter += "Tex files (*.tex)" ;
  if (this->lastPostfix != "all")
  {
    if( filter.right(2) != ";;" )
      filter += ";;"; 
    filter += "All Files (*)";
  }
  return filter;
}

void mainW::setLastFilter(const QStringList& files)
{
  if (files.isEmpty()) return;

  QString ftype;
  for (QStringList::const_iterator itr = files.begin(); itr != files.end(); itr++)
  {
    QString filename (*itr);
    QString tmptype;
    if( filename.right(4) == ".tex" )
      tmptype = "tex";
    else if( filename.right(4) == ".rtf" )
      tmptype = "vdm";
#ifdef VDMSL
    else if( ( filename.right(4) == ".vdm" ) ||
             ( filename.right(6) == ".vdmsl" ) )
      tmptype = "vdm";
#endif // VDMSL
#ifdef VDMPP
//    else if( ( filename.right(4) == ".vdm" ) ||
//             ( filename.right(6) == ".vdmsl" ) )
//      tmptype = "vdm";
    else if( ( filename.right(4) == ".vpp" ) ||
             ( filename.right(6) == ".vdmpp" ) ||
             ( filename.right(5) == ".vice" ) ||
             ( filename.right(6) == ".vdmrt" ) )
      tmptype = "vpp";
    else if( filename.right(5) == ".java" )
      tmptype = "java";
#endif // VDMPP

    if ( ftype == "" && tmptype != "" )
      ftype = tmptype;
    if ( ftype != "" && ftype != tmptype )
      ftype = "all";
      break;
  }
  this->lastPostfix = ftype;
}

void mainW::setLastDir(const QString& file)
{
  if( file.isEmpty() )
  {
    this->lastDir = this->getHome();
  }
  else
  {
    QFileInfo fi( file );
//    if( !fi.exists() ) return;
#if QT_VERSION >= 0x040000
    this->lastDir = fi.absolutePath(); 
#else
    this->lastDir = fi.dirPath(true); 
#endif // QT_VERSION >= 0x040000
  }
}

QString mainW::getLastDir()
{
  return this->lastDir;
}

//add and remove files to project
//
// mainW::addProject()
//
#if QT_VERSION >= 0x040000
void mainW::addProject_qt4(bool)
#else
void mainW::addProject_qt4(bool)
{
}

void mainW::addProject_qt3()
#endif // QT_VERSION >= 0x040000
{
  QString filter (this->getFilter());
  QString title (tr("Select files to Add to project"));
  QStringList files (QtPort::QtGetOpenFileNames(this, title, this->lastDir, filter));
  if (!files.isEmpty()) 
  {
    this->setLastFilter(files);
    this->setLastDir(files[0]);
    this->sendCommand(new AddFilesCMD(files));

    // SyntaxCheck
    this->sendSyntaxCommand(files);
  }
}

//
// mainW::removeProject()
//
#if QT_VERSION >= 0x040000
void mainW::removeProject_qt4(bool)
#else
void mainW::removeProject_qt4(bool)
{
}
void mainW::removeProject_qt3()
#endif // QT_VERSION >= 0x040000
{
  QStringList selectedFiles (this->browserw->getSelectedFiles());
  if(selectedFiles.size() == 0 ) return;

  switch( QMessageBox::information( 
            this, 
	    Qt2TB::GiveToolTitleI(),
	    tr( confirmFileRemovalText ),
	    mf(tr( okText )), mf(tr( cancelText )),
	    0,      
	    1 )) {   
  case 0: {
    this->sendCommand(new RemoveFilesCMD(selectedFiles));
    break;
  }
  case 1: 
    break;
  }
}

//file operations
//
// mainW::newFile()
//
#if QT_VERSION >= 0x040000
void mainW::newFile_qt4(bool)
#else
void mainW::newFile_qt4(bool)
{
}
void mainW::newFile_qt3()
#endif // QT_VERSION >= 0x040000
{
  // check project exists
  if (Qt2TB::getProjectNameI().isNull())
  {
    QMessageBox::warning(this, tr("Creating new File"), QString(tr("Please save project first")));
    return;
  }

  QStringList moduleList (Qt2TB::getModulesI());
  // get class/module name
#ifdef VDMSL
  bool iso = false;
  if ((moduleList.count() == 1) && (moduleList.first() == QString("DefaultMod")))
  {
    iso = true;
  }
#endif // VDMSL

  bool createOk = false;
  QString clmodnm;
  QString filenm;
  while(!createOk)
  {
    bool ok;
#ifdef VDMSL
//    QString title (iso ? tr("new file") : tr("new module"));
//    //QString label (tr("Please input file name:"));
//    QString label (iso ? tr("Please input file name:") : tr("Please input module name:"));
    if (iso) 
    {
      createOk = true;
      break;
    }
    QString title (tr("new module"));
    QString label (tr("Please input module name:"));
#endif // VDMSL
#ifdef VDMPP
    QString title (tr("new class"));
    QString label (tr("Please input class name:"));
#endif // VDMPP
#if QT_VERSION >= 0x040000
    clmodnm = QInputDialog::getText(this, title, label, QLineEdit::Normal, QString(), &ok);
#else
    clmodnm = QInputDialog::getText(title, label, QLineEdit::Normal, QString::null, &ok, this);
#endif // QT_VERSION >= 0x040000
    if (!ok) return;

    if (!clmodnm.isEmpty())
    {
      // check class/module name isn't exists
      bool forall = true;
      QStringList::const_iterator iter;
      for (iter = moduleList.begin(); (iter != moduleList.end()) && forall; ++iter)
      { 
        forall = (clmodnm != *iter);
      }
      if (!forall)
      {
        QString str = clmodnm + " " + QString(tr("already exists"));
        QMessageBox::warning(this, tr("Creating new File"), str);
      }
      else
        createOk = true;
    }
  }
  if (createOk)
    this->saveNewFileAs(clmodnm);
}

//
// mainW::externalEditor()
//
#if QT_VERSION >= 0x040000
void mainW::externalEditor_qt4(bool)
#else
void mainW::externalEditor_qt4(bool)
{
}
void mainW::externalEditor_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tw->get_if_useExternalEditor())
  {
#if QT_VERSION >= 0x040000
    QWidget * aw = this->ws->activeSubWindow();
#else
    QWidget * aw = this->ws->activeWindow();
#endif // QT_VERSION >= 0x040000
    if( aw != NULL )
    {
#if QT_VERSION >= 0x040000
      if( aw == this->browserw->parentWidget() )
#else
      if( aw == this->browserw )
#endif // QT_VERSION >= 0x040000
      {
        QStringList files (this->browserw->getSelectedFiles(true));
        QStringList::const_iterator itr;
        for (itr = files.begin(); itr != files.end(); ++itr)
        {
          QString filename (*itr);
          openEditor(filename, 1, 1);
        }
      }
#if QT_VERSION >= 0x040000
      else if ( ( aw == this->codew->parentWidget() ) ||
                ( aw == this->errorw->parentWidget() ) ||
                ( aw == this->searchw->parentWidget() ) )
#else
      else if ( ( aw == this->codew ) || ( aw == this->errorw ) || ( aw == this->searchw ) )
#endif // QT_VERSION >= 0x040000
      {
        QString editor (this->tw->get_if_editorName());
        QString format (this->tw->get_if_singleLoadFormat());

        this->codew->openCurrentFileWithEditor(editor, format);
      }
    }
  }
}

void mainW::openEditor(QString file, int line, int col)
{
  if (!this->tw->get_if_useExternalEditor())
    return;

  QString editor (this->tw->get_if_editorName());
  QString format (this->tw->get_if_singleLoadFormat());

  this->codew->openFileWithEditor(editor, format, file, line, col);
}

//
// mainW::fileClose()
//
#if QT_VERSION >= 0x040000
void mainW::fileClose_qt4(bool)
#else
void mainW::fileClose_qt4(bool)
{
  fileClose_qt3();
}
void mainW::fileClose_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->codew->closeSelectedFile();
}

//
// mainW::fileCloseAll()
//
#if QT_VERSION >= 0x040000
void mainW::fileCloseAll_qt4(bool)
#else
void mainW::fileCloseAll_qt4(bool)
{
  fileCloseAll_qt3();
}
void mainW::fileCloseAll_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->codew->closeAll();
}

//
// mainW::browser()
//
void mainW::browser_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * sw = this->browserw->parentWidget();
  if (sw->isVisible())
    sw->hide();
  else
  {
    if (!this->browserw->isVisible())
      this->browserw->show();
    sw->show();
    sw->raise();
    this->browserw->setFocus();
  }
#endif // QT_VERSION >= 0x040300
}

#if QT_VERSION < 0x040000
void mainW::browser_qt3()
{
  if (this->browserw->isVisible())
    this->browserw->hide();
  else
  {
    this->browserw->show();
    this->browserw->parentWidget()->raise();
    this->browserw->setFocus();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::codeInspector()
//
void mainW::codeInspector_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * w = this->codew->parentWidget();
  if (w->isVisible())
    w->hide();
  else
  {
    if (!this->codew->isVisible())
      this->codew->show();
    w->show();
    w->raise();
    this->codew->setFocus();
  }
#endif // QT_VERSION >= 0x040300
}

#if QT_VERSION < 0x040000
void mainW::codeInspector_qt3()
{
  if (this->codew->isVisible() )
    this->codew->hide();
  else
  {
    this->codew->show();
    this->codew->parentWidget()->raise();
    this->codew->setFocus();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::errorlist()
//
void mainW::errorlist_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * w = this->errorw->parentWidget();
  if (w->isVisible())
    w->hide();
  else
  {
    if (!this->errorw->isVisible())
      this->errorw->show();
    w->show();
    w->raise();
    this->errorw->setFocus();
  }
#endif // QT_VERSION >= 0x040300
}

#if QT_VERSION < 0x040000
void mainW::errorlist_qt3()
{
  if (this->errorw->isVisible())
    this->errorw->hide();
  else
  {
    this->errorw->show();
    this->errorw->parentWidget()->raise();
    this->errorw->setFocus();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::search()
//
void mainW::search_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * w = this->searchw->parentWidget();
  if (w->isVisible())
    w->hide();
  else
  {
    if (!this->searchw->isVisible())
      this->searchw->show();
    w->show();
    w->raise();
    this->searchw->setFocus();
  }
#endif // QT_VERSION >= 0x040300
}

#if QT_VERSION < 0x040000
void mainW::search_qt3()
{
  if (this->searchw->isVisible())
    this->searchw->hide();
  else
  {
    this->searchw->show();
    this->searchw->parentWidget()->raise();
    this->searchw->setFocus();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::interpreterWindow()
//
void mainW::interpreterWindow_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * w = this->interpreterw->parentWidget();
  if (w->isVisible())
    w->hide();
  else
  {
    if (!this->interpreterw->isVisible())
      this->interpreterw->show();
    w->show();
    w->raise();
    this->interpreterw->setFocus();
    this->interpreterTools->show();
  }
#endif // QT_VERSION < 0x040300
}

#if QT_VERSION < 0x040000
void mainW::interpreterWindow_qt3()
{
  if (this->interpreterw->isVisible())
    this->interpreterw->hide();
  else
  {
    this->interpreterw->show();
    this->interpreterw->parentWidget()->raise();
    this->interpreterw->setFocus();
    this->interpreterTools->show();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::references() 
//
#if QT_VERSION >= 0x040000
void mainW::references_qt4(bool) 
#else
void mainW::references_qt4(bool) 
{
}
void mainW::references_qt3() 
#endif // QT_VERSION >= 0x040000
{
  // references is a temporary modal dialog
//  referencesW * r = new referencesW(this,"", true,0);
//  r->show();
}

//
// mainW::log()
//
void mainW::log_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * w = this->logw->parentWidget();
  if (w->isVisible())
    w->hide();
  else
  {
    if (!this->logw->isVisible())
      this->logw->show();
    w->show();
    this->logw->setFocus();
  }
#endif // QT_VERSION >= 0x040300
}

#if QT_VERSION < 0x040000
void mainW::log_qt3()
{
  if (this->logw->isVisible())
    this->logw->hide();
  else
  {
    this->logw->show();
    this->logw->setFocus();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::pog()
//
void mainW::pog_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * w = this->pogw->parentWidget();
  if (w->isVisible())
    w->hide();
  else
  {
    if (!this->pogw->isVisible())
      this->pogw->show();
    w->show();
    this->pogw->setFocus();
  }
#endif // QT_VERSION >= 0x040300
}

#if QT_VERSION < 0x040000
void mainW::pog_qt3()
{
  if (this->pogw->isVisible())
    this->pogw->hide();
  else
  {
    this->pogw->show();
    this->pogw->setFocus();
  }
}
#endif // QT_VERSION < 0x040300

//
// mainW::rose()
//
#if QT_VERSION >= 0x040000
void mainW::rose_qt4(bool)
#else
void mainW::rose_qt4(bool)
{
}

void mainW::rose_qt3()
#endif // QT_VERSION < 0x040300
{
#ifdef VDMPP
  if (Qt2TB::getProjectNameI().isNull())
  {
    QStringList files(Qt2TB::getProjectFilesI());
    if (!files.isEmpty())
    {
      Qt2TB::setBlock( false );
      bool ok = this->saveProjectAs();
      Qt2TB::setBlock( true );
      if (!ok) return;
      Qt2TB::setBlock( false );
      while(Qt2TB::getProjectNameI().isNull());
      Qt2TB::setBlock( true );
    }
  }
  if (this->rosew == NULL)
  {
    this->rosew = new roseW(this, this, "", true, 0);
    QObject::connect(this->rosew, SIGNAL(sendUMLDiffCommand(QStringList)),
                     this, SLOT(sendUMLDiffCommand(QStringList)));
  }

#ifdef _MSC_VER
  if (this->tw != NULL)
  {
    this->rosew->SetRoseMode(this->tw->isRoseMode());
  }
#endif // _MSC_VER

  if (this->rosew->AddAllClasses())
  {
#if QT_VERSION >= 0x040000
    this->rosew->setWindowIcon(this->getImage( "rose.png", false ));
#else
    this->rosew->setIcon(this->getImage( "rose.png", false ));
#endif // QT_VERSION >= 0x040000
    this->rosew->show();
  }
#endif // VDMPP
}

//class slots
//
// mainW::syntax()
//
#if QT_VERSION >= 0x040000
void mainW::syntax_qt4(bool)
#else
void mainW::syntax_qt4(bool)
{
  syntax_qt3();
}
void mainW::syntax_qt3()
#endif // QT_VERSION >= 0x040000
{ 
  QStringList sel (this->browserw->getSelectedFiles());
  if (!sel.isEmpty()) {
    this->sendSyntaxCommand(sel);
  }
} 

void mainW::sendSyntaxCommand(const QStringList & list)
{
  this->sendCommand(new SyntaxCheckCMD(list));
}

//
// mainW::type()
//
#if QT_VERSION >= 0x040000
void mainW::type_qt4(bool)
#else
void mainW::type_qt4(bool)
{
  type_qt3();
}
void mainW::type_qt3()
#endif // QT_VERSION >= 0x040000
{
  QStringList moduleList (this->browserw->getSelectedModules());
  if( !moduleList.empty() )
  {
    this->sendTypecheckCommand(moduleList);
  }
} 

void mainW::sendTypecheckCommand(const QStringList & list)
{
  this->sendCommand(new TypeCheckCMD(list));
}

//
// mainW::cpp()
//
#if QT_VERSION >= 0x040000
void mainW::cpp_qt4(bool)
#else
void mainW::cpp_qt4(bool)
{
  cpp_qt3();
}
void mainW::cpp_qt3()
#endif // QT_VERSION >= 0x040000
{
  QStringList moduleList (this->browserw->getSelectedModules());
  if( !moduleList.empty() )
  {
    this->sendCommand(new CodeGenerateCMD(moduleList));
  }
}
 
//
// mainW::pretty()
//
#if QT_VERSION >= 0x040000
void mainW::pretty_qt4(bool)
#else
void mainW::pretty_qt4(bool)
{
  pretty_qt3();
}
void mainW::pretty_qt3()
#endif // QT_VERSION >= 0x040000
{
  QStringList sel (this->browserw->getSelectedFiles());
  if( !sel.empty() )
  {
    this->sendCommand(new PrettyPrintCMD(sel));
  }
}

//
// mainW::java()
//
#if QT_VERSION >= 0x040000
void mainW::java_qt4(bool)
#else
void mainW::java_qt4(bool)
{
  java_qt3();
}
void mainW::java_qt3()
#endif // QT_VERSION >= 0x040000
{
#ifdef VDMPP
  QStringList moduleList (this->browserw->getSelectedModules());
  if( !moduleList.empty() ) {
    this->sendCommand(new JavaGenerateCMD(moduleList));
  }
#endif //VDMPP
}

//
// mainW::java2vdm()
//
#if QT_VERSION >= 0x040000
void mainW::java2vdm_qt4(bool)
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
void mainW::java2vdm_qt4(bool)
{
  java2vdm_qt3();
}
void mainW::java2vdm_qt3()
#endif // QT_VERSION < 0x040000
{
#ifdef VDMPP
  if(!this->browserw->isJavaClassSelected())
    QMessageBox::warning( this, tr("Generation of VDM++ from Java"), QString(tr(javaWinSelErr)));
  else 
  {
    QStringList sel (this->browserw->getSelectedModules());
    this->sendCommand(new VDMGenerateCMD(sel, ow->get_j2v_stubsOnly(), true, ow->get_j2v_transforms()));
  }
#endif //VDMPP
}

//
// mainW::run()
//
#if QT_VERSION >= 0x040000
void mainW::run_qt4(bool)
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
void mainW::run_qt4(bool)
{
  run_qt3();
}
void mainW::run_qt3()
#endif // QT_VERSION < 0x040000
{
  QString expr = this->ow->getExpression();
  if (!expr.isEmpty())
  {
#if QT_VERSION >= 0x040000
    this->ipInit_qt4(true);
#else
    this->ipInit_qt3();
#endif // QT_VERSION >= 0x040000
    QString cmd ("debug " + expr);
    this->executeCommand(cmd);
    this->interpreterw->addString(cmd);
  }
}

//
// mainW::callLog()
//
#if QT_VERSION >= 0x040000
void mainW::callLog_qt4(bool)
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
void mainW::callLog_qt4(bool)
{
  callLog_qt3();
}
void mainW::callLog_qt3()
#endif // QT_VERSION < 0x040000
{
  Qt2TB::ToggleCallLog();
  this->setCallLogMenu();
}

//
// mainW::setCallLogMenu()
//
void mainW::setCallLogMenu()
{
  bool callLog = Qt2TB::CallLog();
  QString text (callLog ? tr("Stop Call Log") : tr("Start Call Log"));
#if QT_VERSION >= 0x040000
  this->callLogA->setText(text) ;
#else
  this->callLogA->setMenuText(text);
#endif // QT_VERSION >= 0x040000
}

//
// mainW::genPOG()
//
#if QT_VERSION >= 0x040000
void mainW::genPOG_qt4(bool)
#else
void mainW::genPOG_qt4(bool)
{
  genPOG_qt3();
}
void mainW::genPOG_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->pogw->clearAllMessages();
#if QT_VERSION >= 0x040000
  if (!this->pogw->parentWidget()->isVisible())
  {
    this->pogw->parentWidget()->show();
    this->pogw->show();
  }
#else
  if (!this->pogw->isVisible())
    this->pogw->show();
#endif // QT_VERSION >= 0x040000
  
  QStringList moduleList (this->browserw->getSelectedModules());
  this->sendCommand(new GenPOGCMD(moduleList));
}

//
// mainW::ipInit()
//
#if QT_VERSION >= 0x040000
void mainW::ipInit_qt4(bool)
#else
void mainW::ipInit_qt4(bool)
{
  ipInit_qt3();
}
void mainW::ipInit_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->executeCommand("init");
  emit refreshButtons(this->tbThread->canContinue());
  if( NULL != this->interpreterw )
  {
#if QT_VERSION >= 0x040000
    if( !this->interpreterw->parentWidget()->isVisible() )
       this->interpreterw->parentWidget()->show(); 
    if( !this->interpreterw->isVisible() )
       this->interpreterw->show(); 
    this->interpreterw->parentWidget()->raise();
#else
    if( !this->interpreterw->isVisible() )
      this->interpreterw->show(); 
    this->interpreterw->parentWidget()->raise();
#endif // QT_VERSION >= 0x040000
    this->interpreterw->setFocus();
    this->interpreterTools->show();
  }
}

//
// mainW::ipStep()
//
#if QT_VERSION >= 0x040000
void mainW::ipStep_qt4(bool)
#else
void mainW::ipStep_qt4(bool)
{
  ipStep_qt3();
}
void mainW::ipStep_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tbThread->canContinue()){
    this->executeCommand("step");
  }
  emit refreshButtons(this->tbThread->canContinue());
}

//
// mainW::ipStepin()
//
#if QT_VERSION >= 0x040000
void mainW::ipStepin_qt4(bool)
#else
void mainW::ipStepin_qt4(bool)
{
}
void mainW::ipStepin_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tbThread->canContinue()) {
    this->executeCommand("stepin");
  }
  emit refreshButtons(this->tbThread->canContinue());
}

//
// mainW::ipSingle()
//
#if QT_VERSION >= 0x040000
void mainW::ipSingle_qt4(bool)
#else
void mainW::ipSingle_qt4(bool)
{
  ipSingle_qt3();
}
void mainW::ipSingle_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tbThread->canContinue())
  {
    this->executeCommand("singlestep");
  }
  emit refreshButtons(this->tbThread->canContinue());
}

//
// mainW::ipContinue()
//
#if QT_VERSION >= 0x040000
void mainW::ipContinue_qt4(bool)
#else
void mainW::ipContinue_qt4(bool)
{
  ipContinue_qt3();
}
void mainW::ipContinue_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tbThread->canContinue())
  {
    this->executeCommand("cont");
  }
  emit refreshButtons(this->tbThread->canContinue());
}

//
// mainW::ipFinish()
//
#if QT_VERSION >= 0x040000
void mainW::ipFinish_qt4(bool)
#else
void mainW::ipFinish_qt4(bool)
{
  ipFinish_qt3();
}
void mainW::ipFinish_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tbThread->canContinue())
  {
    this->executeCommand("finish");
  }
  emit refreshButtons(this->tbThread->canContinue());
}

//
// mainW::ipUp()
//
#if QT_VERSION >= 0x040000
void mainW::ipUp_qt4(bool)
#else
void mainW::ipUp_qt4(bool)
{
  ipUp_qt3();
}
void mainW::ipUp_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->executeCommand("up");
}

//
// mainW::ipDown()
//
#if QT_VERSION >= 0x040000
void mainW::ipDown_qt4(bool)
#else
void mainW::ipDown_qt4(bool)
{
  ipDown_qt3();
}
void mainW::ipDown_qt3()
#endif // QT_VERSION >= 0x040000
{
  this->executeCommand("down");
}

//
// mainW::ipStop()
//
#if QT_VERSION >= 0x040000
void mainW::ipStop_qt4(bool)
#else
void mainW::ipStop_qt4(bool)
{
  ipStop_qt3();
}
void mainW::ipStop_qt3()
#endif // QT_VERSION >= 0x040000
{
  if (this->tbThread->canContinue())
  {
    Qt2TB::ipStopI();
  }
  emit refreshButtons(this->tbThread->canContinue());
}

//help slots

//
// mainW::about()
//
#if QT_VERSION >= 0x040000
void mainW::about_qt4(bool)
#else
void mainW::about_qt4(bool)
{
  about_qt3();
}
void mainW::about_qt3()
#endif // QT_VERSION >= 0x040000
{
  QString title (Qt2TB::GiveToolTitleI());
  QString version (Qt2TB::GiveToolVersionDateI());
  QString message ("<p>The answer is <b>42</b>, if the bug isn't in the specification.</p>");
  QString credit (Qt2TB::GetCreditI().replace("\n", "</p>\n<p>"));
  QString link ("<a href=\"http://fmvdm.org/\">http://fmvdm.org/</a>, "
                "<a href=\"http://www.vdmportal.org/\">http://www.vdmportal.org/</a>");

#if QT_VERSION >= 0x040000
  QString text (QMessageBox::tr(
    "<h3>%1</h3>"
    "<p>%2</p>"
    "<p>%3</p>"
    "<p>%4</p>"
    "<p>See %5 for more information.</p>"
  ).arg(title, version, message, credit, link));
#else
  QString text (QMessageBox::tr(
    "<h3>%1</h3>"
    "<p>%2</p>"
    "<p>%3</p>"
    "<p>See <tt>http://www.vdmtools.jp/</tt>, <tt>http://www.vdmportal.org/</tt> for more information.</p>"
  ).arg(title).arg(version).arg(message));
#endif // QT_VERSION >= 0x040000

  QMessageBox mbox(this);
#if QT_VERSION >= 0x040000
  mbox.setWindowTitle( tr("About %1").arg(title) );
#else
  //mbox.setCaption( wtitle );
  mbox.setCaption( tr("About %1").arg(title) );
#endif // QT_VERSION >= 0x040000
  mbox.setIconPixmap( getImageData( "vdm.png" ) );
  mbox.setText( text );
  mbox.setButtonText( QMessageBox::Yes, "&Ok" );
  mbox.exec();
}

//
// mainW::aboutqt()
//
#if QT_VERSION >= 0x040000
void mainW::aboutqt_qt4(bool)
#else
void mainW::aboutqt_qt4(bool)
{ // dummy
  aboutqt_qt3();
}
void mainW::aboutqt_qt3()
#endif // QT_VERSION >= 0x040000
{
  QString title (tr("About %1").arg(QString("Qt")));
  QMessageBox::aboutQt( this, title );
}

//status hint function (for non action functions)
//
// mainW::statusHint(int itemId)
//
/*
void mainW::statusHint(int itemId)
{
  if (hintdict[itemId])
//    QMainWindow::statusBar()->message(*hintdict[itemId], hinttime);
#if QT_VERSION >= 0x040000
    this->statusBar->showMessage(*hintdict[itemId], hinttime);
#else
    this->statusBar->message(*hintdict[itemId], hinttime);
#endif // QT_VERSION >= 0x040000
}
*/

//
// mainW::checkActions()
//
void mainW::checkActions()
{
  bool anySelected = this->browserw->anyFilesOrModulesAreSelected();

  this->syntaxA->setEnabled(anySelected);
  this->typeA->setEnabled(anySelected);
  this->cppA->setEnabled(anySelected);
  this->prettyA->setEnabled(anySelected);

#ifdef VDMPP
  this->javaA->setEnabled(anySelected);
  this->java2vdmA->setEnabled(anySelected && this->browserw->isJavaClassSelected());
#endif //VDMPP
  this->pogA->setEnabled(anySelected &&
                         !this->browserw->isJavaClassSelected() &&
                         Qt2TB::IsEverythingTypeCorrectI() );
}

//
// mainW::checkButtons()
//
void mainW::checkButtons()
{
  emit refreshButtons(this->tbThread->canContinue());
}

//
// mainW::classBrowserShown()
//
void mainW::classBrowserShown()
{ //show toolbar
    //    browserA->setOn(true);
}

//
// mainW::resetFilePosition(QString title, QString filename, int line, 
//
void mainW::resetFilePosition(QString title, QString filename, int line, int col, int length)
{
  QApplication::postEvent(this, new OpenFileEvent(filename, title, line, col, length));  
}

//
// mainW::resetFilePosition( QString filename, int line, 
//
void mainW::resetFilePosition(QString filename, int line, int col)
{
  if (this->browserw)
  {
    QString apparentFile (this->browserw->getFilePath(filename));
    QString filename; // empty
    QApplication::postEvent(this, new OpenFileEvent(filename, apparentFile, line, col, 1));  
  }
}

//
// mainW::setBreakPoint()
//
void mainW::setBreakPoint()
{
  this->codew->setBreakPoint();
}

///////////////////////////////////////////////////////////////// 
// Event handlers
///////////////////////////////////////////////////////////////// 
//
// mainW::event does not have the negative effect wrt toolbars
// that mainW::eventFilter has
//
bool mainW::event (QEvent * e)
{
  if (e->type() == QEvent::WindowActivate)
  {
    if (this->browserw)
      this->browserw->checkModifiedModules();
    if (this->interpreterw)
      this->interpreterw->checkBreakpointsFile();
  }
  else if (e->type () >= QEvent::User)
    this->userEvent( e );

  return QWidget::event(e);
}
//
// mainW::closeEvent (QCloseEvent *e)
//
void mainW::closeEvent (QCloseEvent *e)
{
  this->closing = true;
  this->checkProjectIsSaved();
  QWidget::closeEvent(e);
}

//
// mainW::userEvent(QEvent *e)
//
void mainW::userEvent(QEvent *e)
{
  if (this->cleaningUp) return;

//wcout << e->type() << endl;

#if QT_VERSION < 0x040000
  this->appRef->lock();
#endif // QT_VERSION < 0x040000
  switch((int)e->type()) {
    case GUIEvent::EV_OPENFILE:
    {
      OpenFileEvent *ofe = (OpenFileEvent*) e;
      openFile(ofe->title(), ofe->filename(), ofe->line(), ofe->column(), ofe->length());
      break;
    }
    case GUIEvent::EV_CHANGE_PROJECT_NAME:
    {
       // caution
       // at this point projectname is't changed in repository
       //
      this->loading = false;
      break;
    }
    case GUIEvent::EV_POST_SAVE_PROJECT_AS:
    {
      this->browserw->refreshProjectName();
      if (!this->ow->optionFileExists())
      {
        this->ow->saveOptions();
      }
      this->ow->loadOptions();
      this->loading = false;
      break;
    }
    case GUIEvent::EV_LOAD_PROJECT:
    {
      this->browserw->refreshProjectName();
      this->ow->loadOptions();
      this->loading = false;
      break;
    }
    case GUIEvent::EV_NEW_UNNAMEDPROJECT:
    {
      this->browserw->refreshFiles();
      //this->ow->loadOptions();
      Qt2TB::InitOptions();
      this->ow->initTab();
      this->clearWindows(true);
      break;
    }
    case GUIEvent::EV_ADD_FILES:
    {
      AddFileEvent *afe = (AddFileEvent*) e;
      this->addFiles(afe->files());
      break;
    }
    case GUIEvent::EV_REMOVE_FILES:
    {
      RemoveFilesEvent *rfe = (RemoveFilesEvent*) e;
      this->removeFiles(rfe->files());
      break;
    }
    case GUIEvent::EV_ADD_MODULES:
    {
      AddModulesEvent *ame = (AddModulesEvent*) e;
      this->addModules(ame->modules());
      break;
    }
    case GUIEvent::EV_REMOVE_MODULES:
    {
      RemoveModulesEvent *rme = (RemoveModulesEvent*) e;
      this->removeModules(rme->modules());
      break;
    }
    case GUIEvent::EV_CHANGE_MODULES_STATUS:
    {
      ChangeModulesStatusEvent *cms = (ChangeModulesStatusEvent*) e;
      this->changeModulesStatus(cms->modules());
      break;
    }
    case GUIEvent::EV_CHANGED_FILE_STATUS:
    {
      ChangedFileStatusEvent *cfs = (ChangedFileStatusEvent*) e;
      this->changeFileState(cfs->file());
      break;
    }
    case GUIEvent::EV_ADD_BREAK_NAME:
    {
      AddBreakNameEvent *abn = (AddBreakNameEvent*) e;
      this->addBreakName(abn->breakName(), abn->breakNum());
      break;
    }
    case GUIEvent::EV_ADD_BREAK_POS:
    {
      AddBreakPosEvent *abp = (AddBreakPosEvent*) e;
      this->addBreakPos(abp->breakName(), abp->breakLine(), abp->breakCol(), abp->breakNum());
      break;
    }
    case GUIEvent::EV_REMOVE_BREAK:
    {
      RemoveBreakEvent *rbe = (RemoveBreakEvent*) e;
      this->removeBreak(rbe->breakNum());
      break;
    }
    case GUIEvent::EV_ENABLE_BREAK:
    {
      EnableBreakEvent *ebe = (EnableBreakEvent*) e;
      this->enableBreak(ebe->breakNum());
      break;
    }
    case GUIEvent::EV_DISABLE_BREAK:
    {
      DisableBreakEvent *dbe = (DisableBreakEvent*) e;
      this->disableBreak(dbe->breakNum());
      break;
    }
    case GUIEvent::EV_BACK_TRACE:
    {
      BackTraceEvent *bte = (BackTraceEvent*) e;
      this->setBackTrace(bte->bts());
      break;
    }
    case GUIEvent::EV_GOTO_BT_LEVEL:
    {
      GotoBtLevelEvent *gbl = (GotoBtLevelEvent*) e;
      this->gotoBtLevel(gbl->level());
      break;
    }
    case GUIEvent::EV_CLEAR_DEBUG_WINDOW:
    {
      this->clearDebugWindow();
      break;
    }
    case GUIEvent::EV_UPDATE_SELECTIONS:
    {
      this->updateSelections();
      break;
    }
    case GUIEvent::EV_CLEAR_ALL:
    {
      this->clearWindows(false);
      break;
    }
    case GUIEvent::EV_ROSE_DIFF:
    {
#ifdef VDMPP
      RoseDiffEvent *rde = (RoseDiffEvent*) e;
      this->addRoseDiff(rde->text());
#endif // VDMPP
      break;
    }
    case GUIEvent::EV_INTERPRETER_WRITE:
    {
      InterpreterWriteEvent *iwe = (InterpreterWriteEvent*) e;
      this->interpreterWrite(iwe->text());
      break;
    }
    case GUIEvent::EV_LOG_WRITE:
    {
      LogWriteEvent *lwe = (LogWriteEvent*) e;
      this->logWrite(lwe->text());
      break;
    }
    case GUIEvent::EV_POG_ADD:
    {
      PogAddEvent *pwe = (PogAddEvent*) e;
      this->pogAdd(pwe);
      break;
    }
    case GUIEvent::EV_POG_CLEAN:
    {
      PogCleanUpEvent *pwe = (PogCleanUpEvent*) e;
      this->pogCleanUp(pwe);
      break;
    }
    case GUIEvent::EV_POG_UPDATE_FILTER:
    {
      this->pogUpdateFilters();
      break;
    }
    case GUIEvent::EV_ROSE_ERROR:
    {
#ifdef VDMPP
      RoseErrorEvent *ree = (RoseErrorEvent*) e;
      this->addRoseError(ree->text());
#endif // VDMPP
      break;
    }
    case GUIEvent::EV_MESSAGE_SHOW:
    {
      MessageShowEvent *mse = (MessageShowEvent*) e;
      this->showMessage(mse->index(), mse->message());
      break;
    }
    case GUIEvent::EV_MESSAGE_ADD:
    {
      MessageAddEvent *mae = (MessageAddEvent*) e;
      this->addMessage(mae->errnum(), mae->filename(), mae->line(),
                       mae->column(), mae->message(), mae->tempFilename());
      break;
    }
    case GUIEvent::EV_MESSAGE_CLEAR_ALL:
    {
      if( !this->loading )
        this->clearAllMessages();
      break;
    }
    case GUIEvent::EV_MESSAGE_ALL_DONE:
    {
      this->messagesAllDone();
      break;
    }
    case GUIEvent::EV_MESSAGE_NEXT:
    {
      this->nextMessage();
      break;
    }
    case GUIEvent::EV_MESSAGE_PREV:
    {
      this->prevMessage();
      break;
    }
    case GUIEvent::EV_MESSAGE_FIRST:
    {
      this->firstMessage();
      break;
    }
    case GUIEvent::EV_MESSAGE_LAST:
    {
      this->lastMessage();
      break;
    }
    case GUIEvent::EV_ERROR_STATUS:
    {
      ErrorStatusEvent *ese = (ErrorStatusEvent*) e;
      this->setErrorInfo(ese->errors(), ese->warnings());
      break;
    }
    case GUIEvent::EV_ENABLE_INPUT:
    {
      this->enableGUI();
      break;
    }
    case GUIEvent::EV_DISABLE_INPUT:
    {
      this->disableGUI();
      break;
    }
    case GUIEvent::EV_REFRESH_INTERFACE:
    {
      break;
    }
    case GUIEvent::EV_METER_INIT:
    {
      MeterInitEvent *mie = (MeterInitEvent*) e;
#if QT_VERSION >= 0x040000
      this->statusBar->showMessage(mie->title() + ": " + mie->label());
#else
      this->statusBar->message(mie->title() + ": " + mie->label());
#endif // QT_VERSION >= 0x040000
      this->pb->reset();
      break;
    }
    case GUIEvent::EV_METER_INCREMENT:
    {
      QString label (((MeterIncrementEvent*) e)->label());
#if QT_VERSION >= 0x040000
      if (!label.isEmpty())
        this->statusBar->showMessage(label);
      int p = this->pb->value();
      this->pb->setValue(p + 1);
#else
      if (!label.isEmpty())
        this->statusBar->message(label);
      int p = this->pb->progress();
      this->pb->setProgress(p + 1);
#endif // QT_VERSION >= 0x040000
      break;
    }
    case GUIEvent::EV_METER_SET_TOTAL:
    {
      int total = (int)((MeterSetTotalEvent*) e)->total();
#if QT_VERSION >= 0x040000
      this->pb->setMaximum(total);
#else
      this->pb->setTotalSteps(total);
#endif // QT_VERSION >= 0x040000
      break;
    }
    case GUIEvent::EV_METER_UPDATE:
    {
      MeterUpdateEvent *mue = (MeterUpdateEvent*) e;
#if QT_VERSION >= 0x040000
      this->statusBar->showMessage(mue->label());
      this->pb->setValue((int) mue->percentage());
#else
      this->statusBar->message(mue->label());
      this->pb->setProgress ((int) mue->percentage());
#endif // QT_VERSION >= 0x040000
      break;
    }
    case GUIEvent::EV_METER_DESTROY:
    {
#if QT_VERSION >= 0x040000
      this->statusBar->clearMessage();
#else
      this->statusBar->clear();
#endif // QT_VERSION >= 0x040000
      this->pb->reset();
      break;
    }
    case GUIEvent::EV_REFRESH_SRC_WINDOW:
    {
      RefreshSrcWindowEvent *rsw = (RefreshSrcWindowEvent*) e;
      this->refreshSourceWindow(rsw->title(), rsw->filename());
      break;
    }
    case GUIEvent::EV_CHECK_ACTIONS:
    {
      this->checkActions();
      break;
    }
    case GUIEvent::EV_ENABLE_STOP:
    {
      this->stopEnabled = true;
      break;
    }
    case GUIEvent::EV_DISABLE_STOP:
    {
      this->stopEnabled = false;
      break;
    }
    case GUIEvent::EV_ADD_MODULE_AND_STATUS:
    {
      AddModuleAndStatusEvent * amase = (AddModuleAndStatusEvent*) e;
      this->addModuleAndStatus(amase->module(), amase->files(), amase->syntax(), amase->type(),
                               amase->cg(), amase->jcg(), amase->pp());
      break;
    }
    case GUIEvent::EV_UPDATE_MODULE_STATUS:
    {
      UpdateModuleStatusEvent * umse = (UpdateModuleStatusEvent*) e;
      this->updateModuleStatus(umse->module(), umse->syntax(), umse->type(), umse->cg(), umse->jcg(), umse->pp());
      break;
    }
    default:
      break;
  }
#if QT_VERSION < 0x040000
  this->appRef->unlock();
#endif // QT_VERSION < 0x040000
  this->setComplete(true);
#ifdef __darwin__
#if __cplusplus > 199711L
  std::this_thread::sleep_for(std::chrono::microseconds(20));
#else
  usleep(20);
#endif
#endif // __darwin__
}

///////////////////////////////////////////////////////////////// 
// End of event handlers
///////////////////////////////////////////////////////////////// 


//
// mainW::checkProjectIsSaved()
//
void mainW::checkProjectIsSaved()
{
  if (!Qt2TB::stateIsSavedI())
  {
    switch (QMessageBox::information(this, 
	                             Qt2TB::GiveToolTitleI(),
	                             tr( saveProjectPrompt ),
	                             mf(tr( yesText )),
	                             mf(tr( noText )),
	                             0,
	                             1)) {
      case 0: {
#if QT_VERSION >= 0x040000
        saveProject_qt4(true);
#else
        saveProject_qt3();
#endif // QT_VERSION >= 0x040000
        break;
      }
      case 1: {
        break;
      }
    }
  }
}

//
// mainW::executeCommand(QString commstr)
//
void mainW::executeCommand(QString commstr)
{
//  cmd.SetField(1, qstring2seq(commstr));
  // yen char -> backslash for Japanese Input

  QString command(commstr);
  QString modified_cmd (command.replace( QChar(0x00A5), QChar(0x005C) ));
  this->sendCommand(new ExecuteCommandCMD(modified_cmd));
}

//
// mainW::enableBr(int num)
//
void mainW::enableBr(int num)
{
  QString command;
  command.sprintf("enable %d", num);
  this->executeCommand(command);
}

//
// mainW::disableBr(int num)
//
void mainW::disableBr(int num)
{
  QString command;
  command.sprintf("disable %d", num);
  this->executeCommand(command);
}

//
// mainW::deleteBr(int num)
//
void mainW::deleteBr(int num)
{
  QString command;
  command.sprintf("delete %d", num);
  this->executeCommand(command);
}

//
// mainW::setApp(QApplication* app)
//
void mainW::setApp(QApplication* app)
{
  this->appRef = app;
}

//
// mainW::setFont(const QFont newFont)
//
void mainW::setFont(QFont newFont)
{
  this->logWrite("Setting font to be " + newFont.family());

#if QT_VERSION < 0x040000
  QApplication::setFont(newFont, true); // Set the font for existing windows
#endif // QT_VERSION < 0x040000
  QApplication::setFont(newFont);       // Set the font for future windows
  this->codew->setFont(font());
  this->errorw->setFont(font());
  this->logw->setFont(font());
  this->browserw->setFont(font());
  this->interpreterw->setFont(font());
  this->pogw->setFont(font());
}

QString mainW::getTextCodecName()
{
  QString name;
#if QT_VERSION >= 0x050000
  QTextCodec* pCodec = QTextCodec::codecForLocale();
#else
  QTextCodec* pCodec = QTextCodec::codecForTr();
#endif // QT_VERSION >= 0x050000
  if( pCodec != NULL ) {
    name = pCodec->name();
  }
  return name;
}

//
// mainW::setTextCodec(QString codecName)
//
void mainW::setTextCodec(QString menuName)
{
  this->setEncoding(menuName);

  if( NULL != this->codew )
    this->codew->closeAll();

  QStringList files(Qt2TB::getProjectFilesI());
  if (!files.isEmpty())
  {
    this->sendSyntaxCommand(files);
// 20120820 -->
    this->isModified = true;
    this->mayBeModified = false;
// <-- 20120820
  }
}

//
// mainW::logWrite(QString msg)
//
void mainW::logWrite(QString msg)
{
  if( NULL != this->logw ) this->logw->write( msg );
}

//
// mainW::pogAdd(PogAddEvent *)
//
void mainW::pogAdd(PogAddEvent *event)
{
  this->pogw->addPogItem(event->checkMark(),
                         event->modName(),
                         event->memberName(),
                         event->locationType(),
                         event->pogType(),
                         event->pogNum(),
                         event->filename(),
                         event->tmpFilename(),
                         event->line(),
                         event->col(),
                         event->length(),
                         event->pogDesc());
}

//
// mainW::pogCleanUp(PogCleanUpEvent*)
//
void mainW::pogCleanUp(PogCleanUpEvent* event)
{
  this->pogw->cleanUp(event->modList());
}

//
// mainW::pogUpdateFilters()
//
void mainW::pogUpdateFilters()
{
  this->pogw->updateFilters();
}

///////////////////////////////////////////////////////////////// 
// Functions called from EventToCommands
///////////////////////////////////////////////////////////////// 

//
// mainW::addFiles(QStringList& newFiles)
//
void mainW::addFiles(const QStringList & newFiles)
{
  this->browserw->addFiles(newFiles);

// 20120725 -->
  if (this->mayBeModified)
  {
    this->mayBeModified = false;
    Qt2TB::SetSavedFileState(true);
  }
// <-- 20120725
}

//
// mainW::removeFiles(QStringList& removedFiles)
//
void mainW::removeFiles(const QStringList & removedFiles)
{
  if (this->cleaningUp)
    return;

  this->browserw->removeFiles(removedFiles);

  // remove from source window // 20051222
  this->removeFilesFromGUI(removedFiles);
}

void mainW::removeFilesFromGUI(const QStringList & files)
{
  QStringList::const_iterator itr;
  for (itr = files.begin(); itr != files.end(); ++itr)
  {
    QString filename (*itr);
    this->interpreterw->removeBreakOfFile( filename );
    this->codew->closeFile( filename );
//    this->errorw->removeMessagesOfFile( filename );
  }
}

//
// mainW::addModules(QStringList& moduleNames)
//
void mainW::addModules(const QStringList & moduleNames)
{
  QStringList::const_iterator itr;
  for (itr = moduleNames.begin(); itr != moduleNames.end(); ++itr)
  {
    QStringList files (Qt2TB::getFilesOfModule(*itr));
    StatusType st (Qt2TB::getStatusI(*itr));

    this->browserw->addModule(*itr, files, st);
  }
}

//
//
//
void mainW::addModuleAndStatus(const QString & module, const QStringList & files,
                               int syntax, int type, int cg, int jcg, int pp)
{
  StatusType st ((StatusType::status)syntax,
                 (StatusType::status)type,
                 (StatusType::status)cg,
                 (StatusType::status)jcg,
                 (StatusType::status)pp);
  this->browserw->addModule(module, files, st);
}

//
// mainW::removeModules(QStringList&)
//
void mainW::removeModules(const QStringList & removedModules)
{
  if (this->cleaningUp)
    return;

  this->browserw->removeModules(removedModules);

// 20120725 -->
  if (this->isModified)
  {
    this->isModified = false;
    this->mayBeModified = true;
    Qt2TB::SetSavedFileState(true);
  }
// <-- 20120725
}

//
// mainW::changeModulesStatus(QStringList&)
//
void mainW::changeModulesStatus(const QStringList & modules)
{
  QStringList::const_iterator itr;
  for (itr = modules.begin(); itr != modules.end(); ++itr)
  {
    StatusType st (Qt2TB::getStatusI(*itr));
    this->browserw->updateModuleStatus(*itr, st);
  }
}

//
// mainW::changeModuleStatus(QStringt&)
//
void mainW::updateModuleStatus(const QString & module, int syntax, int type, int cg, int jcg, int pp)
{
  StatusType st ((StatusType::status)syntax,
                 (StatusType::status)type,
                 (StatusType::status)cg,
                 (StatusType::status)jcg,
                 (StatusType::status)pp);
  this->browserw->updateModuleStatus(module, st);

// 20120725 -->
  if (this->mayBeModified)
  {
    this->mayBeModified = false;
    Qt2TB::SetSavedFileState(true);
  }
// <-- 20120725
}
//
// mainW::changeFileState(const QString& filename)
//
void mainW::changeFileState(const QString & filename)
{
  QStringList moduleList (Qt2TB::getModulesI(filename));

  QStringList::const_iterator iter;
  for (iter = moduleList.begin(); iter != moduleList.end(); ++iter)
  { 
    StatusType st (Qt2TB::getStatusI(*iter));
    this->browserw->updateModuleStatus(*iter, st);
  }
// 20120725 -->
  if (this->isModified)
  {
    this->isModified = false;
    this->mayBeModified = true;
    Qt2TB::SetSavedFileState(true);
  }
// <-- 20120725
}

//
// mainW::refreshSourceWindow
//
// This is just a function linking codeF::refreshWindow and the
// RemoveFileChangedMark command extracted by EventToCommands in
// vdm.cc
//
// For details of the intended behaviour of this function, and the
// arguments, see codeF.cc
void mainW::refreshSourceWindow(const QString& title, const QString& src)
{
  bool coloring = this->tw->doSyntaxColoring();
  this->codew->refreshWindow(title, src, coloring);
}

//
// mainW::addBreakName(const QString& name, int num)
//
void mainW::addBreakName(const QString& name, int num)
{
#if QT_VERSION >= 0x040000
  if (!this->interpreterw->parentWidget()->isVisible())
    this->interpreterw->parentWidget()->show();
  if (!this->interpreterw->isVisible())
    this->interpreterw->show();
#else
  if (!this->interpreterw->isShown())
    this->interpreterw->show();
#endif // QT_VERSION >= 0x040000
  this->interpreterw->addBreakName(name, num);
}

//
// mainW::addBreakPos(const QString& filename, int line, int col, 
//
void mainW::addBreakPos(const QString& filename, int line, int col, int num)
{
#if QT_VERSION >= 0x040000
  if (!this->interpreterw->parentWidget()->isVisible())
    this->interpreterw->parentWidget()->show();
  if (!this->interpreterw->isVisible())
    this->interpreterw->show();
#else
  if (!this->interpreterw->isShown())
    this->interpreterw->show();
#endif // QT_VERSION >= 0x040000
  this->interpreterw->addBreakPos(filename, line, col, num);
}

//
// mainW::removeBreak(int num)
//
void mainW::removeBreak(int num)
{
  this->interpreterw->removeBreak(num);
}

//
// mainW::enableBreak(int num)
//
void mainW::enableBreak(int num)
{
  this->interpreterw->enableBreak(num);
}

//
// mainW::disableBreak(int num)
//
void mainW::disableBreak(int num)
{
  this->interpreterw->disableBreak(num);
}

//
// mainW::setBackTrace(const QValueList<BackTraceEntry> & bts)
// mainW::setBackTrace(const QList<BackTraceEntry> & bts)
//
void mainW::setBackTrace(const QLIST<BackTraceEntry> & bts)
{
  this->interpreterw->setBackTrace(bts);
}

//
// mainW::gotoBtLevel(int num)
//
void mainW::gotoBtLevel(int num)
{
  this->interpreterw->gotoBtLevel(num);
}

//
// mainW::clearDebugWindow()
//
void mainW::clearDebugWindow()
{
  this->interpreterw->clearDebugWindow();
}

//
// mainW::updateSelections()
//
void mainW::updateSelections()
{
  this->browserw->updateSelectedModules();
}

///////////////////////////////////////////////////////////////// 
// End of functions called from EventToCommands
///////////////////////////////////////////////////////////////// 

// 
// Functions for the error window
//

//
// mainW::showMessage(int index, const wchar_t* message)
//
void mainW::showMessage(int index, const QString& message)
{
  QMessageBox::about( this, "showMessage","showMessage");
}

//
// mainW::addMessage(int errnum, const wstring filename, int line, int
//
void mainW::addMessage(int errnum, const QString & filename, int line, int col,
                       const QString & message, const QString & tempFilenm)
{
  this->errorw->addErrorMessage(errnum, filename, line, col, message, tempFilenm);
// 20090616 -->
#if QT_VERSION >= 0x040000
  int index = message.indexOf( "  Run-Time Error" );
#else
  int index = message.find( "  Run-Time Error" );
#endif // QT_VERSION >= 0x040000
  if (index != -1 )
  {
    this->interpreterWrite(message.left(index));
    this->interpreterWrite(message.mid(index));
  }
// <-- 20090616
 /*&& col != -1 See item 4) in http://zeus.ifad.dk/bugzilla/show_bug.cgi?id=3*/
#if QT_VERSION >= 0x040000
  if (!this->errorw->parentWidget()->isVisible())
    this->errorw->parentWidget()->show();
  if (!this->errorw->isVisible())
    this->errorw->show();
  this->errorw->parentWidget()->raise();
#else
  if (!this->errorw->isVisible())
    this->errorw->show();
  this->errorw->parentWidget()->raise();
#endif // QT_VERSION >= 0x040000
  this->errorw->setFocus();
}

//
// mainW::clearAllMessages()
//
void mainW::clearAllMessages()
{
  this->errorw->clearAllMessages();
}

//
// mainW::nextMessage()
//
void mainW::nextMessage()
{
  this->errorw->nextMessage();
}

//
// mainW::prevMessage()
//
void mainW::prevMessage()
{
  this->errorw->previousMessage();
}

//
// mainW::firstMessage()
//
void mainW::firstMessage()
{
  this->errorw->firstMessage();
}

//
// mainW::lastMessage()
//
void mainW::lastMessage()
{
  this->errorw->lastMessage();
}

//
// mainW::setErrorInfo(int numErrs, int numWarn)
//
void mainW::setErrorInfo(int numErrs, int numWarn)
{
  if (numErrs != 0 || numWarn != 0) {
    // It would be nice to have these numbers in the header of the
    // error window
    QString caption1, caption2;
    caption1.sprintf("%d error", numErrs);
    if (numErrs != 1)
      caption1.append("s");
    caption2.sprintf(", %d warning", numWarn);
    if (numWarn != 1)
      caption2.append("s");
    this->logWrite(caption1 + caption2);
    this->errorw->setInfo(numErrs, numWarn);
  }
}

//
// mainW::messagesAllDone()
//
void mainW::messagesAllDone()
{
  this->errorw->messagesAllDone();
}

//
// mainW::openFile(const QString title, 
//
void mainW::openFile(const QString & title, const QString & file, int lineno = 1, int col = 1, int length = 1)
{
  bool coloring = this->tw->doSyntaxColoring() && !Qt2TB::isJavaFile(title);
  this->codew->openIn(title, file, lineno, col, length, coloring);

#if QT_VERSION >= 0x040000
  if(!this->codew->parentWidget()->isVisible())
  {
    this->codew->parentWidget()->show();
    this->codew->show();
  }
  this->codew->update();
#else
  if(!this->codew->isVisible())
    this->codew->show();
  this->codew->repaint();
#endif // QT_VERSION >= 0x040000
}

//
// Functions relating to the rose mapper
//

#ifdef VDMPP
//
// mainW::roseIsActive()
//
bool mainW::roseIsActive()
{
  return (this->rosew && this->rosew->isActiveWindow());
}

//
// mainW::roseIsAlive()
//
bool mainW::roseIsAlive()
{
  return (this->rosew && !this->rosew->isVisible());
}

//
// mainW::addRoseDiff(const QString& msg)
//
void mainW::addRoseDiff(const QString& msg)
{
  if (this->rosew)
    this->rosew->addDiff(msg);
  else
    this->logWrite(msg);
}

//
// mainW::addRoseError(const QString& msg)
//
void mainW::addRoseError(const QString& msg)
{
  if (this->rosew)
    this->rosew->addError(msg);
  else
    this->logWrite(msg);
}
#endif // VDMPP

//
// mainW::initOk()
//
bool mainW::initOk()
{
  return envVarsOk;
}

//
// mainW::checkEnvVars(QApplication &app) 
//
bool mainW::checkEnvVars(QApplication &app) 
{
  const char* r = getenv(VDMROOT);

  QString dirPath;
  string dir;
  if( !r )
  {
#ifdef __linux__
#if QT_VERSION >= 0x040000
    dirPath = app.applicationDirPath();
#else
    QString name( app.argv()[0] );
    if( name.length() > 0 )
    {
      if( name.left(1) == "/" )
        return QFileInfo( name ).dirPath();
    }
    char* cwd = getcwd( NULL, 0 );
    if( cwd == NULL ) return QString( "./" );
    QString cdir( cwd );
    free( cwd );

    dirPath = QFileInfo( cdir + "/" + name ).dirPath( true );
#endif // QT_VERSION >= 0x040000
#else
    dirPath = app.applicationDirPath();
#endif // __linux__

#if QT_VERSION >= 0x040000
    int i = dirPath.lastIndexOf('/');
#else
    int i = dirPath.findRev('/');
#endif // QT_VERSION >= 0x040000
    if( i > 0 )
    {
      dirPath = dirPath.left(i);
    }
    dir = TBWSTR::wstring2fsstr(Qt2TB::qstring2wstring(dirPath));
    r = dir.c_str();
  }
 
  if (!r) {
#ifdef _MSC_VER
    QString err; 
    wstring localRoot;
    wstring localErr;
    if (! FindRegEntry(localRoot, localErr)) {
      QString e ("The environment variable ");
      e += QString(VDMROOT) + " is not set \nand the registry lookup\n";
      e += "failed with this error: \n"; 
      e += Qt2TB::wstring2qstring(localErr); 
      e += "\nThe Toolbox does not seem to be properly installed."; 
#if QT_VERSION >= 0x040000
      QMessageBox::critical( this, QString("Error"), e ); 
#else
      QMessageBox::critical( this, QString("Error"), tr(e) ); 
#endif // QT_VERSION >= 0x040000
      return false; 
    }
    else {
      this->rootDir = Qt2TB::wstring2qstring(localRoot);
      err = Qt2TB::wstring2qstring(localErr);
    }
#else
    QMessageBox::critical( this, QString("Error"),
             QString(tr("You don't have a ")) + QString(VDMROOT) +
             QString(tr(" environment variable.\nexiting...\n"))); 
    return false;
#endif //_MSC_VER
  } else {
    this->rootDir = Qt2TB::wstring2qstring(TBWSTR::fsstr2wstring(r)); 
  }

#ifdef USE_IMAGE_FILE
  imgDir = this->rootDir + DIRSEP + "img";
  if(!this->checkForDir(imgDir))
    return false;
#endif // USE_IMAGE_FILE

#ifdef VDMPP
  UML_LIBRARY = this->rootDir + "/" + "uml"; 
//#ifdef _MSC_VER
//  if(!this->checkForDir(UML_LIBRARY))
//    return false; 
//#endif // _MSC_VER
#endif // VDMPP

  return true; 
}

#ifdef VDMPP
//
// mainW::getUMLLibrary()
//
QString mainW::getUMLLibrary()
{
  return UML_LIBRARY;
}
#endif // VDMPP

#ifdef _MSC_VER
wstring WinFormatMessage(long err)
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                (LPTSTR) &lpMsgBuf,
                0,
                NULL);
  wstring s(static_cast<wchar_t*>(lpMsgBuf));
  LocalFree(lpMsgBuf);
  return s; 
}

bool FindRegEntry(wstring& value, wstring& errtxt)
{
  HKEY hcpl;
  long err;
  wstring keyS (L"SOFTWARE\\VDMTOOLS\\");
  keyS += Qt2TB::qstring2wstring(Qt2TB::GiveToolTitleI());
  keyS += L"\\";
  keyS += Qt2TB::qstring2wstring(Qt2TB::GetVersionIdI()); 
  
  err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyS.c_str(), 0, KEY_READ, &hcpl);
  if (err != ERROR_SUCCESS) {
    wstring e (L"Error RegOpenKeyEx");
    e += L"(";
    e += keyS;
    e += L"):\n";
    e += WinFormatMessage(err);
    errtxt = e;
    return false;
  } else {
    unsigned char buffer[200];
    unsigned long size = sizeof(buffer); 
    err = RegQueryValueExA(hcpl, "ROOTDIR", 0, (unsigned long*)0, buffer, &size);
    if (err == ERROR_SUCCESS) {
      value = TBWSTR::string2wstring((char*)buffer);
      errtxt = wstring(L"");
      return true; 
    } else {
      wstring e (L"RegQueryValueEx error:\n");
      e += L"(ROOTDIR)\n\t" + WinFormatMessage(err);
      errtxt = wstring(e);
      return false;
    }      
  }
}
#endif //_MSC_VER

//
// mainW::clearWindows()
//
void mainW::clearWindows(bool isNew)
{
#if QT_VERSION >= 0x040000
  this->fileCloseAll_qt4(true);
#else
  this->fileCloseAll_qt3();
#endif // QT_VERSION >= 0x040000
  this->interpreterw->clearBuffer();
  this->interpreterw->resetWindow();

  QStringList emptyList;
  this->pogw->cleanUp(emptyList);

  if (isNew)
  {
    this->logWrite( tr( "New Project selected" ) );
    this->clearAllMessages();

    this->searchw->clearAllOccurences();

#if QT_VERSION >= 0x040000
//  if (!this->browserw->parentWidget()->isVisible())
  if (!this->browserw->isVisible())
  {
    this->browserw->parentWidget()->show();
    this->browserw->show();
  }
  if (this->interpreterw->parentWidget()->isVisible())
    this->interpreterw->parentWidget()->hide();
  if (this->errorw->parentWidget()->isVisible())
    this->errorw->parentWidget()->hide();
  if (this->searchw->parentWidget()->isVisible())
    this->searchw->parentWidget()->hide();
  if (this->pogw->parentWidget()->isVisible())
    this->pogw->parentWidget()->hide();
#else
  if (!this->browserw->isVisible())
  {
    this->browserw->show();
  }
#endif // QT_VERSION >= 0x040000
  }
  else
    this->browserw->clear();
}

//
// mainW::checkForDir(const QString& s)
//
bool mainW::checkForDir(const QString& s)
{
  string dir (TBWSTR::wstring2fsstr(Qt2TB::qstring2wstring(s)));
  struct stat statv; 
  int e = stat(dir.c_str(), &statv);
  if ((e==0) && (statv.st_mode & S_IFMT) == S_IFDIR) {
    return true; 
  } else {
    QMessageBox::critical( this, QString("Error"),
             QString("The path ") + s + " should point to a directory\n"); 
    return false; 
  }
}

//
// mainW::interpreterHasFocus()
//
bool mainW::interpreterHasFocus()
{
  return !this->interpreterw->isHidden();
}

//
// mainW::apiPeriodic()
//
void mainW::apiPeriodic()
{
#if QT_VERSION >= 0x040000
  if (!this->tbThread->isFinished()) return;
#else
  if (!this->tbThread->finished()) return;
#endif // QT_VERSION >= 0x040000
  if (this->toolRunning) return;
  this->sendCommand(new CallAPICMD());
}

//
// mainW::cleanUp()
//
void mainW::cleanUp()
{
  this->cleaningUp = true;

  this->saveProjectHistory(this->prjHistory);

  if ( NULL != this->tbThread )
  {
#if QT_VERSION >= 0x040000
    if( !this->tbThread->isFinished() )
#else
    if( !this->tbThread->finished() )
#endif // QT_VERSION >= 0x040000
    {
      this->tbThread->terminate();
#if QT_VERSION >= 0x040000
      while( this->tbThread->isRunning() );
#else
      while( this->tbThread->running() );
#endif // QT_VERSION >= 0x040000
    }
    delete this->tbThread;
  }

#ifdef _MSC_VER
  this->gtimer->stop();
#endif // _MSC_VER
}

//
// mainW::interpreterWrite(const Qstring& msg)
//
void mainW::interpreterWrite(const QString & msg)
{
  this->interpreterw->write(msg);

// 20140122 -->
#if QT_VERSION >= 0x040000
  QString smsg = msg.simplified();
#else
  QString smsg = msg.stripWhiteSpace();
#endif // QT_VERSION >= 0x040000
  if ((smsg == "calllog set") || (smsg == "calllog unset"))
    this->setCallLogMenu();
// <-- 20140122
}

//
// mainW::sendCommand(const Record& cmd)
//
void mainW::sendCommand(ToolboxCMD * cmd)
{
  this->tbThread->addCommand(cmd);
#if QT_VERSION >= 0x040000
  if ( this->tbThread->isFinished() ) this->tbThread->start();
#else
  if ( this->tbThread->finished() ) this->tbThread->start();
#endif // QT_VERSION >= 0x040000
#ifdef _MSC_VER
  SleepEx( 100, true );
#endif // _MSC_VER
}

void mainW::setComplete( bool complete )
{
  this->completion_mutex.lock();
  this->completion_flag = complete;
  this->completion_mutex.unlock();

#ifdef _MSC_VER
    Sleep( 1 );
#else
#if __cplusplus > 199711L
    std::this_thread::sleep_for(std::chrono::microseconds(3));
#else
    usleep( 1 );
#endif
#endif // _MSC_VER
}

bool mainW::checkComplete()
{
  if (this->cleaningUp) return true;
  bool ret = false;
  this->completion_mutex.lock();
  ret = this->completion_flag;
  this->completion_mutex.unlock();
  return ret;
}

void mainW::waitComplete()
{
//  if (this->cleaningUp) return;
  if ( !Qt2TB::isBlock() ) return;
  while( !this->checkComplete() )
  {
#ifdef _MSC_VER
//    Sleep( 5 );
    Sleep( 10 );
#else
#if __cplusplus > 199711L
    std::this_thread::sleep_for(std::chrono::microseconds(3));
#else
    usleep( 10 );
#endif
#endif // _MSC_VER
  }
}

void mainW::wakeUp()
{
#if QT_VERSION < 0x040000
  this->appRef->wakeUpGuiThread();
  //this->appRef->processEvents(QEventLoop::AllEvents);
#endif // QT_VERSION < 0x040000
}

void mainW::addProjectHistory(const QString& name)
{
  if( name.isEmpty() ) return;
#if QT_VERSION >= 0x040000
  int index = this->prjHistory.indexOf( name );
  if (index != -1 )
  {
    this->prjHistory.removeAt( index );
  }
#else
  QStringList::iterator it = this->prjHistory.find( name );
  if( it != this->prjHistory.end() )
  {
    this->prjHistory.erase( it );
  }
#endif // QT_VERSION >= 0x040000
  else if( this->prjHistory.size() == 10 )
  {
    this->prjHistory.pop_back();
  }
  this->prjHistory.push_front( name );

  this->history->clear();
  this->createProjectHistory( this->history, this->prjHistory );
}

void mainW::removeProjectHistory(const QString & name)
{
  if( name.isEmpty() ) return;

  QStringList curList (this->prjHistory);
  this->prjHistory.clear();
  QStringList::const_iterator itr;
  for (itr = curList.begin(); itr != curList.end(); ++itr)
  {
    if (*itr != name)
      this->prjHistory.append(*itr);
  }
  this->history->clear();
  this->createProjectHistory( this->history, this->prjHistory );
}

void mainW::createProjectHistory(QMENU* menu, QStringList& list)
{
  this->histAList.clear();
  if (!list.empty())
  {
    for( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
    {
      QString name (*it);
      QString pname (QFileInfo(name).fileName());
    
      QAction *newAction = new QAction(this);
#if QT_VERSION >= 0x040000
      newAction->setIconText( pname );
      newAction->setWhatsThis( name );
      menu->addAction(newAction);
#else
      newAction->setMenuText( pname );
      newAction->setWhatsThis( name );
      newAction->addTo(menu);
#endif // QT_VERSION >= 0x040000
      this->histAList.append(newAction);
    }
    menu->setEnabled(true);
  }
  else
    menu->setEnabled(false);
}

void mainW::saveProjectHistory(QStringList& list)
{
  if( 0 == list.size() ) return;

  QString name (this->getHome() + "/" + PROJECT_HISTORY_FILE);
  QFile prjFile( name );
#if QT_VERSION >= 0x040000
  if( prjFile.open( QIODevice::WriteOnly ) )
#else
  if( prjFile.open( IO_WriteOnly ) )
#endif // QT_VERSION >= 0x040000
  {
    QTextStream prjStream( &prjFile );
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    prjStream.setCodec(codec);
    for( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
    {
      prjStream << *it << "\n";
    }
    prjFile.close();
  }
}

void mainW::loadProjectHistory()
{
  QStringList list;

  QString name (this->getHome() + "/" + PROJECT_HISTORY_FILE);
  QFile prjFile( name );
#if QT_VERSION >= 0x040000
  if( prjFile.open( QIODevice::ReadOnly ) )
#else
  if( prjFile.open( IO_ReadOnly ) )
#endif // QT_VERSION >= 0x040000
  {
    QTextStream prjStream( &prjFile );
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    prjStream.setCodec(codec);
    while( !prjStream.atEnd() )
    {
      QString filename (prjStream.readLine());
      if( filename.length() > 0 )
        list.push_back( filename );
    }
    prjFile.close();
  }

  this->prjHistory = list; 
  this->createProjectHistory( this->history, this->prjHistory );
}

QString mainW::getHome()
{
  if(this->homeDir.isEmpty())
  {
    const char* home = getenv( HOME_ENV_NAME );
    if( NULL != home )
      this->homeDir = Qt2TB::wstring2qstring(TBWSTR::fsstr2wstring(home));
    else
      this->homeDir = ".";
  }
  return this->homeDir;
}

#if QT_VERSION < 0x040000
void mainW::setWindowTitle(QString title)
{
  this->setCaption( title );
}
#endif // QT_VERSION < 0x040000

void mainW::saveWindowsGeometry(const QString & prjname)
{
  QString name (this->getWindowsGeometryFileName(prjname));
  if( name.isEmpty() ) return;
  QFile wgmFile( name );
#if QT_VERSION >= 0x040000
  if( wgmFile.open( QIODevice::WriteOnly ) )
#else
  if( wgmFile.open( IO_WriteOnly ) )
#endif // QT_VERSION >= 0x040000
  {
    QTextStream wgmStream( &wgmFile );
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    wgmStream.setCodec(codec);
    wgmStream << "FormatVersion:2" << endl;
    this->saveGeometry(wgmStream, "mainW", this);
    this->saveGeometry(wgmStream, this->browserw->getWindowName(), this->browserw->parentWidget());
    this->saveGeometry(wgmStream, this->codew->getWindowName(), this->codew->parentWidget());
    this->saveGeometry(wgmStream, this->errorw->getWindowName(), this->errorw->parentWidget());
    this->saveGeometry(wgmStream, this->interpreterw->getWindowName(), this->interpreterw->parentWidget());
    this->saveGeometry(wgmStream, this->logw->getWindowName(), this->logw->parentWidget());
    this->saveGeometry(wgmStream, this->pogw->getWindowName(), this->pogw->parentWidget());
    this->saveGeometry(wgmStream, this->ow->getWindowName(), this->ow);
    this->saveGeometry(wgmStream, this->tw->getWindowName(), this->tw);
    wgmFile.close();
  }
}

void mainW::loadWindowsGeometry(const QString& prjname)
{
  QString name (this->getWindowsGeometryFileName(prjname));
  if( name.isEmpty() ) return;
  QFile wgmFile( name );
#if QT_VERSION >= 0x040000
  if( !wgmFile.open( QIODevice::ReadOnly ) ) return;
#else
  if( !wgmFile.open( IO_ReadOnly ) ) return;
#endif // QT_VERSION >= 0x040000

  QTextStream wgmStream ( &wgmFile );
  QString version (wgmStream.readLine());
  wgmFile.close();

  if( version == "FormatVersion:2" )
    this->loadWindowsGeometryV2(prjname);
  else
    this->loadWindowsGeometryV1(prjname);
}

void mainW::loadWindowsGeometryV1(const QString& prjname)
{
  QString name (this->getWindowsGeometryFileName(prjname));
  if( name.isEmpty() ) return;
  QFile wgmFile ( name );
#if QT_VERSION >= 0x040000
  if( !wgmFile.open( QIODevice::ReadOnly ) ) return;
#else
  if( !wgmFile.open( IO_ReadOnly ) ) return;
#endif // QT_VERSION >= 0x040000

  QTextStream wgmStream( &wgmFile );
  QTextCodec * codec = QTextCodec::codecForName("UTF-8");
  wgmStream.setCodec(codec);
  if( this->loadMainGeometry(wgmStream, this) )
  {
    this->loadQWSGeometry(wgmStream, this->browserw);
    this->loadQWSGeometry(wgmStream, this->codew);
    this->loadQWSGeometry(wgmStream, this->errorw);
    this->loadQWSGeometry(wgmStream, this->interpreterw);
    this->loadQWSGeometry(wgmStream, this->logw);
    this->loadQWSGeometry(wgmStream, this->pogw);
    this->loadGeometry(wgmStream, this->ow);
    this->loadGeometry(wgmStream, this->tw);
    this->codew->parentWidget()->lower();
  }
  wgmFile.close();
//  this->repaint();
  this->update();
}

void mainW::loadWindowsGeometryV2(const QString& prjname)
{
  QString name (this->getWindowsGeometryFileName(prjname));
  if( name.isEmpty() ) return;
  QFile wgmFile( name );
#if QT_VERSION >= 0x040000
  if( !wgmFile.open( QIODevice::ReadOnly ) ) return;
#else
  if( !wgmFile.open( IO_ReadOnly ) ) return;
#endif // QT_VERSION >= 0x040000

  QTextStream wgmStream ( &wgmFile );
  QTextCodec * codec = QTextCodec::codecForName("UTF-8");
  wgmStream.setCodec(codec);

  QMap<QString, QString> wgmMap;
  while( !wgmStream.atEnd() )
  {
    QString tmp (wgmStream.readLine());
    if( tmp.isEmpty() ) continue;
#if QT_VERSION >= 0x040000
    int index = tmp.indexOf( ':' );
#else
    int index = tmp.find( ':' );
#endif // QT_VERSION >= 0x040000
    if( index == -1 ) continue;
#if QT_VERSION >= 0x040000
    QString key (tmp.left( index ).simplified());
    QString value (tmp.right( tmp.length() - index - 1 ).simplified());
#else
    QString key (tmp.left( index ).stripWhiteSpace());
    QString value (tmp.right( tmp.length() - index - 1 ).stripWhiteSpace());
#endif // QT_VERSION >= 0x040000
    wgmMap[ key ] = value;
  }
  wgmFile.close();

  if( this->loadMainGeometry(wgmMap, this) )
  {
    this->loadQWSGeometry(wgmMap, this->browserw->getWindowName(), this->browserw);
    this->loadQWSGeometry(wgmMap, this->codew->getWindowName(), this->codew);
    this->loadQWSGeometry(wgmMap, this->errorw->getWindowName(), this->errorw);
    this->loadQWSGeometry(wgmMap, this->interpreterw->getWindowName(), this->interpreterw);
    this->loadQWSGeometry(wgmMap, this->logw->getWindowName(), this->logw);
    this->loadQWSGeometry(wgmMap, this->pogw->getWindowName(), this->pogw);
//    this->loadGeometry(wgmMap, this->ow->getWindowName(), this->ow);
//    this->loadGeometry(wgmMap, this->tw->getWindowName(), this->tw);
    this->codew->parentWidget()->lower();
#if QT_VERSION >= 0x040000
    if (!this->interpreterw->isHidden())
#else
    if (this->interpreterw->isShown())
#endif // QT_VERSION >= 0x040000
      this->interpreterTools->show();
    else
      this->interpreterTools->hide();
  }
//  this->repaint();
  this->update();
}

void mainW::saveGeometry(QTextStream& out, const QString& name, QWidget * gw)
{
  const QRect & rect = gw->geometry();
  out << name << ":";
  out << rect.left() << ",";
  out << rect.top() << ",";
  out << rect.width() << ",";
  out << rect.height() << ",";
#if QT_VERSION >= 0x040000
  out << ( !gw->isHidden() ? "1" : "0" ) << endl;
#else
  out << ( gw->isShown() ? "1" : "0" ) << endl;
#endif // QT_VERSION >= 0x040000
}

void mainW::loadQWSGeometry(QTextStream& in, QWidget * w)
{
  if( this->loadGeometry( in, w->parentWidget() ) )
#if QT_VERSION >= 0x040000
  {
    w->parentWidget()->show();
    w->show();
  }
  else
    w->parentWidget()->hide();
#else
    w->show();
  else
    w->hide();
#endif // QT_VERSION >= 0x040000
}

void mainW::loadQWSGeometry( const QMap<QString, QString>& wgmMap, const QString & key, QWidget * w)
{
//  if( this->loadGeometry( wgmMap, w->getWindowName(), w->parentWidget() ) )
  if( this->loadGeometry( wgmMap, key,  w->parentWidget() ) )
#if QT_VERSION >= 0x040000
  {
    w->parentWidget()->show();
    w->show();
  }
  else
    w->parentWidget()->hide();
#else
    w->show();
  else
    w->hide();
#endif // QT_VERSION >= 0x040000
}

bool mainW::loadGeometry(QTextStream& in, QWidget * w)
{
  int left, top, width, height, shown;
  char sep;
  in >> left >> sep >> top >> sep >> width >> sep >> height >> sep;
  in >> shown;
  w->setGeometry( left, top, width, height );
  return ( shown == 1 );
}

bool mainW::loadGeometry(const QMap<QString, QString>& wgmMap, 
                         const QString & name, QWidget * w)
{
  if( name == "" ) return false;
  if( !wgmMap.contains( name ) ) return false;
  QRect r (this->getRect( wgmMap[name] ));

  w->setGeometry( r );
  return this->getShown( wgmMap[name] );
}

bool mainW::loadMainGeometry(QTextStream& in, QWidget * w)
{
  int left, top, width, height, shown;
  char sep;
  in >> left >> sep >> top >> sep >> width >> sep >> height >> sep;
  in >> shown;

  QDesktopWidget *d = QApplication::desktop();
  const QRect & rec = d->screenGeometry();

  if( ( left + width > rec.right() ) ||
      ( top + height > rec.bottom() ) ) return false;

  w->setGeometry( left, top, width, height );
  return true;
}

bool mainW::loadMainGeometry(const QMap<QString, QString>& wgmMap, QWidget * w)
{
  if( !wgmMap.contains( "mainW" ) ) return false;
  QRect r (this->getRect( wgmMap["mainW"] ));

  QDesktopWidget *d = QApplication::desktop();
  const QRect & rec = d->screenGeometry();

#ifdef _MSC_VER
  RECT rect;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
  int left = r.left();
  int top = r.top();
  if ( left < rect.left + 4 )
    left = rect.left + 4;
  if ( top < rect.top + 30 )
    top = rect.top + 30;
  r = QRect(left, top, r.width(), r.height());
#endif // _MSC_VER

  if( ( r.left() + r.width() > rec.right() ) ||
      ( r.top() + r.height() > rec.bottom() ) ) return false;

  w->setGeometry( r );
  return true;
}

QRect mainW::getRect( const QString & str)
{
#if QT_VERSION >= 0x040000
  QStringList list (str.split( ',' ));
#else
  QStringList list (QStringList::split( ',', str ));
#endif // QT_VERSION >= 0x040000
  
  int left = list[0].toInt();
  int top = list[1].toInt();
  int width = list[2].toInt();
  int height = list[3].toInt();

  return QRect( left, top, width, height );
}

bool mainW::getShown( const QString & str)
{
#if QT_VERSION >= 0x040000
  QStringList list (str.split( ',' ));
#else
  QStringList list (QStringList::split( ',', str ));
#endif // QT_VERSION >= 0x040000
  if( list.size() < 5 ) return false;
  return ( list[4] == "1" );
}

QString mainW::getWindowsGeometryFileName(const QString& prjname)
{
  QString nm (prjname);
  if (nm.right(4) != ".prj") return QString();
  nm.replace( ".prj", ".wgm" );
  return nm;
}

void mainW::enableEditor(bool enable)
{
  if( enable )
    this->editA->setEnabled( true );
  else
    this->editA->setEnabled( false );
}

void mainW::modified(QStringList modifiedFiles)
{
  if( this->tw->doAutoSyntaxChecking() )
  {
    this->sendSyntaxCommand(modifiedFiles); 
// 20120725 -->
    this->isModified = true;
    this->mayBeModified = false;
// <-- 20120725
  }
  this->removeFilesFromGUI(modifiedFiles);
}

void mainW::selectAll_qt4(bool)
{
#if QT_VERSION >= 0x040000
  QWidget * aw = this->ws->activeSubWindow();
  if( aw != NULL )
  {
    if( aw == this->browserw->parentWidget() )
    {
      this->browserw->selectAll();
    }
    else if ( aw == this->codew->parentWidget() )
      this->codew->selectAll();
  }
#endif // QT_VERSION >= 0x040000
}
#if QT_VERSION < 0x040000
void mainW::selectAll_qt3()
{
  QWidget * aw = this->ws->activeWindow();
  if( aw != NULL )
  {
    if( aw == this->browserw )
    {
      this->browserw->selectAll();
    }
    else if ( aw == this->codew )
      this->codew->selectAll();
  }
}
#endif // QT_VERSION < 0x040000

void mainW::searchId(QString str, bool partial, bool defOnly)
{
  QStringList sel (this->browserw->getSelectedFiles());
  if (sel.isEmpty()) {
    sel = Qt2TB::getProjectFilesI();
  }
  if (!sel.isEmpty()) {
    GUIOccurenceList list (Qt2TB::SearchIdI(sel, str, partial, defOnly));
    int num = 0;
    for (GUIOccurenceList::const_iterator it = list.begin(); it != list.end(); ++it)
    {
      num++;
      GUIOccurenceInfo goif (*it);
      searchw->addOccurence(num,
                            goif.getFilename(),
                            goif.getTempFilename(),
                            goif.getLine(),
                            goif.getCol(),
                            goif.getLength(),
                            goif.isDef());
    }
    searchw->setInfo(num);
    searchw->occurencesAllDone();
  }
}

void mainW::createTimer(bool use_corba)
{
  this->timer = new QTimer(this);
  QObject::connect( this->timer, SIGNAL(timeout()), this, SLOT(apiPeriodic()));
  this->useCorba = use_corba;
  this->startTimer();

#ifdef _MSC_VER
  this->gtimer = new QTimer(this);
  QObject::connect( this->gtimer, SIGNAL(timeout()), this, SLOT(periodicAction()));
#if QT_VERSION >= 0x040000
  this->gtimer->setSingleShot(false);
  this->gtimer->start( 1000 );
#else
  this->gtimer->start( 1000, false );
#endif // QT_VERSION >= 0x040000
#endif // _MSC_VER
}

void mainW::startTimer()
{
  if ((this->timer != NULL) && !this->timer->isActive() && this->useCorba)
  {
#if QT_VERSION >= 0x040000
    this->timer->setSingleShot(false);
    this->timer->start( 200 );
#else
    this->timer->start( 200, false );
#endif // QT_VERSION >= 0x040000
  }
}

void mainW::stopTimer()
{
  if ((this->timer != NULL) && this->timer->isActive() && this->useCorba)
  {
    this->timer->stop();
  }
}

void mainW::sendUMLDiffCommand(QStringList selectedClasses)
{
#ifdef VDMPP
  this->sendCommand(new UMLDiffCMD(selectedClasses));
#endif // VDMPP
}

void mainW::setMaxLogLines(int num)
{
  if (this->logw != NULL)
    this->logw->setMaxLogLines(num);
}

void mainW::setMaxInterpreterLogLines(int num)
{
  if (this->interpreterw != NULL)
    this->interpreterw->setMaxLogLines(num);
}

void mainW::setEncoding(const QString & cnm)
{
  QString codecName (QTLOCALE::MenuNameToCodecName(cnm));

#if QT_VERSION >= 0x040000
#if QT_VERSION >= 0x050000
  QTextCodec* pCodec = QTextCodec::codecForName (codecName.toLatin1());
#else
  QTextCodec* pCodec = QTextCodec::codecForName (codecName.toAscii());
#endif // QT_VERSION >= 0x050000
#else
  QTextCodec* pCodec = QTextCodec::codecForName (codecName);
#endif // QT_VERSION >= 0x040000

  if( NULL != pCodec )
  {
#if QT_VERSION >= 0x050000
    QTextCodec::setCodecForLocale( pCodec );
#else
    QTextCodec::setCodecForTr( pCodec );
#endif // QT_VERSION >= 0x050000

    this->logWrite(QString("Setting text codec to be ") + cnm);

    QString cs (QTLOCALE::MenuNameToCharSet( cnm ));

    if( !cs.isEmpty() )
    {
      Qt2TB::setIOCharSetEnvI( cs );

// 20060425
#ifdef VDMPP
      Qt2TB::SetCharsetI( cs );
#endif // VDMPP
    }
    if (!Qt2TB::getProjectNameI().isNull())
      this->ow->loadOptions();
    this->ow->setCurrentCodecName(cnm);
    this->ow->saveOptions();
  }
}

void mainW::saveInterpreterLog()
{
  bool cont = true;
  while(cont) {
    QString filter ("Log files (*.txt)");
    QString fileNm (QtPort::QtGetSaveFileName(this, tr("Save Log File As..."), this->lastDir, filter));
    if (fileNm.isEmpty()) {
      cont = false;
    }
    else {

      if (fileNm.right(4) != ".txt") {
        fileNm.append(".txt");
      }

      bool doSave = true;
      QFile file (fileNm);
#if QT_VERSION >= 0x040000
      if (file.open(QIODevice::ReadOnly)) {
#else
      if (file.open(IO_ReadOnly)) {
#endif // QT_VERSION >= 0x040000
        file.close();
        // File already exists
        QString confText (tr( confirmFileOverwriteText ) + " " + fileNm);
        switch( QMessageBox::information( 
                this, 
                Qt2TB::GiveToolTitleI(),
                confText,
                mf(tr( okText )), mf(tr( cancelText )),
                0,      
                1 )) {   
          case 0: { // Ok
            break;
          }
          case 1: { // Cancel
            doSave = false;
            break;
          }
        }
      }
      if (doSave) {
            logWrite("save");
        // save
#if QT_VERSION >= 0x040000
        if( file.open(QIODevice::WriteOnly) ) {
#else
        if( file.open(IO_WriteOnly) ) {
#endif // QT_VERSION >= 0x040000
          if (NULL != this->interpreterw) {
            QTextStream logStream(&file);
            logStream << this->interpreterw->getLog() << endl;
            file.close();
          }
        }
        cont = false;
      }
    }
  }
}
