/***
*  * WHAT
*  *    Code Generator Back-End.
*  *    backend.cc: Implementation of mod_cppast.vdm 1.24
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/backend.cc,v $
*  * VERSION
*  *    $Revision: 1.71 $
*  * DATE
*  *    $Date: 2006/04/19 08:11:25 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <fstream>

#include "cgtag.h"
#include "tbutils.h"
#include "backslash.h"
#include "projectval.h"
#include "CGTYPES.h"
#include "tb_wstring.h"
#include "backend.h"

#ifdef TESTSPEC
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _MSC_VER
#include <direct.h>             // For mkdir
#endif // _MSC_VER
extern wstring GetDateString(const char* format);
#else
#include "vdmcg.h"
#include "settings.h"
#include "contextinfo.h"
#endif // TESTSPEC

// The following strings are used when outputting Java,
// to retain hand editted code. They define the "keep tags" which
// appear before and after code
const string CGBackEnd::startString("// ***** VDMTOOLS START");
const string CGBackEnd::endString("// ***** VDMTOOLS END");
const string CGBackEnd::nameString("Name=");
const string CGBackEnd::keepString("KEEP=");

// String used for error messages when reading keep tags in a Java file
const wstring CGBackEnd::illFormed(L"Ignoring ill-formed line ");

void CGBackEnd::IND(ostream & o)
{
  string s;
  for (int i = 0; i < this->indent; i++)
  {
    s += " ";
    if ((i % 40) == 0)
    {
      o << s << flush;
      s = "";
    }
  }
  o << s << flush;
}

wstring CGBackEnd::PrintContext()
{
  switch (GetContext()) {
    case NF:      { return wstring(L"NF"); }
    case INCLUDE: { return wstring(L"INCLUDE"); }
    case IMPORT:  { return wstring(L"IMPORT"); }
    case MACRO:   { return wstring(L"MACRO"); }
    case FDEF:    { return wstring(L"FDEF"); }
    case FDECL:   { return wstring(L"FDECL"); }
    case BE_DECL: { return wstring(L"BE_DECL"); }
    case CLASS:   { return wstring(L"CLASS"); }
    case CSTMT:   { return wstring(L"CSTMT"); }
    case BLOCK:   { return wstring(L"BLOCK"); }
    case FOR:     { return wstring(L"FOR"); }
    case SWITCH:  { return wstring(L"SWITCH"); }
    case ELSE:    { return wstring(L"ELSE"); }
    default:      { return wstring(L"???"); }
  }
}

// GetName is used to extract the name of a class member, to be used
// in a keep tag. If it is applied to a portion of the CPP AST where
// it was not intended to be, the name "UNKNOWN" is generated.
wstring CGBackEnd::GetName(const Generic & decl)
{
  wstring name = L"UNKNOWN";
  if (decl.IsRecord()) {
    Record declRec (decl);

    // Break the record down to extract its declarator
    Record declval;
    switch (declRec.GetTag()) {
      case TAG_TYPE_CPP_FctDecl: {
        declval = declRec.GetRecord(pos_CPP_FctDecl_decl);
        break;
      }
      case TAG_TYPE_CPP_InitDeclarator: {
        declval = declRec.GetRecord(pos_CPP_InitDeclarator_decl);
        break;
      }
      case TAG_TYPE_CPP_ClassHead: {
        declval = TYPE_CPP_Identifier(declRec.GetRecord(pos_CPP_ClassHead_name));
        break;
      }
#ifdef VDMPP
      case TAG_TYPE_CPP_InterfaceHead: {
        declval = TYPE_CPP_Identifier(declRec.GetRecord(pos_CPP_InterfaceHead_name));
        break;
      }
#endif // VDMPP
    }
      // Extract the identifier from the declarator
    switch(declval.GetTag()){
      case TAG_TYPE_CPP_Identifier:
      {
//        Generic idval (declval.GetField(1));
        name = PTAUX::Seq2Str(declval.GetSequence(1));
      }
      default:
        break;
      }
  }
  else if (decl.IsSequence())
  {
      // If this is a sequence, it could be a string itself, ir a
      // sequence of declarators
    Sequence declSeq (decl);
    if (declSeq.Hd().IsChar())
      declSeq.GetString(name);
    else
      return GetName(Sequence(decl).Hd());
  }
  return name;
}

wstring CGBackEnd::GetMangleName(const Generic & decl)
{
  if (!decl.IsRecord() || !Record(decl).Is(TAG_TYPE_CPP_FctDecl))
  {
    return GetName(decl);
  }
  else
  {
    if (isJAVA())
    {
      wstring nm (GetName(decl));
      Generic arg (Record(decl).GetField(pos_CPP_FctDecl_arg));
      if (arg.IsNil()) return nm;

      TYPE_CPP_ArgumentDeclarationList adl (arg);
      Generic ad_lg (adl.GetField(pos_CPP_ArgumentDeclarationList_arg));
      if (ad_lg.IsNil()) return nm;

      SEQ<TYPE_CPP_ArgumentDeclaration> ad_l (ad_lg);
      if (ad_l.IsEmpty()) return nm;

      nm += L"#" + Int(ad_l.Length()).ascii();
      size_t len_ad_l = ad_l.Length();
      for (size_t i = 1; i <= len_ad_l; i++)
      {
        TYPE_CPP_ArgDecl ad (ad_l[i]);
        SEQ<TYPE_CPP_DeclSpecifier> ds_l (ad.GetSequence(pos_CPP_ArgDecl_ds));
        size_t len_ds_l = ds_l.Length();
        for (size_t j = 1; j <= len_ds_l; j++)
        {
          bool doBreak = false;
          TYPE_CPP_DeclSpecifier ds (ds_l[j]);
          if (ds.Is(TAG_TYPE_CPP_TypeSpecifier))
          {
            Generic tpg (ds.GetField(pos_CPP_TypeSpecifier_tp));
            if (tpg.IsRecord())
            {
              Record tp (tpg);
              switch(tp.GetTag()) {
                case TAG_TYPE_CPP_Identifier: {
                  nm += L"|" + tp.GetSequence(pos_CPP_Identifier_id).GetString();
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Char: {
                  nm += L"|char";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Short: {
                  nm += L"|short";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Int: {
                  nm += L"|int";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Bool: {
                  nm += L"|boolean";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Long: {
                  nm += L"|long";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Float: {
                  nm += L"|float";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Double: {
                  nm += L"|double";
                  doBreak = true;
                  break;
                }
                case TAG_TYPE_CPP_Void: {
                  nm += L"|void";
                  doBreak = true;
                  break;
                }
              }
            }
          }
          if (doBreak) break;
        }
      }
      return nm;
    }
    return GetName(decl);
  }
}

// StartKeepTag is used to output an opening keep tag to the generated
// file.
// Parameters:
//   Generic decl: the entity to be tagged
//   bool override: indicates whether the "level" variable should be
//     used or whether we should arbitrarily output the keep tag
//   bool keep: indicates whether this is a yes or no keep tag
// Returns: string representing the opening keep tag
string CGBackEnd::StartKeepTag(const Generic & decl, bool override, bool keep)
{
  if (isJAVA()) {
    // Only generate a keep tag if level is 1, or if the override
    // parameter is true. This ensures that for instance members of
    // inner classes do not have tags generated (breaking the
    // no-nesting rule)
    //if (level == 1 || override)
    if (GetLevel() == 1 || override)
    {
      string result = startString + " "
                      + nameString + TBWSTR::wstring2mbstr(GetMangleName(decl)) + " "
                      + keepString + (keep? "YES" : "NO") + "\n";
      return result;
    }
    else
      return "";
  }
  else
    return "";
}

// EndKeepTag is used to output an opening keep tag to the generated
// file.
// Parameters:
//   Generic decl: the entity to be tagged
//   bool override: indicates whether the "level" variable should be
//     used or whether we should arbitrarily output the keep tag
// Returns: string representing the closing keep tag
string CGBackEnd::EndKeepTag(const Generic & decl, bool override)
{
  if (isJAVA()) {
    // Use the same logic as for StartKeepTag, to ensure that they
    // always appear in matching start/end pairs.
    //if (level == 1 || override) {
    if (GetLevel() == 1 || override) {
      return endString + " " + nameString + TBWSTR::wstring2mbstr(GetMangleName(decl)) + "\n";
    }
    else
      return "";
  }
  else
    return "";
}

void CGBackEnd::GenFiles(const Generic & ast, enum cg_backend_kind kind)
{
  this->asttype = kind;

#ifdef TESTSPEC
  m4.open("m4code", ios::out);
#endif // TESTSPEC

  GenCode(ast);

#ifdef TESTSPEC
  m4.close();
#endif // TESTSPEC
}

#ifdef VDMPP
// GetHeadEntities - extracts the set of entities from the class header.
Set CGBackEnd::GetHeadEntities(const TYPE_CPP_PackageAndImportDeclarations& head)
{
  Set result;
  result.Insert(Sequence(L"package"));
  result.Insert(Sequence(L"imports"));
  return result;
}
#endif // VDMPP

// GetASTEntities - recursively traverses the ast and extracts the set of
// entities. The parameter inClass is used to indicate whether the
// traversal is occurring within the top-level class or not, to ensure
// that members of inner classes are not included in the resulting set
Set CGBackEnd::GetASTEntities(const Generic & ast, bool inClass)
{
  Set result;
  if (ast.IsSequence())
  {
    Generic g;
    Sequence astSeq(ast);
    for (bool bb = astSeq.First(g); bb; bb = astSeq.Next(g))
      result.ImpUnion(GetASTEntities(g, inClass));
  }
  else if (ast.IsRecord())
  {
    Record rc (ast);
    switch(rc.GetTag()) {
      case TAG_TYPE_CPP_TypeSpecifier: {
        return GetASTEntities(rc.GetRecord(pos_CPP_TypeSpecifier_tp), inClass);
        break;
      }
      case TAG_TYPE_CPP_IdentDeclaration:
      {
        if (inClass) {
          Generic dl (rc.GetField(pos_CPP_IdentDeclaration_dl));
          if (dl.IsSequence()) {
            if (!Sequence(dl).IsEmpty()) {
              result.Insert(Sequence(GetName(dl)));
            }
            else {
              // Must be a local class definition
              result.ImpUnion(GetASTEntities(rc.GetSequence(pos_CPP_IdentDeclaration_ds), inClass));
            }
          }
          else if (dl.IsNil()) {
              result.ImpUnion(GetASTEntities(rc.GetSequence(pos_CPP_IdentDeclaration_ds), inClass));
          }
        }
        else {
          result.ImpUnion(GetASTEntities(rc.GetSequence(pos_CPP_IdentDeclaration_ds), inClass));
        }
        break;
      }
      case TAG_TYPE_CPP_FunctionDefinition:
      {
// 20080818
//        result.Insert(Sequence(GetName(rc.GetRecord(pos_CPP_FunctionDefinition_decl))));
        result.Insert(Sequence(GetMangleName(rc.GetRecord(pos_CPP_FunctionDefinition_decl))));
        break;
      }
      case TAG_TYPE_CPP_ClassSpecifier:
#ifdef VDMPP
      case TAG_TYPE_CPP_InterfaceSpecifier:
#endif // VDMPP
      {
        Record ch (rc.GetRecord(1));
        Sequence ml (rc.GetSequence(2));

        if (!inClass)
          result.ImpUnion(GetASTEntities(ml, true));
        else
          result.Insert(Sequence(GetName(ch)));
        break;
      }
      default:
      {
        OutputErr(L"Internal error in the code generator backend (3).");
        OutputErr(L"Please report this error.");
        break;
      }
    }
  }
  return result;
}

#ifdef TESTSPEC
wstring CGBackEnd::GiveToolVersionDate ()
{
  return L"generated the specification of the code generator";
}
#else
#include "tb_version.h"
#endif // TESTSPEC

// CheckExisting is used to check whether an entity occurs in the
// existing file, and if so returns the code corresponding to it.
// Parameters:
//   string entityName - the name of the entity
//   bool override - indicates whether the "level" variable should be
//     used or whether we should arbitrarily output the keep tag
// Returns: string representing existing code, if it exists. Otherwise
//          the empty string.
string CGBackEnd::CheckExisting(wstring entityName, bool override)
{
  Sequence entityNameSeq (entityName);

  //if ((level != 1 || !isJAVA()) && !override)
  if ((GetLevel() != 1 || isCPP()) && !override)
    return "";

  wstring result = L"";
  Generic entityVal;
  if (existingEntitiesMap.DomExists(entityNameSeq, entityVal)) {
    type_cL entityCode = TYPE_CGTYPES_KeepCode(entityVal).get_code();
    entityCode.GetString(result);
  }
  return TBWSTR::wstring2mbstr(result);
}

void CGBackEnd::GenLogInfo (ostream & o)
{
  o << "//" << endl;
  o << "// THIS FILE IS AUTOMATICALLY GENERATED!!" << endl;
  o << "//" << endl;

#ifdef VDMSL
  string vdmLang = "VDM-SL";
#endif // VDMSL
#ifdef VDMPP
  string vdmLang = "VDM++";
#endif // VDMPP

#ifdef TESTSPEC
  o << "// Generated at " << TBWSTR::wstring2string(GetDateString( "%Y-%m-%d" ))
#else
  o << "// Generated at " << TBWSTR::wstring2string(TBUTILS::GetDateString( "%Y-%m-%d" ))
#endif // TESTSPEC
    << " by the " << vdmLang;
  o << " to " << (isJAVA() ? "JAVA" : "C++") << " Code Generator"
    << endl;

#ifdef TESTSPEC
  if (isCPP())
    o << "// (" << TBWSTR::wstring2string(GiveToolVersionDate ()) << ")" << endl;
#else
  o << "// (" << TBWSTR::wstring2string(tb_version.GiveToolVersionDate ()) << ")" << endl;
#endif // TESTSPEC

  o << "//" << endl;;

  if (isJAVA())
  {
/*
    o << "// Supported compilers: ";
    o << "jdk 1.4/1.5/1.6" << endl;
    o << "//" << endl;
    o << endl;
*/
/*
    string existingCode;
    if ((existingCode = CheckExisting(L"HeaderComment", true)) != ""){
      o << StartKeepTag(Sequence(L"HeaderComment"), true, true);
      o << existingCode;
    }
    else {
      o << StartKeepTag(Sequence(L"HeaderComment"), true, false);
    }
*/
    string existingCode (CheckExisting(L"HeaderComment", true));
    o << StartKeepTag(Sequence(L"HeaderComment"), true, !existingCode.empty());
    o << existingCode;
    o << EndKeepTag(Sequence(L"HeaderComment"), true);
    o << endl;
  }
  else
  {
/*
    o << "// Supported compilers:" << endl;
    o << "//   g++ version 3 on Linux, Mac OS X Panther(10.3), Sun Solaris 10" << endl;
    o << "//   g++ version 4 on Linux, Mac OS X Tiger(10.4), Leopard(10.5), Snow Leopard(10.6), Lion(10.7)," << endl;
    o << "//                    FreeBSD 8.2" << endl;
    o << "//   VC++ version 8/9/10 on Windows 2000/XP/Vista/7" << endl;
    o << "//" << endl;
    o << endl << flush;
*/
  }
}

// GetKeepTagName is an auxilliary function used by ReadJavaFile to
// extract the name of a Keep tag.
// Parameters:
//   string text      - text of tag
//   int searchPos    - position in text to start searching for name
//   int &endNamePos  - return the index of the end of the name string
//                      in this ref. param
//   int linenum      - line number in file on which text appears (for err
//                      messages)
//   string filename  - name of file in which text appears
//   bool expectSpace - flag indicating whether the name is terminated
//                      by a space or a newline. This depends on
//                      whether it is an opening or closing tag
//
// Returns: string representing the extracted name, or empty string if
// no name was found
wstring CGBackEnd::GetKeepTagName(const string & text,
                                  int searchPos, int &endNamePos,
                                  int linenum, const wstring& filename,
                                  bool expectSpace)
{
  wstring name;
  std::string::size_type namePos;
  if (expectSpace){
    namePos = text.find(nameString, searchPos + startString.length());
    endNamePos = text.find(" ", namePos);
  }
  else {
    namePos = text.find(nameString, searchPos + endString.length());
    endNamePos = text.find('\n', namePos);
  }

  if (namePos == string::npos){
    OutputLog(illFormed + Int(linenum).ascii() + L" in file " + filename);
    return L"";
  }
  else {
    int startPos = namePos + nameString.length();
    name = TBWSTR::mbstr2wstring(string(text, startPos, endNamePos - startPos));
    return name;
  }
}

