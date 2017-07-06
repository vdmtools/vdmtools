/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/interpreterF.cc,v $
*  * VERSION
*  *    $Revision: 1.40 $
*  * DATE
*  *    $Date: 2006/06/30 06:30:39 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "interpreterF.h"

#if QT_VERSION < 0x040000
#include <qregexp.h>
#include <qwhatsthis.h>
#include <qfile.h>
#include <qtooltip.h>
#include <qgroupbox.h>
#endif // QT_VERSION < 0x040000

#include "interface2TB.h"
#include <iostream>

#ifdef __linux__
#include <stdlib.h>
#endif // __linux__

const QString prompt = ">> ";

//BufferedQTextEdit::BufferedQTextEdit(QWidget *parent=0, const char * name=0)
BufferedQTextEdit::BufferedQTextEdit(QWidget *parent, const char * name)
#if QT_VERSION >= 0x040000
  : QTextEdit(parent)
#else
  : QTextEdit(parent, name)
#endif // QT_VERSION >= 0x040000
{
#if QT_VERSION >= 0x040000
#else
  this->setTextFormat( Qt::PlainText );
#endif // QT_VERSION >= 0x040000
}

void BufferedQTextEdit::addString(const QString& text)
{
#if QT_VERSION >= 0x040000
  if( text.simplified().length() > 0 ) {
    this->buffer.append(text.simplified());
    this->curIndex = this->buffer.count();
  }
#else
  if( text.stripWhiteSpace().length() > 0 ) {
    this->buffer.append(text.stripWhiteSpace());
    this->curIndex = this->buffer.count();
  }
#endif // QT_VERSION >= 0x040000
}

void BufferedQTextEdit::clearBuffer()
{
  this->buffer.clear();
}

void BufferedQTextEdit::keyPressEvent( QKeyEvent *e )
{
#if QT_VERSION >= 0x040000
  if( e->key() == Qt::Key_Up )
#else
  if( e->key() == Key_Up )
#endif // QT_VERSION >= 0x040000
  {
    this->cursorUp( false );
  }
#if QT_VERSION >= 0x040000
  else if( e->key() == Qt::Key_Down )
#else
  else if( e->key() == Key_Down )
#endif // QT_VERSION >= 0x040000
  {
    this->cursorDown( false );
  }
#if QT_VERSION >= 0x040000
  else if( e->key() == Qt::Key_Home )
#else
  else if( e->key() == Key_Home )
#endif // QT_VERSION >= 0x040000
  {
#if QT_VERSION >= 0x040000
    QTextCursor tc (this->textCursor());
    tc.movePosition(QTextCursor::Start);
    for (int i = 0; i < (int)prompt.length(); i++)
      tc.movePosition(QTextCursor::Right);
    this->setTextCursor(tc);
#else
//    int para, index;
//    this->getCursorPosition( &para, &index );
    this->setCursorPosition(0, prompt.length());
#endif // QT_VERSION >= 0x040000
  }
#if QT_VERSION >= 0x040000
  else if( ( e->key() == Qt::Key_Left ) || ( e->key() == Qt::Key_Backspace ) )
#else
  else if( ( e->key() == Key_Left ) || ( e->key() == Key_Backspace ) )
#endif // QT_VERSION >= 0x040000
  {
#if QT_VERSION >= 0x040000
    QTextCursor tc (this->textCursor());
    int index = tc.columnNumber();
#else
    int para, index;
    this->getCursorPosition( &para, &index );
#endif // QT_VERSION >= 0x040000
    if( index > (int)(prompt.length()) ) QTextEdit::keyPressEvent( e );
  }
#if QT_VERSION >= 0x040000
  else if(( e->key() == Qt::Key_Return ) || ( e->key() == Qt::Key_Enter ))
#else
  else if(( e->key() == Key_Return ) || ( e->key() == Key_Enter ))
#endif // QT_VERSION >= 0x040000
  {
    emit returnPressed2();
  }
  else
  {
    QTextEdit::keyPressEvent( e );
  }
}

void BufferedQTextEdit::cursorUp ( bool mark = false ) 
{
  if(curIndex>0)
    curIndex--;
  else
    curIndex = this->buffer.count() -1;
  this->showCurrent();
}

void BufferedQTextEdit::cursorDown ( bool mark = false ) 
{
  if(curIndex < (int)this->buffer.count() - 1)
    curIndex++;
  else
    curIndex = 0;
  this->showCurrent();
}

void BufferedQTextEdit::showCurrent()
{
  if (curIndex >= 0 && curIndex < (int)this->buffer.count())
  {
    this->clear();
#if QT_VERSION >= 0x040000
#if QT_VERSION >= 0x040300
    //this->textCursor().insertText(prompt + this->buffer[curIndex]);
    //this->setPlainText( prompt + this->buffer[curIndex] );
    this->setPlainText( prompt + this->buffer[curIndex] );
#else
    this->setText( prompt + this->buffer[curIndex] );
#endif
    QTextCursor tc (this->textCursor());
    tc.movePosition(QTextCursor::EndOfLine);
    this->setTextCursor(tc);
#else
    this->setText( prompt + this->buffer[curIndex] );
    this->setCursorPosition(0, text().length());
#endif // QT_VERSION >= 0x040000
  }
}

