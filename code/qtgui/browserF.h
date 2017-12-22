/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/browserF.h,v $
*  * VERSION
*  *    $Revision: 1.23 $
*  * DATE
*  *    $Date: 2006/06/29 05:16:37 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __browserf_h__
#define __browserf_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#define QTREEWIDGET QTreeWidget
#define QTREEWIDGETITEM QTreeWidgetItem
#define QTREEWIDGETITEMLIST QList<QTreeWidgetItem *>
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qwidget.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qfileinfo.h>
#include <qaction.h>
#define QTREEWIDGET QListView
#define QTREEWIDGETITEM QListViewItem
#define QTREEWIDGETITEMLIST QPtrList<QListViewItem>
#define QTreeWidgetItem void
#endif // QTVER

class StatusType {
public:
  enum status { status_none, status_error, status_ok, status_pos, status_def };
  status syntax, type, cg, jcg, pp;

  // These are necessary to avoid internal error in VC5.0.
  StatusType() : syntax(status_none), type(status_none), cg(status_none),
                 jcg(status_none), pp(status_none) {};
  StatusType(const StatusType & st) :
   syntax(st.syntax), type(st.type), cg(st.cg), jcg(st.jcg), pp(st.pp) {};
  StatusType & operator=(const StatusType & o)
  {
    this->syntax = o.syntax;
    this->type = o.type;
    this->cg = o.cg;
    this->jcg = o.jcg;
    this->pp = o.pp;
    return *this;
  };
  StatusType(status s1, status s2, status s3, status s4, status s5) :
   syntax(s1), type(s2), cg(s3), jcg(s4), pp(s5) {};
};

