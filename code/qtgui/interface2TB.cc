/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/interface2TB.cc,v $
*  * VERSION
*  *    $Revision: 1.43 $
*  * DATE
*  *    $Date: 2006/06/29 05:16:37 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "interface2TB.h"
#include "metaiv.h"
#include "tools.h"
#include "astaux.h"
#include "settings.h"
#include "tbutils.h"
#include "tb_wstring.h"
#include "BaseTools.h"
#include "projectval.h"
#include "pog-interface.h"

#ifdef VDMPP
#include "MERGE.h"
#endif // VDMPP

#if QT_VERSION < 0x040000
#include <qthread.h>
#include <qtextcodec.h>
#include <qfileinfo.h>
#endif // QT_VERSION < 0x040000
 
bool Qt2TB::blocking_flag;
QMutex Qt2TB::blocking_mutex;

std::set<std::wstring> Qt2TB::existingJavaPackages;
std::set<std::wstring> Qt2TB::existingVDMPackages;
std::set<std::wstring> Qt2TB::oldJavaPackages;
std::set<std::wstring> Qt2TB::oldVDMPackages;

void Qt2TB::setBlock( bool mode )
{
  Qt2TB::blocking_mutex.lock();
  Qt2TB::blocking_flag = mode;
  Qt2TB::blocking_mutex.unlock();
};

bool Qt2TB::isBlock()
{
  Qt2TB::blocking_mutex.lock();
  bool res = Qt2TB::blocking_flag;
  Qt2TB::blocking_mutex.unlock();
  return res;
};

QString Qt2TB::wstring2qstring(const wstring & ws)
{
  size_t len_ws = ws.size();
  QString result;
  for (size_t idx = 0; idx < len_ws; idx++) {
    QChar c = (ushort) ws[idx];
    result.append(c);
  }
  return result;
}

std::wstring Qt2TB::qstring2wstring(const QString & qs)
{
  std::wstring result;
  for (int i = 0; i < (int)qs.length(); i++) {
    wchar_t c = (qs[i]).unicode();
    result.append(1, c);
  }
  return result;
}

QString Qt2TB::getPackageOfFileI(const QString& filename)
{
  TYPE_ProjectTypes_FileName fn (ToolMediator::GetPackageOfFile(
                                    PTAUX::mk_FileName(qstring2wstring(filename)), 
                                    Char('/')));
  return QString(wstring2qstring(PTAUX::ExtractFileName(fn)));
}

#ifdef VDMPP
bool Qt2TB::javaPackageStructureChanged()
{
  return (Qt2TB::oldJavaPackages != Qt2TB::existingJavaPackages);
}
#endif // VDMPP

bool Qt2TB::vdmPackageStructureChanged()
{
  return (Qt2TB::oldVDMPackages != Qt2TB::existingVDMPackages);
}

bool Qt2TB::packageStructureChanged()
{
#ifdef VDMSL
  return vdmPackageStructureChanged();
#endif // VDMSL
#ifdef VDMPP
  return (vdmPackageStructureChanged() || javaPackageStructureChanged());
#endif // VDMPP
}

QStringList Qt2TB::getVDMPackages()
{
  // map ProjectTypes`FileName to set of (ProjectTypes`FileName * ProjectTypes`FileName)
  Map packages (ToolMediator::GetFilesAndPackages(Char('/')));

  SET<TYPE_ProjectTypes_FileName> dom_packages (packages.Dom());
  Generic fnm;
  SET<TYPE_ProjectTypes_FileName> vdmPackages;
  for (bool bb = dom_packages.First(fnm); bb; bb = dom_packages.Next(fnm)) {
    Set files (packages[fnm]); // set of (ProjectTypes`FileName * ProjectTypes`FileName)
    Generic gg;
    for (bool cc = files.First(gg); cc; cc = files.Next(gg)) {
      Tuple pair (gg); // (ProjectTypes`FileName * ProjectTypes`FileName)
      TYPE_ProjectTypes_FileName filename (pair.GetRecord(2));
      QString fnQstr (wstring2qstring(PTAUX::ExtractFileName(filename)));
      if (!isJavaFile(fnQstr)) {
	vdmPackages.Insert(fnm);
        break;
      }
    }
  }

  Qt2TB::oldVDMPackages = Qt2TB::existingVDMPackages;
  Qt2TB::existingVDMPackages.clear();

  QStringList qsl;
  Generic fn;
  for (bool bb = vdmPackages.First(fn); bb; bb = vdmPackages.Next(fn)) {
    wstring filename (PTAUX::ExtractFileName(fn));
    qsl.append(wstring2qstring(filename));
    Qt2TB::existingVDMPackages.insert(filename);
  }
  return qsl;
}

QStringList Qt2TB::getJavaPackages()
{
  // map ProjectTypes`FileName to set of (ProjectTypes`FileName * ProjectTypes`FileName)
  Map packages (ToolMediator::GetFilesAndPackages(Char('/')));
  SET<TYPE_ProjectTypes_FileName> dom_packages (packages.Dom());
  
  SET<TYPE_ProjectTypes_FileName> javaPackages;
  Generic fnm;
  for (bool bb = dom_packages.First(fnm); bb; bb = dom_packages.Next(fnm)) {
    Set files (packages[fnm]); // set of (ProjectTypes`FileName * ProjectTypes`FileName)
    Generic gg;
    for (bool cc = files.First(gg); cc; cc = files.Next(gg)) {
      Tuple pair (gg); // (ProjectTypes`FileName * ProjectTypes`FileName)
      TYPE_ProjectTypes_FileName filename (pair.GetRecord(2));
      QString fnQstr (wstring2qstring(PTAUX::ExtractFileName(filename)));
      if (isJavaFile(fnQstr)) {
	javaPackages.Insert(fnm);
        break;
      }
    }
  }
  Qt2TB::oldJavaPackages = Qt2TB::existingJavaPackages;
  Qt2TB::existingJavaPackages.clear();

  QStringList qsl;
  Generic fn;
  for (bool bb = javaPackages.First(fn); bb; bb = javaPackages.Next(fn)) {
    wstring filename (PTAUX::ExtractFileName(fn));
    qsl.append(wstring2qstring(filename));
    Qt2TB::existingJavaPackages.insert(filename);
  }
  return qsl;
}

QStringList Qt2TB::getModulesInFilesI(const QStringList & fileList)
{
  QStringList result;
  for (QStringList::const_iterator iter = fileList.begin(); iter != fileList.end(); ++iter) {
    QStringList theseModules (getModulesI(*iter));
    result += theseModules;
  }
  return result;
}

QString Qt2TB::getSourceNameI(const QString & apparentName)
{
  wstring file (qstring2wstring(apparentName));
  TYPE_ProjectTypes_FileName tfn (ToolMediator::GetVDMTempFileName (PTAUX::mk_FileName (file)));
  wstring tmpfn (PTAUX::ExtractFileName (tfn));
  if (tmpfn.length() == 0) {
    tmpfn = file;
  }
  return wstring2qstring(tmpfn);
}

bool Qt2TB::isJavaFile(const QString & filename)
{
  return (filename.endsWith(".java"));
}

bool Qt2TB::isJavaModule(const QString & modulename)
{
  return (modulename.startsWith("java|"));
}

QString Qt2TB::removeJavaPrefix(const QString & name)
{
  return (isJavaModule(name) ? name.mid(5) : name);
}

