/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/logF.cc,v $
*  * VERSION
*  *    $Revision: 1.8 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:42 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "logF.h"

/* 
 *  Constructs a logW which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
logW::logW( QWidget* parent,  const char* name, WFlags fl )
#if QT_VERSION >= 0x040000
    : QWidget( parent, fl )
#else
    : QWidget( parent, name, fl )
#endif // QT_VERSION >= 0x040000
{
  this->setWindowName( (name == NULL) ? "logW" : name );

#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( "Log Window" ) );
#else
  if ( !name ) this->setName( "logW" );
  this->setCaption( tr( "Log Window" ) );
#endif // QT_VERSION >= 0x040000

//  this->resize( 596, 161 ); 

  QVBoxLayout* layout = new QVBoxLayout( this ); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addWidget( this->createLog( this ) );

  this->pcount = 0;
}

bool logW::event (QEvent * e)
{
#if QT_VERSION >= 0x040800
#ifdef __APPLE_CC__
  if (e->type() == QEvent::Paint)
  {
    if (this->pcount < 2)
    {
      this->repaint();
      this->pcount++;
    }
    else
      this->pcount = 0;
  }
#endif // __APPLE_CC__
#endif // QT_VERSION >= 0x040000
  return QWidget::event(e);
}

QWidget* logW::createLog( QWidget* parent )
{
  QTextEdit* te = new QTextEdit( this );

#if QT_VERSION >= 0x040000
  te->setAcceptRichText(false);
  te->document()->setMaximumBlockCount(1000);
  te->setLineWrapMode(QTextEdit::WidgetWidth);
  te->setReadOnly(true);
#else
  te->setTextFormat(Qt::PlainText);
  this->maxlines = 1000;
  te->setWordWrap(QTextEdit::WidgetWidth);
  te->setReadOnly(true);
#endif // QT_VERSION >= 0x040000

  this->logML = te;
  return te;
}

/*  
 *  Destroys the object and frees any allocated resources
 */
logW::~logW()
{
    // no need to delete child widgets, Qt does it all for us
}

void logW::write( const QString& msg )
{
  if( this->logML == NULL ) return;

  QString prompt (">>> ");

  QString nhe(msg);
  if (nhe.right(1) == "\n")
    nhe.remove(nhe.length()-1, 1);

#if QT_VERSION >= 0x040000
  this->logML->append(prompt + nhe);
#else
  if (this->maxlines > 0)
  {
    this->logML->setUpdatesEnabled(FALSE);
    while (this->maxlines < this->logML->paragraphs())
    {
      this->logML->removeParagraph(0);
    }
    this->logML->moveCursor(QTextEdit::MoveEnd, false);
    this->logML->setUpdatesEnabled(TRUE);
  }
  this->logML->append(prompt + nhe);
  this->logML->repaint();
#endif // QT_VERSION >= 0x040000
}

void logW::setBusy(bool busy)
{
}

void logW::setMaxLogLines(int num)
{
#if QT_VERSION >= 0x040000
  this->logML->document()->setMaximumBlockCount(num);
#else
  this->maxlines = num;
#endif // QT_VERSION >= 0x040000
  this->logML->repaint();
}

