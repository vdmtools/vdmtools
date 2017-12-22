/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/pogF.h,v $
*  * VERSION
*  *    $Revision: 1.9 $
*  * DATE
*  *    $Date: 2006/06/22 04:13:08 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __pogf_h__
#define __pogf_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#define QICON QIcon
#define QTREEWIDGET QTreeWidget
#define QTREEWIDGETITEM QTreeWidgetItem
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qwidget.h>
#include <qtabwidget.h>
#include <qstring.h>
#include <qsplitter.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qtextedit.h>
#define QTREEWIDGET QListView
#define QTREEWIDGETITEM QListViewItem
#define QICON QIconSet
#define QTreeWidgetItem void
#endif // QTVER >= 4

typedef QStringList (GETAVAILTYPE)();
typedef void (SETFILTERTYPE)(const QStringList &);

struct ColumnInfo
{
  const char* name;
  bool filter;
  GETAVAILTYPE* getAvailable;
  SETFILTERTYPE* setFilter;
};

class POGQListViewItem;
class mainW;

class pogW : public QSplitter
{ 
    Q_OBJECT

public:
    pogW( mainW * mainw, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~pogW();

    QWidget* pogList;   // Widget combining pogMessageList and buttons

    QToolButton* prevB;
    QToolButton* nextB;
    QToolButton* checkB;
    QToolButton* filterB;

    QTREEWIDGET* pogMessageList;
    QTabWidget* pogFilters;
    QTextEdit* pogDescription;

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  void setBusy(bool flg);

signals:
    void resetFilePosition(QString, QString, int, int, int);
    void logWrite(QString);

public:
    static struct ColumnInfo columns[];
    
    void showCurrentIndex();
    int  getColumnIdx(int column);
    static int getColumnKeyByName(const QString & name);

protected:
    void cleanUp(const QString & modName);
    virtual bool event ( QEvent * e );

public:
    void clearAllMessages();
    void messagesAllDone();
    void cleanUp(const QStringList &);
    void addPogItem(const QString & checkMark,
                    const QString & modName,
                    const QString & memberName,
                    const QString & locationType,
                    const QString & pogType,
                    int pogNum,
                    const QString & filename,
                    const QString & tmpFilename,
                    int line,
                    int col,
                    int length,
                    const QString & pogDesc);
    void cleanupFilter();
    void setTextFont(const QFont & font);

public slots:
    void nextMessage();
    void previousMessage();
    void firstMessage();
    void lastMessage();
    void markAsChecked();
    void applyFilters();
    void updateFilters();

private slots:
    void pogSelectionChanged_qt4();
    void pogDoubleClicked_qt4(QTreeWidgetItem*, int);
#if QTVER < 4
    void pogSelectionChanged_qt3(QListViewItem*);
    void pogDoubleClicked_qt3(QListViewItem*);
#endif // QTVER < 4

private:
  QString windowName;
  int pcount;
};

//
//  Custom list box item
//

class POGQListViewItem : public QTREEWIDGETITEM
{
public:
    QString mCheckMark;
    const QString mModName;
    const QString mMemberName;
    const QString mLocationType;
    const QString mPogType;
    const int     mPogNum;
    const QString mFilename;
    const QString mTmpFilename;
    const int     mLine;
    const int     mCol;
    const int     mLength;
    const QString mPogDesc;
public:
  POGQListViewItem(pogW * pogw, 
                   const QString & checkMark,
                   const QString & modName,
                   const QString & memberName,
                   const QString & locationType,
                   const QString & pogType,
                   int pogNum,
                   const QString & filename,
                   const QString & tmpFilename,
                   int line,
                   int col,
                   int length,
                   const QString & pogDesc);
};

class pogFilterW : public QWidget
{ 
    Q_OBJECT

public:
    pogFilterW(
      const QICON & selectIcon,
      const QICON & deselectIcon,
      QWidget* parent = 0,
      const char* name = 0,
      WFlags fl = 0 );

  ~pogFilterW();

  QToolButton* addB;
  QToolButton* removeB;
  QTREEWIDGET* availableList;
  QTREEWIDGET* selectedList;

  void updateFilter();

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  void setTextFont(const QFont & font);

public slots:
  void add();
  void remove();

private:
  QString windowName;
};

#endif // __pogf_h__
