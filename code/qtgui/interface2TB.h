/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/interface2TB.h,v $
*  * VERSION
*  *    $Revision: 1.30 $
*  * DATE
*  *    $Date: 2006/06/29 05:16:38 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __interface2tb_h__
#define __interface2tb_h__

enum MAP_ACTION {
  ACTION_VDM2UML,
  ACTION_UML2VDM, 
  ACTION_DEFAULT,
  ACTION_DELETE
};

enum CLASS_STATUS {
  STATUS_UNCHANGED,
  STATUS_MODIFIED, 
  STATUS_ADDED,
  STATUS_DELETED,
  STATUS_UNKNOWN
};

#define INT_ERROR 0
#define INT_OK 1

#if QTVER >= 4
#include <QtGui/QtGui>
#define QLIST QList
#else
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qthread.h>
#define QLIST QValueList
#endif // QTVER >= 4

#include "browserF.h"
#include <set>
#include <string>

class BackTraceEntry {
public:
  QString name;
  bool args;
  bool isDebug;

  BackTraceEntry(){}
};


class UMLClassState {

public: 
  QString name;
  CLASS_STATUS vdmStatus;
  CLASS_STATUS umlStatus;
  MAP_ACTION action;

  UMLClassState(QString n, CLASS_STATUS vs, CLASS_STATUS us, MAP_ACTION d): 
    name(n), vdmStatus(vs), umlStatus(us), action(d)
  { }

  UMLClassState() { }
};

class GUITokenInfo
{
enum Token_Type { TT_COMMENT     = 1,
                  TT_DEF         = 2,
                  TT_RESERVE     = 3,
                  TT_OPERATOR    = 4,
                  TT_PARENTHESIS = 5,
                  TT_LITERAL     = 6,
                  TT_CHARLITERAL = 7,
                  TT_TEXBREAK    = 8,
                  TT_IDENT       = 9,
                  TT_DEFAULT     = 10,
                  TT_POLY        = 11};

  public:
    GUITokenInfo() {}
    GUITokenInfo( int l, int c, int le, int tp )
      : line(l), col(c), len(le), type((Token_Type)tp) {}
    GUITokenInfo( const GUITokenInfo & ti )
      : line(ti.getLine()), col(ti.getCol()), len(ti.getLen()), type((Token_Type)(ti.getType())) {}
    void operator=( const GUITokenInfo & ti )
    {
      this->line = ti.getLine();
      this->col = ti.getCol();
      this->len = ti.getLen();
      this->type = (Token_Type)(ti.getType());
    }

    int getLine() const { return this->line; }
    int getCol() const { return this->col; }
    int getLen() const { return this->len; }
    int getType() const { return this->type; }
    bool isComment() { return this->type == TT_COMMENT; }
    bool isDefinition() { return this->type == TT_DEF; }
    bool isReserve() { return this->type == TT_RESERVE; }
    bool isOperator() { return this->type == TT_OPERATOR; }
    bool isParenthesis() { return this->type == TT_PARENTHESIS; }
    bool isLiteral() { return this->type == TT_LITERAL; }
    bool isCharLiteral() { return this->type == TT_CHARLITERAL; }
    bool isIdentifier() { return this->type == TT_IDENT; }
    bool isDefault() { return this->type == TT_DEFAULT; }
    bool isTexBreak() { return this->type == TT_TEXBREAK; }
    bool isPoly() { return this->type == TT_POLY; }
  private:
    int line;
    int col;
    int len;
    Token_Type type;
};
 
typedef QLIST<GUITokenInfo> GUITokenList;

class GUIOccurenceInfo
{
public:
  GUIOccurenceInfo() {};
  GUIOccurenceInfo(const QString & filename,
                   const QString & tempfilename,
                   int line,
                   int col,
                   int length,
                   bool def)
  : mFilename(filename), mTempFilename(tempfilename),
    mLine(line), mCol(col), mLength(length), mDef(def) {};
  GUIOccurenceInfo(const GUIOccurenceInfo & oi)
  : mFilename(oi.getFilename()), mTempFilename(oi.getTempFilename()),
    mLine(oi.getLine()), mCol(oi.getCol()), mLength(oi.getLength()),
    mDef(oi.isDef()) {};

  void operator=( const GUIOccurenceInfo & oi )
  {
    this->mFilename = oi.getFilename();
    this->mTempFilename = oi.getTempFilename();
    this->mLine = oi.getLine();
    this->mCol = oi.getCol();
    this->mLength = oi.getLength();
    this->mDef = oi.isDef();
  };

