/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/toolOptionsDialog.cc,v $
*  * VERSION
*  *    $Revision: 1.30 $
*  * DATE
*  *    $Date: 2006/06/12 08:36:17 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "toolOptionsDialog.h"
#include "mainF.h"

#if QT_VERSION < 0x040000
#include <qfile.h>
#include <qtextstream.h>
#include <qfontdialog.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qtextcodec.h>
#endif // QT_VERSION < 0x040000

#include "localeutil.h"
#include "qtport.h"
#include "tb_wstring.h"

#ifdef VDMPP
#include "uml-be.h"
#endif // VDMPP

#ifdef __linux__
#include <stdlib.h>
#endif // __linux__

#ifdef _MSC_VER
#define HOME_ENV_NAME "USERPROFILE"
#else
#define HOME_ENV_NAME "HOME"
#endif // _MSC_VER

#ifdef VDMPP
#define UML_JUDE_XMI "JUDE (XMI)"      // obsolite
#define UML_ASTAH_XMI "astah* (XMI)"   // obsolite
#define UML_ASTAH_PRO_XMI "astah* pro (XMI)"
#define UML_EA_XMI   "Enterprise Architect (XMI)"
#define UML_Rose_OLE "Rose (OLE)"

static const char* umlInterfaces[] = {
  UML_ASTAH_PRO_XMI,
  UML_EA_XMI,
#ifdef _MSC_VER
#ifdef _USE_ROSE
  UML_Rose_OLE,
#endif // _USE_ROSE
#endif // _MSC_VER
  ""
};

#define UML_FTYPE_VDM "VDM++ (*.vpp)"
#define UML_FTYPE_RTF "RTF (*.rtf)"
#define UML_FTYPE_TEX "LATEX (*.tex)"

static const char* umlNewFileTypes[] = {
  UML_FTYPE_VDM,
  UML_FTYPE_RTF,
//  UML_FTYPE_TEX,
  ""
};
#endif // VDMPP

/* 
 *  Constructs a toolOptionsDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
toolOptionsDialog::toolOptionsDialog( QWidget* parent,  const char* name, 
                                      bool modal, WFlags fl )
#if QT_VERSION >= 0x040000
  : QDialog( parent, fl ),
#else
  : QDialog( parent, name, modal, fl ),
#endif // QT_VERSION >= 0x040000
    currentFont(QApplication::font()),
    initFileName(".vdmini"),
    singleLoadFormat(""),
    multiLoadFormat(""),
    printCommand(""),
    currentTextCodec (0),
    useExternalEditor(false),
    multiFilesSupported(false)
{
  // Initialise member vars

  this->setWindowName( (name == NULL) ? "toolOptionsDialog" : name );

#if QT_VERSION >= 0x040000
  this->setModal( modal );
  this->setWindowTitle( tr( "Tool Options" ) );
#else
  // Create layout
  if ( NULL != name ) setName( "toolOptionsDialog" );
  this->setCaption( tr( "Tool Options" ) );
#endif // QT_VERSION >= 0x040000

  this->setSizeGripEnabled( true );

  // Create Window Layout
  QVBoxLayout* layout = this->createVBoxLayout( this );
  layout->setMargin( 11 );
  layout->addWidget( this->createTabPart( this ) );
  layout->addLayout( this->createButtonPart( this ) );

  this->syntaxColoring = true;
  this->autoSyntaxChecking = true;

#if QT_VERSION >= 0x040000
  int fontSize = 11;
  QFont font = QFont("monospace", fontSize);
  font.setStyleHint(QFont::TypeWriter);
  int fontPxSize = QFontMetrics(font).width('0');
  this->currentFont = font;
  this->selectedFont = font;
#endif // QT_VERSION >= 0x040000

#if QT_VERSION >= 0x040000
  this->currentStyle = QApplication::style()->objectName();
#else
  this->currentStyle = QApplication::style().name();
#endif // QT_VERSION >= 0x040000

#ifdef VDMPP
  this->useNewFile = true;
  mainW * mw = (mainW *)parent;
  this->newFileName = mw->getUMLLibrary() + "/NewClass.rtf";
  this->umlinterface = 0;
  this->umlnewfiletype = 0;
#endif // VDMPP

  // set editor
  const char* edName = getenv("EDITOR");
  if ( NULL != edName ) {
    this->editorName = edName;
    this->useExternalEditor = true;
  }

  // Load ini file
/*
  if( !this->loadOptions() )
  {
    const char* edName = getenv("EDITOR");
    if ( NULL != edName )
    {
      this->editorName = edName;
      this->useExternalEditor = true;
    }
  }
*/
  this->loadOptions();
  this->resize( this->sizeHint() );
}

QSpacerItem * toolOptionsDialog::createSpacer()
{
  return new QSpacerItem( 20, 20,
                          QSizePolicy::Minimum,
                          QSizePolicy::Expanding );
}

QFrame * toolOptionsDialog::createFrame( QWidget* parent )
{
  QFrame* frame = new QFrame( parent );
  frame->setFrameShape( QFrame::StyledPanel );
  frame->setFrameShadow( QFrame::Raised );
  return frame;
}

QVBoxLayout* toolOptionsDialog::createVBoxFrameLayout( QFrame* frame )
{
  QVBoxLayout* layout = this->createVBoxLayout( frame );
#if QT_VERSION >= 0x040000
#else
  layout->setMargin( 6 );
#endif // QT_VERSION >= 0x040000
  layout->setAlignment( Qt::AlignTop );
  return layout;
}

QVBoxLayout* toolOptionsDialog::createVBoxLayout( QWidget* parent )
{
  QVBoxLayout* layout = new QVBoxLayout( parent );
  layout->setSpacing( 6 );
  return layout;
}

QHBoxLayout* toolOptionsDialog::createHBoxLayout( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout( parent );
  layout->setSpacing( 6 );
  return layout;
}

QGroupBox * toolOptionsDialog::createVGroupBox( QWidget* parent )
{
  QGroupBox* gbox = new QGroupBox( parent );
#if QT_VERSION >= 0x040000
  QVBoxLayout* layout = this->createVBoxLayout( NULL );
  gbox->setLayout( layout );
#else
  gbox->setOrientation( Qt::Vertical );
  QLayout* layout = gbox->layout();
  layout->setSpacing(6);
  layout->setMargin(11);
#endif // QT_VERSION >= 0x040000
  return gbox;
}