// ReadJavaFile is used to extract those entities from the existing
// file that are to be kept.
// Parameters:
//   ifstream* javaFile - opened stream to the javaFile to be read
//   string filename - name of the file, used for error messages
// Returns:
//   Tuple with 2 fields. Field 1 : the generated keep map
//                        Field 2 : the generated keep sequence (ordering)
Tuple CGBackEnd::ReadJavaFile(ifstream* javaFile, const wstring & filename)
{
  int linenum = 0;
  int startLine = 0;
  bool keepingText = false;
  Sequence currentName;
  type_cL currentCode;
  Map keepMap;
  Sequence keepSequence;
  bool withinTag = false;

  while (javaFile->peek() != EOF) {
    string thisLine = "";
    while ((javaFile->peek() != EOF) && (javaFile->peek() != '\n'))
      thisLine += (char) javaFile->get();
    linenum++;
    javaFile->get();

    std::string::size_type searchPos;
    if ((searchPos = thisLine.find(startString)) != string::npos){
      if (withinTag) {
        OutputLog(L"Warning: Ignoring nested keep tag on line " +
                  Int(linenum).ascii() + L" in file " + filename);
      }
      else {
        currentCode = type_cL();
        int endNamePos;
        wstring nameStr = GetKeepTagName(thisLine, searchPos,
                                         endNamePos, linenum, filename, true);
        if (nameStr != L"") {
          currentName = Sequence(nameStr);
          int keepStart = endNamePos + keepString.length()+1;
          if (thisLine.substr(keepStart, 3) == "YES") {
            keepingText = true;
            startLine = linenum;
            withinTag = true;
          }
          else if (thisLine.substr(keepStart, 2) == "NO") {
            keepingText = false;
            withinTag = true;
          }
          else {
            OutputLog(illFormed + Int(linenum).ascii() + L" in file " + filename);
          }
        }
      }
    }
    else if ((searchPos = thisLine.find(endString)) != string::npos) {
      if (!withinTag) {
        OutputLog(L"Warning: Ignoring nested keep tag on line " +
                  Int(linenum).ascii() + L" in file " + filename);
      }
      else {
        int dummy;
        wstring nameStr (GetKeepTagName(thisLine, searchPos, dummy,
                                       linenum, filename, false));
        if (Sequence(nameStr) != currentName) {
          OutputLog(L"Warning: Ignoring End tag on line " +
                    Int(linenum).ascii() + L" in file " + filename +
                    L" that does not match Start tag on line " +
                    Int(startLine).ascii());
        }
        else {
          withinTag = false;
          if (keepingText) {
            keepingText = false;
            TYPE_CGTYPES_KeepCode kc;
            kc.Init(Int(startLine), currentCode);
            Generic g;
            if (keepMap.DomExists(currentName,g)) {
              TYPE_CGTYPES_KeepCode oldKc(g);
              OutputLog(L"Repeated tag at lines " +
                        oldKc.get_linenum().ascii() +
                        L" and " + Int(linenum).ascii() + L" - ignoring " +
                        Int(linenum).ascii());
            }
            else {
              keepMap.Insert(currentName, kc);
              keepSequence.ImpAppend(currentName);
            }
          }
        }
      }
    }
    else {
      if (keepingText)
      {
        thisLine += "\n";
        currentCode.ImpConc(Sequence(TBWSTR::mbstr2wstring(thisLine)));
      }
    }
  }
  return mk_(keepMap, keepSequence);
}

// Function for outputting newlines. It is important that this is
// used, as it makes debugging much easier than direct insertion of
// newline chars.
void CGBackEnd::GenNewLine(ostream & o)
{
  o << endl << flush;
}

void CGBackEnd::OpenBlock()
{
  m4code << "{";
  OpenScope();
}

void CGBackEnd::CloseBlock()
{
  CloseScope();
  IND(m4code);
  m4code << "}" << flush;
}

void CGBackEnd::GenFile(const TYPE_CPP_File & rc)
{
  SetContext(NF);
  NewScope();

  const Generic & pckname (rc.GetField(pos_CPP_File_dirname));
  const TYPE_CPP_FileName & name (rc.GetSequence(pos_CPP_File_name));
  const Generic & head (rc.GetField(pos_CPP_File_h));
  const TYPE_CPP_CPPAS & cpp (rc.GetSequence(pos_CPP_File_ast));

  wstring id (PTAUX::Seq2Str(name));

  if (isJAVA())
  {
    wstring cwd = L".";
    //cwd = L".";
#ifndef TESTSPEC
    if (Settings.GetJCGUseCodeDir())
    {
      if (!Settings.GetJCGCodeDir().IsNil())
      {
        Sequence dirnm (Settings.GetJCGCodeDir());
        cwd = dirnm.GetString();
      }
    }
#endif // TESTSPEC
    if (! pckname.IsNil())
    {
      Sequence packageDirs (pckname);
      Generic gg;
      for (bool bb = packageDirs.First(gg); bb; bb = packageDirs.Next(gg))
      {
        wstring dirname (PTAUX::Seq2Str(gg));
        cwd += L"/" + dirname;

#ifdef TESTSPEC
        string dname (TBWSTR::wstring2fsstr(cwd));
        struct stat stbuf;
        if ( stat( dname.c_str(), &stbuf ) == -1 )
        {
          OutputLog(L"Making directory " + cwd + L"...");

#ifndef _MSC_VER
          mkdir( dname.c_str(), 0777 );
#else
          _mkdir( dname.c_str() );
#endif // _MSC_VER
        }
#else
        TBUTILS::tb_mkdir(cwd, vdm_log);
#endif // TESTSPEC
      }
    }
    wstring oldid = id;
    if (cwd != L"")
      id = cwd + L"/" + oldid;


    ifstream javaFile (TBWSTR::wstring2fsstr(id).c_str());
    if (!javaFile) {
      // No existing file - set data structures to be empty
      existingEntitiesMap = Map();
      existingEntitiesOrder = Sequence();
      OutputLog(L"  Creating " + id);
    }
    else {
      // Read entities from existing file
      Tuple t ( ReadJavaFile(&javaFile, id) );
      javaFile.close();
      existingEntitiesMap = t.GetMap(1);
      existingEntitiesOrder = t.GetSequence(2);
      OutputLog(L"  Merging " + id + L" with existing file");

#ifndef TESTSPEC
      if (Settings.GetJCGNeedBackup())
#endif // TESTSPEC
      {
        // Back up existing file
        ifstream orgFile (TBWSTR::wstring2fsstr(id).c_str());
        wstring backupName (id + L".bak");
        ofstream bakFile(TBWSTR::wstring2fsstr(backupName).c_str());
        if (!orgFile || !bakFile)
          OutputLog(L"  Warning: unable to back up " + id);
        else {
          OutputLog(L"  Backing up " + id + L" in " + backupName);
          bakFile << orgFile.rdbuf();
        }
        bakFile.close();
        orgFile.close();
      }
    }
  } // end of isJAVA()

  // open output file
  m4code.open(TBWSTR::wstring2fsstr(id).c_str(), ios::out);

  GenLogInfo(m4code);

// 20120522 -->
#ifndef TESTSPEC
  TYPE_CI_ContextId cid (rc.GetInt(pos_CPP_File_cid));
  Tuple flcp (GetCI().GetFileLineColPos(cid));
  if (!flcp.GetField(1).IsNil())
  {
    m4code << "// This file was genereted from " << TBWSTR::wstring2mbstr(flcp.GetSequence(1).ascii()) << "." << endl;
    m4code << endl;
  }
#endif // TESTSPEC
// <--20120522

  if (isCPP()) {
    OutputLog(id + L"...");
  }

  int len = id.length()-2;
  if (len >= 0 && id.substr(len) == L".h") {
    string def ("_" + TBWSTR::wstring2mbstr(id.substr(0, len)) + "_h");

    m4code << "#ifndef " << def;
    GenNewLine(m4code);
    m4code << "#define " << def;
    GenNewLine(m4code);

    if (!head.IsNil() || (head.IsSequence() && !Sequence(head).IsEmpty())) {
      GenCode(head);
      GenNewLine(m4code);
//      GenNewLine(m4code);
    }

    GenCode(cpp);
    GenNewLine(m4code);
    GenNewLine(m4code);

    m4code << "#endif // " << def;
    GenNewLine(m4code);
  }
  else {
#ifdef VDMPP
    if (isJAVA()) {
      generatedEntities = GetHeadEntities(head);
    }
#endif // VDMPP

    GenCode(head);
    GenNewLine(m4code);

    if (isJAVA()) {
      generatedEntities.ImpUnion(GetASTEntities(cpp, false));
      generatedEntities.Insert(Sequence(L"HeaderComment"));
    }
    GenCode(cpp);
    GenNewLine(m4code);
  }

  // close output file
  m4code.close();

#ifdef TESTSPEC
  ifstream tmp;
  char c;
  tmp.open(TBWSTR::wstring2fsstr(id).c_str(),ios::in);
  m4 << "File: " << TBWSTR::wstring2string(id) << endl;
  while (tmp.get(c)) m4.put(c);
  m4 << "\n\n";
  tmp.close();
#endif // TESTSPEC
}

void CGBackEnd::GenTypeSpecifier(const TYPE_CPP_TypeSpecifier & rc)
{
  GenCode(rc.GetField(pos_CPP_TypeSpecifier_tp));
}

#ifdef VDMPP
void CGBackEnd::GenPackageAndImportDeclarations(const TYPE_CPP_PackageAndImportDeclarations & rc)
{
  string existingCode;
  if ((existingCode = CheckExisting(L"package", true)) != "")
  {
    m4code << StartKeepTag(Sequence(L"package"), true, true);
    m4code << existingCode;
  }
  else
  {
    m4code << StartKeepTag(Sequence(L"package"), true, false);
    const Generic & packdecl (rc.GetField(pos_CPP_PackageAndImportDeclarations_pd));  // [PackageDeclaration]
    if (!packdecl.IsNil()) {
      GenCode(packdecl);
//      GenNewLine(m4code); //20120208
    }
  }
  m4code << EndKeepTag(Sequence(L"package"), true);
  GenNewLine(m4code);

  if ((existingCode = CheckExisting(L"imports", true)) != "")
  {
    m4code << StartKeepTag(Sequence(L"imports"), true, true);
    m4code << existingCode;
  }
  else
  {
    const SEQ<TYPE_CPP_ImportDeclaration> & ds (rc.GetSequence(pos_CPP_PackageAndImportDeclarations_ims));
    m4code << StartKeepTag(Sequence(L"imports"), true, false);
    if (!ds.IsEmpty()) {
      SetContext(IMPORT); // 20120208
      GenCodeSeq(ds, "\n");
      GenNewLine(m4code);
    }
  }
  m4code << EndKeepTag(Sequence(L"imports"), true);
}

void CGBackEnd::GenPackageDeclaration(const TYPE_CPP_PackageDeclaration & rc)
{
  const TYPE_CPP_PackageName & pn (rc.GetRecord(pos_CPP_PackageDeclaration_name));

  m4code << "package ";
  GenCode(pn);
  m4code << ";";
  GenNewLine(m4code);
}

void CGBackEnd::GenTypeImportOnDemandDeclaration(const TYPE_CPP_TypeImportOnDemandDeclaration & rc)
{
  const TYPE_CPP_PackageName & tin (rc.GetRecord(pos_CPP_TypeImportOnDemandDeclaration_name));
  if (GetContext() != IMPORT)
    GenNewLine(m4code);

  m4code << "import ";
  GenCode(tin);
  m4code << ".*;";
  SetContext(IMPORT);
}

void CGBackEnd::GenSingleTypeImportDeclaration(const TYPE_CPP_SingleTypeImportDeclaration &rc)
{
  const TYPE_CPP_PackageName & stn (rc.GetRecord(pos_CPP_SingleTypeImportDeclaration_name));

  if (GetContext() != IMPORT) {
    GenNewLine(m4code);
  }
  m4code << "import ";
  GenCode(stn);
  m4code << ";";
  SetContext(IMPORT);
}
#endif // VDMPP

void CGBackEnd::GenIdMacroDef(const TYPE_CPP_IdMacroDef & rc)
{
  wstring id (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_IdMacroDef_id)));
  wstring ts (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_IdMacroDef_ts)));

  if (GetContext() != MACRO) {
    GenNewLine(m4code);
  }
  GenNewLine(m4code);
  m4code << "#define " << TBWSTR::wstring2mbstr(id) << " ";
  m4code << TBWSTR::wstring2mbstr(ts);
  SetContext(MACRO);
}

void CGBackEnd::GenFctMacroDef(const TYPE_CPP_FctMacroDef & rc)
{
  wstring id (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_FctMacroDef_id)));
  const TYPE_AS_Ids & id_l (rc.GetSequence(pos_CPP_FctMacroDef_id_ul) );
  wstring tc (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_FctMacroDef_ts)));

  if (GetContext() != MACRO)
    GenNewLine(m4code);

  GenNewLine(m4code);
  m4code << "#define " << TBWSTR::wstring2mbstr(id) << " ";
  m4code << "(";
  size_t len = id_l.Length();
  for (size_t i = 1; i <= len; i++)
  {
    if ( i > 1 )
      m4code << ", ";
    m4code << TBWSTR::wstring2mbstr(PTAUX::Seq2Str(id_l[i]));
  }
  m4code << ") " << TBWSTR::wstring2mbstr(tc);
  SetContext(MACRO);
}

void CGBackEnd::GenSquareIncl(const TYPE_CPP_SquareIncl& rc)
{
  wstring name (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_SquareIncl_name)));

  if (GetContext() != INCLUDE)
    GenNewLine(m4code);

  m4code << "#include ";
  m4code << "<" << TBWSTR::wstring2mbstr(name) << ">";

  GenNewLine(m4code);
  SetContext(INCLUDE);
}

void CGBackEnd::GenQuoteIncl(const TYPE_CPP_QuoteIncl & rc)
{
  wstring name (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_QuoteIncl_name)));

  if (GetContext() != INCLUDE)
    GenNewLine(m4code);

  m4code << "#include ";
  m4code << "\"" << TBWSTR::wstring2mbstr(name) << "\"";

  GenNewLine(m4code);
  SetContext(INCLUDE);
}

void CGBackEnd::GenIdentDeclaration(const TYPE_CPP_IdentDeclaration & rc)
{
//m4code << "/* GID - last = " << last << " */";
  const SEQ<TYPE_CPP_DeclSpecifier> & ds (rc.GetSequence(pos_CPP_IdentDeclaration_ds));
  const Generic & dl (rc.GetField(pos_CPP_IdentDeclaration_dl));
  const SEQ<TYPE_CPP_Annotation> & annos (rc.GetSequence(pos_CPP_IdentDeclaration_annos));

  if (isCPP())
  { // c++
    if ((GetContext() != CSTMT) && (GetContext() != BLOCK))
    {
      if (dl.IsSequence())
      {
        SEQ<TYPE_CPP_InitDeclarator> dl_l (dl);
        if (!dl_l.IsEmpty())
        {
          TYPE_CPP_InitDeclarator decl (dl_l[1]);
          if( GetContext() != FOR )
          {
//m4code << "/* last = " << last << " */";
            if (decl.Is(TAG_TYPE_CPP_FctDecl))
              SetContext(FDECL);
            else
              SetContext(BE_DECL);

            GenNewLine(m4code);
          }
        }
      }
      else
      {
//m4code << "/* last = " << last << " */";
        GenNewLine(m4code);
      }
    }
  }
  else { // Java
    if (GetContext() == CLASS)
      GenNewLine(m4code);
    if (dl.IsSequence() && !Sequence(dl).IsEmpty() && (GetContext() != FOR))
    {
//m4code << "/* last = " << last << " */";
      GenNewLine(m4code);
    }
  }

  bool useExistingCode = false;
  if (isJAVA())
  {
    if (dl.IsSequence())
      if (!Sequence(dl).IsEmpty()) {
        string existingCode;
        if ((existingCode = CheckExisting(GetName(dl))) == "")
        {
          m4code << StartKeepTag(dl, false, false);
          if (GetContext() != FOR)
            IND(m4code);
        }
        else {
          useExistingCode = true;
          m4code << StartKeepTag(dl, false, true);
          m4code << existingCode;
        }
      }
  }

  if (!useExistingCode) {
    if (isJAVA()) {
      if (!annos.IsEmpty()) {
        size_t len_annos = annos.Length();
        for (size_t idx = 1; idx <= len_annos; idx++) {
          GenCode(annos[idx]);
          IND(m4code);
        }
      }
    }
    GenCodeSeq(ds, " ");
    if (dl.IsSequence()) {
      if (!Sequence(dl).IsEmpty()) {
        m4code << " ";
        GenCodeSeq(dl, ", ");
      }
    }
// 20160609 -->
    bool semicolon = true;
    if ( !ds.IsEmpty() ) {
      const TYPE_CPP_DeclSpecifier & decl (ds[1]);
      if ( decl.Is(TAG_TYPE_CPP_TypeSpecifier) ) {
        const Generic & tp (decl.GetField(pos_CPP_TypeSpecifier_tp));
        if ( tp.Is(TAG_TYPE_CPP_ClassSpecifier) ) {
          semicolon = false;
        }
      }
    }
    if (semicolon ) {
      m4code << ";";
    }
// <-- 20160609
  }

  if (dl.IsSequence()) {
    if (!Sequence(dl).IsEmpty()) {
      if(!useExistingCode && (GetContext() != FOR) && (GetContext() != CSTMT))
        GenNewLine(m4code);
      m4code << EndKeepTag(dl, false);
    }
  }

  if (isCPP() || (dl.IsSequence() && !Sequence(dl).IsEmpty()))
    SetContext(BE_DECL);
  else
    SetContext(CLASS);

//m4code << "/* GDI end - last = " << last << " */";
}

