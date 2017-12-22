/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/pogF.cc,v $
*  * VERSION
*  *    $Revision: 1.16 $
*  * DATE
*  *    $Date: 2006/06/22 04:13:08 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "interface2TB.h"
#include "mainF.h"

#define numof(x) (sizeof(x)/sizeof(x[0]))

#define CHECKED_COLUMN   0
#define MODULE_COLUMN    1
#define MEMBER_COLUMN    2
#define LOCATION_COLUMN  3
#define NUMBER_COLUMN    4
#define TYPE_COLUMN      5
#define FILE_COLUMN      6
#define LINE_COLUMN      7
#define COL_COLUMN       8
#define DESC_COLUMN      9

struct ColumnInfo pogW::columns[] = {
  {"Checked", true, &Qt2TB::getChckMarks, &Qt2TB::setChckMarks},
#ifdef VDMSL
  {"Module", true, &Qt2TB::getModnms, &Qt2TB::setModnms},
#endif // VDMSL
#ifdef VDMPP
  {"Class", true, &Qt2TB::getModnms, &Qt2TB::setModnms},
#endif // VDMPP
  {"Member", true, &Qt2TB::getMemnms, &Qt2TB::setMemnms},
  {"Location", true, &Qt2TB::getLocs, &Qt2TB::setLocs},
  {"Index", false},
  {"Type", true, &Qt2TB::getKinds, &Qt2TB::setKinds}
};

pogW::pogW( mainW * mainw, QWidget* parent, const char* name, WFlags fl )
#if QT_VERSION >= 0x040000
    : QSplitter( parent )
#else
    : QSplitter( parent, name )
#endif // QT_VERSION >= 0x040000
{
#if QT_VERSION >= 0x040000
  this->setOrientation( Qt::Vertical );
#else  
  this->setOrientation( QSplitter::Vertical );
#endif // QT_VERSION >= 0x040000

  pogList = new QWidget(this);

  this->setWindowName( (name == NULL) ? "pogW" : name );

#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( "Integrity properties" ) );
#else
  if ( !name ) setName( "pogW" );
  this->setCaption( tr( "Integrity properties" ) );
#endif // QT_VERSION >= 0x040000
    
    QHBoxLayout * Layout2 = new QHBoxLayout(pogList); 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    QVBoxLayout * Layout1 = new QVBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

#if QT_VERSION >= 0x040000
    const QIcon& prevIcon = mainw->getImage("up.png", false);
    const QIcon& nextIcon = mainw->getImage("down.png", false);
    const QIcon& markIcon = mainw->getImage("checkmark.png", false);
    const QIcon& filterIcon = mainw->getImage("filter.png", false);
    const QIcon& selectIcon = mainw->getImage("right.png", false);
    const QIcon& deselectIcon = mainw->getImage("left.png", false);
#else
    const QIconSet& prevIcon = mainw->getImage("up.png", false);
    const QIconSet& nextIcon = mainw->getImage("down.png", false);
    const QIconSet& markIcon = mainw->getImage("checkmark.png", false);
    const QIconSet& filterIcon = mainw->getImage("filter.png", false);
    const QIconSet& selectIcon = mainw->getImage("right.png", false);
    const QIconSet& deselectIcon = mainw->getImage("left.png", false);
#endif // QT_VERSION >= 0x040000

    prevB = new QToolButton( pogList );
#if QT_VERSION >= 0x040000
    prevB->setIcon(prevIcon);
    prevB->setToolTip( tr( "Click to previous integrity property" ) );
#else
    prevB->setIconSet(prevIcon);
    QToolTip::add( prevB, tr( "Click to previous integrity property" ) );
#endif // QT_VERSION >= 0x040000
    Layout1->addWidget( prevB );

    nextB = new QToolButton( pogList );
#if QT_VERSION >= 0x040000
    nextB->setIcon(nextIcon);
    nextB->setToolTip( tr( "Click to next integrity property" ) );
#else
    nextB->setIconSet(nextIcon);
    QToolTip::add( nextB, tr( "Click to next integrity property" ) );
#endif // QT_VERSION >= 0x040000
    Layout1->addWidget( nextB );

    checkB = new QToolButton( pogList );
#if QT_VERSION >= 0x040000
    checkB->setIcon(markIcon);
    checkB->setToolTip( tr( "Click to check/uncheck the selected integrity property" ) );
#else
    checkB->setIconSet(markIcon);
    QToolTip::add( checkB, tr( "Click to check/uncheck the selected integrity property" ) );
#endif // QT_VERSION >= 0x040000
    Layout1->addWidget( checkB );
    
    filterB = new QToolButton( pogList );
#if QT_VERSION >= 0x040000
    filterB->setIcon(filterIcon);
    filterB->setToolTip( tr( "Click to filter the integrity properties" ) );
#else
    filterB->setIconSet(filterIcon);
    QToolTip::add( filterB, tr( "Click to filter the integrity properties" ) );
#endif // QT_VERSION >= 0x040000
    Layout1->addWidget( filterB );

    Layout2->addLayout( Layout1 );

#if QT_VERSION >= 0x040000
    QStringList hlabels;
    for(int i = 0; i < (int)numof(columns); i++) {
      hlabels.append(tr( columns[i].name ));
    }

    QTreeWidget * lv = new QTreeWidget( pogList );

    int fontSize = 12;
    QFont font = QFont("monospace", fontSize);
    font.setStyleHint(QFont::TypeWriter);
    int fontPxSize = QFontMetrics(font).width('0');
    lv->setFont(font);

    lv->setColumnCount (numof(columns));
    lv->setColumnWidth(0, 40);
    lv->setColumnWidth(4, 40);
    lv->setHeaderLabels(hlabels);

    QHeaderView * header = lv->header();
#if QT_VERSION >= 0x050000
    header->setSectionsMovable(false);
    header->setStretchLastSection(true);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::Fixed);
    header->setSectionResizeMode(5, QHeaderView::Interactive);
