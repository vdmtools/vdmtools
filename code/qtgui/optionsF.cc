/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/optionsF.cc,v $
*  * VERSION
*  *    $Revision: 1.29 $
*  * DATE
*  *    $Date: 2006/02/07 01:52:03 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "optionsF.h"
#include "mainF.h"
#include "interface2TB.h"
#include "interfacesF.h"
#include "qtport.h"

#if QT_VERSION < 0x040000
#include <qfile.h>
#endif // QT_VERSION < 0x040000

/* 
 *  Constructs a optionsW which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
optionsW::optionsW( QWidget* parent, const char* name, bool modal, WFlags fl )
#if QT_VERSION >= 0x040000
  : QDialog( parent, fl )
#else
  : QDialog( parent, name, modal, fl )
#endif // QT_VERSION >= 0x040000
{
  this->mainw = (mainW*)parent;

  this->setWindowName( (name == NULL) ? "optionsW" : name );

#if QT_VERSION >= 0x040000
  this->setModal(modal);
  this->setWindowTitle( tr( "Project Options" ) );
#else
  if ( !name ) {
    this->setName( "optionsW" );
  }
  this->setCaption( tr( "Project Options" ) );
#endif // QT_VERSION >= 0x040000
  this->setSizeGripEnabled( true );

  QVBoxLayout* layout = this->createVBoxLayout( this ); 
  layout->setMargin( 11 );
  layout->addWidget( this->createTabPart( this ) );
  layout->addLayout( this->createButtonPart( this ) );

#ifdef VICE
#ifdef _MSC_VER
  this->resize( this->sizeHint().width() + 40, this->sizeHint().height());
#else
  this->resize( this->sizeHint() );
#endif // _MSC_VER
#else
  this->resize( this->sizeHint() );
#endif // VICE

  this->setOptions();
}

QWidget* optionsW::createTabPart( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QTabWidget* tabWidget = new QTabWidget( this );
  tabWidget->setTabPosition( QTabWidget::North );
#else
  QTabWidget* tabWidget = new QTabWidget( this );
  tabWidget->setTabPosition( QTabWidget::Top );
#endif // QT_VERSION >= 0x040000

  tabWidget->setTabShape( QTabWidget::Rounded );

#if QT_VERSION >= 0x040000
  tabWidget->addTab( this->createInterpreterTab( tabWidget ), tr( "Interpreter" ) );
#else
  tabWidget->insertTab( this->createInterpreterTab( tabWidget ), tr( "Interpreter" ) );
#endif // QT_VERSION >= 0x040000

#ifdef VDMPP
#ifdef VICE
#if QT_VERSION >= 0x040000
  tabWidget->addTab( this->createVICETab( tabWidget ), tr( "VICE" ) );
#else
  tabWidget->insertTab( this->createVICETab( tabWidget ), tr( "VICE" ) );
#endif // QT_VERSION >= 0x040000
#endif // VICE
#endif // VDMPP

#if QT_VERSION >= 0x040000
  tabWidget->addTab( this->createTypeCheckerTab( tabWidget ), tr( "Type checker" ) );
  tabWidget->addTab( this->createPrettyPrinterTab( tabWidget ), tr( "Pretty printer" ) );
  tabWidget->addTab( this->createCppCodeGeneratorTab( tabWidget ), tr( "C++ code generator" ) );

#ifdef VDMPP
  tabWidget->addTab( this->createJavaCodeGeneratorTab( tabWidget ), tr( "Java code generator" ) );
  tabWidget->addTab( this->createJ2VTab( tabWidget ), tr( "Java to VDM++" ) );
#endif // VDMPP
#else
  tabWidget->insertTab( this->createTypeCheckerTab( tabWidget ), tr( "Type checker" ) );
  tabWidget->insertTab( this->createPrettyPrinterTab( tabWidget ), tr( "Pretty printer" ) );
  tabWidget->insertTab( this->createCppCodeGeneratorTab( tabWidget ), tr( "C++ code generator" ) );

#ifdef VDMPP
  tabWidget->insertTab( this->createJavaCodeGeneratorTab( tabWidget ), tr( "Java code generator" ) );
  tabWidget->insertTab( this->createJ2VTab( tabWidget ), tr( "Java to VDM++" ) );
#endif // VDMPP
#endif // QT_VERSION >= 0x040000

  this->maintab = tabWidget;
  return tabWidget;
}

QSpacerItem * optionsW::createSpacer()
{
  return new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
}

QFrame * optionsW::createFrame( QWidget* parent )
{
  QFrame* frame = new QFrame( parent );
  frame->setFrameShape( QFrame::StyledPanel );
  frame->setFrameShadow( QFrame::Raised );
  return frame;
}

QGroupBox * optionsW::createVGroupBox( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QGroupBox* gbox = new QGroupBox( parent );
  QVBoxLayout* layout = this->createVBoxLayout( NULL );
  gbox->setLayout(layout);
#else
  QGroupBox* gbox = new QGroupBox( parent );
  gbox->setOrientation( Qt::Vertical );
  QLayout* layout = gbox->layout(); 
  layout->setSpacing(6);
  layout->setMargin(11);
#endif // QT_VERSION >= 0x040000
  return gbox;
}

QVBoxLayout* optionsW::createVBoxLayout( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout( parent ); 
  layout->setSpacing( 6 );
  layout->setAlignment( Qt::AlignTop );
  return layout;
}

QHBoxLayout* optionsW::createHBoxLayout( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout( parent ); 
  layout->setSpacing( 6 );
  return layout;
}

QVBoxLayout* optionsW::createVBoxFrameLayout( QFrame* frame )
{
  QVBoxLayout* layout = this->createVBoxLayout( frame ); 
#if QT_VERSION >= 0x040000
#else
  layout->setMargin( 6 );
#endif // QT_VERSION >= 0x040000
  layout->setAlignment( Qt::AlignTop );
  return layout;
}

QSpinBox* optionsW::createSpinBox( QWidget* parent, int min, int max, int value )
{
  QSpinBox * sbox = new QSpinBox( parent );
#if QT_VERSION >= 0x040000
  sbox->setMaximum( max );
  sbox->setMinimum( min );
#else
  sbox->setMaxValue( max );
  sbox->setMinValue( min );
#endif // QT_VERSION >= 0x040000
  sbox->setValue( value );

  return sbox;
}

QComboBox* optionsW::createComboBox( QWidget* parent )
{
  QComboBox * cbox = new QComboBox( parent );
#if QT_VERSION >= 0x040000
  cbox->setMaxVisibleItems( 5 );
#else
  cbox->setSizeLimit( 5 );
#endif // QT_VERSION >= 0x040000
//  cbox->setMaxCount( 5 );
  cbox->setAutoCompletion( true );
  cbox->setDuplicatesEnabled( false );

  return cbox;
}

QWidget* optionsW::createInterpreterTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab ); 

  layout->addWidget( this->createInterpreterFrame( tab ) );

  return tab;
}

QWidget* optionsW::createInterpreterFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame ); 

  QHBoxLayout* hlayout = this->createHBoxLayout( NULL ); 
  hlayout->addItem( this->createInterpreterLayout1( frame ) );
  hlayout->addItem( this->createInterpreterLayout2( frame ) );

  layout->addItem(hlayout);
  layout->addItem(this->createExpressionLayout( frame ));

  return frame;
}

QLayout* optionsW::createInterpreterLayout1( QWidget* parent )
{
  QVBoxLayout* layout = this->createVBoxLayout( NULL ); 

  layout->addWidget( this->createRuntimeCheckingGroupBox( parent ));

  this->ip_ppValues = new QCheckBox( parent );
  this->ip_ppValues->setText( mainW::mf(tr( "Pretty printing of &values" )) );
  layout->addWidget( this->ip_ppValues );

  this->ip_exception = new QCheckBox( parent );
  this->ip_exception->setText( mainW::mf(tr( "Catch RunTime Error as &exception" )) );
  layout->addWidget( this->ip_exception );

  this->ip_oldreverse = new QCheckBox( parent );
  this->ip_oldreverse->setText( mainW::mf( tr( "Old reverse in Sequence Loop Stmt" )) );
  layout->addWidget( this->ip_oldreverse );

#ifdef VDMPP
  layout->addItem( this->createSpacer() );
#endif //VDMPP

  QObject::connect( this->ip_dynInvCheck, SIGNAL(clicked()), this, SLOT(invClicked()));

  return layout;
}

QWidget* optionsW::createRuntimeCheckingGroupBox( QWidget * parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Runtime checking") );

  QLayout* layout = gbox->layout(); 

  this->ip_dynTypeCheck = new QCheckBox( parent );
  this->ip_dynTypeCheck->setText( mainW::mf(tr( "&Dynamic type check" )) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->ip_dynTypeCheck );
#else
  layout->add( this->ip_dynTypeCheck );
#endif // QT_VERSION >= 0x040000

  this->ip_dynInvCheck = new QCheckBox( parent );
  this->ip_dynInvCheck->setText( mainW::mf(tr( "Dynamic checks of &invariants" )) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->ip_dynInvCheck );
#else
  layout->add( this->ip_dynInvCheck );
#endif // QT_VERSION >= 0x040000

  this->ip_preCheck = new QCheckBox( parent );
  this->ip_preCheck->setText( mainW::mf(tr( "Check of &pre-conditions" )) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->ip_preCheck );
#else
  layout->add( this->ip_preCheck );
#endif // QT_VERSION >= 0x040000

  this->ip_postCheck = new QCheckBox( parent );
  this->ip_postCheck->setText( mainW::mf(tr( "Check of p&ost-conditions" )) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->ip_postCheck );
#else
  layout->add( this->ip_postCheck );
#endif // QT_VERSION >= 0x040000

  this->ip_measureCheck = new QCheckBox( parent );
  this->ip_measureCheck->setText( mainW::mf(tr( "Check of &measures" )) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->ip_measureCheck );
#else
  layout->add( this->ip_measureCheck );
#endif // QT_VERSION >= 0x040000

  return gbox;
}

QLayout* optionsW::createInterpreterLayout2( QWidget* parent )
{
  QVBoxLayout* layout = this->createVBoxLayout( NULL ); 

  layout->addWidget( this->createRandomGeneratorGroupBox(parent) );

#ifdef VDMPP
  layout->addWidget( this->createMultiThreadGroupBox( parent ) );
#endif // VDMPP
#ifdef VDMSL
  layout->addItem( this->createSpacer() );
#endif // VDMSL

  return layout;
}

QLayout* optionsW::createExpressionLayout( QWidget* parent )
{
  QVBoxLayout* layout = this->createVBoxLayout( NULL ); 

  QHBoxLayout* hlayout = new QHBoxLayout(); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Expression: " ) );
  hlayout->addWidget( label );

  QLineEdit * edit = new QLineEdit( parent );
  hlayout->addWidget( edit );
  this->ip_expression = edit;

  layout->addItem(hlayout);

  return layout;
}

QWidget* optionsW::createRandomGeneratorGroupBox( QWidget * parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Nondeterministic") );

  QLayout* layout = gbox->layout(); 

  layout->addItem( this->createRandomGeneratorSeedLayout( gbox ) );
  layout->addItem( this->createRandomGeneratorMessageLayout( gbox ) );

  return gbox;
}

QLayout* optionsW::createRandomGeneratorSeedLayout( QWidget * parent )
{
  QHBoxLayout* layout = new QHBoxLayout(); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Initialize random generator with:" ) );
  layout->addWidget( label );

  QSpinBox * sbox = this->createSpinBox( parent, -1, 999999, -1 );

  layout->addWidget( sbox );
  this->ip_rndGen = sbox;

  return layout;
}

QLayout* optionsW::createRandomGeneratorMessageLayout( QWidget * parent )
{
  QHBoxLayout* layout = new QHBoxLayout(); 

  layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Minimum) );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "(-1 non random)" ) );
  layout->addWidget( label );

  return layout;
}

#ifdef VDMPP
QWidget* optionsW::createMultiThreadGroupBox( QWidget * parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Multi Thread") );

  QLayout* layout = gbox->layout(); 

  ip_prioritySchd = new QCheckBox( gbox );
  ip_prioritySchd->setText( mainW::mf(tr( "Enable priority-based &scheduling" )) );
#if QT_VERSION >= 0x040000
  layout->addWidget( ip_prioritySchd );
#else
  layout->add( ip_prioritySchd );
#endif // QT_VERSION >= 0x040000

  layout->addItem( this->createPrimarySchdAlgorithmLayout( gbox ) );
  layout->addItem( this->createMaxInstrLayout( gbox ) );
#ifdef VICE
  layout->addItem( this->createMaxTimeLayout( gbox ) );
#endif // VICE

  return gbox;
}

QLayout* optionsW::createMaxInstrLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Maximum instructions per slice:" ) );
  layout->addWidget( label );
  this->ip_maxInstrLabel = label;

  QSpinBox * sbox = this->createSpinBox( parent, 1, 999999, 1000 );

  layout->addWidget( sbox );
  this->ip_maxInstr = sbox;

  return layout;
}

#ifdef VICE
QLayout* optionsW::createMaxTimeLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Maximum time per slice:" ) );
  layout->addWidget( label );
  this->ip_maxTimeLabel = label;

  QSpinBox * sbox = this->createSpinBox( parent, 1, 999999, 1000 );

  layout->addWidget( sbox );
  this->ip_maxTime = sbox;

  return layout;
}
#endif // VICE

QLayout* optionsW::createPrimarySchdAlgorithmLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Primary Scheduling Algorithm" ) );
  layout->addWidget( label );

  QComboBox * cbox = this->createComboBox( parent );

#if QT_VERSION >= 0x040000
  cbox->addItem(tr("pure_cooperative"));
#ifdef VICE
  cbox->addItem(tr("timeslice"));
#endif // VISE
  cbox->addItem(tr("instruction_number_slice"));
#else
  cbox->insertItem(tr("pure_cooperative"));
#ifdef VICE
  cbox->insertItem(tr("timeslice"));
#endif // VISE
  cbox->insertItem(tr("instruction_number_slice"));
#endif // QT_VERSION >= 0x040000
  layout->addWidget( cbox );

  QObject::connect(cbox, SIGNAL(activated(int)), this, SLOT(algorithmChanged(int)));

  this->ip_primaryAlg = cbox;

  return layout;
}

#ifdef VICE
QLayout* optionsW::createStepSizeLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Step Size:" ) );
  layout->addWidget( label );

  QSpinBox * sbox = this->createSpinBox( parent, 0, 999999, 100 );
  layout->addWidget( sbox );

  this->ip_stepSize = sbox;

  return layout;
}

QWidget* optionsW::createVirtualCPUCapacityGroupBox( QWidget * parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Virtual CPU") );

  QLayout* layout = gbox->layout(); 

  QCheckBox* checkbox = new QCheckBox( gbox );
  checkbox->setText( tr( "Specify Virtual CPU Capacity" ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( checkbox );
#else
  layout->add( checkbox );
#endif // QT_VERSION >= 0x040000

  this->ip_vcpuCapacitycb = checkbox;

  QLabel* label2 = new QLabel( gbox );
  label2->setText( tr( "(Unchecking means INFINITE)" ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( label2 );
#else
  layout->add( label2 );
#endif // QT_VERSION >= 0x040000

  QLabel* label = new QLabel( gbox );
  label->setText( tr( "Virtual CPU Capacity:" ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( label );
#else
  layout->add( label );
#endif // QT_VERSION >= 0x040000

  this->ip_vcpuCapacityLabel = label;

  QSpinBox * sbox = this->createSpinBox( gbox, 1, 99999999, 1000000 );

#if QT_VERSION >= 0x040000
  layout->addWidget( sbox );
#else
  layout->add( sbox );
#endif // QT_VERSION >= 0x040000

  this->ip_vcpuCapacity = sbox;

  QObject::connect(this->ip_vcpuCapacitycb, SIGNAL(clicked()), this, SLOT(vcpuCapacityEnabled()));

  return gbox;
}

QLayout* optionsW::createDefaultCapacityLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Default CPU Capacity:" ) );
  layout->addWidget( label );

  QSpinBox * sbox = this->createSpinBox( parent, 1, 99999999, 1000000 );
  layout->addWidget( sbox );
  this->ip_cpuCapacity = sbox;

  return layout;
}

QLayout* optionsW::createJitterModeLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Jitter Mode:" ) );
  layout->addWidget( label );

  QComboBox * cbox = this->createComboBox( parent );

#if QT_VERSION >= 0x040000
  cbox->addItem(tr("Early"));
  cbox->addItem(tr("Random"));
  cbox->addItem(tr("Late"));
#else
  cbox->insertItem(tr("Early"));
  cbox->insertItem(tr("Random"));
  cbox->insertItem(tr("Late"));
#endif // QT_VERSION >= 0x040000
  layout->addWidget( cbox );

  this->ip_jitterMode = cbox;

  return layout;
}

QWidget* optionsW::createTraceLogGroupBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Trace Logging") );

  QLayout* layout = gbox->layout(); 

  QCheckBox * cbox = new QCheckBox( gbox );
  cbox->setText( tr( "Log All Operation's Arguments" ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( cbox );
#else
  layout->add( cbox );
#endif // QT_VERSION >= 0x040000
  this->ip_logArgsAllCheck = cbox;

  QObject::connect(cbox, SIGNAL(clicked()), this, SLOT(logArgsAllEnabled()));

//  QPushButton* button = new QPushButton( gbox, "OpSelectButton" );
//  button->setText( tr( "Select operations" ) );
//  layout->add( button );
//  this->ip_opsSelectButton = button;
//  QObject::connect(button,SIGNAL(clicked()),this,SLOT(interfaces()));

#if QT_VERSION >= 0x040000
  QListWidget * lbox = new QListWidget( gbox );
  layout->addWidget( lbox );
#else
  QListBox * lbox = new QListBox( gbox );
  layout->add( lbox );
#endif // QT_VERSION >= 0x040000
  this->ip_selectedOpsList = lbox;

  layout->addItem( this->createClassInputLayout( gbox ) );
  layout->addItem( this->createOpInputLayout( gbox ) );
  layout->addItem( this->createAddDelButtonLayout( gbox ) );
 
  return gbox;
}

QLayout* optionsW::createClassInputLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Class Name:" ) );
  layout->addWidget( label );
  this->ip_classNameLabel = label;

  QComboBox * cbox = this->createComboBox( parent );

  QObject::connect(cbox, SIGNAL(activated(int)), this, SLOT(classChanged(int)));

  layout->addWidget( cbox );
  this->ip_className = cbox;

  return layout;
}

QLayout* optionsW::createOpInputLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Operation Name:" ) );
  layout->addWidget( label );
  this->ip_opNameLabel = label;

  QComboBox * cbox = this->createComboBox( parent );

  layout->addWidget( cbox );
  this->ip_opName = cbox;

  return layout;
}

QLayout* optionsW::createAddDelButtonLayout( QWidget * parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  layout->addWidget( this->createAddButton( parent ) );
  layout->addWidget( this->createDeleteButton( parent ) );
  return layout;
}

QPushButton * optionsW::createAddButton( QWidget * parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Add Operation" ) );
  this->ip_opAddButton = button;
  QObject::connect(button,SIGNAL(clicked()),this,SLOT(addOperation()));
  return button;
}

QPushButton * optionsW::createDeleteButton( QWidget * parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Remove Operation" ) );
  this->ip_opDelButton = button;
  QObject::connect(button,SIGNAL(clicked()),this,SLOT(delOperation()));
  return button;
}
#endif // VICE
#endif // VDMPP

QWidget* optionsW::createTypeCheckerTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab ); 

  layout->addWidget( this->createTypeCheckerFrame( tab ) );

  return tab;
}

QWidget* optionsW::createTypeCheckerFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame ); 

  layout->addWidget(this->createTypeCheckerButtonGroup( frame ));
  layout->addWidget( this->createTypeCheckerExtCheckBox( frame ) );
  layout->addWidget( this->createTypeCheckerMsgSepCheckBox( frame ) );
#ifdef VDMSL
  layout->addWidget( this->createStandardVDMSLCheckBox( frame ) );
#endif // VDMSL
  layout->addWidget( this->createVDM10CheckBox( frame ) );
  layout->addItem( this->createSpacer() );
 
  return frame;
}

QWidget* optionsW::createTypeCheckerButtonGroup( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QGroupBox * bg = new QGroupBox( tr("Type Check Mode"), parent );
  QRadioButton* radio1 = new QRadioButton( tr( "\"pos\" type check" ) );
  this->tc_posTc = radio1;
  QRadioButton* radio2 = new QRadioButton( tr( "\"def\" type check" ) );
  this->tc_defTc = radio2;

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(radio1);
  layout->addWidget(radio2);
  bg->setLayout(layout);
#else
  QVButtonGroup* bg = new QVButtonGroup(tr("Type Check Mode"), parent, "tcButtons");

  QRadioButton* radio1 = new QRadioButton(bg, "tcPos");
  radio1->setText( tr( "\"pos\" type check" ) );
  this->tc_posTc = radio1;

  QRadioButton* radio2 = new QRadioButton(bg, "tcDef");
  radio2->setText( tr( "\"def\" type check" ) );
  this->tc_defTc = radio2;
#endif // QT_VERSION >= 0x040000

  return bg;
}

QCheckBox* optionsW::createTypeCheckerExtCheckBox( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "Extended type check" ) );
  this->tc_extTc = cbox;
  return cbox;
}

QCheckBox* optionsW::createTypeCheckerMsgSepCheckBox( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "Warning/error message separation" ) );
  this->tc_msgSeparation = cbox;
  return cbox;
}

QCheckBox* optionsW::createStandardVDMSLCheckBox( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "Standard VDMSL" ) );
  this->tc_standardVDMSL = cbox;
  return cbox;
}

QCheckBox* optionsW::createVDM10CheckBox( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "VDM10 Compatible" ) );
  this->tc_vdm10 = cbox;
  return cbox;
}

QWidget* optionsW::createPrettyPrinterTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab ); 

  layout->addWidget( this->createPrettyPrinterFrame( tab ) );

  return tab;
}

QWidget* optionsW::createPrettyPrinterFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame ); 

  layout->addWidget( this->createPrettyPrinterIndex( frame ) );
  layout->addWidget( this->createPrettyPrinterCloring( frame ) );
  layout->addItem( this->createSpacer() );

  return frame;
}

QWidget* optionsW::createPrettyPrinterIndex( QWidget* parent )
{
#if QT_VERSION >= 0x040000
  QGroupBox * bg = new QGroupBox( tr("Output index"), parent );
  QRadioButton* radio1 = new QRadioButton( tr( "No output index" ) );
  this->pp_noIndex = radio1;
  QRadioButton* radio2 = new QRadioButton( tr( "Output index of definitions" ) );
  this->pp_defIndex = radio2;
  QRadioButton* radio3 = new QRadioButton( tr( "Output index of definitions and uses" ) );
  this->pp_useIndex = radio3;

  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(radio1);
  layout->addWidget(radio2);
  layout->addWidget(radio3);
  bg->setLayout(layout);
#else
  QVButtonGroup* bg = new QVButtonGroup(tr("Output index"), parent, "ppButtons");

  QRadioButton* radio1 = new QRadioButton(bg, "pp_noIndex");
  radio1->setText( tr( "No output index" ) );
  this->pp_noIndex = radio1;

  QRadioButton* radio2 = new QRadioButton(bg, "pp_defIndex");
  radio2->setText( tr( "Output index of definitions" ) );
  this->pp_defIndex = radio2;

  QRadioButton* radio3 = new QRadioButton(bg, "pp_useIndex");
  radio3->setText( tr( "Output index of definitions and uses" ) );
  this->pp_useIndex = radio3;
#endif // QT_VERSION >= 0x040000

  return bg;
}

QCheckBox* optionsW::createPrettyPrinterCloring( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "Test coverage coloring" ) );
  this->pp_tcovCol = cbox;
  return cbox;
}

QWidget* optionsW::createCppCodeGeneratorTab( QWidget* parent )
{
  QWidget* widget = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( widget ); 

  layout->addWidget( this->createCppCodeGeneratorFrame( widget ) );

  return widget;
}

QWidget* optionsW::createCppCodeGeneratorFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame ); 
  layout->addWidget( this->createCppCodeGenPosInfCheckBox( frame ) );
#ifdef VDMPP
  layout->addWidget( this->createCppCodeGenCheckCondsCheckBox( frame ) );
#endif // VDMPP
  layout->addItem( this->createSpacer() );

  return frame;
}

QCheckBox* optionsW::createCppCodeGenPosInfCheckBox( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "Output position information" ) );
  this->cpp_posInfo = cbox;
  return cbox;
}

QCheckBox* optionsW::createCppCodeGenCheckCondsCheckBox( QWidget* parent )
{
  QCheckBox * cbox = new QCheckBox( parent );
  cbox->setText( tr( "Check pre and post conditions" ) );
  this->cpp_checkConds = cbox;
  return cbox;
}

#ifdef VDMPP
QWidget* optionsW::createJavaCodeGeneratorTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab ); 

  layout->addWidget( this->createJavaCodeGeneratorFrame( tab ) );
  return tab;
}

QWidget* optionsW::createJavaInterfacesButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Select interfaces" ) );
  QObject::connect(button,SIGNAL(clicked()),this,SLOT(interfaces()));
  return button;
}

QLayout* optionsW::createJavaPackageLayout( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Package:" ) );
  layout->addWidget( label );

  QLineEdit * edit = new QLineEdit( parent );
  layout->addWidget( edit );
  this->jcg_packageName = edit;

  return layout;
}

QLayout* optionsW::createOutputDirCheckBox( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Specify Code Output Directory" ) );
  this->jcg_useOutputDir = checkbox;

  layout->addWidget( checkbox );

  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Select Code Output Directory" ) );
  connect( button, SIGNAL(clicked()), this, SLOT(selectJCGDir()));
  this->jcg_selectDir = button;
  layout->addWidget( button );

  return layout;
}

QLayout* optionsW::createOutputDirNamePart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Output Directroy:" ) );
  layout->addWidget( label );

  this->jcg_outputDirLabel = label;

  QLineEdit* le = new QLineEdit( parent );

  layout->addWidget( le );
  this->jcg_outputDirName = le;

  return layout;
}


QWidget* optionsW::createJavaCodeGeneratorFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame ); 

  QHBoxLayout* hlayout = this->createHBoxLayout( NULL ); 
  QVBoxLayout* vlayout1 = this->createVBoxLayout( NULL ); 
  QVBoxLayout* vlayout2 = this->createVBoxLayout( NULL ); 

  this->jcg_skeletonsOnly = new QCheckBox( frame );
  this->jcg_skeletonsOnly->setText( tr( "Generate only skeletons, except for types" ) );
  vlayout1->addWidget( this->jcg_skeletonsOnly );

  this->jcg_typesOnly = new QCheckBox( frame );
  this->jcg_typesOnly->setText( tr( "Generate only types" ) );
  vlayout1->addWidget( this->jcg_typesOnly );

  this->jcg_useLongs = new QCheckBox( frame );
  this->jcg_useLongs->setText( tr( "Generate integers as longs" ) );
  vlayout1->addWidget( this->jcg_useLongs );

  this->jcg_genConc = new QCheckBox( frame );
  this->jcg_genConc->setText( tr( "Generate code with concurrency constructs" ) );
  vlayout1->addWidget( this->jcg_genConc );

  this->jcg_genConds = new QCheckBox( frame );
  this->jcg_genConds->setText( tr( "Generate pre and post functions/operations" ) );
  vlayout2->addWidget( this->jcg_genConds );

  this->jcg_checkConds = new QCheckBox( frame );
  this->jcg_checkConds->setText( tr( "Check pre and post conditions" ) );
  vlayout2->addWidget( this->jcg_checkConds );

  this->jcg_vdmPrefix = new QCheckBox( frame );
  this->jcg_vdmPrefix->setText( tr( "Disable generate \"vdm_\" prefix" ) );
  vlayout2->addWidget( this->jcg_vdmPrefix);

  this->jcg_needBackup = new QCheckBox( frame );
  this->jcg_needBackup->setText( tr( "Create backup file (*.bak)" ) );
  vlayout2->addWidget( this->jcg_needBackup);

  hlayout->addLayout(vlayout1);
  hlayout->addLayout(vlayout2);
  layout->addLayout(hlayout);

  layout->addWidget( this->createJavaInterfacesButton( frame ) );
  layout->addLayout( this->createJavaPackageLayout( frame ) );

  layout->addItem( this->createOutputDirCheckBox( frame ) );
  layout->addItem( this->createOutputDirNamePart( frame ) );

  return frame;
}

QWidget* optionsW::createJ2VTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab ); 

  layout->addWidget( this->createJ2VFrame( tab ) );

  return tab;
}

QWidget* optionsW::createJ2VFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame ); 

  j2v_stubsOnly = new QCheckBox( frame );
  j2v_stubsOnly->setText( mainW::mf(tr( "Generate &stubs only" )) );
  layout->addWidget( j2v_stubsOnly );

//  j2v_autoRenaming = new QCheckBox( frame );
//  j2v_autoRenaming->setText( tr( "Automatic &renaming" ) );
//  layout->addWidget( j2v_autoRenaming );

//  j2v_genAccessors = new QCheckBox( frame );
//  j2v_genAccessors->setText( tr( "&Generate acessor functions" ) );
//  layout->addWidget( j2v_genAccessors );

  j2v_transforms = new QCheckBox( frame );
  j2v_transforms->setText( mainW::mf(tr( "Apply VDM++ &transformations" )) );
  layout->addWidget( j2v_transforms );
  layout->addItem( this->createSpacer() );

  return frame;
}
#ifdef VICE
QWidget* optionsW::createVICETab( QWidget* parent )
{
  QWidget* widget = new QWidget( parent );
  QHBoxLayout* layout = this->createHBoxLayout( widget ); 

  layout->addItem( this->createVICELayout1( widget ) );
  layout->addItem( this->createVICELayout2( widget ) );

  return widget;
}

QLayout* optionsW::createVICELayout1( QWidget* parent )
{
  QVBoxLayout* layout = this->createVBoxLayout( NULL ); 

  layout->addLayout( this->createStepSizeLayout(parent) );
  layout->addLayout( this->createJitterModeLayout(parent) );
  layout->addLayout( this->createDefaultCapacityLayout(parent) );
  layout->addWidget( this->createVirtualCPUCapacityGroupBox(parent) );
  layout->addItem( this->createSpacer() );

  return layout;
}

QLayout* optionsW::createVICELayout2( QWidget* parent )
{
  QVBoxLayout* layout = this->createVBoxLayout( NULL ); 

  layout->addWidget( this->createTraceLogGroupBox( parent ) );

  return layout;
}
#endif // VICE
#endif // VDMPP

QLayout* optionsW::createButtonPart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  layout->addWidget( this->createSaveOptionsCheckbox( parent ) );
  layout->addItem( this->createSpacer() );
  layout->addWidget( this->createCancelButton( parent ) );
  layout->addWidget( this->createApplyButton( parent ) );
  layout->addWidget( this->createOkButton( parent ) );

  return layout;
}

QWidget* optionsW::createHelpButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&Help" )) );
  button->setAutoDefault( true );
  return button;
}

QWidget* optionsW::createSaveOptionsCheckbox( QWidget* parent )
{
  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Save Options" ) );
  checkbox->setChecked(true);
  this->saveOptionsFlag = checkbox;
  return checkbox;
}

QWidget* optionsW::createOkButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&OK" )) );
  button->setAutoDefault( true );
  button->setDefault( true );
  connect( button, SIGNAL( clicked() ), this, SLOT( okPressed() ) );
  return button;
}

QWidget* optionsW::createCancelButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&Cancel" )) );
  button->setAutoDefault( true );
  connect( button, SIGNAL( clicked() ), this, SLOT( reject() ) );
  return button;
}

QWidget* optionsW::createApplyButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&Apply" )) );
  button->setAutoDefault( true );
  connect( button,  SIGNAL( clicked() ), this, SLOT( putOptions() ));
  return button;
}

/*  
 *  Destroys the object and frees any allocated resources
 */
