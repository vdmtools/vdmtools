/***
*  * WHAT
*  *    Implementation of preliminary methods in class StateStore
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/StateStore_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.11 $
*  * DATE
*  *    $Date: 2006/02/20 01:50:44 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    IFAD-VDM28
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "StateStore.h"
#include "projectval.h"
#include "tbutils.h"
#include "tb_wstring.h"

#include <fstream>
#include <string>

#ifdef VDMSL
static Quote project_id = Quote (L"ProjectFile");
#endif // VDMSL
#ifdef VDMPP
static Quote project_id = Quote (L"ProjectFilePP");
#endif // VDMPP

int StateStoreAux::current_ast_version = 3;
std::wstring StateStoreAux::projectfile;
StateStoreAux::stasto_type StateStoreAux::stype;
Sequence StateStoreAux::ProjectStore;

void StateStoreAux::AppendItem(const Generic & e)
{
  ProjectStore.ImpAppend(e);
}

Generic StateStoreAux::GetHeadItem()
{
  if (!ProjectStore.IsEmpty ()) {
    Generic e = ProjectStore.Hd();
    ProjectStore.ImpTl();
    return e;
  }
  else {
    return Nil();
  }
}

void StateStoreAux::ClearItems()
{
  ProjectStore.Clear();
}

void StateStoreAux::SetItems(const Sequence & s)
{
  ProjectStore = s;
}

Sequence StateStoreAux::GetItems()
{
  return ProjectStore;
}

void StateStoreAux::SetProjectFilename(const std::wstring & fnm)
{
  projectfile = fnm;
}

std::wstring StateStoreAux::GetProjectFilename()
{
  return projectfile;
}

void StateStoreAux::SetReadMode()
{
  stype = stasto_read;
}

void StateStoreAux::SetWriteMode()
{
  stype = stasto_write;
}

bool StateStoreAux::IsWriteMode()
{
  return (stype == stasto_write);
}

int StateStoreAux::ExtractVersion(const Generic & project, int current_version)
{
  if (project.IsSequence() && Sequence (project).Length() > 1) {
    Sequence l = project;
    Generic id = l[1];
    Generic num = l[2];
    if (id.IsQuote() && (id == project_id) && num.IsInt()) {
      int v = Int(num).GetValue();
      if (v > 1 && v <= current_version) {
        return v;
      }
    }
  }
  else {
    if (project.IsTuple () && Tuple (project).Length () == 2) {
      Generic version = Tuple (project).GetField (1);
      Generic ast = Tuple (project).GetField (2);
      if (version == Int (2) && ast.IsSequence ()) {
        return 1; // project version 1 identified
      }
    }
  }
  return 0; // invalid project
}

Sequence StateStoreAux::ConvertProject (int version, Generic project, int cur_ver)
{
  switch(version) {
    case 1: {
      Sequence old_project = Tuple(project).GetField(2);
      Sequence new_project;

      new_project.ImpAppend(Quote(L"ProjectFile"));
      new_project.ImpAppend(Int(2));

      if (old_project.Length() > 0) {
        old_project.ImpTl(); // Remove version number
      }
      // Remove the module environment map
      size_t len_old_project = old_project.Length();
      for (size_t idx = 1; idx <= len_old_project; idx++) {
        const Generic & e (old_project[idx]);
        if (e.IsMap()) {
          continue; // don't include the module environment map
        }
        else if (e.IsRecord()) {
          // potential problem with ast version
          new_project.ImpAppend(StateStoreAux::WriteAST(e));
        }
        else {
          new_project.ImpAppend(e);
        }
      }
      return new_project;
    }
    case 2: {
      // Version before AST restructuring.
      // Will remove AST, keep list of files in project.
      // Find files in version 2 project
      Sequence ConvProj;
      Sequence proj = project;
      size_t len_proj = proj.Length();
      for (size_t idx = 1; idx <= len_proj; idx++) {
        const Generic & e (proj[idx]);
        if (e.IsTuple() && Tuple(e).Length () == 2) {
          Tuple val (e);
          if (val.GetField (1).IsToken()) {
            Token tp = val.GetField (1);
            if (tp == Token(L"file")) {
              ConvProj.ImpAppend (val);
            }
          }
        }
      }
      // Prepend with number of files
      int numFiles = ConvProj.Length ();
      ConvProj.ImpPrepend (Int (numFiles));
      // Prepend with new version and Project file type
      ConvProj.ImpPrepend (Int (cur_ver));
      ConvProj.ImpPrepend (Sequence (project)[1]);
      return ConvProj;
    }
    default: {
      return Sequence(); // unknown version
    }
  }
}

Sequence StateStoreAux::AbsToRelPath(const Sequence & store, const std::wstring & pname)
{
  Sequence ret;
  ret.ImpAppend( store[1] ); // id
  ret.ImpAppend( store[2] ); // version
  ret.ImpAppend( store[3] ); // number of files
  int n = (Int)store[3];
  for( int i = 0; i < n; i++ ) {
    Tuple t (store[ i + 4 ]);
    if (Token(L"file") == t.GetField(1)) {
      Token s (t.GetField(2));
      std::wstring afile (s.GetString());
      std::wstring rfile (TBUTILS::GetRelativePath( afile, pname ));
      t.SetField( 2, Token( rfile ) );
    }
    ret.ImpAppend( t );
  }
  return ret;
}

// RelToAbsPath
// store :
// pname :
// ==>
Sequence StateStoreAux::RelToAbsPath(const Sequence & store, const std::wstring & pname)
{
  Sequence ret;
  ret.ImpAppend( store[1] ); // id
  ret.ImpAppend( store[2] ); // version
  ret.ImpAppend( store[3] ); // number of files
  int n = (Int)store[3];
  for( int i = 0; i < n; i++ ) {
    Tuple t (store[ i + 4 ]);
    if (Token(L"file") == t.GetField(1)) {
      Token s (t.GetField(2));
      std::wstring rfile (s.GetString());
      std::wstring afile (TBUTILS::GetAbsolutePath( rfile, pname ));
      t.SetField( 2, Token( afile ) );
    }
    ret.ImpAppend( t );
  }
  return ret;
}

/*
// WriteAST writes AST into the file name given by fnm result:
//   1 => success
//   2 => something went wrong. The AST has not written
bool TBUTILS::WriteAST (const Sequence& AST, const std::wstring& fnm)
{
  ofstream ostr;
  ostr.open(wstring2fsstr(fnm).c_str());
  if (!ostr.good ()) {
    vdm_log << L"Couldn't open file '" << fnm << L"'" << endl << flush;
    return false;
  }
  Tuple astrepos (2);
  astrepos.SetField (1, Int (current_ast_version));
  astrepos.SetField (2, AST);
  astrepos.WriteVal (ostr);
  ostr.close ();
  return true;
}
*/