QString Qt2TB::addJavaPrefix(const QString & name)
{
  return (isJavaModule(name) ? name : (QString("java|") + name));
}

int Qt2TB::getModulesOfPackage(const QString & packageName,
                               bool isJava,
                               QStringList & moduleStrings, 
                               QStringList & absStrings)
{
  TYPE_ProjectTypes_FileName packageFileName (PTAUX::mk_FileName(qstring2wstring(packageName)));
  // map ProjectTypes`FileName to set of (ProjectTypes`FileName * ProjectTypes`FileName)
  Map packages (ToolMediator::GetFilesAndPackages(Char('/')));
  if (packages.DomExists(packageFileName)) {
    Set pairs (packages[packageFileName]); // set of (ProjectTypes`FileName * ProjectTypes`FileName)
    Generic g;
    for (bool bb = pairs.First(g); bb; bb = pairs.Next(g)) {
      Tuple pair (g); // (ProjectTypes`FileName * ProjectTypes`FileName)
      QString fileNm (wstring2qstring(PTAUX::ExtractFileName(pair.GetRecord(1))));
      if ((isJava && isJavaFile(fileNm)) || (!isJava && !isJavaFile(fileNm))) {
        moduleStrings.append(fileNm);
        QString absFileNm (wstring2qstring(PTAUX::ExtractFileName(pair.GetRecord(2))));
        absStrings.append(absFileNm);
      }
    }
  }
  return INT_OK;
}

int Qt2TB::getModulesOfVDMPackage(const QString & packageName, QStringList & moduleStrings, QStringList & absStrings)
{
  return getModulesOfPackage(packageName, false, moduleStrings, absStrings);
}

int Qt2TB::getModulesOfJavaPackage(const QString & packageName, QStringList & moduleStrings, QStringList & absStrings)
{
  return getModulesOfPackage(packageName, true, moduleStrings, absStrings);
}

QStringList Qt2TB::getProjectFilesI()
//returns a list with all the project files
{
  SET<TYPE_ProjectTypes_FileName> files (ToolMediator::Files());
  QStringList qsl;
  Generic fn;
  for (bool bb = files.First(fn); bb; bb = files.Next(fn)) {
    qsl.append(wstring2qstring(PTAUX::ExtractFileName(fn)));
  }
  return qsl;
} 

QStringList Qt2TB::getFilesOfModule(const QString & mnm)
{
  TYPE_ProjectTypes_ModuleName mname (PTAUX::mk_ModuleName(qstring2wstring(mnm)));
  SET<TYPE_ProjectTypes_FileName> files (ToolMediator::FileOfModule(mname));
  QStringList qsl;
  Generic fn;
  for (bool bb = files.First(fn); bb; bb = files.Next(fn)) {
    qsl.append(wstring2qstring(PTAUX::ExtractFileName(fn)));
  }
  return qsl;
}

QStringList Qt2TB::getFilesOfModules(bool vdm)
{
  SET<TYPE_ProjectTypes_ModuleName> modules (ToolMediator::AllModules());
  QStringList qsl;
  Generic mn;
  for (bool bb = modules.First(mn); bb; bb = modules.Next(mn)) {
    SET<TYPE_ProjectTypes_FileName> files (ToolMediator::FileOfModule(mn));
    Generic fn;
    for (bool cc = files.First(fn); cc;  cc = files.Next(fn)) {
      QString filenm (wstring2qstring(PTAUX::ExtractFileName(fn)));
      if (!qsl.contains(filenm) &&
          ((vdm && !isJavaFile(filenm)) || (!vdm && isJavaFile(filenm)))) {
        qsl.append(filenm);
      }
    }
  }
  return qsl;
}

//returns all modules of open project
QStringList Qt2TB::getModulesI()
{
  SET<TYPE_ProjectTypes_ModuleName> modules (ToolMediator::AllModules());
  QStringList qsl;
  Generic mn;
  for (bool bb = modules.First(mn); bb; bb = modules.Next(mn)) {
    qsl.append(wstring2qstring(PTAUX::ExtractModuleName(mn)));
  }
  return qsl;
}

#ifdef VDMSL
QString Qt2TB::GetDefaultModName()
{
  return wstring2qstring(ASTAUX::GetDefaultModName());
}
#endif // VDMSL

void Qt2TB::GetModulePosInfoI(const QString & modnm, int * line, int * col, int * len)
{
  Tuple t (ToolMediator::GetModulePosInfo(PTAUX::mk_ModuleName(qstring2wstring(modnm))));
  if (t.GetBoolValue(1)) {
    *line = t.GetIntValue(2);
    *col = t.GetIntValue(3);
    *len = t.GetIntValue(4);
  }
}

void Qt2TB::GetFnOpPosInfoI(const QString & modnm, int * line, int * col, int * len)
{
  Tuple t (ToolMediator::GetFnOpPosInfo(qstring2wstring(modnm)));
  if (t.GetBoolValue(1)) {
    *line = t.GetIntValue(2);
    *col = t.GetIntValue(3);
    *len = t.GetIntValue(4);
  }
}
 
//returns all modules of specified file
QStringList Qt2TB::getModulesI(const QString & filename)
{
  TYPE_ProjectTypes_FileName fname (PTAUX::mk_FileName(qstring2wstring(filename)));
  SET<TYPE_ProjectTypes_ModuleName> mnm_s (ToolMediator::ModulesInFile(fname));
  QStringList modules;
  Generic mnm;
  for (bool bb = mnm_s.First(mnm); bb; bb = mnm_s.Next(mnm)) {
    modules.append(wstring2qstring(PTAUX::ExtractModuleName(mnm)));
  }
  return modules;
}

#ifdef VDMPP
int Qt2TB::getOperationsI(const QString & clsnm, QStringList & ops)
{
  ops.clear();
  wstring classnameI (qstring2wstring(clsnm));
  Tuple t (TOOLS::GetOperations(classnameI)); // bool * seq of seq of char
  if (t.GetBoolValue(1)) {
    Sequence nm_l (t.GetSequence(2));
    size_t len_nm_l = nm_l.Length();
    for (size_t idx = 1; idx <= len_nm_l; idx++) {
      Sequence s (nm_l[idx]);
      ops.append(wstring2qstring(s.GetString()));
    }
    return INT_OK;
  }
  return INT_ERROR;
}
#endif // VDMPP

//gets the status of a module
StatusType Qt2TB::getStatusI(const QString & module)
{
  TYPE_ProjectTypes_ModuleName mname (PTAUX::mk_ModuleName(qstring2wstring(module)));
  status_type st (PTAUX::ExtractStatus(ToolMediator::Status(mname)));
  StatusType res (convertStatusType(st.syntax),
                  convertStatusType(st.type),
                  convertStatusType(st.cg),
                  convertStatusType(st.jcg),
                  convertStatusType(st.pp));
  return res;
}

StatusType::status Qt2TB::convertStatusType(int st)
{
  switch(st) {
    case status_type::status_none: { return StatusType::status_none; }
    case status_type::status_error: { return StatusType::status_error; }
    case status_type::status_ok: { return StatusType::status_ok; }
    case status_type::status_pos: { return StatusType::status_pos; }
    case status_type::status_def: { return StatusType::status_def; }
    default: { return StatusType::status_none; }
  }
}

bool Qt2TB::syntaxCheck(const QString & filename)
{
  return ToolMediator::SyntaxCheckOk(PTAUX::mk_FileName(qstring2wstring(filename)));
}