QWidget* toolOptionsDialog::createTabPart( QWidget* parent )
{
  QTabWidget* tw = new QTabWidget( parent );
#if QT_VERSION >= 0x040000
  tw->setTabPosition( QTabWidget::North );
#else
  tw->setTabPosition( QTabWidget::Top );
#endif // QT_VERSION >= 0x040000
  tw->setTabShape( QTabWidget::Rounded );

#if QT_VERSION >= 0x040000
  tw->addTab( this->createFontTab( tw ), tr( "General" ) );
  tw->addTab( this->createEditAndPrintTab( tw ), tr( "Edit and Print" ) );
#ifdef VDMPP
  tw->addTab( this->createUMLTab( tw ), tr( "UML Link" ) );
#endif // VDMPP
#else
  tw->insertTab( this->createFontTab( tw ), tr( "General" ) );
  tw->insertTab( this->createEditAndPrintTab( tw ), tr( "Edit and Print" ) );
#ifdef VDMPP
  tw->insertTab( this->createUMLTab( tw ), tr( "UML Link" ) );
#endif // VDMPP
#endif // QT_VERSION >= 0x040000

  return tw;
}

QLayout* toolOptionsDialog::createButtonPart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  layout->addWidget( this->createSaveToolOptionsCheckbox( parent ) );

  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  layout->addItem( spacer );

  layout->addWidget( this->createCancelButton( parent ) );
  layout->addWidget( this->createApplyButton( parent ) );
  layout->addWidget( this->createOkButton( parent ) );

  return layout;
}

QWidget* toolOptionsDialog::createCancelButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&Cancel" )) );
  button->setAutoDefault( true );
  connect( button, SIGNAL( clicked() ), this, SLOT( reject() ) );
  return button;
}

QWidget* toolOptionsDialog::createApplyButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&Apply" )) );
  button->setAutoDefault( true );
  connect( button, SIGNAL( clicked()), this, SLOT(applyDialog()));
  return button;
}

QWidget* toolOptionsDialog::createOkButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( mainW::mf(tr( "&OK" )) );
  button->setAutoDefault( true );
  button->setDefault( true );
  connect( button, SIGNAL( clicked() ), this, SLOT( accept() ) );
  return button;
}

QWidget* toolOptionsDialog::createEditAndPrintTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab );

  layout->addWidget( this->createEditAndPrintFrame( tab ) );

  return tab;
}

QWidget* toolOptionsDialog::createEditAndPrintFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame );

  layout->addWidget( this->createEditGroupBox( frame ) );
  layout->addWidget( this->createPrintGroupBox( frame ) );
  layout->addItem( this->createSpacer() );

  return frame;
}

QWidget* toolOptionsDialog::createEditGroupBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("External Editor") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createUseExternalEditorCheckBox( gbox ) );
  layout->addItem( this->createEditorNamePart( gbox ) );
  layout->addItem( this->createSingleLoadFormatPart( gbox ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->createMultiFileSupportedCheckBox( gbox ) );
#else
  layout->add( this->createMultiFileSupportedCheckBox( gbox ) );
#endif // QT_VERSION >= 0x040000
  layout->addItem( this->createMultiLoadFormatPart( gbox ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->createLoadFormatHelp( gbox ) );
#else
  layout->add( this->createLoadFormatHelp( gbox ) );
#endif // QT_VERSION >= 0x040000

  QObject::connect( this->if_useExternalEditor,   SIGNAL(clicked()), this, SLOT(editerNameEnabled()));
  QObject::connect( this->if_multiFilesSupported, SIGNAL(clicked()), this, SLOT(multiLoadEnabled()));

  return gbox;
}

QWidget* toolOptionsDialog::createPrintGroupBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Print" ) );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createPrintCommandPart( gbox ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->createPrintCommendHelp( gbox ) );
#else
  layout->add( this->createPrintCommendHelp( gbox ) );
#endif // QT_VERSION >= 0x040000

  return gbox;
}

QLayout* toolOptionsDialog::createUseExternalEditorCheckBox( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Use external editor" ) );
  this->if_useExternalEditor = checkbox; 

  layout->addWidget( checkbox );

  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Select Editor" ) );
  connect( button, SIGNAL(clicked()), this, SLOT(selectEditor()));
  this->if_selectEditor = button;
  layout->addWidget( button );

  return layout;
}

QLayout* toolOptionsDialog::createEditorNamePart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Editor Name: " ) );
  layout->addWidget( label );

  this->if_editorNameLabel = label;

  QLineEdit* le = new QLineEdit( parent );

  /*
  QFrame* line = new QFrame( le, "Line1" );
  line->setGeometry( QRect( 26, 7, 2, 3 ) ); 
  line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  */
  layout->addWidget( le );
  this->if_editorName = le;

  return layout;
}

QLayout* toolOptionsDialog::createSingleLoadFormatPart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 
  
  QLabel* label = new QLabel( parent );
  label->setText( tr( "Format to load single file:" ) );
  layout->addWidget( label );

  this->if_singleLoadFormatLabel = label;

  QLineEdit* le = new QLineEdit( parent );
  layout->addWidget( le );
  this->if_singleLoadFormat = le;

  return layout;
}

QWidget* toolOptionsDialog::createMultiFileSupportedCheckBox( QWidget* parent )
{
  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Editor suports multiple files in one editor" ) );
  this->if_multiFilesSupported = checkbox;
  this->if_multiFilesSupported->setChecked(false);
  this->if_multiFilesSupported->setEnabled( false );
  return checkbox;
}

QLayout* toolOptionsDialog::createMultiLoadFormatPart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Format to load multiple files:" ) );
  layout->addWidget( label );

  this->if_multiLoadFormatLabel = label;

  QLineEdit* le = new QLineEdit( parent );
  layout->addWidget( le );
  this->if_multiLoadFormat = le;

  return layout;
}

QWidget* toolOptionsDialog::createLoadFormatHelp( QWidget* parent )
{
  QLabel* label = new QLabel( parent );
  label->setText( tr( "(%f expands to filename; %l expands to line number)" ) );
  this->if_loadFormatHelp = label;
  return label;
}

QLayout* toolOptionsDialog::createPrintCommandPart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Print command:" ) );
  layout->addWidget( label );

  QLineEdit* le = new QLineEdit( parent );
  layout->addWidget( le );
  this->if_printCommand = le;

  return layout;
}

