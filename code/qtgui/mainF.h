/***
*  * WHAT
*  *    Main Window for the toolbox
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/mainF.h,v $
*  * VERSION
*  *    $Revision: 1.57 $
*  * DATE
*  *    $Date: 2006/06/30 02:40:33 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#ifndef __mainF_h__
#define __mainF_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#define QMENU QMenu
#define QLIST QList
#define QACTIONPTRLIST QList<QAction *>
#define QMDIAREA QMdiArea
#else
#include <qapplication.h>
#include <qaction.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qlistview.h>
#include <qintdict.h>
#include <qprogressdialog.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qvbox.h>
#include <qstatusbar.h>
#include <qtimer.h>
#include <qmenubar.h>
#include <qworkspace.h>
#define QMENU QPopupMenu
#define QLIST QValueList
#define QACTIONPTRLIST QPtrList<QAction>
#define QMDIAREA QWorkspace
#endif // QTVER

#include "customevents.h"
#include "browserF.h"
#include "codeF.h"
#include "errorF.h"
#include "interface2TB.h"
#include "interpreterF.h"
#include "logF.h"
#include "optionsF.h"
//#include "referencesF.h"
#include "toolboxthread.h"
#include "toolOptionsDialog.h"
#include "pogF.h"
#include "searchF.h"

#ifdef VDMPP
#include "roseF.h"
#endif // VDMPP

#ifdef _MSC_VER
#define DIRSEP '\\'
#else
#define DIRSEP '/'
#endif

struct actionSpec 
{
  const char* text;
  int accel;
  const char* tooltip;
  const char* imgfile;
  bool tool;
};

class PogAddEvent;
class PogCleanUpEvent;

class mainW : public QMainWindow
{

  Q_OBJECT
public:
  mainW(QApplication &app);
  ~mainW();

  // Functions used by EventToCommands in vdm.cc

  void pogAdd(PogAddEvent*);
  void pogCleanUp(PogCleanUpEvent*);
  void pogUpdateFilters();

  // Other public functions
  void cleanUp();                         // Used in vdm.cc 
  void openProject(const QString & file);
  QString getTextCodecName();
  QString getDirectory();
  QString getXMIFile();

protected:
  // Event handlers
  virtual void closeEvent (QCloseEvent *);
  virtual bool event ( QEvent * e );

  // Auxiliary functions
//  void viewFile(QListViewItem*, bool);
  void checkProjectIsSaved();
  bool checkForDir(const QString& s);
  void clearWindows(bool);

public:
  void loadProject(const QString& filename);
  void setApp(QApplication*);
  bool checkEnvVars(QApplication &app);
  bool initOk();
  QPixmap getImage(const QString&, bool optional);

  //  virtual bool eventFilter(QObject *o, QEvent *e);

  // Following functions used by interpreterF.cc

  void setComplete( bool complete );
  bool checkComplete();
  void waitComplete();
//  void setBlock( bool mode );
//  bool isBlock();
  void wakeUp();

  void loadProjectHistory();

#ifdef VDMPP
  QString getUMLLibrary();
  bool roseIsActive();
  bool roseIsAlive();
#endif // VDMPP

  static QString mf( const QString & );

  void createTimer(bool use_corba);
  void startTimer();
  void stopTimer();

  void setLastDir(const QString & file);
  QString getLastDir();

signals:
  void refreshButtons(bool);

public slots:
  void apiPeriodic();
  void logWrite(QString);    

  void enableBr(int);
  void disableBr(int);
  void deleteBr(int);
  void periodicAction();
  void modified(QStringList);
  void sendUMLDiffCommand(QStringList);
  void setMaxLogLines(int);
  void setMaxInterpreterLogLines(int);
  void saveInterpreterLog();

private slots:
  // Slots for project menu
  void newProject_qt4(bool);
  void loadProject_qt4(bool);
  void saveProject_qt4(bool);
  void saveAsProject_qt4(bool);
  void addProject_qt4(bool);
  void removeProject_qt4(bool);
  void projectOptions_qt4(bool);
  void toolOptions_qt4(bool);
#if QTVER < 4
  void newProject_qt3();
  void loadProject_qt3();
  void saveProject_qt3();
  void saveAsProject_qt3();
  void addProject_qt3();
  void removeProject_qt3();
  void projectOptions_qt3();
  void toolOptions_qt3();
#endif // Q4VER < 4

  void projectHistory_qt4( QAction * );
  void projectPath_qt4( QAction * );
#if QTVER < 4
  void projectHistory_qt3( int id );
  void projectPath_qt3( int id );
#endif // Q4VER < 4

  // Slots for file menu
  void newFile_qt4(bool);
  void externalEditor_qt4(bool);
  void fileClose_qt4(bool);
  void fileCloseAll_qt4(bool);
  void selectAll_qt4(bool);
#if QTVER < 4
  void newFile_qt3();
  void externalEditor_qt3();
  void fileClose_qt3();
  void fileCloseAll_qt3();
  void selectAll_qt3();
#endif // Q4VER < 4

  void openEditor(QString, int, int);

  // Slots for windows menu
  void browser_qt4(bool);
  void codeInspector_qt4(bool);
  void errorlist_qt4(bool);
  void interpreterWindow_qt4(bool);
  void references_qt4(bool);
  void log_qt4(bool);
  void rose_qt4(bool);
  void pog_qt4(bool);
  void search_qt4(bool);
#if QTVER < 4
  void browser_qt3();
  void codeInspector_qt3();
  void errorlist_qt3();
  void interpreterWindow_qt3();
  void references_qt3();
  void log_qt3();
  void rose_qt3();
  void pog_qt3();
  void search_qt3();
#endif // Q4VER < 4

  // Slots for actions menu
  void syntax_qt4(bool);
  void type_qt4(bool);
  void cpp_qt4(bool);
  void pretty_qt4(bool);
  void java_qt4(bool);
  void java2vdm_qt4(bool);
  void genPOG_qt4(bool);
  void run_qt4(bool);
  void callLog_qt4(bool);
#if QTVER < 4
  void syntax_qt3();
  void type_qt3();
  void cpp_qt3();
  void pretty_qt3();
  void java_qt3();
  void java2vdm_qt3();
  void genPOG_qt3();
  void run_qt3();
  void callLog_qt3();
#endif // Q4VER < 4

  void executeCommand(QString);

  void setCallLogMenu();

  // Slots for interpreter menu
  void ipInit_qt4(bool);
  void ipStep_qt4(bool);
  void ipStepin_qt4(bool);
  void ipSingle_qt4(bool);
  void ipContinue_qt4(bool);
  void ipFinish_qt4(bool);
  void ipUp_qt4(bool);
  void ipDown_qt4(bool);
  void ipStop_qt4(bool);
#if QTVER < 4
  void ipInit_qt3();
  void ipStep_qt3();
  void ipStepin_qt3();
  void ipSingle_qt3();
  void ipContinue_qt3();
  void ipFinish_qt3();
  void ipUp_qt3();
  void ipDown_qt3();
  void ipStop_qt3();
#endif // Q4VER < 4

  // Slots for help menu
  void about_qt4(bool);
  void aboutqt_qt4(bool);
#if QTVER < 4
  void about_qt3();
  void aboutqt_qt3();
#endif // Q4VER < 4
//  void statusHint(int itemId);

  // Other slots
  void checkButtons();
  void classBrowserShown();
  void resetFilePosition(QString, QString, int, int, int);
  void resetFilePosition(QString, int, int);
  void setBreakPoint(); 
#if QTVER < 4
  void setWindowTitle(QString);
#endif // Q4VER < 4
  void checkActions();
  void setFont(QFont);
  void setTextCodec(QString);
  void enableEditor(bool);
  void searchId(QString, bool, bool);

// Instance variables
private:

  // GUI related instance variables
  QApplication* appRef;
  QMDIAREA * ws;
  QToolBar *fileTools, *windowTools, *helpTools, *actionTools, 
           *interpreterTools, *projectTools;
  QAction *interpreterA, *browserA, *logA,*codeInspectorA,*errorListA;
  QAction *syntaxA, *typeA, *cppA, *prettyA;
  QAction *pogA, *pogListA;
  QAction *searchListA;
  QAction *newFileA, *editA, *closeA, *closeAllA, *selectAllA;
#ifdef VDMPP
  QAction *roseA;
  QAction *javaA;
  QAction *java2vdmA;
#endif //VDMPP
  QAction *runA, *callLogA;
  QAction *newProjectA, *loadProjectA, *saveProjectA, *saveAsProjectA;
  QAction *addProjectA, *removeProjectA, *projectOptionsA, *toolOptionsA;
  QAction *initA, *stepA, *stepinA, *singleA, *continueA;
  QAction *stopA, *upA, *downA, *finishA;

//  QMap<int, String> hintdict; //maps long 2 wchar_t *
  QACTIONPTRLIST histAList;

  QString lastDir;
  QString lastFilter;
  QString lastPostfix;
  QString projectName;
  QString rootDir;
  QString imgDir;
  QString homeDir;

  bool envVarsOk;
  bool cleaningUp;
  bool loading;
  bool closing;

  QMENU * projectMenu;
  QMENU * fileMenu;
  QMENU * windowsMenu;
  QMENU * actionsMenu;
  QMENU * interpreterMenu;
  QMENU * helpMenu;

  QProgressBar *pb;               // progress bar in status bar
//  QLabel* statusLabel;            // text label in status bar
  QStatusBar* statusBar;            // text label in status bar
  QLabel* statusLabel2;           // text label in status bar
  optionsW* ow;                   // project options window
  browserW * browserw;            // project/class browser
  codeW * codew;                  // source window
  errorW * errorw;                // error window
  interpreterW * interpreterw;    // interpreter window
  logW * logw;                    // log window
  toolOptionsDialog* tw;          // tool options window
  pogW * pogw;                    // pog window
  searchW * searchw;              // search window

  ToolboxThread* tbThread;

  QMutex completion_mutex;
  bool completion_flag;
//  bool blocking_flag;

#ifdef VDMPP
  roseW *rosew;
  QString UML_LIBRARY;
//  friend roseW;                   // Needed for access to sendCommand
#endif // VDMPP

  QStringList prjHistory;         // Project History
  QMENU * history;

  bool stopEnabled;

  bool isModified;
  bool mayBeModified;

  QStringList delayedRemoveFiles;
  QStringList delayedRemoveModules;
  QStringList delayedAddFiles;

// Private functions
private:
  void userEvent(QEvent *);
  //void sendCommand(const Record &);
  void sendCommand(ToolboxCMD *);

  // Functions for initialization of the main window
  QAction* addAction(const actionSpec&, QMENU*, QToolBar*);
  void createProjectMenu();
  void createFileMenu();
  void createWindowsMenu();
  void createActionsMenu();
  void createInterpreterMenu();
  void createHelpMenu();
  void createProjectHistoryMenu(QMENU* menu);
  void createDocWindow();

  browserW * createBrowserWindow(QWidget * parent);
  codeW * createCodeWindow(QWidget * parent);
  errorW * createErrorWindow(QWidget * parent);
  interpreterW * createInterpreterWindow(QWidget * parent);
  logW * createLogWindow(QWidget * parent);
  pogW * createPogWindow(QWidget * parent);
  searchW * createSearchWindow(QWidget * parent);
  optionsW * createOptionsWindow(QWidget * parent);
  toolOptionsDialog * createToolOptionsDialog(QWidget * parent);

  QTimer * timer;
  QTimer * gtimer;

  void disableGUI();
  void enableGUI();
  bool toolRunning;
  bool useCorba;

#ifdef VDMPP
  void addRoseDiff(const QString &);
  void addRoseError(const QString &);
#endif // VDMPP
  void interpreterWrite(const QString &);    
  void addFiles(const QStringList &);
  void removeFiles(const QStringList &);
  void removeFilesFromGUI(const QStringList &);
  void addModules(const QStringList &);
  void addModuleAndStatus(const QString &, const QStringList &, int, int, int, int, int);
  void removeModules(const QStringList &);
  void changeModulesStatus(const QStringList &);
  void updateModuleStatus(const QString &, int, int, int, int, int);
  void changeFileState(const QString &);
  void refreshSourceWindow(const QString &, const QString &);
  void addBreakName(const QString &, int);
  void addBreakPos(const QString &, int, int, int);
  void removeBreak(int);
  void enableBreak(int);
  void disableBreak(int);
  void gotoBtLevel(int);
  void clearDebugWindow();
  void updateSelections();
  bool interpreterHasFocus();
  void setBackTrace(const QLIST<BackTraceEntry> &);

  void sendSyntaxCommand(const QStringList &);
  void sendTypecheckCommand(const QStringList &);

  // Functions used by classes qtInterface and qtErrors
  void showMessage(int, const QString &);
  void addMessage(int, const QString &, int, int, const QString &, const QString &);
  void clearAllMessages();
  void nextMessage();
  void prevMessage();
  void firstMessage();
  void lastMessage();
  void setErrorInfo(int, int);
  void messagesAllDone();
  void openFile(const QString &, const QString &, int, int, int);

  bool saveProjectAs();
  void saveNewFileAs(const QString & name);
  bool createVDMFile(const QString & fname, const QString & clmodnm);
  void addProjectHistory(const QString & name);
  void removeProjectHistory(const QString & name);
  void createProjectHistory(QMENU* menu, QStringList & list);
  void saveProjectHistory(QStringList & list);
  void saveWindowsGeometry(const QString & prjname);
  void loadWindowsGeometry(const QString & prjname);
  void loadWindowsGeometryV1(const QString & prjname);
  void loadWindowsGeometryV2(const QString & prjname);
  void saveGeometry(QTextStream &, const QString &, QWidget *);
  bool loadGeometry(QTextStream &, QWidget *);
  bool loadGeometry(const QMap<QString, QString> &, const QString &, QWidget *);
  bool loadMainGeometry(QTextStream &, QWidget *);
  bool loadMainGeometry(const QMap<QString, QString> &, QWidget *);
  void loadQWSGeometry(QTextStream &, QWidget *);
  void loadQWSGeometry(const QMap<QString, QString> &, const QString & key, QWidget *);
  QRect getRect( const QString & );
  bool getShown( const QString & );
  void setEncoding(const QString & cnm);
  
  QString getHome();
  QString getWindowsGeometryFileName(const QString & prjname);

  QString getFilter();
  void setLastFilter(const QStringList & files);
};

#endif // __mainF_h__
