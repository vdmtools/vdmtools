/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/optionsF.h,v $
*  * VERSION
*  *    $Revision: 1.13 $
*  * DATE
*  *    $Date: 2006/02/07 01:52:03 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __optionsw_h__
#define __optionsw_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#endif
#define QLISTWIDGET QListWidget
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qdialog.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtabwidget.h>
#define QLISTWIDGET QListBox
#endif // QTVER >= 4

class mainW;

class optionsW : public QDialog
{ 
  Q_OBJECT

public:
  optionsW( QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
  ~optionsW();

#ifdef VDMPP
  bool get_j2v_stubsOnly();
//  bool get_j2v_autoRenaming();
  bool get_j2v_transforms();
#endif //VDMPP

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  void setCurrentCodecName(const QString & name) { this->currentCodecName = name; };
  QString getCurrentCodecName() { return this->currentCodecName; };
  void loadOptions();
  void saveOptions();
  bool optionFileExists();
  void initTab();
  QString getCodecName(const QString & prj);
  QString getExpression();

public slots:
  void interfaces();
  void show();

private slots:
  void putOptions();
  void okPressed();
  void algorithmChanged(int);
  void vcpuCapacityEnabled();
  void logArgsAllEnabled();
  void classChanged(int);
  void addOperation();
  void delOperation();
  void invClicked();
  void selectJCGDir();

private:
  mainW* mainw;

  void setOptions();
  void clearJ2VOptions();
  bool String2Bool(const QString & str);
  QMap<QString, QString> readOption2(const QString & filename); 
  QString getOptionFileName(const QString & prj);
  void loadOptionsV1();
  void loadOptionsV2();

  QSpacerItem * createSpacer();
  QFrame * createFrame( QWidget* parent );
  QGroupBox * createVGroupBox( QWidget* parent );
  QVBoxLayout* createVBoxLayout( QWidget* parent );
  QHBoxLayout* createHBoxLayout( QWidget* parent );
  QVBoxLayout* createVBoxFrameLayout( QFrame* frame );
  QSpinBox* createSpinBox( QWidget* parent, int min, int max, int value );
  QComboBox* createComboBox( QWidget* parent );

  QWidget* createTabPart( QWidget* parent );
  QWidget* createInterpreterTab( QWidget* parent );
  QWidget* createInterpreterFrame( QWidget* parent );
  QLayout* createInterpreterLayout1( QWidget* parent );
  QWidget* createRuntimeCheckingGroupBox( QWidget * parent );
  QLayout* createInterpreterLayout2( QWidget* parent );
  QLayout* createExpressionLayout( QWidget* parent );
  QWidget* createTypeCheckerTab( QWidget* parent );
  QWidget* createTypeCheckerFrame( QWidget* parent );
  QCheckBox* createTypeCheckerExtCheckBox( QWidget* parent );
  QCheckBox* createTypeCheckerMsgSepCheckBox( QWidget* parent );
  QCheckBox* createStandardVDMSLCheckBox( QWidget* parent );
  QCheckBox* createVDM10CheckBox( QWidget* parent );
  QWidget* createTypeCheckerButtonGroup( QWidget* parent );
  QWidget* createPrettyPrinterTab( QWidget* parent );
  QWidget* createPrettyPrinterFrame( QWidget* parent );
  QWidget* createPrettyPrinterIndex( QWidget* parent );
  QCheckBox* createPrettyPrinterCloring( QWidget* parent );
  QWidget* createCppCodeGeneratorTab( QWidget* parent );
  QWidget* createCppCodeGeneratorFrame( QWidget* parent );
  QCheckBox* createCppCodeGenPosInfCheckBox( QWidget* parent );
  QCheckBox* createCppCodeGenCheckCondsCheckBox( QWidget* parent );

#ifdef VDMPP
  QWidget* createJavaCodeGeneratorTab( QWidget* parent );
  QWidget* createJavaInterfacesButton( QWidget* parent );
  QLayout* createJavaPackageLayout( QWidget* parent );
  QLayout* createOutputDirCheckBox( QWidget* parent );
  QLayout* createOutputDirNamePart( QWidget* parent );
  QWidget* createJavaCodeGeneratorFrame( QWidget* parent );
  QWidget* createJ2VTab( QWidget* parent );
  QWidget* createJ2VFrame( QWidget* parent );
#ifdef VICE
  QWidget* createVICETab( QWidget* parent );
  QLayout* createVICELayout1( QWidget* parent );
  QLayout* createVICELayout2( QWidget* parent );
#endif // VICE
#endif // VDMPP

  QWidget* createRandomGeneratorGroupBox( QWidget * parent ); 
  QLayout* createRandomGeneratorSeedLayout( QWidget * parent ); 
  QLayout* createRandomGeneratorMessageLayout( QWidget * parent ); 
#ifdef VDMPP
  QWidget* createMultiThreadGroupBox( QWidget * parent ); 
  QLayout* createMaxInstrLayout( QWidget * parent ); 
  QLayout* createPrimarySchdAlgorithmLayout( QWidget * parent ); 

#ifdef VICE
  QLayout* createMaxTimeLayout( QWidget * parent ); 
  QLayout* createStepSizeLayout( QWidget * parent ); 
  QLayout* createDefaultCapacityLayout( QWidget * parent ); 
  QWidget* createVirtualCPUCapacityGroupBox( QWidget * parent ); 
  QLayout* createJitterModeLayout( QWidget * parent ); 
  QWidget* createTraceLogGroupBox( QWidget* parent );
  QLayout* createClassInputLayout( QWidget * parent ); 
  QLayout* createOpInputLayout( QWidget * parent ); 
  QLayout* createAddDelButtonLayout( QWidget * parent ); 
  QPushButton * createAddButton( QWidget * parent );
  QPushButton * createDeleteButton( QWidget * parent );
  
  void loadClassName();
#endif // VICE
#endif // VDMPP

  QLayout* createButtonPart( QWidget* parent );
  QWidget* createHelpButton( QWidget* parent );
  QWidget* createOkButton( QWidget* parent );
  QWidget* createCancelButton( QWidget* parent );
  QWidget* createApplyButton( QWidget* parent );

  QWidget* createSaveOptionsCheckbox( QWidget* parent );

  QTabWidget* maintab;

  QCheckBox* saveOptionsFlag;

  // Interpreter options
  QCheckBox* ip_dynTypeCheck;
  QCheckBox* ip_dynInvCheck;
  QCheckBox* ip_preCheck;
  QCheckBox* ip_postCheck;
  QCheckBox* ip_prioritySchd;
  QCheckBox* ip_ppValues;
  QCheckBox* ip_exception;
  QSpinBox* ip_rndGen;
  QSpinBox* ip_maxInstr;
  QLabel * ip_maxInstrLabel;
  QSpinBox* ip_maxTime;
  QLabel * ip_maxTimeLabel;
  QSpinBox* ip_stepSize;
  QSpinBox* ip_cpuCapacity;
  QSpinBox* ip_vcpuCapacity;
  QLabel * ip_vcpuCapacityLabel;
  QCheckBox* ip_vcpuCapacitycb;
  QComboBox* ip_primaryAlg;
  QComboBox* ip_jitterMode;
  QCheckBox* ip_logArgsAllCheck;
  QPushButton* ip_opsSelectButton;
  QLISTWIDGET* ip_selectedOpsList;
  QLabel * ip_classNameLabel;
  QComboBox* ip_className;
  QLabel * ip_opNameLabel;
  QComboBox* ip_opName;
  QPushButton* ip_opAddButton;
  QPushButton* ip_opDelButton;
  QLineEdit* ip_expression;

  // Type checker options
  QRadioButton* tc_posTc;
  QRadioButton* tc_defTc;
  QCheckBox* tc_extTc;
  QCheckBox* tc_msgSeparation;
  QCheckBox* tc_standardVDMSL;
  QCheckBox* tc_vdm10;

  // Pretty printer options
  QRadioButton* pp_noIndex;
  QRadioButton* pp_defIndex;
  QRadioButton* pp_useIndex;
  QCheckBox* pp_tcovCol;

  // C++ Code generator options
  QCheckBox* cpp_posInfo;
  QCheckBox* cpp_checkConds;

#ifdef VDMPP
  // Java code generator options
  QCheckBox* jcg_skeletonsOnly;
  QCheckBox* jcg_typesOnly;
  QCheckBox* jcg_useLongs;
  QCheckBox* jcg_genConc;
  QCheckBox* jcg_genConds;
  QCheckBox* jcg_checkConds;
  QLineEdit* jcg_packageName;
  QCheckBox* jcg_vdmPrefix;
  QCheckBox* jcg_useOutputDir;
  QPushButton* jcg_selectDir;
  QLabel* jcg_outputDirLabel;
  QLineEdit* jcg_outputDirName;
  QCheckBox* jcg_needBackup;

  // Java2VDM options
  QCheckBox* j2v_stubsOnly;
  QCheckBox* j2v_autoRenaming;
  QCheckBox* j2v_genAccessors;
  QCheckBox* j2v_transforms;
#endif //VDMPP

private:
  QString windowName;
  QString currentCodecName;
};

#endif // __optionsw_h__