void BufferedQTextEdit::insertPrompt()
{
  this->setText( prompt );
#if QT_VERSION >= 0x040000
  QTextCursor tc (this->textCursor());
  tc.movePosition(QTextCursor::EndOfLine);
  this->setTextCursor(tc);
#else
  this->setCursorPosition(0, text().length());
#endif // QT_VERSION >= 0x040000
}

/*
 *  Constructs a interpreterW which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
interpreterW::interpreterW(QWidget* parent, const char* name, WFlags fl)
#if QT_VERSION >= 0x040000
  : QWidget( parent, fl )
#else
  : QWidget( parent, name, fl )
#endif // QT_VERSION >= 0x040000
{
  this->setWindowName( (name == NULL) ? "interpreterW" : name );

#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( "Interpreter Window" ) );
#else
  if ( !name ) setName( "interpreterW" );
  this->setCaption( tr( "Interpreter Window" ) );
#endif // QT_VERSION >= 0x040000

  QVBoxLayout* interpreterWLayout = new QVBoxLayout( this );
  interpreterWLayout->setSpacing( 0 );
  interpreterWLayout->setMargin( 0 );
  interpreterWLayout->addWidget( this->createInterpreterLE( this ) );
  interpreterWLayout->addWidget( this->createInLE( this ) );
  //interpreterWLayout->addWidget( this->createToggleB( this ) );
  interpreterWLayout->addLayout( this->createToggleAndLogOperationBox( this ) );
  interpreterWLayout->addLayout( this->createTraceAndBreakPointsBox( this ) );

  this->pcount = 0;
}

bool interpreterW::event (QEvent * e)
{
#if QT_VERSION >= 0x040800
#ifdef __darwin__
  if (e->type() == QEvent::Paint)
  {
    if (this->pcount < 2) {
      this->repaint();
      this->pcount++;
    }
    else {
      this->pcount = 0;
    }
  }
#endif // __darwin__
#endif // QT_VERSION >= 0x040000
  return QWidget::event(e);
}

QWidget* interpreterW::createInterpreterLE( QWidget* parent )
{
  QTextEdit* te = new QTextEdit( parent );
#if QT_VERSION >= 0x040000
  te->setAcceptRichText(false);
  te->document()->setMaximumBlockCount(1000);
  te->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding ));
                        
#else
  te->setTextFormat(Qt::PlainText);
  this->maxlines = 1000;
  te->setSizePolicy( 
           QSizePolicy( QSizePolicy::Expanding, 
                        QSizePolicy::MinimumExpanding, 
                        te->sizePolicy().hasHeightForWidth() ) );
#endif // QT_VERSION >= 0x040000
  te->setMinimumSize( QSize( 0, 150 ) );
  te->setReadOnly(true);
  this->interpreterLE = te;
  return te;
}

QWidget* interpreterW::createInLE( QWidget* parent )
{
  BufferedQTextEdit* te = new BufferedQTextEdit( parent, "inLE" );
#if QT_VERSION >= 0x040000
  te->setAcceptRichText(false);
#else
  te->setTextFormat(Qt::PlainText);
#endif // QT_VERSION >= 0x040000
  te->setMaximumSize( QSize( 32767, 50 ) );
  te->insertPrompt();
  connect( te, SIGNAL(returnPressed2()), this, SLOT(returnPressed()) );
  this->inLE = te;
  return te;
}

QLayout* interpreterW::createToggleAndLogOperationBox( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout();
#ifdef __darwin__
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
#else
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
#endif
  layout->addWidget( this->createSaveLogButton( parent ) );
  layout->addWidget( this->createToggleB( parent ) );
  layout->addWidget( this->createClearLogButton( parent ) );
  return layout;
}

QWidget* interpreterW::createToggleB( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
//#ifdef __darwin__
//  button->setMaximumSize( QSize( 32767, 30 ) );
//#else
//  button->setMaximumSize( QSize( 32767, 16 ) );
//#endif
#if QT_VERSION >= 0x040000
  button->setCheckable( true );
  button->setText( ">" );
  button->setToolTip( tr("Toggle maximized / minimized interpreter window"));
  button->setWhatsThis( tr("Toggle maximized / minimized interpreter window"));
  connect( button, SIGNAL( toggled(bool) ), this, SLOT( toggle(bool) ) );
#else
  button->setToggleButton( true );
  button->setText( ">" );
  QToolTip::add(button, tr("Toggle maximized / minimized interpreter window"));
  QWhatsThis::add(button, tr("Toggle maximized / minimized interpreter window"));
  connect( button, SIGNAL( toggled(bool) ), this, SLOT( toggle(bool) ) );
#endif // QT_VERSION >= 0x040000

  this->toggleB = button;
  return button;
}

QWidget* interpreterW::createClearLogButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Clear Log" ) );
#if QT_VERSION >= 0x040000
  connect( button, SIGNAL(clicked(bool)), this, SLOT(clearLog_qt4(bool)) );
#else
  connect( button, SIGNAL(clicked()), this, SLOT(clearLog_qt3()) );
#endif // QT_VERSION >= 0x040000
  this->clearLogB = button;
  return button;
}

QWidget* interpreterW::createSaveLogButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Save Log" ) );
#if QT_VERSION >= 0x040000
  connect( button, SIGNAL(clicked(bool)), this, SLOT(saveLog_qt4(bool)) );
#else
  connect( button, SIGNAL(clicked()), this, SLOT(saveLog_qt3()) );
#endif // QT_VERSION >= 0x040000
  this->saveLogB = button;
  return button;
}

QLayout* interpreterW::createTraceAndBreakPointsBox( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout();
#ifdef __darwin__
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
#else
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
#endif
  layout->addWidget( this->createTraceLVBox( parent ) );
  layout->addWidget( this->createBreakPointsBox( parent ) );
  return layout;
}

QWidget* interpreterW::createTraceLVBox( QWidget* parent )
{
  QGroupBox *gbox = new QGroupBox( parent );
  this->traceBox = gbox;
  gbox->setTitle( tr( "Trace" ) );
#if QT_VERSION >= 0x040000
  QVBoxLayout *layout = new QVBoxLayout();
#ifdef __darwin__
  layout->setSpacing( 10 );
  layout->setMargin( 0 );
#else
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
#endif
  layout->addWidget( this->createTraceList( gbox ) );
  gbox->setLayout( layout );
#else
  gbox->setOrientation( Qt::Vertical );

  QLayout* layout = gbox->layout();
//  layout->setAlignment( Qt::AlignTop );
#ifdef __darwin__
  layout->setSpacing( 10 );
  layout->setMargin( 0 );
#else
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
#endif

  layout->add( this->createTraceList( gbox ) );
#endif // QT_VERSION >= 0x040000
  return gbox;
}

QWidget* interpreterW::createTraceList( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QStringList hlabels;
  hlabels.append(tr("Function/Operation"));
  hlabels.append(tr("Value"));

  QTreeWidget* lv = new QTreeWidget( parent );
  lv->setColumnCount (3);
  lv->setColumnHidden(2, true);
  lv->setHeaderLabels(hlabels);
#if QT_VERSION >= 0x050000
  lv->header()->setSectionsMovable(false);
#else
  lv->header()->setMovable(false);
#endif // QT_VERSION >= 0x040000
  lv->setRootIsDecorated(false);
  lv->setSelectionMode(QAbstractItemView::ExtendedSelection);
  lv->setColumnWidth(0, 150);
  lv->setColumnWidth(1, 150);

  connect(lv,   SIGNAL(itemClicked(QTreeWidgetItem *, int)), 
          this, SLOT(traceClicked_qt4(QTreeWidgetItem *, int)));
#else
  QListView* lv = new QListView( parent );
  int cn1 = lv->addColumn( tr( "Function/Operation" ) );
  lv->setColumnWidth(cn1, 200);
  int cn2 = lv->addColumn( tr( "Value" ) );
#ifdef __darwin__
  lv->setColumnWidth(cn2, 158);
#else
  lv->setColumnWidth(cn2, 160);
#endif
  lv->setSelectionMode( QListView::Extended );
  lv->setSorting(-1);
  connect( lv,   SIGNAL(clicked(QListViewItem *, const QPoint& , int)), 
           this, SLOT(traceClicked_qt3(QListViewItem *, const QPoint& , int) ) );
#endif // QT_VERSION >= 0x040000
  this->traceLV = lv;
  return lv;
}

QWidget* interpreterW::createBreakPointsBox( QWidget* parent )
{
  QGroupBox* gbox = new QGroupBox( parent );
  this->breakBox = gbox;
  gbox->setTitle( tr( "BreakPoints" ) );
#if QT_VERSION >= 0x040000
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setSpacing( 1 );
  layout->setMargin( 1 );
  layout->addLayout( this->createButtonPart( gbox ) );
  layout->addWidget( this->createBreakPointsList( gbox ) );
  gbox->setLayout( layout );
#else
  gbox->setOrientation( Qt::Vertical );
  QLayout* layout = gbox->layout();
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
  layout->addItem( this->createButtonPart( gbox ) );
  layout->add( this->createBreakPointsList( gbox ) );
#endif // QT_VERSION >= 0x040000
  return gbox;
}

QLayout* interpreterW::createButtonPart( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  layout->addWidget( this->createEnableButton( parent ) );
  layout->addWidget( this->createDisableButton( parent ) );
  layout->addWidget( this->createDeleteButton( parent ) );
  return layout;
}

QWidget* interpreterW::createEnableButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Enable" ) );
#if QT_VERSION >= 0x040000
  connect( button, SIGNAL(clicked(bool)), this, SLOT(enableBR_qt4(bool)) );
#else
  connect( button, SIGNAL(clicked()), this, SLOT(enableBR_qt3()) );
#endif // QT_VERSION >= 0x040000
  this->enableB = button;
  return button;
}

QWidget* interpreterW::createDisableButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Disable" ) );
#if QT_VERSION >= 0x040000
  connect(button, SIGNAL(clicked(bool)), this, SLOT(disableBR_qt4(bool)));
#else
  connect(button, SIGNAL(clicked()), this, SLOT(disableBR_qt3()));
#endif // QT_VERSION >= 0x040000
  this->disableB = button;
  return button;
}

QWidget* interpreterW::createDeleteButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr("Delete") );
#if QT_VERSION >= 0x040000
  connect(button, SIGNAL(clicked(bool)), this, SLOT(deleteBR_qt4(bool)));
#else
  connect(button, SIGNAL(clicked()), this, SLOT(deleteBR_qt3()));
#endif // QT_VERSION >= 0x040000
  this->deleteB = button;
  return button;
}

// createBreakPointsList
QWidget* interpreterW::createBreakPointsList( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QStringList labels;
  labels.append(tr("Enabled"));
  labels.append(tr("No."));
  labels.append(tr("Line"));

  QTreeWidget* lv = new QTreeWidget(parent);
  lv->setColumnCount (6);
  lv->setColumnHidden(3, true);
  lv->setColumnHidden(4, true);
  lv->setColumnHidden(5, true);
  lv->setHeaderLabels(labels);
#if QT_VERSION >= 0x050000
  lv->header()->setSectionsMovable(false);
#else
  lv->header()->setMovable(false);
#endif // QT_VERSION >= 0x040000
  lv->setRootIsDecorated(false);
  lv->setSelectionMode(QAbstractItemView::ExtendedSelection);
  lv->setColumnWidth(0, 150);
  lv->setColumnWidth(1, 50);
  lv->setColumnWidth(2, 50);

  connect(lv,   SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), 
          this, SLOT(breakDoubleClicked_qt4(QTreeWidgetItem *, int)));
  connect(lv,   SIGNAL(itemClicked(QTreeWidgetItem *, int)), 
          this, SLOT(breakClicked_qt4(QTreeWidgetItem *, int)));
  this->breakLV = lv;
  return lv;
#else
  QListView* lv = new QListView( parent );
  int cn1 = lv->addColumn( tr("Enabled") );
  lv->setColumnWidth(cn1, 150);
  int cn2 = lv->addColumn( tr("No.") );
  lv->setColumnWidth(cn2, 50);
  int cn3 = lv->addColumn( tr("Line") );
  lv->setColumnWidth(cn3, 50);
  lv->setSelectionMode( QListView::Extended );
  lv->setSorting(1);
  connect(lv,   SIGNAL(doubleClicked(QListViewItem *)), 
          this, SLOT(breakDoubleClicked_qt3(QListViewItem *)));
  connect(lv,   SIGNAL(clicked(QListViewItem *, const QPoint &, int)), 
          this, SLOT(breakClicked_qt3(QListViewItem *, const QPoint &, int)));
  this->breakLV = lv;
  return lv;
#endif // QT_VERSION >= 0x040000
}

/*
 *  Destroys the object and frees any allocated resources
 */