class browserW : public QWidget
{ 
  Q_OBJECT

public:
  browserW( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~browserW();

  void updateSelectedModules();
  void selectAll();

  QStringList getSelectedFiles(bool abs = false);
  QStringList getSelectedModules();
  bool isJavaClassSelected();
  bool anyFilesOrModulesAreSelected();

  void clear();
  void addModule(const QString &, const QStringList &, const StatusType &);
  void removeModule(const QString& module);
  void removeModules(const QStringList& modules);

  void updateModuleStatus(const QString&, const StatusType &);
  void checkModifiedModules();
  void refreshProjectName();
  QString getFilePath(const QString& filename);
  void setImageDir( const QString & dir );
  void newProject();
  void addFiles(const QStringList& newFiles);
  void removeFiles(const QStringList& removedFiles);

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  void setBusy(bool flg);
  void setTextFont(const QFont & font);

protected:
  virtual bool event ( QEvent * e );

signals:
  void viewFileSignal(QString);
  void resetFilePosition(QString, QString, int, int, int);
  void setMainWindowTitle(QString);
  void checkActions();
  void logWrite(QString);
  void modified(QStringList);
  void br_syntax(bool);
  void br_typecheck(bool);
  void br_integrity(bool);
  void br_cpp(bool);
  void br_java(bool);
  void br_j2v(bool);
  void br_pretty(bool);
  void br_option(bool);
  void br_selectAll(bool);

public slots:
  void refreshFiles();

private slots:
  void viewModuleFile_qt4(QTreeWidgetItem*, int);
  void viewProjectFile_qt4(QTreeWidgetItem*, int);
#if QTVER < 4
  void viewProjectFile_qt3(QListViewItem*);
  void viewModuleFile_qt3(QListViewItem*);
#endif // QTVER < 4
  void selectionChanged();

  void contextMenuTriggered(QAction *);

private:
  QString getFile(QTREEWIDGETITEM* node, const QString& filename);
  void addModuleToFile(const QString& module, const QString& file);
#ifdef VDMPP
  QTREEWIDGETITEM* findJavaParent();
#endif //VDMPP
  QTREEWIDGETITEM* findVDMParent();
  void updateBrowser(QTREEWIDGET* lv, bool isVDM);
  void updateProjectBrowser();
  void updateClassBrowser();
#ifdef VDMPP
  void updateJavaBrowser();
#endif //VDMPP
  QTREEWIDGETITEM* findModule(const QString &);
  QStringList getFilesInPackage(QTREEWIDGETITEM*);
  QTREEWIDGETITEMLIST getSelectedNodes(const QTREEWIDGETITEM* );
  void removeModuleFromFile(const QString&, const QString&);
  QWidget * createBrowserWidget( QWidget * parent );
  QWidget * createBrowserTab( QWidget * parent );
#ifdef VDMPP
  QWidget * createClassWidget( QWidget * parent );
  QWidget * createClassTab( QWidget * parent );
  QWidget * createVDMViewTab( QWidget * parent );
  QWidget * createJavaViewTab( QWidget * parent );
  QTREEWIDGET * createClassListView( QWidget * parent );
  QTREEWIDGET * createJavaListView( QWidget * parent );
#endif // VDMPP
#ifdef VDMSL
  QWidget * createModuleWidget( QWidget * parent );
  QTREEWIDGET * createModuleListView( QWidget * parent );
#endif // VDMSL
  QTREEWIDGET * createProjectListView( QWidget * parent );
  void preloadPixmap();
  QString getPackageName( const QString & fileName );
  void addFileNodes( QTREEWIDGETITEM* parent, const QString & packageName, bool isJava );
  void addModuleNodes( QTREEWIDGETITEM* parent, const QString & fileName, bool isJava );
  QTREEWIDGETITEM * findLVIByLabel1( QTREEWIDGETITEM *, const QString & );
  QTREEWIDGETITEM * findLVIByLabel2( QTREEWIDGETITEM *, const QString & );
  int countChilds( QTREEWIDGETITEM * );
  void addFile(QTREEWIDGETITEM* parent, const QString & fileName);
  void removeFile(QTREEWIDGETITEM* parent, const QString & fileName);
  void selectAllNodes(QTREEWIDGET *, bool);
  
  void createEmptyView();
  void createProjectListViewItem();
  QStringList getSelectedModulesOfProject();
  QStringList getSelectedFilesOfProject(bool abs);
  QStringList getSelectedModulesOfVDM();
  QStringList getSelectedFilesOfVDM(bool abs);
  QStringList getAllFilesOfProject(bool vdm);
  void updateSelectedModulesOfVDM();
#ifdef VDMPP
  QStringList getSelectedModulesOfJava();
  QStringList getSelectedFilesOfJava(bool abs);
  void updateSelectedModulesOfJava();
#endif // VDMPP
  bool isProjectManagerSelected();
  bool isVDMClassBrowserSelected();
#ifdef VDMPP
  bool isJavaClassBrowserSelected();
#endif // VDMPP

  QString imageDir;

  QPixmap nonePixmap;
  QPixmap syntaxOkPixmap;
  QPixmap syntaxErrPixmap; 
  QPixmap syntaxChangedPixmap;
  QPixmap typeOkPixmap;
  QPixmap typeErrPixmap;      
  QPixmap cppcgOkPixmap;      
  QPixmap cppcgErrPixmap;     
  QPixmap javacgOkPixmap;     
  QPixmap javacgErrPixmap;    
  QPixmap ppOkPixmap;         
  QPixmap ppErrPixmap;        
  QPixmap vdmcgOkPixmap;      
  QPixmap vdmcgErrPixmap;     

protected:
  QTabWidget* browserTW; //main tab
  QTREEWIDGET* projectLV;
  QTabWidget* classTW;
  QTREEWIDGET* classLV;
#ifdef VDMPP
  QTREEWIDGET* javaLV;
#endif // VDMPP

#if QTVER >= 4
  virtual void contextMenuEvent ( QContextMenuEvent * event );
#endif // QTVER >= 4

  QPixmap loadPixmap(const QString &);
  void setModuleStatusPixmap(QTREEWIDGETITEM*, int, StatusType::status,
                             const QPixmap & okPixmap,
                             const QPixmap & errPixmap);
  void setModuleStatus(QTREEWIDGETITEM*, const StatusType &, bool);
#ifdef VDMPP
  void addJavaModule(const QString &, const QStringList &, const StatusType &);
#endif //VDMPP
  void addVDMModule(const QString &, const QStringList &, const StatusType &);
  void checkModifiedModuleItems(QTREEWIDGET*);

  void addPackages(QTREEWIDGETITEM*, const QStringList&, bool);

  void viewFile(const QString &, const QString &);

  QString windowName;
  int pcount;
};

#endif // __browserf_h__
