/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/logF.h,v $
*  * VERSION
*  *    $Revision: 1.8 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:42 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __logf_h__
#define __logf_h__

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
#include <qtextedit.h>
#endif // QTVER >= 4

class logW : public QWidget
{ 
  Q_OBJECT

public:
  logW( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~logW();

  void write( const QString& msg );

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  void setBusy(bool flg);
  void setMaxLogLines(int num);
  void setTextFont(const QFont & font);

protected:
  virtual bool event ( QEvent * e );

private:
  QWidget* createLog( QWidget* parent );

#if QTVER < 4
  int maxlines;
#endif // QTVER < 4

  QTextEdit* logML;
  QString windowName;
  int pcount;
};

#endif // __logf_h__