interpreterW::~interpreterW()
{
    // no need to delete child widgets, Qt does it all for us
}

void interpreterW::addBreakPos(const QString& filename, int line, int col, int num)
{
  QString shortName (QFileInfo(filename).fileName());

  QString lineStr, numStr, colStr;
  lineStr.sprintf("%d", line);
  colStr.sprintf("%d", col);
  numStr.sprintf("%d", num);

#if QT_VERSION >= 0x040000
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  item->setTextAlignment(0, Qt::AlignLeft);
  item->setTextAlignment(1, Qt::AlignRight);
  item->setTextAlignment(2, Qt::AlignRight);
  item->setCheckState(0, Qt::Checked);
  item->setText(0, shortName);
  item->setText(1, numStr);
  item->setText(2, lineStr);
  item->setText(3, colStr);
  item->setText(4, filename);
  item->setText(5, "ON"); 

  this->breakLV->addTopLevelItem(item);
  this->breakLV->resizeColumnToContents(0);
#else
  QCheckListItem * item = new QCheckListItem(this->breakLV, shortName, QCheckListItem::CheckBox);
  item->setText(1, numStr);
  item->setText(2, lineStr);
  item->setText(3, colStr);
  item->setText(5, filename); 
  item->setText(6, "ON"); 
  item->setOn(true);
#endif // QT_VERSION >= 0x040000
}

