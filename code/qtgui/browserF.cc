/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/browserF.cc,v $
*  * VERSION
*  *    $Revision: 1.36 $
*  * DATE
*  *    $Date: 2006/06/29 05:26:28 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "browserF.h"
#include "interface2TB.h"

#ifndef USE_IMAGE_FILE
#include "imagedata.h"
#endif // USE_IMAGE_FILE

#include <iostream>
using namespace std;

#define HEADER_VDM "[VDM Files]"
#define HEADER_JAVA "[Java Files]"

/*
 *  Constructs a browserW which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
browserW::browserW( QWidget* parent,  const char* name, WFlags fl )
#if QT_VERSION >= 0x040000
    : QWidget( parent, fl )
#else
    : QWidget( parent, name, fl )
#endif // QT_VERSION >= 0x040000
{
  this->setWindowName( (name == NULL) ? "browserW" : name );
#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( "Manager" ) );
  this->setFocusPolicy(Qt::StrongFocus);
  this->setWhatsThis( tr( "The Project Browser allows the user to easily mantain and execute every project related action" ) );
  this->setContextMenuPolicy(Qt::DefaultContextMenu);
#else
  if ( !name ) setName( "browserW" );
  this->setCaption( tr( "Manager" ) );
  this->setFocusPolicy(QWidget::StrongFocus);
  this->setIconText( tr( "" ) );
  QWhatsThis::add(  this, tr( "The Project Browser allows the user to easily mantain and execute every project related action" ) );
#endif // QT_VERSION >= 0x040000

  QBoxLayout * layout = new QVBoxLayout( this );
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
  layout->addWidget( this->createBrowserWidget( this ) );

  // Preload pixmaps
  this->preloadPixmap();

  this->createEmptyView();
  this->update();

  this->pcount = 0;
}

/*
 *  Destroys the object and frees any allocated resources
 */
browserW::~browserW()
{
    // no need to delete child widgets, Qt does it all for us
}

bool browserW::event (QEvent * e)
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

void browserW::preloadPixmap()
{
  QString nonePix          = "none.png";
  QString syntaxOkPix      = "syntaxcheckdone.png";
  QString syntaxErrPix     = "syntaxcheckerror.png";
  QString syntaxChangedPix = "syntaxcheckmodified.png";
  QString typeOkPix        = "typecheckdone.png";
  QString typeErrPix       = "typecheckerror.png";
  QString cppcgOkPix       = "cplusplusdone.png";
  QString cppcgErrPix      = "cpluspluserror.png";
  QString javacgOkPix      = "javadone.png";
  QString javacgErrPix     = "javaerror.png";
  QString ppOkPix          = "prettyprintdone.png";
  QString ppErrPix         = "prettyprinterror.png";
  QString vdmcgOkPix       = "java2vdmdone.png";
  QString vdmcgErrPix      = "java2vdmerror.png";

  nonePixmap          = loadPixmap(nonePix);
  syntaxOkPixmap      = loadPixmap(syntaxOkPix);
  syntaxErrPixmap     = loadPixmap(syntaxErrPix); 
  syntaxChangedPixmap = loadPixmap(syntaxChangedPix);
  typeOkPixmap        = loadPixmap(typeOkPix);
  typeErrPixmap       = loadPixmap(typeErrPix);      
  cppcgOkPixmap       = loadPixmap(cppcgOkPix);      
  cppcgErrPixmap      = loadPixmap(cppcgErrPix);     
  javacgOkPixmap      = loadPixmap(javacgOkPix);     
  javacgErrPixmap     = loadPixmap(javacgErrPix);    
  ppOkPixmap          = loadPixmap(ppOkPix);         
  ppErrPixmap         = loadPixmap(ppErrPix);        
  vdmcgOkPixmap       = loadPixmap(vdmcgOkPix);      
  vdmcgErrPixmap      = loadPixmap(vdmcgErrPix);     
}

QWidget * browserW::createBrowserWidget( QWidget * parent )
{
  QTabWidget * widget = new QTabWidget( this );

#if QT_VERSION >= 0x040000
  widget->addTab( this->createBrowserTab( widget ), tr( "Project" ) );

#ifdef VDMPP
  widget->addTab( this->createClassWidget( widget ), tr( "Class" ) );
#endif //VDMPP
#ifdef VDMSL
  widget->addTab( this->createModuleWidget( widget ), tr( "Module" ) );
#endif //VDMSL
#else
  widget->insertTab( this->createBrowserTab( widget ), tr( "Project" ) );

#ifdef VDMPP
  widget->insertTab( this->createClassWidget( widget ), tr( "Class" ) );
#endif //VDMPP
#ifdef VDMSL
  widget->insertTab( this->createModuleWidget( widget ), tr( "Module" ) );
#endif //VDMSL
#endif // QT_VERSION >= 0x040000

  this->browserTW = widget;
  return widget;
}

QWidget * browserW::createBrowserTab( QWidget * parent )
{
  QWidget * tab = new QWidget( parent ); //tab, project tab
  QBoxLayout * layout = new QVBoxLayout( tab );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  layout->addWidget( this->createProjectListView( tab ) );
  return tab;
}

QTREEWIDGET * browserW::createProjectListView( QWidget * parent )
{
#if QT_VERSION >= 0x040000
  QStringList hlabels;
  hlabels.append(tr("Files"));

  QTreeWidget* listview = new QTreeWidget( parent );
  listview->setColumnCount (1);
  listview->setHeaderLabels(hlabels);
  listview->setRootIsDecorated(false);
  listview->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QObject::connect(listview, SIGNAL(itemSelectionChanged()),
                   this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
                   this, SLOT(viewProjectFile_qt4(QTreeWidgetItem*, int)));
#else
  QListView * listview = new QListView( parent, "projectLV" );
  listview->setSorting(0);
  listview->addColumn( tr( "Files" ) );
  listview->setSelectionMode( QListView::Extended );

  QObject::connect(listview, SIGNAL(selectionChanged()),
                   this,     SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(doubleClicked (QListViewItem *)),
                   this,     SLOT(viewProjectFile_qt3(QListViewItem*)));
#endif // QT_VERSION >= 0x040000

  this->projectLV = listview;

  return listview;
}

#ifdef VDMPP
QWidget * browserW::createClassWidget( QWidget * parent )
{
  QWidget * tab = new QWidget( parent );
  QBoxLayout * layout = new QVBoxLayout( tab );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addWidget( this->createClassTab( tab ) );
  return tab;
}

QWidget * browserW::createClassTab( QWidget * parent )
{
  QTabWidget * widget = new QTabWidget( parent );
#if QT_VERSION >= 0x040000
  widget->addTab( this->createVDMViewTab( widget ), tr( "VDM View" ) );
  widget->addTab( this->createJavaViewTab( widget ), tr( "Java View" ) );
#else
  widget->insertTab( this->createVDMViewTab( widget ), tr( "VDM View" ) );
  widget->insertTab( this->createJavaViewTab( widget ), tr( "Java View" ) );
#endif // QT_VERSION >= 0x040000
  this->classTW = widget;
  return widget;
}

QWidget * browserW::createVDMViewTab( QWidget * parent )
{
  QWidget * widget = new QWidget( parent ); //tab3 class tab
  QBoxLayout * layout = new QVBoxLayout( widget );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addWidget( this->createClassListView( widget ) );
  return widget;
}

QWidget * browserW::createJavaViewTab( QWidget * parent )
{
  QWidget * widget = new QWidget( parent );
  QBoxLayout * layout = new QVBoxLayout( widget );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addWidget( this->createJavaListView( widget ) );
  return widget;
}

