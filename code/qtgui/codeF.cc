/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/codeF.cc,v $
*  * VERSION
*  *    $Revision: 1.53 $
*  * DATE
*  *    $Date: 2006/06/27 06:48:41 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "codeF.h"
#include "interface2TB.h"
#include "tb_wstring.h"

#if QT_VERSION < 0x040000
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include <qlayout.h>
#endif // QT_VERSION < 0x040000

#ifdef _MSC_VER
#include <process.h>
#include "tbutils.h"
#include <windows.h>
#else
#include <unistd.h>
#endif //_MSC_VER

#define HEADER_SIZE 7

/*
 *  Constructs a codeW which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
codeW::codeW( QWidget* parent,  const char* name, WFlags fl )
#if QT_VERSION >= 0x040000
    : QWidget( parent, fl )
#else
    : QWidget( parent, name, fl )
#endif // QT_VERSION >= 0x040000
{
  // Initialize member variables

  this->setWindowName( (name == NULL) ? "codeW" : name );

#if QT_VERSION >= 0x040000
  this->setWindowTitle( tr( "Source Window" ) );
#else
  if ( !name ) setName( "codeW" );
  this->setCaption( tr( "Source Window" ) );
#endif // QT_VERSION >= 0x040000

//  this->setFont( QApplication::font() );

  this->mainTAB = new QTabWidget( this );

  QVBoxLayout* layout = new QVBoxLayout( this );
  layout->setSpacing( 0 );
  layout->setMargin( 0 );
  layout->addWidget( this->mainTAB );

  this->setupDefaultColor();

#if QT_VERSION >= 0x040000
  this->oldState = this->windowState();
#endif // QT_VERSION >= 0x040000
  this->openAtMinimized = false;

  this->pcount = 0;

#if QTVER >= 4
    int fontSize = 12;
    QFont font = QFont("monospace", fontSize);
    font.setStyleHint(QFont::TypeWriter);
    int fontPxSize = QFontMetrics(font).width('0');
    this->currentFont = font;
#endif // QTVER >= 4
}

/*
 *  Destroys the object and frees any allocated resources
 */
codeW::~codeW()
{
  // no need to delete child widgets, Qt does it all for us
}

bool codeW::event (QEvent * e)
{
#if QT_VERSION >= 0x040800
#ifdef __APPLE_CC__
  if (e->type() == QEvent::Paint)
  {
    if (this->pcount < 2)
    {
      this->repaint();
      this->pcount++;
    }
    else
      this->pcount = 0;
  }
#endif // __APPLE_CC__
#endif // QT_VERSION >= 0x040000
  return QWidget::event(e);
}


void codeW::resizeEvent(QResizeEvent * e)
{
  QWidget::resizeEvent(e);
  if( this->mainTAB != NULL )
    this->mainTAB->setMaximumSize(this->size());
}

//
// Open file with name "filename" in a tab with label "title"
//
void codeW::open(const QString & title, const QString & filenameI, bool coloring)
{
#if QT_VERSION >= 0x040000
  this->mainTAB->setMaximumSize(this->parentWidget()->size());
#else
  this->mainTAB->setMaximumSize(this->size());
#endif // QT_VERSION >= 0x040000

  //test if exists
  if (this->tabMap.contains(title))
  {
    //if exists show it
#if QT_VERSION >= 0x040000
    this->mainTAB->setCurrentIndex(this->mainTAB->indexOf( this->tabMap[title] ));
#else
    this->mainTAB->showPage( this->tabMap[title] );
#endif // QT_VERSION >= 0x040000
    return;
  }
  else
  {
    //if doesn't exist create it
    QFile f( filenameI );
#if QT_VERSION >= 0x040000
    if ( f.open( QIODevice::ReadOnly ) ) 
#else
    if ( f.open( IO_ReadOnly ) ) 
#endif // QT_VERSION >= 0x040000
    {
      QWidget* tab = new QWidget(this);
      QVBoxLayout* layout = new QVBoxLayout(tab);
      layout->setSpacing( 0 );
      layout->setMargin( 0 );

      MyTextEdit * edit = this->createEdit( tab ); 

      QString tabName (QFileInfo(title).fileName());
      this->readText( f, edit, tabName, coloring );

      f.close();

      layout->addWidget(edit);

#if QT_VERSION >= 0x040000
      this->mainTAB->addTab(tab, tabName); 
#else
      this->mainTAB->insertTab(tab, tabName); 
#endif // QT_VERSION >= 0x040000

      //insert it on lists
      this->tabMap[title] = tab;
      this->editMap[title] = edit;
      this->fileMap[title] = filenameI;
#if QT_VERSION >= 0x040000
      this->mainTAB->setCurrentIndex(this->mainTAB->indexOf( tab ));
#else
      this->mainTAB->showPage(tab);
#endif // QT_VERSION >= 0x040000
    }
  }
}