void interpreterW::addBreakName(const QString& name, int num)
{
#if QT_VERSION >= 0x040000
  QStringList list (name.split( '`' ));
#else
  QStringList list (QStringList::split( '`', name ));
#endif // QT_VERSION >= 0x040000

  int line = 1;
  int col = 1;
  int length = 1;
  Qt2TB::GetFnOpPosInfoI(name, &line, &col, &length);

  QString filename (name);
  QStringList mlist (Qt2TB::getFilesOfModule(list.front()));
  if (!mlist.isEmpty()) {
    //filename = mlist.front();
    filename = (QFileInfo(mlist.front()).fileName());
  }

  QString methodName (name);
#ifdef VDMSL
  if ((list.count() == 2) && (list.front() == "DefaultMod")) {
    methodName = list.back();
  }
#endif // VDMSL
  
  QString lineStr, numStr, colStr;
  numStr.sprintf("%d", num);
  lineStr.sprintf("%d", line);
  colStr.sprintf("%d", col);

#if QT_VERSION >= 0x040000
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  item->setTextAlignment(0, Qt::AlignLeft);
  item->setTextAlignment(1, Qt::AlignRight);
  item->setTextAlignment(2, Qt::AlignRight);
  item->setCheckState(0, Qt::Checked);
  item->setText(0, methodName);
  item->setText(1, numStr);
  item->setText(2, lineStr);
  item->setText(3, colStr);
  item->setText(4, filename);
  item->setText(5, "ON"); 

  this->breakLV->addTopLevelItem(item);
  this->breakLV->resizeColumnToContents(0);
#else
  QCheckListItem * item = new QCheckListItem(this->breakLV, methodName, QCheckListItem::CheckBox);
  item->setText(1, numStr);
  item->setText(2, lineStr);
  item->setText(3, colStr);
  item->setText(5, filename); 
  item->setText(6, "ON"); 
  item->setOn(true);
#endif // QT_VERSION >= 0x040000
}

