/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/roseF.cc,v $
*  * VERSION
*  *    $Revision: 1.19 $
*  * DATE
*  *    $Date: 2006/04/19 08:11:25 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "roseF.h"
#include "mainF.h"
#include "interface2TB.h"

#include "tbutils.h"

static const char* const image0_data[] = { 
"33 24 2 1",
"# c #000000",
". c #d5d2d5",
".................................",
".................................",
".................................",
".................................",
".....######.##.##................",
".....########.##.##..............",
".....##########.##.##............",
".....###########..#..#...........",
"...........######..#..#..........",
"............######..#..#.........",
".............#####..#..#.........",
"..............####..#..#.........",
"..........##..####...##.##.##....",
"..........###.####..###..#..#....",
"..........########.####..#..#....",
"..........#############..#..#....",
"..........#############..#..#....",
"...........###########..#..#.....",
"............#########..#..#......",
".............#######..#..#.......",
"..............#####..#..#........",
"...............###.##..#.........",
".................................",
"................................."};

static const char* const rightArrow[] = { 
//"44 24 2 1",
"44 23 2 1",
"# c #000000",
". c #ffffff",
//"............................................",
"............................................",
"............................................",
"............................................",
"..........######.##.##......................",
"..........########.##.##....................",
"..........##########.##.##..................",
"..........###########..#..#.................",
"................######..#..#................",
".................######..#..#...............",
"..................#####..#..#...............",
"...................####..#..#...............",
"...............##..####...##.##.##..........",
"...............###.####..###..#..#..........",
"...............########.####..#..#..........",
"...............#############..#..#..........",
"...............#############..#..#..........",
"................###########..#..#...........",
".................#########..#..#............",
"..................#######..#..#.............",
"...................#####..#..#..............",
"....................###.##..#...............",
"............................................",
"............................................"};

static const char* const image1_data[] = { 
"34 23 2 1",
"# c #000000",
". c #d5d2d5",
"..................................",
"..................................",
"..................................",
".................##.##.######.....",
"...............##.##.########.....",
".............##.##.##########.....",
"............#..#..###########.....",
"...........#..#..######...........",
"..........#..#..######............",
"..........#..#..#####.............",
"..........#..#..####..............",
".....##.##.##...####..##..........",
".....#..#..###..####.###..........",
".....#..#..####.########..........",
".....#..#..#############..........",
".....#..#..#############..........",
"......#..#..###########...........",
".......#..#..#########............",
"........#..#..#######.............",
".........#..#..#####..............",
"..........#..##.###...............",
"..................................",
".................................."};

static const char* const leftArrow[] = { 
"44 23 2 1",
"# c #000000",
". c #ffffff",
"............................................",
"............................................",
"............................................",
"......................##.##.######..........",
"....................##.##.########..........",
"..................##.##.##########..........",
".................#..#..###########..........",
"................#..#..######................",
"...............#..#..######.................",
"...............#..#..#####..................",
"...............#..#..####...................",
"..........##.##.##...####..##...............",
"..........#..#..###..####.###...............",
"..........#..#..####.########...............",
"..........#..#..#############...............",
"..........#..#..#############...............",
"...........#..#..###########................",
"............#..#..#########.................",
".............#..#..#######..................",
"..............#..#..#####...................",
"...............#..##.###....................",
"............................................",
"............................................"};

static const char* const bothArrows[] = {
"44 23 2 1",
"# c #000000",
". c #ffffff",
"............................................",
"............................................",
"............................................",
"..........######..........#######...........",
"..........########......#########...........",
"..........##########...##########...........",
"..........###########.###########...........",
"................###########.................",
"..................#######...................",
"...................#####....................",
"...................#####....................",
"...............##..#####..##................",
"...............###.#####.###................",
"...............#############................",
"...............#############................",
"...............#############................",
"................###########.................",
".................#########..................",
"..................#######...................",
"...................#####....................",
"....................###.....................", 
"............................................",
"............................................"};