void CGBackEnd::GenResScopeQualifiedClassName(const TYPE_CPP_ResScopeQualifiedClassName & rc)
{
  const TYPE_CPP_QualifiedClassName & qcn (rc.GetRecord(pos_CPP_ResScopeQualifiedClassName_qcn));

  if (isCPP())
    m4code << "::";
  GenCode(qcn);
}

void CGBackEnd::GenEnumSpecifier(const TYPE_CPP_EnumSpecifier & rc)
{
  const Generic & id (rc.GetField(pos_CPP_EnumSpecifier_id));
  const SEQ<TYPE_CPP_Enumerator> & el (rc.GetSequence(pos_CPP_EnumSpecifier_el));

  if (!el.IsEmpty())
  {
    if (isJAVA())
      m4code << "no enumeration in Java!!!";
    else {
      m4code << "enum ";
      if (!id.IsNil()) {
        GenCode(id);
        m4code << " ";
      }

      OpenBlock();

      GenNewLine(m4code);

      size_t len = el.Length();
      for (size_t i = 1; i <= len; i++)
      {
        IND(m4code);
        GenCode(el[i]);
        if ( i < len )
        {
          m4code << "," << endl << flush;
        }
      }

      GenNewLine(m4code);

      CloseBlock();
    }
    SetContext(BE_DECL);
  }
}

void CGBackEnd::GenEnumDefaultAssgn(const TYPE_CPP_EnumDefaultAssgn & rc)
{
  const TYPE_CPP_Identifier & id (rc.GetRecord(pos_CPP_EnumDefaultAssgn_id));
  const TYPE_CPP_Expr & ce (rc.GetRecord(pos_CPP_EnumDefaultAssgn_ce));

  GenCode(id);
  m4code << " = ";
  GenCode(ce);
}

void CGBackEnd::GenElaboratedTypeSpecifier(const TYPE_CPP_ElaboratedTypeSpecifier &rc)
{
  Quote ck (rc.GetField(pos_CPP_ElaboratedTypeSpecifier_ck));
  const Generic & id (rc.GetField(pos_CPP_ElaboratedTypeSpecifier_id));

  m4code << Quote2String(ck) + " ";
  GenCode(id);
}

void CGBackEnd::GenFunctionDefinition(const TYPE_CPP_FunctionDefinition & rc)
{
  const TYPE_CPP_Declarator & decl (rc.GetRecord(pos_CPP_FunctionDefinition_decl));

// 20150123 -->
//  GenNewLine(m4code);
// <-- 20150123
  GenNewLine(m4code);

  string existingCode;
//  if ((existingCode = CheckExisting(GetName(decl))) != L"")
  if ((existingCode = CheckExisting(GetMangleName(decl))) != "")
  {
    m4code << StartKeepTag(decl, false, true);
    m4code << existingCode;
  }
  else
  {
    const TYPE_CPP_AnnotationSpec & annos (rc.GetSequence(pos_CPP_FunctionDefinition_annos));
    const TYPE_CPP_Modifiers & ms (rc.GetRecord(pos_CPP_FunctionDefinition_m));
    const SEQ<TYPE_CPP_DeclSpecifier> & ds (rc.GetSequence(pos_CPP_FunctionDefinition_ds));
    const Generic & ci (rc.GetField(pos_CPP_FunctionDefinition_ci));
    const Generic & tc (rc.GetField(pos_CPP_FunctionDefinition_throw));
    const Generic & body (rc.GetField(pos_CPP_FunctionDefinition_fb));

    m4code << StartKeepTag(decl, false, false);
    IND(m4code);
    //level++;
    IncLevel();

    if (!annos.IsEmpty()) {
      size_t len_annos = annos.Length();
      for (size_t idx = 1; idx <= len_annos; idx++) {
        GenCode(annos[idx]);
        IND(m4code);
      }
    }

    if (!ms.IsNil()) {
      TYPE_CPP_Modifiers mss (ms);
      size_t len_mss = mss.Length();
      for (size_t i = 1; i <= len_mss; i++)
        m4code << Quote2String(Quote(mss[i].get_ma())) << " ";
    }

    if (!ds.IsEmpty()) {
      GenCodeSeq(ds, " ");
      m4code << " ";
    }
    GenCode(decl);
    m4code << " ";

    if (!ci.IsNil()) {
      SEQ<TYPE_CPP_MemInitializer> cis (ci);
      if (!cis.IsEmpty()) {
        m4code << ": ";
        GenCodeSeq(cis, ", ");
        m4code << " ";
      }
    }

    if (!tc.IsNil()) {
      SEQ<TYPE_CPP_SimpleTypeName> excs (tc);
      if (!excs.IsEmpty()){
        m4code << "throws ";
        GenCodeSeq(excs, ", ");
        m4code << " ";
      }
    }

    SetContext(BE_DECL);

    if (!body.IsNil())
      GenCode(body);
    else
      m4code << ";";

    GenNewLine(m4code);
    //level--;
    DecLevel();
  }
  SetContext(FDEF);
  m4code << EndKeepTag(decl, false);
}

void CGBackEnd::GenMemInitializer(const TYPE_CPP_MemInitializer & rc)
{
  const Record & name (rc.GetRecord(pos_CPP_MemInitializer_name));
  const SEQ<TYPE_CPP_Expr> & el (rc.GetSequence(pos_CPP_MemInitializer_el));

  GenCode(name);
  m4code << "(";
  GenCodeSeq(el, ", ");
  m4code << ")";
}

void CGBackEnd::GenFctBody(const TYPE_CPP_FctBody & rc)
{
//m4code << "/* GenFctBody - last = " << last << " */";
  const TYPE_CPP_CompoundStmt & stmt (rc.GetRecord(pos_CPP_FctBody_stmt));
  GenCode(stmt);
//m4code << "/* GenFctBody end */";
}

void CGBackEnd::GenInitDeclarator(const TYPE_CPP_InitDeclarator & rc)
{
  const TYPE_CPP_Declarator & decl (rc.GetRecord(pos_CPP_InitDeclarator_decl));
  const Generic & i (rc.GetField(pos_CPP_InitDeclarator_i));
  GenCode(decl);
  if (!i.IsNil())
    GenCode(i);
}

void CGBackEnd::GenAsgnInit(const TYPE_CPP_AsgnInit & rc)
{
  const TYPE_CPP_Expr & expr (rc.GetRecord(pos_CPP_AsgnInit_expr));
  m4code << " = ";
  GenCode(expr);
}

void CGBackEnd::GenEnumInit(const TYPE_CPP_EnumInit & rc)
{
  const TYPE_CPP_InitializerList & ilist (rc.GetRecord(pos_CPP_EnumInit_in_ul));

  m4code << " = {";
  GenCode(ilist);
  m4code << "}";
}

void CGBackEnd::GenObjectInit(const TYPE_CPP_ObjectInit & rc)
{
  const SEQ<TYPE_CPP_Expr> & exprs (rc.GetSequence(pos_CPP_ObjectInit_exprs));

  m4code << " (";
  GenCodeSeq(exprs, ", ");
  m4code << ")";
}

void CGBackEnd::GenInitializerList(const TYPE_CPP_InitializerList & rc)
{
  const type_dL & seq (rc.GetSequence(pos_CPP_InitializerList_list));
  const Record & elem (seq[1]);

  if (elem.Is(TAG_TYPE_CPP_InitializerList)) {
    m4code << "{";
    GenCodeSeq(seq, ", ");
    m4code << "}";
  }
  else
    GenCodeSeq(seq, ", ");
}

void CGBackEnd::GenTypeDefName(const TYPE_CPP_TypeDefName & rc)
{
  const TYPE_CPP_Identifier & id (rc.GetRecord(pos_CPP_TypeDefName_id));
  GenCode(id);
}