  const QString & getFilename() const { return this->mFilename; };
  const QString & getTempFilename() const { return this->mTempFilename; };
  int getLine() const { return this->mLine; };
  int getCol() const { return this->mCol; };
  int getLength() const { return this->mLength; };
  bool isDef() const { return this->mDef; };

private:
  QString mFilename;
  QString mTempFilename;
  int mLine;
  int mCol;
  int mLength;
  bool mDef;
};

typedef QLIST<GUIOccurenceInfo> GUIOccurenceList;

class PogInfo
{
public:
  PogInfo() {}
  PogInfo(const QString & cm, const QString & modnm, const QString & memnm,
          const QString & loctp, const QString & ptp, int no,
          const QString & fnm, const QString & tmpfnm,
          int l, int c, int len, const QString & desc) :
  checkMark(cm), modName(modnm), memberName(memnm), locationType(loctp),
  pogType(ptp), pogNum(no), filename(fnm), tmpFilename(tmpfnm), 
  line(l), col(c), length(len), pogDesc(desc) {}

  const QString & getCheckMark() { return this->checkMark; }
  const QString & getModName() { return this->modName; }
  const QString & getMemberName() { return this->memberName; }
  const QString & getLocationType() { return this->locationType; }
  const QString & getPogType() { return this->pogType; }
  int getPogNum() { return this->pogNum; }
  const QString & getFilename() { return this->filename; }
  const QString & getTmpFilename() { return this->tmpFilename; }
  int getLine() { return this->line; }
  int getCol() { return this->col; }
  int getLength() { return this->length; }
  const QString & getPogDesc() { return this->pogDesc; }

private:
  QString checkMark;
  QString modName;
  QString memberName;
  QString locationType;
  QString pogType;
  int pogNum;
  QString filename;
  QString tmpFilename;
  int line;
  int col;
  int length;
  QString pogDesc;
};

typedef QLIST<PogInfo> PogList;

class Qt2TB
{
public:
//  static bool canContinueI();

  static QString wstring2qstring(const std::wstring &);
  static std::wstring qstring2wstring(const QString &);

  static bool vdmPackageStructureChanged();
#ifdef VDMPP
  static bool javaPackageStructureChanged();
#endif // VDMPP
  static bool packageStructureChanged();

  static QString getPackageOfFileI(const QString &);

  static QStringList getVDMPackages();
  static QStringList getJavaPackages();
  static int getModulesOfVDMPackage(const QString &, QStringList &, QStringList &);
  static int getModulesOfJavaPackage(const QString &, QStringList &, QStringList &);
  static QStringList getFilesOfModule(const QString & mnm);
  static QStringList getFilesOfModules(bool vdm);
  static QStringList getModulesInFilesI(const QStringList &);
  static void GetModulePosInfoI(const QString & modnm, int * line, int * col, int * len);
  static void GetFnOpPosInfoI(const QString & modnm, int * line, int * col, int * len);

  static QString getSourceNameI(const QString &);

  static bool isJavaFile(const QString & name);
  static bool isJavaModule(const QString & name);
  static QString removeJavaPrefix(const QString & name);
  static QString addJavaPrefix(const QString & name);

  //returns a list with all the project files   
  static QStringList getProjectFilesI();

  //returns all modules of open project
  static QStringList getModulesI();


  //returns all modules of specified file
  static QStringList getModulesI(const QString & filename);

  //gets the status of a module
  static StatusType getStatusI(const QString & module);
  static StatusType::status convertStatusType(int st);
  static bool syntaxCheck(const QString & filename);

#ifdef VDMPP
  static int getOperationsI(const QString & clsnm, QStringList & ops);
#endif // VDMPP

  static int saveProjectI();
  static int saveProjectI(const QString & name);
  static QString getProjectNameI();

  static QStringList getPossibleInterfacesI();
  static QStringList getSelectedInterfacesI(const QStringList & posSet);
  static void setSelectedInterfacesI(const QStringList &);
  static QStringList getActivatedInterfacesI(const QStringList &);

  static void setExpression(const QString & package);
  static QString getExpression();

  static void setJavaCGPackage(const QString & package);
  static QString getJavaCGPackage();