QWidget* toolOptionsDialog::createPrintCommendHelp( QWidget* parent )
{
  QLabel* label = new QLabel( parent );
  label->setText( tr( "(%f name of the frame; %d current date; %t current time)" ) );
  return label;
}

QWidget* toolOptionsDialog::createFontTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab );

  layout->addWidget( this->createFontFrame( tab ) );

  return tab;
}

QWidget* toolOptionsDialog::createFontFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame );

  layout->addWidget( this->createFontGroupBox( frame ) );
  layout->addWidget( this->createEncodingBox( frame ) );
  layout->addWidget( this->createStyleBox( frame ) );
  layout->addWidget( this->createSyntaxGroupBox( frame ) );
  layout->addWidget( this->createMaxLogLinesGroupBox( frame ) );
  layout->addItem( this->createSpacer() );

  return frame;
}

QWidget* toolOptionsDialog::createFontGroupBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Font") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createFontLayout( gbox ) );
#if QT_VERSION >= 0x040000
  layout->addWidget( this->createFontButton( gbox ) );
#else
  layout->add( this->createFontButton( gbox ) );
#endif // QT_VERSION >= 0x040000

  return gbox;
}

QLayout* toolOptionsDialog::createFontLayout( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Current font is:" ) );
  layout->addWidget( label );

  QLabel* flabel = new QLabel( parent );
  this->currentFontName = flabel;
  layout->addWidget( flabel );

  return layout;
}

QPushButton* toolOptionsDialog::createFontButton( QWidget* parent )
{
  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Select Font" ) );
  connect( button, SIGNAL(clicked()), this, SLOT(selectFont()));
  return button;
}

QWidget* toolOptionsDialog::createEncodingBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Text Encoding") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createEncodingLayout( gbox ) );

  return gbox;
}

QLayout* toolOptionsDialog::createEncodingLayout( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Current encoding is:" ) );
  layout->addWidget( label );

  QComboBox* cbox = new QComboBox( parent );
#if QT_VERSION >= 0x040000
  cbox->setMaxVisibleItems( 20 );
#else
  cbox->setSizeLimit( 20 );
#endif // QT_VERSION >= 0x040000

  int count = 0;
  int defaultIndex = 0;
  QStringList menulist (QTLOCALE::getMenuList());
  for (QStringList::const_iterator itr = menulist.begin(); itr != menulist.end(); itr++) {
    if (itr->indexOf("UTF-8") > 0) {
      defaultIndex = count;
    }
#if QT_VERSION >= 0x040000
    cbox->addItem(*itr);
#else
    cbox->insertItem(*itr);
#endif // QT_VERSION >= 0x040000
    count++;
  }
#if QT_VERSION >= 0x040000
    cbox->setCurrentIndex(defaultIndex);
#else
    cbox->setCurrentItem(defaultIndex);
#endif // QT_VERSION >= 0x040000

  this->encodingList = cbox;
  layout->addWidget( cbox );

  return layout;
}

QWidget* toolOptionsDialog::createStyleBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Window Style") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createStyleLayout( gbox ) );

  return gbox;
}

QLayout* toolOptionsDialog::createStyleLayout( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Current style is:" ) );
  layout->addWidget( label );

  QComboBox* cbox = new QComboBox( parent );
#if QT_VERSION >= 0x040000
  cbox->setMaxVisibleItems( 20 );
#else
  cbox->setSizeLimit( 20 );
#endif // QT_VERSION >= 0x040000

  QStringList list = QStyleFactory::keys();
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
#if QT_VERSION >= 0x040000
    cbox->addItem( *it );
#else
    cbox->insertItem( *it );
#endif // QT_VERSION >= 0x040000
  }
  this->styleList = cbox;

  layout->addWidget( cbox );

  connect( cbox, SIGNAL(activated( const QString& )), this, SLOT(selectStyle( const QString& )));

#if QT_VERSION >= 0x040000
//  this->styleList->setEnabled( false );
#endif // QT_VERSION >= 0x040000
  return layout;
}

QWidget* toolOptionsDialog::createSyntaxGroupBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Syntax") );
  QLayout* layout = gbox->layout();

//  QHBoxLayout* hlayout = new QHBoxLayout();
  QHBoxLayout* hlayout = this->createHBoxLayout( NULL );

#if QT_VERSION >= 0x040000
  hlayout->addWidget( this->createSyntaxColoringBox( gbox ) );
  hlayout->addWidget( this->createAutoSyntaxCheckingBox( gbox ) );
#else
  hlayout->add( this->createSyntaxColoringBox( gbox ) );
  hlayout->add( this->createAutoSyntaxCheckingBox( gbox ) );
#endif // QT_VERSION >= 0x040000

  layout->addItem(hlayout);

  return gbox;
}

QWidget* toolOptionsDialog::createSyntaxColoringBox( QWidget* parent )
{
  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Syntax Coloring" ) );
  this->if_syntaxColoring = checkbox;
  return checkbox;
}

QWidget* toolOptionsDialog::createAutoSyntaxCheckingBox( QWidget* parent )
{
  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Auto Syntax Checking" ) );
  this->if_autoSyntaxChecking = checkbox;
  return checkbox;
}

QWidget* toolOptionsDialog::createMaxLogLinesGroupBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("Log Lines") );
  QLayout* layout = gbox->layout();

//  QHBoxLayout* hlayout = new QHBoxLayout();
  QHBoxLayout* hlayout = this->createHBoxLayout( NULL );

  hlayout->addItem( this->createMaxLogLinesLayout( gbox ) );
  hlayout->addItem( this->createMaxInterpreterLogLinesLayout( gbox ) );

  layout->addItem(hlayout);

  return gbox;
}

QLayout* toolOptionsDialog::createMaxLogLinesLayout( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout();

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Max Log Lines:" ) );
  layout->addWidget( label );

#if QT_VERSION >= 0x040000
  QSpinBox * sbox = this->createSpinBox( parent, 0, 999999, 1000 );
#else
  QSpinBox * sbox = this->createSpinBox( parent, -1, 999999, 1000 );
#endif // QT_VERSION >= 0x040000

  layout->addWidget( sbox );
  this->if_logMax = sbox;

  return layout;
}