void CGBackEnd::GenCode(const Generic & ast)
{
  if (ast.IsRecord()) {
    Record rc(ast);
    switch(rc.GetTag()) {

     // Files and preprocessors
    case TAG_TYPE_CPP_File: {
      GenFile(ast);
      break;
    }

    case TAG_TYPE_CPP_TypeSpecifier: {
      GenTypeSpecifier(ast);
      break;
    }

#ifdef VDMPP
    // ******* JAVA TAGS ************************************//

    case TAG_TYPE_CPP_PackageAndImportDeclarations: {
      GenPackageAndImportDeclarations(ast);
      break;
    }

    case TAG_TYPE_CPP_PackageDeclaration: {
      GenPackageDeclaration(ast);
      break;
     }

    case TAG_TYPE_CPP_TypeImportOnDemandDeclaration: {
      GenTypeImportOnDemandDeclaration(ast);
      break;
    }

    case TAG_TYPE_CPP_SingleTypeImportDeclaration: {
      GenSingleTypeImportDeclaration(ast);
      break;
    }
#endif // VDMPP

    // ******* CPP TAGS ************************************//

    case TAG_TYPE_CPP_IdMacroDef: {
      GenIdMacroDef(ast);
      break;
    }

    case TAG_TYPE_CPP_FctMacroDef: {
      GenFctMacroDef(ast);
      break;
    }

    case TAG_TYPE_CPP_SquareIncl: {
      GenSquareIncl(ast);
      break;
    }

    case TAG_TYPE_CPP_QuoteIncl: {
      GenQuoteIncl(ast);
      break;
    }

    // ******* CPP and JAVA TAGS ************************************//

    // Definitions
    case TAG_TYPE_CPP_IdentDeclaration: {
      GenIdentDeclaration(ast);
      break;
    }

    case TAG_TYPE_CPP_ResScopeQualifiedClassName: {
      GenResScopeQualifiedClassName(ast);
      break;
    }

    /** no enumeration in Java **/
    case TAG_TYPE_CPP_EnumSpecifier: {
      GenEnumSpecifier(ast);
      break;
    }

    case TAG_TYPE_CPP_EnumDefaultAssgn: {
      GenEnumDefaultAssgn(ast);
      break;
    }

    case TAG_TYPE_CPP_ElaboratedTypeSpecifier: {
      GenElaboratedTypeSpecifier(ast);
      break;
    }

    case TAG_TYPE_CPP_Char:
      m4code << "char";
      break;

    case TAG_TYPE_CPP_Short:
      m4code << "short";
      break;

    case TAG_TYPE_CPP_Int:
      m4code << "int";
      break;

    case TAG_TYPE_CPP_Bool:
      m4code << (isJAVA() ? "boolean" : "bool");
      break;

    case TAG_TYPE_CPP_Long:
      m4code << "long";
      break;

    case TAG_TYPE_CPP_Signed:
      if (isCPP())
        m4code <<  "signed";
      break;

    case TAG_TYPE_CPP_Unsigned:
      if (isCPP())
        m4code <<  "unsigned";
      break;

    case TAG_TYPE_CPP_Float:
      m4code << "float";
      break;

    case TAG_TYPE_CPP_Double:
      m4code << "double";
      break;

    case TAG_TYPE_CPP_Void:
      m4code << "void";
      break;

    case TAG_TYPE_CPP_Friend:
      m4code << (isJAVA() ? "no friends in JAVA" : "friend");
      break;

    case TAG_TYPE_CPP_TypeDef:
      m4code << (isJAVA() ? "no typedef in JAVA" : "typedef");
      break;

    case TAG_TYPE_CPP_FunctionDefinition: {
      GenFunctionDefinition(ast);
      break;
    }

    case TAG_TYPE_CPP_MemInitializer: {
      GenMemInitializer(ast);
      break;
    }

    case TAG_TYPE_CPP_FctBody: {
      GenFctBody(ast);
      break;
    }

    // Declarators
    case TAG_TYPE_CPP_InitDeclarator: {
      GenInitDeclarator(ast);
      break;
    }

    case TAG_TYPE_CPP_AsgnInit: {
      GenAsgnInit(ast);
      break;
    }

    case TAG_TYPE_CPP_EnumInit: {
      GenEnumInit(ast);
      break;
    }

    case TAG_TYPE_CPP_ObjectInit: {
      GenObjectInit(ast);
      break;
    }

    case TAG_TYPE_CPP_InitializerList: {
      GenInitializerList(ast);
      break;
    }

    case TAG_TYPE_CPP_TypeDefName: {
      GenTypeDefName(ast);
      break;
    }

//    case TAG_TYPE_CPP_ClassResScopeQualifiedClassName:
    case TAG_TYPE_CPP_IndirectionDecl:
    case TAG_TYPE_CPP_IndirectionAbsDecl: {
      const Generic & cvlist (rc.GetField(1));
      const Generic & decl (rc.GetField(2));

      if (isJAVA()) {
        m4code << "*???";
      }
      else {
        m4code << "*";
      }
      if (!cvlist.IsNil()) {
        GenCodeSeq(cvlist, " ");
      }
      if (!decl.IsNil()) {
        m4code << " ";
        GenCode(decl);
      }
      break;
    }

    case TAG_TYPE_CPP_RefDecl:
    case TAG_TYPE_CPP_RefAbsDecl: {
      const Generic & cvlist (rc.GetField(1));
      const Generic & decl (rc.GetField(2));

      if (isCPP()) {
        m4code << "&";
      }
      if (!cvlist.IsNil()) {
        GenCodeSeq(cvlist, " ");
        m4code << " ";
      }
      if (!decl.IsNil()) {
        GenCode(decl);
      }
      break;
    }

    case TAG_TYPE_CPP_PointerToMember:
    case TAG_TYPE_CPP_PointerToMemberAbs: {
      const TYPE_CPP_CompleteClassName & ccn (rc.GetField(1));
      const Generic & cvlist (rc.GetField(2));
      const Record & decl (rc.GetField(3));

      GenCode(ccn);
      if (isJAVA()) {
        m4code << "::* does not exist in JAVA";
      }
      else {
        m4code << "::*";
      }
      if (!cvlist.IsNil()) {
        GenCodeSeq(cvlist, " ");
        m4code << " ";
      }
      GenCode(decl);
      break;
    }
    case TAG_TYPE_CPP_FctDecl:
    case TAG_TYPE_CPP_FctAbsDecl: {
      const Generic & decl (rc.GetField(1));
      const Generic & arg (rc.GetField(2));
      const Generic & cvlist (rc.GetField(3));

      GenCode(decl);

      if (!arg.IsNil()){
        m4code <<  " (";
        GenCode(arg);
        m4code <<  ")";
      }
      if (!cvlist.IsNil()) {
        m4code <<  " ";
        GenCodeSeq(cvlist, " ");
      }
      SetContext(BE_DECL);
      break;
    }

    case TAG_TYPE_CPP_ArrayDecl:
    case TAG_TYPE_CPP_ArrayAbsDecl: {
      const Generic & decl (rc.GetField(1));
      const Generic & expr (rc.GetField(2));

      GenCode(decl);
      m4code << "[";
      GenCode(expr);
      m4code << "]";
      break;
    }

    case TAG_TYPE_CPP_BracketedDecl:
    case TAG_TYPE_CPP_BracketedAbsDecl: {
      const Generic & decl (rc.GetField(1));

      m4code << "(";
      GenCode(decl);
      m4code << ")";
      break;
    }

    case TAG_TYPE_CPP_TypeName: {
      const SEQ<TYPE_CPP_TypeSpecifier> & tsp (rc.GetSequence(pos_CPP_TypeName_tsp));
      const Generic ad (rc.GetField(pos_CPP_TypeName_ad));

      GenCodeSeq(tsp, " ");
      if (!ad.IsNil()) {
        m4code <<  " ";
        GenCode(ad);
      }
      break;
    }

    case TAG_TYPE_CPP_ArgumentDeclarationList: {
      const Generic & arg (rc.GetField(pos_CPP_ArgumentDeclarationList_arg));
      const Generic & ellipsis (rc.GetField(pos_CPP_ArgumentDeclarationList_ellipsis));

      if (!arg.IsNil()) {
        GenCodeSeq(arg, ", ");
      }
      if (!ellipsis.IsNil()) {
        if (isJAVA()) {
          m4code << "... not possible in JAVA";
        }
        else {
          m4code << " " + Quote2String(Quote(ellipsis));
        }
      }
      break;
    }

    case TAG_TYPE_CPP_ArgDecl:
    case TAG_TYPE_CPP_ArgAbsDecl: {
      const SEQ<TYPE_CPP_DeclSpecifier> & ds (rc.GetSequence(1));
      const Generic & decl (rc.GetField(2));

      GenCodeSeq(ds, " ");
      if (!decl.IsNil()) {
        m4code << " ";
        GenCode(decl);
      }
      break;
    }

    case TAG_TYPE_CPP_DefaultArgDecl:
    case TAG_TYPE_CPP_DefaultArgAbsDecl: {
      const SEQ<TYPE_CPP_DeclSpecifier> & ds (rc.GetSequence(1));
      const Generic & decl (rc.GetField(2));

      GenCodeSeq(ds, " ");
      if (!decl.IsNil()) {
        m4code << " ";
        GenCode(decl);
      }
      Generic expr (rc.GetField(3));
      m4code <<  "= ";
      GenCode(expr);
      break;
    }

    case TAG_TYPE_CPP_VarArgDecl: {
      const SEQ<TYPE_CPP_DeclSpecifier> & ds (rc.GetSequence(1));
      const Generic & decl (rc.GetField(2));

      GenCodeSeq(ds, " ");
      if (!decl.IsNil()) {
        m4code << " ... ";
        GenCode(decl);
      }
      break;
    }

    // Class Declarations
    case TAG_TYPE_CPP_ClassSpecifier: {
      GenClassSpecifier(rc);
      break;
    }

    case TAG_TYPE_CPP_ClassHead: {
      GenClassHead(rc);
      break;
    }

    case TAG_TYPE_CPP_MemberList: {
      GenMemberList(rc);
      break;
    }

    case TAG_TYPE_CPP_MemberSpecifier: {
      GenMemberSpecifier(rc);
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_CPP_InterfaceSpecifier: {
      GenInterfaceSpecifier(rc);
      break;
    }

    case TAG_TYPE_CPP_InterfaceHead: {
      GenInterfaceHead(rc);
      break;
    }
#endif // VDMPP

    case TAG_TYPE_CPP_DeclPureSpec: {
      const Record & decl (rc.GetRecord(pos_CPP_DeclPureSpec_decl));
      const Generic & pure (rc.GetField(pos_CPP_DeclPureSpec_puresp));

      GenCode(decl);

      if (!pure.IsNil()) {
        m4code << "=0";
      }
      break;
    }

    /** no virtuals in Java**/
    case TAG_TYPE_CPP_VirCompleteClassSpecifier: {
      const Generic & a (rc.GetField(pos_CPP_VirCompleteClassSpecifier_a));
      const TYPE_CPP_CompleteClassName & cen (rc.GetRecord(pos_CPP_VirCompleteClassSpecifier_ccn));

      m4code << "virtual";

      if (isJAVA()) {
        m4code << "????";
      }
      if (!a.IsNil()) {
        m4code << " " << Quote2String(Quote(a));
      }
      m4code << " ";

      GenCode(cen);
      break;
    }

    // Java only
    case TAG_TYPE_CPP_Annotation: {
      const TYPE_CPP_Identifier & cls (rc.GetRecord(pos_CPP_Annotation_cls));
      const Generic & args (rc.GetRecord(pos_CPP_Annotation_args));

      m4code << "@";
      GenCode(cls);

      if (!args.IsNil()) {
        m4code << "(";
        m4code << TBWSTR::wstring2mbstr(PTAUX::Seq2Str(args));
        m4code << ")";
      }
      GenNewLine(m4code);
      break;
    }

    case TAG_TYPE_CPP_AccCompleteClassSpecifier: {
      Quote a (rc.GetField(pos_CPP_AccCompleteClassSpecifier_a));
      const Generic & v (rc.GetField(pos_CPP_AccCompleteClassSpecifier_v));
      const TYPE_CPP_CompleteClassName & ccn (rc.GetRecord(pos_CPP_AccCompleteClassSpecifier_ccn));

      m4code << Quote2String(a); // <PRIVATE> | <PROTECTED> | <PUBLIC>
      if (!v.IsNil()) {
        m4code << " virtual";

        if (isJAVA()) {
          m4code << "????";
        }
      }
      m4code << " ";

      GenCode(ccn);
      break;
    }

    // Statements
    /** Syntax for label-statement the same in Java and C++ **/
    case TAG_TYPE_CPP_LabelStmt: {
      const TYPE_CPP_Identifier & id (rc.GetRecord(pos_CPP_LabelStmt_id));
      const TYPE_CPP_Stmt & stmt (rc.GetRecord(pos_CPP_LabelStmt_stmt));

      GenCode(id);
      m4code << ":";
      GenNewLine(m4code);
      SetContext(CSTMT);
      GenCode(stmt);
      break;
    }

    /** Syntax for case-statement the same in Java and C++ **/
    case TAG_TYPE_CPP_CaseStmt: {
      const TYPE_CPP_Expr & cexpr (rc.GetRecord(pos_CPP_CaseStmt_cexpr));
      const Generic & stmt (rc.GetRecord(pos_CPP_CaseStmt_stmt));

      GenNewLine(m4code); // 20070911

      IND(m4code);

      m4code << "case ";
      GenCode(cexpr);
      m4code << ": ";

      SetContext(BLOCK);

      OpenScope();
      GenStmt(stmt, true);
      CloseScope();
      break;
    }

    /** Syntax for default-statement the same in Java and C++ **/
    case TAG_TYPE_CPP_DefaultStmt: {
      const Generic & stmt (rc.GetRecord(pos_CPP_DefaultStmt_stmt));

      GenNewLine(m4code); // 20070911
      IND(m4code);

      m4code << "default: ";

      SetContext(BLOCK);

      OpenScope();
      GenStmt(stmt, true);
      CloseScope();
      break;
    }

    /** Syntax for if-statement the same in Java and C++ **/
    case TAG_TYPE_CPP_IfStmt: {
      GenIfStmt(rc);
      break;
    }

    /** Syntax for expression-statement the same in Java and C++ **/
    case TAG_TYPE_CPP_ExpressionStmt: {
      GenExpressionStmt(rc);
      break;
    }

    /** Syntax for statement block the same in Java and C++ **/
    case TAG_TYPE_CPP_CompoundStmt: {
      GenCompoundStmt(rc);
      break;
    } // TAG_TYPE_CPP_CompoundStmt

    /** Syntax for switch-statement the same in Java and C++ **/
    case TAG_TYPE_CPP_SwitchStmt: {
      const TYPE_CPP_Expr & expr (rc.GetRecord(pos_CPP_SwitchStmt_expr));
      const TYPE_CPP_SwitchBlock & block (rc.GetRecord(pos_CPP_SwitchStmt_block));

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);
      m4code << "switch(";
      GenCode(expr);
      m4code << ") ";

      SetContext(SWITCH);
      GenStmt(block, true);
      break;
    }

    case TAG_TYPE_CPP_SwitchBlock: {
      const SEQ<TYPE_CPP_SwitchBlockStmtGrp> & groups (rc.GetSequence(pos_CPP_SwitchBlock_groups));
      const Generic & labels (rc.GetField(pos_CPP_SwitchBlock_labels));

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);

      OpenBlock();

      SetContext(BLOCK);

      GenCodeSeq(groups, "");

      if (!labels.IsNil()) {
        GenCode(labels);
      }
      GenNewLine(m4code);
      CloseBlock();

      GenNewLine(m4code);
      break;
    }

    case TAG_TYPE_CPP_SwitchBlockStmtGrp: {
      const Generic & labels (rc.GetField(pos_CPP_SwitchBlockStmtGrp_labels));
      const Generic & stmt (rc.GetField(pos_CPP_SwitchBlockStmtGrp_stmt));

      if (!labels.IsNil()) {
        GenCode(labels);
      }
      if (!stmt.IsNil()) {
        GenCode(stmt);
      }
      break;
    }

    case TAG_TYPE_CPP_SwitchLabels: {
      const Record & stmt (rc.GetRecord(pos_CPP_SwitchLabels_stmt));
      const Generic & labels (rc.GetField(pos_CPP_SwitchLabels_labels));

      GenCode(stmt);
      if (!labels.IsNil()) {
        GenCode(labels);
      }
      break;
    }

    /** Syntax for while-statement the same in Java and C++ **/

    case TAG_TYPE_CPP_WhileStmt: {
      const TYPE_CPP_Expr & expr_ (rc.GetRecord(pos_CPP_WhileStmt_expr));
      const TYPE_CPP_Stmt & stmt (rc.GetRecord(pos_CPP_WhileStmt_stmt));

      TYPE_CPP_Expr expr (expr_);
      while (expr.Is(TAG_TYPE_CPP_BracketedExpr)) {
        expr = expr.GetRecord(pos_CPP_BracketedExpr_expr);
      }
      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);

      m4code << "while (";
      GenCode(expr);
      m4code << ") ";

      SetContext(BLOCK);
      GenStmt(stmt);
      break;
    }

    /** Syntax for dowhile-statement the same in Java and C++ **/

    case TAG_TYPE_CPP_DoWhileStmt: {
      const TYPE_CPP_Stmt & stmt (rc.GetRecord(pos_CPP_DoWhileStmt_stmt));
      const TYPE_CPP_Expr & expr (rc.GetRecord(pos_CPP_DoWhileStmt_expr));

      IND(m4code);

      m4code << "do ";
      SetContext(BLOCK);
      GenStmt(stmt);

      GenNewLine(m4code);
      IND(m4code);

      m4code << "while (";
      GenCode(expr);
      m4code << ");" << flush;

      SetContext(CSTMT);
      break;
    }

    /** Syntax for for-statement the same in Java and C++ **/

    case TAG_TYPE_CPP_ForStmt: {
      const SEQ<TYPE_CPP_Stmt> & i (rc.GetSequence(pos_CPP_ForStmt_i));
      const Generic & expr1 (rc.GetField(pos_CPP_ForStmt_expr1));
      const SEQ<TYPE_CPP_Expr> & expr2 (rc.GetSequence(pos_CPP_ForStmt_expr2));
      const TYPE_CPP_Stmt & stmt (rc.GetRecord(pos_CPP_ForStmt_stmt));

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);
      m4code << "for (";
      SetContext(FOR);
      GenCode(i);
      m4code << " ";

      GenCode(expr1);
      m4code << "; ";

      GenCode(expr2);
      m4code << ") ";

      SetContext(BLOCK);
      GenStmt(stmt);
      break;
    }

    /** Syntax for foreach-statement in Java **/

    case TAG_TYPE_CPP_ForEachStmt: {
      const TYPE_CPP_DeclarationStmt & e (rc.GetRecord(pos_CPP_ForEachStmt_e));
      const TYPE_CPP_Expr & expr (rc.GetRecord(pos_CPP_ForEachStmt_expr));
      const TYPE_CPP_Stmt & stmt (rc.GetRecord(pos_CPP_ForEachStmt_stmt));

      const TYPE_CPP_IdentDeclaration & id (e.GetRecord(pos_CPP_DeclarationStmt_decl));
      const SEQ<TYPE_CPP_DeclSpecifier> & ds (id.GetSequence(pos_CPP_IdentDeclaration_ds));
      const SEQ<TYPE_CPP_InitDeclarator> & dl (id.GetSequence(pos_CPP_IdentDeclaration_dl));

      GenNewLine(m4code);

      IND(m4code);
      m4code << "for (";
      GenCodeSeq(ds, " ");
      m4code << " ";
      GenCode(dl[1]);
      m4code << ": ";
      GenCode(expr);
      m4code << ") ";

      SetContext(BLOCK);
      GenStmt(stmt);
      break;
    }

    /** break, continue and return the same in Java and C++ **/

    case TAG_TYPE_CPP_Break: {

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);
      m4code << "break;" << flush;
      SetContext(CSTMT);
      break;
    }

    case TAG_TYPE_CPP_Continue: {

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);
      m4code << "continue;" << flush;
      SetContext(CSTMT);
      break;
    }

    case TAG_TYPE_CPP_Return: {
      const Generic & expr (rc.GetField(pos_CPP_Return_expr));

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      else if (GetContext() == BE_DECL) {
        GenNewLine(m4code);
      }
      IND(m4code);
      m4code << "return";

      if (!expr.IsNil()) {
        m4code << " ";
        GenCode(expr);
      }

      m4code << ";" << flush;
      SetContext(CSTMT);
      break;
    }

    /** goto statement not supported in Java **/
    case TAG_TYPE_CPP_Goto: {
      if (isJAVA()) {
        m4code << "goto statement not supported!!!!!";
      }
      else { // C++
        TYPE_CPP_Identifier id (rc.GetRecord(pos_CPP_Goto_id));

        IND(m4code);
        m4code << "goto ";
        GenCode(id);
        m4code << ";";
        SetContext(CSTMT);
      }
      break;
    }

    case TAG_TYPE_CPP_DeclarationStmt: {
      const TYPE_CPP_Declaration & decl (rc.GetRecord(pos_CPP_DeclarationStmt_decl));

//m4code << "/* DS - last = " << last << " */";

      if (GetContext() != FOR) {

        if(isCPP()) {
          IND(m4code);
        }
        SetContext(CSTMT);
      }
      GenCode(decl);
      break;
    }

    case TAG_TYPE_CPP_IncludeStmt: {
      const TYPE_CPP_QuoteIncl & incl (rc.GetRecord(pos_CPP_IncludeStmt_incl));
      GenCode(incl);
      GenNewLine(m4code);
      SetContext(CSTMT);
      break;
    }

    case TAG_TYPE_CPP_PreIf: {

      if (GetContext() != MACRO)
        GenNewLine(m4code);
      if (isJAVA())
        m4code << "#if not supported by JAVA";
      else {
        m4code << "#if ";
        GenCode(rc.GetRecord(pos_CPP_PreIf_expr));
        GenNewLine(m4code);
      }
      SetContext(MACRO);
      break;
    }

    case TAG_TYPE_CPP_PreIfDef: {

      if (GetContext() != MACRO)
        GenNewLine(m4code);
      if (isJAVA())
        m4code << "#ifdef not supported by JAVA";
      else {
        m4code << "#ifdef ";
        GenCode(rc.GetRecord(pos_CPP_PreIfDef_id));
        GenNewLine(m4code);
      }
      SetContext(MACRO);
      break;
    }

    case TAG_TYPE_CPP_PreIfNotDef: {

      if (GetContext() != MACRO)
        GenNewLine(m4code);
      if (isJAVA())
        m4code << "#ifndef not supported by JAVA";
      else {
        m4code << "#ifndef ";
        GenCode(rc.GetRecord(pos_CPP_PreIfNotDef_id));
        GenNewLine(m4code);
      }
      SetContext(MACRO);
      break;
    }

    case TAG_TYPE_CPP_PreElse: {

      if (GetContext() != MACRO)
        GenNewLine(m4code);
      if (isJAVA())
        m4code << "#else not supported by JAVA";
      else {
        m4code << "#else";
        GenNewLine(m4code);
      }
      SetContext(MACRO);
      break;
    }

    case TAG_TYPE_CPP_PreEndIf: {

      if (GetContext() != MACRO)
        GenNewLine(m4code);
      if (isJAVA())
        m4code << "#endif not supported by JAVA";
      else
      {
        m4code << "#endif // ";
        GenCode(rc.GetRecord(pos_CPP_PreEndIf_id));
        GenNewLine(m4code);
        GenNewLine(m4code);
      }
      SetContext(MACRO);
      break;
    }

    case TAG_TYPE_CPP_PreDefine: {

      if (GetContext() != MACRO)
        GenNewLine(m4code);
      if (isJAVA())
        m4code << "#define not supported by JAVA";
      else {
        m4code << "#define ";
        GenCode(rc.GetRecord(pos_CPP_PreDefine_id));
        m4code << " ";
        GenCode(rc.GetRecord(pos_CPP_PreDefine_val));
        GenNewLine(m4code);
      }
      SetContext(MACRO);
      break;
    }

    case TAG_TYPE_CPP_PreMacro: {
      GenNewLine(m4code);
      GenCode(rc.GetRecord(pos_CPP_PreMacro_id));
      m4code << " ";
      SetContext(MACRO);
      break;
    }

    // Expressions

    /** Syntax of C++ and Java are the same for the following expressions **/

    case TAG_TYPE_CPP_AssignExpr:
    case TAG_TYPE_CPP_LogicalExpr:
    case TAG_TYPE_CPP_BitExpr:
    case TAG_TYPE_CPP_EqualityExpr:
    case TAG_TYPE_CPP_RelationalExpr:
    case TAG_TYPE_CPP_ShiftExpr:
    case TAG_TYPE_CPP_BinExpr:
    case TAG_TYPE_CPP_PmExpr:
    case TAG_TYPE_CPP_TypeCompExpr:{

      TYPE_CPP_Expr lhs, rhs;
      if (rc.Is(TAG_TYPE_CPP_TypeCompExpr)) {
        // Note that the rhs operand is the first field, and the lhs
        // the third field
        lhs = rc.GetField(3);
        rhs = rc.GetField(1);
      }
      else {
        lhs = rc.GetField(1);
        rhs = rc.GetField(3);
      }

      Quote op (Record(rc.GetField(2)).GetField(1));
      int prec = OpPrec(op);
      if (OpPrec(lhs) < prec ) {
        m4code << "(";
        GenCode(lhs);
        m4code << ") " <<   Quote2String(op);
      }
      else {
        GenCode(lhs);
        m4code << " " << Quote2String(op);
      }
      if (OpPrec(rhs) < prec ) {
        m4code << " (";
        GenCode(rhs);
        m4code << ")";
      }
      else {
        m4code << " ";
        GenCode(rhs);
      }
      break;
    }

    /** Syntax of C++ and Java are the same for conditional expressions **/

    case TAG_TYPE_CPP_ConditionalExpr: {
      const TYPE_CPP_Expr & cond (rc.GetRecord(pos_CPP_ConditionalExpr_cond));
      const TYPE_CPP_Expr & expr1 (rc.GetRecord(pos_CPP_ConditionalExpr_expr1));
      const TYPE_CPP_Expr & expr2 (rc.GetRecord(pos_CPP_ConditionalExpr_expr2));

      int prec = OpPrec(rc);
      if (OpPrec(cond) < prec) {
        m4code << "(";
        GenCode(cond);
        m4code << ") ? ";
      }
      else {
        GenCode(cond);
        m4code << " ? ";
      }
      if (OpPrec(expr1) < prec) {
        m4code << "(";
        GenCode(expr1);
        m4code << ") : ";
      }
      else {
        GenCode(expr1);
        m4code << " : ";
      }
      if (OpPrec(expr2) < prec) {
        m4code << "(";
        GenCode(expr2);
        m4code << ")";
      }
      else
        GenCode(expr2);
      break;
    }

    /** Syntax of C++ and Java are the same for cast expressions **/

    case TAG_TYPE_CPP_CastExpr: {
      const Record & rhstypenm (rc.GetRecord(pos_CPP_CastExpr_typenm));
      const TYPE_CPP_Expr & expr_ (rc.GetRecord(pos_CPP_CastExpr_expr));

      TYPE_CPP_Expr expr (expr_);
      while (expr.Is(TAG_TYPE_CPP_BracketedExpr))
        expr = expr.GetRecord(pos_CPP_BracketedExpr_expr);

// 20111208 -->
//      if ( OpPrec(expr) < OpPrec(rc) ) {
//        m4code << "(";
//        GenCode(rhstypenm);
//        m4code << ") (";
//        GenCode(expr);
//        m4code << ")";
//      }
//      else {
//        m4code << "(";
//        GenCode(rhstypenm);
//        m4code << ") ";
//        GenCode(expr);
//      }
      if (isJAVA())
      {
        m4code << "(";
        GenCode(rhstypenm);
        m4code << ")";
        if ( OpPrec(expr) < OpPrec(rc) ) {
          m4code << "(";
//          if (expr.Is(TAG_TYPE_CPP_BracketedExpr))
//            GenCode(expr.GetRecord(pos_CPP_BracketedExpr_expr));
//          else
            GenCode(expr);
          m4code << ")";
        }
        else {
          GenCode(expr);
        }
      }
      else
      {
        m4code << "static_cast<";
        GenCode(rhstypenm);
        m4code << ">(";
//        if (expr.Is(TAG_TYPE_CPP_BracketedExpr))
//          GenCode(expr.GetRecord(pos_CPP_BracketedExpr_expr));
//        else
          GenCode(expr);
        m4code << ")";
      }
// <-- 20111208
      break;
    }

    /** Syntax of C++ and Java are the same for Prefix and UnaryOp expressions **/

    case TAG_TYPE_CPP_PreInDecrementExpr:
    case TAG_TYPE_CPP_UnaryOpExpr: {
      Quote op (Record(rc.GetField(1)).GetField(1));
      const TYPE_CPP_Expr & arg (rc.GetField(2));

      int prec = OpPrec(op);
      if (OpPrec(arg) < prec ) {
        m4code << Quote2String(op) << "(";
        GenCode(arg);
        m4code << ")";
      }
      else {
        m4code << Quote2String(op);
        GenCode(arg);
      }
      break;
    }

    /** Syntax of C++ and Java are the same for Postfix expressions **/

    case TAG_TYPE_CPP_PostFixInDecrementExpr: {
      Quote op (rc.GetField(pos_CPP_PostFixInDecrementExpr_op));
      const TYPE_CPP_Expr & arg (rc.GetRecord(pos_CPP_PostFixInDecrementExpr_expr));

      int prec = OpPrec(op);
      if (OpPrec(arg) < prec ) {
        m4code << "(";
        GenCode(arg);
        m4code << ")" << Quote2String(op);
      }
      else {
        GenCode(arg);
        m4code << Quote2String(op);
      }
      break;
    }

    /** The sizeof-expression does not exist in Java **/
    case TAG_TYPE_CPP_SizeOfExpr:
    case TAG_TYPE_CPP_SizeOfType: {
      if (isJAVA())
        m4code << "The sizeof expression does not exist in Java!!!!!!!!!!!!!";
      else {
        const Generic & arg (rc.GetField(1));
        m4code << "sizeof(";
        GenCode(arg);
        m4code << ")";
      }
      break;
    }

    /* I'm not sure,if Java and C++ syntax are the same for the generated code */

    case TAG_TYPE_CPP_AllocationNewTypeExpr: {
      const Generic & sc (rc.GetField(pos_CPP_AllocationNewTypeExpr_scoperes));
      const SEQ<TYPE_CPP_Expr> & place (rc.GetField(pos_CPP_AllocationNewTypeExpr_placement));
      const TYPE_CPP_TypeName & nt (rc.GetRecord(pos_CPP_AllocationNewTypeExpr_typename));
      const Generic & ni (rc.GetField(pos_CPP_AllocationNewTypeExpr_newinit));

      if (!sc.IsNil()) {
        m4code << "::new ";
      }
      else {
        m4code << "new ";
      }
      if (!place.IsEmpty()) {
        m4code << "(";
        GenCodeSeq(place, ", ");
        m4code << ") ";
      }

      //if (rc.Is(TAG_TYPE_CPP_AllocationTypeExpr))
      //  GenCode(nt);
      //else {
        m4code << "(";
        GenCode(nt);
        m4code << ")";
      //}

      if (!ni.IsNil()) {
        GenCode(ni);
      }
      break;
    }

    case TAG_TYPE_CPP_AllocationTypeExpr: {
      const Generic & sc (rc.GetField(pos_CPP_AllocationTypeExpr_scoperes));
      const SEQ<TYPE_CPP_Expr> & place (rc.GetField(pos_CPP_AllocationTypeExpr_placement));
      const TYPE_CPP_NewTypeName & nt (rc.GetField(pos_CPP_AllocationTypeExpr_newtypename));
      const Generic & ni (rc.GetField(pos_CPP_AllocationTypeExpr_newinit));

      if (!sc.IsNil())
        m4code << "::new ";
      else
        m4code << "new ";

      if (!place.IsEmpty()) {
        m4code << "(";
        GenCodeSeq(place, ", ");
        m4code << ") ";
      }

      //if (rc.Is(TAG_TYPE_CPP_AllocationTypeExpr))
        GenCode(nt);
      //else {
      //  m4code << "(";
      //  GenCode(nt);
      //  m4code << ")";
      //}

      if (!ni.IsNil())
        GenCode(ni);
      break;
    }

    case TAG_TYPE_CPP_NewTypeName: {
      const SEQ<TYPE_CPP_TypeSpecifier> & tsl (rc.GetSequence(pos_CPP_NewTypeName_typespeclist));
      const Generic & nd (rc.GetField(pos_CPP_NewTypeName_newdecl));

      if (!nd.IsNil()) {
        GenCodeSeq(tsl, " ");
        m4code << " ";
        GenCode(nd);
      }
      else
        GenCodeSeq(tsl, " ");
      break;
    }

    case TAG_TYPE_CPP_ArrayCreationExpr: {
      const TYPE_CPP_SimpleTypeName & type (rc.GetRecord(pos_CPP_ArrayCreationExpr_type));
      const Generic & dimExpr (rc.GetField(pos_CPP_ArrayCreationExpr_dimExpr));
      const Generic & dim (rc.GetField(pos_CPP_ArrayCreationExpr_dim));
      const Generic & arrayInit (rc.GetField(pos_CPP_ArrayCreationExpr_arrayInit));

      m4code << "new ";
      GenCode(type);
      m4code << "[]";
      if (!arrayInit.IsNil()) {
        GenCode(arrayInit);
      }
      break;
    }

    case TAG_TYPE_CPP_ArrayInitializer: {
      // [seq of (CPP`Expr | CPP`ArrayInitializer)]
      const Generic & vi (rc.GetField(pos_CPP_ArrayInitializer_vi));
      if (!vi.IsNil()) {
        m4code << "{";
        GenCodeSeq(vi, ", ");
        m4code << "}";
      }
      break;
    }

    case TAG_TYPE_CPP_CVNewDeclarator: {
      const Generic & cv (rc.GetField(pos_CPP_CVNewDeclarator_cv_uqual_ulist)); // [CVQualifierList]
      const Generic & ndecl (rc.GetField(pos_CPP_CVNewDeclarator_newdecl));     // [NewDeclarator]

      m4code << "*";
      if (!cv.IsNil())
        GenCode(cv);

      if (!ndecl.IsNil()) {
        m4code << " ";
        GenCode(ndecl);
      }
      break;
    }

    case TAG_TYPE_CPP_ClassNewDecl: {
      const TYPE_CPP_CompleteClassName & cname (rc.GetRecord(pos_CPP_ClassNewDecl_class_uname));
      const Generic & cv (rc.GetField(pos_CPP_ClassNewDecl_cv_uqual_ulist));    // [CVQualifierList]
      const Generic & ndcl (rc.GetField(pos_CPP_ClassNewDecl_newdecl));     // [NewDeclarator]

      GenCode(cname);
      m4code << "::*";

      if (!cv.IsNil())
        GenCode(cv);

      if (!ndcl.IsNil()) {
        m4code << " ";
        GenCode(ndcl);
      }
      break;
    }

    case TAG_TYPE_CPP_ArrayNewDecl: {
      const Generic & ndcl (rc.GetField(pos_CPP_ArrayNewDecl_newdecl));    // [NewDeclarator]
      const Generic & expr (rc.GetField(pos_CPP_ArrayNewDecl_expr));       // [Expr]

      if (!ndcl.IsNil()) {
        GenCode(ndcl);
        m4code << " ";
      }
      m4code << "[";
      GenCode(expr);
      m4code << "]";
      break;
    }

    case TAG_TYPE_CPP_NewInitializer: {
      const Generic & ni (rc.GetField(pos_CPP_NewInitializer_initl)); // [InitializerList]

      m4code << "(";
      if (!ni.IsNil())
        GenCode(ni);
      m4code << ")";
      break;
    }

    /* Deallocation not part of Java */

    case TAG_TYPE_CPP_DeallocationArrayExpr:
    case TAG_TYPE_CPP_DeallocationExpr: {

      if (isJAVA())
        m4code << "deallocation not part of Java !!!!!!!!!!!!!!!!";
      else {
        const Generic & sc (rc.GetField(1));
        const TYPE_CPP_Expr & expr (rc.GetRecord(2));

        if (!sc.IsNil())
          m4code << "::";

        if (rc.Is(TAG_TYPE_CPP_DeallocationExpr))
          m4code << "delete ";
        else
          m4code << "delete[] ";

        if (OpPrec(expr) < OpPrec(rc)) {
          m4code << "(";
          GenCode(expr);
          m4code << ")";
        }
        else
          GenCode(expr);
      }
      break;
    }

    /** Syntax of C++ and Java are the same for array access**/

    case TAG_TYPE_CPP_ArrayApply: {
      const TYPE_CPP_Expr & array (rc.GetRecord(pos_CPP_ArrayApply_array));
      const TYPE_CPP_Expr & arg (rc.GetRecord(pos_CPP_ArrayApply_arg));

      if (OpPrec(array) < OpPrec(rc)) {
        m4code << "(";
        GenCode(array);
        m4code << ")[";
        GenCode(arg);
        m4code << "]";
      }
      else {
        GenCode(array);
        m4code << "[";
        GenCode(arg);
        m4code << "]";
      }
      break;
    }

    /** method call in Java and function call in C++ seems to be the same **/

    case TAG_TYPE_CPP_FctCall:
    case TAG_TYPE_CPP_ExplTypeConv: {
      const Generic & fct (rc.GetField(1));
      SEQ<TYPE_CPP_Expr> exprs (rc.GetSequence(2));

      if (OpPrec(fct) < OpPrec(rc)) {
        m4code << "(";
        GenCode(fct);
        m4code << ")";
      }
      else
        GenCode(fct);

//20131129 -->
      if (exprs.Length() == 1)
      {
        TYPE_CPP_Expr e (exprs[1]);
        while (e.Is(TAG_TYPE_CPP_BracketedExpr))
          e = e.GetRecord(pos_CPP_BracketedExpr_expr);
        exprs.ImpModify(1, e);
      }
// <-- 20131129

      m4code << "(";
      GenCodeSeq(exprs, ", ");
      m4code << ")";
      break;
    }

    /** The -> tag does not exist in Java, otherwise ObjectMember access is the same as in C++ **/

    case TAG_TYPE_CPP_ObjectMemberAccess:
    case TAG_TYPE_CPP_PointerToObjectMemberAccess:  {
      const TYPE_CPP_Expr & obj (rc.GetRecord(1));
      const TYPE_CPP_Expr & name (rc.GetRecord(2));

      if (OpPrec(obj) < OpPrec(rc)) {
        m4code << "(";
        GenCode(obj);
        m4code << ")";
      }
      else
        GenCode(obj);

      if (rc.Is(TAG_TYPE_CPP_ObjectMemberAccess) || isJAVA())
        m4code << ".";
      else
        m4code << "->";
      GenCode(name);
      break;
    }

    /** Syntax of C++ and Java is the same for literals**/

    case TAG_TYPE_CPP_IntegerLit: {
      m4code << TBWSTR::wstring2string(rc.GetInt(pos_CPP_IntegerLit_val).ascii());
      break;
    }

    case TAG_TYPE_CPP_FloatingLit: {
      m4code << TBWSTR::wstring2string(rc.GetReal(pos_CPP_FloatingLit_val).ascii());
      break;
    }

    case TAG_TYPE_CPP_BoolLit: {
      m4code << TBWSTR::wstring2string(rc.GetBool(pos_CPP_BoolLit_val).ascii());
      break;
    }

    case TAG_TYPE_CPP_CharacterLit:
    case TAG_TYPE_CPP_WCharacterLit: {
      const Char & ch (rc.GetChar(1));

      wstring wstr;
      wstr += ch.GetValue();

      wstring res;
      if (isJAVA())
        Backslashed::convert_internal_to_java_backslash (wstr, res);
      else
        Backslashed::convert_internal_to_cxx_backslash (wstr, res);

      if (rc.Is(TAG_TYPE_CPP_CharacterLit) || isJAVA())
        m4code << "\'" << TBWSTR::wstring2mbstr(res) << "\'";
      else
        m4code << "L\'" << TBWSTR::wstring2mbstr(res) << "\'";
      break;
    }

    case TAG_TYPE_CPP_StringLit:
    case TAG_TYPE_CPP_WStringLit: {
      wstring str (PTAUX::Seq2Str(rc.GetSequence(1)));

      wstring res;
      if (isJAVA())
        Backslashed::convert_internal_to_java_backslash (str, res);
      else
        Backslashed::convert_internal_to_cxx_backslash (str, res);

      if (rc.Is(TAG_TYPE_CPP_StringLit) || isJAVA()) // This is a hack - jcg
        // should never generate WStringLit
        m4code << "\"" + TBWSTR::wstring2mbstr(res) + "\"";
      else
        m4code << "L\"" + TBWSTR::wstring2mbstr(res) + "\"";
      break;
    }

    /** Syntax of C++ and Java is the same **/

    case TAG_TYPE_CPP_BracketedExpr: {
      const TYPE_CPP_Expr & expr (rc.GetRecord(pos_CPP_BracketedExpr_expr));

      m4code << "(";
//
      if (expr.Is(TAG_TYPE_CPP_BracketedExpr))
        GenCode(expr.GetRecord(pos_CPP_BracketedExpr_expr));
      else
        GenCode(expr);
//
      m4code << ")";
      break;
    }

    case TAG_TYPE_CPP_ThrowExpression: {
      const Generic & expr (rc.GetField(pos_CPP_ThrowExpression_expr));

      if (!expr.IsNil()) {
        m4code << "throw ";
        GenCode(expr);
      }
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_CPP_ClassInstanceCreationExpr: {
      const Record & cn (rc.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype));
      const Generic & somethingDotNew (rc.GetField(pos_CPP_ClassInstanceCreationExpr_somethingDotNew));
      const SEQ<TYPE_CPP_Expr> & es (rc.GetSequence(pos_CPP_ClassInstanceCreationExpr_arg));
      const Generic & ml (rc.GetField(pos_CPP_ClassInstanceCreationExpr_ml));

      if (!somethingDotNew.IsNil()) {
        GenCode(somethingDotNew);
        m4code << ".";
      }
      m4code << "new ";
      GenCode(cn);

      m4code << "(";
      if (!es.IsEmpty()) {
        GenCodeSeq(es, ", ");
      }
      m4code << ")";

      if (!(ml.IsNil())){
        m4code << "{ ";
        GenCode(ml);
        GenNewLine(m4code);
        m4code << "}";
      }
      break;
    }