#else
    header->setMovable(false);
    header->setStretchLastSection(true);
    header->setResizeMode(0, QHeaderView::Fixed);
    header->setResizeMode(1, QHeaderView::Interactive);
    header->setResizeMode(2, QHeaderView::Interactive);
    header->setResizeMode(3, QHeaderView::ResizeToContents);
    header->setResizeMode(4, QHeaderView::Fixed);
    header->setResizeMode(5, QHeaderView::Interactive);
#endif // QT_VERSION >= 0x050000

    lv->setRootIsDecorated(false);
    lv->setSelectionMode(QAbstractItemView::SingleSelection);

    this->pogMessageList = lv;
#else
    this->pogMessageList = new QListView( pogList );
    this->pogMessageList->setSorting(1);
#endif // QT_VERSION >= 0x040000

    this->pogFilters = new QTabWidget( this );

    for(int i = 0; i < (int)numof(columns); i++)
    {
#if QT_VERSION < 0x040000
      this->pogMessageList->addColumn( tr( columns[i].name ) );
#endif // QT_VERSION < 0x040000
      if(columns[i].filter) {
#if QT_VERSION >= 0x040000
        this->pogFilters->addTab(
          new pogFilterW( selectIcon, deselectIcon, this, columns[i].name),
          tr( columns[i].name )
        );
#else
        this->pogFilters->insertTab(
          new pogFilterW( selectIcon, deselectIcon, this, columns[i].name),
          tr( columns[i].name )
        );
#endif // QT_VERSION >= 0x040000
      }
    }
    this->updateFilters(); // show "Yes", "No"

    this->pogMessageList->setAllColumnsShowFocus(true);

    Layout2->addWidget( this->pogMessageList );
    
    this->pogDescription = new QTextEdit( this );
#if QT_VERSION >= 0x040000
    int fontSize2 = 12;
    QFont font2 = QFont("monospace", fontSize2);
    font2.setStyleHint(QFont::TypeWriter);
    int fontPxSize2 = QFontMetrics(font2).width('0');
    this->pogDescription->setFont(font2);

    this->pogDescription->setAcceptRichText(false);
    this->pogDescription->setSizePolicy( 
                        QSizePolicy( QSizePolicy::Expanding,
                                     QSizePolicy::Minimum ));