static const char* const image3_data[] = {
"44 23 2 1",
"# c #000000",
". c #ffffff",
"............................................",
"............................................",
"............................................",
"............................................",
"............................................",
".................#........#.................",
"................###......###................",
"................####....####................",
".................####..####.................",
"..................########..................",
"...................######...................",
"....................####....................",
"...................######...................",
"..................########..................",
".................####..####.................",
"................####....####................",
"................###......###................",
".................#........#.................",
"............................................",
"............................................",
"............................................",
"............................................",
"............................................"}; 


/* 
 *  Constructs a roseW which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
roseW::roseW( mainW* mainWptr, QWidget* parent,  const char* name, bool modal, WFlags fl )
#if QT_VERSION >= 0x040000
    : QDialog( parent, fl )
#else
    : QDialog( parent, name, modal, fl )
#endif // QT_VERSION >= 0x040000
{
  this->mw = mainWptr;

  this->setWindowName( (name == NULL) ? "roseW" : name );

#if QT_VERSION >= 0x040000
  this->setModal(modal);
  this->setWindowTitle( tr( "UML Link Window (VDM++ <-> UML)" ) );
#else
  if ( !name ) this->setName( "roseW" );
  this->setCaption( tr( "UML Link Window (VDM++ <-> UML)" ) );
#endif // QT_VERSION >= 0x040000

  setSizeGripEnabled( true );

  this->CreateRoseWindow();

  this->rosemode = false;
}

void roseW::CreateRoseWindow()
{
  // Setup the pixmaps used for the mapping direction
  this->vdm2umlPixmap = new QPixmap( ( const char** ) rightArrow );
  this->uml2vdmPixmap = new QPixmap( ( const char** ) leftArrow );
  this->defaultPixmap = new QPixmap( ( const char** ) bothArrows );
  this->deletePixmap  = new QPixmap( ( const char** ) image3_data );
  

  QVBoxLayout * roseWLayout = this->createVBoxLayout( this );

#if QT_VERSION >= 0x040000
  roseWLayout->addWidget(this->createClassesListView( this ));
  roseWLayout->addItem(this->createDiffAndMapBoxLayout( this ));
  roseWLayout->addWidget(this->createOutputBox( this ));
  roseWLayout->addWidget(this->createCancelButton( this ));
#else
  roseWLayout->setMargin( 6 );
  roseWLayout->add(this->createClassesListView( this ));
  roseWLayout->addItem(this->createDiffAndMapBoxLayout( this ));
  roseWLayout->add(this->createOutputBox( this ));
  roseWLayout->add(this->createCancelButton( this ));
#endif // QT_VERSION >= 0x040000
}

QLayout * roseW::createDiffAndMapBoxLayout( QWidget * parent )
{
  QHBoxLayout * layout = this->createHBoxLayout( NULL );
#if QT_VERSION >= 0x040000
  layout->addWidget(this->createDiffBox( parent )); 
  layout->addWidget(this->createMapBox( parent )); 
#else
  layout->add(this->createDiffBox( parent )); 
  layout->add(this->createMapBox( parent )); 
#endif // QT_VERSION >= 0x040000
  return layout;
}

QGroupBox * roseW::createMapBox( QWidget * parent )
{
  QGroupBox * gb = this->createVGroupBox( parent );
  gb->setTitle( tr( "Map" ) );
  QLayout * layout = gb->layout();
#if QT_VERSION >= 0x040000
#ifdef __darwin__
  layout->setMargin(0);
#endif // __darwin__
#endif // QT_VERSION >= 0x040000

  QBoxLayout * hlayout = this->createHBoxLayout( NULL );
#if QT_VERSION >= 0x040000
  hlayout->addWidget(this->createMapButton(gb));
  hlayout->addWidget(this->createMapDirBox(gb));
#else
  hlayout->add(this->createMapButton(gb));
  hlayout->add(this->createMapDirBox(gb));
#endif // QT_VERSION >= 0x040000

  layout->addItem(hlayout);
  return gb;
}

QGroupBox * roseW::createMapDirBox( QWidget * parent )
{
  QGroupBox * gb = this->createVGroupBox( parent );
  gb->setTitle( tr( "All" ) );

  QLayout * layout = gb->layout();

  QHBoxLayout * hlayout = this->createHBoxLayout( NULL );
#if QT_VERSION >= 0x040000
  hlayout->addWidget(this->createVdmUmlButton( gb ));
  hlayout->addWidget(this->createUmlVdmButton( gb ));
#else
  hlayout->add(this->createVdmUmlButton( gb ));
  hlayout->add(this->createUmlVdmButton( gb ));
#endif // QT_VERSION >= 0x040000
  layout->addItem(hlayout);

  gb->setFixedSize( gb->sizeHint() );
  return gb;
}

QToolButton * roseW::createVdmUmlButton( QWidget * parent )
{
  QPixmap* vdm2umlButton = new QPixmap( ( const char** ) image0_data );

  QToolButton * tb = new QToolButton( parent );
  tb->setMaximumSize( QSize( 30, 30 ) );
  tb->setText( tr( "" ) );
#if QT_VERSION >= 0x040000
  tb->setIconSize(QSize(30,30));
  tb->setIcon(QIcon(QPixmap(*vdm2umlButton)));
  tb->setToolTip(tr( "All VDM to UML" ));
  tb->setWhatsThis(( "All VDM to UML" ));
#else
  tb->setPixmap( QPixmap(*vdm2umlButton ));
  tb->setUsesBigPixmap( true );
  QToolTip::add( tb, tr( "All VDM to UML" ) );
  QWhatsThis::add( tb, tr( "All VDM to UML" ) );
#endif // QT_VERSION >= 0x040000

  connect( tb, SIGNAL(clicked()), this, SLOT( defaultToUML()));

  return tb;
}

QToolButton * roseW::createUmlVdmButton( QWidget * parent )
{
  QPixmap* uml2vdmButton = new QPixmap( ( const char** ) image1_data );

  QToolButton * tb = new QToolButton( parent );
  tb->setMaximumSize( QSize( 30, 30 ) );
  tb->setText( tr( "" ) );
#if QT_VERSION >= 0x040000
  tb->setIconSize(QSize(30,30));
  tb->setIcon(QIcon(QPixmap(*uml2vdmButton)));
  tb->setToolTip(tr( "All UML to VDM" ));
  tb->setWhatsThis(tr( "All UML to VDM" ));
#else
  tb->setPixmap( QPixmap(*uml2vdmButton ));
  tb->setUsesBigPixmap( true );
  QToolTip::add( tb, tr( "All UML to VDM" ) );
  QWhatsThis::add( tb, tr( "All UML to VDM" ) );
#endif // QT_VERSION >= 0x040000

  connect( tb, SIGNAL(clicked()), this, SLOT( defaultToVDM()));

  return tb;
}

QPushButton * roseW::createMapButton( QWidget * parent )
{
  QPushButton * pb = new QPushButton( parent );
  pb->setText( mainW::mf(tr( "&Map" )) );
  pb->setAutoDefault( true );
#if QT_VERSION >= 0x040000
  pb->setToolTip(tr( "Perform mapping between VDM and UML models" ));
  pb->setWhatsThis(tr( "Map classes" ));
#else
  QToolTip::add( pb, tr( "Perform mapping between VDM and UML models" ) );
  QWhatsThis::add( pb, tr( "Map classes" ) );
#endif // QT_VERSION >= 0x040000

  connect( pb,    SIGNAL(clicked()), this, SLOT( mapClasses()));

  return pb;
}

QPushButton * roseW::createCancelButton( QWidget * parent )
{
  QPushButton * pb = new QPushButton( parent );
  pb->setText( mainW::mf(tr( "&Cancel" )) );
  pb->setAutoDefault( true );
#if QT_VERSION >= 0x040000
  pb->setToolTip(tr( "Cancel changes" ));
#else
  QToolTip::add( pb, tr( "Cancel changes" ) );
#endif // QT_VERSION >= 0x040000

  connect( pb, SIGNAL(clicked()), this, SLOT( reject() ) );

  return pb;
}

QGroupBox * roseW::createDiffBox( QWidget * parent )
{
  QGroupBox * gb = this->createVGroupBox( parent );
  gb->setTitle( tr( "Differences" ) );
  QLayout * layout = gb->layout();
#if QT_VERSION >= 0x040000
#ifdef __darwin__
  layout->setMargin(0);
#endif // __darwin__
#endif // QT_VERSION >= 0x040000

  QHBoxLayout * hlayout = this->createHBoxLayout(NULL);
#if QT_VERSION >= 0x040000
  hlayout->addWidget( this->createDiffButton( gb ) );
#else
  hlayout->add( this->createDiffButton( gb ) );
#endif // QT_VERSION >= 0x040000
  hlayout->addItem( this->createSelectLayout( gb ) );

  layout->addItem(hlayout);

//  gb->setFixedSize( gb->sizeHint() );
  return gb;
}

QLayout * roseW::createSelectLayout( QWidget * parent )
{
  QVBoxLayout * layout = this->createVBoxLayout( NULL );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->createSelectAllButton( parent ) );
  layout->addWidget( this->createSelectNoneButton( parent ) );
#else
  layout->add( this->createSelectAllButton( parent ) );
  layout->add( this->createSelectNoneButton( parent ) );
#endif // QT_VERSION >= 0x040000
  return layout;
}

QToolButton * roseW::createSelectAllButton( QWidget * parent )
{
  QToolButton * tb = new QToolButton( parent );
  tb->setText( tr( "Select All" ) );
#if QT_VERSION >= 0x040000
  tb->setToolTip(tr( "Select all classes" ));
#else
  tb->setTextLabel( tr( "..." ) );
  QToolTip::add( tb, tr( "Select all classes" ) );
#endif // QT_VERSION >= 0x040000

  connect( tb,    SIGNAL(clicked()), this, SLOT( selectAllClasses()));

  return tb;
}

QToolButton * roseW::createSelectNoneButton( QWidget * parent )
{
  QToolButton * tb = new QToolButton( parent );
  tb->setText( tr( "Select None" ) );
#if QT_VERSION >= 0x040000
  tb->setToolTip(tr( "Deselect all" ));
#else
  QToolTip::add( tb, tr( "Deselect all" ) );
#endif // QT_VERSION >= 0x040000

  connect( tb,   SIGNAL(clicked()), this, SLOT( selectNoClasses()));

  return tb;
}

QPushButton * roseW::createDiffButton( QWidget * parent )
{
  QPushButton * pb = new QPushButton( parent );
  pb->setText( mainW::mf(tr( "&Diff Selected" )) );
  pb->setAutoDefault( true );
#if QT_VERSION >= 0x040000
  pb->setToolTip(tr( "Show differences between selected classes" ));
  pb->setWhatsThis(tr( "Show differences between selected classes" ));
#else
  QToolTip::add( pb, tr( "Show differences between selected classes" ));
  QWhatsThis::add( pb, tr( "Show differences between selected classes" ));
#endif // QT_VERSION >= 0x040000

  connect( pb,   SIGNAL(clicked()), this, SLOT( diffSelectedClasses()));

  return pb;
}

QGroupBox * roseW::createOutputBox( QWidget * parent )
{
  QGroupBox * gb = this->createVGroupBox( parent );
  gb->setTitle(tr("Messages"));

  QLayout * layout = gb->layout();
  layout->setMargin(0);
#if QT_VERSION >= 0x040000
  layout->addWidget(this->createOutputLE(gb));
#else
  layout->add(this->createOutputLE(gb));
#endif // QT_VERSION >= 0x040000

  return gb;
}

QTextEdit * roseW::createOutputLE( QWidget * parent )
{
  QTextEdit * te = new QTextEdit( parent );
#if QT_VERSION >= 0x040000
  te->setAcceptRichText(false);
#else
  te->setTextFormat( Qt::PlainText );
#endif // QT_VERSION >= 0x040000
  te->setReadOnly( true );
  this->outputW = te;
  return te;
}

QTREEWIDGET* roseW::createClassesListView( QWidget * parent )
{
#if QT_VERSION >= 0x040000
  QStringList hlabels;
  hlabels.append(tr( "Classes" ));
  hlabels.append(tr( "VDM++" ));
  hlabels.append(tr( "Action" ));
  hlabels.append(tr( "UML  " ));

  QTreeWidget * lv = new QTreeWidget( parent );
  lv->setColumnCount (4);
  lv->setHeaderLabels(hlabels);
  lv->header()->setMovable(false);
  lv->header()->setStretchLastSection(false);
  lv->header()->setResizeMode(0, QHeaderView::Interactive);
  lv->header()->setResizeMode(1, QHeaderView::Fixed);
  lv->header()->setResizeMode(2, QHeaderView::Fixed);
  lv->header()->setResizeMode(3, QHeaderView::Fixed);

  lv->setRootIsDecorated(false);
  lv->setSelectionMode(QAbstractItemView::ExtendedSelection);
  lv->setColumnWidth(0, 100);
  lv->setColumnWidth(1, 60);
  lv->setColumnWidth(2, 60);
  lv->setColumnWidth(3, 60);
  lv->setIconSize(QSize(32, 32));

  connect( lv, SIGNAL(itemClicked(QTreeWidgetItem*, int)), 
           this, SLOT(toggleAction_qt4(QTreeWidgetItem*, int)));
#else
  // Add List view for list of classes with VDM++ and UML states
  QListView * lv = new QListView( parent );
  lv->addColumn( tr( "Classes" ) );
  lv->addColumn( tr( "VDM++" ) );
  lv->addColumn( tr( "Action" ) );
  lv->addColumn( tr( "UML  " ) );
  lv->setColumnWidth(0, 360);
    
  lv->setFrameShape( QListView::StyledPanel );
  lv->setFrameShadow( QListView::Sunken );
  lv->setSelectionMode( QListView::Extended );
  lv->setAllColumnsShowFocus(true);

  connect( lv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), 
           this, SLOT(toggleAction_qt3(QListViewItem*, const QPoint&, int)));
#endif // QT_VERSION >= 0x040000

  this->classes = lv;
  return lv;
}

QVBoxLayout* roseW::createVBoxLayout( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout( parent );
  layout->setSpacing( 6 );
  return layout;
}

QHBoxLayout* roseW::createHBoxLayout( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout( parent );
  layout->setSpacing( 6 );
  return layout;
}

QGroupBox * roseW::createVGroupBox( QWidget* parent )
{
  QGroupBox* gbox = new QGroupBox( parent );
#if QT_VERSION >= 0x040000
  QVBoxLayout* layout = this->createVBoxLayout( NULL );
  gbox->setLayout( layout );
#else
  gbox->setOrientation( Qt::Vertical );
  QLayout* layout = gbox->layout();
  layout->setSpacing(6);
  layout->setMargin(6);
#endif // QT_VERSION >= 0x040000
  return gbox;
}

bool roseW::AddAllClasses()
{
//
  if(!IsRoseMode())
  {
    Qt2TB::setBlock( false );
    QString prjnm (Qt2TB::getProjectNameI());
    Qt2TB::setBlock( true );

    if (prjnm.isNull())
    {
      Qt2TB::setBlock( false );
      QStringList files(Qt2TB::getProjectFilesI());
      Qt2TB::setBlock( true );
      if (files.empty())
      {
        QString file (mw->getXMIFile());
        if (!file.isEmpty()) //at least one item 
        {
          Qt2TB::SetModelNamesI(file, QString(""));
        }
        else
          return false;
      }
    }
    else
    {
      Qt2TB::SetModelNamesI(QString(""), QString(""));
    }
  }
//

  // Initialize the Rose mapper
  Qt2TB::setBlock( false );
  bool st = Qt2TB::SetMapperStateI(true);
  Qt2TB::setBlock( true );

  this->SetMapperEnabled(st);

  if (this->MapperEnabled())
  {
    this->classes->clear();
#if QT_VERSION >= 0x040000
    QList<UMLClassState> cls_l;
#else
    QValueList<UMLClassState> cls_l;
#endif // QT_VERSION >= 0x040000

    Qt2TB::setBlock( false );
    Qt2TB::GetClassStatusI(cls_l);
    Qt2TB::setBlock( true );

    InitWindow(cls_l);
    return true;
  }
  else
    return false;
}

#if QT_VERSION >= 0x040000
void roseW::InitWindow(const QList<UMLClassState> & classes)
#else
void roseW::InitWindow(const QValueList<UMLClassState> & classes)
#endif // QT_VERSION >= 0x040000
{
#if QT_VERSION >= 0x040000
  for (QList<UMLClassState>::const_iterator iter = classes.begin(); iter != classes.end(); ++iter)
#else
  for (QValueList<UMLClassState>::const_iterator iter = classes.begin(); iter != classes.end(); ++iter)
#endif // QT_VERSION >= 0x040000
  {
    this->addClass(*iter);
  }
}

/*  
 *  Destroys the object and frees any allocated resources
 */
