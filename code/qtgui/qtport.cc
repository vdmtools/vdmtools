#include "qtport.h"

QString QtPort::QtGetExistingDirectory(QWidget * parent, const QString & caption, const QString & dir)
{
#if QT_VERSION >= 0x040000
  return QFileDialog::getExistingDirectory( parent, caption, dir );
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
  return QFileDialog::getExistingDirectory( dir, parent, QString::null, caption );
#endif // QT_VERSION < 0x040000
}

QString QtPort::QtGetOpenFileName(QWidget * parent,
                                  const QString & caption,
                                  const QString & dir,
                                  const QString & filter,
                                  QString * selectedFilter)
{
#if QT_VERSION >= 0x040000
  return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter );
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
  return QFileDialog::getOpenFileName( dir, filter, parent, QString::null, caption, selectedFilter );
#endif // QT_VERSION < 0x040000
}

QStringList QtPort::QtGetOpenFileNames(QWidget * parent,
                                       const QString & caption,
                                       const QString & dir,
                                       const QString & filter,
                                       QString * selectedFilter)
{
#if QT_VERSION >= 0x040000
  return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter );
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
  return QFileDialog::getOpenFileNames( filter, dir, parent, QString::null, caption, selectedFilter );
#endif // QT_VERSION < 0x040000
}

QString QtPort::QtGetSaveFileName(QWidget * parent,
                                  const QString & caption,
                                  const QString & dir,
                                  const QString & filter,
                                  QString * selectedFilter)
{
#if QT_VERSION >= 0x040000
  return QFileDialog::getSaveFileName( parent, caption, dir, filter, selectedFilter );
#endif // QT_VERSION >= 0x040000
#if QT_VERSION < 0x040000
  return QFileDialog::getSaveFileName( dir, filter, parent, QString::null, caption, selectedFilter );
#endif // QT_VERSION < 0x040000
}