QString interpreterW::removePrompt(const QString& text)
{
  QString command(text);
  while( command.startsWith( ">" ) ) command.remove(0,1);
#if QT_VERSION >= 0x040000
  return command.simplified();
#else
  return command.stripWhiteSpace();
#endif // QT_VERSION >= 0x040000
}

QTREEWIDGETITEM* interpreterW::findBreakNum(int num)
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem* lvi = (QTreeWidgetItem*)NULL; 
  bool found = false;
  int count = this->breakLV->topLevelItemCount();
  for (int index = 0; (index < count) && !found; index++) {
    QTreeWidgetItem * item = this->breakLV->topLevelItem(index);
    if (item->text(1).toInt() == num) {
      found = true;
      lvi = item;
    }
  }
  return lvi;
#else
  QListViewItem* lvi = (QListViewItem*)NULL;
  QListViewItemIterator it (this->breakLV);
  bool found = false;
  for (; it.current() && !found; ++it) {
    if (atoi(it.current()->text(1)) == num) {
      found = true;
      lvi = it.current();
    }
  }
  return lvi;
#endif // QT_VERSION >= 0x040000
}

void interpreterW::removeBreak(int num)
{
  QString fmt (tr( "Deleting breakpoint %1\n" ));
  this->write(fmt.arg(num));

  QTREEWIDGETITEM* lvi = findBreakNum(num);
  if (NULL != lvi) {
#if QT_VERSION >= 0x040000
    int index = this->breakLV->indexOfTopLevelItem(lvi);
    this->breakLV->takeTopLevelItem(index);
#else
    this->breakLV->takeItem(lvi);
#endif // QT_VERSION >= 0x040000
  }
}

void interpreterW::enableBreak(int num)
{
  QString fmt (tr( "Enabling breakpoint %1\n" ));
  this->write( fmt.arg(num) );

  QTREEWIDGETITEM * lvi = findBreakNum(num);
  if (NULL != lvi) {
#if QT_VERSION >= 0x040000
    if (lvi->checkState(0) != Qt::Checked) {
      lvi->setCheckState(0, Qt::Checked);
      lvi->setText(5, "ON"); 
    }
#else
    QCheckListItem * qcli = (QCheckListItem *)lvi;
    if (!qcli->isOn()) {
      qcli->setOn(true);
      qcli->setText(6, "ON");
    }
#endif // QT_VERSION >= 0x040000
  }
}

void interpreterW::disableBreak(int num)
{
  QString fmt (tr( "Disabling breakpoint %1\n" ));
  this->write( fmt.arg(num) );

  QTREEWIDGETITEM* lvi = findBreakNum(num);
  if (NULL != lvi) {
#if QT_VERSION >= 0x040000
    if (lvi->checkState(0) != Qt::Unchecked) {
      lvi->setCheckState(0, Qt::Unchecked);
      lvi->setText(5, "OFF"); 
    }
#else
    QCheckListItem * qcli = (QCheckListItem *)lvi;
    if (qcli->isOn()) {
      qcli->setOn(false);
      qcli->setText(6, "OFF");
    }
#endif // QT_VERSION >= 0x040000
  }
}

void interpreterW::gotoBtLevel(int num)
{
#if QT_VERSION >= 0x040000
  int count = this->traceLV->topLevelItemCount();
  for (int index = 0; index < count; index++) {
    QTreeWidgetItem * item = this->traceLV->topLevelItem(index);
    if (item->isSelected()) {
      item->setSelected(false);
    }
    if (item->text(2).toInt() == num) {
      item->setSelected(true);
    }
  }
#else
  QListViewItemIterator iter (this->traceLV);
  int index = 1;
  for (; iter.current(); ++iter) {
    if (iter.current()->isSelected()) {
      iter.current()->setSelected(false);
    }
    if (index == num) {
      iter.current()->setSelected(true);
    }
    index++;
  }
#endif // QT_VERSION >= 0x040000
//  this->traceLV->repaint();
  this->traceLV->update();
}

void interpreterW::clearBuffer()
{
  this->inLE->clearBuffer();
}

void interpreterW::clearDebugWindow()
{
  this->interpreterLE->clear();
  this->traceLV->clear();
}

void interpreterW::resetWindow()
{
  this->clearDebugWindow();
  this->breakLV->clear();
}

void interpreterW::saveLog_qt4(bool)
{
  emit saveInterpreterLog();
}

#if QT_VERSION < 0x040000
void interpreterW::saveLog_qt3()
{
  emit saveInterpreterLog();
}
#endif // QT_VERSION < 0x040000

void interpreterW::clearLog_qt4(bool)
{
  this->interpreterLE->clear();
}

#if QT_VERSION < 0x040000
void interpreterW::clearLog_qt3()
{
  this->interpreterLE->clear();
}
#endif // QT_VERSION < 0x040000

void interpreterW::enableBR_qt4(bool)
{
#if QT_VERSION >= 0x040000
  int count = this->breakLV->topLevelItemCount();
  for (int index = 0; index < count; index++) {
    QTreeWidgetItem * item = this->breakLV->topLevelItem(index);
    if (item->isSelected()) {
      if (item->checkState(0) != Qt::Checked) {
        item->setCheckState(0, Qt::Checked);
        item->setText(5, "ON");
        int num = item->text(1).toInt();
        emit enableBr(num);
      }
    }
  }
#endif // QT_VERSION >= 0x040000
}
#if QT_VERSION < 0x040000
void interpreterW::enableBR_qt3()
{
  QListViewItemIterator it (this->breakLV);
  for (; it.current();++it) {
    if (it.current()->isSelected()) {
      QCheckListItem * qcli = (QCheckListItem *)it.current();
      if (!qcli->isOn()) {
        qcli->setOn(true);
        qcli->setText(6, "ON");
        int num = atoi(qcli->text(1));
        emit enableBr(num);
      }
    }
  }
}
#endif // QT_VERSION < 0x040000

