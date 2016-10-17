/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/searchF.cc,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE
*  *    $Date: 2006/06/13 07:02:12 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "searchF.h"
#if QT_VERSION >= 0x040000
#else
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#endif // QT_VERSION >= 0x040000

#include <iostream>
using namespace std;

searchW::searchW( QWidget* parent,  const char* name, WFlags fl )
#if QT_VERSION >= 0x040000
  : QWidget( parent, fl ), 
#else
  : QWidget( parent, name, fl ), 
#endif // QT_VERSION >= 0x040000
    currentIndex(-1),
    numOccurences(0)
{
  this->setWindowName( (name == NULL) ? "searchW" : name );

#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( "Search Identifier" ) );
#else
  if ( !name ) setName( "searchW" );
  this->setCaption( tr( "Search Identifier" ) );
#endif // QT_VERSION >= 0x040000

  QVBoxLayout* layout = new QVBoxLayout( this ); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addLayout( this->createSearchInputPart( this ) );
  layout->addLayout( this->createOccurenceListPart( this ) );

  this->pcount = 0;
}

bool searchW::event (QEvent * e)
{
#if QT_VERSION >= 0x040800
#ifdef __darwin__
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
#endif // __darwin__
#endif // QT_VERSION >= 0x040000
  return QWidget::event(e);
}

QLayout* searchW::createOccurenceListPart( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 6 );
  layout->addLayout( this->createButtonPart( parent ) );
  layout->addWidget( this->createOccurenceList( parent ) );
  return layout;
}

QLayout* searchW::createButtonPart( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 6 );
  layout->addWidget( this->createPrevButton( parent ) );
  layout->addWidget( this->createNextButton( parent ) );
  return layout;
}

QWidget* searchW::createPrevButton( QWidget* parent )
{
  QToolButton* button = new QToolButton( parent );
  button->setText( tr( "<" ) );
#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Ctrl - click to previous occurence" ) );
#else
  button->setTextLabel( tr( "<" ) );
  QToolTip::add( button, tr( "Ctrl - click to previous occurence" ) );
#endif // QT_VERSION >= 0x040000
  QObject::connect(button, SIGNAL(pressed()), this, SLOT(previousOccurence()));
  return button;
} 

QWidget* searchW::createNextButton( QWidget* parent )
{
  QToolButton* button = new QToolButton( parent );
  button->setText( tr( ">" ) );
#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Ctrl - click to next occurence" ) );
#else
  button->setTextLabel( tr( ">" ) );
  QToolTip::add( button, tr( "Ctrl - click to next occurence" ) );
#endif // QT_VERSION >= 0x040000
  QObject::connect(button, SIGNAL(pressed()), this, SLOT(nextOccurence()));
 return button;
} 

QWidget* searchW::createOccurenceList( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QListWidget* lb = new QListWidget( parent );
  QObject::connect(lb, SIGNAL(currentRowChanged(int)), this, SLOT(occurenceSelected(int)));
#else
  QListBox* lb = new QListBox( parent );
  QObject::connect(lb, SIGNAL(selected(int)),    this, SLOT(occurenceSelected(int)));
  QObject::connect(lb, SIGNAL(highlighted(int)), this, SLOT(occurenceSelected(int)));
#endif // QT_VERSION >= 0x040000
  this->occurenceList = lb;
  return lb;
}

QLayout* searchW::createSearchInputPart( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setSpacing( 6 );
  layout->setMargin( 6 );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Identifier for search: " ) );
  layout->addWidget( label );

  QLineEdit* le = new QLineEdit( parent );
  layout->addWidget( le );
  this->if_searchId = le;

  QLabel* label2 = new QLabel( parent );
  label2->setText( tr( "\"`\" is't supported..." ) );
  layout->addWidget( label2 );

  QHBoxLayout* hlayout = new QHBoxLayout(); 

  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Partial Matching" ) );
  hlayout->addWidget( checkbox );
  this->if_partialSearch = checkbox;

  QCheckBox* checkbox2 = new QCheckBox( parent );
  checkbox2->setText( tr( "Definition Only" ) );
  hlayout->addWidget( checkbox2 );
  this->if_definitionOnly = checkbox2;

  layout->addLayout( hlayout );

  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Search" ) );
  connect( button, SIGNAL(clicked()), this, SLOT(search()));
  layout->addWidget( button );

  return layout;
}
                                                                                