MyTextEdit * codeW::createEdit(QWidget * parent)
{
  MyTextEdit* edit = new MyTextEdit(parent);
#if QT_VERSION >= 0x040000
  edit->setFont(this->currentFont);
  edit->setAcceptRichText( false );
  edit->setReadOnly(true);
  edit->ensureCursorVisible();
  edit->setTabStopWidth(20);

  QObject::connect(edit, SIGNAL(setBreakPoint()), this, SLOT(setBreakPoint()));
  QObject::connect(edit, SIGNAL(runCommand(QString)), this, SLOT(runCommand(QString)));
  QObject::connect(edit, SIGNAL(ip_init()), this, SLOT(ipInit()));
  QObject::connect(edit, SIGNAL(ip_cont()), this, SLOT(ipCont()));
  QObject::connect(edit, SIGNAL(ip_step()), this, SLOT(ipStep()));
  QObject::connect(edit, SIGNAL(ip_stop()), this, SLOT(ipStop()));
  QObject::connect(edit, SIGNAL(cd_editor()), this, SLOT(cdEditor()));
#else
  edit->setTextFormat( Qt::PlainText );
  edit->setReadOnly(true);
  edit->ensureCursorVisible();
  edit->setTabStopWidth(20);
//
  edit->setSelectionAttributes(1, QColor(Qt::white), true);
  edit->setSelectionAttributes(1, QColor("#375"), true);
  QObject::connect(edit, SIGNAL(doubleClicked( int, int )), this, SLOT(setBreakPoint()));
#endif // QT_VERSION >= 0x040000

  return edit;
}