void interpreterW::disableBR_qt4(bool)
{
#if QT_VERSION >= 0x040000
  int count = this->breakLV->topLevelItemCount();
  for (int index = 0; index < count; index++) {
    QTreeWidgetItem * item = this->breakLV->topLevelItem(index);
    if (item->isSelected()) {
      if (item->checkState(0) != Qt::Unchecked) {
        item->setCheckState(0, Qt::Unchecked);
        item->setText(5, "OFF");
        int num = item->text(1).toInt();
        emit disableBr(num);
      }
    }
  }
#endif // QT_VERSION >= 0x040000
}
#if QT_VERSION < 0x040000
void interpreterW::disableBR_qt3()
{
  QListViewItemIterator it (this->breakLV);
  for (; it.current(); ++it) {
    if (it.current()->isSelected()) {
      QCheckListItem * qcli = (QCheckListItem *)it.current();
      if (qcli->isOn()) {
        qcli->setOn(false);
        qcli->setText(6, "OFF");
        int num = atoi(qcli->text(1));
        emit disableBr(num);
      }
    }
  }
}
#endif // QT_VERSION < 0x040000

void interpreterW::deleteBR_qt4(bool)
{
#if QT_VERSION >= 0x040000
    bool cont = false;
    do {
      bool exists = false;
      int count = this->breakLV->topLevelItemCount();
      for (int index = 0; (index < count) && !exists; index++) {
        QTreeWidgetItem * item = this->breakLV->topLevelItem(index);
        if (item->isSelected()) {
          int num = item->text(1).toInt();
          this->breakLV->takeTopLevelItem(index);
          emit deleteBr(num);
          exists = true;
        }
      }
      cont = exists;
    } while (cont);
#endif // QT_VERSION >= 0x040000
}
#if QT_VERSION < 0x040000
void interpreterW::deleteBR_qt3()
{
  bool cont = false;
  do {
    QListViewItemIterator it (this->breakLV);
    bool exists = false;
    while (it.current() && !exists) {
      if (it.current()->isSelected()) {
        int num = atoi(it.current()->text(1));
        this->breakLV->takeItem(it.current());
        emit deleteBr(num);
        exists = true;
      }
      else
        ++it;
    }
    cont = exists;
  } while (cont);
}
#endif // QT_VERSION < 0x040000

//toggle mode view
void interpreterW::toggle(bool)
{
#if QT_VERSION >= 0x040000
  if (this->toggleB->isChecked())
#else
  if (this->toggleB->isOn())
#endif // QT_VERSION >= 0x040000
  {
    this->traceBox->hide();
    this->breakBox->hide();
    this->toggleB->setText( tr( "<" ) );
  }
  else {
    this->traceBox->show();
    this->breakBox->show();
    this->toggleB->setText( tr( ">" ) );
  }
}

void interpreterW::returnPressed()
{
#if QT_VERSION >= 0x040000
  QString text (this->inLE->toPlainText());
  this->write(text + "\n"); //
#else
  QString text (this->inLE->text());
  this->write(text); //
#endif // QT_VERSION >= 0x040000

  text.replace(QRegExp("\n"), "");
  this->inLE->setText(text);
  QString command (removePrompt(text));
  this->inLE->addString(command);
  this->inLE->clear();
  this->inLE->insertPrompt();
  emit executeCommand(command);
}

void interpreterW::breakDoubleClicked_qt4(QTreeWidgetItem * item, int column)
{
#if QT_VERSION >= 0x040000
  if (item) {
    if( item->text(2) == "-" ) {
      emit resetFilePosition(item->text(4), 1, 1);
    }
    else {
      emit resetFilePosition(item->text(4), item->text(2).toInt(), item->text(3).toInt());
    }
  }
#endif // QT_VERSION >= 0x040000
}

#if QT_VERSION < 0x040000
//void interpreterW::breakDoubleClicked_qt3(QListViewItem* item, const QPoint &, int)
void interpreterW::breakDoubleClicked_qt3(QListViewItem* item)
{
  if (NULL != item)
  {
    //call show on source window with line
    //needs intermediary function on mainF
    //QMessageBox::about(this,
    //                   "open in source window",
    //                   "file: "+item->text(5)+ " on line: "+item->text(2));
//    emit resetFilePosition(item->text(5), item->text(2).toInt());
    if( item->text(2) == "-" )
    {
      emit resetFilePosition(item->text(5), 1, 1);
    }
    else
    {
      emit resetFilePosition(item->text(5), item->text(2).toInt(), item->text(3).toInt());
    }
  }
}
#endif // QT_VERSION < 0x040000

