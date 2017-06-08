/***
*  * WHAT
*  *    This class is used to allow the user to select which classes
*  *    should be generated as java interfaces by the java code generator
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/interfacesF.cc,v $
*  * VERSION
*  *    $Revision: 1.8 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:42 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "interfacesF.h"
#include "interface2TB.h"
#include "optionsF.h"
#include "mainF.h"

#include <iostream>
//using namespace std;

// This class essentially implements a mini document-view pattern; the
// document is represented by the instance variables
// possibleInterfaces and selectedInterfaces. Both of these are
// QStringList pointers; the view for them is respectively possibleLB
// and selectedLB - QListBox objects. 

interfacesW::interfacesW( QWidget* parent,  const char* name, bool modal, WFlags fl )
#if QT_VERSION >= 0x040000
    : QDialog( parent, fl )
#else
    : QDialog( parent, name, modal, fl )
#endif // QT_VERSION >= 0x040000
{
  this->optionsw = (optionsW*)parent;

#if QT_VERSION >= 0x040000
  this->setModal( modal );
  this->setWindowTitle( tr( "Select Java Interfaces" ) );
#else
  if ( !name ) setName( "interfacesW" );
  this->setCaption( tr( "Select Java Interfaces" ) );
#endif // QT_VERSION >= 0x040000

  this->resize( 515, 286 ); 
  this->setSizeGripEnabled( true );

  QVBoxLayout* layout = new QVBoxLayout( this ); 
  layout->setSpacing( 6 );
  layout->setMargin( 6 );

  layout->addLayout( this->createListPart( this ) );
  layout->addLayout( this->createButtonPart( this ) );

  this->createInterfacesList();
}

QLayout* interfacesW::createListPart( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  layout->setAlignment( Qt::AlignBottom );

  layout->addWidget( this->createPossibleInterfacesListBox( parent ) );
  layout->addLayout( this->createSelectButtonPart( parent ) );
  layout->addWidget( this->createSelectedInterfacesListBox( parent ) );
  return layout;
}

QWidget* interfacesW::createPossibleInterfacesListBox( QWidget* parent )
{
  QGroupBox* gbox = new QGroupBox( parent );
  gbox->setTitle( tr( "Possible Interfaces" ) );
#if QT_VERSION >= 0x040000
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  QListWidget* listbox = new QListWidget( gbox );
  this->possibleLB = listbox;
  layout->addWidget( listbox );
  gbox->setLayout( layout );
#else
//  gbox->setColumnLayout(0, Qt::Vertical );
  gbox->setOrientation( Qt::Vertical );

  QLayout* layout = gbox->layout();
  layout->setAlignment( Qt::AlignTop );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  QListBox* listbox = new QListBox( gbox );
  this->possibleLB = listbox;
  layout->add( listbox );
#endif // QT_VERSION >= 0x040000
  return gbox;
}

QLayout* interfacesW::createSelectButtonPart( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );
//  layout->setAlignment( Qt::AlignHCenter );
  layout->setAlignment( Qt::AlignJustify );

  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  layout->addItem( spacer );

  layout->addWidget( this->createAddAllButton( parent ) );
  layout->addWidget( this->createAddButton( parent ) );
  layout->addWidget( this->createRemoveButton( parent ) );
  layout->addWidget( this->createRemoveAllButton( parent ) );

  return layout;
}

QWidget* interfacesW::createAddButton( QWidget* parent )
{
  QToolButton* button = new QToolButton( parent );
  button->setText( tr( ">" ) );
#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Add selected" ) );
#else
  QToolTip::add( button, tr( "Add selected" ) );
#endif // QT_VERSION >= 0x040000
  connect( button, SIGNAL( clicked()), this, SLOT(addSelected()));
  this->addB = button;
  return button;
}

QWidget* interfacesW::createAddAllButton( QWidget* parent )
{
  QToolButton* button = new QToolButton( parent );
  button->setText( tr( ">>" ) );
#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Add all" ) );
#else
//  button->setTextLabel( tr( "<" ) );
  QToolTip::add( button, tr( "Add all" ) );
#endif // QT_VERSION >= 0x040000
  connect( button, SIGNAL( clicked()), this, SLOT(addAll()));
  this->addallB = button;
  return button;
}

QWidget* interfacesW::createRemoveButton( QWidget* parent )
{
  QToolButton* button = new QToolButton( parent );
  button->setText( tr( "<" ) );
#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Remove selected" ) );
#else
  QToolTip::add( button, tr( "Remove selected" ) );
#endif // QT_VERSION >= 0x040000
  connect( button, SIGNAL( clicked()), this, SLOT(removeSelected()));
  this->removeB = button;
  return button;
}

QWidget* interfacesW::createRemoveAllButton( QWidget* parent )
{
  QToolButton* button = new QToolButton( parent );
  button->setText( tr( "<<" ) );
#if QT_VERSION >= 0x040000
  button->setToolTip( tr( "Remove all" ) );
#else
  QToolTip::add( button, tr( "Remove all" ) );
#endif // QT_VERSION >= 0x040000
  connect( button, SIGNAL( clicked()), this, SLOT(removeAll()));
  this->removeallB = button;
  return button;
}

QWidget* interfacesW::createSelectedInterfacesListBox( QWidget* parent )
{
  QGroupBox* gbox = new QGroupBox( parent );
  gbox->setTitle( tr( "Selected Interfaces" ) );
#if QT_VERSION >= 0x040000
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  QListWidget* listbox = new QListWidget( gbox );
  this->selectedLB = listbox;
  layout->addWidget( listbox );
  gbox->setLayout( layout );
#else
//  gbox->setColumnLayout(0, Qt::Vertical );
  gbox->setOrientation( Qt::Vertical );

  QLayout* layout = gbox->layout();
  layout->setAlignment( Qt::AlignTop );
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  QListBox* listbox = new QListBox( gbox );
  this->selectedLB = listbox;
  layout->add( listbox );
#endif // QT_VERSION >= 0x040000

  return gbox;
}

QLayout* interfacesW::createButtonPart( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout(); 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout->addItem( spacer );

  layout->addWidget( this->createCancelButton( parent ) );
  layout->addWidget( this->createOkButton( parent ) );

  return layout;
}

QWidget* interfacesW::createOkButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&OK" )) );
  button->setAutoDefault( true );
  button->setDefault( true );
  connect( button, SIGNAL( clicked() ), this, SLOT( okClicked() ) );
  return button;
}

QWidget* interfacesW::createCancelButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&Cancel" )) );
  button->setAutoDefault( true );
  connect( button, SIGNAL( clicked() ), this, SLOT( cancelClicked()));
  return button;
}

/*  
 *  Destroys the object and frees any allocated resources
 */