#else
    this->pogDescription->setTextFormat( Qt::PlainText );
    this->pogDescription->setSizePolicy( 
                        QSizePolicy( QSizePolicy::Expanding, 
                                     QSizePolicy::Minimum,
                                     this->pogDescription->sizePolicy().hasHeightForWidth() ) );
#endif // QT_VERSION >= 0x040000

#if QT_VERSION >= 0x040000
    QObject::connect(this->pogMessageList, SIGNAL(itemSelectionChanged()),
                     this,           SLOT(pogSelectionChanged_qt4()));

    QObject::connect(this->pogMessageList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                     this,           SLOT(pogDoubleClicked_qt4(QTreeWidgetItem*, int)));
#else
    QObject::connect(this->pogMessageList, SIGNAL(selectionChanged(QListViewItem*)),
                     this,           SLOT(pogSelectionChanged_qt3(QListViewItem*)));

    QObject::connect(this->pogMessageList, SIGNAL(doubleClicked(QListViewItem*)),
                     this,           SLOT(pogDoubleClicked_qt3(QListViewItem*)));
#endif // QT_VERSION >= 0x040000

    QObject::connect(nextB, SIGNAL(pressed()), this, SLOT(nextMessage()));
    QObject::connect(prevB, SIGNAL(pressed()), this, SLOT(previousMessage()));
    QObject::connect(filterB, SIGNAL(pressed()), this, SLOT(applyFilters()));
    QObject::connect(checkB, SIGNAL(pressed()), this, SLOT(markAsChecked()));

#if QT_VERSION >= 0x040000
    QObject::connect(this->pogFilters, SIGNAL(currentChanged(int)),
                     this,             SLOT(updateFilters()));
#else
    QObject::connect(this->pogFilters, SIGNAL(currentChanged(QWidget*)),
                     this,             SLOT(updateFilters()));
#endif // QT_VERSION >= 0x040000

#if QT_VERSION >= 0x040000
    this->setStretchFactor(this->indexOf(pogList), 1);
#else
    this->setResizeMode(pogList, QSplitter::Stretch);
#endif // QT_VERSION >= 0x040000
    this->pcount = 0;
}

bool pogW::event (QEvent * e)
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

int pogW::getColumnIdx(int column)
{
  int res = -1;
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = this->pogMessageList->headerItem();
  int count = item->columnCount();
  bool exists = false;
  for (int index = 0; (index < count ) && !exists; index++)
  {
    if (item->text(index) == QString(tr(columns[column].name)))
    {
      res = index;
      exists = true;
    }
  }
#else
  for(int i = 0; i < this->pogMessageList->columns(); i++) {
    if(this->pogMessageList->columnText(i) == QString(tr(columns[column].name))) {
      res = i;
      break;
    }
  }
#endif // QT_VERSION >= 0x040000
  return res;
}

int pogW::getColumnKeyByName(const QString & name)
{
  int res = -1;
  for(int i = 0; i < (int)numof(pogW::columns); i++) {
    if(QString(pogW::columns[i].name) == name) {
      res = i;
      break;
    }
  }
  return res;
}

void pogW::addPogItem(const QString & checkMark,
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
                      const QString & pogDesc)
{
  new POGQListViewItem(this,
                       checkMark,
                       modName,
                       memberName,
                       locationType,
                       pogType,
                       pogNum,
                       filename,
                       tmpFilename,
                       line,
                       col,
                       length,
                       pogDesc);
#if QT_VERSION >= 0x040000
  this->pogMessageList->sortItems(1, Qt::AscendingOrder);
  this->pogMessageList->resizeColumnToContents(1);
  this->pogMessageList->resizeColumnToContents(2);
#endif // QT_VERSION >= 0x040000
}

void pogW::cleanUp(const QStringList & modList)
{
#if QT_VERSION >= 0x040000
  int count = this->pogMessageList->topLevelItemCount();
  if (modList.isEmpty() && count > 0)
#else
  if(modList.isEmpty() && this->pogMessageList->firstChild())
#endif // QT_VERSION >= 0x040000
  {
    this->pogMessageList->clear();
    this->pogDescription->clear();
  } else {
    for ( QStringList::const_iterator it = modList.begin(); it != modList.end(); ++it ) {
        cleanUp(*it);
    }
  }
  this->cleanupFilter();
}