optionsW::~optionsW()
{
    // no need to delete child widgets, Qt does it all for us
}

void optionsW::interfaces()
{
  interfacesW * interfacesF = new interfacesW(this, "", true, 0);
#if QT_VERSION >= 0x040000
  interfacesF->setWindowIcon(windowIcon());
#else
  interfacesF->setIcon(*icon());
#endif // QT_VERSION >= 0x040000
  interfacesF->show();
  interfacesF->exec();
  if (interfacesF->result() == QDialog::Accepted)
  {
    QStringList interfaces (interfacesF->getInterfaces());
    Qt2TB::setSelectedInterfacesI(interfaces);
  }
}

void optionsW::show()
{
//  this->loadOptions();
  this->setOptions();
  QDialog::show();
}

void optionsW::okPressed()
{
  this->putOptions();
  accept();
  this->saveOptions();
}

void optionsW::setOptions()
{
  QMap<QString, QString> optionMap (Qt2TB::GetOptions());

  this->ip_dynTypeCheck->setChecked(String2Bool(optionMap[ "DTC" ]));
  this->ip_dynInvCheck->setChecked(String2Bool(optionMap[ "INV" ]));
  this->ip_preCheck->setChecked(String2Bool(optionMap[ "PRE" ]));
  this->ip_postCheck->setChecked(String2Bool(optionMap[ "POST" ]));
  this->ip_measureCheck->setChecked(String2Bool(optionMap[ "MEASURE" ]));
#ifdef VDMPP
  this->ip_prioritySchd->setChecked(String2Bool(optionMap[ "PRIORITY" ]));

  QString alg (optionMap[ "PRIMARYALGORITHM" ]);
  for(int i = 0; i < this->ip_primaryAlg->count(); i++) {
#if QT_VERSION >= 0x040000
    this->ip_primaryAlg->setCurrentIndex(i);
#else
    this->ip_primaryAlg->setCurrentItem(i);
#endif // QT_VERSION >= 0x040000
    if(alg == this->ip_primaryAlg->currentText()) {
      this->algorithmChanged(i);
      break;
    }
  }
  this->ip_maxInstr->setValue(optionMap[ "MAXINSTR" ].toInt());
#ifdef VICE
  this->ip_maxTime->setValue(optionMap[ "MAXTIME" ].toInt());
  this->ip_stepSize->setValue(optionMap[ "STEPSIZE" ].toInt());
  this->ip_cpuCapacity->setValue(optionMap[ "DEFAULTCPUCAPACITY" ].toInt());

  if(optionMap[ "DEFAULTVCPUCAPACITY" ] == "<INFINITE>") {
    this->ip_vcpuCapacitycb->setChecked(false);
  }
  else {
    this->ip_vcpuCapacitycb->setChecked(true);
    this->ip_vcpuCapacity->setValue(optionMap[ "DEFAULTVCPUCAPACITY" ].toInt());  
  }

  this->vcpuCapacityEnabled();

  QString jmode (optionMap[ "JITTERMODE" ]);
  for(int j = 0; j < this->ip_jitterMode->count(); j++) {
#if QT_VERSION >= 0x040000
    this->ip_jitterMode->setCurrentIndex(j);
#else
    this->ip_jitterMode->setCurrentItem(j);
#endif // QT_VERSION >= 0x040000
    if(jmode == this->ip_jitterMode->currentText()) {
      break;
    }
  }
  this->ip_selectedOpsList->clear();
  QString logargs (optionMap[ "LOGARGS" ]);
  if (logargs == "<ALL>") {
    this->ip_logArgsAllCheck->setChecked(true);
  }
  else {
    this->ip_logArgsAllCheck->setChecked(false);
#if QT_VERSION >= 0x040000
    QStringList list (logargs.split( ',' ));
#else
    QStringList list (QStringList::split( ',', logargs ));
#endif // QT_VERSION >= 0x040000
    for( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
#if QT_VERSION >= 0x040000
      this->ip_selectedOpsList->addItem(*it);
#else
      this->ip_selectedOpsList->insertItem(*it);
#endif // QT_VERSION >= 0x040000
    }
  }
  this->loadClassName();
#endif // VICE
#endif //VDMPP
  this->ip_ppValues->setChecked(String2Bool(optionMap[ "PRINT_FORMAT" ]));
  this->ip_exception->setChecked(String2Bool(optionMap[ "RTERR_EXCEPTION" ]));
  this->ip_rndGen->setValue(optionMap[ "Seed_nondetstmt" ].toInt());
  this->ip_expression->setText(optionMap[ "EXPRESSION" ]);
  this->ip_oldreverse->setChecked(String2Bool(optionMap[ "OLD_REVERSE" ]));
  
  // Type checker options
  this->tc_posTc->setChecked(optionMap[ "DEF" ] != "def");
  this->tc_defTc->setChecked(optionMap[ "DEF" ] == "def");
  this->tc_extTc->setChecked(String2Bool(optionMap[ "errlevel" ]));
  this->tc_msgSeparation->setChecked(String2Bool(optionMap[ "SEP" ]));
#ifdef VDMSL
  this->tc_standardVDMSL->setChecked(String2Bool(optionMap[ "VDMSLMOD" ]));
#endif // VDMSL
  this->tc_vdm10->setChecked(String2Bool(optionMap[ "VDM10" ]));
  
  // Pretty printer options
  int index = optionMap[ "INDEX" ].toInt();
  this->pp_noIndex->setChecked(index == 0);
  this->pp_defIndex->setChecked(index == 1);
  this->pp_useIndex->setChecked(index == 2);
  this->pp_tcovCol->setChecked(String2Bool(optionMap[ "PrettyPrint_RTI" ]));
  
  // C++ Code generator options
  this->cpp_posInfo->setChecked(String2Bool(optionMap[ "CG_RTI" ]));
#ifdef VDMPP  
  this->cpp_checkConds->setChecked(String2Bool(optionMap[ "CG_CHECKPREPOST" ]));
#endif // VDMPP

#ifdef VDMPP  
  // Java code generator options
  this->jcg_skeletonsOnly->setChecked(String2Bool(optionMap[ "JCG_SKEL" ]));
  this->jcg_typesOnly->setChecked(String2Bool(optionMap[ "JCG_TYPES" ]));
  this->jcg_useLongs->setChecked(String2Bool(optionMap[ "JCG_LONGS" ]));
  this->jcg_genConc->setChecked(String2Bool(optionMap[ "JCG_CONCUR" ]));
  this->jcg_genConds->setChecked(String2Bool(optionMap[ "JCG_GENPREPOST" ]));
  this->jcg_checkConds->setChecked(String2Bool(optionMap[ "JCG_CHECKPREPOST" ]));
  this->jcg_packageName->setText(optionMap[ "JCG_PACKAGE" ]);
  this->jcg_vdmPrefix->setChecked(!String2Bool(optionMap[ "JCG_VDMPREFIX" ]));
  this->jcg_useOutputDir->setChecked(String2Bool(optionMap[ "JCG_USEDIRNAME" ]));
  this->jcg_outputDirName->setText(optionMap[ "JCG_DIRNAME" ]);
  this->jcg_needBackup->setChecked(String2Bool(optionMap[ "JCG_NEEDBACKUP" ]));

  if (this->jcg_useOutputDir->isChecked()) {
    QDir dir (this->jcg_outputDirName->text());
    if (!dir.exists()) {
      this->jcg_useOutputDir->setChecked(false);
      this->jcg_outputDirName->setText("");
    }
  }
#endif //VDMPP
  
  // Java2VDM options
  //  j2v_stubsOnly->setChecked();
  //  j2v_autoRenaming->setChecked();
  //  j2v_genAccessors->setChecked();

  this->invClicked();
}