interfacesW::~interfacesW()
{
  // no need to delete child widgets, Qt does it all for us
  this->possibleInterfaces.clear();
  this->selectedInterfaces.clear();
}


void interfacesW::createInterfacesList()
{
  Qt2TB::setBlock( false );
  this->possibleInterfaces = Qt2TB::getPossibleInterfacesI();
  this->selectedInterfaces = Qt2TB::getSelectedInterfacesI(this->possibleInterfaces);
  this->resetPossibleInterfaces();
  this->resetSelectedInterfaces();
  Qt2TB::setBlock( true );
}

// Accessor
QStringList interfacesW::getInterfaces() const
{
  return this->selectedInterfaces;
}

// The following two functions update the view to be consistent with
// the current document.
void interfacesW::resetPossibleInterfaces()
{
  this->possibleLB->clear();
  QStringList::Iterator iter;
  for (iter = this->possibleInterfaces.begin(); iter != this->possibleInterfaces.end(); ++iter)
  {
#if QT_VERSION >= 0x040000
    if (this->selectedInterfaces.indexOf(*iter) == -1)
      this->possibleLB->addItem(*iter);
#else
    if (this->selectedInterfaces.find(*iter) == this->selectedInterfaces.end())
      this->possibleLB->insertItem(*iter);
#endif // QT_VERSION >= 0x040000
  }
  this->addB->setEnabled(!this->possibleInterfaces.isEmpty());
  this->addallB->setEnabled(!this->possibleInterfaces.isEmpty());
}  

void interfacesW::resetSelectedInterfaces()
{
  this->selectedLB->clear();
  QStringList::Iterator iter;
  for (iter = this->selectedInterfaces.begin(); iter != this->selectedInterfaces.end(); ++iter)
  {
#if QT_VERSION >= 0x040000
      this->selectedLB->addItem(*iter);
#else
      this->selectedLB->insertItem(*iter);
#endif // QT_VERSION >= 0x040000
  }
  this->removeB->setEnabled(!this->selectedInterfaces.isEmpty());
  this->removeallB->setEnabled(!this->selectedInterfaces.isEmpty());
}