void pogW::cleanUp(const QString & modName)
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem*> list (this->pogMessageList->findItems(modName, Qt::MatchExactly, 0));
  for (QList<QTreeWidgetItem*>::Iterator itr = list.begin(); itr != list.end(); itr++)
  {
    int index = this->pogMessageList->indexOfTopLevelItem(*itr);
    this->pogMessageList->takeTopLevelItem(index);
  }
#else
  int modnmIdx = getColumnIdx(MODULE_COLUMN);
  this->pogMessageList->setSorting(modnmIdx);
  this->pogMessageList->sort();
  QListViewItem* i = this->pogMessageList->firstChild ();
  while(i) {
    if(i->text(modnmIdx)==modName) break;
    i = i->nextSibling();
  }
  while(i) {
    QListViewItem* inext = i ? i->nextSibling() : NULL;
    this->pogMessageList->takeItem(i);
    if(inext==NULL || inext->text(modnmIdx)!=modName) {
      break;
    } 
    i = inext;
  }
#endif // QT_VERSION >= 0x040000
}

void pogW::cleanupFilter()
{
  for(int i = 0; i < (int)numof(columns); i++)
  {
#if QT_VERSION >= 0x040000
    pogFilterW * filter = (pogFilterW*)this->pogFilters->widget(i);
#else
    pogFilterW * filter = (pogFilterW*)this->pogFilters->page(i);
#endif // QT_VERSION >= 0x040000
    if(filter != NULL)
    {
      filter->selectedList->clear();
      filter->availableList->clear();
      filter->updateFilter();
    }
  }
#if QT_VERSION >= 0x040000
  this->pogFilters->setCurrentIndex(0);
#else
  this->pogFilters->setCurrentPage(0);
#endif // QT_VERSION >= 0x040000
}

void pogW::pogSelectionChanged_qt4()
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> list (this->pogMessageList->selectedItems());
  if (list.isEmpty())
    this->pogDescription->clear();
  else
    showCurrentIndex();
#endif // QT_VERSION >= 0x040000
}

#if QT_VERSION < 0x040000
void pogW::pogSelectionChanged_qt3(QListViewItem *item)
{
  if (item->isSelected()) {
    showCurrentIndex();
  } else {
    this->pogDescription->clear();
  }
}
#endif // QT_VERSION < 0x040000

void pogW::showCurrentIndex()
{
  this->pogDescription->clear();

#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> list (this->pogMessageList->selectedItems());
  POGQListViewItem * item = (POGQListViewItem*)list[0];
#else
  POGQListViewItem * item = (POGQListViewItem*)this->pogMessageList->selectedItem ();
#endif // QT_VERSION >= 0x040000
  
  if(item != NULL) {

#if QT_VERSION >= 0x040000
    this->pogDescription->setPlainText(item->mPogDesc);
    this->pogDescription->update();
#else
    this->pogDescription->setText(item->mPogDesc);
    this->pogDescription->repaint();
    this->pogMessageList->ensureItemVisible(item);
#endif // QT_VERSION >= 0x040000

    int line   = item->mLine;
    int column = item->mCol;
    int length = item->mLength;

    QString fn (item->mTmpFilename.isEmpty() ? item->mFilename : item->mTmpFilename);

    emit resetFilePosition(item->mFilename, fn, line, column, length);
  }
}

void pogW::clearAllMessages()
{
  this->pogMessageList->clear();
  this->pogDescription->clear();
}

void pogW::nextMessage()
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = this->pogMessageList->currentItem();
  if (item != NULL)
  { 
    int index = this->pogMessageList->indexOfTopLevelItem(item);
    index++;
    QTreeWidgetItem * nitem = this->pogMessageList->topLevelItem(index);
    if (nitem != NULL)
    {
      this->pogMessageList->setCurrentItem(nitem);
      showCurrentIndex();
    }
  }
#else
  QListViewItem* item = this->pogMessageList->currentItem ();
  if(item != NULL) {
    QListViewItem* nexti = item->itemBelow() ; 
    if(nexti != NULL) {
      this->pogMessageList->setCurrentItem(nexti);
      showCurrentIndex();
    }
  }
#endif // QT_VERSION >= 0x040000
}

