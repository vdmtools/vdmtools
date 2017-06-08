/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/referencesF.h,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __referencesw_h_
#define __referencesw_h__

#ifdef QT4
#include <QtGui/QtGui>
#define QLISTWIDGET QListWidget
#define WFlags Qt::WindowFlags
#else
#include <qvariant.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#define QLISTWIDGET QListBox
#endif // QT4

class referencesW : public QDialog
{ 
    Q_OBJECT

public:
    referencesW( QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~referencesW();

    QGroupBox* GroupBox1;
    QLISTWIDGET* classesLB;
    QGroupBox* GroupBox2;
    QLISTWIDGET* usesLB;
    QGroupBox* GroupBox3;
    QLISTWIDGET* usedLB;
    QPushButton* buttonOk;

protected:
    QVBoxLayout* referencesWLayout;
    QHBoxLayout* Layout4;
    QVBoxLayout* GroupBox1Layout;
    QVBoxLayout* Layout3;
    QVBoxLayout* GroupBox2Layout;
    QVBoxLayout* GroupBox3Layout;
    QHBoxLayout* Layout2;
};

#endif // __referencesw_h__