void optionsW::putOptions()
{
  QMap<QString, QString> optionMap;
  optionMap[ "DTC" ] = (this->ip_dynTypeCheck->isChecked() ? "1" : "0");
  optionMap[ "INV" ] = (this->ip_dynInvCheck->isChecked() ? "1" : "0");
  optionMap[ "PRE" ] = (this->ip_preCheck->isChecked() ? "1" : "0");
  optionMap[ "POST" ] = (this->ip_postCheck->isChecked() ? "1" : "0");
  optionMap[ "MEASURE" ] = (this->ip_measureCheck->isChecked() ? "1" : "0");

#ifdef VDMPP
  optionMap[ "PRIORITY" ] = (this->ip_prioritySchd->isChecked() ? "1" : "0");
  optionMap[ "PRIMARYALGORITHM" ] = this->ip_primaryAlg->currentText();
  optionMap[ "MAXINSTR" ] = QString::number(this->ip_maxInstr->value());

#ifdef VICE
  optionMap[ "MAXTIME" ] = QString::number(this->ip_maxTime->value());
  optionMap[ "STEPSIZE" ] = QString::number(this->ip_stepSize->value());
  optionMap[ "DEFAULTCPUCAPACITY" ] = QString::number(this->ip_cpuCapacity->value());

  if( this->ip_vcpuCapacitycb->isChecked() ) {
    optionMap[ "DEFAULTVCPUCAPACITY" ] = QString::number(this->ip_vcpuCapacity->value());
  }
  else {
    optionMap[ "DEFAULTVCPUCAPACITY" ] = "<INFINITE>";
  }

  optionMap[ "JITTERMODE" ] = this->ip_jitterMode->currentText();

  if( this->ip_logArgsAllCheck->isChecked() ) {
    optionMap[ "LOGARGS" ] = "<ALL>";
  }
  else {
    int len = this->ip_selectedOpsList->count();
    QString str;
    for(int i = 0; i < len; i++) {
      if( i > 0 ) str += ",";
#if QT_VERSION >= 0x040000
      QListWidgetItem * item = this->ip_selectedOpsList->item(i);
      str += item->text();
#else
      str += this->ip_selectedOpsList->text(i);
#endif // QT_VERSION >= 0x040000
    }
    optionMap[ "LOGARGS" ] = str;
  }
#endif // VICE
#endif //VDMPP
  optionMap[ "PRINT_FORMAT" ] = (this->ip_ppValues->isChecked() ? "1" : "0");
  optionMap[ "RTERR_EXCEPTION" ] = (this->ip_exception->isChecked() ? "1" : "0");
  optionMap[ "EXPRESSION" ] = this->ip_expression->text();
  optionMap[ "Seed_nondetstmt" ] = QString::number(this->ip_rndGen->value());
  optionMap[ "OLD_REVERSE" ] = (this->ip_oldreverse->isChecked() ? "1" : "0");

  optionMap[ "DEF" ] = (this->tc_defTc->isChecked() ? "def" : "pos");
  optionMap[ "errlevel" ] = (this->tc_extTc->isChecked() ? "1" : "0");
  optionMap[ "SEP" ] = (this->tc_msgSeparation->isChecked() ? "1" : "0");

#ifdef VDMSL 
  optionMap[ "VDMSLMOD" ] = (this->tc_standardVDMSL->isChecked() ? "1" : "0");
#endif // VDMSL

  optionMap[ "VDM10" ] = (this->tc_vdm10->isChecked() ? "1" : "0");
  
  // Pretty printer options
  if (this->pp_useIndex->isChecked())  {
    optionMap[ "INDEX" ] = "2";
  }
  else if (this->pp_defIndex->isChecked()) {
    optionMap[ "INDEX" ] = "1";
  }
  else {
    optionMap[ "INDEX" ] = "0";
  }
  optionMap[ "PrettyPrint_RTI" ] = (this->pp_tcovCol->isChecked() ? "1" : "0");
  
  // C++ Code generator options
  optionMap[ "CG_RTI" ] = (this->cpp_posInfo->isChecked() ? "1" : "0");
#ifdef VDMPP
  optionMap[ "CG_CHECKPREPOST" ] = (this->cpp_checkConds->isChecked() ? "1" : "0");
#endif // VDMPP

#ifdef VDMPP  
  // Java code generator options
  optionMap[ "JCG_SKEL" ] = (this->jcg_skeletonsOnly->isChecked() ? "1" : "0");
  optionMap[ "JCG_TYPES" ] = (this->jcg_typesOnly->isChecked() ? "1" : "0");
  optionMap[ "JCG_LONGS" ] = (this->jcg_useLongs->isChecked() ? "1" : "0");
  optionMap[ "JCG_CONCUR" ] = (this->jcg_genConc->isChecked() ? "1" : "0");
  optionMap[ "JCG_GENPREPOST" ] = (this->jcg_genConds->isChecked() ? "1" : "0");
  optionMap[ "JCG_CHECKPREPOST" ] = (this->jcg_checkConds->isChecked() ? "1" : "0");
  optionMap[ "JCG_PACKAGE" ] = this->jcg_packageName->text();
  optionMap[ "JCG_VDMPREFIX" ] = (!this->jcg_vdmPrefix->isChecked() ? "1" : "0");
  optionMap[ "JCG_USEDIRNAME" ] = (this->jcg_useOutputDir->isChecked() ? "1" : "0");
  optionMap[ "JCG_DIRNAME" ] = this->jcg_outputDirName->text();
  optionMap[ "JCG_NEEDBACKUP" ] = (this->jcg_needBackup->isChecked() ? "1" : "0");
#endif //VDMPP  

  Qt2TB::SetOptions(optionMap);

  // Java2VDM options

  //  j2v_stubsOnly->setChecked();
  //  j2v_autoRenaming->setChecked();
  //  j2v_genAccessors->setChecked();
}