Generic StateStoreAux::WriteAST (const Generic & AST)
{
  return mk_(Int (current_ast_version), AST);
}

/*
// ReadAST read an AST from the file name given by fnm
// result:
//   1 => success
//        Parameter AST will contain the resultion AST
//   2 => something went wrong. No AST has been read.
// side effects:
//   the global variable TestSuiteFile is updated with the name of the
//   file from which the AST was read.
bool TBUTILS::ReadAST (Sequence& AST, const std::wstring& fnm)
{
  ifstream istr (wstring2fsstr(fnm).c_str());
  if (istr) {
    Generic astrep = ReadVal (istr);
    if (!astrep.IsTuple () || Tuple (astrep).Length () != 2) {
      vdm_log << L"Content of '" << fnm << L"' is not a valid format"
              << endl << flush;
      return false;
    }
    Generic version = Tuple (astrep).GetField (1);
    Generic ast = Tuple (astrep).GetField (2);
    if (! (version == Int (current_ast_version)) || !ast.IsSequence ()) {
      vdm_log << L"Content of '" << fnm << L"' is outdated"
              << endl << flush;
      return false;
    }
    // We have a valid AST!
    AST = ast;
    //    TestSuiteFile = fnm;
    return true;
  }
  else {
    vdm_log << L"Couldn't open file '" << fnm << L"'" << endl << flush;
    return false;
  }
}
*/

