/****************************************************************************
** Form interface generated from reading ui file 'toolOptionsDialog.ui'
**
** Created: Mon Jun 11 11:55:08 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef __tooloptionsdialog_h__
#define __tooloptionsdialog_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qdialog.h>
#include <qfont.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#endif // QTVER >= 4

class toolOptionsDialog : public QDialog
{ 
    Q_OBJECT

public:
  toolOptionsDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
  ~toolOptionsDialog();

  const QString & get_if_editorName();
  const QString & get_if_singleLoadFormat();
  bool get_if_useExternalEditor();
  void setFontAndCodec();
  bool doSyntaxColoring() const;
  bool doAutoSyntaxChecking() const;
  void setEditor();
  void setUMLOptions();

  void setWindowName(const char* name) { this->windowName = name; }
  const QString & getWindowName() { return this->windowName; }
  bool isRoseMode();
  void setCurrentCodec(const QString & cnm);

public slots:
  void show();

signals:
  void logWrite(QString);
  void setFont(QFont);
  void setTextCodec(QString);
  void enableEditor(bool);
  void setMaxLogLines(int);
  void setMaxInterpreterLogLines(int);

protected slots:
  void selectFont();
  void applyDialog();
  void editerNameEnabled();
  void multiLoadEnabled();
  void selectEditor();
  void selectUMLNewFile();

protected:
  QString getInitFileName();

private:
  void saveOptions();
  bool loadOptions();
  bool loadOptionsV1();
  bool loadOptionsV2();
  void setOptions();
  QString getHome();

  QSpacerItem * createSpacer();
  QFrame * createFrame( QWidget* parent );
  QVBoxLayout* createVBoxFrameLayout( QFrame* frame );
  QVBoxLayout* createVBoxLayout( QWidget* parent );
  QHBoxLayout* createHBoxLayout( QWidget* parent );
  QGroupBox * createVGroupBox( QWidget* parent );
  QSpinBox* createSpinBox( QWidget* parent, int min, int max, int value );

  QWidget* createTabPart( QWidget* parent );
  QLayout* createButtonPart( QWidget* parent );
  QWidget* createCancelButton( QWidget* parent );
  QWidget* createApplyButton( QWidget* parent );
  QWidget* createOkButton( QWidget* parent );
  QWidget* createEditAndPrintTab( QWidget* parent );
  QWidget* createEditAndPrintFrame( QWidget* parent );
  QWidget* createEditGroupBox( QWidget* parent );
  QWidget* createPrintGroupBox( QWidget* parent );
  QWidget* createFontTab( QWidget* parent );
  QWidget* createFontFrame( QWidget* parent );
  QLayout* createUseExternalEditorCheckBox( QWidget* parent );
  QLayout* createEditorNamePart( QWidget* parent );
  QLayout* createSingleLoadFormatPart( QWidget* parent );
  QWidget* createMultiFileSupportedCheckBox( QWidget* parent );
  QLayout* createMultiLoadFormatPart( QWidget* parent );
  QWidget* createLoadFormatHelp( QWidget* parent );
  QLayout* createPrintCommandPart( QWidget* parent );
  QWidget* createPrintCommendHelp( QWidget* parent );
  QWidget* createSaveToolOptionsCheckbox( QWidget* parent );
  QWidget* createEncodingBox( QWidget* parent );
  QLayout* createEncodingLayout( QWidget* parent );
  QWidget* createFontGroupBox( QWidget* parent );
  QLayout* createFontLayout( QWidget* parent );
  QPushButton* createFontButton( QWidget* parent );
  QWidget* createStyleBox( QWidget* parent );
  QLayout* createStyleLayout( QWidget* parent );
  QWidget* createSyntaxGroupBox( QWidget* parent );
  QWidget* createSyntaxColoringBox( QWidget* parent );
  QWidget* createAutoSyntaxCheckingBox( QWidget* parent );
  QWidget* createMaxLogLinesGroupBox( QWidget* parent );
  QLayout* createMaxLogLinesLayout( QWidget* parent );
  QLayout* createMaxInterpreterLogLinesLayout( QWidget* parent );

#ifdef VDMPP
  QWidget* createUMLTab( QWidget* parent );
  QWidget* createUMLFrame( QWidget* parent );
  QWidget* createUMLInterfaceBox( QWidget* parent );
  QWidget* createUMLFileTypeBox( QWidget* parent );
  QWidget* createUMLNewFileBox( QWidget* parent );
  QLayout* createUMLInterfaceLayout( QWidget* parent );
  QLayout* createUMLFileTypeLayout( QWidget* parent );
  QLayout* createUseNewFileCheckBox( QWidget* parent );
  QLayout* createNewFileNamePart( QWidget* parent );
#endif // VDMPP

  QCheckBox* if_useExternalEditor;
  QPushButton* if_selectEditor;
  QLineEdit* if_editorName;
  QLabel* if_editorNameLabel;
  QLineEdit* if_singleLoadFormat;
  QLabel* if_singleLoadFormatLabel;
  QCheckBox* if_multiFilesSupported;
  QLineEdit* if_multiLoadFormat;
  QLabel* if_multiLoadFormatLabel;
  QLabel* if_loadFormatHelp;
  QLineEdit* if_printCommand;
  QCheckBox* saveToolOptions;
  QComboBox* encodingList;
  QComboBox* fsencodingList;
  QComboBox* styleList;
  QLabel* currentFontName;
  QLabel* currentFontSize;
  QCheckBox* if_syntaxColoring;
  QCheckBox* if_autoSyntaxChecking;

  QSpinBox* if_logMax;
  QSpinBox* if_interpreterLogMax;

#ifdef VDMPP
  QComboBox* umlInterfaceList;
  QComboBox* umlFileTypeList;
  QCheckBox* if_useNewFile;
  QPushButton* if_selectNewFile;
  QLineEdit* if_newFileName;
  QLabel* if_newFileNameLabel;
#endif // VDMPP

  QFont currentFont;
  QFont selectedFont;
  QString initFileName;
  QString editorName;
  QString singleLoadFormat;
  QString multiLoadFormat;
  QString printCommand;

#ifdef VDMPP
  QString newFileName;
#endif // VDMPP

  int currentTextCodec;
  bool useExternalEditor;
  bool multiFilesSupported;
  QString currentStyle;
  bool syntaxColoring;
  bool autoSyntaxChecking;

#ifdef VDMPP
  bool useNewFile;
  int umlinterface;
  int umlnewfiletype;
#endif // VDMPP

  QString homeDir;

private slots:
  void accept();
  void reject();
  void selectStyle( const QString& style );

private:
  QString windowName;
};

#endif // __tooloptionsdialog_h__