roseW::~roseW()
{
    // no need to delete child widgets, Qt does it all for us
}

string roseW::statusToChar(CLASS_STATUS st) const
{
  switch(st) {
   case STATUS_UNCHANGED: return "-";
   case STATUS_MODIFIED:  return "M";
   case STATUS_ADDED:     return "A";
   case STATUS_DELETED:   return "D"; 
   case STATUS_UNKNOWN:   return "?";
  }
  return "?"; // dummy
}

CLASS_STATUS textToStatus(QString st)
{
#if QT_VERSION >= 0x040000
  if(st.indexOf(QString("-"))>=0) return STATUS_UNCHANGED;
  if(st.indexOf(QString("M"))>=0) return STATUS_MODIFIED;
  if(st.indexOf(QString("A"))>=0) return STATUS_ADDED;
  if(st.indexOf(QString("D"))>=0) return STATUS_DELETED;
#else
  if(st.find(QString("-"))>=0) return STATUS_UNCHANGED;
  if(st.find(QString("M"))>=0) return STATUS_MODIFIED;
  if(st.find(QString("A"))>=0) return STATUS_ADDED;
  if(st.find(QString("D"))>=0) return STATUS_DELETED;
#endif // QT_VERSION >= 0x040000
  return STATUS_UNKNOWN;
}