QTREEWIDGET * browserW::createClassListView( QWidget * parent )
{
#if QT_VERSION >= 0x040000
  QStringList hlabels;
  hlabels.append(tr("Classes"));
  hlabels.append(tr("Syntax"));
  hlabels.append(tr("Type"));
  hlabels.append(tr("C++"));
  hlabels.append(tr("Java"));
  hlabels.append(tr("Pretty Print"));

  QTreeWidget* listview = new QTreeWidget( parent );
  listview->setColumnCount (6);
  listview->setHeaderLabels(hlabels);

  QHeaderView * header = listview->header();
  header->setMovable(false);
  header->setStretchLastSection(false);
  header->setDefaultSectionSize(50);
  header->setResizeMode(0, QHeaderView::Interactive);
  header->setResizeMode(1, QHeaderView::Fixed);
  header->setResizeMode(2, QHeaderView::Fixed);
  header->setResizeMode(3, QHeaderView::Fixed);
  header->setResizeMode(4, QHeaderView::Fixed);
  header->setResizeMode(5, QHeaderView::Fixed);

  listview->setColumnWidth(0, 100);
  listview->setColumnWidth(1, 50);
  listview->setColumnWidth(2, 50);
  listview->setColumnWidth(3, 50);
  listview->setColumnWidth(4, 50);
  listview->setColumnWidth(5, 50);
  listview->setRootIsDecorated(false);
  listview->setSelectionMode(QAbstractItemView::ExtendedSelection);
  listview->setIconSize(QSize(32, 32));

  QObject::connect(listview, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
                   this, SLOT(viewModuleFile_qt4(QTreeWidgetItem*, int)));
#else
  QListView * listview = new QListView( parent, "classLV" );
  listview->setSorting(0);
  int cn1 = listview->addColumn( tr("Classes") );
  int cn2 = listview->addColumn( tr("Syntax") );
  int cn3 = listview->addColumn( tr("Type") );
  int cn4 = listview->addColumn( tr("C++") );
  int cn5 = listview->addColumn( tr("Java") );
  int cn6 = listview->addColumn( tr("Pretty Print") );
  listview->setColumnWidth(cn1, 100);
  listview->setColumnWidth(cn2, 50);
  listview->setColumnWidth(cn3, 50);
  listview->setColumnWidth(cn4, 50);
  listview->setColumnWidth(cn5, 50);
  listview->setColumnWidth(cn6, 50);
  listview->setSelectionMode( QListView::Extended );

  QObject::connect(listview, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(doubleClicked ( QListViewItem * ) ),
                   this, SLOT(viewModuleFile_qt3(QListViewItem*)));
#endif // QT_VERSION >= 0x040000

  this->classLV = listview;
  return listview;
}

QTREEWIDGET * browserW::createJavaListView( QWidget * parent )
{
#if QT_VERSION >= 0x040000
  QStringList hlabels;
  hlabels.append(tr("Classes"));
  hlabels.append(tr("Syntax"));
  hlabels.append(tr("Type"));
  hlabels.append(tr("VDM"));

  QTreeWidget* listview = new QTreeWidget( parent );
  listview->setColumnCount (4);
  listview->setHeaderLabels(hlabels);

  QHeaderView * header = listview->header();
  header->setMovable(false);
  header->setStretchLastSection(false);
  header->setDefaultSectionSize(50);
  header->setResizeMode(0, QHeaderView::Interactive);
  header->setResizeMode(1, QHeaderView::Fixed);
  header->setResizeMode(2, QHeaderView::Fixed);
  header->setResizeMode(3, QHeaderView::Fixed);

  listview->setColumnWidth(0, 100);
  listview->setColumnWidth(1, 50);
  listview->setColumnWidth(2, 50);
  listview->setColumnWidth(3, 50);
  listview->setRootIsDecorated(false);
  listview->setSelectionMode(QAbstractItemView::ExtendedSelection);
  listview->setIconSize(QSize(32, 32));

  QObject::connect(listview, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
                   this, SLOT(viewModuleFile_qt4(QTreeWidgetItem*, int)));
#else
  QListView * listview = new QListView( parent, "javaLV" );  //tab_4 java tab
  listview->setSorting(0);
  int cn1 = listview->addColumn( tr( "Classes" ) );
  int cn2 = listview->addColumn( tr( "Syntax" ) );
  int cn3 = listview->addColumn( tr( "Type" ) );
  int cn4 = listview->addColumn( tr( "VDM" ) );
  listview->setColumnWidth(cn1, 100);
  listview->setColumnWidth(cn2, 50);
  listview->setColumnWidth(cn3, 50);
  listview->setColumnWidth(cn4, 50);
  listview->setSelectionMode( QListView::Extended );

  QObject::connect(listview, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(doubleClicked ( QListViewItem * ) ),
                   this, SLOT(viewModuleFile_qt3(QListViewItem*)));
#endif // QT_VERSION >= 0x040000

  this->javaLV = listview;
  return listview;
}
#endif //VDMPP

#ifdef VDMSL
QWidget * browserW::createModuleWidget( QWidget * parent )
{
  QWidget * widget = new QWidget( parent );
  QBoxLayout * layout = new QVBoxLayout( widget );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
  layout->addWidget( this->createModuleListView( widget ) );
  return widget;
}