QLayout* toolOptionsDialog::createMaxInterpreterLogLinesLayout( QWidget* parent )
{
  QHBoxLayout* layout = new QHBoxLayout();

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Max Interpreter Log Lines:" ) );
  layout->addWidget( label );

#if QT_VERSION >= 0x040000
  QSpinBox * sbox = this->createSpinBox( parent, 0, 999999, 1000 );
#else
  QSpinBox * sbox = this->createSpinBox( parent, -1, 999999, 1000 );
#endif // QT_VERSION >= 0x040000

  layout->addWidget( sbox );
  this->if_interpreterLogMax = sbox;

  return layout;
}

QSpinBox* toolOptionsDialog::createSpinBox( QWidget* parent, int min, int max, int value )
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

QWidget* toolOptionsDialog::createSaveToolOptionsCheckbox( QWidget* parent )
{
  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Save Options" ) );
  checkbox->setChecked(true);
  this->saveToolOptions = checkbox;
  return checkbox;
}

#ifdef VDMPP
QWidget* toolOptionsDialog::createUMLTab( QWidget* parent )
{
  QWidget* tab = new QWidget( parent );
  QVBoxLayout* layout = this->createVBoxLayout( tab );

  layout->addWidget( this->createUMLFrame( tab ) );

  return tab;
}

QWidget* toolOptionsDialog::createUMLFrame( QWidget* parent )
{
  QFrame* frame = this->createFrame( parent );
  QVBoxLayout* layout = this->createVBoxFrameLayout( frame );

  layout->addWidget( this->createUMLInterfaceBox( frame ) );
  layout->addWidget( this->createUMLFileTypeBox( frame ) );
  layout->addWidget( this->createUMLNewFileBox( frame ) );
  layout->addItem( this->createSpacer() );

  return frame;
}

QWidget* toolOptionsDialog::createUMLInterfaceBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("UML Interface") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createUMLInterfaceLayout( gbox ) );

  return gbox;
}

QLayout* toolOptionsDialog::createUMLInterfaceLayout( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Current interface is:" ) );
  layout->addWidget( label );

  QComboBox* cbox = new QComboBox( parent );
#if QT_VERSION >= 0x040000
  cbox->setMaxVisibleItems( 20 );
#else
  cbox->setSizeLimit( 20 );
#endif // QT_VERSION >= 0x040000

  int index = 0;
  while (true)
  {
    QString item (umlInterfaces[index]);
    if (item == "") break;
#if QT_VERSION >= 0x040000
    cbox->addItem(item);
#else
    cbox->insertItem(item);
#endif // QT_VERSION >= 0x040000
    index++;
  }

  this->umlInterfaceList = cbox;
  layout->addWidget( cbox );

  return layout;
}

QWidget* toolOptionsDialog::createUMLFileTypeBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("New File Type") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createUMLFileTypeLayout( gbox ) );

  return gbox;
}

QLayout* toolOptionsDialog::createUMLFileTypeLayout( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL );

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Current new file type is:" ) );
  layout->addWidget( label );

  QComboBox* cbox = new QComboBox( parent );
#if QT_VERSION >= 0x040000
  cbox->setMaxVisibleItems( 20 );
#else
  cbox->setSizeLimit( 20 );
#endif // QT_VERSION >= 0x040000

  int index = 0;
  while (true)
  {
    QString item (umlNewFileTypes[index]);
    if (item == "") break;
#if QT_VERSION >= 0x040000
    cbox->addItem(item);
#else
    cbox->insertItem(item);
#endif // QT_VERSION >= 0x040000
    index++;
  }

  this->umlFileTypeList = cbox;
  layout->addWidget( cbox );

  return layout;
}

QWidget* toolOptionsDialog::createUMLNewFileBox( QWidget* parent )
{
  QGroupBox* gbox = this->createVGroupBox( parent );
  gbox->setTitle( tr("RTF New File") );
  QLayout* layout = gbox->layout();

  layout->addItem( this->createUseNewFileCheckBox( gbox ) );
  layout->addItem( this->createNewFileNamePart( gbox ) );
  return gbox;
}

QLayout* toolOptionsDialog::createUseNewFileCheckBox( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QCheckBox* checkbox = new QCheckBox( parent );
  checkbox->setText( tr( "Use Template File" ) );
  this->if_useNewFile = checkbox; 

  layout->addWidget( checkbox );

  QPushButton* button = new QPushButton( parent );
  button->setText( tr( "Select Template File" ) );
  connect( button, SIGNAL(clicked()), this, SLOT(selectUMLNewFile()));
  this->if_selectNewFile = button;
  layout->addWidget( button );

  return layout;
}

QLayout* toolOptionsDialog::createNewFileNamePart( QWidget* parent )
{
  QHBoxLayout* layout = this->createHBoxLayout( NULL ); 

  QLabel* label = new QLabel( parent );
  label->setText( tr( "Template File: " ) );
  layout->addWidget( label );

  this->if_newFileNameLabel = label;

  QLineEdit* le = new QLineEdit( parent );

  layout->addWidget( le );
  this->if_newFileName = le;

  return layout;
}
#endif // VDMPP

/*  
 *  Destroys the object and frees any allocated resources
 */
toolOptionsDialog::~toolOptionsDialog()
{
  this->saveOptions();
}