QPixmap* roseW::actionToImage(MAP_ACTION action) const
{
  switch(action) {
   case ACTION_VDM2UML: return vdm2umlPixmap;
   case ACTION_UML2VDM: return uml2vdmPixmap;
   case ACTION_DEFAULT: return defaultPixmap;
   case ACTION_DELETE:  return deletePixmap;
  }
  return defaultPixmap; // dummy
}

MAP_ACTION roseW::imageToAction(const QPixmap* pixmap) const
{
  int serialNum = pixmap->serialNumber();

  if (serialNum == vdm2umlPixmap->serialNumber())
    return ACTION_VDM2UML;

  else if (serialNum == defaultPixmap->serialNumber())
    return ACTION_DEFAULT;

  else if (serialNum == uml2vdmPixmap->serialNumber())
    return ACTION_UML2VDM;

  else
    return ACTION_DELETE;
}

void roseW::addClass(const UMLClassState & st)
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = new QTreeWidgetItem(this->classes); 
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  item->setTextAlignment(0, Qt::AlignLeft);
  item->setTextAlignment(1, Qt::AlignCenter);
  item->setTextAlignment(2, Qt::AlignCenter);
  item->setTextAlignment(3, Qt::AlignCenter);
  item->setText( 0, st.name);
  item->setText( 1, tr( statusToChar(st.vdmStatus).c_str() ));
  item->setIcon( 2, QIcon(QPixmap( *actionToImage(st.action) )));
  item->setText( 3, tr( statusToChar(st.umlStatus).c_str() ));
  this->classes->resizeColumnToContents(0);
  this->classes->sortItems(0, Qt::AscendingOrder);
