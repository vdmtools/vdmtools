/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/interfacesF.h,v $
*  * VERSION
*  *    $Revision: 1.7 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:42 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __interfacesw_h__
#define __interfacesw_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#define QLISTWIDGET QListWidget
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qthread.h>
#define QLISTWIDGET QListBox
#endif // QTVER >= 4

class optionsW;

class interfacesW : public QDialog
{ 
  Q_OBJECT

public:
  interfacesW( QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
  ~interfacesW();

  QStringList getInterfaces() const;

protected:
  QLISTWIDGET* possibleLB;
  QToolButton* addallB;
  QToolButton* addB;
  QToolButton* removeB;
  QToolButton* removeallB;
  QLISTWIDGET* selectedLB;

private:
  void addPossibleInterface(const QString&);
  void addSelectedInterface(const QString&);
  void updatePossibleInterfaces();

  void resetPossibleInterfaces();
  void resetSelectedInterfaces();

  void assertInvariant();

  void createInterfacesList();

  QLayout* createListPart( QWidget* parent );
  QWidget* createPossibleInterfacesListBox( QWidget* parent );
  QLayout* createSelectButtonPart( QWidget* parent );
  QWidget* createAddButton( QWidget* parent );
  QWidget* createAddAllButton( QWidget* parent );
  QWidget* createRemoveButton( QWidget* parent );
  QWidget* createRemoveAllButton( QWidget* parent );
  QWidget* createSelectedInterfacesListBox( QWidget* parent );
  QLayout* createButtonPart( QWidget* parent );
  QWidget* createOkButton( QWidget* parent );
  QWidget* createCancelButton( QWidget* parent );

  optionsW* optionsw;
  QStringList possibleInterfaces;
  QStringList selectedInterfaces;

protected slots:
    void okClicked();
    void cancelClicked();
    void addSelected();
    void addAll();
    void removeSelected();
    void removeAll();
};

#endif // __interfacesw_h__
