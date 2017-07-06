/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/codeF.h,v $
*  * VERSION
*  *    $Revision: 1.29 $
*  * DATE
*  *    $Date: 2006/06/22 04:13:08 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __codef_h__
#define __codef_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qasciidict.h>
#include <qfile.h>
#include <qtextedit.h>
#include <qaction.h>
#endif // QTVER >= 4

class MyTextEdit : public QTextEdit
{
  Q_OBJECT

public:
  MyTextEdit(QWidget * parent) : QTextEdit(parent) {};

public slots:
  void contextMenuTriggered(QAction *);

signals:
  void setBreakPoint(); 
  void runCommand(QString);
  void ip_init();
  void ip_cont();
  void ip_step();
  void ip_stop();
  void cd_editor();

protected:
#if QTVER >= 4
  virtual void contextMenuEvent ( QContextMenuEvent * event );
#endif // QTVER >= 4

private:
  QString CMENU_PRINTX;
};

class codeW : public QWidget
{
  Q_OBJECT

public:
  codeW( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~codeW();

  void openIn(const QString &, const QString &, int, int, int, bool);
  void closeFile(const QString &);
  void closeSelectedFile();
  void closeAll();
  void externalEdit(const QStringList &, const QString &);
  void refreshWindow(const QString &, const QString &, bool);
  static int offset();
  int pages();
  void selectAll();
  void openFileWithEditor(const QString & editor,
                          const QString & format,
                          const QString & file,
                          int line, int col);
  void openCurrentFileWithEditor(const QString & editor, const QString & format);

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  void setBusy(bool flg);

protected:
  virtual void changeEvent(QEvent *);
  virtual bool event ( QEvent * e );

public slots:
  void setBreakPoint();
  void runCommand(QString);
  void ipInit();
  void ipCont();
  void ipStep();
  void ipStop();
  void cdEditor();

signals:
  void ip_init(bool);
  void ip_cont(bool);
  void ip_step(bool);
  void ip_stop(bool);
  void cd_editor(bool);
  void executeCommand(QString);
  void logWrite(QString);
  void resetFilePosition(QString, QString, int, int, int);

private:
  QTabWidget* mainTAB;

  QColor * c_header;
  QColor * c_comment;
  QColor * c_reserve;
  QColor * c_identifier;
  QColor * c_literal;
  QColor * c_poly;
  QColor * c_parenthesis;
  QColor * c_definition;

  MyTextEdit * createEdit(QWidget * parent);  
  void open(const QString &, const QString &, bool);
  void setupDefaultColor();
  void readText(QFile & file, MyTextEdit * edit, const QString & title, bool coloring); 
  QString getCurrentTitle();
#ifdef _MSC_VER
  QString ConvertToShortPathName(const QString path);
#endif // _MSC_VER

  int oldState;
  bool openAtMinimized;
  QString oldTitle;
  QString oldFilename;
  int oldLine;
  int oldCol;
  int oldLength;

protected:
// QDict ( int size=17, bool caseSensitive=TRUE, bool)
// size must be a prime number large enough to contain all the
// keys. (if 131 isnt enough can use eg: 953, 2069, 9887) 

  QMap <QString, MyTextEdit*> editMap;
  QMap <QString, QWidget*> tabMap;
  QMap <QString, QString> fileMap;

  virtual void resizeEvent ( QResizeEvent * );

private:
  QString windowName;
  int pcount;
};

#endif // __codef_h__