void pogW::previousMessage()
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = this->pogMessageList->currentItem();
  if (item != NULL)
  { 
    int index = this->pogMessageList->indexOfTopLevelItem(item);
    index--;
    QTreeWidgetItem * nitem = this->pogMessageList->topLevelItem(index);
    if (nitem != NULL)
    {
      this->pogMessageList->setCurrentItem(nitem);
      showCurrentIndex();
    }
  }
#else
  QListViewItem* item = this->pogMessageList->currentItem ();
  if(item != NULL) {
    QListViewItem* nexti = item->itemAbove() ; 
    if(nexti != NULL) {
      this->pogMessageList->setCurrentItem(nexti);
      showCurrentIndex();
    }
  }
#endif // QT_VERSION >= 0x040000
}

void pogW::firstMessage()
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = this->pogMessageList->topLevelItem(0);
#else
  QListViewItem* item = this->pogMessageList->firstChild  ();
#endif // QT_VERSION >= 0x040000
  if(item != NULL) {
    this->pogMessageList->setCurrentItem(item);
    showCurrentIndex();
  }
}

void pogW::lastMessage()
{
}

void pogW::messagesAllDone()
{
  firstMessage();
}

void pogW::updateFilters()
{
#if QT_VERSION >= 0x040000
  pogFilterW * filter = (pogFilterW*)this->pogFilters->currentWidget();
#else
  pogFilterW * filter = (pogFilterW*)this->pogFilters->currentPage();
#endif // QT_VERSION >= 0x040000
  if(filter != NULL) filter->updateFilter();
}

#if QT_VERSION >= 0x040000
void pogW::pogDoubleClicked_qt4(QTreeWidgetItem* i, int column)
#else
void pogW::pogDoubleClicked_qt4(QTreeWidgetItem* i, int column)
{
}

void pogW::pogDoubleClicked_qt3(QListViewItem* i)
#endif // QT_VERSION >= 0x040000
{
  POGQListViewItem* item = (POGQListViewItem*)i;
  item->mCheckMark = Qt2TB::setCheckMark(item->mModName, item->mMemberName, item->mLocationType, item->mPogNum);
  item->setText(getColumnIdx(CHECKED_COLUMN), item->mCheckMark);
  this->pogMessageList->repaint();
//  this->pogMessageList->update();
}

void pogW::markAsChecked()
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> list (this->pogMessageList->selectedItems());
  if (!list.isEmpty())
  {
    pogDoubleClicked_qt4(list[0], 0);
  }
#else
  POGQListViewItem * item = (POGQListViewItem*)this->pogMessageList->selectedItem ();
  if(item != NULL) pogDoubleClicked_qt3(item);
#endif // QT_VERSION >= 0x040000
}

void pogW::applyFilters()
{
  clearAllMessages();
  PogList pos (Qt2TB::applyFilters());
  for( PogList::Iterator it = pos.begin(); it != pos.end(); ++it )
  {
    PogInfo pi (*it);
    addPogItem(pi.getCheckMark(),    // checked
               pi.getModName(),      // modnm
               pi.getMemberName(),   // memnm
               pi.getLocationType(), // loc class
               pi.getPogType(),      // kind
               pi.getPogNum(),       // no
               pi.getFilename(),     // file
               pi.getTmpFilename(),  // temp file
               pi.getLine(),         // line
               pi.getCol(),          // column
               pi.getLength(),       // length
               pi.getPogDesc()       // po
    );
  }
}

/*  
 *  Destroys the object and frees any allocated resources
 */
pogW::~pogW()
{
    // no need to delete child widgets, Qt does it all for us
}

//------------------------------ Filter Window ----------------------------

pogFilterW::pogFilterW(const QICON & selectIcon,
                       const QICON & deselectIcon,
                       QWidget* parent, const char* name, WFlags fl )
#if QT_VERSION >= 0x040000
    : QWidget( parent, fl )
#else
    : QWidget( parent, name, fl )
