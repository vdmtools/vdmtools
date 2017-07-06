/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/splashF.h,v $
*  * VERSION
*  *    $Revision: 1.9 $
*  * DATE
*  *    $Date: 2006/05/18 08:19:00 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __splashw_h__
#define __splashw_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#endif
class splashW : public QSplashScreen
{
  public:
    splashW() : QSplashScreen( GetSplashImage() ) {};
    ~splashW() {};
  private:
    static const char *VDMToolsSplash_image[];
    static QPixmap GetSplashImage();
    static QPixmap Data2Pixmap( const char * data[] );
};
#else
#include <qpixmap.h>

#if QT_VERSION >= 0x030200
#include <qsplashscreen.h>
class splashW : public QSplashScreen
{
  public:
    splashW() : QSplashScreen( GetSplashImage() ) {};
    ~splashW() {};
  private:
    static const char *VDMToolsSplash_image[];
    static QPixmap GetSplashImage();
    static QPixmap Data2Pixmap( const char * data[] );
};
#else
#include <qlabel.h>
class splashW : public QLabel
{
  public:
    splashW() : QLabel( 0, "Splash", WStyle_Splash )
    { setPixmap( GetSplashImage() ); };
    ~splashW() {};
    void show() { QLabel::show(); repaint(); };
    void finish( QWidget * mainWin ) { hide(); };
  private:
    static const char *VDMToolsSplash_image[];
    static QPixmap GetSplashImage();
    static QPixmap Data2Pixmap( const char * data[] );
};
#endif // QT_VERSION
#endif // QTVER >= 4

#endif // __splashw_h__