QTREEWIDGET* browserW::createModuleListView( QWidget * parent )
{
#if QT_VERSION >= 0x040000
  QStringList hlabels;
  hlabels.append(tr("Modules"));
  hlabels.append(tr("Syntax"));
  hlabels.append(tr("Type"));
  hlabels.append(tr("C++"));
  hlabels.append(tr("Pretty Print"));

  QTreeWidget* listview = new QTreeWidget( parent );
  listview->setColumnCount (5);
  listview->setHeaderLabels(hlabels);

  QHeaderView * header = listview->header();
  header->setMovable(false);
  header->setStretchLastSection(false);
  header->setDefaultSectionSize(50);
  header->setResizeMode(0, QHeaderView::Interactive);
  header->setResizeMode(1, QHeaderView::Fixed);
  header->setResizeMode(2, QHeaderView::Fixed);
  header->setResizeMode(3, QHeaderView::Fixed);
  header->setResizeMode(4, QHeaderView::Fixed);

  listview->setColumnWidth(0, 100);
  listview->setColumnWidth(1, 50);
  listview->setColumnWidth(2, 50);
  listview->setColumnWidth(3, 50);
  listview->setColumnWidth(4, 50);
  listview->setRootIsDecorated(false);
  listview->setSelectionMode(QAbstractItemView::ExtendedSelection);
  listview->setIconSize(QSize(32, 32));

  QObject::connect(listview, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
                   this, SLOT(viewModuleFile_qt4(QTreeWidgetItem*, int)));
#else
  QListView * listview = new QListView( parent, "classLV" );
  int cn1 = listview->addColumn( tr( "Modules" ) );
  int cn2 = listview->addColumn( tr( "Syntax" ) );
  int cn3 = listview->addColumn( tr( "Type" ) );
  int cn4 = listview->addColumn( tr( "C++" ) );
  int cn5 = listview->addColumn( tr( "Pretty Print" ) );
  listview->setSelectionMode( QListView::Extended );
  listview->setColumnWidth(cn1, 100);
  listview->setColumnWidth(cn2, 50);
  listview->setColumnWidth(cn3, 50);
  listview->setColumnWidth(cn4, 50);
  listview->setColumnWidth(cn5, 50);

  QObject::connect(listview, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  QObject::connect(listview, SIGNAL(doubleClicked ( QListViewItem * ) ),
                   this, SLOT(viewModuleFile_qt3(QListViewItem*)));
#endif // QT_VERSION >= 0x040000

  this->classLV = listview;
  return listview;
}
#endif //VDMSL

//
// browserW::getSelectedFiles
//
// This function computes the list of names of those files which are
// currently selected. It does this irrespective of whether the
// project browser or class browser is active.
//SelectionType browserW::getSelectedFiles(QStringList& selected, bool abs)
QStringList browserW::getSelectedFiles(bool abs)
{ 
  if (this->isProjectManagerSelected())
  {
    return this->getSelectedFilesOfProject(abs);
  }
#ifdef VDMPP
  else if (this->isJavaClassBrowserSelected())
  {
    return this->getSelectedFilesOfJava(abs);
  }
#endif //VDMPP
  else
  {
    return this->getSelectedFilesOfVDM(abs);
  }
} 

QStringList browserW::getSelectedFilesOfProject(bool abs)
{
  QStringList list;

#if QT_VERSION >= 0x040000
  QTreeWidgetItem * root = this->projectLV->topLevelItem(0);
  int tcount = root->childCount();
  for (int tindex = 0; tindex < tcount; tindex++)
  {
    QTreeWidgetItem * vdmjava = root->child(tindex);
    int pcount = vdmjava->childCount();
    if (pcount > 0)
    {
      for (int pindex = 0; pindex < pcount; pindex++)
      {
        QTreeWidgetItem * package = vdmjava->child(pindex);
        bool package_selected = (package->isSelected());
        int fcount = package->childCount();
        for (int findex = 0; findex < fcount; findex++)
        {
          QTreeWidgetItem * file = package->child(findex);
          if (package_selected || file->isSelected())
          {
            list.append(file->text(1));
          }
        }
      }
    }
  }
#else
  QListViewItemIterator itL (this->projectLV);
  for (;itL.current();++itL)
  {
    if (itL.current()->isSelected())
    { 
      // 1 is the hidden column where full path name resides for file items 
      const QString selname = itL.current()->text(1);
      if (!selname.isEmpty()) {
        // This is a proper file name
        list += itL.current()->text(1); 
      }
      else if (itL.current()->childCount() == 0) {
        // A module has been selected
        QListViewItem *parent = itL.current()->parent();
        // ASSERT: parent->text(1) is the filename
        if (abs)
          list += parent->text(8);
        else
          list += parent->text(1);
      }
      else {
        QStringList filesInPackage = getFilesInPackage(itL.current());
        list += filesInPackage;
      }
    }  
  }
#endif // QT_VERSION >= 0x040000
  return list;
}

QStringList browserW::getSelectedFilesOfVDM(bool abs)
{
  QStringList list;
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> items (this->classLV->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = items.begin(); itr != items.end(); itr++)
  {
    if (list.indexOf((*itr)->text(8)) == -1)
      list.append((*itr)->text(8));
  } 
#else
  QListViewItemIterator itL(this->classLV);
  for (;itL.current();++itL)
  {
    if (itL.current()->isSelected())
    { 
      if (list.find(itL.current()->text(8)) == list.end())
      {
        list += itL.current()->text(8); 
        // 8 is the hidden column where full path name resides
      }
    }  
  }
#endif // QT_VERSION >= 0x040000
  return list;
}

#ifdef VDMPP
QStringList browserW::getSelectedFilesOfJava(bool abs)
{
  QStringList list;
  return list;
}
#endif // VDMPP

QStringList browserW::getFilesInPackage(QTREEWIDGETITEM* node) 
{
  QStringList result;
  const QString fileName (node->text(1));
  if (!fileName.isEmpty())
    result += fileName;
  else {
#if QT_VERSION >= 0x040000
    int count = node->childCount();
    for (int index = 0; index < count; index++)
    {
// 20120522 -->
      //result += getFilesInPackage(node->child(index));
      QStringList list (getFilesInPackage(node->child(index)));
      for (QStringList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
        if (!result.contains(*itr))
          result.append(*itr);
// <-- 20120522
    }
#else
    QListViewItem *child = node->firstChild();
    while (child) {
      result += getFilesInPackage(child);
      child = child->nextSibling();
    }
#endif // QT_VERSION >= 0x040000
  }
  return result;
}

QStringList browserW::getAllFilesOfProject(bool vdm)
{
  QStringList list;

#if QT_VERSION >= 0x040000
  QTreeWidgetItem * root = this->projectLV->topLevelItem(0);
  int tcount = root->childCount();
  for (int tindex = 0; tindex < tcount; tindex++)
  {
    QTreeWidgetItem * vdmjava = root->child(tindex);
    int pcount = vdmjava->childCount();
    if (pcount > 0)
    {
      for (int pindex = 0; pindex < pcount; pindex++)
      {
        QTreeWidgetItem * package = vdmjava->child(pindex);
        int fcount = package->childCount();
        for (int findex = 0; findex < fcount; findex++)
        {
          QTreeWidgetItem * file = package->child(findex);
          QString fnm (file->text(1));
          if ((vdm && !Qt2TB::isJavaFile(fnm)) || (!vdm && Qt2TB::isJavaFile(fnm)))
            if (!list.contains(fnm))
              list.append(fnm);
        }
      }
    }
  }
#else
  QListViewItemIterator itL (this->projectLV);
  for (;itL.current();++itL)
  {
    // 1 is the hidden column where full path name resides for file items 
    const QString name = itL.current()->text(1);
    if (!name.isEmpty()) {
      // This is a proper file name
      if ((vdm && !Qt2TB::isJavaFile(name)) || (!vdm && Qt2TB::isJavaFile(name)))
        if (!list.contains(name))
          list.append(name);
    }
    else if (itL.current()->childCount() == 0) {
      // A module has been selected
      QListViewItem *parent = itL.current()->parent();
      const QString name = parent->text(1);
      // ASSERT: parent->text(1) is the filename
//      if (abs)
//        list += parent->text(8);
//      else
      if (!name.isEmpty())
        if ((vdm && !Qt2TB::isJavaFile(name)) || (!vdm && Qt2TB::isJavaFile(name)))
          if (!list.contains(name))
            list.append(name);
    }
    else {
      QStringList filesInPackage = getFilesInPackage(itL.current());
      for (QStringList::const_iterator itr = filesInPackage.begin(); itr != filesInPackage.end(); ++itr)
      {
        QString fnm (*itr);
        if ((vdm && !Qt2TB::isJavaFile(fnm)) || (!vdm && Qt2TB::isJavaFile(fnm)))
          if (!list.contains(fnm))
            list.append(fnm);
      }
    }  
  }
#endif // QT_VERSION >= 0x040000
  return list;
}
void browserW::updateSelectedModules()
{
#ifdef VDMPP
  if (this->isJavaClassBrowserSelected())
  {
    this->updateSelectedModulesOfJava();
  }
  else
#endif // VDMPP
    this->updateSelectedModulesOfVDM();
}

void browserW::updateSelectedModulesOfVDM()
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> items (this->classLV->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = items.begin(); itr != items.end(); itr++)
  {
    const QString moduleName ((*itr)->text(0));
    StatusType st (Qt2TB::getStatusI(moduleName));
    this->setModuleStatus(*itr, st, true);
  } 
#else
  QListViewItemIterator itL (this->classLV);
  for (;itL.current();++itL)
  {
    if (itL.current()->isSelected())
    {
      QListViewItem * lvi = itL.current();
      const QString moduleName (lvi->text(0));
      StatusType st (Qt2TB::getStatusI(moduleName));
      this->setModuleStatus(lvi, st, true); }
  }
#endif // QT_VERSION >= 0x040000
}

#ifdef VDMPP
void browserW::updateSelectedModulesOfJava()
{
#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> items (this->javaLV->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = items.begin(); itr != items.end(); itr++)
  {
    const QString moduleName (Qt2TB::addJavaPrefix((*itr)->text(0)));
    StatusType st (Qt2TB::getStatusI(moduleName));
    this->setModuleStatus(*itr, st, true);
  } 
#else
  QListViewItemIterator itL(this->javaLV);
  for (;itL.current();++itL)
  {
    if (itL.current()->isSelected())
    {
      QListViewItem * lvi = itL.current();
      const QString moduleName (Qt2TB::addJavaPrefix(lvi->text(0)));
      StatusType st (Qt2TB::getStatusI(moduleName));
      this->setModuleStatus(lvi, st, false);
    }
  }
#endif // QT_VERSION >= 0x040000
}
#endif // VDMPP

QTREEWIDGETITEMLIST browserW::getSelectedNodes(const QTREEWIDGETITEM * root)
{
  if (root == NULL)
    return QTREEWIDGETITEMLIST();

#if QT_VERSION >= 0x040000
  QList<QTreeWidgetItem *> nodes;
  int count = root->childCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = root->child(index);
    if (item->isSelected())
    {
      nodes.append(item);
    } 
    else
    {
      if (item->childCount() > 0)
      {
        nodes += getSelectedNodes(item);
      }
    }
  }
  return nodes;
#else
  QPtrList<QListViewItem> nodes;
  QListViewItem* currentNode = (QListViewItem*)root;
  while (currentNode != NULL)
  {
    if (currentNode->isSelected()) 
    {
      nodes.append(currentNode);
    }
    else if (currentNode->childCount() != 0)
    {
      QPtrList<QListViewItem> subnodes (getSelectedNodes(currentNode->firstChild()));
      if (!subnodes.isEmpty())
      {
        for (int idx = 0; idx < (int)subnodes.count(); idx++)
          nodes.append(subnodes.at(idx));
      }
    }
    currentNode = currentNode->nextSibling();
  }
  return nodes;
#endif // QT_VERSION >= 0x040000
}

QStringList browserW::getSelectedModules()
{ 
  if (this->isProjectManagerSelected())
  {
    return Qt2TB::getModulesInFilesI(this->getSelectedModulesOfProject());
  }
#ifdef VDMPP
  else if (this->isJavaClassBrowserSelected())
  {
    return  this->getSelectedModulesOfJava();
  }
#endif // VDMPP
  else
  {
    return this->getSelectedModulesOfVDM();
  }
}

bool browserW::isJavaClassSelected()
{
  if (this->isProjectManagerSelected())
    return false;
#ifdef VDMPP
  else if (this->isJavaClassBrowserSelected())
    return true;
#endif // VDMPP
  else
    return false;
}

bool browserW::anyFilesOrModulesAreSelected()
{
  return !this->getSelectedModules().isEmpty();
}

QStringList browserW::getSelectedModulesOfProject()
{
#if QT_VERSION >= 0x040000
  QStringList list;
  QTreeWidgetItem * root = this->projectLV->topLevelItem(0);
  int count = root->childCount();
  for (int index = 0; index < count; index++)
  {
    QList<QTreeWidgetItem *> selectedNodes (getSelectedNodes(root->child(index)));
    if (!selectedNodes.isEmpty())
    {
      for (QList<QTreeWidgetItem *>::const_iterator itr = selectedNodes.begin();
           itr != selectedNodes.end(); ++itr)
      {
        QStringList thisNodesModules (getFilesInPackage(*itr));
        list += thisNodesModules;
      }
    }
  }
  return list;
#else
  QStringList list;
  QListViewItemIterator itL (this->projectLV);

  QPtrList<QListViewItem> selectedNodes (getSelectedNodes(itL.current()));
  if (!selectedNodes.isEmpty());
  {
    QListViewItem* thisNode;
    for (thisNode = selectedNodes.first(); thisNode != 0; thisNode = selectedNodes.next())
    {
      QStringList thisNodesModules (getFilesInPackage(thisNode));
      list += thisNodesModules;
    }
  }
  return list;
#endif // QT_VERSION >= 0x040000
}

