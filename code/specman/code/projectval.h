/***
*  * WHAT
*  *    Auxiliary functions to access definitions in
*  *    class ProjectTypes
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/projectval.h,v $
*  * VERSION
*  *    $Revision: 1.49 $
*  * DATE
*  *    $Date: 2005/07/11 05:53:46 $
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

#ifndef __projectval_h__
#define __projectval_h__

#include "metaiv.h"

// Quotes in ProjectTypes

extern Quote none_session;
extern Quote flat_session;
extern Quote struct_session;
extern Quote disabled_session;

extern Quote action_syntax;
extern Quote action_type;
extern Quote action_cg;
extern Quote action_pp;

extern Quote tc_pos;
extern Quote tc_def;

// status ans status type is used to decode status of files and
// units

class status_type {
public:
  enum status { status_none, status_error, status_ok, status_pos, status_def };
  status syntax, type, cg, jcg, pp;

  // These are necessary to avoid internal error in VC5.0.
  status_type() :
    syntax(status_none), type(status_none), cg(status_none), jcg(status_none), pp(status_none) {};
  status_type(const status_type & st) :
    syntax(st.syntax), type(st.type), cg(st.cg), jcg(st.jcg), pp(st.pp) {};
  status_type& operator=(const status_type & o)
  {
    this->syntax = o.syntax;
    this->type = o.type;
    this->cg = o.cg;
    this->jcg = o.jcg;
    this->pp = o.pp;
    return *this;
  };
};

#include "metaiv.h"
#include "ProjectTypes.h"
#include "AS.h"
#include <string>

class PTAUX
{
public:
  static wstring Seq2Str (const SEQ<Char> & msg)
  {
    return msg.GetString();
  }

  static SEQ<Char> Str2Seq (const std::wstring & msg)
  {
    return SEQ<Char>(msg);
  }

  static TYPE_ProjectTypes_FileName mk_FileName (const std::wstring & file);
  static TYPE_ProjectTypes_Name mk_Name (const std::wstring & name);
  static TYPE_ProjectTypes_ModuleName mk_ModuleName (const std::wstring & file);
//  static TYPE_ProjectTypes_ModuleName mk_ModuleName (const TYPE_AS_Name & file);
  static TYPE_ProjectTypes_Module mk_Module (const TYPE_AS_Name & AsName, const Record & AsModule);
#ifdef VDMSL
  static TYPE_ProjectTypes_FlatSpec mk_FlatSpec (const TYPE_AS_Name & AsName,
                                                 const Record & AsDefs,
                                                 const Record & Origin);
#endif // VDMSL
  static TYPE_ProjectTypes_AstVal mk_AstVal (const Record & AST);
  static TYPE_ProjectTypes_CGInfo mk_CGInfo (const Tuple & info);
  static TYPE_ProjectTypes_ToolCommand mk_ToolCommand (const std::string & command);
  static TYPE_ProjectTypes_ToolCommand mk_ToolCommand (const std::wstring & command);
  static TYPE_ProjectTypes_ErrMsg mk_ErrMsg(const TYPE_CI_FileId & fid,
                                            const TYPE_CI_Line & line,
                                            const TYPE_CI_Column & col,
                                            const type_cLL & msg);
  static TYPE_ProjectTypes_WarnMsg mk_WarnMsg(const TYPE_CI_FileId & fid,
                                              const TYPE_CI_Line & line,
                                              const TYPE_CI_Column & col,
                                              const type_cLL & msg);
  static TYPE_ProjectTypes_InfoMsg mk_InfoMsg(const type_cLL & msg);
  static TYPE_ProjectTypes_BreakNameSet mk_BreakNameSet (const TYPE_AS_Name & AsName, const Int & num);
  static TYPE_ProjectTypes_BreakPosSet mk_BreakPosSet (const std::wstring & file,
                                                       const Int & line,
                                                       const Int & col,
                                                       const Int & num);
  static TYPE_ProjectTypes_BreakEnable mk_BreakEnable(const Int & num);
  static TYPE_ProjectTypes_BreakDisable mk_BreakDisable(const Int & num);
  static TYPE_ProjectTypes_BreakRemove mk_BreakRemove (const Int & num);
  static TYPE_ProjectTypes_BackTrace mk_BackTrace (const Sequence& bt_l);
  static TYPE_ProjectTypes_BtGotoLevel mk_BtGotoLevel(int level);
  static int ExtractBtGotoLevel(const TYPE_ProjectTypes_BtGotoLevel &);
  static TYPE_ProjectTypes_DependInfo mk_DependInfo (
                                 const TYPE_ProjectTypes_ModuleName & name,
                                 const SET<TYPE_ProjectTypes_ModuleName> & supers,
                                 const SET<TYPE_ProjectTypes_ModuleName> & subs,
                                 const SET<TYPE_ProjectTypes_ModuleName> & uses,
                                 const SET<TYPE_ProjectTypes_ModuleName> & used,
                                 const Bool &);
  static TYPE_ProjectTypes_DependUpdate mk_DependUpdate (const SET<TYPE_ProjectTypes_ModuleName> & modules);
  static TYPE_ProjectTypes_DrawInhTree mk_DrawInhTree(const Sequence & node_line_s);
  static TYPE_ProjectTypes_ClearInhTree mk_ClearInhTree();
  static TYPE_ProjectTypes_ClearDebugWindow mk_ClearDebugWindow();
  static TYPE_ProjectTypes_UpdateSelections mk_UpdateSelections();
  static TYPE_ProjectTypes_Node mk_Node(const Int &, const Int &, const Token &);
  static TYPE_ProjectTypes_Line mk_Line(const Int &, const Int &, const Int &, const Int &);
  static TYPE_ProjectTypes_SemVal mk_SemVal(const Record &);
  static TYPE_ProjectTypes_InfoString mk_InfoString (const std::wstring &);
  static TYPE_ProjectTypes_DepGraph mk_DepGraph (const Record &);
  static TYPE_ProjectTypes_TCEnv mk_TCEnv (const Tuple &);

  static bool is_ErrMsg(const TYPE_ProjectTypes_Message & em);
  static bool is_WarnMsg(const TYPE_ProjectTypes_Message & wm);
  static bool is_ToolCommand(const Record & toolcom);
  static bool is_FileName(const Record & r);

  static Tuple ExtractTCEnv (const TYPE_ProjectTypes_TCEnv &);
  static Record ExtractDepGraph (const TYPE_ProjectTypes_DepGraph &);
#ifdef VDMSL
  static Tuple ExtractFlatSpec (const TYPE_ProjectTypes_FlatSpec & FlatSpec);
#endif // VDMSL
  static std::wstring ExtractFileName (const TYPE_ProjectTypes_FileName & fnm);
  static std::wstring ExtractName (const TYPE_ProjectTypes_Name & fnm);
  static std::wstring ExtractInfoString (const TYPE_ProjectTypes_InfoString & name);
  static std::wstring ExtractModuleName (const TYPE_ProjectTypes_ModuleName & fnm);
  static std::wstring ExtractMessageAll (const std::wstring & file,
                                         const TYPE_ProjectTypes_Message  & msg);
  static Record ExtractAstOrFlat (const TYPE_ProjectTypes_AstVal & astval);
  static Record ExtractAstVal (const TYPE_ProjectTypes_AstVal & astval);
//  static TYPE_ProjectTypes_AstVal UpdateAstOrFlat (const TYPE_ProjectTypes_AstVal & AstVal,
//                                                   const TYPE_AS_Module & AST);
  static SEQ<Record> ExtractAstOrFlats (const SEQ<TYPE_ProjectTypes_AstVal> & Asts);
  static std::wstring ExtractMessage (const TYPE_ProjectTypes_InfoMsg & msg);
  static status_type ExtractStatus (const Tuple & FileAndModuleStatus);
  static Generic ExtractCGInfo (const Generic & CGInfo);
  static std::wstring ExtractToolCommand (const TYPE_ProjectTypes_ToolCommand & ToolCommand);
  static Tuple ExtractBreakNameSet (const TYPE_ProjectTypes_BreakNameSet & bp);
  static Tuple ExtractBreakPosSet(const TYPE_ProjectTypes_BreakPosSet & bp);
  static int ExtractBreakEnable(const TYPE_ProjectTypes_BreakEnable& bp);
  static int ExtractBreakDisable(const TYPE_ProjectTypes_BreakDisable& bp);
  static int ExtractBreakRemove(const TYPE_ProjectTypes_BreakRemove& bp);
  static Sequence ExtractBackTrace (const TYPE_ProjectTypes_BackTrace& bt);
  static Set ExtractFunctionSet (const TYPE_ProjectTypes_FunctionSet& fn);
  static Generic ExtractSemVal(const Generic&);
  static Record ExtractModuleAst(const TYPE_ProjectTypes_Module&);
  static std::wstring ExtractOldProjName (const TYPE_ProjectTypes_ChangedProjName&);
  static std::wstring ExtractNewProjName (const TYPE_ProjectTypes_ChangedProjName&);

  static TYPE_AS_Name ModuleName2ASName (const TYPE_ProjectTypes_ModuleName&);
  static TYPE_ProjectTypes_ModuleName ASName2ModuleName (const TYPE_AS_Name&);
  static SET<TYPE_ProjectTypes_ModuleName> ASNameSet2ModuleNameSet (const SET<TYPE_AS_Name>&);
  static SET<TYPE_AS_Name> ModuleNameSet2ASNameSet (const SET<TYPE_ProjectTypes_ModuleName>&);

  static bool IsEverythingTypeCorrect();
  static void UpdateIsEverythingTypeCorrect();
  static void SomethingIsTypeIncorrect();

  static int Compare (const TYPE_ProjectTypes_Message & tp1,
                      const TYPE_ProjectTypes_Message & tp2,
                      bool sep);
  static SEQ<TYPE_ProjectTypes_Message> QS (const SEQ<TYPE_ProjectTypes_Message> & sq, bool sep);

private:
  static bool isEverythingTypeCorrect;
};

#endif // __projectval_h__