void codeW::readText(QFile & f, MyTextEdit * edit, const QString & title, bool coloring)
{
  // read text
  QTextStream t (&f);
#if QT_VERSION >= 0x050000
  QTextCodec * codec = QTextCodec::codecForLocale();
#else
  QTextCodec * codec = QTextCodec::codecForTr();
#endif // QT_VERSION >= 0x040000
  if( codec != 0 ) t.setCodec( codec );

  GUITokenList tl;
  GUITokenList::Iterator git;
  if (coloring)
  {
    tl = Qt2TB::getGUITokenListI( title );
    git = tl.begin();
  }

  int count = 1;
  char buf[ HEADER_SIZE + 1 ];
  while ( !t.atEnd() )
  {
    QString s (t.readLine());
    sprintf( buf, "%5d: ", count );
    QString line_number( buf ); 
    if( !coloring )
    {
      edit->append( line_number + s );
    }
    else
    {
#if QT_VERSION >= 0x040000
      edit->setTextColor( *c_header );
      edit->setFontWeight( QFont::Normal );
#else
      edit->setColor( *c_header );
      edit->setBold( false );
#endif // QT_VERSION >= 0x040000
      edit->append( line_number );
      QColor * pcolor = c_header;

      // coloring
      if( git == tl.end() )
      {
#if QT_VERSION >= 0x040000
        if( !s.simplified().isEmpty() )
#else
        if( !s.stripWhiteSpace().isEmpty() )
#endif // QT_VERSION >= 0x040000
        {
          if( pcolor != c_comment )
          {
#if QT_VERSION >= 0x040000
            edit->setTextColor( *c_comment );
            edit->setFontWeight( QFont::Normal );
#else
            edit->setColor( *c_comment );
            edit->setBold( false );
#endif // QT_VERSION >= 0x040000
            pcolor = c_comment;
          }
#if QT_VERSION >= 0x040000
          edit->insertPlainText(s);
#else
          edit->insertAt( s, count - 1, HEADER_SIZE );
#endif // QT_VERSION >= 0x040000
        }
      }
      else
      {
        int line = (*git).getLine();
        if( count < line )
        {
#if QT_VERSION >= 0x040000
          if( !s.simplified().isEmpty() )
#else
          if( !s.stripWhiteSpace().isEmpty() )
#endif // QT_VERSION >= 0x040000
          {
            if( pcolor != c_comment )
            {
#if QT_VERSION >= 0x040000
              edit->setTextColor( *c_comment );
              edit->setFontWeight( QFont::Normal );
#else
              edit->setColor( *c_comment );
              edit->setBold( false );
#endif // QT_VERSION >= 0x040000
              pcolor = c_comment;
            }
#if QT_VERSION >= 0x040000
            edit->insertPlainText(s);
#else
            edit->insertAt( s, count - 1, HEADER_SIZE );
#endif // QT_VERSION >= 0x040000
          }
        }
        else
        {
          int index = 0;
          int offset = 0;
          while( index < (int)s.length() )
          {
            int col = (*git).getCol() - 1 + offset;
            int len = (*git).getLen();
//            int adjust = 0;
            if( index < col )
            {
              QString tmp = s.mid( index, col - index );
#if QT_VERSION >= 0x040000
              if( !s.simplified().isEmpty() )
#else
              if( !s.stripWhiteSpace().isEmpty() )
#endif // QT_VERSION >= 0x040000
              {
                if( pcolor != c_comment )
                {
#if QT_VERSION >= 0x040000
                  edit->setTextColor( *c_comment );
                  edit->setFontWeight( QFont::Normal );
#else
                  edit->setColor( *c_comment );
                  edit->setBold( false );
#endif // QT_VERSION >= 0x040000
                  pcolor = c_comment;
                }
              }
#if QT_VERSION >= 0x040000
              edit->insertPlainText(tmp);
#else
              edit->insertAt( tmp, count - 1, HEADER_SIZE + index );
#endif // QT_VERSION >= 0x040000
              index = col;
            }
            QColor * color = this->c_comment;
            if( (*git).isComment() )
            {
              color = this->c_comment;
            }
            else if( (*git).isDefinition() ) // 
            {
              color = this->c_definition;
            }
            else if( (*git).isReserve() ) // reserved word
            {
              color = this->c_reserve;
            }
            else if( (*git).isOperator() ) // operator
            {
              color = this->c_reserve;
            }
            else if( (*git).isPoly() ) // @
            {
              color = this->c_poly;
            }
            else if( (*git).isParenthesis() ) // parenthesis
            {
              color = this->c_parenthesis;
            }
            else if( (*git).isLiteral() ) // literal
            {
              color = this->c_literal;
            }
            else if( (*git).isCharLiteral() )
            {
              color = this->c_literal;
#if QT_VERSION >= 0x040000
#else
/*
              adjust = 2;
              if( len == 2 )
              {
                QString str = s.mid( index, len );
                if( str == "\"\"" ) adjust = 0;
              }
              offset += adjust;
              len += adjust;
*/
#endif // QT_VERSION >= 0x040000
            }
            else if( (*git).isIdentifier() )
            {
              QString str = s.mid( index, len );
              if( str == "RESULT" )
                color = this->c_reserve;
              else
                color = this->c_identifier;
            }
            QString tmp = s.mid( index , len );
#if QT_VERSION >= 0x040000
            if( !s.simplified().isEmpty() )
#else
            if( !s.stripWhiteSpace().isEmpty() )
#endif // QT_VERSION >= 0x040000
            {
              if( pcolor != color )
              {
#if QT_VERSION >= 0x040000
                edit->setTextColor( *color );
                if ((color == this->c_reserve) || (color == this->c_definition))
                  edit->setFontWeight( QFont::Bold );
                else
                  edit->setFontWeight( QFont::Normal );
#else
                edit->setColor( *color );
                if ((color == this->c_reserve) || (color == this->c_definition))
                  edit->setBold( true );
                else
                  edit->setBold( false );
#endif // QT_VERSION >= 0x040000
                pcolor = color;
              }
            }
#if QT_VERSION >= 0x040000
            edit->insertPlainText(tmp);
#else
            edit->insertAt( tmp, count - 1, HEADER_SIZE + index );
#endif // QT_VERSION >= 0x040000
            index += len;
            git++;
            if( git == tl.end() )
            {
              QString tmp = s.mid( index );
#if QT_VERSION >= 0x040000
              if( !s.simplified().isEmpty() )
#else
              if( !s.stripWhiteSpace().isEmpty() )
#endif // QT_VERSION >= 0x040000
              {
                if( pcolor != c_comment )
                {
#if QT_VERSION >= 0x040000
                  edit->setTextColor( *c_comment );
                  edit->setFontWeight( QFont::Normal );
#else
                  edit->setColor( *c_comment );
                  edit->setBold( false );
#endif // QT_VERSION >= 0x040000
                  pcolor = c_comment;
                }
#if QT_VERSION >= 0x040000
                edit->insertPlainText(tmp);
#else
                edit->insertAt( tmp, count - 1, HEADER_SIZE + index );
#endif // QT_VERSION >= 0x040000
              }
              break;
            }
            line = (*git).getLine();
            if( count < line )
            {
              QString tmp = s.mid( index );
#if QT_VERSION >= 0x040000
              if( !s.simplified().isEmpty() )
#else
              if( !s.stripWhiteSpace().isEmpty() )
#endif // QT_VERSION >= 0x040000
              {
                if( pcolor != c_comment )
                {
#if QT_VERSION >= 0x040000
                  edit->setTextColor( *c_comment );
                  edit->setFontWeight( QFont::Normal );
#else
                  edit->setColor( *c_comment );
                  edit->setBold( false );
#endif // QT_VERSION >= 0x040000
                  pcolor = c_comment;
                }
#if QT_VERSION >= 0x040000
                edit->insertPlainText(tmp);
#else
                edit->insertAt( tmp, count - 1, HEADER_SIZE + index );
#endif // QT_VERSION >= 0x040000
              }
              break;
            } 
          }
        }
      }
    }
    count++;
  }
}

