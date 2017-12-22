/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/errorF.cc,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE
*  *    $Date: 2006/06/13 07:02:12 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "errorF.h"
#include "interface2TB.h"

#include <iostream>
using namespace std;

/* 
 *  Constructs a errorW which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
errorW::errorW(QWidget* parent,  const char* name, WFlags fl)
#if QT_VERSION >= 0x040000
  : QWidget(parent, fl), 
#else
  : QWidget(parent, name, fl), 
#endif // QT_VERSION >= 0x040000
    currentIndex(-1),
    numErrors(0),
    numWarnings(0)
{
  this->setWindowName((name == NULL) ? "errorW" : name);

#if QT_VERSION >= 0x040000
  this->setWindowTitle(tr("Error List"));
#else
  if (!name) setName("errorW");
  this->setCaption(tr("Error List"));
#endif // QT_VERSION >= 0x040000

  QVBoxLayout* layout = new QVBoxLayout( this ); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addLayout( this->createErrorListPart( this ) );
  layout->addWidget( this->createErrorDescription( this ) );
}

bool errorW::event (QEvent * e)
{
#if QT_VERSION >= 0x040800
#ifdef __APPLE_CC__
  if (e->type() == QEvent::Paint) {
    if (this->pcount < 2) {
      this->repaint();
      this->pcount++;
    }
    else {
      this->pcount = 0;
    }
  }
#endif // __APPLE_CC__
#endif // QT_VERSION >= 0x040000
  return QWidget::event(e);
}

QLayout* errorW::createErrorListPart( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addLayout( this->createButtonPart( parent ) );
  layout->addWidget( this->createErrorList( parent ) );
  return layout;
}

QLayout* errorW::createButtonPart( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addWidget( this->createPrevButton( parent ) );
  layout->addWidget( this->createNextButton( parent ) );
  return layout;
}

QWidget* errorW::createPrevButton(QWidget* parent)
{
  QToolButton* button = new QToolButton(parent);
  button->setText( tr( "<" ) );

#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Ctrl - click to previous error in class" ) );
#else
  button->setTextLabel( tr( "<" ) );
  QToolTip::add( button, tr( "Ctrl - click to previous error in class" ) );
#endif // QT_VERSION >= 0x040000

  QObject::connect(button, SIGNAL(pressed()), this, SLOT(previousMessage()));
  return button;
} 

QWidget* errorW::createNextButton(QWidget* parent)
{
  QToolButton* button = new QToolButton(parent);
  button->setText(tr( ">" ));

#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Ctrl - click to next error in class" ) );
#else
  button->setTextLabel( tr( ">" ) );
  QToolTip::add( button, tr( "Ctrl - click to next error in class" ) );
#endif // QT_VERSION >= 0x040000

  QObject::connect(button, SIGNAL(pressed()), this, SLOT(nextMessage()));
 return button;
} 

QWidget* errorW::createErrorList(QWidget* parent)
{
#if QT_VERSION >= 0x040000
  QListWidget* lb = new QListWidget(parent);
  QObject::connect(lb, SIGNAL(currentRowChanged(int)), this, SLOT(errorMessageSelected(int)));
#else
  QListBox* lb = new QListBox( parent );
  QObject::connect(lb, SIGNAL(selected(int)), this, SLOT(errorMessageSelected(int)));
  QObject::connect(lb, SIGNAL(highlighted(int)), this, SLOT(errorMessageSelected(int)));
#endif // QT_VERSION >= 0x040000

  this->errorMessageList = lb;
  return lb;
}

QWidget* errorW::createErrorDescription( QWidget* parent )
{
  QTextEdit* te = new QTextEdit(parent);

#if QT_VERSION >= 0x040000
  te->setReadOnly(true);
  te->setMinimumHeight(100);
  te->setMaximumHeight(150);
  te->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#else
  te->setTextFormat(Qt::PlainText);
  te->setReadOnly(true);
  te->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, 
                                QSizePolicy::Minimum, 
                                te->sizePolicy().hasHeightForWidth()));
#endif // QT_VERSION >= 0x040000

  this->errorDescription = te;
  return te;
}
                                                                                
void errorW::addErrorMessage(int errnum,
                             const QString & filename,
                             int line, 
                             int col,
                             const QString & message,
                             const QString & tempFilename)
{
  ErrorInfo ei (filename, tempFilename, line, col, message, errnum); 

#if QT_VERSION >= 0x040000
  this->errorMessageList->addItem(ei.getErrorMessage());
#else
  this->errorMessageList->insertItem(ei.getErrorMessage());
#endif // QT_VERSION >= 0x040000

  this->errors.append(ei);
}

void errorW::removeMessagesOfFile(const QString & removedFile)
{
#if QT_VERSION >= 0x040000
  QList<ErrorInfo>::iterator eit = this->errors.begin();
#else
  QValueList<ErrorInfo>::iterator eit = this->errors.begin();
#endif // QT_VERSION >= 0x040000

  int index = 0;
  while(eit != this->errors.end())
  {
    ErrorInfo ei (*eit);
    if (ei.getTitle() == removedFile)
    {
#if QT_VERSION >= 0x040000
      eit = this->errors.erase(eit);
      QListWidgetItem * item = this->errorMessageList->item(index);
      if (item != 0)
        this->errorMessageList->removeItemWidget (item);
#else
      eit = this->errors.remove(eit);
      this->errorMessageList->removeItem(index);
#endif // QT_VERSION >= 0x040000
    }
    else
    {
      eit++;
      index++;
    }
  }
  this->currentIndex = -1;
}

void errorW::errorMessageSelected(int selectedIndex)
{
  if ((selectedIndex >= 0) && (selectedIndex < (int)(this->errors.count())))
  {
    this->currentIndex = selectedIndex;
    this->showDescription(this->currentIndex);
  }
}

void errorW::showDescription( int index )
{
  if ((index >= 0) && (index < (int)(this->errorMessageList->count())))
  {
    ErrorInfo ei (this->errors[index]);

    this->errorDescription->clear();

#if QT_VERSION >= 0x040000
    if (index != this->errorMessageList->currentRow())
      this->errorMessageList->setCurrentRow(index);

    this->errorDescription->insertPlainText(ei.getDescription());
//    this->errorDescription->update();
#else
    this->errorDescription->setText(ei.getDescription());
    this->errorDescription->repaint();
    this->errorMessageList->setSelected(index, true);
    this->errorMessageList->centerCurrentItem();
    this->errorMessageList->repaint();
#endif // QT_VERSION >= 0x040000

    QString name (ei.getDescription().section( "\"", 1, 1));
    int length = (( name.length() > 0 ) ? name.length() : 1);

    if (!ei.getTitle().isEmpty())
      emit resetFilePosition(ei.getTitle(), ei.getFilename(), ei.getLine(), ei.getCol() + 1, length);
  }
}

void errorW::clearAllMessages()
{
  this->errorMessageList->clear();
  this->errorDescription->clear();
  this->errors.clear();
  this->currentIndex = -1;
  this->numErrors = 0;
  this->numWarnings = 0;
  this->setCaption( tr( "Error List" ) );
}

void errorW::nextMessage()
{
  if (this->currentIndex + 1 < (int)(this->errors.count())) {
    this->currentIndex++;
#if QT_VERSION >= 0x040000
    this->errorMessageList->setCurrentRow(this->currentIndex);
#else
    this->showDescription(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void errorW::previousMessage()
{
  if (this->currentIndex > 0) {
    this->currentIndex--;
#if QT_VERSION >= 0x040000
    this->errorMessageList->setCurrentRow(this->currentIndex);
#else
    this->showDescription(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void errorW::firstMessage()
{
  if (!this->errors.empty()) {
    this->currentIndex = 0;
#if QT_VERSION >= 0x040000
    this->errorMessageList->setCurrentRow(this->currentIndex);
#else
    this->showDescription(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void errorW::lastMessage()
{
  if (!this->errors.empty()) {
    this->currentIndex = this->errors.count() - 1;
#if QT_VERSION >= 0x040000
    this->errorMessageList->setCurrentRow(this->currentIndex);
#else
    this->showDescription(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void errorW::setCaption(const QString & msg)
{
  QString caption1;
  QString caption2;
  if (this->numErrors != 0 || this->numWarnings != 0)
  {
    QString fm1 (QString( ": %1 ") + (this->numErrors == 1 ? "Error" : "Errors"));
    QString fm2 (QString( ", %1 ") + (this->numWarnings == 1 ? "Warning" : "Warnings"));

#if QT_VERSION >= 0x040000
    QString fme (tr( fm1.toLatin1().data() ));
    QString fmw (tr( fm2.toLatin1().data() ));
#else
    QString fme (tr( fm1 ));
    QString fmw (tr( fm2 ));
#endif // QT_VERSION >= 0x040000

    caption1 = (fme.arg( this->numErrors ));
    caption2 = (fmw.arg( this->numWarnings ));
  }
#if QT_VERSION >= 0x040000
  this->setWindowTitle(msg + caption1 + caption2);
#else
  QWidget::setCaption(msg + caption1 + caption2);
#endif // QT_VERSION >= 0x040000
}

void errorW::messagesAllDone()
{
  this->firstMessage();
}

void errorW::setInfo(int currErrors, int currWarnings)
{
  this->numErrors += currErrors;
  this->numWarnings += currWarnings;
  this->setCaption( tr( "Error List" ) );
}
  
/*  
 *  Destroys the object and frees any allocated resources
 */