QStringList browserW::getSelectedModulesOfVDM()
{
#if QT_VERSION >= 0x040000
  QStringList list;
  QList<QTreeWidgetItem *> items (this->classLV->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = items.begin(); itr != items.end(); itr++)
  {
    list.append((*itr)->text(0));
  } 
  return list;
#else
  QStringList list;
  QListViewItemIterator itL (this->classLV);
  for (;itL.current();++itL)
  {
    if (itL.current()->isSelected())
      list += itL.current()->text(0);
  }
  return list;
#endif // QT_VERSION >= 0x040000
}

#ifdef VDMPP
QStringList browserW::getSelectedModulesOfJava()
{
#if QT_VERSION >= 0x040000
  QStringList list;
  QList<QTreeWidgetItem *> items (this->javaLV->selectedItems());
  for (QList<QTreeWidgetItem *>::const_iterator itr = items.begin(); itr != items.end(); itr++)
  {
    list.append(Qt2TB::addJavaPrefix((*itr)->text(0)));
  } 
  return list;
#else
  QStringList list;
  QListViewItemIterator itL(this->javaLV);
  for (;itL.current();++itL)
  {
    if (itL.current()->isSelected())
      list += Qt2TB::addJavaPrefix(itL.current()->text(0));
  }
  return list;
#endif // QT_VERSION >= 0x040000
}
#endif // VDMPP

//
// browserW::clear
//
// Removes all files and modules from the project browser
//
// Parameters: none
// Returns: none
void browserW::clear()
{
  this->projectLV->clear();
  this->classLV->clear();
#ifdef VDMPP
  this->javaLV->clear();
#endif //VDMPP

#if QT_VERSION >= 0x040000
  // hack for update
  this->projectLV->hide();
  this->projectLV->show();
  this->classLV->hide();
  this->classLV->show();
#ifdef VDMPP
  this->javaLV->hide();
  this->javaLV->show();
#endif //VDMPP
#endif // QT_VERSION >= 0x040000

  this->createEmptyView();
}

//
// browserW::addModuleToFile
//
// Adds a module to its owning file in the file browser window;
// no action performed if the file can not be found
// If module is already there
// (this could be the case if a new class was added by UMLMapper),
// then update modules's file attribute.
//
// Parameters:
//   QString module: name of module to add
//   QString file: name of file owning module
// Returns: none
void browserW::addModuleToFile(const QString & module, const QString & file)
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * root = this->projectLV->topLevelItem(0);
  bool exists = false;
  int count = root->childCount();
  for (int index = 0; (index < count) && !exists; index++)
  {
    QTreeWidgetItem * vdmjava = root->child(index);
    int pcount = vdmjava->childCount();
    for (int pindex = 0; (pindex < pcount) && !exists; pindex++)
    {
      QTreeWidgetItem * package = vdmjava->child(pindex);
      int fcount = package->childCount();
      for (int findex = 0; (findex < fcount) && !exists; findex++)
      {
        QTreeWidgetItem * filename = package->child(findex);
        if (filename->text(1) == file)
        {
          int mcount = filename->childCount();
          for (int mindex = 0; (mindex < mcount) && !exists; mindex++)
          {
            QTreeWidgetItem * modnm = filename->child(mindex);
            exists = (modnm->text(0) == module); 
          }
          if (!exists)
          {
            QTreeWidgetItem * modnm = new QTreeWidgetItem();
            modnm->setFlags(Qt::ItemIsEnabled);
            modnm->setText(0, module);
            modnm->setText(1, file);
            filename->addChild(modnm);
            filename->sortChildren(0, Qt::AscendingOrder);
            exists = true;
          }
        }
      }
    }
  }
#else
  QListViewItemIterator itL (this->projectLV);
  bool found = false;
  for (;itL.current() && !found;++itL)
  {
    if (itL.current()->text(1) == file)
    { 
      found = true;
      QListViewItem* lvi = itL.current()->firstChild();
      while((lvi != NULL) && (lvi->text(0) != module)) {
        lvi = lvi->nextSibling();
      }
      if (lvi != NULL)
        lvi->setText(1, file);
      else
        lvi = new QListViewItem(itL.current(), module, file);
    }
  }
#endif // QT_VERSION >= 0x040000
}

//
// browserW::removeModuleFromFile
//
// Removes a module from its owning file in the file browser window;
// no action performed if the file can not be found
//
// Parameters:
//   QString module: name of module to remove
//   QString file: name of file owning module
// Returns: none
void browserW::removeModuleFromFile(const QString & module, const QString & file)
{
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * root = this->projectLV->topLevelItem(0);
  bool exists = false;
  int count = root->childCount();
  for (int index = 0; (index < count) && !exists; index++)
  {
    QTreeWidgetItem * vdmjava = root->child(index);
    int pcount = vdmjava->childCount();
    for (int pindex = 0; (pindex < pcount) && !exists; pindex++)
    {
      QTreeWidgetItem * package = vdmjava->child(pindex);
      int fcount = package->childCount();
      for (int findex = 0; (findex < fcount) && !exists; findex++)
      {
        QTreeWidgetItem * filename = package->child(findex);
        if (filename->text(1) == file)
        {
          int mcount = filename->childCount();
          for (int mindex = 0; (mindex < mcount) && !exists; mindex++)
          {
            if (filename->child(mindex)->text(0) == module)
            {
              QTreeWidgetItem * modnm = filename->takeChild(mindex);
              delete modnm;
              exists = true;
            }
          }
        }
      }
    }
  }
#else
  QListViewItemIterator itL(this->projectLV);

  bool found = false;
  for (;itL.current() && !found;++itL)
  {
    if (itL.current()->text(1) == file)
    { 
      QListViewItem* lvi = itL.current()->firstChild();
      while (lvi && !found)
      {
        if (module == lvi->text(0))
          found = true;
        else
          lvi = lvi->nextSibling();
      }
      if (lvi)
        delete lvi;
    }
  }
#endif // QT_VERSION >= 0x040000
}

//
// browserW::removeModule
//
// Removes a module from either the VDM module window or the java
// class window, depending on whether the module name begins with the
// string "java|" (FIXME: Define a function IsJavaModule to perform
// this test). No action is performed if the module is not found.
//
// Parameters
//   QString module: name of module to remove
//
// Returns: none
void browserW::removeModule(const QString & module)
{
  QTREEWIDGETITEM* moduleLvi = this->findModule(module);

  if (NULL != moduleLvi)
  {
    QString filename (moduleLvi->text(8));
    QString moduleName (module);
#ifdef VDMPP
    if (Qt2TB::isJavaModule(moduleName))
      moduleName = Qt2TB::removeJavaPrefix(moduleName);
#endif // VDMPP
    this->removeModuleFromFile(moduleName, filename);
    delete moduleLvi;
  }
}

void browserW::removeModules(const QStringList & modules)
{
  QStringList::const_iterator itr;
  for (itr = modules.begin(); itr != modules.end(); ++itr)
    this->removeModule(*itr);
}
//
// browserW::findModule
//
// This is an auxiliary function which given a module name, finds the
// QListViewItem corresponding to this module.
//
// Parameters:
//   const QString module: name of module to be found
// Returns:
//   QListViewItem* containing this module, or null if the module was
//   not found
QTREEWIDGETITEM* browserW::findModule(const QString & module)
{
  QString moduleName (module);

#if QT_VERSION >= 0x040000
#ifdef VDMPP
  if (Qt2TB::isJavaModule(module))
  {
    int count = this->javaLV->topLevelItemCount();
    for (int index = 0; index < count; index++)
    {
      QTreeWidgetItem * item = this->javaLV->topLevelItem(index);
      if (item->text(0) == moduleName)
        return item;
    }
    return NULL;
  }
#endif // VDMPP
  int count = this->classLV->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = this->classLV->topLevelItem(index);
    if (item->text(0) == moduleName)
      return item;
  }
  return NULL;
#else
  QListViewItem* moduleLvi = NULL;
#ifdef VDMPP
  if (Qt2TB::isJavaModule(module))
  {
    moduleLvi = this->javaLV->firstChild();
    moduleName = Qt2TB::removeJavaPrefix(moduleName);
  }
  else
#endif // VDMPP
    moduleLvi = this->classLV->firstChild();;

  while( moduleLvi && ( moduleLvi->text(0) != moduleName ) )
    moduleLvi = moduleLvi->nextSibling();

  return moduleLvi;
#endif // QT_VERSION >= 0x040000
}