#else
  QString padding = "   ";
  QListViewItem * item = new QListViewItem( this->classes, 0 );
  item->setText( 0, st.name);
  item->setText( 1, padding + tr( statusToChar(st.vdmStatus).c_str() ));
  item->setPixmap( 2, QPixmap( *actionToImage(st.action) ));
  item->setText( 3, padding + tr( statusToChar(st.umlStatus).c_str() ));
#endif // QT_VERSION >= 0x040000
}

void roseW::defaultToUML()
{
#if QT_VERSION >= 0x040000
  int count = this->classes->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = this->classes->topLevelItem(index);
    item->setIcon(2, QIcon(QPixmap(*vdm2umlPixmap)));
  }
#else
  QListViewItemIterator iter(this->classes);
  for( ; iter.current(); ++ iter)
    iter.current()->setPixmap(2, QPixmap(*vdm2umlPixmap));
#endif // QT_VERSION >= 0x040000
}

void roseW::defaultToVDM()
{
#if QT_VERSION >= 0x040000
  int count = this->classes->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    this->classes->topLevelItem(index)->setIcon(2, QIcon(QPixmap(*uml2vdmPixmap)));
  }
#else
  QListViewItemIterator iter(this->classes);
  for( ; iter.current(); ++iter)
    iter.current()->setPixmap(2, QPixmap(*uml2vdmPixmap));