// openIn
void codeW::openIn(const QString & title, const QString & filenameI_, int lineI, int colI, int length, bool coloring)
{
  QString filenameI (filenameI_);
  if (filenameI.isEmpty())
  {
    filenameI = Qt2TB::getSourceNameI(title); // TODO
  }

  this->oldTitle = title;
  this->oldFilename = filenameI;
  this->oldLine = lineI;
  this->oldCol = colI;
  this->oldLength = length;

  bool syntaxOk = Qt2TB::syntaxCheck(title);

  this->open(title, filenameI, coloring && syntaxOk);

  if (!this->tabMap.contains(title))
    return ; //something wicked has happened

#if QT_VERSION >= 0x040000
  if (this->windowState() == Qt::WindowMinimized)
  {
    this->openAtMinimized = true;
    return;
  }
  this->openAtMinimized = false;
#endif // QT_VERSION >= 0x040000

  if ((lineI == 1) && (colI == 1) && (length == 1))
  {
#if QT_VERSION >= 0x040000
    QTextCursor tc (this->editMap[title]->textCursor());
    tc.clearSelection();
    tc.movePosition(QTextCursor::Start);
    this->editMap[title]->setTextCursor(tc);
#else
    this->editMap[title]->setCursorPosition(0, 0);
#endif // QT_VERSION >= 0x040000
  }
  else
  {
    int para = lineI - 1;
#if QT_VERSION >= 0x040000
    QTextCursor tc (this->editMap[title]->textCursor());
    tc.clearSelection();
    tc.movePosition(QTextCursor::Start);

    int count = 0;
    while (tc.blockNumber() < para)
    {
      tc.movePosition(QTextCursor::Down);
      count++;
      if (!syntaxOk && (count >= para))
        break;
    }

    if (syntaxOk)
    {
      int cpos = colI - 1 + codeW::offset();
      int oldpos = tc.columnNumber() - 1;
      while ((tc.columnNumber() < cpos) && (oldpos < tc.columnNumber()))
      {
        oldpos = tc.columnNumber();
        tc.movePosition(QTextCursor::Right);
      }
      tc.select(QTextCursor::WordUnderCursor);
    }
    this->editMap[title]->setTextCursor(tc);

    // ExtraSelection
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor("#9DB");

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = tc;
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    this->editMap[title]->setExtraSelections(extraSelections);
    //

    this->editMap[title]->update();
#else
    int index = colI - 1 + codeW::offset();
    this->editMap[title]->setSelection(para, index, para, index + length);
    this->editMap[title]->setSelection(para, 0, para, codeW::offset() - 1, 1);
    this->editMap[title]->setCursorPosition(para, 0);
#endif // QT_VERSION >= 0x040000
  }
  this->update();
}