//
// browserW::setModuleStatusPixmapp(QListViewItem* moduleLvi, 
//
void  browserW::setModuleStatusPixmap(QTREEWIDGETITEM* moduleLvi, int pos, StatusType::status st,
                                      const QPixmap & okPixmap, const QPixmap & errPixmap)
{
  if (moduleLvi == (QTREEWIDGETITEM*)NULL) return;

  switch (st) {
    case StatusType::status_ok: 
    case StatusType::status_pos:
    case StatusType::status_def: {
#if QT_VERSION >= 0x040000
      moduleLvi->setIcon(pos, QIcon(okPixmap));
#else
      moduleLvi->setPixmap(pos, okPixmap);
#endif // QT_VERSION >= 0x040000
      break;
    }
    case StatusType::status_error: {
#if QT_VERSION >= 0x040000
      moduleLvi->setIcon(pos, QIcon(errPixmap));
#else
      moduleLvi->setPixmap(pos, errPixmap);
#endif // QT_VERSION >= 0x040000
      break;
    }
    case StatusType::status_none:
    default: {
#if QT_VERSION >= 0x040000
      moduleLvi->setIcon(pos, QIcon(nonePixmap));
#else
      moduleLvi->setPixmap(pos, nonePixmap);
#endif // QT_VERSION >= 0x040000
      break;
    }
  }
}

//
// browserW::setModuleStatus(QListViewItem* moduleLvi, StatusType st,
//
void  browserW::setModuleStatus(QTREEWIDGETITEM* moduleLvi, const StatusType & st, bool isVDM)
{
  if (moduleLvi == (QTREEWIDGETITEM*)NULL) return;

  this->setModuleStatusPixmap(moduleLvi, 1, st.syntax, syntaxOkPixmap, syntaxErrPixmap);
  this->setModuleStatusPixmap(moduleLvi, 2, st.type, typeOkPixmap, typeErrPixmap);
#ifdef VDMSL
  this->setModuleStatusPixmap(moduleLvi, 3, st.cg, cppcgOkPixmap, cppcgErrPixmap);
  this->setModuleStatusPixmap(moduleLvi, 4, st.pp, ppOkPixmap, ppErrPixmap);
#endif // VDMSL
#ifdef VDMPP
  if( isVDM )
    this->setModuleStatusPixmap(moduleLvi, 3, st.cg, cppcgOkPixmap, cppcgErrPixmap);
  else 
    this->setModuleStatusPixmap(moduleLvi, 3, st.cg, vdmcgOkPixmap, vdmcgErrPixmap);
  this->setModuleStatusPixmap(moduleLvi, 4, st.jcg, javacgOkPixmap, javacgErrPixmap);
  this->setModuleStatusPixmap(moduleLvi, 5, st.pp, ppOkPixmap, ppErrPixmap);
#endif // VDMPP
}

//
// browserW::loadPixmap
//
// Auxiliary function FIXME: should be static
QPixmap browserW::loadPixmap(const QString & fname)
{
#ifdef USE_IMAGE_FILE
  QPixmap pixmap((this->imageDir) + DIRSEP + fname);
  if (pixmap.isNull())
    qDebug("Unable to read " + (this->imageDir) + DIRSEP + fname);
  return pixmap;
#else
  QPixmap img = getImageData( fname );
  if ( img.isNull() )
    emit logWrite(QString("Warning: unable to read image data: ") + fname);
  return img;
#endif // USE_IMAGE_FILE
}

//
// updateBrowser
//
// Updates the class & java  browser, by rereading all of the module
// information from the Toolbox repository
//
void browserW::updateBrowser(QTREEWIDGET* lv, bool isVDM)
{
  // Clear ListView
  lv->clear();

/*
  // Get module names
  QStringList moduleNames (Qt2TB::getModulesI());

  QStringList::const_iterator itr;
  for (itr = moduleNames.begin(); itr != moduleNames.end(); ++itr)
  {
#ifdef VDMPP
    if ( ( isVDM && Qt2TB::isJavaModule(*itr) ) ||
         ( !isVDM && !Qt2TB::isJavaModule(*itr) ) ) continue;
#endif //VDMPP

    QString moduleName (*itr);

#ifdef VDMPP
    if ( !isVDM )
      moduleName = Qt2TB::removeJavaPrefix(*itr);
#endif // VDMPP

    // Create module node
#if QT_VERSION >= 0x040000
    QTreeWidgetItem *moduleLvi = new QTreeWidgetItem(lv);
    moduleLvi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    moduleLvi->setText(0, moduleName);
#else
    QListViewItem *moduleLvi = new QListViewItem(lv, moduleName);
    moduleLvi->setSelectable(true);
    moduleLvi->setOpen(true);
#endif // QT_VERSION >= 0x040000
    
    // Set file name of module
    QStringList fileNames (Qt2TB::getFilesOfModule(*itr));
    // Note that for VDM++ there can only be one file per module
    QStringList::const_iterator fitr;
    for (fitr = fileNames.begin(); fitr != fileNames.end(); ++fitr)
      moduleLvi->setText(8, *fitr);

    // Get status of module
    StatusType st (Qt2TB::getStatusI(*itr));

    // Show module status
    this->setModuleStatus(moduleLvi, st, isVDM);
  }
*/
}
//
// updateClassBrowser
//
// Updates the class browser, by rereading all of the module
// information from the Toolbox repository
//
void browserW::updateClassBrowser()
{
  this->updateBrowser(this->classLV, true);
}

//
// updateJavaBrowser
//
// Updates the java browser
//
#ifdef VDMPP
//
// browserW::updateJavaBrowser()
//
void  browserW::updateJavaBrowser()
{
  this->updateBrowser(this->javaLV, false);
}
#endif //VDMPP

// This should really be in browserF.cc
//
// browserW::updateModuleStatus(const QString moduleName)
//
void  browserW::updateModuleStatus(const QString& moduleName, const StatusType & st)
{
#ifdef VDMSL
  bool isVDM = true;
#endif // VDMSL
#ifdef VDMPP
  bool isVDM = !Qt2TB::isJavaModule( moduleName );
#endif //VDMPP

  QTREEWIDGETITEM* moduleLvi = this->findModule( moduleName );
  if ( moduleLvi != NULL ) {
    // Show module status
    this->setModuleStatus( moduleLvi, st, isVDM );
  }
}

//
// updateProjectBrowser
//
// Updates the project browser, by rereading all of the module
// information from the Toolbox repository
//
// Takes as argument QListViewItem *prj - a reference to the parent
// list view item
//
// browserW::updateProjectBrowser()
//
void  browserW::updateProjectBrowser()
{
  this->createProjectListViewItem();

  this->refreshProjectName();

  // Set up packages in file browser
  QTREEWIDGETITEM * vdmf = this->findVDMParent();
  QStringList packageNames (Qt2TB::getVDMPackages());
  if (!packageNames.isEmpty())
    this->addPackages(vdmf, packageNames, false);

#ifdef VDMPP
  QTREEWIDGETITEM * javaf = this->findJavaParent();
  QStringList javaPackageNames (Qt2TB::getJavaPackages());
  if (!javaPackageNames.isEmpty())
    this->addPackages(javaf, javaPackageNames, true);
#endif //VDMPP

#if QT_VERSION >= 0x040000
  vdmf->setExpanded(true);
#ifdef VDMPP
  javaf->setExpanded(true);
#endif //VDMPP
#endif // QT_VERSION >= 0x040000
}

//
// browserW::addPackages(QListViewItem* lvi, QStringList packageNames, 
//
void  browserW::addPackages(QTREEWIDGETITEM* parent, const QStringList & packageNames, bool isJava)
{
  bool defaultUsed = false;
  QStringList::const_iterator iter;
  for (iter = packageNames.begin(); iter != packageNames.end(); ++iter)
  {
    QString packageName;
    if ((*iter).isEmpty() && defaultUsed)
      packageName = "/";
    else if ((*iter).isEmpty() && !defaultUsed) {
      packageName = "Default";
      defaultUsed = true;
    }
    else {
      packageName = *iter;
      defaultUsed = true;
    }

    // create package node
#if QT_VERSION >= 0x040000
    QTreeWidgetItem *pckg = new QTreeWidgetItem();
    pckg->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pckg->setText(0, packageName);
    parent->addChild(pckg);
    parent->sortChildren(0, Qt::AscendingOrder);
#else
    QListViewItem *pckg = new QListViewItem( parent, packageName );
    pckg->setSelectable(true);
    pckg->setOpen(true);
#endif // QT_VERSION >= 0x040000

    this->addFileNodes( pckg, *iter, isJava );

#if QT_VERSION >= 0x040000
    pckg->setExpanded(true);
#endif // QT_VERSION >= 0x040000
  }
  this->update();
}