// The following four functions are used to update the document
void interfacesW::assertInvariant()
{
  // Asserts invariant that selectedInterfaces is always a subset of
  // possibleInterfaces  
  QStringList::Iterator it = this->selectedInterfaces.begin();
  
  while (it != this->selectedInterfaces.end())
  {
#if QT_VERSION >= 0x040000
    if (this->possibleInterfaces.indexOf(*it) == -1)
      it = this->selectedInterfaces.erase(it);
#else
    if (this->possibleInterfaces.findIndex(*it) == -1)
      it = this->selectedInterfaces.remove(it);
#endif // QT_VERSION >= 0x040000
    else
      ++it;
  }
}

// This function adds a QString to possibleInterfaces, ensuring that
// there are no repetitions
void interfacesW::addPossibleInterface(const QString& name)
{
#if QT_VERSION >= 0x040000
  if (this->possibleInterfaces.indexOf(name) == -1)
#else
  if (this->possibleInterfaces.findIndex(name) == -1)
#endif // QT_VERSION >= 0x040000
  {
    this->possibleInterfaces.append(name);
    this->possibleInterfaces.sort();
  }
}

// This function adds a QString to selectedInterfaces, ensuring that
// there are no repetitions
void interfacesW::addSelectedInterface(const QString& name)
{
#if QT_VERSION >= 0x040000
  if (this->selectedInterfaces.indexOf(name) == -1)
#else
  if (this->selectedInterfaces.findIndex(name) == -1)
#endif // QT_VERSION >= 0x040000
  {
    this->selectedInterfaces.append(name);
    this->selectedInterfaces.sort();
  }
}

// This function is used to perform an absolute update of the
// possibleInterfaces when any class has been removed from
// selectedInterfaces. This is because due to inheritance, some
// classes may only be generated as interfaces, it is is known that
// other classes *are to be* generated as interfaces
void interfacesW::updatePossibleInterfaces()
{
  this->possibleInterfaces.clear();
  Qt2TB::setBlock( false );
  //Qt2TB::getActivatedInterfacesI(this->selectedInterfaces, this->possibleInterfaces);
  this->possibleInterfaces = Qt2TB::getActivatedInterfacesI(this->selectedInterfaces);
  Qt2TB::setBlock( true );
}

// The following slots correspond to the different buttons available
// in the dialog
void interfacesW::okClicked()
{
  this->accept();
}

void interfacesW::cancelClicked()
{
  this->reject();
}

void interfacesW::addSelected()
{
  for (int i = 0; i < (int)this->possibleLB->count(); i++)
  {
#if QT_VERSION >= 0x040000
    QListWidgetItem* item = this->possibleLB->item(i);
    if (item->isSelected())
#else
    QListBoxItem* item = this->possibleLB->item(i);
    if (item->selected())
#endif // QT_VERSION >= 0x040000
      this->addSelectedInterface(item->text());
  }
  this->updatePossibleInterfaces();
  this->resetPossibleInterfaces();
  this->resetSelectedInterfaces();
}

void interfacesW::addAll()
{
  this->selectedInterfaces = this->possibleInterfaces;
  this->updatePossibleInterfaces();
  this->resetPossibleInterfaces();
  this->resetSelectedInterfaces();
}

void interfacesW::removeSelected()
{
  QStringList remainingInterfaces;
  for (int i = 0; i < (int)this->selectedLB->count(); i++)
  {
#if QT_VERSION >= 0x040000
    QListWidgetItem* item = this->selectedLB->item(i);
    if (item->isSelected())
    {
      int index = this->selectedInterfaces.indexOf(item->text());
      if (index != -1 )
        this->selectedInterfaces.removeAt(index);
    }
#else
    QListBoxItem* item = this->selectedLB->item(i);
    if (item->selected())
      this->selectedInterfaces.remove(item->text());
#endif // QT_VERSION >= 0x040000
  }
  this->updatePossibleInterfaces();
  this->assertInvariant();
  this->resetPossibleInterfaces();
  this->resetSelectedInterfaces();
}

void interfacesW::removeAll()
{
  this->selectedInterfaces.clear();
  this->updatePossibleInterfaces();
  this->resetPossibleInterfaces();
  this->resetSelectedInterfaces();
}