void searchW::addOccurence(int occnum,
                           const QString & filename,
                           const QString & tempFilename,
                           int line, 
                           int col,
                           int length,
                           bool def)
{
  OccurenceInfo oi (filename, tempFilename, line, col, length, occnum, def); 

  QString numStr;
  if (oi.isDef())
    numStr.sprintf("+(%d): ", occnum);
  else
    numStr.sprintf(" (%d): ", occnum);

  QString tmpStr;
  tmpStr.sprintf(" Line %d, Column %d", line, col);

  QString posStr;
// 20111201 -->
  //posStr = filename + tmpStr;
  posStr = QFileInfo(filename).fileName() + tmpStr;
// <-- 20111201

#if QT_VERSION >= 0x040000
  this->occurenceList->addItem(numStr + posStr);
#else
  this->occurenceList->insertItem(numStr + posStr);
#endif // QT_VERSION >= 0x040000

  this->occurences.append(oi);
}

void searchW::occurenceSelected(int selectedIndex)
{
  if ((selectedIndex >= 0) && (selectedIndex < (int)(this->occurences.count())))
  {
    this->currentIndex = selectedIndex;
    this->showOccurence(this->currentIndex);
  }
}

void searchW::showOccurence( int index )
{
  if ((index >= 0) && (index < (int)(this->occurenceList->count())))
  {
#if QT_VERSION >= 0x040000
    if (index != this->occurenceList->currentRow())
      this->occurenceList->setCurrentRow(index);
#else
    if (index != this->occurenceList->currentItem())
      this->occurenceList->setCurrentItem(index);
#endif // QT_VERSION >= 0x040000

    OccurenceInfo oi = this->occurences[index];

    emit resetFilePosition(oi.getTitle(),
                           oi.getFilename(),
                           oi.getLine(),
                           oi.getCol(),
                           oi.getLength());
  }
}

void searchW::clearAllOccurences()
{
  this->occurenceList->clear();
  this->occurences.clear();
  this->currentIndex = -1;
  this->numOccurences = 0;
  this->setCaption( tr( "Search Identifier" ) );
}

void searchW::nextOccurence()
{
  if (this->currentIndex + 1 < (int)(this->occurences.count())) {
    this->currentIndex++;
#if QT_VERSION >= 0x040000
    this->occurenceList->setCurrentRow(this->currentIndex);
#else
    this->showOccurence(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void searchW::previousOccurence()
{
  if (this->currentIndex > 0) {
    this->currentIndex--;
#if QT_VERSION >= 0x040000
    this->occurenceList->setCurrentRow(this->currentIndex);
#else
    this->showOccurence(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void searchW::firstOccurence()
{
  if (!this->occurences.empty()) {
    this->currentIndex = 0;
#if QT_VERSION >= 0x040000
    this->occurenceList->setCurrentRow(this->currentIndex);
#else
    this->showOccurence(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void searchW::lastOccurence()
{
  if (!this->occurences.empty()) {
    this->currentIndex = this->occurences.count() - 1;
#if QT_VERSION >= 0x040000
    this->occurenceList->setCurrentRow(this->currentIndex);
#else
    this->showOccurence(this->currentIndex);
#endif // QT_VERSION >= 0x040000
  }
}

void searchW::setCaption(const QString & msg)
{
  QString caption;
  if (this->numOccurences != 0)
  {
    QString fm1 (QString( ": %1 ") + (this->numOccurences == 1 ? "Occurence" : "Occurences"));

#if QT_VERSION >= 0x040000
    QString fmo (tr( fm1.toLatin1().data() ));
#else
    QString fmo (tr( fm1 ));
#endif // QT_VERSION >= 0x040000

    caption = (fmo.arg( this->numOccurences ));
  }
#if QT_VERSION >= 0x040000
  this->setWindowTitle(msg + caption);
#else
  QWidget::setCaption(msg + caption);
#endif // QT_VERSION >= 0x040000
}

void searchW::occurencesAllDone()
{
  this->firstOccurence();
}

void searchW::setInfo(int nOccurences)
{
  this->numOccurences = nOccurences;
  this->setCaption( tr( "Search Identifier" ) );
}
  
/*  
 *  Destroys the object and frees any allocated resources
 */
searchW::~searchW()
{
  this->clearAllOccurences();
}

OccurenceInfo::OccurenceInfo(const QString & fname,
                             const QString & tempFilename,
                             int line,
                             int col,
                             int length,
                             int occnum,
                             bool def)
{
  this->title = fname;
  if (!tempFilename.isEmpty())
    this->filename = tempFilename;
  else
    this->filename = fname;
  this->mLine = line;
  this->mCol = col;
  this->mLength = length;
  this->mNum = occnum;
  this->mDef = def;
}

void searchW::search()
{
  bool partial = this->if_partialSearch->isChecked();
  bool defOnly = this->if_definitionOnly->isChecked();
  QString str = this->if_searchId->text();

  if (!str.isEmpty())
  {
    this->clearAllOccurences();
    emit searchId(str, partial, defOnly);
  }
}

void searchW::setBusy(bool busy)
{
}