void browserW::addFileNodes( QTREEWIDGETITEM* parent, const QString & packageName, bool isJava )
{
  QStringList fileNames;
  QStringList absFileNames;
#ifdef VDMPP
  if (isJava)
    Qt2TB::getModulesOfJavaPackage(packageName, fileNames, absFileNames);
  else
#endif //VDMPP
    Qt2TB::getModulesOfVDMPackage(packageName, fileNames, absFileNames);

  for (int i = 0; i < (int)fileNames.count(); i++)
  {
    const QString fileNm (fileNames[i]);
    const QString absFileNm (absFileNames[i]);

    // create file node
#if QT_VERSION >= 0x040000
    QTreeWidgetItem *fileLvi = new QTreeWidgetItem();
    fileLvi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    fileLvi->setText(0, fileNm);
    fileLvi->setText(1, absFileNm);
    parent->addChild(fileLvi);
    parent->sortChildren(0, Qt::AscendingOrder);
#else
    QListViewItem *fileLvi = new QListViewItem(parent, fileNm, absFileNm);
    fileLvi->setSelectable(true);
#endif // QT_VERSION >= 0x040000
 
    this->addModuleNodes( fileLvi, absFileNm, isJava );
  }
}

void browserW::addModuleNodes( QTREEWIDGETITEM* parent, const QString & fileName, bool isJava )
{
  // Insert modules for selected file here
  QStringList moduleNames (Qt2TB::getModulesI(fileName));
      
  QStringList::const_iterator itr;
  for (itr = moduleNames.begin(); itr != moduleNames.end(); ++itr)
  {
    QString moduleName;
#ifdef VDMPP
    if (isJava)
      moduleName = Qt2TB::removeJavaPrefix(*itr);
    else
#endif //VDMPP
      moduleName = *itr;

    // create module node
#if QT_VERSION >= 0x040000
    QTreeWidgetItem *moduleLvi = new QTreeWidgetItem();
    moduleLvi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    moduleLvi->setText(0, moduleName);
    parent->addChild(moduleLvi);
    parent->sortChildren(0, Qt::AscendingOrder);
#else
    QListViewItem *moduleLvi = new QListViewItem( parent, moduleName );
    moduleLvi->setSelectable(true);
#endif // QT_VERSION >= 0x040000
  }
}

void browserW::addModule(const QString & moduleName, const QStringList & files, const StatusType & st)
{
#ifdef VDMPP
  if (Qt2TB::isJavaModule((moduleName)))
    this->addJavaModule(moduleName, files, st);
  else
#endif //VDMPP
    this->addVDMModule(moduleName, files, st);
}

#ifdef VDMPP
//
// browserW::addJavaModule(QString moduleName)
//
void  browserW::addJavaModule(const QString & moduleName, const QStringList & files, const StatusType & st)
{
  // Note that for VDM there can only be one file per module

  QString name (Qt2TB::removeJavaPrefix(moduleName));
#if QT_VERSION >= 0x040000
  QTreeWidgetItem* moduleLvi = new QTreeWidgetItem();
  moduleLvi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  moduleLvi->setText(0, name);
  this->javaLV->addTopLevelItem(moduleLvi);
  this->javaLV->sortItems(0, Qt::AscendingOrder);
  this->javaLV->resizeColumnToContents(0);
#else
  QListViewItem* moduleLvi = new QListViewItem(this->javaLV, name);
#endif // QT_VERSION >= 0x040000

  for (QStringList::const_iterator itr = files.begin(); itr != files.end(); ++itr)
  {
    moduleLvi->setText(8, *itr);
    this->addModuleToFile(name, *itr);
  }
  this->setModuleStatus(moduleLvi, st, false);
}
#endif //VDMPP

//
// browserW::addVDMModule(QString name)
//
void browserW::addVDMModule(const QString & name, const QStringList & files, const StatusType & st)
{
  QTREEWIDGETITEM* moduleLvi = this->findModule(name);
  if( moduleLvi == NULL )
  {
    // Note that for VDM there can only be one file per module
#if QT_VERSION >= 0x040000
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    newItem->setText(0, name);
    this->classLV->addTopLevelItem(newItem);
    this->classLV->sortItems(0, Qt::AscendingOrder);
    this->classLV->resizeColumnToContents(0);
    moduleLvi = newItem;
#else
    moduleLvi = new QListViewItem(this->classLV, name);
    moduleLvi->setSelectable(true);
#endif // QT_VERSION >= 0x040000
    moduleLvi->setSelected(true);

    for (QStringList::const_iterator itr = files.begin(); itr != files.end(); ++itr) {
      moduleLvi->setText(8, *itr);
      this->addModuleToFile(name, *itr);
    }
  }
  this->setModuleStatus(moduleLvi, st, true);
}

void  browserW::checkModifiedModules()
{
  this->checkModifiedModuleItems(this->classLV);
#ifdef VDMPP
  this->checkModifiedModuleItems(this->javaLV);
#endif //VDMPP
}

//
// browserW::checkModifiedModuleItems(QListView* parent)
//
void  browserW::checkModifiedModuleItems(QTREEWIDGET* parent)
{
  bool isVDM = (parent == this->classLV);

  QStringList glist (getAllFilesOfProject(isVDM));

  QStringList list;

#if QT_VERSION >= 0x040000
  int count = parent->topLevelItemCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = parent->topLevelItem(index);
    QString fileName (item->text(8));
    if (((isVDM && !Qt2TB::isJavaFile(fileName)) || (!isVDM && Qt2TB::isJavaFile(fileName))))
    {
      if (glist.contains(fileName))
        glist.removeAll(fileName);
      if (Qt2TB::isFileModifiedI(fileName))
      {
        item->setIcon(1, QIcon(syntaxChangedPixmap));
        if (!list.contains(fileName))
          list.append(fileName);
      }
    }
  }
#else
  QListViewItem *lvi = parent->firstChild();
  while (lvi)
  {
    QString fileName (lvi->text(8));
    if((isVDM && !Qt2TB::isJavaFile(fileName)) || (!isVDM && Qt2TB::isJavaFile(fileName)))
    {
      if (glist.contains(fileName))
        glist.remove(fileName);
      if (Qt2TB::isFileModifiedI(fileName))
      {
        lvi->setPixmap(1, syntaxChangedPixmap);
        if(!list.contains(fileName))
          list.append(fileName);
      }
    }
    lvi = lvi->nextSibling();
  }
#endif // QT_VERSION >= 0x040000
  if(!list.isEmpty())
    emit modified(list);

  QStringList mlist;
  for (QStringList::const_iterator itr = glist.begin(); itr != glist.end(); ++itr)
  {
    QString fileName (*itr);
    if (Qt2TB::isFileModifiedI(fileName) && !mlist.contains(fileName))
      mlist.append(fileName);
  }
  if(!mlist.isEmpty())
    emit modified(mlist);
}

//
// browserW::refreshProjectName()
//
//void  browserW::refreshProjectName()
void  browserW::refreshProjectName()
{
  QFileInfo qfi (Qt2TB::getProjectNameI());
  QString projectName (qfi.fileName());

  QString projectLabel ("[" + tr("Project") + " - ");
  if (projectName.length() > 0)
    projectLabel += projectName;
  else
    projectLabel += "(No Name)";
  projectLabel += "]";

#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = this->projectLV->topLevelItem(0);
  item->setText( 0, projectLabel );
#else
  QListViewItem * prj = this->projectLV->firstChild();
  prj->setText( 0, projectLabel );
#endif // QT_VERSION >= 0x040000

  QString menuTitle (Qt2TB::GiveToolTitleI());
  if (projectName.length() > 0) 
    menuTitle += QString(" (") + projectName + ")";
  emit setMainWindowTitle(menuTitle);
}

//
// browserW::refreshFiles
//
// Refreshes the whole class/project browser window from scratch
//
void  browserW::refreshFiles()
{
  this->updateProjectBrowser();
  this->updateClassBrowser();
#ifdef VDMPP
  this->updateJavaBrowser();
#endif //VDMPP
} 

QString browserW::getFilePath(const QString & filename)
{ 
#if QT_VERSION >= 0x040000
  QTreeWidgetItem * root = this->projectLV->topLevelItem(0);
  int count = root->childCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = root->child(index);
    QString file (this->getFile( item, filename ));
    if( !file.isEmpty() ) return file;
  }
#else
  QListViewItemIterator itL(this->projectLV);
  for (;itL.current();++itL)
  {
    QString file (this->getFile( itL.current(), filename ));
    if( !file.isEmpty() ) return file;
  }
#endif // QT_VERSION >= 0x040000
  return QString();
}

QString browserW::getFile(QTREEWIDGETITEM* node, const QString & filename)
{
  QStringList result;
  const QString fileName (node->text(1));
  if (!fileName.isEmpty())
  {
    QFileInfo fi( fileName );
    if( fi.fileName() == filename ) return fileName;
  }
  else {
#if QT_VERSION >= 0x040000
    int count = node->childCount();
    for (int index = 0; index < count; index++)
    {
      QTreeWidgetItem * item = node->child(index);
      QString file (this->getFile( item, filename ));
      if( !file.isEmpty() ) return file;
    }
#else
    QListViewItem *child = node->firstChild();
    while (child) {
//      result += getFilesInPackage(child);
      QString file (this->getFile( child, filename ));
      if( !file.isEmpty() ) return file;
      child = child->nextSibling();
    }
#endif // QT_VERSION >= 0x040000
  }
  return QString();
}

