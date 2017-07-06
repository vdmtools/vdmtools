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
#ifndef __errorf_h__
#define __errorf_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
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
#define QLIST QValueList
#define QLISTWIDGET QListBox
#define QListWidgetItem void
#endif // QTVER >= 4

class ErrorInfo {
public:
  ErrorInfo() {};
  ErrorInfo(const QString & filename,
            const QString & tempFilename,
            int line,
            int col,
            const QString & errorDesc,
            int num);
    
  QString getErrorMessage() const;
  QString getErrorMessage(int num) const;

  QString getTitle() const { return this->title; };
  QString getFilename() const { return this->filename; };
  int getLine() const { return this->lineNum; };
  int getCol() const { return this->colNum; };
  QString getDescription() const { return this->description; };

private:
  QString title;
  QString filename;
  int lineNum;
  int colNum;
  QString description;
  int num;
};

class errorW : public QWidget
{ 
    Q_OBJECT

public:
  errorW( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~errorW();

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }

  void addErrorMessage(int, const QString&, int, 
                       int, const QString&, const QString&);
  void clearAllMessages();
  void messagesAllDone();
  void setInfo(int, int);
  void removeMessagesOfFile(const QString & removedFile);
  void setBusy(bool flg);

signals:
  void resetFilePosition(QString, QString, int, int, int);

protected:
  virtual bool event ( QEvent * e );

private:
  QLayout* createErrorListPart( QWidget* parent );
  QLayout* createButtonPart( QWidget* parent );
  QWidget* createPrevButton( QWidget* parent );
  QWidget* createNextButton( QWidget* parent );
  QWidget* createErrorList( QWidget* parent );
  QWidget* createErrorDescription( QWidget* parent );

public slots:
  void nextMessage();
  void previousMessage();
  void firstMessage();
  void lastMessage();
  virtual void setCaption ( const QString & );

private slots:
  void errorMessageSelected(int);

private:
  QLIST<ErrorInfo> errors;
  int currentIndex;
  int numErrors;
  int numWarnings;
  void showDescription(int);

  QLISTWIDGET* errorMessageList;
  QTextEdit* errorDescription;

  QString windowName;
  int pcount;
};

#endif // __errorf_h__