errorW::~errorW()
{
  this->clearAllMessages();
}

ErrorInfo::ErrorInfo(const QString & fname,
                     const QString & tempFilename,
                     int line,
                     int col,
                     const QString & errorDesc,
                     int errnum)
{
  this->title = fname;
  if (!tempFilename.isEmpty())
    this->filename = tempFilename;
  else
    this->filename = fname;
  this->lineNum = line;
  this->colNum = col;
  this->description = errorDesc;
  this->num = errnum;
}

QString ErrorInfo::getErrorMessage() const
{
  return getErrorMessage(this->num);
}

QString ErrorInfo::getErrorMessage(int num) const
{
  QString numStr;
  numStr.sprintf("(%d): ", num);

  QString posStr;
  if (this->lineNum == 0 && this->colNum == -1) {
    posStr = QString(": No precise position information available");
  }
  else {
    QString tmpStr;
    tmpStr.sprintf(" Line %d, Column %d", this->lineNum, this->colNum + 1);
//    posStr = filename + tmpStr;
    posStr = title + tmpStr;
  }
  return numStr + posStr;
}

void errorW::setBusy(bool busy)
{
}

void errorW::setTextFont(const QFont & font)
{
  this->errorMessageList->setFont(font);
  this->errorDescription->setFont(font);
}
