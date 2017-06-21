/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/errorF.h,v $
*  * VERSION
*  *    $Revision: 1.12 $
*  * DATE
*  *    $Date: 2006/04/04 01:06:13 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __searchF_h__
#define __searchF_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#define QLIST QList
#define QLISTWIDGET QListWidget
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#define QLIST QValueList
#define QLISTWIDGET QListBox
#define QListWidgetItem void
#endif // QTVER >= 4

class OccurenceInfo {
public:
  OccurenceInfo() {};
  OccurenceInfo(const QString & filename,
                const QString & tempFilename,
                int line,
                int col,
                int length,
                int num,
                bool def);
    
  QString getTitle() const { return this->title; };
  QString getFilename() const { return this->filename; };
  int getLine() const { return this->mLine; };
  int getCol() const { return this->mCol; };
  int getLength() const { return this->mLength; };
  bool isDef() const { return this->mDef; } ;

private:
  QString title;
  QString filename;
  int mLine;
  int mCol;
  int mLength;
  int mNum;
  bool mDef;
};

class searchW : public QWidget
{ 
    Q_OBJECT

public:
  searchW( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~searchW();

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }

  void addOccurence(int num,
                     const QString & filename,
                     const QString & tempFilename,
                     int line,
                     int col,
                     int length,
                     bool def);
  void clearAllOccurences();
  void occurencesAllDone();
  void setInfo(int);
  void setBusy(bool flg);

protected:
  virtual bool event ( QEvent * e );

signals:
  void resetFilePosition(QString, QString, int, int, int);
  void searchId(QString, bool, bool);

private:
  QLayout* createOccurenceListPart( QWidget* parent );
  QLayout* createButtonPart( QWidget* parent );
  QWidget* createPrevButton( QWidget* parent );
  QWidget* createNextButton( QWidget* parent );
  QWidget* createOccurenceList( QWidget* parent );
  QLayout* createSearchInputPart( QWidget* parent );

public slots:
  void search();
  void nextOccurence();
  void previousOccurence();
  void firstOccurence();
  void lastOccurence();
  virtual void setCaption ( const QString & );

private slots:
  void occurenceSelected(int);

private:
  QLIST<OccurenceInfo> occurences;
  int currentIndex;
  int numOccurences;
  void showOccurence( int index );

  QLISTWIDGET* occurenceList;

  QLineEdit* if_searchId;
  QCheckBox* if_partialSearch;
  QCheckBox* if_definitionOnly;

  QString windowName;
  int pcount;
};

#endif // __searchF_h__