bool Qt2TB::CPPGenerate(const QStringList & modules)
{
  SEQ<TYPE_ProjectTypes_ModuleName> module_s;
  for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it) {
    if (!(*it).isEmpty()) {
      module_s.ImpAppend(PTAUX::mk_ModuleName(Qt2TB::qstring2wstring(*it)));
    }
  }

  return ToolMediator::BTools()->vdm_CodeGenerate(module_s,
                                                  Quote(L"CPP"),
                                                  Bool(false),
                                                  Bool(true),
                                                  Bool(false),
                                                  Bool(false),
                                                  Nil(),
                                                  Bool(false),
                                                  Bool(Settings.GetCGCheckPrePost()));
}

bool Qt2TB::JavaGenerate(const QStringList & modules)
{
  SEQ<TYPE_ProjectTypes_ModuleName> module_s;
  for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it) {
    if (!(*it).isEmpty()) {
      module_s.ImpAppend(PTAUX::mk_ModuleName(Qt2TB::qstring2wstring(*it)));
    }
  }

  return ToolMediator::BTools()->vdm_CodeGenerate(module_s,
                                                  Quote(L"JAVA"),
                                                  Bool(Settings.GetJCGSkel()),
                                                  Bool(Settings.GetJCGGenPrePost()),
                                                  Bool(Settings.GetJCGTypes()),
                                                  Bool(Settings.GetJCGSmallTypes()),
                                                  Settings.GetJCGPackage(),
                                                  Bool(Settings.GetJCGConcur()),
                                                  Bool(Settings.GetJCGCheckPrePost()));
}

void Qt2TB::LoadProject(const QString & project)
{
  ToolMediator::BTools()->vdm_LoadProject(PTAUX::mk_FileName(qstring2wstring(project)));
}

void Qt2TB::SyntaxCheck(const QStringList & files)
{
  if(ToolMediator::CheckModified()) {
    ToolMediator::ResetTCStatus();
  }
  if (!files.isEmpty()) {
    SEQ<TYPE_ProjectTypes_FileName> fileList;
    for (QStringList::const_iterator it = files.begin(); it != files.end(); ++it) {
      fileList.ImpAppend(PTAUX::mk_FileName(Qt2TB::qstring2wstring(*it)));
    }
    ToolMediator::BTools()->vdm_SyntaxCheck(fileList);
  }
}

void Qt2TB::JavaSyntaxCheck(const QStringList & files)
{
#ifdef VDMPP
  if(ToolMediator::CheckModified())
    ToolMediator::ResetTCStatus();

  if (!files.isEmpty()) {
    SEQ<TYPE_ProjectTypes_FileName> fileList;
    for (QStringList::const_iterator it = files.begin(); it != files.end(); ++it) {
      fileList.ImpAppend(PTAUX::mk_FileName(Qt2TB::qstring2wstring(*it)));
    }
    ToolMediator::BTools()->vdm_JavaSyntaxCheck(fileList);
  }
#endif // VDMPP
}

void Qt2TB::TypeCheck(const QStringList & modules)
{
  if (!modules.isEmpty()) {
    SEQ<TYPE_ProjectTypes_ModuleName> module_l;
    for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it) {
      if (!(*it).isEmpty()) {
        module_l.ImpAppend(PTAUX::mk_ModuleName(Qt2TB::qstring2wstring(*it)));
      }
    }
    if (!module_l.IsEmpty()) {
      ToolMediator::BTools()->vdm_TypeCheck(module_l);
    }
  }
}

void Qt2TB::JavaTypeCheck(const QStringList & modules)
{
  if (!modules.isEmpty()) {
    SEQ<TYPE_ProjectTypes_ModuleName> module_l;
    for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it) {
      if (!(*it).isEmpty()) {
        module_l.ImpAppend(PTAUX::mk_ModuleName(Qt2TB::qstring2wstring(*it)));
      }
    }
    if (!module_l.IsEmpty())
      ToolMediator::BTools()->vdm_JavaTypeCheck(module_l);
  }
}

void Qt2TB::PrettyPrint(const QStringList & files)
{
  if (!files.isEmpty()) {
    SEQ<TYPE_ProjectTypes_FileName> file_l;
    for (QStringList::const_iterator it = files.begin(); it != files.end(); ++it) {
      file_l.ImpAppend(PTAUX::mk_FileName(Qt2TB::qstring2wstring(*it)));
    }
    ToolMediator::BTools()->vdm_PrettyPrint(file_l);
  }
}

void Qt2TB::JavaGenerateVDM(const QStringList & modules, bool stubOnly, bool autoRenaming, bool transforms)
{
  SEQ<TYPE_ProjectTypes_ModuleName> module_l;
  for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it) {
    if (!(*it).isEmpty()) {
      module_l.ImpAppend(PTAUX::mk_ModuleName(Qt2TB::qstring2wstring(*it)));
    }
  }
  ToolMediator::BTools()->vdm_JavaGenerateVDM(module_l, Bool(stubOnly), Bool(autoRenaming), Bool(transforms));
}

bool Qt2TB::ExecuteCommand(const QString & cmd)
{
  Bool cont (ToolMediator::BTools ()->vdm_ExecuteCommand (PTAUX::mk_ToolCommand(qstring2wstring(cmd))));
  return cont.GetValue();
}

void Qt2TB::AddFiles(const QStringList & files)
{
  SET<TYPE_ProjectTypes_FileName> file_s;
  for (QStringList::const_iterator it = files.begin();it != files.end(); ++it) {
    file_s.Insert(PTAUX::mk_FileName(Qt2TB::qstring2wstring(*it)));
  }
  ToolMediator::BTools()->vdm_AddFiles(file_s);
}

void Qt2TB::RemoveFiles(const QStringList & files)
{
  SET<TYPE_ProjectTypes_FileName> file_s;
  for (QStringList::const_iterator it = files.begin();it != files.end(); ++it) {
    file_s.Insert(PTAUX::mk_FileName(Qt2TB::qstring2wstring(*it)));
  }
  ToolMediator::BTools()->vdm_RemoveFiles(file_s);
}

void Qt2TB::SaveProjectAs(const QString & project)
{
  ToolMediator::BTools()->vdm_SaveProjectAs(PTAUX::mk_FileName(qstring2wstring(project)));
}

void Qt2TB::NewUnnamedProject()
{
  ToolMediator::BTools()->vdm_NewUnnamedProject();
}

void Qt2TB::PogGenerate(const QStringList & modules)
{
  SEQ<TYPE_ProjectTypes_ModuleName> module_l;
  for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it) {
    if (!(*it).isEmpty()) {
      module_l.ImpAppend(PTAUX::mk_ModuleName(Qt2TB::qstring2wstring(*it)));
    }
  }
  ToolMediator::BTools()->vdm_PogGenerate(module_l);
}

int Qt2TB::saveProjectI(const QString & pname)
{
  wstring prjname (qstring2wstring(pname));
  Bool result (ToolMediator::BTools()->vdm_SaveProject(PTAUX::mk_FileName(prjname)));
  return (result.GetValue() ? INT_OK : INT_ERROR);
}

int Qt2TB::saveProjectI()
{
  return saveProjectI(getProjectNameI());
}
 
QString Qt2TB::getProjectNameI()
{
  Generic pnm (ToolMediator::BTools()->vdm_GetProjectName());
  wstring nm (pnm.IsNil() ? L"" : PTAUX::ExtractFileName(pnm));
  return wstring2qstring(nm);
}

