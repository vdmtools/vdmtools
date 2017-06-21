/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/pngconv.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2006/06/27 02:35:29 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <iostream>
using namespace std;

#if QTVER >= 4
#include <QtGui/QtGui>
#else
#include <qapplication.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qbuffer.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qcstring.h>
#include <qmessagebox.h>
#endif // QTVER >= 4

QPixmap getPNGImage( const QString & filename )
{
  QPixmap img;
  if(!img.load(filename))
#if QT_VERSION >= 0x040000
    wcout << L"Warning: unable to read image file " << filename.toLatin1().data() << endl;
#else
    wcout << L"Warning: unable to read image file " << filename << endl;
#endif // QT_VERSION >= 0x040000
  return img;
}

QStringList Pixmap2StringList( const QPixmap & pm )
{
  QByteArray baraw;
#if QT_VERSION >= 0x040000
  QBuffer buffer ( &baraw, NULL );
  buffer.open( QIODevice::WriteOnly );
#else
  QBuffer buffer ( baraw );
  buffer.open( IO_WriteOnly );
#endif // QT_VERSION >= 0x040000
  pm.save( &buffer, "PNG", 100 );
  buffer.close();
  QByteArray ba (qCompress(baraw));

  QStringList list;
  QString str;
  int length = ba.size();
  int count = 0;
  for( int i = 0; i < length; i++ )
  {
    unsigned char c = ba[i];
    char buf[3];
    sprintf (buf, "%02X", c);
    str += QString(buf);
    count++;
    if( count == 20 )
    {
      list.append( str );
      str = "";
      count = 0;
    }
  }
  if( count > 0 )
  {
    list.append( str );
  }
  return list;
}

QString toXPM( const QPixmap& pmap, const QString& name )
{
#if QT_VERSION >= 0x040000
  QByteArray ba;
  QBuffer buffer( &ba, NULL );
  buffer.open( QIODevice::WriteOnly );
#else
  QCString ba;
  QBuffer buffer( ba );
  buffer.open( IO_WriteOnly );
#endif // QT_VERSION >= 0x040000
//  pmap.save( &buffer, "XPM", -1 );
  pmap.save( &buffer, "XPM", 100 );
  
  QString xpm( ba );
  QString before( "*dummy[]" );
  QString after = "*" + name + "_image[]";
  xpm.replace( before, after );
  return xpm;
}

QStringList getFileList( const QString& dir )
{
  QFileInfo fi( dir );
  QStringList res;
  if( fi.isDir() )
  {
    QDir d( dir );
#if QT_VERSION >= 0x040000
    QStringList fi;
    fi.append(QString("*.png"));
    res = d.entryList( fi ); 
#else
    res = d.entryList( "*.png" ); 
#endif // QT_VERSION >= 0x040000
  }
  return res;
}

void WriteData( QTextStream & ofs, const QString name, const QStringList & list )
{
  QString s = "static const char *" + name + "_image[]={";
  ofs << s << endl;
  for( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
  {
    ofs << "\"" << *it << "\"," << endl;
  }
  ofs << "0,};"<< endl;
}

void genImage( QTextStream & ofs, const QString & pngpath  )
{
  QStringList list = getFileList( pngpath );
  for( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    QString filename(*it);
    QString imagefile = pngpath + "/" + filename;
    QFileInfo fi( imagefile );
    QString basename = fi.baseName();
    QPixmap pmap = getPNGImage( imagefile );
//    QString xpm = toXPM( pmap, basename );
//    ofs << xpm << endl;
    QStringList sl = Pixmap2StringList( pmap );
    WriteData( ofs, basename, sl );
  }
}

void genMethod( QTextStream & ofs, const QString & pngpath  )
{
  ofs << endl;
  ofs << "QPixmap getImageData( const QString& fname )" << endl;
  ofs << "{" << endl;
  QStringList list = getFileList( pngpath );
  for( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    QString filename(*it);
    QString imagefile = pngpath + "/" + filename;
    QFileInfo fi( imagefile );
    QString basename = fi.baseName();
    ofs << "  if( fname == \"" << filename << "\" )";
//    ofs << " return QPixmap( (const char **)" << basename << "_image );" << endl;
    ofs << " return Data2Pixmap( " << basename << "_image );" << endl;
    
  }
  ofs << "  return QPixmap();" << endl;
  ofs << "}" << endl;
}

void genHeader( QTextStream & ofs )
{
  ofs << "// This file is generated automaticaly." << endl;
  ofs << endl;
  ofs << "#if QTVER >= 4" << endl;
  ofs << "#include <QtGui/QtGui>" << endl;
  ofs << "#else" << endl;
  ofs << "#include <qpixmap.h>" << endl;
  ofs << "#include <qstring.h>" << endl;
  ofs << "#include <qstringlist.h>" << endl;
  ofs << "#include <qcstring.h>" << endl;
  ofs << "#endif // QTVER >= 4" << endl;
  ofs << endl;
}

void genConv( QTextStream & ofs )
{
  ofs << endl;
  ofs << "QPixmap Data2Pixmap( const char * data[] )" << endl;
  ofs << "{"                                          << endl;
  ofs << "  QString str;"                             << endl;
  ofs << "  int count = 0;"                           << endl;
  ofs << "  while( data[count] != 0 )"                << endl;
  ofs << "  {"                                        << endl;
  ofs << "    str += QString(data[count]);"           << endl;
  ofs << "    count++;"                               << endl;
  ofs << "  }"                                        << endl;
  ofs << "  int size = str.length()/2;"               << endl;
  ofs << "#if QT_VERSION >= 0x040000"                 << endl;
  ofs << "  QByteArray ba(size, ' ');"                << endl;
  ofs << "#else"                                      << endl;
  ofs << "  QByteArray ba(size);"                     << endl;
  ofs << "#endif // QT_VERSION >= 0x040000"           << endl;
  ofs << "  for(int i = 0; i < size; i++)"            << endl;
  ofs << "  {"                                        << endl;
  ofs << "    QString s (str.mid(i * 2, 2 ));"        << endl;
  ofs << "    bool ok;"                               << endl;
  ofs << "    ba[i] = s.toInt( &ok, 16 );"            << endl;
  ofs << "  }"                                        << endl;
  ofs << "  QPixmap pm;"                              << endl;
  ofs << "  pm.loadFromData( qUncompress(ba), \"PNG\" ); " << endl;
  ofs << "  return pm;"                               << endl;
  ofs << "}"                                          << endl;
}

void genFile( const QString & filename, const QString & pngpath )
{
  QFile of( filename );
#if QT_VERSION >= 0x040000
  if( !of.open( QIODevice::WriteOnly ) ) 
#else
  if( !of.open( IO_WriteOnly ) ) 
#endif // QT_VERSION >= 0x040000
  {
    wcout << L"open failed" << endl;
    return;
  } 
  QTextStream ofs( &of ); 

  genHeader( ofs );
  genImage( ofs, pngpath );
  genConv( ofs );
  genMethod( ofs, pngpath );

  of.close();
}

int main( int argc, char * argv[] )
{
  QApplication app( argc, argv );

  QString path( "./img" );
  QString filename( "imagedata.cc" );

  genFile( filename, path );

  return 0;
}
