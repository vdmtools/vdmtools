#ifndef __qtport_h__
#define __qtport_h__

#if QTVER >= 4
#if QTVER >= 5
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif
#else
#include <qstring.h>
#include <qstringlist.h>
#include <qfiledialog.h>
#endif // QTVER >= 4

class QtPort
{
public:
static QString QtGetExistingDirectory(QWidget * parent = 0,
                                      const QString & caption = QString(),
                                      const QString & dir = QString());
static QString QtGetOpenFileName(QWidget * parent = 0,
                                 const QString & caption = QString(),
                                 const QString & dir = QString(),
                                 const QString & filter = QString(),
                                 QString * selectedFilter = 0);
static QStringList QtGetOpenFileNames(QWidget * parent = 0,
                                      const QString & caption = QString(),
                                      const QString & dir = QString(),
                                      const QString & filter = QString(),
                                      QString * selectedFilter = 0);
static QString QtGetSaveFileName(QWidget * parent = 0,
                                 const QString & caption = QString(),
                                 const QString & dir = QString(),
                                 const QString & filter = QString(),
                                 QString * selectedFilter = 0);
};
#endif // __qtport_h__
