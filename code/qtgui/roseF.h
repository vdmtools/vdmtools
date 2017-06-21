/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/roseF.h,v $
*  * VERSION
*  *    $Revision: 1.11 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __rosew_h__
#define __rosew_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#define QLIST QList
#define QTREEWIDGET QTreeWidget
#define QTREEWIDGETITEM QTreeWidgetItem
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlistview.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qwhatsthis.h>
#include <qtextedit.h>
#define QLIST QValueList
#define QTREEWIDGET QListView
#define QTREEWIDGTITEM QListViewItem
#define QTreeWidgetItem void
#endif // QTVER >= 4

#include <string>
#include "interface2TB.h"

class mainW;

class roseW : public QDialog
{ 
  Q_OBJECT

public:
  roseW( mainW*, QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
  ~roseW();

  void addDiff(const QString &);
  void addError(const QString &);
  bool MapperEnabled();
  bool AddAllClasses();

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  bool IsRoseMode();
  void SetRoseMode(bool b);

signals:
  void refreshFiles();
  void sendUMLDiffCommand(QStringList);

public slots:
  void defaultToUML();
  void defaultToVDM();
  void diffSelectedClasses();
  void mapClasses();
  void selectAllClasses();
  void selectNoClasses();
  void show();

  void toggleAction_qt4(QTreeWidgetItem *, int);
#if QTVER < 4
  void toggleAction_qt3(QListViewItem *, const QPoint &, int);
#endif // QTVER < 4

private:
  std::string statusToChar(CLASS_STATUS) const;
  void addClass(const UMLClassState & st);
  QPixmap* actionToImage(MAP_ACTION action) const;
  MAP_ACTION imageToAction(const QPixmap *) const;
  QPixmap nextPixmap(const QPixmap *);
  void InitWindow(const QLIST<UMLClassState> &);
  void SetMapperEnabled(bool e);
  void CreateRoseWindow();

  QLayout * createDiffAndMapBoxLayout( QWidget * parent );
  QGroupBox * createMapBox( QWidget * parent );
  QGroupBox * createMapDirBox( QWidget * parent );
  QToolButton * createVdmUmlButton( QWidget * parent );
  QToolButton * createUmlVdmButton( QWidget * parent );
  QPushButton * createMapButton( QWidget * parent );
  QPushButton * createCancelButton( QWidget * parent );
  QGroupBox * createDiffBox( QWidget * parent );
  QLayout * createSelectLayout( QWidget * parent );
  QToolButton * createSelectAllButton( QWidget * parent );
  QToolButton * createSelectNoneButton( QWidget * parent );
  QPushButton * createDiffButton( QWidget * parent );
  QGroupBox * createOutputBox( QWidget * parent );
  QTextEdit * createOutputLE( QWidget * parent );
  QTREEWIDGET * createClassesListView( QWidget * parent );
  QVBoxLayout* createVBoxLayout( QWidget* parent );
  QHBoxLayout* createHBoxLayout( QWidget* parent );
  QGroupBox * createVGroupBox( QWidget* parent );

private:
  QTREEWIDGET* classes;
  QTextEdit* outputW;

  QPixmap* vdm2umlPixmap;
  QPixmap* uml2vdmPixmap;
  QPixmap* defaultPixmap;
  QPixmap* deletePixmap;
  mainW* mw;
  bool mapper_enabled;

private:
  QString windowName;
  bool rosemode;
};

#endif // __rosew_h__