//
// Functions for Rose mapper
//

#ifdef VDMPP
bool Qt2TB::SetMapperStateI(bool i)
{
  return (ToolMediator::UMLT()->vdm_SetMapperState(Bool(i))).GetValue();
}

void Qt2TB::SetModelNamesI(const QString & pnm, const QString & fnm)
{
  ToolMediator::UMLT()->vdm_SetModelNames(Sequence(qstring2wstring(pnm)), Sequence(qstring2wstring(fnm)));
}

bool Qt2TB::MapUMLI(QLIST<UMLClassState> & settings)
{
  Map guiSettings;
  for (int i = 0; i < (int)settings.count(); i++) {
    SEQ<Char> className (qstring2wstring(settings[i].name));
    switch(settings[i].action) {
      case ACTION_DEFAULT: {
        guiSettings.Insert(className, mk_(Bool(true), Bool(true)));
        break;
      }
      case ACTION_VDM2UML: {
        if(settings[i].vdmStatus == STATUS_DELETED) {
          guiSettings.Insert(className, mk_(Bool(false), Bool(false)));
        }
        else {
          guiSettings.Insert(className, mk_(Bool(true), Bool(false)));
        }
        break;
      }
      case ACTION_UML2VDM: {
        if(settings[i].umlStatus == STATUS_DELETED) {
          guiSettings.Insert(className, mk_(Bool(false), Bool(false)));
        }
        else {
          guiSettings.Insert(className, mk_(Bool(false), Bool(true)));
        }
        break;
      }
      case ACTION_DELETE: {
        guiSettings.Insert(className, mk_(Bool(false), Bool(false)));
        break;
      }
      default: {
        guiSettings.Insert(className, mk_(Bool(false), Bool(false)));
        break;
      }
    }
  }
  return (ToolMediator::UMLT()->vdm_MapUML(guiSettings)).IsEmpty();
}

CLASS_STATUS Qt2TB::ConvertStatusStr(int sttag)
{
  switch (sttag) {
    case TAG_TYPE_MERGE_None:     { return STATUS_UNCHANGED; }
    case TAG_TYPE_MERGE_Added:    { return STATUS_ADDED; }
    case TAG_TYPE_MERGE_Modified: { return STATUS_MODIFIED; }
    case TAG_TYPE_MERGE_Deleted:  { return STATUS_DELETED; }
    case TAG_TYPE_MERGE_Unknown:
    default:                      { return STATUS_UNKNOWN; }
  }
}

MAP_ACTION Qt2TB::ConvertDefaultAction(bool l, bool r)
{
  if (l && r) {
    return ACTION_DEFAULT;
  }
  else if (l) {
    return ACTION_VDM2UML;
  }
  else if (r) {
    return ACTION_UML2VDM;
  }
  else {
    return ACTION_DELETE;
  }
}

int Qt2TB::GetClassStatusI(QLIST<UMLClassState> & classes)
{
  type_cLdd2PM stat (ToolMediator::UMLT()->vdm_GetClassStatus());
  type_cLbb2PM defaults (ToolMediator::UMLT()->vdm_GetDefaultSettings(stat));

  Generic fn;
  for (bool bb = stat.First(fn); bb; bb = stat.Next(fn)) {
    // Class name
    UMLClassState thisState;
    thisState.name = wstring2qstring(PTAUX::Seq2Str(fn));

    Tuple t (stat[fn]);     // (BUILD`State * BUILD`State)
    //thisState.vdmStatus = ConvertStatusStr(PTAUX::Seq2Str(t.GetRecord(1).GetSequence(1)));
    thisState.vdmStatus = ConvertStatusStr(t.GetRecord(1).GetTag());
    //thisState.umlStatus = ConvertStatusStr(PTAUX::Seq2Str(t.GetRecord(2).GetSequence(1)));
    thisState.umlStatus = ConvertStatusStr(t.GetRecord(2).GetTag());

    Tuple d (defaults[fn]);  // (bool * bool)
    Bool vp (d.GetBool(1)), um (d.GetBool(2));
    thisState.action = ConvertDefaultAction(vp.GetValue(), um.GetValue());
    classes.append(thisState);
  } 
  return INT_OK;
}

void Qt2TB::SetCharsetI(const QString & cs)
{
  ToolMediator::UMLT()->vdm_SetCharSet(SEQ<Char>(qstring2wstring(cs)));
}

void Qt2TB::SetToolInterfaceI(int n)
{
  ToolMediator::UMLT()->vdm_SetToolInterface(Int(n));
}

void Qt2TB::SetNewFileTypeI(int n)
{
  ToolMediator::UMLT()->vdm_SetNewFileType(Int(n));
}

void Qt2TB::SetRTFTemplateI(const QString & t)
{
  ToolMediator::UMLT()->vdm_SetRTFTemplate(Sequence(qstring2wstring(t)));
}

void Qt2TB::ResetMapper()
{
  ToolMediator::UMLT()->vdm_ResetMapper();
}

void Qt2TB::UMLDiff(const QStringList & classes)
{
  type_cLS classesSet;
  for (QStringList::const_iterator it = classes.begin(); it != classes.end(); ++it) {
    classesSet.Insert(SEQ<Char>(Qt2TB::qstring2wstring(*it)));
  }
  ToolMediator::UMLT()->vdm_Diff(classesSet);
}
#endif // VDMPP

void Qt2TB::api_Periodic()
{
  TOOLS::api_Periodic();
}

void Qt2TB::setIOCharSetEnvI(const QString & cs)
{
  TBWSTR::setIOCharSetEnv( qstring2wstring(cs) );
}

//
// Java interfaces
//

QStringList Qt2TB::getPossibleInterfacesI()
{
  SET<TYPE_ProjectTypes_ModuleName> possibleInterfaces (ToolMediator::BTools()->vdm_GetPossibleInterfaces());
  QStringList qsl;
  Generic mn;
  for (bool bb = possibleInterfaces.First(mn); bb; bb = possibleInterfaces.Next(mn)) {
    qsl.append(wstring2qstring(PTAUX::ExtractModuleName(mn)));
  }
  return qsl;
}

QStringList Qt2TB::getSelectedInterfacesI(const QStringList & posSet)
{
  SET<TYPE_ProjectTypes_ModuleName> actualInterfaces (Settings.GetJCGInterfaces());
  SET<TYPE_ProjectTypes_ModuleName> obs;
  QStringList qsl;
  Generic mn;
  for (bool bb = actualInterfaces.First(mn); bb; bb = actualInterfaces.Next(mn)) {
    QString mnm (wstring2qstring(PTAUX::ExtractModuleName(mn)));
    if (posSet.contains(mnm)) {
      qsl.append(mnm);
    }
    else {
      obs.Insert(mn);
    }
  }
  if (!obs.IsEmpty()) {
    Settings.SetJCGInterfaces(actualInterfaces.Diff(obs));
  }
  return qsl;
}

void Qt2TB::setSelectedInterfacesI(const QStringList & interfaces)
{
  SET<TYPE_ProjectTypes_ModuleName> newSelectedSet;
  for (QStringList::const_iterator it = interfaces.begin(); it != interfaces.end(); ++it) {
    newSelectedSet.Insert(PTAUX::mk_ModuleName(qstring2wstring(*it)));
  }
  Settings.SetJCGInterfaces(newSelectedSet);
}

