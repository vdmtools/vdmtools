/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/vdm.cc,v $
*  * VERSION
*  *    $Revision: 1.56 $
*  * DATE
*  *    $Date: 2006/06/13 07:02:12 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <locale.h>

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#else
#include <qregexp.h>
#include <qapplication.h>
#include <qthread.h>
#include <qmessagebox.h>
#include <qtranslator.h>
#include <qfileinfo.h>
#include <qpainter.h>
#endif // QTVER >= 4

#include "mainF.h"
#include "splashF.h"

#include "customevents.h"
#include "tbutils.h"
#include "localeutil.h"
#include "tb_exceptions.h"
#include "tb_wstring.h"
#include "ToolKit.h"
#include "cli_options.h"
#include "qtInterface.h"

#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#endif // __linux__

#ifdef __APPLE_CC__
#include <unistd.h>
#endif // __APPLE_CC__

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#ifdef __CYGWIN__
#include <unistd.h>
#endif // __CYGWIN__

#if __cplusplus >= 201103L
#if !defined( __APPLE_CC__ ) || (__APPLE_CC__ > 5658)
#include <thread>
#endif
#endif

TBOptions *cliOpt = NULL;        

static mainW * mainw = NULL;

static QTranslator *translator = NULL;

static bool quiting = false;
 
void postGUIEvent(QEvent* ev)
{
  if (quiting) return;

  if (mainw != NULL)
  {
    mainw->setComplete(false);
    QApplication::postEvent(mainw, ev);
#if QT_VERSION >= 0x040000
    QApplication::processEvents();
#else
    mainw->wakeUp();
#endif // QT_VERSION >= 0x040000
    mainw->waitComplete();
  }
}

static int Log_Event(const wchar_t * msg, int n)
{
  wstring st(msg, n); 
  QString qstr (Qt2TB::wstring2qstring(st));
#ifdef VDMPP
  if (mainw->roseIsActive())
    postGUIEvent(new RoseDiffEvent(qstr));
  else
#endif // VDMPP
    postGUIEvent(new LogWriteEvent(qstr));
  return 1;
}

static int IPLog_Event(const wchar_t * msg, int n)
{
  wstring st(msg, n); 
  QString qstr (Qt2TB::wstring2qstring(st));
  postGUIEvent(new InterpreterWriteEvent(qstr));
  return 1;
}

static int ELog_Event (const wchar_t* msg, int n)
{
  wstring st (msg, n); 
  QString qstr (Qt2TB::wstring2qstring(st));
#ifdef VDMPP
  if (mainw->roseIsAlive())
    postGUIEvent(new RoseErrorEvent(qstr));
  else
#endif // VDMPP
    postGUIEvent(new LogWriteEvent(qstr));
  return 1;
}

QString getApplicationPath( QApplication& app )
{
#ifdef __linux__
#if QT_VERSION >= 0x040000
  return app.applicationDirPath();
#else
// for Qt 3.1
//  QString name( arg0 );
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

  return QFileInfo( cdir + "/" + name ).dirPath( true );
#endif // QT_VERSION >= 0x040000
#else
  return app.applicationDirPath();
#endif // __linux__
}

void setTranslation( QApplication& app )
{
  QString path (getApplicationPath( app ));

  QString lang (QTLOCALE::getLanguage());
  if( lang == "" ) return;
  QString langfile (path + DIRSEP + "vdm_" + lang + ".qm");
  QFileInfo fi( langfile );

  if( !fi.exists() )
  {
    lang = QTLOCALE::getAltLang( lang );
    if( lang == "" ) return;
    langfile = path + DIRSEP + "vdm_" + lang + ".qm";
    QFileInfo fi2( langfile );
    if( !fi2.exists() ) return;
  }

  // initialize translation
  translator = new QTranslator( 0 );
  translator->load( langfile, "." );
  app.installTranslator( translator );
}