//
// browserW::viewProjectFile(QListViewItem* lvi)
//
void browserW::viewProjectFile_qt4(QTreeWidgetItem* lvi, int)
{
#if QT_VERSION >= 0x040000
//  this->viewFile(lvi->text(0), lvi->text(1));
  this->viewFile(QString(""), lvi->text(1));
#endif // QT_VERSION >= 0x040000
}

#if QT_VERSION < 0x040000
void browserW::viewProjectFile_qt3(QListViewItem* lvi)
{
//  this->viewFile(lvi->text(0), lvi->text(1));
  this->viewFile(QString(""), lvi->text(1));
}
#endif // QT_VERSION < 0x040000

//
// browserW::viewModuleFile(QListViewItem* lvi)
//
void browserW::viewModuleFile_qt4(QTreeWidgetItem* lvi, int column)
{
#if QT_VERSION >= 0x040000
#ifdef VDMSL
  switch(column) {
    case 0: { this->viewFile(lvi->text(0), lvi->text(8)); break; } // Class or Module
    case 1: { emit br_syntax(false);    break; } // Syntax
    case 2: { emit br_typecheck(false); break; } // Type
    case 3: { emit br_cpp(false);       break; } // C++
    case 4: { emit br_pretty(false);    break; } // Pretty Print
    default: { break; }
  }
#endif // VDMSL
#ifdef VDMPP
  if (isVDMClassBrowserSelected())
  {
    switch(column) {
      case 0: { this->viewFile(lvi->text(0), lvi->text(8)); break; } // Class or Module
      case 1: { emit br_syntax(false);    break; } // Syntax
      case 2: { emit br_typecheck(false); break; } // Type
      case 3: { emit br_cpp(false);       break; } // C++
      case 4: { emit br_java(false);      break; } // Java
      case 5: { emit br_pretty(false);    break; } // Pretty Print
      default: { break; }
    }
  }
  else
  {
    switch(column) {
      case 0: { this->viewFile(lvi->text(0), lvi->text(8)); break; } // Class or Module
      case 1: { emit br_syntax(false);    break; } // Syntax
      case 2: { emit br_typecheck(false); break; } // Type
      case 3: { emit br_j2v(false);       break; } // J2V
      default: { break; }
    }
  }
#endif // VDMPP
#endif // QT_VERSION >= 0x040000
}

#if QT_VERSION < 0x040000
void browserW::viewModuleFile_qt3(QListViewItem* lvi)
{
  this->viewFile(lvi->text(0), lvi->text(8));
}
#endif // QT_VERSION < 0x040000

//
// browserW::viewFile(const QString & modnm const QString & apparentFile)
//
void browserW::viewFile(const QString & modnm, const QString & apparentFile)
{
  if (apparentFile != "")
  {
    int line = 1;
    int col = 1;
    int len = 1;
    if (!modnm.isEmpty())
    {
      Qt2TB::GetModulePosInfoI(modnm, &line, &col, &len);
    }
    QString filename;
    emit resetFilePosition( apparentFile, filename, line, col, len );
  }
}

void browserW::selectionChanged()
{
  emit checkActions();
}

void browserW::setImageDir( const QString & dir )
{
  this->imageDir = dir;
}

void browserW::selectAll()
{
  QTREEWIDGET * lv = NULL;
  if( this->isProjectManagerSelected() )
    lv = this->projectLV;
#ifdef VDMPP
  else if( this->isJavaClassBrowserSelected() )
    lv = this->javaLV;
#endif // VDMPP
  else
    lv = this->classLV;

  this->selectAllNodes(lv, true);
}

void browserW::selectAllNodes(QTREEWIDGET * lv, bool select)
{
#if QT_VERSION >= 0x040000
  if (lv != NULL)
  {
    QTreeWidgetItemIterator it (lv);
    while (*it) 
    {
      (*it)->setSelected(select);
      ++it;
    }
  }
#else
  if (lv != NULL)
  {
    QListViewItemIterator itr(lv);
    for (;itr.current();++itr)
    {
      itr.current()->setSelected( select );
      itr.current()->repaint();
    }
  }
#endif // QT_VERSION >= 0x040000
}

#ifdef VDMPP
//
// browserW::findJavaParent()
//
QTREEWIDGETITEM* browserW::findJavaParent()
{
  if (this->projectLV == NULL) return NULL;

#if QT_VERSION >= 0x040000
  return this->findLVIByLabel1( this->projectLV->topLevelItem(0), tr(HEADER_JAVA) );
#else
  return this->findLVIByLabel1( this->projectLV->firstChild(), tr(HEADER_JAVA) );
#endif // QT_VERSION >= 0x040000
}
#endif //VDMPP

//
// browserW::findVDMParent()
//
QTREEWIDGETITEM* browserW::findVDMParent()
{
  if (this->projectLV == NULL) return NULL;

#if QT_VERSION >= 0x040000
  return this->findLVIByLabel1( this->projectLV->topLevelItem(0), tr(HEADER_VDM) );
#else
  return this->findLVIByLabel1( this->projectLV->firstChild(), tr(HEADER_VDM) );
#endif // QT_VERSION >= 0x040000
}

//
// browserW::addFiles(QStringList& newFiles)
//
void browserW::addFiles(const QStringList & newFiles)
{
  // Setup package name
  QStringList vpl (Qt2TB::getVDMPackages());
#ifdef VDMPP
  QStringList jpl (Qt2TB::getJavaPackages());
#endif //VDMPP

  if (Qt2TB::packageStructureChanged())
  {
    QStringList::const_iterator itr;
    for (itr = newFiles.begin(); itr != newFiles.end(); ++itr)
      emit logWrite("Adding " + *itr + " to project");

    this->updateProjectBrowser();
  }
  else
  {
    QTREEWIDGETITEM *vdmParent = this->findVDMParent();
#ifdef VDMPP
    QTREEWIDGETITEM *javaParent = this->findJavaParent();
#endif //VDMPP

    QStringList::const_iterator itr;
    for (itr = newFiles.begin(); itr != newFiles.end(); ++itr)
    {
      emit logWrite("Adding " + *itr + " to project");

#ifdef VDMSL
      this->addFile( vdmParent, *itr );
#endif // VDMSL

#ifdef VDMPP
      if (Qt2TB::isJavaFile(*itr))
        this->addFile( javaParent, *itr );
      else
        this->addFile( vdmParent, *itr );
#endif //VDMPP
    }
  }
}

void browserW::addFile(QTREEWIDGETITEM* parent, const QString & fileName)
{
  if ( parent == NULL ) return;

  // search package node
  QString packageName ( this->getPackageName(fileName) );
  QTREEWIDGETITEM* packageItem = this->findLVIByLabel1(parent, packageName);
 
  // create package node
#if QT_VERSION >= 0x040000
  if ( packageItem == NULL ) // New package
  {
    packageItem = new QTreeWidgetItem();
    packageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    packageItem->setText(0, packageName);
    parent->addChild(packageItem);
    parent->sortChildren(0, Qt::AscendingOrder);
  }
#else
  if ( packageItem == NULL ) // New package
    packageItem = new QListViewItem(parent, packageName);
  packageItem->setOpen(true);
#endif // QT_VERSION >= 0x040000

  // search file node
  QString thisItemName (QFileInfo(fileName).fileName());
  QTREEWIDGETITEM* packageMember = this->findLVIByLabel1(packageItem, thisItemName);
  // create file node
  if( packageMember == NULL )
  {
#if QT_VERSION >= 0x040000
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    newItem->setText(0, thisItemName);
    newItem->setText(1, fileName);
    packageItem->addChild(newItem);
    packageItem->sortChildren(0, Qt::AscendingOrder);
#else
    QListViewItem * newItem = new QListViewItem(packageItem, thisItemName, fileName);
    newItem->setSelectable(true);
#endif // QT_VERSION >= 0x040000
// 20120629 -->
    newItem->setSelected(true);
// <-- 20120629
  }
#if QT_VERSION >= 0x040000
  parent->setExpanded(true);
  packageItem->setExpanded(true);
#endif // QT_VERSION >= 0x040000
}