QStringList Qt2TB::getActivatedInterfacesI(const QStringList & newSelectedInterfaces)
{
  SET<TYPE_ProjectTypes_ModuleName> newSelectedSet;
  for (QStringList::const_iterator it = newSelectedInterfaces.begin(); it != newSelectedInterfaces.end(); ++it) {
    newSelectedSet.Insert(PTAUX::mk_ModuleName(qstring2wstring(*it)));
  }
  SET<TYPE_ProjectTypes_ModuleName> origSetting_JCG_INTERFACES (Settings.GetJCGInterfaces());
  Settings.SetJCGInterfaces(newSelectedSet);
  QStringList newPossibleInterfaces (getPossibleInterfacesI());
  Settings.SetJCGInterfaces(origSetting_JCG_INTERFACES);
  return newPossibleInterfaces;
}

void Qt2TB::setExpression(const QString & expr)
{
  if(expr.isEmpty()) {
    Settings.SetExpression(Nil());
  }
  else {
    Settings.SetExpression(Sequence(Qt2TB::qstring2wstring(expr)));
  }
}

QString Qt2TB::getExpression()
{
  QString expr;
  if (!Settings.GetExpression().IsNil()) {
    Sequence exprSeq (Settings.GetExpression());
    expr = Qt2TB::wstring2qstring(exprSeq.GetString());
  }
  return expr;
}

void Qt2TB::setJavaCGPackage(const QString & package)
{
  if(package.isEmpty()) {
    Settings.SetJCGPackage(Nil());
  }
  else {
    Settings.SetJCGPackage(Sequence(Qt2TB::qstring2wstring(package)));
  }
}

QString Qt2TB::getJavaCGPackage()
{
  QString package;
  if (!Settings.GetJCGPackage().IsNil()) {
    Sequence packageSeq (Settings.GetJCGPackage());
    package = Qt2TB::wstring2qstring(packageSeq.GetString());
  }
  return package;
}

bool Qt2TB::CallLog()
{
  return Settings.CallLog();
}

void Qt2TB::ToggleCallLog()
{
  bool calllog = Settings.CallLog();
  if (calllog) {
    Settings.CallLogOff();
    TOOLS::CloseCallLog();
  }
  else {
    Settings.CallLogOn();
    TOOLS::OpenCallLog();
  }
}