void codeW::closeFile(const QString & title)
{
  if (!this->tabMap.contains(title))
    return ; //file not open

#if QT_VERSION >= 0x040000
  this->mainTAB->removeTab(this->mainTAB->indexOf(this->tabMap[title]));
#else
  this->mainTAB->removePage(this->tabMap[title]);
#endif // QT_VERSION >= 0x040000

  delete this->tabMap[title];
  this->tabMap.remove(title);
  this->editMap.remove(title);
  this->fileMap.remove(title);
  if (this->tabMap.isEmpty())
  {
#if QT_VERSION >= 0x040000
    this->parentWidget()->hide();
#else
    this->hide();
#endif // QT_VERSION >= 0x040000
  }
}

//
// Close the file corresponding to the tab currently selected
//
void codeW::closeSelectedFile()
{
  QString title (getCurrentTitle());
  if (!title.isEmpty())
  {
    this->closeFile(title);
  }
}

int codeW::offset()
{
  return HEADER_SIZE;
}

//
// Edit the files with names given by the argument "files" using the
// editor with name "editorName"
//
void codeW::externalEdit(const QStringList& files, const QString& editorName)
{
  for (QStringList::ConstIterator it = files.begin(); it != files.end(); it++)
  {
#ifdef _MSC_VER  
    wstring winName (TBUTILS::MakeWindowsName(Qt2TB::qstring2wstring(*it)));
    QString winNameSpace (Qt2TB::wstring2qstring(winName));
    //    winNameSpace.replace(QRegExp(" "), "\\ ");
    winNameSpace.prepend('\"');
    winNameSpace.append('\"');
    _wspawnl(_P_NOWAIT, 
            Qt2TB::qstring2wstring(editorName).c_str(), 
            Qt2TB::qstring2wstring(winNameSpace).c_str(), 
            Qt2TB::qstring2wstring(winNameSpace).c_str(), 
            (const wchar_t*) NULL);
#else
    int pid = fork();
    if (pid == 0) // child process
    {
#ifdef __APPLE_CC__
#if QT_VERSION >= 0x040000
      execlp("open",
             "-a" ,
             (const char*) editorName.toLatin1().data(), 
             (*it).toLatin1().data(),
             (*it).toLatin1().data(),
             (const char*) NULL);
#else
      execlp("open",
             "-a",
             (const char*) editorName.latin1(), 
             (*it).latin1(),
             (*it).latin1(),
             (const char*) NULL);
#endif // QT_VERSION >= 0x040000
#else
#if QT_VERSION >= 0x040000
      execlp((const char*) editorName.toLatin1().data(), 
             (*it).toLatin1().data(),
             (*it).toLatin1().data(),
             (const char*) NULL);
#else
      execlp((const char*) editorName.latin1(), 
             (*it).latin1(),
             (*it).latin1(),
             (const char*) NULL);
#endif // QT_VERSION >= 0x040000
#endif // __APPLE_CC__
    }
#endif //_MSC_VER
  }
}

void codeW::openCurrentFileWithEditor(const QString & editor, const QString & format)
{
  QString title (getCurrentTitle());
  if (!title.isEmpty())
  {
#if QT_VERSION >= 0x040000
    QTextCursor tc = this->editMap[title]->textCursor();
    int paraFrom = tc.blockNumber();
    int indexFrom = tc.columnNumber();
#else
    int paraFrom = 0, indexFrom = 0;
    int paraTo = 0, indexTo = 0;
    this->editMap[title]->getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
#endif // QT_VERSION >= 0x040000

    int line = paraFrom + 1;
    int col = indexFrom + 1 - codeW::offset();
    if( col < 1 ) col = 1;

    openFileWithEditor(editor, format, title, line, col);
  }
}