Generic StateStoreAux::ReadAST (const Generic & store)
{
  if (!store.IsTuple () || Tuple (store).Length () != 2) {
    return Nil();
  }
  Generic version = Tuple (store).GetField (1);
  Generic ast = Tuple (store).GetField (2);
  if (! (version == Int (current_ast_version)) || !ast.IsRecord ()) {
    return Nil();
  }
  // We (possible) have a valid AST!
  return ast;
}

// Open
// fnm : ProjectTypes`FileName
// tp : (<READ> | <WRITE>)
// ==> bool
Bool vdm_StateStore::vdm_Open(const TYPE_ProjectTypes_FileName &fnm, const Generic & tp)
{
  std::wstring file = PTAUX::ExtractFileName (fnm);

  if (tp == Quote (L"WRITE")) {
// FIX : 20050928
// if the projectfile exists and is read only
    std::ifstream istr (TBWSTR::wstring2fsstr(file).c_str());
    if( istr ) {
      istr.close();
      std::ofstream ostr (TBWSTR::wstring2fsstr(file).c_str(), ios_base::app);
      if( !ostr ) {
        return Bool (false);
      }
    }
    StateStoreAux::ClearItems();
    StateStoreAux::SetWriteMode();
    StateStoreAux::SetProjectFilename(file);
    StateStoreAux::AppendItem(project_id);
    StateStoreAux::AppendItem(vdm_version);
    return Bool (true);
  }
  else {
    // (tp == Quote (L"READ"))
    std::ifstream istr (TBWSTR::wstring2fsstr(file).c_str());
    if (istr) {
      Generic astrep = ReadVal (istr);
      int version = StateStoreAux::ExtractVersion (astrep, vdm_version);
      if (version == 0) {
        vdm_log << L"Invalid project file " << file << std::endl;
        return Bool(false);
      }

      Sequence ss;
      if (version != vdm_version) {
        vdm_log << L"Project file " << file << L" is outdated (version " << version << L")." << std::endl;
        vdm_log << L"Updating project to version " << vdm_version << std::endl;
        ss = StateStoreAux::ConvertProject(version, astrep, vdm_version);
      }
      else {
        ss = astrep;
      }
      // hack for rerative path 2005/11/16
      Sequence s (StateStoreAux::RelToAbsPath( ss, file ));
      Sequence s2 (s.ImpTl()); // remove id
      Sequence s3 (s2.ImpTl()); // remove version

      StateStoreAux::ClearItems();
      StateStoreAux::SetReadMode();
      StateStoreAux::SetItems(s3);

      return Bool (true);
    }
    else {
      return Bool(false);
    }
  }
}

// Close
// ==> ()
void vdm_StateStore::vdm_Close()
{
  if (StateStoreAux::IsWriteMode()) {
    std::wstring projectfile (StateStoreAux::GetProjectFilename());
    ofstream ostr;
    ostr.open(TBWSTR::wstring2fsstr(projectfile).c_str());
    if (!ostr.good ()) {
      vdm_log << L"Couldn't open file '" << projectfile << std::endl;
    }
    else {

      Sequence ss (StateStoreAux::GetItems());
      // hack for rerative path 2005/11/16
      Sequence s (StateStoreAux::AbsToRelPath( ss, projectfile ));
      s.WriteVal (ostr);
      ostr.close ();
    }
  }
  StateStoreAux::ClearItems();
}

// WriteASTVal
// val : ProjectTypes`AstVal | <NONE>
// ==> bool
Bool vdm_StateStore::vdm_WriteASTVal(const Generic & val)
{
  if (val.IsRecord()) {
    StateStoreAux::AppendItem(StateStoreAux::WriteAST(PTAUX::ExtractAstVal(val)));
  }
  else {
    StateStoreAux::AppendItem(val);
  }
  return Bool (true);
}

// WriteName
// nm : ProjectTypes`FileName | ProjectTypes`ModuleName
// ==> bool
Bool vdm_StateStore::vdm_WriteName(const Record & nm)
{
  switch (nm.GetTag()) {
    case TAG_TYPE_ProjectTypes_ModuleName: {
      Tuple val (mk_(Token(L"module"), Token(PTAUX::ExtractModuleName(nm))));
      StateStoreAux::AppendItem(val);
      return Bool (true);
    }
    case TAG_TYPE_ProjectTypes_FileName: {
      Tuple val (mk_(Token(L"file"), Token(PTAUX::ExtractFileName(nm))));
      StateStoreAux::AppendItem(val);
      return Bool (true);
    }
    default: {
      return Bool (false);
    }
  }
}