void toolOptionsDialog::saveOptions()
{
  if (!this->saveToolOptions->isChecked()) return;

  QStringList menulist (QTLOCALE::getMenuList());

  QFile initFile(getInitFileName());
#if QT_VERSION >= 0x040000
  if ( !initFile.open(QIODevice::WriteOnly) ) return;
#else
  if ( !initFile.open(IO_WriteOnly) ) return;
#endif // QT_VERSION >= 0x040000

  QTextStream initStream(&initFile);
  QTextCodec * codec = QTextCodec::codecForName("UTF-8");
  initStream.setCodec(codec);

  initStream << "FormatVersion:2" << endl;
  initStream << "FontFamily:"  << this->currentFont.family() << endl;
  initStream << "FontPointSize:"  << this->currentFont.pointSize() << endl;
  initStream << "FontWeight:"  << this->currentFont.weight() << endl;
  initStream << "FontItalic:"  << this->currentFont.italic() << endl;
  initStream << "TextCodecName:"  << menulist[this->currentTextCodec] << endl;
  initStream << "UseExternalEditor:" << (this->if_useExternalEditor->isChecked() ? "true" : "false") << endl;
  initStream << "EditorName:" << this->if_editorName->text() << endl;
  initStream << "SingleLoadFormat:" << this->if_singleLoadFormat->text() << endl;
  initStream << "MultiFilesSupported:" << (this->if_multiFilesSupported->isChecked() ? "true" : "false")  << endl;
  initStream << "MultiLoadFormat:" << this->if_multiLoadFormat->text() << endl;
  initStream << "PrintCommand:" << this->if_printCommand->text() << endl;
#if QT_VERSION >= 0x040000
  initStream << "Style:" << QApplication::style()->objectName() << endl;
#else
  initStream << "Style:" << QApplication::style().name() << endl;
#endif // QT_VERSION >= 0x040000
  initStream << "SyntaxColoring:" << ( this->syntaxColoring ? "true" : "false" ) << endl;
  initStream << "AutoSyntaxChecking:" << ( this->autoSyntaxChecking ?  "true" : "false" ) << endl;
#ifdef VDMPP
  initStream << "UMLInterface:" << QString(umlInterfaces[this->umlinterface]) << endl;
  initStream << "UMLFileType:" << QString(umlNewFileTypes[this->umlnewfiletype]) << endl;
  initStream << "UseNewFile:" << (this->if_useNewFile->isChecked() ? "true" : "false") << endl;
  initStream << "NewFileName:" << this->if_newFileName->text() << endl;
#endif // VDMPP
  initStream << "MaxLogLines:" << this->if_logMax->value() << endl;
  initStream << "MaxInterpreterLogLines:" << this->if_interpreterLogMax->value() << endl;
  initStream << "JCGHackParser:" << (Qt2TB::GetJCGHackParser() ? "true" : "false") << endl;

  initFile.close();
}

bool toolOptionsDialog::loadOptions()
{
  QFile initFile( getInitFileName() );
#if QT_VERSION >= 0x040000
  if (!initFile.open(QIODevice::ReadOnly)) return false;
#else
  if (!initFile.open(IO_ReadOnly)) return false;
#endif // QT_VERSION >= 0x040000
  QTextStream initStream(&initFile);
  QString tmp = initStream.readLine();
  initFile.close();

  if( tmp == "FormatVersion:2" )
  {
    return this->loadOptionsV2();
  }
  else
  {
    return this->loadOptionsV1();
  }
}

bool toolOptionsDialog::loadOptionsV1()
{
  QFile initFile( getInitFileName() );
#if QT_VERSION >= 0x040000
  if (!initFile.open(QIODevice::ReadOnly)) return false;
#else
  if (!initFile.open(IO_ReadOnly)) return false;
#endif // QT_VERSION >= 0x040000

  QTextStream initStream(&initFile);
  QTextCodec * codec = QTextCodec::codecForName("UTF-8");
  initStream.setCodec(codec);

  // Get font
  QString fontFamily = initStream.readLine();
  QString sizeStr = initStream.readLine();
  QString weightStr = initStream.readLine();
  QString italicStr = initStream.readLine();
  int fontPtSize = sizeStr.toInt();
  int fontWeight = weightStr.toInt();
  int fontItalic = italicStr.toInt();
  this->currentFont = QFont(fontFamily, fontPtSize, fontWeight, fontItalic);

  // Get codec index
  QString codecname = initStream.readLine();
  int num = codecname.toInt();
  if( num > 0 || codecname == "0" )
    codecname = QTLOCALE::NumberToMenuName( num );

  QString menuname (QTLOCALE::Q3CodecNameToMenuName(codecname));

  QStringList menulist (QTLOCALE::getMenuList());
  this->currentTextCodec = 0;
  int index = 0;
  for (QStringList::const_iterator itr = menulist.begin(); itr != menulist.end(); itr++)
  {
    if( menuname == *itr )
    {
      this->currentTextCodec = index;
      break;
    }
    index++;
  }

  // Get interface settings
  QString useExternalEditorStr = initStream.readLine();
  useExternalEditor = useExternalEditorStr.toInt();

  QString tempEditorName = initStream.readLine();
  if (tempEditorName != "") this->editorName = tempEditorName;
  this->singleLoadFormat = initStream.readLine();

  QString multiFilesSupportedStr = initStream.readLine();
  multiFilesSupported = multiFilesSupportedStr.toInt();

  this->multiLoadFormat = initStream.readLine();
  this->printCommand = initStream.readLine();

  if( !initStream.atEnd() )
  {
    QString style = initStream.readLine();
    QStringList list = QStyleFactory::keys();
    if( list.contains( style ) > 0 )
    {
      this->selectStyle( style );
    }
  }

  if( !initStream.atEnd() )
  {
    QString syntaxColoringStr = initStream.readLine();
    if( syntaxColoringStr == "true" )
      this->syntaxColoring = true;
    else
      this->syntaxColoring = false;
  }
  initFile.close();
  return true;
}