#endif // QT_VERSION >= 0x040000
{
  this->setWindowName( name );

  QHBoxLayout * Layout2 = new QHBoxLayout(this); 
  Layout2->setSpacing( 6 );
  Layout2->setMargin( 0 );

  QVBoxLayout * Layout1 = new QVBoxLayout; 
  Layout1->setSpacing( 6 );
  Layout1->setMargin( 0 );

  addB = new QToolButton( this );
#if QT_VERSION >= 0x040000
  addB->setIcon(selectIcon);
  addB->setToolTip( tr( "Click to add the selected items" ) );
#else
  addB->setIconSet(selectIcon);
  QToolTip::add(  addB, tr( "Click to add the selected items" ) );
#endif // QT_VERSION >= 0x040000
  Layout1->addWidget( addB );

  removeB = new QToolButton( this );
#if QT_VERSION >= 0x040000
  removeB->setIcon(deselectIcon);
  removeB->setToolTip( tr( "Click to remove the selected items" ) );
#else
  removeB->setIconSet(deselectIcon);
  QToolTip::add(  removeB, tr( "Click to remove the selected items" ) );
#endif // QT_VERSION >= 0x040000
  Layout1->addWidget( removeB );

#if QT_VERSION >= 0x040000
  QStringList alabels;
  alabels.append(tr( "Available" ));

  QTreeWidget* alv = new QTreeWidget( this );

  int fontSize3 = 12;
  QFont font3= QFont("monospace", fontSize3);
  font3.setStyleHint(QFont::TypeWriter);
  int fontPxSize3= QFontMetrics(font3).width('0');
  alv->setFont(font3);

  alv->setColumnCount (1);
  alv->setHeaderLabels(alabels);
  alv->setRootIsDecorated(false);
  alv->setSelectionMode(QAbstractItemView::MultiSelection);
  alv->setColumnWidth(0, 220);
  this->availableList = alv;
#else
  this->availableList = new QListView( this );
  this->availableList->addColumn( tr( "Available" ) );
  this->availableList->setSelectionMode(QListView::Multi);
  this->availableList->setColumnWidthMode (0, QListView::Manual );
  this->availableList->setColumnWidth (0, 220 );
#endif // QT_VERSION >= 0x040000

  Layout2->addWidget( this->availableList );

  Layout2->addLayout( Layout1 );

#if QT_VERSION >= 0x040000
  QStringList slabels;
  slabels.append(tr( "Selected" ));

  QTreeWidget* slv = new QTreeWidget( this );

  int fontSize4 = 12;
  QFont font4 = QFont("monospace", fontSize4);
  font4.setStyleHint(QFont::TypeWriter);
  int fontPxSize4 = QFontMetrics(font4).width('0');
  slv->setFont(font4);

  slv->setColumnCount (1);
  slv->setHeaderLabels(slabels);
  slv->setRootIsDecorated(false);
  slv->setSelectionMode(QAbstractItemView::MultiSelection);
  slv->setColumnWidth(0, 220);
  this->selectedList = slv;
#else
  this->selectedList = new QListView( this );
  this->selectedList->addColumn( tr( "Selected" ) );
  this->selectedList->setColumnWidthMode (0, QListView::Manual );
  this->selectedList->setColumnWidth (0, 220 );
  this->selectedList->setSelectionMode(QListView::Multi);
#endif // QT_VERSION >= 0x040000

  Layout2->addWidget( this->selectedList );

  QObject::connect(addB, SIGNAL(pressed()), this, SLOT(add()));
  QObject::connect(removeB, SIGNAL(pressed()), this, SLOT(remove()));
}

void pogW::setBusy(bool busy)
{
}

pogFilterW::~pogFilterW()
{
}