#endif // VDMPP

    /** Templates do not exist in Java **/

    case TAG_TYPE_CPP_TemplateClassName: {
      if (isJAVA())
        m4code << "Templates not defined in Java!!!!!!!!!!!!!!!";
      else {
        const TYPE_CPP_TemplateName & nm (rc.GetRecord(pos_CPP_TemplateClassName_nm));
        const SEQ<TYPE_CPP_TemplateArg> & arg (rc.GetSequence(pos_CPP_TemplateClassName_arg));

        GenCode(nm);
        m4code << "< ";
        GenCodeSeq(arg, ",");
        m4code << " >";
      }
      break;
    }

    /** Syntax of C++ and Java is the same for names and identifiers **/
#ifdef VDMPP
    case TAG_TYPE_CPP_SimplePackageName: {
      GenCode(rc.GetRecord(pos_CPP_SimplePackageName_id));
      break;
    }
#endif // VDMPP

    case TAG_TYPE_CPP_EnumName: {
      GenCode(rc.GetRecord(pos_CPP_EnumName_id));
      break;
    }

    case TAG_TYPE_CPP_TemplateName: {
      GenCode(rc.GetRecord(pos_CPP_TemplateName_id));
      break;
    }

    case TAG_TYPE_CPP_Identifier: {
      wstring id (PTAUX::Seq2Str(rc.GetSequence(pos_CPP_Identifier_id)));
      m4code << TBWSTR::wstring2mbstr(id);
      break;
    }

    /** Syntax of C++ and Java is the same for "this" **/

    case TAG_TYPE_CPP_This: {
      m4code << "this";
      break;
    }

    /** Scope syntax for Java is slightly different **/