void Qt2TB::SetOptions(const QMap<QString, QString> & optionMap)
{
//
//
  if( optionMap.contains( "DTC" ) ) {
    if(String2Bool(optionMap[ "DTC" ])) {
      Settings.DtcOn();
    }
    else {
      Settings.DtcOff();
    }
  }
  if( optionMap.contains( "PRE" ) ) {
    if(String2Bool(optionMap[ "PRE" ])) {
      Settings.PreOn();
    }
    else {
      Settings.PreOff();
    }
  }
  if( optionMap.contains( "POST" ) ) {
    if(String2Bool(optionMap[ "POST" ])) {
      Settings.PostOn();
    }
    else {
      Settings.PostOff();
    }
  }
  if( optionMap.contains( "MEASURE" ) ) {
    if(String2Bool(optionMap[ "MEASURE" ])) {
      Settings.MeasureOn();
    }
    else {
      Settings.MeasureOff();
    }
  }
  if( optionMap.contains( "INV" ) ) {
    if(String2Bool(optionMap[ "INV" ])) {
      Settings.InvOn();
      if (!Settings.DTC()) {
        Settings.DtcOn();
      }
    }
    else {
      Settings.InvOff();
    }
  }
  if( optionMap.contains( "SEP" ) ) {
    if(String2Bool(optionMap[ "SEP" ])) {
      Settings.SepOn();
    }
    else {
      Settings.SepOff();
    }
  }
  if( optionMap.contains( "CONTEXT" ) ) {
    if(String2Bool(optionMap[ "CONTEXT" ])) {
      Settings.ContextOn();
    }
    else {
      Settings.ContextOff();
    }
  }
#ifdef VDMPP
  if( optionMap.contains( "MAXINSTR" ) ) {
    Settings.SetMaxInstr(optionMap[ "MAXINSTR" ].toInt());
  }
  if( optionMap.contains( "PRIORITY" ) ) {
    if(String2Bool(optionMap[ "PRIORITY" ])) {
      Settings.PriorityBasedOn();
    }
    else {
      Settings.PriorityBasedOff();
    }
  }
  if( optionMap.contains( "PRIMARYALGORITHM" ) ) {
    QString algorithm (optionMap[ "PRIMARYALGORITHM" ]);
    if( algorithm == "pure_cooperative" ) {
      Settings.SetPureCooperative();
    }
    else if( algorithm == "instruction_number_slice" ) {
      Settings.SetInstrnumSlice();
    }
#ifdef VISE
    else if( algorithm == "timeslice" ) {
      Settings.SetTimeSlice();
    }
#endif // VICE
    else {
      Settings.SetPureCooperative();
    }
  }
  if( optionMap.contains( "TASKSWITCH" ) ) {
    Settings.SetTaskSwitch(optionMap[ "TASKSWITCH" ].toInt());
  }
  if( optionMap.contains( "MAXTIME" ) ) {
    Settings.SetTimeSlice(optionMap[ "MAXTIME" ].toInt());
  }
  if( optionMap.contains( "TIMEFACTOR" ) ) {
    Settings.SetTimeFactor(optionMap[ "TIMEFACTOR" ].toInt());
  }
  if( optionMap.contains( "STEPSIZE" ) ) {
    Settings.SetStepSize(optionMap[ "STEPSIZE" ].toInt());
  }
  if( optionMap.contains( "JITTERMODE" ) ) {
    Settings.SetJitterModeStr(Qt2TB::qstring2wstring(optionMap[ "JITTERMODE" ]));
  }
  if( optionMap.contains( "DEFAULTCPUCAPACITY" ) ) {
    Settings.SetDefaultCapacity(optionMap[ "DEFAULTCPUCAPACITY" ].toInt());
  }
  if( optionMap.contains( "DEFAULTVCPUCAPACITY" ) ) {
    Settings.SetVirtualCPUCapacityStr(Qt2TB::qstring2wstring(optionMap[ "DEFAULTVCPUCAPACITY" ]));
  }
  if( optionMap.contains( "LOGARGS" ) ) {
    Settings.SetLogArgsStr(Qt2TB::qstring2wstring(optionMap[ "LOGARGS" ]));
  }
#endif // VDMPP
  if( optionMap.contains( "VDMSLMODE" ) ) {
    if(String2Bool(optionMap[ "VDMSLMODE" ])) {
      Settings.VDMSLmodeOn();
    }
    else {
      Settings.VDMSLmodeOff();
    }
  }
  if( optionMap.contains( "VDM10" ) ) {
    if(String2Bool(optionMap[ "VDM10" ])) {
      if (!Settings.VDM10()) {
        Settings.VDM10On();
        SEQ<TYPE_ProjectTypes_FileName> file_l (ToolMediator::Repos()->vdm_Files().ToSequence());
        if (!file_l.IsEmpty()) {
          ToolMediator::BTools ()->vdm_SyntaxCheck (file_l);
        }
      }
    }
    else {
      if (Settings.VDM10()) {
        Settings.VDM10Off();
        SEQ<TYPE_ProjectTypes_FileName> file_l (ToolMediator::Repos()->vdm_Files().ToSequence());
        if (!file_l.IsEmpty()) {
          ToolMediator::BTools ()->vdm_SyntaxCheck (file_l);
        }
      }
    }
  }
  if( optionMap.contains( "OLD_REVERSE" ) ) {
    ToolMediator::SetOldReverseEnable(String2Bool(optionMap[ "OLD_REVERSE" ]));
  }
  if( optionMap.contains( "errlevel" ) ) {
    if(String2Bool(optionMap[ "errlevel" ])) {
      Settings.ErrorLevelPRF();
    }
    else {
      Settings.ErrorLevelWRN1();
    }
  }
  if( optionMap.contains( "PRINT_FORMAT" ) ) {
    if(String2Bool(optionMap[ "PRINT_FORMAT" ])) {
      Settings.PrintFormatOn();
    }
    else {
      Settings.PrintFormatOff();
    }
  }
  if( optionMap.contains( "RTERR_EXCEPTION" )) {
    if(String2Bool(optionMap[ "RTERR_EXCEPTION" ])) {
      Settings.RTErrExceptionOn();
    }
    else {
      Settings.RTErrExceptionOff();
    }
  }
  if( optionMap.contains( "EXPRESSION" ) ) {
    QString expr = optionMap[ "EXPRESSION" ];
    Qt2TB::setExpression(expr);
  }
  if( optionMap.contains( "CG_RTI" ) ) {
    Settings.SetCGRTI(optionMap[ "CG_RTI" ] == "1");
  }
  if( optionMap.contains( "CG_CHECKPREPOST" ) ) {
    Settings.SetCGCheckPrePost(optionMap[ "CG_CHECKPREPOST" ] == "1");
  }
  if( optionMap.contains( "DEF" ) ) {
    if( optionMap[ "DEF" ] == "def" ) {
      Settings.DefOn();
    }
    else {
      Settings.DefOff();
    }
  }
  if( optionMap.contains( "INDEX" ) ) {
    Settings.SetIndex(optionMap[ "INDEX" ].toInt());
  }
  if( optionMap.contains( "PrettyPrint_RTI" ) ) {
    Settings.SetPrettyPrintRTI(optionMap[ "PrettyPrint_RTI" ] == "1");
  }
  if( optionMap.contains( "C_flag" ) ) {
    Settings.SetCFlag(optionMap[ "C_flag" ].toInt());
  }
  if( optionMap.contains( "JCG_SKEL" ) ) {
    Settings.SetJCGSkel(optionMap[ "JCG_SKEL" ] == "1");
  }
  if( optionMap.contains( "JCG_GENPREPOST" ) ) {
    Settings.SetJCGGenPrePost(optionMap[ "JCG_GENPREPOST" ] == "1");
  }
  if( optionMap.contains( "JCG_TYPES" ) ) {
    Settings.SetJCGTypes(optionMap[ "JCG_TYPES" ] == "1");
  }
  if( optionMap.contains( "JCG_SMALLTYPES" ) ) {
    Settings.SetJCGSmallTypes(optionMap[ "JCG_SMALLTYPES" ] == "1");
  }
  if( optionMap.contains( "JCG_LONGS" ) ) {
    Settings.SetJCGLongs(optionMap[ "JCG_LONGS" ] == "1");
  }
  if( optionMap.contains( "JCG_PACKAGE" ) ) {
    QString package = optionMap[ "JCG_PACKAGE" ];
    Qt2TB::setJavaCGPackage(package);
  }
  if( optionMap.contains( "JCG_CONCUR" ) ) {
    Settings.SetJCGConcur(optionMap[ "JCG_CONCUR" ] == "1");
  }
  if( optionMap.contains( "JCG_CHECKPREPOST" ) ) {
    Settings.SetJCGCheckPrePost(optionMap[ "JCG_CHECKPREPOST" ] == "1");
  }
  if( optionMap.contains( "JCG_INTERFACES" ) ) {
    QStringList s;
    QString interfaces (optionMap[ "JCG_INTERFACES" ]);
    if( !interfaces.isEmpty() ) {
#if QT_VERSION >= 0x040000
      QStringList list (interfaces.split( ',' ));
#else
      QStringList list (QStringList::split( ',', interfaces ));
#endif // QT_VERSION >= 0x040000
      for( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        s.append(*it);
      }
    }
    Qt2TB::setSelectedInterfacesI(s);
  }
  if( optionMap.contains( "JCG_VDMPREFIX" ) ) {
    Settings.SetJCGVDMPrefix(optionMap[ "JCG_VDMPREFIX" ] == "1");
  }
  if( optionMap.contains( "Seed_nondetstmt" ) ) {
    int seed = optionMap[ "Seed_nondetstmt" ].toInt();
    if(seed == -1) {
      Settings.RandomOff();
    }
    else {
      Settings.RandomOn(seed);
    }
  }
  if( optionMap.contains( "JCG_USEDIRNAME" ) ) {
    Settings.SetJCGUseCodeDir(optionMap[ "JCG_USEDIRNAME" ] == "1");
  }
  if( optionMap.contains( "JCG_DIRNAME" ) ) {
    QString dirname = optionMap[ "JCG_DIRNAME" ];
    if(dirname.isEmpty()) {
      Settings.SetJCGCodeDir(Nil());
    }
    else {
      Settings.SetJCGCodeDir(Sequence(Qt2TB::qstring2wstring(dirname)));
    }
  }
  if( optionMap.contains( "JCG_NEEDBACKUP" ) ) {
    Settings.SetJCGNeedBackup(optionMap[ "JCG_NEEDBACKUP" ] == "1");
  }
}