void interpreterW::breakClicked_qt4(QTreeWidgetItem * item, int column)
{
#if QT_VERSION >= 0x040000
  if (item && (column == 0))
  {
    if (item->checkState(0) == Qt::Checked)
    {
      int num = item->text(1).toInt();
      if (item->text(5) == "OFF")
      {
        item->setText(5, "ON");
        emit enableBr(num);
      }
    }
    else if (item->checkState(0) == Qt::Unchecked)
    {
      int num = item->text(1).toInt();
      if (item->text(5) == "ON")
      {
        item->setText(5, "OFF");
        emit disableBr(num);
      }
    }
  }
#endif // QT_VERSION >= 0x040000
}
#if QT_VERSION < 0x040000
void interpreterW::breakClicked_qt3(QListViewItem* item, const QPoint &, int col)
{
  if (NULL != item)
  {
    QCheckListItem * qcli = (QCheckListItem *)item;
    if (qcli->isOn() && (qcli->text(6) == "OFF"))
    {
      int num = qcli->text(1).toInt();
      qcli->setText(6, "ON");
      emit enableBr(num);
    }
    else if (!qcli->isOn() && (qcli->text(6) == "ON"))
    {
      int num = qcli->text(1).toInt();
      qcli->setText(6, "OFF");
      emit disableBr(num);
    }
  }
}
#endif // QT_VERSION < 0x040000

#if QT_VERSION >= 0x040000
void interpreterW::traceClicked_qt4(QTreeWidgetItem * item, int column)
#else
void interpreterW::traceClicked_qt4(QTreeWidgetItem * item, int column)
{
}

void interpreterW::traceClicked_qt3(QListViewItem * item, const QPoint & pnt, int column )
#endif// QT_VERSION >= 0x040000
{
  if (item && (column == 1))
  {
    if (item->text(1) == "[...]")
    {
#if QT_VERSION >= 0x040000
      int index = item->text(2).toInt();
#else
      int index = atoi(item->text(2));
#endif // QT_VERSION >= 0x040000
      QString arguments (Qt2TB::getTraceArgsI(index));
      //we should show the trace
#if QT_VERSION >= 0x040000
      item->setText(1, arguments);
#else
      if (arguments.contains('\n'))
      {
        QListViewItem* lastItem = item;
        int startIndex = 0;
        for(int endIndex = arguments.find('\n');
            endIndex != -1;
            endIndex = arguments.find('\n', startIndex))
        {
          QString argumentLine = arguments.mid(startIndex, endIndex -startIndex);
          if (startIndex != 0) 
          {
            QListViewItem *lvi = new QListViewItem(this->traceLV, lastItem, "");
            lastItem = lvi;
          }
          lastItem->setText(1, argumentLine);
          startIndex = endIndex + 1;
        }
        QString argumentLine = arguments.mid(startIndex);
        QListViewItem *lvi = new QListViewItem(this->traceLV, lastItem, "");
        lvi->setText(1, argumentLine);
      }
      else
      {
        item->setText(1, arguments + "");
      }
#endif // QT_VERSION >= 0x040000
    }
    else
    {
#if QT_VERSION >= 0x040000
      QList<QTreeWidgetItem *> list (item->takeChildren());
      item->setText(1, "[...]");
#else
      QListViewItem *firstItem = item;
      while ((firstItem != 0) && firstItem->text(0) == "")
             firstItem = firstItem->itemAbove();
      if (firstItem != 0)
      {
        firstItem->setText(1, "[...]");
        QListViewItem* nextItem = firstItem->nextSibling();
        while (nextItem && nextItem->text(0) == "")
        {
          QListViewItem* thisItem = nextItem;
          nextItem = nextItem->nextSibling();
          delete thisItem;
        }
      }
      else
        emit logWrite("Internal Error: interpreterW::traceClicked");
#endif // QT_VERSION >= 0x040000
    }
// 20130425 -->
#if QT_VERSION >= 0x040000
    this->traceLV->resizeColumnToContents(1);
#endif // QT_VERSION >= 0x040000
// <-- 20130425
  }
}

#if QT_VERSION >= 0x040000
void interpreterW::setBackTrace(const QList<BackTraceEntry> & bts)
#else
void interpreterW::setBackTrace(const QValueList<BackTraceEntry> & bts)
#endif // QT_VERSION >= 0x040000
{
#if QT_VERSION >= 0x040000
  this->traceLV->clear();
  int index = 1;
  for (QList<BackTraceEntry>::const_iterator iter = bts.begin(); iter != bts.end(); ++iter)
  {
    QTreeWidgetItem * item = new QTreeWidgetItem(this->traceLV);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setTextAlignment(0, Qt::AlignLeft);
    item->setTextAlignment(1, Qt::AlignLeft);
    item->setText(0, (*iter).name);
    item->setText(1, "[...]");

    QString indexStr;
    indexStr.sprintf("%d", index);
    item->setText(2, indexStr);
    index++;
  }
  this->traceLV->resizeColumnToContents(0);
#else
  this->traceLV->clear();
  int index = 1;
  QListViewItem* lastItem = (QListViewItem*) NULL;

  for (QValueList<BackTraceEntry>::const_iterator iter = bts.begin(); iter != bts.end(); ++iter)
  {
    QListViewItem *lvi;
    if (lastItem != NULL)
      lvi = new QListViewItem(this->traceLV, lastItem);
    else
      lvi = new QListViewItem(this->traceLV);

    lastItem = lvi;
    lvi->setText(0, (*iter).name);
    lvi->setText(1, "[...]");
    QString indexStr;
    indexStr.sprintf("%d", index);
    lvi->setText(2, indexStr);
    lvi->setSelectable(true);

    index++;
  }
#endif // QT_VERSION >= 0x040000
}