void pogFilterW::updateFilter()
{
//
// Get the set of available values...
//
  QString nm = this->getWindowName();
  int key = pogW::getColumnKeyByName(nm);

  QStringList available;
  if(key >= 0 && (pogW::columns[key].getAvailable != NULL)) {
    available = pogW::columns[key].getAvailable();
  }
//
// Update the list of available...
//
  this->availableList->clear();

  QStringList setOfSelected;
  for( QStringList::const_iterator it = available.begin(); it != available.end(); ++it )
  {
    QString itemText (*it);

#if QT_VERSION >= 0x040000
    int count = this->selectedList->topLevelItemCount();    
    bool exists = false;
    for (int index = 0; (index < count) && !exists; index++)
    {
      QTreeWidgetItem * item = this->selectedList->topLevelItem(index);
      exists = (item->text(0) == itemText);
    }
    if (exists)
    {
      setOfSelected.append(itemText);
    }
    else
    {
      // check duplivcate item ("non-emptiness of let be such binding")
      int count = this->availableList->topLevelItemCount();    
      bool item_exists = false;
      for (int index = 0; (index < count) && !item_exists; index++)
      {
        QTreeWidgetItem * item = this->availableList->topLevelItem(index);
        item_exists = (item->text(0) == itemText);
      }
      if (!item_exists)
      {
        QTreeWidgetItem * sitem = new QTreeWidgetItem();
        sitem->setText(0, itemText);
        this->availableList->addTopLevelItem(sitem);
      }
    }
#else
    bool found = false;
    QListViewItem *item = this->selectedList->firstChild();
    while(item) {
      if(item->text(0)==itemText) {
        found = true;
        break;
      }
      item = item->nextSibling();
    }
    if(!found) {
      new QListViewItem(this->availableList, itemText);
    } else {
      setOfSelected.append(itemText);
    }
#endif // QT_VERSION >= 0x040000
  }
  pogW::columns[key].setFilter(setOfSelected);
#if QT_VERSION >= 0x040000
  this->selectedList->repaint();
  this->availableList->repaint();
#endif // QT_VERSION >= 0x040000
}

void pogFilterW::add()
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> sel (this->availableList->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = sel.begin(); itr != sel.end(); itr++)
  {
    QTreeWidgetItem * sitem = new QTreeWidgetItem();
    sitem->setText(0, (*itr)->text(0));
    this->selectedList->addTopLevelItem(sitem);
  }
#else
  QListViewItemIterator it (this->availableList);
  for ( ; it.current(); ++it ) {
    if ( it.current()->isSelected() ) {
      QListViewItem* i = it.current();
      new QListViewItem(this->selectedList, i->text(0));
    }
  }
#endif // QT_VERSION >= 0x040000
  this->updateFilter();
}

void pogFilterW::remove()
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> sel (this->selectedList->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = sel.begin(); itr != sel.end(); itr++)
  {
    int index = this->selectedList->indexOfTopLevelItem(*itr);
    this->selectedList->takeTopLevelItem(index);
  }
  sel.clear();
#else
  QListViewItemIterator it (this->selectedList);
  for ( ; it.current(); ++it ) {
    if ( it.current()->isSelected() ) {
      QListViewItem* i = it.current();
      this->selectedList->takeItem(i);
    }
  }
#endif // QT_VERSION >= 0x040000
  this->updateFilter();
}

//
// Custom list view item
//
POGQListViewItem::POGQListViewItem(pogW * pogw, 
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
                                   const QString & pogDesc)
#if QT_VERSION >= 0x040000
      : QTreeWidgetItem(pogw->pogMessageList),
#else
      : QListViewItem(pogw->pogMessageList, checkMark),
#endif // QT_VERSION >= 0x040000
        mCheckMark(checkMark),
        mModName(modName),
        mMemberName(memberName),
        mLocationType(locationType),
        mPogType(pogType),
        mPogNum(pogNum),
        mFilename(filename),
        mTmpFilename(tmpFilename),
        mLine(line),
        mCol(col),
        mLength(length),
        mPogDesc(pogDesc)
{
#if QT_VERSION >= 0x040000
  this->setText(pogw->getColumnIdx(CHECKED_COLUMN), checkMark);
#endif // QT_VERSION >= 0x040000
  this->setText(pogw->getColumnIdx(MODULE_COLUMN), modName);
  this->setText(pogw->getColumnIdx(MEMBER_COLUMN), memberName);
  this->setText(pogw->getColumnIdx(LOCATION_COLUMN), locationType);
  QString tmp;
  tmp.sprintf("%3d", pogNum);
  this->setText(pogw->getColumnIdx(NUMBER_COLUMN), tmp);
  this->setText(pogw->getColumnIdx(TYPE_COLUMN), pogType);
#if QT_VERSION >= 0x040000
  this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
#else
  this->setSelectable(true);
#endif // QT_VERSION >= 0x040000
}
