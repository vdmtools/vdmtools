/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/localeutil.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2005/11/08 08:13:13 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __localeutil_h__
#define __localeutil_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#else
#include <qstring.h>
#include <qstringlist.h>
#endif // QTVER >= 4

class QTLOCALE
{
public:
  static QString getLanguage();
  static QString getAltLang(const QString & lang);
  static QStringList getMenuList();
  static QString MenuNameToCharSet(const QString & name);
  static QString MenuNameToCodecName(const QString & name);
  static QString Q3CodecNameToMenuName(const QString & name);
  static QString NumberToMenuName(int num);

private:
  static QString getDefaultCodec();
  static QString checkQCodec(const QString & name);
};

#endif // __localeutil_h__