void drawVersion( QPixmap * pmap )
{
  QRect rect (pmap->rect());
  int right = rect.right();
  int bottom = rect.bottom();

#if QTVER >= 4
  int x = 0;
  int y = bottom * 19/78;
  int h = bottom * 12/78;
  int weight = QFont::DemiBold; // 63
#ifdef __APPLE_CC__
  int f1size = 20;
  int f2size = 16;
#else
  int f1size = 18;
  int f2size = 12;
#endif // __APPLE_CC__
#else
  int x = 0;
  int y = bottom * 28/78;
  int h = bottom * 8/78;
  int weight = QFont::Bold; // 75
  int f1size = 18;
  int f2size = 12;
#endif // QtVER >= 4

#ifdef __linux__
  QFont f("Courier", f1size, weight);
  QFont f2("Courier", f2size, weight);
#else
#ifdef __FreeBSD__
  QFont f("Courier 10 Pitch", f1size, weight);
  QFont f2("Courier 10 Pitch", f2size, weight);
#else
  QFont f("Courier New", f1size, weight);
  QFont f2("Courier New", f2size, weight);
#endif
#ifndef __APPLE_CC__
  f.setStretch( QFont::SemiCondensed );
#endif // __APPLE_CC__
#endif // __linux__

  QString title (Qt2TB::GiveToolTitleI());
  QString add;
  if( title.endsWith("Academic") )
  {
    int len = title.length();
    title = title.left(len - 9);
    add = "Academic";
  }
  else if( title.endsWith("Lite") )
  {
    int len = title.length();
    title = title.left(len - 5);
    add = "Lite";
  }
  QString titlever (title + " " + Qt2TB::GetVersionIdI());   

  QPainter paint;
  paint.begin( pmap ); 
  paint.setFont( f ); 

#if QTVER >= 4
  QColor cl (80,80,80);
  paint.setPen( cl ); 
  paint.drawText(x + 14, y, right - 8, h, Qt::AlignLeft, titlever);
#else
  paint.setPen( Qt::black ); 
  paint.drawText(x, y, right - 8, h, Qt::AlignRight, titlever);
#endif // QTVER >= 4

  if( !add.isEmpty() )
  {
    y += 20;
    paint.setPen( Qt::darkGray ); 
#if QTVER >= 4
    paint.drawText(x + 14, y, right - 8, h, Qt::AlignLeft, add);
#else
    paint.drawText(x, y, right - 8, h, Qt::AlignRight, add);
#endif // QTVER >= 4
  }

  y += 20;
  QString os;
#ifdef __APPLE_CC__
  os = "for Mac OS X";
#endif // __APPLE_CC__
#ifdef __linux__
#if QT_VERSION >= 0x040000
  os = "for Linux 2.6/3";
#else
  os = "for Linux 2.4/2.6/3";
#endif // QT_VERSION >= 0x040000
#endif //__linux__
#ifdef __SunOS__
  os = "for Solaris 10";
#endif // __SunOS__
#ifdef _MSC_VER
#if (_MSC_VER < 1500 )
  os = "for Windows 2000/XP/Vista/7";
#else
#ifdef _WIN64
  os = "for Windows 7 (64 bit)";
#else
  os = "for Windows XP/Vista/7";
#endif // _WIN64
#endif // (_MSC_VER < 1500 )
#endif // _MSC_VER
#ifdef __FreeBSD__
  os = "for FreeBSD";
#endif // __SunOS__

  paint.setFont( f2 ); 
  paint.setPen( Qt::darkBlue ); 
#if QTVER >= 4
  paint.drawText(x + 14, y, right - 8, h, Qt::AlignLeft, os);
#else
  paint.drawText(x, y, right - 8, h, Qt::AlignRight, os);
#endif // QTVER >= 4

  paint.end();
}