//
// browserW::removeFiles(QStringList& removedFiles)
//
void browserW::removeFiles(const QStringList & removedFiles)
{
  // Setup package name
  QStringList vpl (Qt2TB::getVDMPackages());
#ifdef VDMPP
  QStringList jpl (Qt2TB::getJavaPackages());
#endif //VDMPP

  if (Qt2TB::packageStructureChanged())
  {
    QStringList::const_iterator itr;
    for (itr = removedFiles.begin(); itr != removedFiles.end(); ++itr)
      emit logWrite("Removing " + *itr + " from project");

    this->updateProjectBrowser();
  }
  else
  {
#ifdef VDMPP
    QTREEWIDGETITEM *javaParent = this->findJavaParent();
#endif //VDMPP
    QTREEWIDGETITEM *vdmParent = this->findVDMParent();

    QStringList::const_iterator itr;
    for (itr = removedFiles.begin(); itr != removedFiles.end(); ++itr)
    {
      emit logWrite("Removing " + *itr + " from project");

#ifdef VDMSL
      this->removeFile( vdmParent, *itr );
#endif // VDMSL
#ifdef VDMPP
      if (Qt2TB::isJavaFile(*itr))
        this->removeFile( javaParent, *itr );
      else
        this->removeFile( vdmParent, *itr );
#endif //VDMPP
    }
  }
}

void browserW::removeFile(QTREEWIDGETITEM* parent, const QString& fileName)
{
  if ( parent == NULL ) return;

  // search package node
  QString packageName (this->getPackageName(fileName));
  QTREEWIDGETITEM* packageItem = this->findLVIByLabel1(parent, packageName);

  // Package will not be found if we have selected new project
  // and are therefore removing all files
  if ( packageItem != NULL )
  {
    QTREEWIDGETITEM* packageMember = this->findLVIByLabel2(packageItem, fileName);
    if ( packageMember != NULL )
    {
#if QT_VERSION >= 0x040000
      int index = packageItem->indexOfChild(packageMember);
      packageItem->takeChild(index);
#else
      packageItem->takeItem( packageMember );
#endif // QT_VERSION >= 0x040000
      delete packageMember;

      // if no file in package delete package
      if( this->countChilds( packageItem ) == 0 )
      {
#if QT_VERSION >= 0x040000
        int index = parent->indexOfChild(packageItem);
        parent->takeChild(index);
#else
        parent->takeItem( packageItem );
#endif // QT_VERSION >= 0x040000
        delete packageItem;
      }
    }
    else
      emit logWrite("Internal Error: Unable to find file " + fileName);
  }
}

void browserW::newProject()
{
  // FIXME: This is a hack due to a problem at the specification level;
  // this problem means that the call back to removeFiles after a
  // loadProject does not find the correct package for the file to
  // be removed

  this->createEmptyView();
  this->update();
  // End of hack
}

void browserW::createEmptyView()
{
  this->createProjectListViewItem();

#if QT_VERSION >= 0x040000
  this->browserTW->setCurrentIndex(0);
#endif // QT_VERSION >= 0x040000
}

void browserW::createProjectListViewItem()
{
  this->projectLV->clear();
  QString projectLabel ("[" + tr("Project") + " - (No Name)]");

#if QT_VERSION >= 0x040000
  QTreeWidgetItem * item = new QTreeWidgetItem(this->projectLV);
  item->setText(0, projectLabel);
  item->setFlags(Qt::ItemIsEnabled);

#ifdef VDMSL
  QTreeWidgetItem * vdmi = new QTreeWidgetItem();
  vdmi->setFlags(Qt::ItemIsEnabled);
  vdmi->setText( 0, tr( HEADER_VDM ) );
  item->insertChild(0, vdmi);
#endif // VDMSL
#ifdef VDMPP
  QTreeWidgetItem * javai = new QTreeWidgetItem();
  javai->setFlags(Qt::ItemIsEnabled);
  javai->setText( 0, tr( HEADER_JAVA ) );
  item->insertChild(0, javai);
  QTreeWidgetItem * vdmi = new QTreeWidgetItem();
  vdmi->setFlags(Qt::ItemIsEnabled);
  vdmi->setText( 0, tr( HEADER_VDM ) );
  item->insertChild(1, vdmi);
#endif // VDMPP

  this->projectLV->expandItem(item);
#else
  QListViewItem *prj = new QListViewItem( this->projectLV );
  prj->setOpen(true);
  prj->setSelectable(false);
  prj->setText( 0, projectLabel );
  prj->setExpandable(true);
  QListViewItem * vdmf = new QListViewItem( prj, NULL );
  vdmf->setOpen( true );
  vdmf->setSelectable(false);
  vdmf->setText( 0, tr( HEADER_VDM ) );

#ifdef VDMPP
  QListViewItem * javaf = new QListViewItem( prj, vdmf );
  javaf->setOpen( true );
  javaf->setSelectable(false);
  javaf->setText( 0, tr( HEADER_JAVA ) );
#endif // VDMPP
#endif // QT_VERSION >= 0x040000
}

QString browserW::getPackageName( const QString & fileName )
{
  QString packageName ( Qt2TB::getPackageOfFileI( fileName ) );
  if (packageName.isEmpty())
    packageName = "Default";
  return packageName;
}

QTREEWIDGETITEM * browserW::findLVIByLabel1(QTREEWIDGETITEM * lvi, const QString & label1)
{
  if( lvi == NULL ) return NULL;

#if QT_VERSION >= 0x040000
  int count = lvi->childCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = lvi->child(index);
    if (item->text(0) == label1)
      return item;
  }
  return NULL;
#else
  QListViewItem * reslvi = lvi->firstChild();
  while( reslvi && ( reslvi->text(0) != label1 ) )
    reslvi = reslvi->nextSibling();
  return reslvi; 
#endif // QT_VERSION >= 0x040000
}

QTREEWIDGETITEM * browserW::findLVIByLabel2(QTREEWIDGETITEM * lvi, const QString & label2)
{
  if( lvi == NULL ) return NULL;

#if QT_VERSION >= 0x040000
  int count = lvi->childCount();
  for (int index = 0; index < count; index++)
  {
    QTreeWidgetItem * item = lvi->child(index);
    if (item->text(1) == label2)
      return item;
  }
  return NULL;
#else
  QListViewItem * reslvi = lvi->firstChild();
  while( reslvi && ( reslvi->text(1) != label2 ) )
    reslvi = reslvi->nextSibling();
  return reslvi; 
#endif // QT_VERSION >= 0x040000
}

int browserW::countChilds( QTREEWIDGETITEM * lvi )
{
  if( lvi == NULL ) return 0;
#if QT_VERSION >= 0x040000
  return lvi->childCount();
#else
  int count = 0;
  QListViewItem * reslvi = lvi->firstChild();
  while( reslvi )
  {
    reslvi = reslvi->nextSibling();
    count++;
  }
  return count;
#endif // QT_VERSION >= 0x040000
}

bool browserW::isProjectManagerSelected()
{
#if QT_VERSION >= 0x040000
  return (this->browserTW->currentIndex() == 0);
#else
  return (this->browserTW->currentPageIndex() == 0);
#endif // QT_VERSION >= 0x040000
}

bool browserW::isVDMClassBrowserSelected()
{
#if QT_VERSION >= 0x040000
  return (this->browserTW->currentIndex() == 1) &&
         (this->classTW->currentIndex() == 0);
#else
  return (this->browserTW->currentPageIndex() == 1) &&
         (this->classTW->currentPageIndex() == 0);
#endif // QT_VERSION >= 0x040000
}

#ifdef VDMPP
bool browserW::isJavaClassBrowserSelected()
{
#if QT_VERSION >= 0x040000
  return (this->browserTW->currentIndex() == 1) &&
         (this->classTW->currentIndex() == 1);
#else
  return (this->browserTW->currentPageIndex() == 1) &&
         (this->classTW->currentPageIndex() == 1);
#endif // QT_VERSION >= 0x040000
}
#endif // VDMPP

void browserW::setBusy(bool busy)
{
}

#if QT_VERSION >= 0x040000
void browserW::contextMenuEvent ( QContextMenuEvent * event )
{
  QMenu * menu = new QMenu(this);
  menu->addSeparator();
  menu->addAction(tr("Select All"));
  menu->addSeparator();
  menu->addAction(tr("Type Check"));
  menu->addAction(tr("Integrity Check"));
  menu->addAction(tr("Pretty Print"));
  menu->addSeparator();
  menu->addAction(tr("Project Option"));
  QObject::connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(contextMenuTriggered(QAction *)));

  menu->exec(event->globalPos());
  delete menu;
}
#endif // QT_VERSION >= 0x040000

void browserW::contextMenuTriggered(QAction * a)
{
#if QT_VERSION >= 0x040000
  if (a->text() == tr("Type Check"))
    emit br_typecheck(false);
  else if (a->text() == tr("Integrity Check"))
    emit br_integrity(false);
  else if (a->text() == tr("Pretty Print"))
    emit br_pretty(false);
  else if (a->text() == tr("Project Option"))
    emit br_option(false);
  else if (a->text() == tr("Select All"))
    emit br_selectAll(false);
#endif // QT_VERSION >= 0x040000
}

