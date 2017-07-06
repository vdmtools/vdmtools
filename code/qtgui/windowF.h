/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/windowF.h,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __windowf_h__
#define __windowf_h__

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
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#endif // QTVER >= 4

class windowF : public QDialog
{ 
    Q_OBJECT

public:
    windowF( QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~windowF();

    QGroupBox* ButtonGroup1;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;
    QRadioButton* RadioButton3;
    QRadioButton* RadioButton4;
    QGroupBox* ButtonGroup2;
    QCheckBox* CheckBox1;
    QCheckBox* CheckBox2;
    QCheckBox* CheckBox3;
    QPushButton* PushButton1;
    QPushButton* PushButton2;

protected:
    QVBoxLayout* windowFLayout;
    QHBoxLayout* Layout1;
    QVBoxLayout* ButtonGroup1Layout;
    QVBoxLayout* ButtonGroup2Layout;
    QHBoxLayout* Layout2;
};

#endif // __windowf_h__