void codeW::openFileWithEditor(const QString & editor_, const QString & format_, const QString & file,
                               int line, int col)
{
  if (editor_.isEmpty() || file.isEmpty())
    return;

  QString format (format_);
#if QT_VERSION >= 0x040000
  if (file.toLower().endsWith(".rtf"))
#else
  if (file.lower().endsWith(".rtf"))
#endif // QT_VERSION >= 0x040000
  {
//    emit logWrite(QString(tr("External editor function does't support to edit rtf file.")));
//    return;
#ifdef _MSC_VER  
    format = QString("\"%f\"");
#else
    format = QString("%f");
#endif // _MSC_VER
  }

#ifdef _MSC_VER  
  QString editor (ConvertToShortPathName(editor_));
#else
  QString editor (editor_);
#endif // _MSC_VER
  if (format.isEmpty())
  {
#ifdef _MSC_VER  
#if QT_VERSION >= 0x040000
    QString lweditor (editor.toLower());
#else
    QString lweditor (editor.lower());
#endif // QT_VERSION >= 0x040000
    if(lweditor.endsWith("sakura.exe"))
      format = QString("-Y=%l -X=%c \"%f\"");
    else if(lweditor.endsWith("hidemaru.exe"))
      format = QString("/j%l,%c \"%f\"");
    else if(lweditor.endsWith("terapad.exe"))
      format = QString("/jl=%l \"%f\"");
    else if(lweditor.endsWith("gvim.exe"))
      format = QString("%l \"%f\"");
    else if(lweditor.endsWith("emacs.exe"))
      format = QString("+%l:%c \"%f\"");
    else
      format = QString("\"%f\"");
#else
    if(editor.endsWith("emacs"))
      format = QString("+%l:%c %f");
    else
      format = QString("%f");
#endif // _MSC_VER
  }

  QString lineStr;
  lineStr.sprintf("%d", line); 
  QString colStr;
  colStr.sprintf("%d", col); 
  
#if QT_VERSION >= 0x040000
  QStringList arg_l (format.split(' '));
#else
  QStringList arg_l (QStringList::split(' ', format));
#endif // QT_VERSION >= 0x040000

  QStringList args;
  for (QStringList::const_iterator it0 = arg_l.begin(); it0 != arg_l.end(); ++it0)
  {
    QString arg (*it0);
#ifdef _MSC_VER  
    arg.replace("%f", ConvertToShortPathName(file));
#else
    arg.replace("%f", file);
#endif // _MSC_VER
    arg.replace("%l", lineStr);
    arg.replace("%c", colStr);
    args.append(arg);
  }

#ifdef _MSC_VER  
  wchar_t ** argv = new wchar_t *[args.size() + 2];
  wstring cstrapp (Qt2TB::qstring2wstring(editor));
  size_t len_cstrapp = cstrapp.length() + 1;
  argv[0] = new wchar_t[len_cstrapp];
  wcscpy_s(argv[0], len_cstrapp, cstrapp.c_str());
  int index = 1;
  for (QStringList::const_iterator it = args.begin(); it != args.end(); ++it)
  {
    wstring cstr (Qt2TB::qstring2wstring(*it));
    size_t len_cstr = cstr.length() + 1;
    argv[index] = new wchar_t[len_cstr];
    wcscpy_s(argv[index], len_cstr, cstr.c_str());
    index++;
  }
  argv[index] = NULL;
  
  _wspawnv(_P_NOWAIT, cstrapp.c_str(), argv);

  for (int i = 0; i <= (int)args.size(); i++)
    delete[] argv[i];
  delete[] argv;
#else
  QString eapp (editor);
  QStringList eargs (args);

#ifdef __APPLE_CC__
  if (editor.endsWith(".app"))
  {
    eapp = QString("open");
    eargs.prepend(QString(editor)); 
    eargs.prepend(QString("-a"));
  }
#endif // __APPLE_CC__

  int pid = fork();
  if (pid == 0) // child process
  {
    char ** argv = new char *[eargs.size() + 2];
    string cstrapp (TBWSTR::wstring2coutstr(Qt2TB::qstring2wstring(eapp)));
    size_t len_cstrapp = cstrapp.length() + 1;
    argv[0] = new char[len_cstrapp];
    memcpy(argv[0], cstrapp.c_str(), len_cstrapp);
    int index = 1;
    for (QStringList::const_iterator it = eargs.begin(); it != eargs.end(); ++it)
    {
      string cstr (TBWSTR::wstring2coutstr(Qt2TB::qstring2wstring(*it)));
      size_t len_cstr = cstr.length() + 1;
      argv[index] = new char[len_cstr];
      memcpy(argv[index], cstr.c_str(), len_cstr);
      index++;
    }
    argv[index] = NULL;

    execvp(cstrapp.c_str(), argv);

    for (int i = 0; i <= (int)eargs.size(); i++)
      delete[] argv[i];
    delete[] argv;
  }
#endif // _MSC_VER
}