QMap<QString, QString> Qt2TB::GetOptions()
{
  QMap<QString, QString> optionMap;
  optionMap[ "DTC" ] = (Settings.DTC() ? "1" : "0");
  optionMap[ "PRE" ] = (Settings.PreCheck() ? "1" : "0");
  optionMap[ "POST" ] = (Settings.PostCheck() ? "1" : "0");
  optionMap[ "INV" ] = (Settings.INV() ? "1" : "0");
  optionMap[ "MEASURE" ] = (Settings.Measure() ? "1" : "0");
  optionMap[ "CONTEXT" ] = (Settings.Context() ? "1" : "0");
#ifdef VDMPP
  optionMap[ "MAXINSTR" ] = wstring2qstring(Int(Settings.GetMaxInstr()).ascii());
  optionMap[ "PRIORITY" ] = (Settings.PriorityBased() ? "1" : "0");
  optionMap[ "PRIMARYALGORITHM" ] = wstring2qstring(Settings.GetPrimaryAlgorithmStr());
  optionMap[ "TASKSWITCH" ] = wstring2qstring(Int(Settings.GetTaskSwitch()).ascii());
  optionMap[ "MAXTIME" ] = wstring2qstring(Int(Settings.GetTimeSlice()).ascii());
  optionMap[ "TIMEFACTOR" ] = wstring2qstring(Int(Settings.GetTimeFactor()).ascii());
  optionMap[ "STEPSIZE" ] = wstring2qstring(Int(Settings.GetStepSize()).ascii());
  optionMap[ "JITTERMODE" ] = wstring2qstring(Settings.GetJitterModeStr());
  optionMap[ "DEFAULTCPUCAPACITY" ] = wstring2qstring(Int(Settings.GetDefaultCapacity()).ascii());
  optionMap[ "DEFAULTVCPUCAPACITY" ] = wstring2qstring(Settings.GetVirtualCPUCapacityStr());
  optionMap[ "LOGARGS" ] = wstring2qstring(Settings.GetLogArgsStr());
#endif // VDMPP
  optionMap[ "PRINT_FORMAT" ] = (Settings.PrintFormat() ? "1" : "0");
  optionMap[ "RTERR_EXCEPTION" ] = (Settings.RTErrException() ? "1" : "0");
  optionMap[ "EXPRESSION" ] = getExpression();
  optionMap[ "DEF" ] = (Settings.IsDEF() ? "def" : "pos");
  optionMap[ "errlevel" ] = (Settings.IsErrorLevelPRF() ? "1" : "0");
  optionMap[ "SEP" ] = (Settings.SEP() ? "1" : "0");
  optionMap[ "VDMSLMOD" ] = (Settings.VDMSLmode() ? "1" : "0");
  optionMap[ "VDM10" ] = (Settings.VDM10() ? "1" : "0");
  optionMap[ "OLD_REVERSE" ] = (Settings.OldReverse() ? "1" : "0");
  optionMap[ "INDEX" ] = wstring2qstring(Int(Settings.GetIndex()).ascii());
  optionMap[ "PrettyPrint_RTI" ] = (Settings.GetPrettyPrintRTI() ? "1" : "0");
  optionMap[ "CG_RTI" ] = (Settings.GetCGRTI() ? "1" : "0");
  optionMap[ "CG_CHECKPREPOST" ] = (Settings.GetCGCheckPrePost() ? "1" : "0");
  optionMap[ "C_flag" ] = wstring2qstring(Int(Settings.GetCFlag()).ascii());
  optionMap[ "JCG_SKEL" ] = (Settings.GetJCGSkel() ? "1" : "0");
  optionMap[ "JCG_GENPREPOST" ] = (Settings.GetJCGGenPrePost() ? "1" : "0");
  optionMap[ "JCG_TYPES" ] = (Settings.GetJCGTypes() ? "1" : "0");
  optionMap[ "JCG_SMALLTYPES" ] = (Settings.GetJCGSmallTypes() ? "1" : "0");
  optionMap[ "JCG_LONGS" ] = (Settings.GetJCGLongs() ? "1" : "0");
  optionMap[ "JCG_PACKAGE" ] = getJavaCGPackage();
  optionMap[ "JCG_CONCUR" ] = (Settings.GetJCGConcur() ? "1" : "0");
  optionMap[ "JCG_CHECKPREPOST" ] = (Settings.GetJCGCheckPrePost() ? "1" : "0");
  optionMap[ "JCG_VDMPREFIX" ] = (Settings.GetJCGVDMPrefix() ? "1" : "0");

  SET<TYPE_ProjectTypes_ModuleName> actualInterfaces (Settings.GetJCGInterfaces());
  QStringList interfaces_l;
  Generic mn;
  for (bool bb = actualInterfaces.First(mn); bb; bb = actualInterfaces.Next(mn)) {
    interfaces_l.append(wstring2qstring(PTAUX::ExtractModuleName(mn)));
  }
  QString interfaces;
  if( !interfaces_l.isEmpty() ) {
    for (QStringList::const_iterator it = interfaces_l.begin(); it != interfaces_l.end(); ++it) {
      if( interfaces.length() > 0 ) {
        interfaces += ",";
      }
      interfaces += (*it);
    }
  }
  optionMap[ "JCG_INTERFACES" ] = interfaces;
  optionMap[ "Seed_nondetstmt" ] = wstring2qstring(Int(Settings.Random()).ascii());
  optionMap[ "JCG_USEDIRNAME" ] = (Settings.GetJCGUseCodeDir() ? "1" : "0");
  QString dirname;
  if (!Settings.GetJCGCodeDir().IsNil()) {
    Sequence dirnameSeq (Settings.GetJCGCodeDir());
    QString dir = Qt2TB::wstring2qstring(dirnameSeq.GetString());
    QFileInfo fi (dir);
    if (fi.exists()) {
      optionMap[ "JCG_DIRNAME" ] = Qt2TB::wstring2qstring(dirnameSeq.GetString());
    }
    else {
      optionMap[ "JCG_USEDIRNAME" ] = "0";
      optionMap[ "JCG_DIRNAME" ] = "";
      Settings.SetJCGUseCodeDir(false);
      Settings.SetJCGCodeDir(Nil());
    }
  }
  optionMap[ "JCG_NEEDBACKUP" ] = (Settings.GetJCGNeedBackup() ? "1" : "0");
  return optionMap;
}

void Qt2TB::InitOptions()
{
  Settings.InitSettings();
}

void Qt2TB::SetJCGHackParser(bool b)
{
  Settings.SetJCGHackParser(b);
}

bool Qt2TB::GetJCGHackParser()
{
  return Settings.GetJCGHackParser();
}

bool Qt2TB::String2Bool(const QString & str)
{
  if( str == "0" || str == "false" || str == "off" ) {
    return false;
  }
  else if( str == "1" || str == "true" || str == "on" ) {
    return true;
  }
  return false;
}

//
// Interpreter
//
QString Qt2TB::getTmpBreakFileNameI()
{
  string dir;
#ifdef _MSC_VER
  char *tmpdir = getenv("TMP");
  if (tmpdir == 0) {
    dir = "c:/";
  }
  else {
    dir = tmpdir;
  }
  dir += "/vdm.bkp";
#else
  dir = "~/.vdm-breakpoint";
#endif
  return QString(dir.c_str());
}

void Qt2TB::ipStopI()
{
  TOOLS::BreakIP();
}

bool Qt2TB::isFileModifiedI(const QString& name)
{
  return ToolMediator::IsFileModified(PTAUX::mk_FileName(qstring2wstring(name)));
}

bool Qt2TB::stateIsSavedI()
{
  QStringList list (getProjectFilesI());
  if( !list.empty() ) {
    Bool b (ToolMediator::StateIsSaved());
    return b.GetValue();
  }
  return true;
}

void Qt2TB::SetSavedFileState(bool b)
{
  ToolMediator::SetSavedFileState(b);
}

bool Qt2TB::IsEverythingTypeCorrectI()
{
  return PTAUX::IsEverythingTypeCorrect();
}

QString Qt2TB::GiveToolTitleI()
{
  return wstring2qstring( TOOLS::giveToolTitle() );
}

QString Qt2TB::GiveToolVersionDateI()
{
  return wstring2qstring( TOOLS::giveToolVersionDate() );
}

QString Qt2TB::GetCompilerVersionI()
{
  return wstring2qstring( TOOLS::getCompilerVersion() );
}

QString Qt2TB::GetVersionIdI()
{
  return wstring2qstring( TOOLS::getVersionId() );
}

QString Qt2TB::GetCreditI()
{
  return wstring2qstring( TOOLS::getCredit() );
}

QString Qt2TB::getTraceArgsI(int index)
{
  return wstring2qstring(TOOLS::GetTraceArgs(index));
}

GUITokenList Qt2TB::getGUITokenListI( const QString & filename )
{
  SEQ<Char> file (ASTAUX::MkId(qstring2wstring( filename )));
  Sequence s (TOOLS::getGUITokenInfo( file ));
  GUITokenList list;
  size_t len_s = s.Length();
  for (size_t idx = 1; idx <= len_s; idx++) {
    Tuple t (s[idx]);
    GUITokenInfo gti (t.GetIntValue(1), t.GetIntValue(2), t.GetIntValue(3), t.GetIntValue(4));
    list.append(gti);
  }
  return list;
}