int main (int argc, char * argv[])
{
  //
  // Set the application locale to that of the system
  //
  setlocale(LC_ALL, "");

  // Set numelic locale to English
  TBUTILS::setLCNumericEnglish();

  // Set encoding for File System
  TBWSTR::setCodePageEnv(TBUTILS::GetDefaultCodePage());

  //
  // Create application object
  //
  QApplication app(argc,argv);

#if QT_VERSION >= 0x040000
#ifndef __linux__
  QString pdirnm (QCoreApplication::applicationDirPath () + QString("/plugins"));
  QDir pdir (pdirnm);
  if (pdir.exists())
  {
    // remove default (hard coded) plugin directory path and
    // add local plugin directory path
    QStringList paths;
    paths.append(pdirnm);
    QCoreApplication::setLibraryPaths ( paths );
  }
#endif // __linux__
#endif // QT_VERSION >= 0x04000

  // show Splash Screen
  splashW spw;
#if QT_VERSION >= 0x040000
  QPixmap tmap (spw.pixmap());
#else
  QPixmap tmap (*spw.pixmap());
#endif // QT_VERSION >= 0x040000
  drawVersion(&tmap);
  spw.setPixmap(tmap);
  spw.show(); 

#ifdef _MSC_VER
  //Sleep(10000);
#else
  //sleep(60);
  sleep(1);
#endif // _MSC_VER

  // set Translation file
  setTranslation( app );

// -->
  // set up log buffer
  lbuf.SetLog (Log_Event);
  ebuf.SetLog (ELog_Event);
  ipbuf.SetLog(IPLog_Event);

#ifdef _MSC_VER
  wcout.rdbuf(vdm_log.rdbuf());
  wcerr.rdbuf(vdm_err.rdbuf());
#endif // _MSC_VER

  // set up error handler
  (void) VDMSetErrorHandler(TB_Exception::EXC_ThrowHandler);

  // set up cli option
  GUIOptions thisGUIOptions;
  cliOpt = &thisGUIOptions;

  // Initialize all of the generated modules
  Qt2TB::InitModuleTypesI();

  // Set up the ToolKit
  type_ref_Interface if_r (new QtInterface());

  vdm_ErrorState errorState;
  type_ref_ErrorState base_state (&errorState);

  vdm_Errors * errs = new QtErrors();
  errs->vdm_InitState(base_state);
  type_ref_Errors errs_r (errs);

  vdm_Errors * exprerrs = new QtExprErrors();
  exprerrs->vdm_InitState(base_state);
  type_ref_Errors exprerrs_r (exprerrs);

  vdm_ToolKit toolkit;
  toolkit.vdm_Init (if_r, errs_r, exprerrs_r);

  // Set up the call back structure used by the specification manager
  Qt2TB::InitToolboxI(false); 
// <-

#ifdef VDMPP
  // Establish the UML mapper instance
  if (!ToolMediator::UMLT()->vdm_InitUMLMapper()) {
    QString errmsg("UML instance was not initialized");
    QMessageBox::critical(mainw, "Fatal Error", errmsg); 
    Qt2TB::CleanUpI();
    Qt2TB::ExitToolboxI(-1);
  }
#endif // VDMPP

  // Make the main window
  mainw = new mainW(app);

  // Connect closure of the main window to shutdown of the application
  QObject::connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );

  // Check that the env vars etc could be read ok
  if (!mainw->initOk()) Qt2TB::ExitToolboxI(-1);

  bool uses_corba = false;
  wstring err;
  if(!Qt2TB::InitCorbaApiI(argc, argv, err)) {
    QString errmsg ("Unable to initialise CORBA API ");
    errmsg += (Qt2TB::wstring2qstring(err));
    QMessageBox::critical(mainw, "Toolbox", errmsg); 
    Qt2TB::ExitToolboxI(0);
  }
  else
  {
    uses_corba = true;
  }

  // Load Recent Project
  mainw->loadProjectHistory();

  if (argc > 1)
  {
    QString file (Qt2TB::wstring2qstring(TBWSTR::fsstr2wstring(argv[1])));
#ifdef _MSC_VER
    file.replace("\\", "/");
#endif // _MSC_VER
    if (QFile(file).exists())
      mainw->loadProject(file);
  }

  mainw->createTimer(uses_corba);

  // Show the main window
  mainw->show();
  spw.finish( mainw ); 

//  mainw->repaint();
  mainw->update();

  // Run the app
  int result = app.exec();

  quiting = true;

  // Exiting: clean up after ourselves
  mainw->stopTimer();

  Qt2TB::TerminateCorbaApiI();

#ifdef VDMPP
  ToolMediator::UMLT()->vdm_TerminateUMLMapper();
#endif // VDMPP

  Qt2TB::CleanUpI();
  mainw->cleanUp();

#if defined( __APPLE_CC__ ) || defined( __linux__ )
#if __cplusplus >= 201103L
#if !defined( __APPLE_CC__ ) || (__APPLE_CC__ > 5658)
  std::this_thread::sleep_for(std::chrono::milliseconds (100));
#else
  usleep(100000);
#endif
#else
  usleep(100000);
#endif
#endif // __APPLE_CC__ || __linux__

  return result;
}