//    case TAG_TYPE_CPP_ScopeResIdentifier: {
//      const TYPE_CPP_Name & id (rc.GetRecord(pos_CPP_ScopeResIdentifier_id));
//      m4code << (isJAVA() ? "." : "::"); 
//      GenCode(id);
//      break;
//    }
//    case TAG_TYPE_CPP_ScopeResOpFctName: {
//      const TYPE_CPP_Name & opfctname (rc.GetRecord(pos_CPP_ScopeResOpFctName_opfctname));
//      m4code << (isJAVA() ? "." : "::"); 
//      GenCode(opfctname);
//      break;
//    }
//    case TAG_TYPE_CPP_ScopeResQualifiedName: {
//      const TYPE_CPP_Name & qualname (rc.GetRecord(pos_CPP_ScopeResQualifiedName_qualname));
//      m4code << (isJAVA() ? "." : "::"); 
//      GenCode(qualname);
//      break;
//    }

//    case TAG_TYPE_CPP_OperatorFunctionName: {
//      const TYPE_CPP_Operator & opr (rc.GetRecord(pos_CPP_OperatorFunctionName_op));
//      Quote op (opr.GetField(pos_CPP_Operator_op));
//
//      m4code << "operator " << Quote2String(op);
//      break;
//    }

#ifdef VDMPP
    case TAG_TYPE_CPP_QualifiedPackageName: {
      const TYPE_CPP_Expr & pn (rc.GetRecord(pos_CPP_QualifiedPackageName_pn));
      const TYPE_CPP_Name & id (rc.GetRecord(pos_CPP_QualifiedPackageName_id));

      GenCode(pn);
      m4code << (isJAVA() ? "." : "::"); 
      GenCode(id);
      break;
    }
#endif // VDMPP

    case TAG_TYPE_CPP_QualifiedName: {
      const TYPE_CPP_Expr & qcn (rc.GetRecord(pos_CPP_QualifiedName_qcn));
      const TYPE_CPP_Name & id (rc.GetRecord(pos_CPP_QualifiedName_id));

      GenCode(qcn);
      m4code << (isJAVA() ? "." : "::"); 
      GenCode(id);
      break;
    }

//    case TAG_TYPE_CPP_ConversionFunctionName: {
//      const TYPE_CPP_ConversionTypeName & ctn (rc.GetRecord(pos_CPP_ConversionFunctionName_ctn));
//
//      m4code <<  "operator ";
//      GenCode(ctn);
//      break;
//    }

//    case TAG_TYPE_CPP_ConversionTypeName: {
//      const SEQ<TYPE_CPP_TypeSpecifier> & ts (rc.GetSequence(pos_CPP_ConversionTypeName_ts));
//      const Generic & ptr (rc.GetField(pos_CPP_ConversionTypeName_ptr));
//
//      size_t len_ts = ts.Length();
//      for (size_t idx = 1; idx <= len_ts; idx++) {
//        GenCode(ts[idx]);
//      }
//      GenCode(ptr);
//      break;
//    }

    /** Pointers do not exist for Java **/

//    case TAG_TYPE_CPP_PointerDecl: {
//      const Generic & cvl (rc.GetField(pos_CPP_PointerDecl_cvl)); // [CVQualifierList]
//      m4code << (isJAVA() ? "pointer not defined in Java" : "*");
//      GenCode(cvl);
//      break;
//    }

//    case TAG_TYPE_CPP_RefTypeDecl: {
//      const Generic & cvl (rc.GetField(pos_CPP_RefTypeDecl_cvl)); // [CVQualifierList]
//      m4code << (isJAVA() ? "pointer not defined in Java" : "&");
//      GenCode(cvl);
//      break;
//    }

//    case TAG_TYPE_CPP_PointerToMemberDecl: {
//      const TYPE_CPP_CompleteClassName & ccn (rc.GetRecord(pos_CPP_PointerToMemberDecl_ccn));
//      const Generic & cvl (rc.GetField(pos_CPP_PointerToMemberDecl_cvl));  // [CVQualifierList]
//
//      GenCode(ccn);
//      m4code << (isJAVA() ? "pointer not defined in Java" : "::*");
//      GenCode(cvl);
//      break;
//    }

    case TAG_TYPE_CPP_Destructor: {
      const TYPE_CPP_Identifier & id (rc.GetRecord(pos_CPP_Destructor_id));

      m4code << "~";
      GenCode(id);
      break;
    }

    // Exception handling

    case TAG_TYPE_CPP_TryBlock: {
      const TYPE_CPP_CompoundStmt & id (rc.GetRecord(pos_CPP_TryBlock_stmt));
      const SEQ<TYPE_CPP_Handler> & handlers (rc.GetSequence(pos_CPP_TryBlock_hdl));
      const Generic & fin (rc.GetField(pos_CPP_TryBlock_fin));

      if (isJAVA()) {
        GenNewLine(m4code);
      }
      IND(m4code);
      m4code << "try ";

      SetContext(BLOCK);

      GenCode(id);
      GenNewLine(m4code);
      if (!handlers.IsEmpty()) {
        GenCodeSeq(handlers, "\n");
      }
      if (!fin.IsNil()) {
        if (!handlers.IsEmpty()) {
          GenNewLine(m4code);
        }
        if (isJAVA ()) {
          IND(m4code);
          m4code << "finally ";
          GenCode(fin);
        }
        else {
          SEQ<TYPE_CPP_Stmt> stmts (Record(fin).GetSequence(pos_CPP_CompoundStmt_stms));
          if (stmts.Length() == 1) {
            GenCode(stmts[1]);
          }
          else {
            GenCodeSeq(stmts, "\n");
          }
        }
      }
      break;
    }

    case TAG_TYPE_CPP_Handler: {
      const Record & decl (rc.GetRecord(pos_CPP_Handler_exepdecl));
      const TYPE_CPP_Stmt & stmt (rc.GetRecord(pos_CPP_Handler_stmt));

      IND(m4code);
      m4code << "catch ";

      if (decl.Is(TAG_TYPE_CPP_All)) {
        m4code << "(...)";
      }
      else {
        GenCode(decl);
      }
      m4code << " ";

      SetContext(BLOCK);
      if (!stmt.Is(TAG_TYPE_CPP_CompoundStmt)) {
        OpenBlock();

        GenCode(stmt);

        GenNewLine(m4code);
        CloseBlock();
      }
      else {
        GenCode(stmt);
      }
      break;
    }

    case TAG_TYPE_CPP_ExceptionDeclaration: {
      const SEQ<TYPE_CPP_TypeSpecifier> & ts (rc.GetSequence(pos_CPP_ExceptionDeclaration_tsl));
      const Generic & decl (rc.GetField(pos_CPP_ExceptionDeclaration_decl));

      m4code << "(";
      if (!ts.IsEmpty())
        GenCodeSeq(ts, " ");

      m4code << " ";
      GenCode(decl);
      m4code << ")";
      break;
    }

    case TAG_TYPE_CPP_Modifier: {
      m4code << Quote2String(rc.GetField(pos_CPP_Modifier_ma));
      break;
    }

    case TAG_TYPE_CPP_StorageClassSpecifier: {
      m4code << Quote2String(rc.GetField(pos_CPP_StorageClassSpecifier_val));
      break;
    }

    case TAG_TYPE_CPP_FctSpecifier: {
      m4code << Quote2String(rc.GetField(pos_CPP_FctSpecifier_val));
      break;
    }

    case TAG_TYPE_CPP_BlockComments:
    case TAG_TYPE_CPP_DocComments: {
      break;
    }

    case TAG_TYPE_CPP_SingleLineComments: {
      const SEQ<Char> & txt (rc.GetSequence(pos_CPP_SingleLineComments_txt));
      if (isJAVA())
        GenNewLine(m4code);
      IND(m4code);
      m4code << "/* " << TBWSTR::wstring2mbstr(txt.GetString()) << " */";
      break;
    }
    case TAG_TYPE_CPP_TrailingComments: {
      const SEQ<Char> & txt (rc.GetSequence(pos_CPP_TrailingComments_txt));
      m4code << " /* " << TBWSTR::wstring2mbstr(txt.GetString()) << " */";
      GenNewLine(m4code);
      IND(m4code);
      break;
    }
    case TAG_TYPE_CPP_EndOfLineComments: {
      const SEQ<Char> & txt (rc.GetSequence(pos_CPP_EndOfLineComments_txt));
      m4code << " // " << TBWSTR::wstring2mbstr(txt.GetString());
      GenNewLine(m4code);
      IND(m4code);
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_CPP_GenericsSpec: {
      const SEQ<TYPE_CPP_GenericsClasses> & defs (rc.GetSequence(pos_CPP_GenericsSpec_defs));
      m4code << "<";
      if (!defs.IsEmpty())
      {
        GenCodeSeq(defs, ", ");
      }
      m4code << ">";
      break;
    }

    case TAG_TYPE_CPP_GenericsClasses: {
      const TYPE_CPP_PackageName & id (rc.GetRecord(pos_CPP_GenericsClasses_id));
      const SEQ<TYPE_CPP_PackageName> & extends (rc.GetSequence(pos_CPP_GenericsClasses_exteds));
      GenCode(id);
      if (!extends.IsEmpty())
      {
        m4code << " extends ";
        GenCodeSeq(extends, " & ");
      }
      break;
    }
#endif // VDMPP

    default:
      OutputErr(L"Internal error in the code generator backend (2).");
      OutputErr(L"Please report this error.");
      OutputErr(rc.ascii());
      GenNewLine(m4code);
      m4code << "// This part is not implemented (internal error).";
      GenNewLine(m4code);
      break;
    }
  }
  else if (ast.IsSequence()) {
    Sequence seq (ast);
    Generic g;
    for (bool bb = seq.First(g); bb; bb = seq.Next(g))
      GenCode(g);
  }

  else if (ast.IsSet()) {
    Set set(ast);
    Generic g;
    for (bool bb = set.First(g); bb; bb = set.Next(g))
      GenCode(g);
  }

  else if (ast.IsQuote())
    m4code << Quote2String((Quote) ast);

  else {
    OutputErr(L"Internal error in the code generator backend (3).");
    OutputErr(L"Please report this error.");
    OutputErr(ast.ascii());
    m4code << endl << "// This part is not implemented (internal error)." << endl;
  }
}

void CGBackEnd::GenClassSpecifier(const TYPE_CPP_ClassSpecifier & rc)
{
  const TYPE_CPP_ClassHead & ch (rc.GetRecord(pos_CPP_ClassSpecifier_ch));
  const Generic & ml (rc.GetField(pos_CPP_ClassSpecifier_ml));

  GenNewLine(m4code);
  GenNewLine(m4code);

  string existingCode;
  if ((existingCode = CheckExisting(GetName(ch))) != "")
  {
    m4code << StartKeepTag(ch, false, true);
    m4code << existingCode;
  }
  else
  {
    m4code << StartKeepTag(ch, false, false);
    //level++;
    IncLevel();
    IND(m4code);
    GenCode(ch);
    SetContext(CLASS);

    m4code << " ";

    OpenBlock();

    if ((GetLevel() == 1) && (isJAVA()))
    {
      Generic g;
      for (bool bb = existingEntitiesOrder.First(g); bb; bb = existingEntitiesOrder.Next(g)) {
        if (!generatedEntities.InSet(g)) {
          Sequence keepTag (g);
          GenNewLine(m4code);
          m4code << StartKeepTag(g, true, true);
          wstring code;
          TYPE_CGTYPES_KeepCode kc (existingEntitiesMap[g]);
          Sequence seq (kc.get_code());
          seq.GetString(code);
          m4code << TBWSTR::wstring2mbstr(code);
          m4code << EndKeepTag(g, true) << "";
        }
      }
    }
    GenCode(ml);

    GenNewLine(m4code);
    CloseBlock();

// 20160609 -->
    if (isCPP())
      m4code << ";";
// <-- 20160609

    GenNewLine(m4code);
    //level--;
    DecLevel();
  }
  if (isJAVA())
    SetContext(CLASS);

  m4code << EndKeepTag(ch, false);
}