GUIOccurenceList Qt2TB::SearchIdI(const QStringList & files, const QString & id, bool partial, bool defOnly)
{
  SEQ<TYPE_ProjectTypes_FileName> file_l;
  for (QStringList::const_iterator it = files.begin(); it != files.end(); ++it) {
    file_l.ImpAppend(PTAUX::mk_FileName(qstring2wstring(*it)));
  }
  Sequence s (TOOLS::SearchId(file_l, qstring2wstring(id), partial, defOnly));

  GUIOccurenceList res;
  size_t len_s = s.Length();
  for (size_t idx = 1; idx <= len_s; idx++) {
    const Tuple & t (s[idx]);
    GUIOccurenceInfo oi (wstring2qstring(t.GetSequence(1).GetString()),
                         wstring2qstring(t.GetSequence(2).GetString()),
                         t.GetIntValue(3),
                         t.GetIntValue(4),
                         id.length(),
                         t.GetBoolValue(5));
    res.append(oi);
  }
  return res;
}

QString Qt2TB::getLanguageI()
{
  return wstring2qstring(TBUTILS::getLanguage()); 
}

QStringList Qt2TB::getChckMarks()
{
  Set s (PogInterface::getChckMarks());
  QStringList list;
  Generic g;
  for (bool bb = s.First(g); bb; bb = s.Next(g)) {
    list.append(wstring2qstring(PogInterface::printBool(g)));
  }
  return list;
}

QStringList Qt2TB::getModnms()
{
  Set s (PogInterface::getModnms());
  QStringList list;
  Generic g;
  for (bool bb = s.First(g); bb; bb = s.Next(g)) {
    list.append(wstring2qstring(PogInterface::printName(g)));
  }
  return list;
}

QStringList Qt2TB::getMemnms()
{
  Set s (PogInterface::getMemnms());
  QStringList list;
  Generic g;
  for (bool bb = s.First(g); bb; bb = s.Next(g)) {
    list.append(wstring2qstring(PogInterface::printName(g)));
  }
  return list;
}

QStringList Qt2TB::getLocs()
{
  Set s (PogInterface::getLocs());
  QStringList list;
  Generic g;
  for (bool bb = s.First(g); bb; bb = s.Next(g)) {
    list.append(wstring2qstring(PogInterface::printPOLocClassification(g)));
  }
  return list;
}

QStringList Qt2TB::getKinds()
{
  Set s (PogInterface::getKinds());
  QStringList list;
  Generic g;
  for (bool bb = s.First(g); bb; bb = s.Next(g)) {
    list.append(wstring2qstring(PogInterface::printPOKind(g)));
  }
  return list;
}

void Qt2TB::setChckMarks(const QStringList & l)
{
  Set s;
  for( QStringList::const_iterator it = l.begin(); it != l.end(); ++it ) {
    QString str (*it);
    s.Insert(str == "Yes" ? Bool(true) : Bool(false));
  }
  PogInterface::setChckMarks(s);
}

void Qt2TB::setModnms(const QStringList & l)
{
  Set s;
  for( QStringList::const_iterator it = l.begin(); it != l.end(); ++it ) {
    QString str (*it);
    Generic modName = Nil();
    if (!str.isEmpty()) {
      modName = ASTAUX::MkName(qstring2wstring(str));
    }
    s.Insert(modName);
  }
  PogInterface::setModnms(s);
}

void Qt2TB::setMemnms(const QStringList & l)
{
  Set s;
  for( QStringList::const_iterator it = l.begin(); it != l.end(); ++it ) {
    QString str (*it);
    Generic memName = Nil();
    if (!str.isEmpty()) {
      memName = ASTAUX::MkName(qstring2wstring(str));
    }
    s.Insert(memName);
  }
  PogInterface::setMemnms(s);
}

void Qt2TB::setLocs(const QStringList & l)
{
  Set s;
  for( QStringList::const_iterator it = l.begin(); it != l.end(); ++it ) {
    s.Insert(PogInterface::toLocs(qstring2wstring(*it)));
  }
  PogInterface::setLocs(s);
}

void Qt2TB::setKinds(const QStringList & l)
{
  Set s;
  for( QStringList::const_iterator it = l.begin(); it != l.end(); ++it ) {
    s.ImpUnion(PogInterface::toKind(qstring2wstring(*it)));
  }
  PogInterface::setKinds(s);
}

PogList Qt2TB::applyFilters()
{
  Sequence pos (GetPogInterface().applyFiltersText());
  PogList res;
  size_t len_pos = pos.Length();
  for (size_t idx = 1; idx <= len_pos; idx++) {
    Tuple t (pos[idx]);

    PogInfo pi (Qt2TB::wstring2qstring(t.GetSequence(1).GetString()), // checked
                Qt2TB::wstring2qstring(t.GetSequence(2).GetString()), // modnm
                Qt2TB::wstring2qstring(t.GetSequence(3).GetString()), // memnm
                Qt2TB::wstring2qstring(t.GetSequence(4).GetString()), // loc class
                Qt2TB::wstring2qstring(t.GetSequence(5).GetString()), // kind
                t.GetInt(6).GetValue(),                               // no
                Qt2TB::wstring2qstring(t.GetSequence(7).GetString()), // file
                Qt2TB::wstring2qstring(t.GetSequence(12).GetString()), // temp file
                t.GetInt(8).GetValue(),                               // line
                t.GetInt(9).GetValue(),                               // column
                t.GetInt(10).GetValue(),                              // length
                Qt2TB::wstring2qstring(t.GetSequence(11).GetString()) // po
    );
    res.append(pi);
  }
  return res;
}

QString Qt2TB::setCheckMark(const QString & modnm, const QString  & memnm,
                            const QString & kind, int no)
{
  Generic modName = Nil();
  if (!modnm.isEmpty()) {
    modName = ASTAUX::MkName(qstring2wstring(modnm));
  }
  Generic memName = Nil();
  if (!memnm.isEmpty()) {
    memName = ASTAUX::MkName(qstring2wstring(memnm));
  }

  TYPE_POGTP_LocContext loc;
  loc.Init(PogInterface::toLocs(qstring2wstring(kind)), modName, memName);

  Generic New (GetPogInterface().setCheckMark(mk_(loc, Int(no))));

  return wstring2qstring(PogInterface::printBool(New));
}

void Qt2TB::InitModuleTypesI()
{
  TOOLS::InitModuleTypes();
}

void Qt2TB::InitToolboxI(bool batch_mode)
{
  TOOLS::InitToolbox(batch_mode);
}

void Qt2TB::CleanUpI()
{
  TOOLS::CleanUp();
}

void Qt2TB::ExitToolboxI(int i)
{
  TOOLS::ExitToolbox(i);
}

bool Qt2TB::InitCorbaApiI(int argc, char *argv[], wstring &err)
{
  return TOOLS::InitCorbaApi(argc, argv, err);
}

void Qt2TB::TerminateCorbaApiI()
{
  TOOLS::TerminateCorbaApi();
}

std::wstring Qt2TB::GetFileNameI(int fileid)
{
  return ToolMediator::GetFileName (fileid);
}

std::wstring Qt2TB::GetSrcFileI(const wstring & file)
{
  if (!file.empty()) {
    TYPE_ProjectTypes_FileName tfn (ToolMediator::GetVDMTempFileName (PTAUX::mk_FileName (file)));
    std::wstring src (PTAUX::ExtractFileName (tfn));
    if (src.empty()) {
      return file;
    }
    else {
      return src;
    }
  }
  return std::wstring(L"");
}