#endif // QT_VERSION >= 0x040000
}

void roseW::diffSelectedClasses()
{
  QStringList selectedClasses;
#if QT_VERSION >= 0x040000
  int count = this->classes->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = this->classes->topLevelItem(index);
    selectedClasses.append(item->text(0));
  }
#else
  QListViewItemIterator iter(this->classes);
  for ( ; iter.current(); ++ iter)
    if (iter.current()->isSelected())
      selectedClasses += iter.current()->text(0);
#endif // QT_VERSION >= 0x040000

  emit sendUMLDiffCommand(selectedClasses);
}

void roseW::mapClasses()
{
#if QT_VERSION >= 0x040000
  QList<UMLClassState> settings;

  int count = this->classes->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = this->classes->topLevelItem(index);
    UMLClassState thisState;
    thisState.name = item->text(0);
    QPixmap pmap (item->icon(2).pixmap(44,23));
    thisState.action = imageToAction(&pmap); //
    thisState.vdmStatus = textToStatus(item->text(1)); 
    thisState.umlStatus = textToStatus(item->text(3)); 
    settings += thisState;
  }
#else
  QValueList<UMLClassState> settings;

  QListViewItemIterator iter (classes);
  for ( ; iter.current(); ++ iter)
  {
    UMLClassState thisState;
    thisState.name = iter.current()->text(0);
    thisState.action = imageToAction(iter.current()->pixmap(2));
    thisState.vdmStatus = textToStatus(iter.current()->text(1)); 
    thisState.umlStatus = textToStatus(iter.current()->text(3)); 
    settings += thisState;
  }