#ifdef VDMPP
bool optionsW::get_j2v_stubsOnly()
{
  return j2v_stubsOnly->isChecked();
}

/*
bool optionsW::get_j2v_autoRenaming()
{
  return j2v_autoRenaming->isChecked();
}
*/

bool optionsW::get_j2v_transforms()
{
  return j2v_transforms->isChecked();
}

void optionsW::clearJ2VOptions()
{
  this->j2v_stubsOnly->setChecked(false);
  this->j2v_transforms->setChecked(false);
}
#endif //VDMPP

QString optionsW::getOptionFileName(const QString & prj)
{
  QString nm = prj;
  if (nm.right(4) != ".prj") return QString();
  nm.replace( ".prj", ".opt" );
  return nm;
}

void optionsW::loadOptions()
{
#ifdef VDMPP
  this->clearJ2VOptions();
#endif // VDMPP
  QString filenm( getOptionFileName(Qt2TB::getProjectNameI()) );

  if( !filenm.isEmpty() ) {
    this->mainw->logWrite(QString("loadOptions for ") + filenm);

    QFile optionFile( filenm );
#if QT_VERSION >= 0x040000
    if( optionFile.open(QIODevice::ReadOnly) ) {
#else
    if( optionFile.open(IO_ReadOnly) ) {
#endif // QT_VERSION >= 0x040000

      QTextStream optionStream(&optionFile);
      QString version = optionStream.readLine();
      optionFile.close();

      if( version == "FormatVersion:2" ) {
        this->loadOptionsV2();
      }
      else {
        this->loadOptionsV1();
      }
#if QT_VERSION >= 0x040000
      this->maintab->setCurrentIndex(0);
#else
      this->maintab->setCurrentPage(0);
#endif // QT_VERSION >= 0x040000
      return;
    }
  }
  Qt2TB::InitOptions();
}

bool optionsW::String2Bool(const QString & str)
{
  if( str == "0" || str == "false" || str == "off" ) {
    return false;
  }
  else if( str == "1" || str == "true" || str == "on" ) {
    return true;
  }
  return false;
}

bool optionsW::optionFileExists()
{
  QString filenm( getOptionFileName(Qt2TB::getProjectNameI()) );
  if( !filenm.isEmpty() ) {
    QFile optionFile( filenm );
#if QT_VERSION >= 0x040000
    if( optionFile.open(QIODevice::ReadOnly) ) {
#else
    if( optionFile.open(IO_ReadOnly) ) {
#endif // QT_VERSION >= 0x040000
      optionFile.close();
      return true;
    }
  }
  return false;
}

void optionsW::loadOptionsV1()
{
  QString filenm( getOptionFileName(Qt2TB::getProjectNameI()) );
  if( !filenm.isEmpty() ) {

    QFile optionFile( filenm );
#if QT_VERSION >= 0x040000
    if( optionFile.open(QIODevice::ReadOnly) ) {
#else
    if( optionFile.open(IO_ReadOnly) ) {
#endif // QT_VERSION >= 0x040000
      QTextStream optionStream(&optionFile);

      QMap<QString, QString> optionMap;
      optionMap[ "DTC" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "PRE" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "POST" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "INV" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "SEP" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "CONTEXT" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");

#ifdef VDMPP
      optionMap[ "MAXINSTR" ] = (optionStream.readLine());
      optionMap[ "PRIORITY" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
#ifdef VICE
      optionMap[ "PRIMARYALGORITHM" ] = (optionStream.readLine());
      optionMap[ "TASKSWITCH" ] = (optionStream.readLine());
      optionMap[ "MAXTIME" ] = (optionStream.readLine());
      optionMap[ "TIMEFACTOR" ] = (optionStream.readLine());
#endif //VICE
#endif //VDMPP

      optionMap[ "VDMSLMODE" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "errlevel" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "PRINT_FORMAT" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "RTERR_EXCEPTION" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "CG_RTI" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "CG_CHECKPREPOST" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");

      QString tmp;
      tmp = optionStream.readLine();
      optionMap[ "DEF" ] = ((tmp == "1" || tmp == "def") ? "def" : "pos");

      optionMap[ "INDEX" ] = (optionStream.readLine());
      optionMap[ "PrettyPrint_RTI" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "C_flag" ] = (optionStream.readLine());
      optionMap[ "JCG_SKEL" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_GENPREPOST" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_TYPES" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_SMALLTYPES" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_LONGS" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_PACKAGE" ] = (optionStream.readLine());
      optionMap[ "JCG_CONCUR" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_CHECKPREPOST" ] = (String2Bool(optionStream.readLine()) ? "1" : "0");
      optionMap[ "JCG_INTERFACES" ] = (optionStream.readLine());
      optionMap[ "Seed_nondetstmt" ] = (optionStream.readLine());

      Qt2TB::SetOptions(optionMap);

#ifdef VDMPP
      j2v_stubsOnly->setChecked(String2Bool(optionStream.readLine()));
      j2v_transforms->setChecked(String2Bool(optionStream.readLine()));
#endif // VDMPP
  
      optionFile.close();
    }
  }
}

void optionsW::loadOptionsV2()
{
  QString filenm( getOptionFileName(Qt2TB::getProjectNameI()) );
  if( !filenm.isEmpty() ) {
    QMap<QString, QString> optionMap (readOption2(filenm));;
#if QT_VERSION >= 0x040000
    if (!optionMap.empty()) {
#else
    if (!optionMap.isEmpty()) {
#endif // QT_VERSION >= 0x040000

#ifdef VDMPP
      if( optionMap.contains( "FVStatic" ) && !optionMap.contains( "VDM10" ) ) {
        optionMap["VDM10"] == optionMap["FVStatic"];
      }
#endif // VDMPP

      Qt2TB::SetOptions(optionMap);

#ifdef VDMPP
      if( optionMap.contains( "j2v_stubsOnly" ) ) {
        j2v_stubsOnly->setChecked(optionMap["j2v_stubsOnly"] == "1");
      }
      if( optionMap.contains( "j2v_transforms" ) ) {
        j2v_transforms->setChecked(optionMap["j2v_transforms"] == "1");
      }
#endif // VDMPP
    }
  }
}

QMap<QString, QString> optionsW::readOption2(const QString & filename)
{
  QMap<QString, QString> optionMap;

  QFile optionFile( filename );
#if QT_VERSION >= 0x040000
  if( optionFile.open(QIODevice::ReadOnly) ) {
#else
  if( optionFile.open(IO_ReadOnly) ) {
#endif // QT_VERSION >= 0x040000
    QTextStream optionStream(&optionFile);

    while( !optionStream.atEnd() ) {
      QString tmp = optionStream.readLine();
      if( !tmp.isEmpty() ) {
#if QT_VERSION >= 0x040000
        int index = tmp.indexOf( ':' ); 
        if( index != -1 ) {
          QString key = tmp.left( index ).simplified();
          QString value = tmp.right( tmp.length() - index - 1 ).simplified();
          optionMap[ key ] = value;
        }
#else
        int index = tmp.find( ':' ); 
        if( index != -1 ) {
          QString key = tmp.left( index ).stripWhiteSpace();
          QString value = tmp.right( tmp.length() - index - 1 ).stripWhiteSpace();
          optionMap[ key ] = value;
        }
#endif // QT_VERSION >= 0x040000
      }
    }
    optionFile.close();
  }
  return optionMap;
}

void optionsW::saveOptions()
{
  if( this->saveOptionsFlag->isChecked() ) {

    QString filenm( getOptionFileName(Qt2TB::getProjectNameI()) );
    if( !filenm.isEmpty() ) {
      QFile optionFile( filenm );
#if QT_VERSION >= 0x040000
      if( optionFile.open(QIODevice::WriteOnly) ) {
#else
      if( optionFile.open(IO_WriteOnly) ) {
#endif // QT_VERSION >= 0x040000
        QTextStream optionStream(&optionFile);

        optionStream << "FormatVersion:2" << endl;

        QMap<QString, QString> optionMap (Qt2TB::GetOptions());
#if QT_VERSION >= 0x040000
        QList<QString> keys (optionMap.keys());
        for (QList<QString>::const_iterator it = keys.begin();it != keys.end();++it) {
#else
        QValueList<QString> keys (optionMap.keys());
        for (QValueList<QString>::const_iterator it = keys.begin();it != keys.end();++it) {
#endif // QT_VERSION >= 0x040000
          optionStream << *it << ":" << optionMap[*it] << endl;
        }

#ifdef VDMPP
        optionStream << "j2v_stubsOnly:" << ( j2v_stubsOnly->isChecked() ? "1" : "0" ) << endl;
        optionStream << "j2v_transforms:" << ( j2v_transforms->isChecked() ? "1" : "0" ) << endl;
#endif // VDMPP
  
        optionStream << "TextCodecName:"  << this->currentCodecName << endl;

        optionFile.close();
      }
    }
  }
}

void optionsW::algorithmChanged(int)
{
#ifdef VDMPP
  QString alg (this->ip_primaryAlg->currentText());
  if( alg == "instruction_number_slice" ) {
    this->ip_maxInstr->setEnabled(true);
    this->ip_maxInstrLabel->setEnabled(true);
#ifdef VICE
    this->ip_maxTime->setEnabled(false);
    this->ip_maxTimeLabel->setEnabled(false);
#endif // VICE
  }
#ifdef VICE
  else if( alg == "timeslice" ) {
    this->ip_maxInstr->setEnabled(false);
    this->ip_maxInstrLabel->setEnabled(false);
    this->ip_maxTime->setEnabled(true);
    this->ip_maxTimeLabel->setEnabled(true);
  }
#endif // VICE
  else {
    this->ip_maxInstr->setEnabled(false);
    this->ip_maxInstrLabel->setEnabled(false);
#ifdef VICE
    this->ip_maxTime->setEnabled(false);
    this->ip_maxTimeLabel->setEnabled(false);
#endif // VICE
  }
#endif // VDMPP
}

void optionsW::vcpuCapacityEnabled()
{
#ifdef VICE
  if (this->ip_vcpuCapacitycb->isChecked()) {
    this->ip_vcpuCapacityLabel->setEnabled(true);
    this->ip_vcpuCapacity->setEnabled(true);
  }
  else {
    this->ip_vcpuCapacityLabel->setEnabled(false);
    this->ip_vcpuCapacity->setEnabled(false);
  }
#endif // VICE
}

#ifdef VICE
void optionsW::loadClassName()
{
  QStringList clist (Qt2TB::getModulesI());

  this->ip_className->clear();
  for (QStringList::const_iterator it= clist.begin();it != clist.end();++it) {
#if QT_VERSION >= 0x040000
    this->ip_className->addItem(*it);
#else
    this->ip_className->insertItem(*it);
#endif // QT_VERSION >= 0x040000
  }
#if QT_VERSION >= 0x040000
  this->ip_className->setCurrentIndex(0);
#else
  this->ip_className->setCurrentItem(0);
#endif // QT_VERSION >= 0x040000
  this->classChanged(0);
}
#endif // VICE

void optionsW::logArgsAllEnabled()
{
#ifdef VICE
  if(this->ip_logArgsAllCheck->isChecked()) {
    this->ip_selectedOpsList->setEnabled(false);
    this->ip_classNameLabel->setEnabled(false);
    this->ip_className->setEnabled(false);
    this->ip_opNameLabel->setEnabled(false);
    this->ip_opName->setEnabled(false);
    this->ip_opAddButton->setEnabled(false);
    this->ip_opDelButton->setEnabled(false);
  }
  else {
    this->ip_selectedOpsList->setEnabled(true);
    this->ip_classNameLabel->setEnabled(true);
    this->ip_className->setEnabled(true);
    this->ip_opNameLabel->setEnabled(true);
    this->ip_opName->setEnabled(true);
    this->ip_opAddButton->setEnabled(true);
    this->ip_opDelButton->setEnabled(true);
  }
#endif // VICE
}

void optionsW::classChanged(int i)
{
#ifdef VICE
  this->ip_opName->clear();

  QString clsnm (this->ip_className->currentText()); 
  if( !clsnm.isEmpty() ) {
    QStringList oplist;
    Qt2TB::setBlock(false);
    int res = Qt2TB::getOperationsI(clsnm, oplist);
    Qt2TB::setBlock(true);

    if( res == INT_OK ) {
      for (QStringList::const_iterator it= oplist.begin();it != oplist.end();++it) {
#if QT_VERSION >= 0x040000
        this->ip_opName->addItem(*it);
#else
        this->ip_opName->insertItem(*it);
#endif // QT_VERSION >= 0x040000
      }
#if QT_VERSION >= 0x040000
      this->ip_opName->setCurrentIndex(0);
#else
      this->ip_opName->setCurrentItem(0);
#endif// QT_VERSION >= 0x040000
    }
    else {
      this->mainw->logWrite("Specification is not initialized.");
    }
  }
#endif // VICE
}

void optionsW::addOperation()
{
#ifdef VICE
  QString clsnm (this->ip_className->currentText());
  QString opnm (this->ip_opName->currentText());
  if( !clsnm.isEmpty() && !opnm.isEmpty() ) {

  QString fullnm (clsnm + "`" + opnm);
#if QT_VERSION >= 0x040000
    if( (this->ip_selectedOpsList->findItems(fullnm, 0)).empty() ) {
      this->ip_selectedOpsList->addItem(fullnm);
    }
#else
    if( this->ip_selectedOpsList->findItem(fullnm) == 0 ) {
      this->ip_selectedOpsList->insertItem(fullnm);
    }
#endif // QT_VERSION >= 0x040000
  }
#endif // VICE
}

void optionsW::delOperation()
{
#ifdef VICE
#if QT_VERSION >= 0x040000
  int ci = this->ip_selectedOpsList->currentRow();
  QListWidgetItem * item = this->ip_selectedOpsList->item(ci);
  if( item->isSelected() ) {
    this->ip_selectedOpsList->removeItemWidget(item);
  } 
#else
  int ci = this->ip_selectedOpsList->currentItem();
  if( this->ip_selectedOpsList->isSelected( ci ) ) {
    this->ip_selectedOpsList->removeItem(ci);
  } 
#endif // QT_VERSION >= 0x040000
#endif // VICE
}

void optionsW::invClicked()
{
  if(this->ip_dynInvCheck->isChecked()) {
    this->ip_dynTypeCheck->setChecked(true);
    this->ip_dynTypeCheck->setEnabled(false);
  }
  else {
    this->ip_dynTypeCheck->setEnabled(true);
  }
}

void optionsW::selectJCGDir()
{
#ifdef VDMPP
  QString res (QtPort::QtGetExistingDirectory( this, tr("Choose a java code directory"), QString()));
  if( !res.isNull() ) {
    this->jcg_outputDirName->setText(res);
  }
#endif // VDMPP
}

void optionsW::initTab()
{
#if QT_VERSION >= 0x040000
  this->maintab->setCurrentIndex(0);
#else
  this->maintab->setCurrentPage(0);
#endif // QT_VERSION >= 0x040000
}

QString optionsW::getCodecName(const QString & prj)
{
  QString ofn (getOptionFileName(prj));
  QMap<QString, QString> optionMap (readOption2(ofn));
  if( optionMap.contains( "TextCodecName" ) ) {
    return optionMap["TextCodecName"];
  }
  else {
    return QString("");
  }
}

QString optionsW::getExpression()
{
  QString filenm( getOptionFileName(Qt2TB::getProjectNameI()) );
  if( !filenm.isEmpty() ) {
    QMap<QString, QString> optionMap (readOption2(filenm));;
    if( optionMap.contains( "EXPRESSION" ) ) {
      return optionMap["EXPRESSION"];
    }
  }
  return QString("");
}

