/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/windowF.cc,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "windowF.h"

/* 
 *  Constructs a windowF which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
windowF::windowF( QWidget* parent,  const char* name, bool modal, WFlags fl )
#if QT_VERSION >= 0x040000
    : QDialog( parent, fl )
#else
    : QDialog( parent, name, modal, fl )
#endif // QT_VERSION >= 0x040000
{
#if QT_VERSION >= 0x040000
  this->setModal(modal);
  if ( !name )
    setWindowTitle( "windowF" );
  else
    setWindowTitle( name );
#else
  if ( !name )
    setName( "windowF" );
#endif // QT_VERSION >= 0x040000

    resize( 237, 173 ); 

//    setCaption( tr( "Form1" ) );

    windowFLayout = new QVBoxLayout( this ); 
    windowFLayout->setSpacing( 6 );
    windowFLayout->setMargin( 11 );

    Layout1 = new QHBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setTitle( tr( "ButtonGroup1" ) );
    ButtonGroup1->setColumnLayout(0, Qt::Vertical );
    ButtonGroup1->layout()->setSpacing( 0 );
    ButtonGroup1->layout()->setMargin( 0 );
    ButtonGroup1Layout = new QVBoxLayout( ButtonGroup1->layout() );
    ButtonGroup1Layout->setAlignment( Qt::AlignTop );
    ButtonGroup1Layout->setSpacing( 6 );
    ButtonGroup1Layout->setMargin( 11 );

#if QT_VERSION >= 0x040000
    RadioButton1 = new QRadioButton( ButtonGroup1 );
#else
    RadioButton1 = new QRadioButton( ButtonGroup1, "RadioButton1" );
#endif // QT_VERSION >= 0x040000
    RadioButton1->setText( tr( "RadioButton1" ) );
    ButtonGroup1Layout->addWidget( RadioButton1 );

    RadioButton2 = new QRadioButton( ButtonGroup1, "RadioButton2" );
    RadioButton2->setText( tr( "RadioButton2" ) );
    ButtonGroup1Layout->addWidget( RadioButton2 );

    RadioButton3 = new QRadioButton( ButtonGroup1, "RadioButton3" );
    RadioButton3->setText( tr( "RadioButton3" ) );
    ButtonGroup1Layout->addWidget( RadioButton3 );

    RadioButton4 = new QRadioButton( ButtonGroup1, "RadioButton4" );
    RadioButton4->setText( tr( "RadioButton4" ) );
    ButtonGroup1Layout->addWidget( RadioButton4 );
    Layout1->addWidget( ButtonGroup1 );

    ButtonGroup2 = new QButtonGroup( this, "ButtonGroup2" );
    ButtonGroup2->setTitle( tr( "ButtonGroup2" ) );
    ButtonGroup2->setColumnLayout(0, Qt::Vertical );
    ButtonGroup2->layout()->setSpacing( 0 );
    ButtonGroup2->layout()->setMargin( 0 );
    ButtonGroup2Layout = new QVBoxLayout( ButtonGroup2->layout() );
    ButtonGroup2Layout->setAlignment( Qt::AlignTop );
    ButtonGroup2Layout->setSpacing( 6 );
    ButtonGroup2Layout->setMargin( 11 );

    CheckBox1 = new QCheckBox( ButtonGroup2, "CheckBox1" );
    CheckBox1->setText( tr( "CheckBox1" ) );
    ButtonGroup2Layout->addWidget( CheckBox1 );

    CheckBox2 = new QCheckBox( ButtonGroup2, "CheckBox2" );
    CheckBox2->setText( tr( "CheckBox2" ) );
    ButtonGroup2Layout->addWidget( CheckBox2 );

    CheckBox3 = new QCheckBox( ButtonGroup2, "CheckBox3" );
    CheckBox3->setText( tr( "CheckBox3" ) );
    ButtonGroup2Layout->addWidget( CheckBox3 );
    Layout1->addWidget( ButtonGroup2 );
    windowFLayout->addLayout( Layout1 );

    Layout2 = new QHBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setText( tr( "PushButton1" ) );
    Layout2->addWidget( PushButton1 );

    PushButton2 = new QPushButton( this, "PushButton2" );
    PushButton2->setText( tr( "PushButton2" ) );
    Layout2->addWidget( PushButton2 );
    windowFLayout->addLayout( Layout2 );

    // signals and slots connections
    connect( PushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
windowF::~windowF()
{
    // no need to delete child widgets, Qt does it all for us
}