void interpreterW::write(const QString& msg)
{
#if QT_VERSION >= 0x040000
  QTextCursor tc (this->interpreterLE->textCursor());
  tc.movePosition(QTextCursor::End);
  tc.insertText( msg );
  this->interpreterLE->setTextCursor(tc);
  this->interpreterLE->update();
#else
  if (this->maxlines > 0)
  {
    this->interpreterLE->setUpdatesEnabled(FALSE);
    while (this->maxlines < this->interpreterLE->paragraphs())
    {
      this->interpreterLE->removeParagraph(0);
    }
    this->interpreterLE->moveCursor(QTextEdit::MoveEnd, false);
    this->interpreterLE->setUpdatesEnabled(TRUE);
  }
  QString text(msg);
  this->interpreterLE->append( text );
  this->interpreterLE->repaint();
#endif // QT_VERSION >= 0x040000
}

//
// interpreterW::checkBreakpointsFile()
//
// This function is called when the toolbox is reawoken. It checks
// whether a files has been made by Word, containing a breakpoint. If
// this file is found, it is parsed, the breakpoint(s) is(are) set and
// the file is then deleted.
void interpreterW::checkBreakpointsFile()
{
  // Name of file depends on local environment, so use auxiliary function for this
  QString breakPointFile = Qt2TB::getTmpBreakFileNameI();

  // Check if file exists
  QFile file( breakPointFile );
#if QT_VERSION >= 0x040000
  if ( !file.open( QIODevice::ReadOnly ) ) 
#else
  if ( !file.open( IO_ReadOnly ) ) 
#endif // QT_VERSION >= 0x040000
    return;

  // Read file
  QTextStream ts(&file);
  while (!ts.atEnd())
  {
    QString line = ts.readLine();
    QString filename;
    int lineNum = -1, colNum = -1;
    if (!line.isEmpty()){
      // FIXME: Note that this code will be easier when support for
      // regular expressions in Qt is improved from that available in
      // 2.3.0 
      // QRegExpr re("(.*)-\\+-\\+-(.*)-\\+-\\+-(.*)$");
      int index = 0;
      bool parseOk = true;

      // Get name of file containing breakpoint
#if QT_VERSION >= 0x040000
      int termIndex = line.indexOf("-+-+-");
#else
      int termIndex = line.find("-+-+-");
#endif // QT_VERSION >= 0x040000
      if (termIndex != -1)
        filename = line.left(termIndex);
      else 
        parseOk = false;

      // Get line number of breakpoint
      index = termIndex + 5;
#if QT_VERSION >= 0x040000
      termIndex = line.indexOf("-+-+-", index);
#else
      termIndex = line.find("-+-+-", index);
#endif // QT_VERSION >= 0x040000
      bool ok;
      if (termIndex != -1){
        lineNum = line.mid(index, termIndex - index).toInt(&ok);
        parseOk = parseOk && ok;
      }
      else 
       parseOk = false;

      // Get column number of breakpoint
      colNum = line.mid(termIndex + 5).toInt(&ok);
      parseOk = parseOk && ok;
      if (parseOk)
      {
        filename.replace(QRegExp("\\\\"), "/");
        QString fnm (QFileInfo(filename).fileName());
        QString cmd (QString( "break %1 %2 %3" ).arg( fnm ).arg( lineNum ).arg( colNum ));
        emit logWrite( cmd );
        emit executeCommand( cmd );
      }
      else
        emit logWrite("Unable to parse breakpoint file");
    }
  }
  file.close();

  // It is important that the file is deleted, since otherwise it will
  // be reread every time the toolbox is reawoken.
  if(!file.remove())
    emit logWrite("Unable to remove breakpoint file " + breakPointFile);
}

void interpreterW::removeBreakOfFile( QString & filename )
{
  QString fnm (QFileInfo(filename).fileName());
#if QT_VERSION >= 0x040000
  bool cont = false;
  do
  {
    bool exists = false;
    int count = this->breakLV->topLevelItemCount();
    for (int index = 0; (index < count ) && !exists; index++)
    {
      QTreeWidgetItem * item = this->breakLV->topLevelItem(index);
      if (item->text(4) == fnm)
      {
        int num = item->text(1).toInt();
        this->breakLV->takeTopLevelItem(index);
        emit deleteBr(num);
        exists = true;
      }
    }
    cont = exists;
  } while(cont);
#else
  bool cont = false;
  do
  {
    QListViewItemIterator it (this->breakLV);
    bool exists = false;
    while (it.current() && !exists)
    {
      if( fnm == it.current()->text( 5 ) )
      {
        int num = atoi(it.current()->text(1));
        this->breakLV->takeItem(it.current());
        emit deleteBr(num);
        exists = true;
      }
      else
        ++it;
    }
    cont = exists;
  } while (cont);
#endif // QT_VERSION >= 0x040000
}

void interpreterW::setBusy(bool busy)
{
  this->inLE->setReadOnly(busy);
}

void interpreterW::setMaxLogLines(int num)
{
#if QT_VERSION >= 0x040000
  this->interpreterLE->document()->setMaximumBlockCount(num);
#else
  this->maxlines = num;
#endif // QT_VERSION >= 0x040000
  this->interpreterLE->repaint();
}

void interpreterW::addString(const QString & cmd)
{
  this->inLE->addString(cmd);
}

QString interpreterW::getLog()
{
  return this->interpreterLE->toPlainText();
}