void CGBackEnd::GenClassHead(const TYPE_CPP_ClassHead & rc)
{
  const SEQ<TYPE_CPP_Modifier> & ms (rc.GetSequence(pos_CPP_ClassHead_m));
  Quote ck (rc.GetField(pos_CPP_ClassHead_ck));
  const Generic & name (rc.GetField(pos_CPP_ClassHead_name));
  const SEQ<TYPE_CPP_BaseSpecifier> & bs (rc.GetSequence(pos_CPP_ClassHead_bs));
  const Generic & ispec (rc.GetField(pos_CPP_ClassHead_ispec));
  const Generic & tpspec (rc.GetField(pos_CPP_ClassHead_tpspec));

  if(!ms.IsEmpty())
  {
    GenCodeSeq(ms, " ");
    m4code << " ";
  }

  m4code << Quote2String(ck) << " ";

  GenCode(name);

  if (!tpspec.IsNil())  // Java only
  {
    SEQ<TYPE_CPP_Identifier> id_l (tpspec);
    if (!id_l.IsEmpty())
    {
      m4code << "<";
      GenCodeSeq(id_l, ", ");
      m4code << ">";
    }
  }

  if(!bs.IsEmpty())
  {
    m4code << (isJAVA() ? " extends " : " : ");

    GenCodeSeq(bs, ", ");
  }

  if (!ispec.IsNil()) {
    SEQ<TYPE_CPP_PackageName> ispec_s (ispec);
    if (!ispec_s.IsEmpty()) {
      m4code << " implements ";
      GenCodeSeq(ispec_s, ", ");
    }
  }
}

void CGBackEnd::GenMemberList(const TYPE_CPP_MemberList & rc)
{
  const Generic & a (rc.GetField(pos_CPP_MemberList_a));
  const Generic & md (rc.GetField(pos_CPP_MemberList_md));

  GenNewLine(m4code);

  if (a.IsQuote())
    m4code << Quote2String(Quote(a)) << ":";

  if (md.IsSequence())
  {
    SEQ<TYPE_CPP_MemberDeclaration> s (md);
    size_t len_s = s.Length();
    for (size_t idx = 1; idx<= len_s; idx++)
    {
      const TYPE_CPP_MemberDeclaration & md (s[idx]);
      switch(md.GetTag()) {
        case TAG_TYPE_CPP_FunctionDefinition:
        case TAG_TYPE_CPP_IdentDeclaration: {
          GenCode(md);
          break;
        }
        default: {
          GenNewLine(m4code);
          GenCode(md);
          m4code << ";";
          break;
        }
      }
    }
  }
}

void CGBackEnd::GenMemberSpecifier(const TYPE_CPP_MemberSpecifier & rc)
{
  const SEQ<TYPE_CPP_DeclSpecifier> & ds (rc.GetSequence(pos_CPP_MemberSpecifier_ds));
  const Generic & mdl (rc.GetField(pos_CPP_MemberSpecifier_mdl));

  IND(m4code);
  GenCodeSeq(ds, " ");

  switch (ds.Length()) {
    case 0: { break; }
    case 1: {
      if (ds[1].Is(TAG_TYPE_CPP_TypeSpecifier)) {
        Generic tp (ds[1].GetField(pos_CPP_TypeSpecifier_tp));
        if (tp.Is(TAG_TYPE_CPP_EnumSpecifier)){
          ;
        }
        else {
          m4code << " ";
        }
      }
      else {
        m4code << " ";
      }
      break;
    }
    default: {
      m4code << " ";
      break;
    }
  }

  if (!mdl.IsNil())
    GenCode(mdl);
}

#ifdef VDMPP
void CGBackEnd::GenInterfaceSpecifier(const TYPE_CPP_InterfaceSpecifier & rc)
{
  const TYPE_CPP_InterfaceHead & ih (rc.GetRecord(pos_CPP_InterfaceSpecifier_ih));
  const SEQ<TYPE_CPP_MemberDeclaration> & ml (rc.GetSequence(pos_CPP_InterfaceSpecifier_ml));

  m4code << StartKeepTag(ih, false, false);

  IncLevel();
  IND(m4code);

  GenCode(ih);

  SetContext(CLASS);

  m4code << " ";

  OpenBlock();

  GenCode(ml);

  GenNewLine(m4code);

  CloseBlock();

  GenNewLine(m4code);

  DecLevel();

  m4code << EndKeepTag(ih, false);
}

void CGBackEnd::GenInterfaceHead(const TYPE_CPP_InterfaceHead & rc)
{
  const SEQ<TYPE_CPP_Modifier> & ms (rc.GetSequence(pos_CPP_InterfaceHead_m));
  const TYPE_CPP_Identifier & name (rc.GetRecord(pos_CPP_InterfaceHead_name));
  const SEQ<TYPE_CPP_PackageName> & ispec (rc.GetSequence(pos_CPP_InterfaceHead_bs));

  if (!ms.IsEmpty())
  {
    GenCodeSeq(ms, " ");
    m4code << " ";
  }

  m4code << "interface ";
  GenCode(name);

  if (!ispec.IsEmpty()) {
    m4code << " extends ";
    GenCodeSeq(ispec, ", ");
  }
}
#endif // VDMPP

void CGBackEnd::GenIfStmt(const TYPE_CPP_IfStmt & rc)
{
  TYPE_CPP_Expr expr (rc.GetRecord(pos_CPP_IfStmt_expr));
  const TYPE_CPP_Stmt & alt1 (rc.GetRecord(pos_CPP_IfStmt_alt1));
  const Generic & alt2 (rc.GetRecord(pos_CPP_IfStmt_alt2));

  if (GetContext() == ELSE) {
    SetContext(BLOCK);
  }
  else {
    if (isJAVA()) {
      GenNewLine(m4code);
    }
    IND(m4code);
    SetContext(BLOCK);
  }

  while (expr.Is(TAG_TYPE_CPP_BracketedExpr)) {
    expr = expr.GetRecord(pos_CPP_BracketedExpr_expr);
  }
  m4code << "if (";
  if (expr.Is(TAG_TYPE_CPP_AssignExpr) && isCPP()) {
    m4code << "(";
    GenCode(expr);
    m4code << ")";
  }
  else {
    GenCode(expr);
  }
  m4code << ") " << flush;

  SetContext(BLOCK);
  if (alt1.Is(TAG_TYPE_CPP_IfStmt) && alt1.GetRecord(pos_CPP_IfStmt_alt2).IsNil()) {
    TYPE_CPP_CompoundStmt cs;
    cs.Init(SEQ<TYPE_CPP_Stmt>().ImpAppend(alt1), -1);
    GenStmt(cs);
  }
  else {
    GenStmt(alt1);
  }
  m4code << flush;

  if (!alt2.IsNil()) {
    GenNewLine(m4code);
    IND(m4code);

    m4code << "else ";

    if (alt2.Is(TAG_TYPE_CPP_IfStmt)) {
      SetContext(ELSE);
      if (Record(alt2).GetRecord(pos_CPP_IfStmt_alt2).IsNil()) {
        TYPE_CPP_CompoundStmt cs;
        cs.Init(SEQ<TYPE_CPP_Stmt>().ImpAppend(alt2), -1);
        SetContext(BLOCK);
        OpenScope();
        GenStmt(cs, true);
        CloseScope();
      }
      else {
        GenStmt(alt2, true);
      }
    }
    else {
      SetContext(BLOCK);
      GenStmt(alt2);
    }
    m4code << flush;
  }
  SetContext(CSTMT);
}

void CGBackEnd::GenExpressionStmt(const TYPE_CPP_ExpressionStmt & rc)
{
  const Generic & expr (rc.GetField(pos_CPP_ExpressionStmt_expr));

  if (GetContext() == FOR)
  {
    SetContext(CSTMT);
    if (!expr.IsNil())
      GenCode(expr);

    m4code << ";";
  }
  else if(!expr.IsNil())
  {
    if (isJAVA())
      GenNewLine(m4code);

    SetContext(CSTMT);
    IND(m4code);
    GenCode(expr);
    m4code << ";";
  }
  else
  {
// 20110908 -->
    if (isJAVA())
      GenNewLine(m4code);

    SetContext(CSTMT);
    IND(m4code);
// <-- 20110908
    m4code << ";";
  }
}

void CGBackEnd::GenCompoundStmt(const TYPE_CPP_CompoundStmt & rc)
{
  SEQ<TYPE_CPP_Stmt> stmts (rc.GetSequence(pos_CPP_CompoundStmt_stms));

  switch(stmts.Length()) {
    case 0: {
      m4code << "{}";
      break;
    }
    case 1: {
      const TYPE_CPP_Stmt & st (stmts[1]);
      switch(st.GetTag()) {
        case TAG_TYPE_CPP_CompoundStmt: {
          GenCode(st);
          break;
        }
        case TAG_TYPE_CPP_SwitchStmt: {
          OpenBlock();

          GenNewLine(m4code);

          GenCode(st);

          GenNewLine(m4code);
          CloseBlock();

          break;
        }
        default: { //
// 20110908 -->
          if (isCPP())
            if (last != BLOCK)
              IND(m4code);
// <-- 20110908

          OpenBlock();

// 20070918
// hack for accessor
//              if(isCPP())
          if(isCPP() && (!st.Is(TAG_TYPE_CPP_Return) || (this->indent > 2)))
            GenNewLine(m4code);

          GenCode(st);

          GenNewLine(m4code);
          CloseBlock();
          break;
        }
      }
      break;
    }
    default: { // stmts.Length() > 1
      if (isCPP())
        if (GetContext() != BLOCK)
          IND(m4code);

      OpenBlock();

// 20120321 -->
      if (isCPP())
// <-- 20120321
        GenNewLine(m4code);

      SetContext(CSTMT);

      const TYPE_CPP_Stmt & st (stmts[1]);

      if (isCPP()) {
        if (st.Is(TAG_TYPE_CPP_CompoundStmt)) {
          TYPE_CPP_CompoundStmt ics (st);
          GenCodeSeq(ics.get_stms(), "\n");
          stmts.ImpTl();
          GenNewLine(m4code);
          IND(m4code);
        }
      }
      size_t len_stmts = stmts.Length();
      for (size_t idx = 1; idx <= len_stmts; idx++) {
        const TYPE_CPP_Stmt & stmt (stmts[idx]);
        if (isJAVA()) {
//          if (stmt.IsRecord())
          if (stmt.Is(TAG_TYPE_CPP_CompoundStmt)) {
            GenNewLine(m4code);
            IND(m4code);
          }
        }
        GenCode(stmt);
        //if (isCPP())
        if (isCPP())
          GenNewLine(m4code);
      }
//      if( stmts.IsEmpty() ) GenNewLine();
      if ( isJAVA() || stmts.IsEmpty() )
        GenNewLine(m4code);

      CloseBlock();
      break;
    }
  }
}

int CGBackEnd::Quote2Int(const Quote & op)
{
  wstring q (op.GetValue());
  int res = 0;
  if (q == L"ASEQUAL")
    res = TAG_quote_ASEQUAL;
  else if (q == L"ASMULT")
    res = TAG_quote_ASMULT;
  else if (q == L"ASDIV")
    res = TAG_quote_ASDIV;
  else if (q == L"ASMOD")
    res = TAG_quote_ASMOD;
  else if (q == L"ASPLUS")
    res = TAG_quote_ASPLUS;
  else if (q == L"ASMINUS")
    res = TAG_quote_ASMINUS;
  else if (q == L"ASRIGHTSHIFT")
    res = TAG_quote_ASRIGHTSHIFT;
  else if (q == L"ASLEFTSHIFT")
    res = TAG_quote_ASLEFTSHIFT;
  else if (q == L"ASBITWISEAND")
    res = TAG_quote_ASBITWISEAND;
  else if (q == L"ASBITWISEEXOR")
    res = TAG_quote_ASBITWISEEXOR;
  else if (q == L"ASBITWISEINCLOR")
    res = TAG_quote_ASBITWISEINCLOR;
  else if (q == L"OR")
    res = TAG_quote_OR;
  else if (q == L"AND")
    res = TAG_quote_AND;
  else if (q == L"EXCLOR")
    res = TAG_quote_EXCLOR;
  else if (q == L"REFERENCE")
    res = TAG_quote_REFERENCE;
  else if (q == L"BITAND")
    res = TAG_quote_BITAND;
  else if (q == L"BITOR")
    res = TAG_quote_BITOR;
  else if (q == L"EQ")
    res = TAG_quote_EQ;
  else if (q == L"NEQ")
    res = TAG_quote_NEQ;
  else if (q == L"LT")
    res = TAG_quote_LT;
  else if (q == L"GT")
    res = TAG_quote_GT;
  else if (q == L"LEQ")
    res = TAG_quote_LEQ;
  else if (q == L"GEQ")
    res = TAG_quote_GEQ;
  else if (q == L"LEFTSHIFT")
    res = TAG_quote_LEFTSHIFT;
  else if (q == L"RIGHTSHIFT")
    res = TAG_quote_RIGHTSHIFT;
  else if (q == L"PLUS")
    res = TAG_quote_PLUS;
  else if (q == L"MINUS")
    res = TAG_quote_MINUS;
  else if (q == L"MULT")
    res = TAG_quote_MULT;
  else if (q == L"DIV")
    res = TAG_quote_DIV;
  else if (q == L"MOD")
    res = TAG_quote_MOD;
  else if (q == L"DOTSTAR")
    res = TAG_quote_DOTSTAR;
  else if (q == L"ARROWSTAR")
    res = TAG_quote_ARROWSTAR;
  else if (q == L"PPLUS")
    res = TAG_quote_PPLUS;
  else if (q == L"PMINUS")
    res = TAG_quote_PMINUS;
  else if (q == L"ISINSTANCEOF")
    res = TAG_quote_ISINSTANCEOF;
  else if (q == L"INDIRECTION")
    res = TAG_quote_INDIRECTION;
  else if (q == L"ADDROP")
    res = TAG_quote_ADDROP;
  else if (q == L"NEG")
    res = TAG_quote_NEG;
  else if (q == L"COMPL")
    res = TAG_quote_COMPL;
//  else if (q == L"OPEQUAL")
//    res = TAG_quote_OPEQUAL;
//  else if (q == L"STAREQUAL")
//    res = TAG_quote_STAREQUAL;
//  else if (q == L"BACKSLASHEQUAL")
//    res = TAG_quote_BACKSLASHEQUAL;
//  else if (q == L"PERCENTAGEMARKEQUAL")
//    res = TAG_quote_PERCENTAGEMARKEQUAL;
//  else if (q == L"PLUSEQUAL")
//    res = TAG_quote_PLUSEQUAL;
//  else if (q == L"MINUSEQUAL")
//    res = TAG_quote_MINUSEQUAL;
//  else if (q == L"DOUBLEGREATEREQUAL")
//    res = TAG_quote_DOUBLEGREATEREQUAL;
//  else if (q == L"DOUBLELESSEQUAL")
//    res = TAG_quote_DOUBLELESSEQUAL;
//  else if (q == L"ANDEQUAL")
//    res = TAG_quote_ANDEQUAL;
//  else if (q == L"CIRCUMFLEXEQUAL")
//    res = TAG_quote_CIRCUMFLEXEQUAL;
//  else if (q == L"VERTICALLINEEQUAL")
//    res = TAG_quote_VERTICALLINEEQUAL;
//  else if (q == L"DOUBLEAND")
//    res = TAG_quote_DOUBLEAND;
//  else if (q == L"DOUBLEVERTICALLINE")
//    res = TAG_quote_DOUBLEVERTICALLINE;
//  else if (q == L"CIRCUMFEX")
//    res = TAG_quote_CIRCUMFEX;
//  else if (q == L"AMPERSAND")
//    res = TAG_quote_AMPERSAND;
//  else if (q == L"VERTICALLINE")
//    res = TAG_quote_VERTICALLINE;
//  else if (q == L"DOUBLEEQUAL")
//    res = TAG_quote_DOUBLEEQUAL;
//  else if (q == L"EXCLAMATIONMARKEQUAL")
//    res = TAG_quote_EXCLAMATIONMARKEQUAL;
//  else if (q == L"LESS")
//    res = TAG_quote_LESS;
//  else if (q == L"GREATER")
//    res = TAG_quote_GREATER;
//  else if (q == L"LESSEQUAL")
//    res = TAG_quote_LESSEQUAL;
//  else if (q == L"GREATEREQUAL")
//    res = TAG_quote_GREATEREQUAL;
//  else if (q == L"DOUBLELESS")
//    res = TAG_quote_DOUBLELESS;
//  else if (q == L"DOUBLEGREATER")
//    res = TAG_quote_DOUBLEGREATER;
//  else if (q == L"STAR")
//    res = TAG_quote_STAR;
//  else if (q == L"PRECENTAGEMARK")
//    res = TAG_quote_PRECENTAGEMARK;
//  else if (q == L"DOUBLEPLUS")
//    res = TAG_quote_DOUBLEPLUS;
//  else if (q == L"DOUBLEMINUS")
//    res = TAG_quote_DOUBLEMINUS;
//  else if (q == L"EXCLAMATIONMARK")
//    res = TAG_quote_EXCLAMATIONMARK;
//  else if (q == L"TILDE")
//    res = TAG_quote_TILDE;
//  else if (q == L"NEW")
//    res = TAG_quote_NEW;
//  else if (q == L"DELETE")
//    res = TAG_quote_DELETE;
//  else if (q == L"COMMA")
//    res = TAG_quote_COMMA;
//  else if (q == L"ARROW")
//    res = TAG_quote_ARROW;
//  else if (q == L"BRACKETS")
//    res = TAG_quote_BRACKETS;
//  else if (q == L"SQUAREBRACKETS")
//    res = TAG_quote_SQUAREBRACKETS;
  else if (q == L"CONST")
    res = TAG_quote_CONST;
  else if (q == L"VOLATILE")
    res = TAG_quote_VOLATILE;
  else if (q == L"ELLIPSIS")
    res = TAG_quote_ELLIPSIS;
  else if (q == L"CLASS")
    res = TAG_quote_CLASS;
  else if (q == L"STRUCT")
    res = TAG_quote_STRUCT;
  else if (q == L"UNION")
    res = TAG_quote_UNION;
  else if (q == L"PRIVATE")
    res = TAG_quote_PRIVATE;
  else if (q == L"PROTECTED")
    res = TAG_quote_PROTECTED;
  else if (q == L"ABSTRACT")
    res = TAG_quote_ABSTRACT;
  else if (q == L"PUBLIC")
    res = TAG_quote_PUBLIC;
  else if (q == L"FINAL")
    res = TAG_quote_FINAL;
  else if (q == L"INLINE")
    res = TAG_quote_INLINE;
  else if (q == L"VIRTUAL")
    res = TAG_quote_VIRTUAL;
  else if (q == L"AUTO")
    res = TAG_quote_AUTO;
  else if (q == L"REGISTER")
    res = TAG_quote_REGISTER;
  else if (q == L"STATIC")
    res = TAG_quote_STATIC;
  else if (q == L"EXTERN")
    res = TAG_quote_EXTERN;

  return res;
}

