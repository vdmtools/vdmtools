/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/referencesF.cc,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "referencesF.h"

/* 
 *  Constructs a referencesW which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
referencesW::referencesW( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "referencesW" );
    resize( 515, 286 ); 
    setCaption( tr( "References" ) );
    setSizeGripEnabled( true );
    referencesWLayout = new QVBoxLayout( this ); 
    referencesWLayout->setSpacing( 6 );
    referencesWLayout->setMargin( 0 );

    Layout4 = new QHBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( tr( "Classes" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    GroupBox1Layout = new QVBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 6 );
    GroupBox1Layout->setMargin( 0 );

    classesLB = new QListBox( GroupBox1, "classesLB" );
    classesLB->insertItem( tr( "Bank" ) );
    classesLB->insertItem( tr( "Teller" ) );
    classesLB->insertItem( tr( "Automatic Teller" ) );
    GroupBox1Layout->addWidget( classesLB );
    Layout4->addWidget( GroupBox1 );

    Layout3 = new QVBoxLayout; 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setTitle( tr( "Uses" ) );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 0 );
    GroupBox2->layout()->setMargin( 0 );
    GroupBox2Layout = new QVBoxLayout( GroupBox2->layout() );
    GroupBox2Layout->setAlignment( Qt::AlignTop );
    GroupBox2Layout->setSpacing( 6 );
    GroupBox2Layout->setMargin( 0 );

    usesLB = new QListBox( GroupBox2, "usesLB" );
    usesLB->insertItem( tr( "bank" ) );
    GroupBox2Layout->addWidget( usesLB );
    Layout3->addWidget( GroupBox2 );

    GroupBox3 = new QGroupBox( this, "GroupBox3" );
    GroupBox3->setTitle( tr( "Used by" ) );
    GroupBox3->setColumnLayout(0, Qt::Vertical );
    GroupBox3->layout()->setSpacing( 0 );
    GroupBox3->layout()->setMargin( 0 );
    GroupBox3Layout = new QVBoxLayout( GroupBox3->layout() );
    GroupBox3Layout->setAlignment( Qt::AlignTop );
    GroupBox3Layout->setSpacing( 6 );
    GroupBox3Layout->setMargin( 0 );

    usedLB = new QListBox( GroupBox3, "usedLB" );
    usedLB->insertItem( tr( "Teller" ) );
    GroupBox3Layout->addWidget( usedLB );
    Layout3->addWidget( GroupBox3 );
    Layout4->addLayout( Layout3 );
    referencesWLayout->addLayout( Layout4 );

    Layout2 = new QHBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
    buttonOk->setFlat( false );
    Layout2->addWidget( buttonOk );
    referencesWLayout->addLayout( Layout2 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
referencesW::~referencesW()
{
    // no need to delete child widgets, Qt does it all for us
}