//
// codeW::refreshWindow
//
// This function is used to refresh a window, when the file displayed
// in that window  has been reparsed in the tool. This is to ensure
// that the version of the file displayed and the AST used internally
// are consistent
//
// Arguments:
//   QString title: The title of the window (name of the window's tab)
//   QString filenameI: 
//                  Actual path to window source (in the case of
//                  an RTF file, filenameI is the name of the
//                  temporary file containing the VDM chunks extracted
//                  from the RTF file. Otherwise filenameI is the full
//                  path from which title is extracted).
//
// Returns: no return value
void codeW::refreshWindow(const QString & title, const QString & filenameI, bool coloring)
{
  if (!this->tabMap.contains(title)) // File is not currently loaded in source viewer
    return;

  bool isShown = isVisible();
  this->closeFile(title);
  this->open(title, filenameI, coloring);
#if QT_VERSION >= 0x040000
  this->mainTAB->setCurrentIndex(this->mainTAB->indexOf( this->tabMap[title] ));
#else
  this->mainTAB->showPage(this->tabMap[title]);
#endif // QT_VERSION >= 0x040000
  if (isShown)
    this->show();
}

// 
// Close all of the windows currently open in the code window
//
void codeW::closeAll()
{
  QStringList keyList (this->tabMap.keys());
  for (QStringList::const_iterator it = keyList.begin(); it != keyList.end(); ++it)
    this->closeFile(*it);

#if QT_VERSION >= 0x040000
  this->parentWidget()->hide();
#else
  this->hide();
#endif // QT_VERSION >= 0x040000
}

//
// Set a breakpoint at the current cursor position
//
void codeW::setBreakPoint()
{
  QString title (getCurrentTitle());
  if (!title.isEmpty())
  {
#if QT_VERSION >= 0x040000
    QTextCursor tc = this->editMap[title]->textCursor();
    int paraFrom = tc.blockNumber();
    int indexFrom = tc.columnNumber();
#else
    int paraFrom = 0, indexFrom = 0;
    int paraTo = 0, indexTo = 0;
    this->editMap[title]->getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
#endif // QT_VERSION >= 0x040000

    int line = paraFrom + 1;
    int col = indexFrom + 1 - codeW::offset();
    if( col < 1 ) col = 1;

#if QT_VERSION >= 0x040000
    QString filename (QFileInfo(title).fileName().simplified());
#else
    QString filename (QFileInfo(title).fileName().stripWhiteSpace());
#endif // QT_VERSION >= 0x040000
    QString cmd (QString( "break %1 %2 %3" ).arg( filename ).arg( line ).arg( col ));
    emit executeCommand( cmd );
  }
}

void codeW::runCommand(QString cmd)
{
  emit executeCommand(cmd);
}

void codeW::ipInit()
{
  emit ip_init(false);
}

void codeW::ipCont()
{
  emit ip_cont(false);
}

void codeW::ipStep()
{
  emit ip_step(false);
}

void codeW::ipStop()
{
  emit ip_stop(false);
}

void codeW::cdEditor()
{
  emit cd_editor(false);
}

void codeW::setupDefaultColor()
{
  this->c_header =  new QColor("#777");
  this->c_comment = new QColor("#050");
  this->c_reserve = new QColor("#008");
  this->c_identifier = new QColor("#000");
  this->c_literal = new QColor("#828");
  this->c_poly = new QColor("#520");
  this->c_parenthesis = new QColor("#700");
  this->c_definition = new QColor("#44C");
}

int codeW::pages()
{
  return this->tabMap.size();
}

void codeW::selectAll()
{
  QString title (getCurrentTitle());
  if (!title.isEmpty())
  {
    if (!this->tabMap.contains(title)) 
      return ; //file not open

#if QT_VERSION >= 0x040000
    this->editMap[title]->selectAll();
#else
    this->editMap[title]->selectAll(true);
#endif // QT_VERSION >= 0x040000
  }
}