  static void SetOptions(const QMap<QString, QString> &);
  static QMap<QString, QString> GetOptions();
  static void InitOptions();
  static void SetJCGHackParser(bool);
  static bool GetJCGHackParser();

  static bool CallLog();
  static void ToggleCallLog();

  // Interpreter interface
  static QString getTmpBreakFileNameI();
  static void ipStopI();
  static QString getTraceArgsI(int);

  static bool CPPGenerate(const QStringList & modules);
  static bool JavaGenerate(const QStringList & modules);

  static void LoadProject(const QString & project);
  static void SyntaxCheck(const QStringList & files);
  static void JavaSyntaxCheck(const QStringList & files);
  static void TypeCheck(const QStringList & modules);
  static void JavaTypeCheck(const QStringList & modules);
  static void PrettyPrint(const QStringList & files);
  static void JavaGenerateVDM(const QStringList & modules, bool stubOnly, bool autoRenaming, bool transforms);
  static bool ExecuteCommand(const QString & cmd);
  static void AddFiles(const QStringList & files);
  static void RemoveFiles(const QStringList & files);
  static void SaveProjectAs(const QString & project);
  static void NewUnnamedProject();
  static void PogGenerate(const QStringList & modules);

  static void api_Periodic();

#ifdef VDMPP
  static bool SetMapperStateI(bool);
  static void SetModelNamesI(const QString & pnm, const QString & fnm);
  static bool MapUMLI(QLIST<UMLClassState>&);
  static int GetClassStatusI(QLIST<UMLClassState>&);
  static void SetCharsetI(const QString & cs);
  static void SetToolInterfaceI(int n);
  static void SetNewFileTypeI(int n);
  static void SetRTFTemplateI(const QString & t);

  static void ResetMapper();
  static void UMLDiff(const QStringList & classes);
#endif // VDMPP

  static void setIOCharSetEnvI(const QString & cs);
  static bool isFileModifiedI(const QString &);
  static bool stateIsSavedI();
  static void SetSavedFileState(bool);
  static bool IsEverythingTypeCorrectI();

  static QString GiveToolTitleI();
  static QString GetVersionIdI();
  static QString GetCreditI();
  static QString GiveToolVersionDateI();
  static QString GetCompilerVersionI();

  static GUITokenList getGUITokenListI( const QString & filename );
  static GUIOccurenceList SearchIdI(const QStringList & files,
                                    const QString & id,
                                    bool partial, bool defOnly);

  static QString getLanguageI();

  static QStringList getChckMarks();
  static QStringList getModnms();
  static QStringList getMemnms();
  static QStringList getLocs();
  static QStringList getKinds();

  static void setChckMarks(const QStringList &);
  static void setModnms(const QStringList &);
  static void setMemnms(const QStringList &);
  static void setLocs(const QStringList &);
  static void setKinds(const QStringList &);

  static QString setCheckMark(const QString & modnm, const QString  & memnm,
                              const QString & kind, int no);

  static PogList applyFilters();

  static void InitModuleTypesI();
  static void InitToolboxI(bool);
  static void CleanUpI();
  static void ExitToolboxI(int);
  static bool InitCorbaApiI(int argc, char *argv[], std::wstring &err);
  static void TerminateCorbaApiI();

  static std::wstring GetFileNameI(int);
  static std::wstring GetSrcFileI(const std::wstring &);

  static void setBlock( bool mode );
  static bool isBlock();

private:
  static bool String2Bool(const QString & str);
  static int getModulesOfPackage(const QString & packageName, 
                                 bool isJava,
                                 QStringList & moduleStrings,
                                 QStringList & absStrings);
#ifdef VDMPP
//  static Tuple ConvertMapAction(MAP_ACTION a, CLASS_STATUS vdm, CLASS_STATUS uml);
  static CLASS_STATUS ConvertStatusStr(const std::wstring & st);
  static MAP_ACTION ConvertDefaultAction(bool l, bool r);
#endif // VDMPP

  //static Set existingJavaPackages;
  //static Set existingVDMPackages;
  //static Set oldJavaPackages;
  //static Set oldVDMPackages;
  static std::set<std::wstring> existingJavaPackages;
  static std::set<std::wstring> existingVDMPackages;
  static std::set<std::wstring> oldJavaPackages;
  static std::set<std::wstring> oldVDMPackages;

  static bool blocking_flag;
  static QMutex blocking_mutex;
};
  
#endif // __interface2tb_h__