#endif // QT_VERSION >= 0x040000

  hide();

  Qt2TB::setBlock( false );
  QString prjnm (Qt2TB::getProjectNameI());
  Qt2TB::setBlock( true );

  // if ProjectName is empty
  // then get working dir and set path
  if( prjnm.isNull() )
  {
    QString path (mw->getDirectory());
    if( path.isNull() )
    {
      show();
      return;
    }
    
    wstring newpath (Qt2TB::qstring2wstring(path));

    Qt2TB::setBlock( false );
    TBUTILS::SetDefaultPath(newpath);
    TBUTILS::tb_chdir(newpath, vdm_log);
    Qt2TB::setBlock( true );
  }

  Qt2TB::setBlock( false );
  bool ok = Qt2TB::MapUMLI(settings);
  Qt2TB::setBlock( true );

  if(ok) {
    emit refreshFiles();
    accept();
  }
  else
    show();
}

void roseW::selectAllClasses()
{
#if QT_VERSION >= 0x040000
  int count = this->classes->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = this->classes->topLevelItem(index);
    item->setSelected(true);
  }
//  this->classes->repaint();
  this->classes->update();
#else
  QListViewItemIterator iter(classes);
  for( ; iter.current(); ++ iter) {
    iter.current()->setSelected(true);
    iter.current()->repaint();
  }