bool toolOptionsDialog::loadOptionsV2()
{
  QFile initFile( getInitFileName() );
#if QT_VERSION >= 0x040000
  if ( !initFile.open(QIODevice::ReadOnly)) return false;
#else
  if ( !initFile.open(IO_ReadOnly)) return false;
#endif // QT_VERSION >= 0x040000
  QTextStream initStream(&initFile);
  QTextCodec * codec = QTextCodec::codecForName("UTF-8");
  initStream.setCodec(codec);

  QMap<QString, QString> initMap;
  while( !initStream.atEnd() )
  {
    QString tmp = initStream.readLine();
    if( tmp.isEmpty() ) continue;
#if QT_VERSION >= 0x040000
    int index = tmp.indexOf( ':' );
#else
    int index = tmp.find( ':' );
#endif // QT_VERSION >= 0x040000
    if( index == -1 ) continue;
#if QT_VERSION >= 0x040000
    QString key = tmp.left( index ).simplified();
    QString value = tmp.right( tmp.length() - index - 1 ).simplified();
#else
    QString key = tmp.left( index ).stripWhiteSpace();
    QString value = tmp.right( tmp.length() - index - 1 ).stripWhiteSpace();
#endif // QT_VERSION >= 0x040000
    initMap[ key ] = value;
  }
  initFile.close();

  QString fontFamily = this->currentFont.family();
  if( initMap.contains( "FontFamily" ) )
    fontFamily = initMap[ "FontFamily" ];
  int fontPtSize = this->currentFont.pointSize();
  if( initMap.contains( "FontPointSize" ) )
    fontPtSize = initMap[ "FontPointSize" ].toInt();
  int fontWeight = this->currentFont.weight();
  if( initMap.contains( "FontWeight" ) )
    fontWeight = initMap[ "FontWeight" ].toInt();
  int fontItalic = this->currentFont.italic();
  if( initMap.contains( "FontItalic" ) )
    fontItalic = initMap[ "FontItalic" ].toInt();
  this->currentFont = QFont(fontFamily, fontPtSize, fontWeight, fontItalic);

  // Get codec index
  if( initMap.contains( "TextCodecName" ) ) {
    QString codecname = initMap[ "TextCodecName" ];
    int num = codecname.toInt();
    if ( num > 0 || codecname == "0" ) {
      codecname = QTLOCALE::NumberToMenuName( num );
    }

    QString menuname (QTLOCALE::Q3CodecNameToMenuName(codecname));

    QStringList menulist (QTLOCALE::getMenuList());
    this->currentTextCodec = 0;
    int index = 0;
    for (QStringList::const_iterator itr = menulist.begin(); itr != menulist.end(); itr++) {
      if( menuname == *itr ) {
        this->currentTextCodec = index;
        break;
      }
      index++;
    }
  }

  // Get interface settings
  if( initMap.contains( "UseExternalEditor" ) ) {
    if( initMap[ "UseExternalEditor" ] == "1" ||
        initMap[ "UseExternalEditor" ] == "true" )
      this->useExternalEditor = true;
    else
      this->useExternalEditor = false;
  }
  if( initMap.contains( "EditorName" ) ) {
    this->editorName = initMap[ "EditorName" ];
  }
  if( initMap.contains( "SingleLoadFormat" ) ) {
    this->singleLoadFormat = initMap[ "SingleLoadFormat" ];
  }
  if( initMap.contains( "MultiFilesSupported" ) ) {
    if( initMap[ "MultiFilesSupported" ] == "1"  ||
        initMap[ "MultiFilesSupported" ] == "true" ) {
      this->multiFilesSupported = true;
    }
    else {
      this->multiFilesSupported = false;
    }
  }
  if( initMap.contains( "MultiLoadFormat" ) ) {
    this->multiLoadFormat = initMap[ "MultiLoadFormat" ];
  }
  if( initMap.contains( "PrintCommand" ) ) {
    this->printCommand = initMap[ "PrintCommand" ];
  }
  if( initMap.contains( "Style" ) ) {
    QString style = initMap[ "Style" ];
    QStringList list = QStyleFactory::keys();
    bool found = false;
    for (QStringList::const_iterator itr = list.begin(); (itr != list.end()) && !found; itr++) {
      QString st(*itr);
#if QT_VERSION >= 0x040000
      if (st.toLower() == style.toLower()) {
#else
      if (st.lower() == style.lower()) {
#endif // QT_VERSION >= 0x040000
        this->selectStyle( st );
        this->currentStyle = style;
        found = true;
      }
    }
  }
  if( initMap.contains( "SyntaxColoring" ) ) {
    if( initMap[ "SyntaxColoring" ] == "true" ) {
      this->syntaxColoring = true;
    }
    else {
      this->syntaxColoring = false;
    }
  }
  if( initMap.contains( "AutoSyntaxChecking" ) ) {
    if( initMap[ "AutoSyntaxChecking" ] == "true" ) {
      this->autoSyntaxChecking = true;
    }
    else {
      this->autoSyntaxChecking = false;
    }
  }

#ifdef VDMPP
  this->umlinterface = 0;
  if( initMap.contains( "UMLInterface" ) )
  {
    QString umlinterfaceStr = initMap[ "UMLInterface" ];

    if ((umlinterfaceStr == UML_JUDE_XMI) || (umlinterfaceStr == UML_ASTAH_XMI))
      umlinterfaceStr = UML_ASTAH_PRO_XMI;

    int index = 0;
    while(true) {
      QString item (umlInterfaces[index]);
      if (item == "") break;
      if (item == umlinterfaceStr)
      {
        this->umlinterface = index;
        break;
      }
      index++;
    }
  }
  this->umlnewfiletype = 0;
  if( initMap.contains( "UMLFileType" ) )
  {
    QString umlnewfiletypeStr = initMap[ "UMLFileType" ];
    int index = 0;
    while(true) {
      QString item (umlNewFileTypes[index]);
      if (item == "") break;
      if (item == umlnewfiletypeStr)
      {
        this->umlnewfiletype = index;
        break;
      }
      index++;
    }
  }
  if( initMap.contains( "UseNewFile" ) )
  {
    if( initMap[ "UseNewFile" ] == "true" )
      this->useNewFile = true;
    else
      this->useNewFile = false;
  }
  if( initMap.contains( "NewFileName" ) )
    this->newFileName = initMap[ "NewFileName" ];
#endif // VDMPP

  if (initMap.contains( "MaxLogLines" ))
  {
    int max = initMap[ "MaxLogLines" ].toInt();
#if QT_VERSION >= 0x040000
    if (max < 0 ) max = 0;
#else
    if (max == 0 ) max = -1;
#endif // QT_VERSION >= 0x040000
    this->if_logMax->setValue(max);
  }

  if (initMap.contains( "MaxInterpreterLogLines" ))
  {
    int max = initMap[ "MaxInterpreterLogLines" ].toInt();
#if QT_VERSION >= 0x040000
    if (max < 0 ) max = 0;
#else
    if (max == 0 ) max = -1;
#endif // QT_VERSION >= 0x040000
    this->if_interpreterLogMax->setValue(max);
  }

  if (initMap.contains( "JCGHackParser" ))
  {
    Qt2TB::SetJCGHackParser(initMap[ "JCGHackParser" ] == "true");
  }

  return true;
}

void toolOptionsDialog::setCurrentCodec(const QString & cnm)
{
  QStringList menulist (QTLOCALE::getMenuList());
  this->currentTextCodec = 0;
  int index = 0;
  for (QStringList::const_iterator itr = menulist.begin(); itr != menulist.end(); itr++)
  {
    if( cnm == *itr )
    {
      this->currentTextCodec = index;
      break;
    }
    index++;
  }
}

void toolOptionsDialog::accept()
{
  this->saveOptions();
  QDialog::accept();
#if QT_VERSION >= 0x040000
  this->currentStyle = QApplication::style()->objectName();
#else
  this->currentStyle = QApplication::style().name();
#endif // QT_VERSION >= 0x040000
}

void toolOptionsDialog::reject()
{
#if QT_VERSION >= 0x040000
  QString style = QApplication::style()->objectName();
  if (style != this->currentStyle)
  {
    int count = this->styleList->count();
    bool found = false;
    for (int idx = 1; (idx <= count) && ! found; idx++)
    {
      QString st (this->styleList->itemText(idx));
      if (st.toLower() == style.toLower())
      {
        this->styleList->setCurrentIndex(idx);
        found = true;
      }
    }
    selectStyle(this->currentStyle);
  }
#else
  QString style = QApplication::style().name();
  if (style != this->currentStyle)
  {
    int count = this->styleList->count();
    bool found = false;
    for (int idx = 1; (idx <= count) && ! found; idx++)
    {
      QString st (this->styleList->text(idx));
      if (st.lower() == style.lower())
      {
        this->styleList->setCurrentItem(idx);
        found = true;
      }
    }
    selectStyle(this->currentStyle);
  }
#endif // QT_VERSION >= 0x040000
  QDialog::reject();
}

void toolOptionsDialog::selectFont()
{
  this->selectedFont = QFontDialog::getFont( 0, this->currentFont );
  this->currentFontName->setText( selectedFont.family() );
}

void toolOptionsDialog::selectEditor()
{
//  QString filter ("Project files (*.prj);; all (*.*)");
  QString file (QtPort::QtGetOpenFileName( this,
                                           tr("Select Editor..."),
                                           this->getHome()));
  if (! file.isEmpty()) //at least one item
  {
    this->editorName = file;
    this->if_editorName->setText(this->editorName);
  }
}

void toolOptionsDialog::selectUMLNewFile()
{
#ifdef VDMPP
  QString filter ("RTF files (*.rtf);; all (*.*)");
  QString file (QtPort::QtGetOpenFileName( this,
                                           tr("Select Template File..."),
                                           this->getHome(),
                                           filter));
  if (! file.isEmpty()) { //at least one item
    this->newFileName = file;
    this->if_newFileName->setText(this->newFileName);
  }
#endif // VDMPP
}

void toolOptionsDialog::applyDialog()
{
  if (this->selectedFont != this->currentFont) {
    this->currentFont = this->selectedFont;
    emit setFont(this->currentFont);
  }

#if QT_VERSION >= 0x040000
  int selectedCodec = this->encodingList->currentIndex();
#else
  int selectedCodec = this->encodingList->currentItem();
#endif // QT_VERSION >= 0x040000
  if (selectedCodec != this->currentTextCodec) {
    QStringList menulist (QTLOCALE::getMenuList());
    this->currentTextCodec = selectedCodec;
    emit setTextCodec(menulist[this->currentTextCodec]);
#if QT_VERSION >= 0x040000
    this->encodingList->setCurrentIndex(this->currentTextCodec);
#else
    this->encodingList->setCurrentItem(this->currentTextCodec);
#endif // QT_VERSION >= 0x040000
  }

  this->syntaxColoring = this->if_syntaxColoring->isChecked();

  this->autoSyntaxChecking = this->if_autoSyntaxChecking->isChecked();

  if( (this->useExternalEditor != this->if_useExternalEditor->isChecked()) ||
      (this->editorName != this->if_editorName->text()) ) {
    this->useExternalEditor = this->if_useExternalEditor->isChecked();
    this->editorName = this->if_editorName->text();
    this->setEditor();
  }
  this->singleLoadFormat = this->if_singleLoadFormat->text();
  this->multiFilesSupported = false;
  this->multiLoadFormat = this->if_multiLoadFormat->text();
  this->printCommand = this->if_printCommand->text();

#ifdef VDMPP
#if QT_VERSION >= 0x040000
  this->umlinterface = this->umlInterfaceList->currentIndex();
  this->umlnewfiletype = this->umlFileTypeList->currentIndex();
#else
  this->umlinterface = this->umlInterfaceList->currentItem();
  this->umlnewfiletype = this->umlFileTypeList->currentItem();
#endif // QT_VERSION >= 0x040000
  this->setUMLOptions();
#endif // VDMPP

  emit setMaxLogLines(this->if_logMax->value());
  emit setMaxInterpreterLogLines(this->if_interpreterLogMax->value());
}

QString toolOptionsDialog::getHome()
{
  if(this->homeDir.isEmpty())
  {
    const char* home = getenv( HOME_ENV_NAME );
    if( NULL != home )
      this->homeDir = Qt2TB::wstring2qstring(TBWSTR::fsstr2wstring(home));
    else
      this->homeDir = ".";
  }
  return this->homeDir;
}

QString toolOptionsDialog::getInitFileName()
{
  return this->getHome() + "/" + initFileName;
}

const QString & toolOptionsDialog::get_if_editorName()
{
  return this->editorName;
}

const QString & toolOptionsDialog::get_if_singleLoadFormat()
{
  return this->singleLoadFormat;
}

bool toolOptionsDialog::get_if_useExternalEditor()
{
  return this->useExternalEditor;
}

void toolOptionsDialog::selectStyle( const QString& style )
{
#if QT_VERSION >= 0x040000
  QString st = QApplication::style()->objectName();
  if(st.toLower() != style.toLower())
  {
    QApplication::setStyle( style );
  }
#else
  QString st = QApplication::style().name();
  if(st.lower() != style.lower())
  {
    QApplication::setStyle( style );
  }
#endif // QT_VERSION >= 0x040000
}

void toolOptionsDialog::setFontAndCodec()
{
  QStringList menulist (QTLOCALE::getMenuList());
  emit setFont(this->currentFont);

  emit setTextCodec(menulist[this->currentTextCodec]);

#if QT_VERSION >= 0x040000
  this->encodingList->setCurrentIndex(this->currentTextCodec);
#else
  this->encodingList->setCurrentItem(this->currentTextCodec);
#endif // QT_VERSION >= 0x040000

  emit setMaxLogLines(this->if_logMax->value());
  emit setMaxInterpreterLogLines(this->if_interpreterLogMax->value());
}

bool toolOptionsDialog::doSyntaxColoring() const
{
  return this->syntaxColoring;
}

bool toolOptionsDialog::doAutoSyntaxChecking() const
{
  return this->autoSyntaxChecking;
}

void toolOptionsDialog::show()
{
  this->setOptions();
  this->editerNameEnabled();
  this->multiLoadEnabled();
  QDialog::show();
}

void toolOptionsDialog::setOptions()
{
  this->if_useExternalEditor->setChecked(this->useExternalEditor);
  this->if_editorName->setText(this->editorName);
  this->if_singleLoadFormat->setText(this->singleLoadFormat);
  this->if_multiFilesSupported->setChecked(false);
  this->if_multiLoadFormat->setText(this->multiLoadFormat);
  this->if_printCommand->setText(this->printCommand);
  this->if_syntaxColoring->setChecked(this->syntaxColoring);
  this->if_autoSyntaxChecking->setChecked(this->autoSyntaxChecking);
#if QT_VERSION >= 0x040000
  this->encodingList->setCurrentIndex(this->currentTextCodec);
  QString style = QApplication::style()->objectName();
  int count = this->styleList->count();
  bool found = false;
  for (int idx = 1; (idx <= count) && ! found; idx++)
  {
    QString st (this->styleList->itemText(idx));
    if (st.toLower() == style.toLower())
    {
      this->styleList->setCurrentIndex(idx);
      found = true;
    }
  }
#else
  this->encodingList->setCurrentItem(this->currentTextCodec);
//  this->styleList->setCurrentText( QApplication::style().name() );
  QString style = QApplication::style().name();
  int count = this->styleList->count();
  bool found = false;
  for (int idx = 1; (idx <= count) && ! found; idx++)
  {
    QString st (this->styleList->text(idx));
    if (st.lower() == style.lower())
    {
      this->styleList->setCurrentItem(idx);
      found = true;
    }
  }
#endif // QT_VERSION >= 0x040000
  this->currentFontName->setText( this->currentFont.family() );
  this->selectStyle( this->currentStyle );
  this->currentStyle = style;

#ifdef VDMPP
#if QT_VERSION >= 0x040000
  this->umlInterfaceList->setCurrentIndex(this->umlinterface);
  this->umlFileTypeList->setCurrentIndex(this->umlnewfiletype);
#else
  this->umlInterfaceList->setCurrentItem(this->umlinterface);
  this->umlFileTypeList->setCurrentItem(this->umlnewfiletype);
#endif // QT_VERSION >= 0x040000
  this->if_useNewFile->setChecked(this->useNewFile);
  this->if_newFileName->setText(this->newFileName);
#endif // VDMPP
}

void toolOptionsDialog::setEditor()
{
  if( ( this->useExternalEditor == true ) &&
      ( this->editorName.length() > 0 ) )
    emit enableEditor(true);
  else
//    emit enableEditor(false);
    emit enableEditor(true);
}

void toolOptionsDialog::editerNameEnabled()
{
  if(this->if_useExternalEditor->isChecked()) {
    this->if_selectEditor->setEnabled(true);
    this->if_editorNameLabel->setEnabled(true);
    this->if_editorName->setEnabled(true);
    this->if_singleLoadFormatLabel->setEnabled(true);
    this->if_singleLoadFormat->setEnabled(true);
    this->if_multiFilesSupported->setEnabled(false);
    this->if_loadFormatHelp->setEnabled(true);
    this->multiLoadEnabled();
  }
  else {
    this->if_selectEditor->setEnabled(false);
    this->if_editorNameLabel->setEnabled(false);
    this->if_editorName->setEnabled(false);
    this->if_singleLoadFormatLabel->setEnabled(false);
    this->if_singleLoadFormat->setEnabled(false);
    this->if_multiFilesSupported->setEnabled(false);
    this->if_multiLoadFormatLabel->setEnabled(false);
    this->if_multiLoadFormat->setEnabled(false);
    this->if_loadFormatHelp->setEnabled(false);
  }
}

void toolOptionsDialog::multiLoadEnabled()
{
  if(this->if_multiFilesSupported->isChecked()) {
    this->if_multiLoadFormatLabel->setEnabled(true);
    this->if_multiLoadFormat->setEnabled(true);
  }
  else {
    this->if_multiLoadFormatLabel->setEnabled(false);
    this->if_multiLoadFormat->setEnabled(false);
  }
}

#ifdef VDMPP
void toolOptionsDialog::setUMLOptions()
{
  QString itrf (umlInterfaces[this->umlinterface]);
  if ((itrf == UML_JUDE_XMI) || (itrf == UML_ASTAH_XMI) || (itrf == UML_ASTAH_PRO_XMI)) {
    Qt2TB::SetToolInterfaceI(INTERFACE_XMI11_UML14_ASTAH);
  }
  else if (itrf == UML_EA_XMI) {
    Qt2TB::SetToolInterfaceI(INTERFACE_XMI11_UML14_EA);
  }
#ifdef _MSC_VER
#ifdef _USE_ROSE
  else if (itrf == UML_Rose_OLE) {
    Qt2TB::SetToolInterfaceI(INTERFACE_ROSE98);
  }
#endif // _USE_ROSE
#endif // _MSC_VER

  QString ftp (umlNewFileTypes[this->umlnewfiletype]);
  if (ftp == UML_FTYPE_VDM) {
    Qt2TB::SetNewFileTypeI(FILETYPE_VPP);
  }
  else if (ftp == UML_FTYPE_RTF) {
    Qt2TB::SetNewFileTypeI(FILETYPE_RTF);
  }
  else if (ftp == UML_FTYPE_TEX) {
    Qt2TB::SetNewFileTypeI(FILETYPE_TEX);
  }

  if (this->useNewFile) {
    Qt2TB::SetRTFTemplateI(this->newFileName);
  }
  else {
    Qt2TB::SetRTFTemplateI(QString(""));
  }
}
#endif // VDMPP

bool toolOptionsDialog::isRoseMode()
{
#ifdef VDMPP
#ifdef _MSC_VER
#ifdef _USE_ROSE
  QString itrf (umlInterfaces[this->umlinterface]);
  return (itrf == UML_Rose_OLE);
#else
  return false;
#endif // _USE_ROSE
#else
  return false;
#endif // _MSC_VER
#else
  return false;
#endif // VDMPP
}