// WriteSession
// session : ProjectTypes`SessionType
// ==> bool
Bool vdm_StateStore::vdm_WriteSession(const TYPE_ProjectTypes_SessionType & session)
{
  StateStoreAux::AppendItem(session);
  return Bool (true);
}

// WriteVal
// val : int | bool
// ==> bool
Bool vdm_StateStore::vdm_WriteVal(const Generic &val)
{
  StateStoreAux::AppendItem(val);
  return Bool (true);
}

// WriteStatus
// status : ProjectTypes`Status
// ==> bool
Bool vdm_StateStore::vdm_WriteStatus(const TYPE_ProjectTypes_Status & status)
{
  StateStoreAux::AppendItem(status);
  return Bool (true);
}

// WriteDep
// graph : ProjectTypes`DepGraph ==> bool
Bool vdm_StateStore::vdm_WriteDep (const TYPE_ProjectTypes_DepGraph & graph)
{
  StateStoreAux::AppendItem(graph);
  return Bool (true);
}

// WriteTCEnv
// e : ProjectTypes`TCEnv
//  ==> bool
Bool vdm_StateStore::vdm_WriteTCEnv (const TYPE_ProjectTypes_TCEnv & e)
{
  StateStoreAux::AppendItem(e);
  return Bool (true);
}

// ReadTCEnv
// ==> [ProjectTypes`TCEnv]
Generic vdm_StateStore::vdm_ReadTCEnv ()
{
  Generic e (StateStoreAux::GetHeadItem());
  if (e.Is(TAG_TYPE_ProjectTypes_TCEnv)) {
    return e;
  }
  else {
    return Nil();
  }
}

// ReadDep
// ==> [ProjectTypes`DepGraph]
Generic vdm_StateStore::vdm_ReadDep ()
{
  Generic d (StateStoreAux::GetHeadItem());
  if (d.Is(TAG_TYPE_ProjectTypes_DepGraph)) {
    return d;
  }
  else {
    return Nil();
  }
}

// ReadASTVal
// ==> [AstVal | <NONE>]
Generic vdm_StateStore::vdm_ReadASTVal()
{
  Generic val (StateStoreAux::GetHeadItem());
  if (!val.IsNil()) {
    if (val == Quote(L"NONE")) {
      return val;
    }
    else {
      Generic ast (StateStoreAux::ReadAST(val));
      if (ast.IsRecord()) {
        return PTAUX::mk_AstVal(ast);
      }
    }
  }
  return Nil();
}

// ReadName
// ==> [FileName | ModuleName]
Generic vdm_StateStore::vdm_ReadName()
{
  Generic g (StateStoreAux::GetHeadItem());
  if (g.IsTuple() && Tuple(g).Length () == 2) {
    Tuple val (g);
    Token tp (val.GetField (1));
    Token nm (val.GetField (2));
    if (tp == Token(L"module")) {
      return PTAUX::mk_ModuleName (nm.GetString());
    }
    else if (tp == Token(L"file")) {
      return PTAUX::mk_FileName (nm.GetString());
    }
  }
  return Nil();
}

// ReadSession
// ==> [SessionType]
Generic vdm_StateStore::vdm_ReadSession()
{
  Generic g (StateStoreAux::GetHeadItem());
  if (g == none_session || g == flat_session || g == struct_session) {
    return g;
  }
  else {
    return Nil ();
  }
}

// ReadVal
// ==> [int | bool]
Generic vdm_StateStore::vdm_ReadVal()
{
  Generic g (StateStoreAux::GetHeadItem());
  if (g.IsInt() || g.IsBool ()) {
    return g;
  }
  else {
    return Nil ();
  }
}

// ReadStatus
// ==>  [Status]
Generic vdm_StateStore::vdm_ReadStatus()
{
  Generic g (StateStoreAux::GetHeadItem());
  if (g.IsQuote()) {
    return g;
  }
  else {
    return Nil ();
  }
}