#endif // QT_VERSION >= 0x040000
}

void roseW::selectNoClasses()
{
#if QT_VERSION >= 0x040000
  int count = this->classes->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = this->classes->topLevelItem(index);
    item->setSelected(false);
  }
//  this->classes->repaint();
  this->classes->update();
#else
  QListViewItemIterator iter(classes);
  for( ; iter.current(); ++ iter)
  {
    iter.current()->setSelected(false);
    iter.current()->repaint();
  }
#endif // QT_VERSION >= 0x040000
}

QPixmap roseW::nextPixmap(const QPixmap* currentPixmap)
{
  int currentSerialNum = currentPixmap->serialNumber();
  if (currentSerialNum == vdm2umlPixmap->serialNumber())
    return QPixmap(*defaultPixmap);
  else if (currentSerialNum == defaultPixmap->serialNumber())
    return QPixmap(*uml2vdmPixmap);
  else if (currentSerialNum == uml2vdmPixmap->serialNumber())
    return QPixmap(*deletePixmap);
  else
    return QPixmap(*vdm2umlPixmap);
}

#if QT_VERSION >= 0x040000
void roseW::toggleAction_qt4(QTreeWidgetItem* item, int column)
#else
void roseW::toggleAction_qt4(QTreeWidgetItem* item, int column)
{
}

void roseW::toggleAction_qt3(QListViewItem * item, const QPoint &, int column)
#endif // QT_VERSION >= 0x040000
{
  if (item != NULL )
  {
    if (column == 2)
    {
      item->setSelected(false);
#if QT_VERSION >= 0x040000
      QPixmap pmap (item->icon(column).pixmap(44,23));
      item->setIcon(column, nextPixmap(&pmap));
#else
      item->setPixmap(column, nextPixmap(item->pixmap(column)));
      item->repaint();
#endif // QT_VERSION >= 0x040000
    }
  }
}


void roseW::addDiff(const QString & msg)
{
  this->outputW->append(msg);
#if QT_VERSION >= 0x040000
#else
  this->outputW->setCursorPosition(outputW->paragraphs(), 1);
#endif // QT_VERSION >= 0x040000
//  this->outputW->repaint();
  this->outputW->update();
}

void roseW::addError(const QString & msg)
{
  this->outputW->append(msg);
//  this->outputW->repaint();
  this->outputW->update();
}

void roseW::SetMapperEnabled(bool e)
{
  this->mapper_enabled = e;
}

bool roseW::MapperEnabled()
{
  return this->mapper_enabled;
}

bool roseW::IsRoseMode()
{
  return this->rosemode;
}

void roseW::SetRoseMode(bool b)
{
#ifdef _MSC_VER
  this->rosemode = b;
#endif // _MSC_VER
}

void roseW::show()
{
  string title;
  if (this->rosemode)
    title = "Rose Link Window (VDM++ <-> UML)";
  else
    title = "UML Link Window (VDM++ <-> UML)";

#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( title.c_str() ) );
#else
  this->setCaption( tr( title.c_str() ) );
#endif // QT_VERSION >= 0x040000
  QDialog::show();
}