QString codeW::getCurrentTitle()
{
  if (this->tabMap.isEmpty())
    return QString("");

#ifdef __linux__
#if QT_VERSION >= 0x040000
  for (QMap<QString, QWidget*>::const_iterator it = this->tabMap.constBegin();
       it != this->tabMap.constEnd(); it++)
#else
// QT 3,1 has't constBegin/constEnd
  for (QMap<QString, QWidget*>::iterator it = this->tabMap.begin();
       it != this->tabMap.end(); it++)
#endif // QT_VERSION >= 0x040000
#else
  for (QMap<QString, QWidget*>::const_iterator it = this->tabMap.constBegin();
       it != this->tabMap.constEnd(); it++)
#endif // __linux__
  {
#if QT_VERSION >= 0x040000
    if (it.value() == this->mainTAB->currentWidget())
#else
    if (it.data() == this->mainTAB->currentPage())
#endif // QT_VERSION >= 0x040000
      return it.key();
  }
  return QString("");
}

void codeW::setBusy(bool busy)
{
}

//bool codeW::event ( QEvent * e )
//{
////wcout << e->type () << endl << flush;
//  return QWidget::event(e);
//}

#if QT_VERSION >= 0x040000
void MyTextEdit::contextMenuEvent ( QContextMenuEvent * event )
{
  QMenu * menu = createStandardContextMenu();
  menu->addSeparator();
  menu->addAction(tr("External Editor"));
  menu->addSeparator();
  menu->addAction(tr("Initialize"));
  menu->addAction(tr("Continue"));
  menu->addAction(tr("Step"));
  menu->addSeparator();
  menu->addAction(tr("Set Breakpoint"));
  QString sel (this->textCursor().selectedText());
  if (!sel.isEmpty())
  {
    CMENU_PRINTX = tr("Print %1").arg(sel);
    menu->addSeparator();
    //menu->addAction(QString("Print ") + sel);
    menu->addAction(CMENU_PRINTX);
  }
  QObject::connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(contextMenuTriggered(QAction *)));

  menu->exec(event->globalPos());
  delete menu;
}
#endif // QT_VERSION >= 0x040000

void MyTextEdit::contextMenuTriggered(QAction * a)
{
#if QT_VERSION >= 0x040000
  if (a->text() == tr("Initialize"))
    emit ip_init();
  else if (a->text() == tr("Continue"))
    emit ip_cont();
  else if (a->text() == tr("Step"))
    emit ip_step();
  else if (a->text() == tr("External Editor"))
    emit cd_editor();
  else if (a->text() == tr("Set Breakpoint"))
  {
    if (this->textCursor().hasSelection())
      emit setBreakPoint();
  }
  else if (a->text() == CMENU_PRINTX)
  {
    QString cmd (QString("print %1").arg(this->textCursor().selectedText()));
    emit runCommand( cmd );
  }
#endif // QT_VERSION >= 0x040000
}

void codeW::changeEvent(QEvent * e)
{
#if QT_VERSION >= 0x040000
  int newState = this->windowState();
  // Qt::WindowNoState
  // Qt::WindowMinimized
  // Qt::WindowMaximized

  QWidget::changeEvent(e);

  if ((this->oldState == Qt::WindowMinimized) && (newState == Qt::WindowNoState))
  {
    if (this->openAtMinimized)
    {
      emit resetFilePosition( this->oldTitle, this->oldFilename, this->oldLine, this->oldCol, this->oldLength );
    }
  }
  this->oldState = this->windowState();
#endif // QT_VERSION >= 0x040000
}

#ifdef _MSC_VER
QString codeW::ConvertToShortPathName(const QString path)
{
  //emit logWrite(path);
  wstring pathstr (Qt2TB::qstring2wstring(path));

  long length = 0;
  wchar_t *buffer = NULL;

  length = GetShortPathNameW(pathstr.c_str(), NULL, 0);
  if (length == 0) return path;

  length += 2;
  buffer = new wchar_t[length];
  length = GetShortPathNameW(pathstr.c_str(), buffer, length);
  wstring newpath (buffer);
  delete [] buffer;

  //emit logWrite(Qt2TB::wstring2qstring(newpath));
  return Qt2TB::wstring2qstring(newpath);  
}
#endif // _MSC_VER

void codeW::setTextFont(const QFont & font) {
  this->currentFont = font;
#if QT_VERSION >= 0x040000
  QMap <QString, MyTextEdit*>::iterator i;
  for (i = this->editMap.begin(); i != this->editMap.end(); ++i) {
    i.value()->setFont(font);
  }
#endif // QT_VERSION >= 0x040000
}