string CGBackEnd::Quote2String(const Quote & op)
{
  int q = Quote2Int(op);
  string res;
  switch(q) {

    // AssignOp -- the same for Java and C++
  case TAG_quote_ASEQUAL:
//  case TAG_quote_OPEQUAL:
    res = "=";
    break;

  case TAG_quote_ASMULT:
//  case TAG_quote_STAREQUAL:
    res = "*=";
    break;

  case TAG_quote_ASDIV:
//  case TAG_quote_BACKSLASHEQUAL:
    res = "/=";
    break;

  case TAG_quote_ASMOD:
//  case TAG_quote_PERCENTAGEMARKEQUAL:
    res = "%=";
    break;

  case TAG_quote_ASPLUS:
//  case TAG_quote_PLUSEQUAL:
    res = "+=";
    break;

  case TAG_quote_ASMINUS:
//  case TAG_quote_MINUSEQUAL:
    res = "-=";
    break;

  case TAG_quote_ASRIGHTSHIFT:
//  case TAG_quote_DOUBLEGREATEREQUAL:
    res = ">>=";
    break;

  case TAG_quote_ASLEFTSHIFT:
//  case TAG_quote_DOUBLELESSEQUAL:
    res = "<<=";
    break;

  case TAG_quote_ASBITWISEAND:
//  case TAG_quote_ANDEQUAL:
    res = "&=";
    break;

  case TAG_quote_ASBITWISEEXOR:
//  case TAG_quote_CIRCUMFLEXEQUAL:
    res = "^=";
    break;

  case TAG_quote_ASBITWISEINCLOR:
//  case TAG_quote_VERTICALLINEEQUAL:
    res = "|=";
    break;

    // LogOp -- the same for Java and C++
  case TAG_quote_OR:
//  case TAG_quote_DOUBLEVERTICALLINE:
    res = "||";
    break;

  case TAG_quote_AND:
//  case TAG_quote_DOUBLEAND:
    res = "&&";
    break;

    // BitOp -- the same for Java and C++
  case TAG_quote_EXCLOR:
//  case TAG_quote_CIRCUMFEX:
    res = "^";
    break;

  case TAG_quote_BITAND:
//  case TAG_quote_AMPERSAND:
    res = "&";
    break;

  case TAG_quote_REFERENCE:
    res = (isJAVA() ? "" : "&");
    break;

  case TAG_quote_BITOR:
//  case TAG_quote_VERTICALLINE:
    res = "|";
    break;

    // EqOp  -- the same for Java and C++
  case TAG_quote_EQ:
//  case TAG_quote_DOUBLEEQUAL:
    res = "==";
    break;

  case TAG_quote_NEQ:
//  case TAG_quote_EXCLAMATIONMARKEQUAL:
    res = "!=";
    break;

    // RelOp -- the same for Java and C++
  case TAG_quote_LT:
//  case TAG_quote_LESS:
    res = "<";
    break;

  case TAG_quote_GT:
//  case TAG_quote_GREATER:
    res = ">";
    break;

  case TAG_quote_LEQ:
//  case TAG_quote_LESSEQUAL:
    res = "<=";
    break;

  case TAG_quote_GEQ:
//  case TAG_quote_GREATEREQUAL:
    res = ">=";
    break;

    // ShOp -- the same for Java and C++
  case TAG_quote_LEFTSHIFT:
//  case TAG_quote_DOUBLELESS:
    res = "<<";
    break;

  case TAG_quote_RIGHTSHIFT:
//  case TAG_quote_DOUBLEGREATER:
    res = ">>";
    break;

    // BinOp -- the same for Java and C++
  case TAG_quote_PLUS:
    res = "+";
    break;

  case TAG_quote_MINUS:
    res = "-";
    break;

  case TAG_quote_MULT:
//  case TAG_quote_STAR:
    res = "*";
    break;

  case TAG_quote_DIV:
    res = "/";
    break;

  case TAG_quote_MOD:
//  case TAG_quote_PRECENTAGEMARK:
    res = "%";
    break;

    // PmOp -- does not exist for Java
  case TAG_quote_DOTSTAR:
#ifdef VDMPP
    if (isJAVA())
      res = "???";
    else
#endif // VDMPP
      res = ".* ";
    break;

  case TAG_quote_ARROWSTAR:
#ifdef VDMPP
    if (isJAVA())
      res = "???";
    else
#endif // VDMPP
      res = "->*";
    break;

  // IncDecOp -- the same for Java and C++

  case TAG_quote_PPLUS:
//  case TAG_quote_DOUBLEPLUS:
    res = "++";
    break;

  case TAG_quote_PMINUS:
//  case TAG_quote_DOUBLEMINUS:
    res = "--";
    break;

  case TAG_quote_ISINSTANCEOF:
    res = "instanceof";
    break;

  // UnaryOp -- indirection op not supported by Java (because no pointers)
  case TAG_quote_INDIRECTION:
#ifdef VDMPP
    if (isJAVA())
      res = "???";
    else
#endif // VDMPP
      res = "*";
    break;

  case TAG_quote_ADDROP:
    res = "&";
    break;

  case TAG_quote_NEG:
//  case TAG_quote_EXCLAMATIONMARK:
    res = "!";
    break;

  case TAG_quote_COMPL:
//  case TAG_quote_TILDE:
    res = "~";
    break;

    // Operator -- delete operator not supported by Java
//  case TAG_quote_NEW:
//    res = "new";
//    break;

//  case TAG_quote_DELETE:
//    if (isJAVA())
//      res = "???";
//    else
//      res = "delete";
//    break;

//  case TAG_quote_COMMA:
//    res = ",";
//    break;

//  case TAG_quote_ARROW:
//    res = "->";
//    break;

//  case TAG_quote_BRACKETS:
//    res = "()";
//    break;

//  case TAG_quote_SQUAREBRACKETS:
//    res = "[]";
//    break;

    // cv-qualitier
  case TAG_quote_CONST: //constants in Java has to be declared final
#ifdef VDMPP
    if (isJAVA())
      res = "final";
    else
#endif // VDMPP
      res = "const";
    break;

  case TAG_quote_VOLATILE:
    res = "volatile";
    break;

  case TAG_quote_ELLIPSIS:
    res = "...";
    break;

  case TAG_quote_CLASS:
    res = "class";
    break;

  case TAG_quote_STRUCT: // struct type not supported in Java should be a class instead....
#ifdef VDMPP
    if (isJAVA())
      res = "struct-class";
    else
#endif // VDMPP
      res = "struct";
    break;

  case TAG_quote_UNION:  // union type not supported in Java, simulate by subclassing
#ifdef VDMPP
    if (isJAVA())
      res = "union(should be simulated by subclassing)";
    else
#endif // VDMPP
      res = "union";
    break;

  case TAG_quote_PRIVATE:
    res = "private";
    break;

  case TAG_quote_PROTECTED:
    res = "protected";
    break;

  case TAG_quote_ABSTRACT:
    res = "abstract";
    break;

  case TAG_quote_PUBLIC:
    res = "public";
    break;

  case TAG_quote_FINAL:
    res = "final";
    break;

  case TAG_quote_INLINE:
#ifdef VDMPP
    if (isJAVA())
      res = "inline???";
    else
#endif // VDMPP
      res = "inline";
    break;

  case TAG_quote_VIRTUAL:
#ifdef VDMPP
    if (isJAVA())
      res = "virtual???";
    else
#endif // VDMPP
      res = "virtual";
    break;

  case TAG_quote_AUTO:
#ifdef VDMPP
    if (isJAVA())
      res = "auto???";
    else
#endif // VDMPP
      res = "auto";
    break;

  case TAG_quote_REGISTER:
#ifdef VDMPP
    if (isJAVA())
      res = "register???";
    else
#endif // VDMPP
      res = "register";
    break;

  case TAG_quote_STATIC:
    res = "static";
    break;

  case TAG_quote_EXTERN:
#ifdef VDMPP
    if (isJAVA()) {
      OutputErr(L"Internal error in the code generator backend (1).");
      OutputErr(L"Please report this error.");
      res = "\n// This part is not implemented (internal error).\n";
    }
    else
#endif // VDMPP
      res = "extern";
    break;

  default:
    res = " ";
    break;
  }
  return res;
}

///////////////////////////////
// Name : OpPrec
// Descr:
// Calculate the precedence of given constructs.
// The functions return a value based on its precedence -
// 1 is the lowest. The values generated is based on the
// table given in Bjarne Stroustrups C++ book(Operator Summary).
// "::" is given precedence 18 and "," 1.
// This functions is used to reconstruct the structure
// of e.g. an expressions without introducing to many
// brackets. In the abstract syntax brackets
// has been removed. and we do not want to codegenerate
// 2 * 3 + 5 as (2 * 3) + 5 so we need to know about the
// precedence of various constructs.
///////////////////////////////

int CGBackEnd::OpPrec(const Generic & g)
{
  int prec;
  if (g.IsQuote()) {
    Quote i(g);
    int op = Quote2Int(i);
    switch(op) {

      // AssignOp
    case TAG_quote_ASEQUAL:
    case TAG_quote_ASMULT:
    case TAG_quote_ASDIV:
    case TAG_quote_ASMOD:
    case TAG_quote_ASPLUS:
    case TAG_quote_ASMINUS:
    case TAG_quote_ASRIGHTSHIFT:
    case TAG_quote_ASLEFTSHIFT:
    case TAG_quote_ASBITWISEAND:
    case TAG_quote_ASBITWISEEXOR:
    case TAG_quote_ASBITWISEINCLOR:
      prec = 3;
      break;
      // LogOp
    case TAG_quote_OR:
      prec = 5;
      break;
    case TAG_quote_AND:
      prec = 6;
      break;

      // BitOp
    case TAG_quote_BITOR:
      prec = 7;
      break;
    case TAG_quote_EXCLOR:
      prec = 8;
      break;
    case TAG_quote_BITAND:
      prec = 9;
      break;

      // EqOp
    case TAG_quote_EQ:
    case TAG_quote_NEQ:
      prec = 10;
      break;

      // TypeComparison
    case TAG_quote_ISINSTANCEOF:
      prec = 10;
      break;

      // RelOp
    case TAG_quote_LT:
    case TAG_quote_GT:
    case TAG_quote_LEQ:
    case TAG_quote_GEQ:
      prec = 11;
      break;

      // ShOp
    case TAG_quote_LEFTSHIFT:
    case TAG_quote_RIGHTSHIFT:
      prec = 12;
      break;

      // BinOp
    case TAG_quote_PLUS:
    case TAG_quote_MINUS:
      prec = 13;
      break;
    case TAG_quote_MULT:
    case TAG_quote_DIV:
    case TAG_quote_MOD:
      prec = 14;
      break;

      // PmOp
    case TAG_quote_DOTSTAR:
    case TAG_quote_ARROWSTAR:
      prec = 15;
      break;

      // IncDecOp
    case TAG_quote_PPLUS:
    case TAG_quote_PMINUS:

      // UnaryOp
    case TAG_quote_INDIRECTION:
    case TAG_quote_ADDROP:
    case TAG_quote_NEG:
    case TAG_quote_COMPL:
      prec = 16;
      break;
    default:
      prec = 20;
      break;
    }
  }
  else if (g.IsRecord())
  {
    TYPE_CPP_Expr rc (g);
    switch (rc.GetTag()) {
    case TAG_TYPE_CPP_AssignExpr:
    case TAG_TYPE_CPP_LogicalExpr:
    case TAG_TYPE_CPP_BitExpr:
    case TAG_TYPE_CPP_EqualityExpr:
    case TAG_TYPE_CPP_RelationalExpr:
    case TAG_TYPE_CPP_ShiftExpr:
    case TAG_TYPE_CPP_BinExpr:
    case TAG_TYPE_CPP_PmExpr: {
      Quote op (Record(rc.GetField(2)).GetField(1));
      prec = OpPrec(op);
      break;
    }
      // ConditionalExpr
    case TAG_TYPE_CPP_ConditionalExpr:
      prec = 4;
      break;
    case TAG_TYPE_CPP_AllocationTypeExpr:
    case TAG_TYPE_CPP_DeallocationExpr:
    case TAG_TYPE_CPP_DeallocationArrayExpr:
    case TAG_TYPE_CPP_CastExpr:
      prec = 16;
      break;
    case TAG_TYPE_CPP_UnaryOpExpr:{
      Quote op (Record(rc.GetField(1)).GetField(1));
      prec = OpPrec(op);
      break;
    }
    case TAG_TYPE_CPP_PreInDecrementExpr:
    case TAG_TYPE_CPP_PostFixInDecrementExpr: {
      Quote op (rc.GetField(1));
      prec = OpPrec(op);
      break;
    }
    case TAG_TYPE_CPP_ObjectMemberAccess:
    case TAG_TYPE_CPP_PointerToObjectMemberAccess:
    case TAG_TYPE_CPP_ArrayApply:
    case TAG_TYPE_CPP_FctCall:
    case TAG_TYPE_CPP_SizeOfExpr:
    case TAG_TYPE_CPP_SizeOfType: {
      prec = 17;
      break;
    }
//    case TAG_TYPE_CPP_ScopeResIdentifier:
//    case TAG_TYPE_CPP_ScopeResOpFctName:
//    case TAG_TYPE_CPP_ScopeResQualifiedName:
    case TAG_TYPE_CPP_QualifiedName: {
      prec = 18;
      break;
    }
    default:
      // identifiers, literals, fct. call, etc.
      prec = 20;
      break;
    }
  }
  else
    prec = 20;
  return prec;
}

void CGBackEnd::GenStmt(const TYPE_CPP_Stmt & stmt, bool sw)
{
  switch(stmt.GetTag()) {
    case TAG_TYPE_CPP_CompoundStmt: {
      if (sw)
        CloseScope();

      GenCode(stmt);

      if (sw)
        OpenScope();
      break;
    }
    default: {
      if (!sw)
        OpenScope();

      if (isCPP() && (GetContext() != ELSE))
        GenNewLine(m4code);

      GenCode(stmt);

      if (!sw)
        CloseScope();
    }
  }
}

void CGBackEnd::GenCodeSeq(const Sequence & s, const string & delim)
{
  size_t len = s.Length();
  for (size_t i = 1; i <= len; i++)
  {
    if ( i > 1 )
      m4code << delim;
    GenCode(s[i]);
  }
}

void CGBackEnd::OutputErr(const wstring & msg)
{
#ifdef TESTSPEC
  wcerr << msg << endl << flush;
#else
  vdm_err << msg << endl << flush;
#endif // TESTSPEC
}

void CGBackEnd::OutputLog(const wstring & msg)
{
#ifdef TESTSPEC
  wcout << msg << endl << flush;
#else
  vdm_log << msg << endl << flush;
#endif // TESTSPEC
}

