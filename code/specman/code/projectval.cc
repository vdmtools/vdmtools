/***
*  * WHAT
*  *    Auxiliary functions to access definitions in
*  *    class ProjectTypes
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/projectval.cc,v $
*  * VERSION
*  *    $Revision: 1.61 $
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

#include "ProjectTypes.h"
#include "projectval.h"
#include "tbutils.h"
#include "cg_aux.h"
#include "contextinfo.h"
#include "astaux.h"
#include "UMLTool.h"
#include "UTIL.h"
#include "Repository.h"
#include "tb_wstring.h"
#include "tools.h"
#include <stdio.h>
#include <assert.h>

// Quotes in ProjectTypes

Quote none_session = Quote (L"NONE");
Quote flat_session = Quote (L"FLAT");
Quote struct_session = Quote (L"STRUCTURED");
Quote disabled_session = Quote (L"DISABLED");

Quote action_syntax = Quote (L"SYNTAXCHECK");
Quote action_type   = Quote (L"TYPECHECK");
Quote action_cg     = Quote (L"CG");
Quote action_jcg    = Quote (L"JCG");
Quote action_pp     = Quote (L"PP");

Quote tc_pos        = Quote (L"POS");
Quote tc_def        = Quote (L"DEF");

// FlatStore is used to store AS`Definitions and OriginOfActiveConstructs
// of a flat specification in ONE record. This record is the value (ast)
// of type FlatSpec (see class ProjectTypes).
// NOTE: If this enum change, the AST version should also be changed!
#ifdef VDMSL
enum {
  vdm_PROJECTVAL_FlatStore = TAG_TYPE_PROJECTVAL_FlatStore,
  length_PROJECTVAL_FlatStore = 2,
  pos_PROJECTVAL_FlatStore_Ast = 1,
  pos_PROJECTVAL_FlatStore_Origin = 2
};

class TYPE_PROJECTVAL_FlatStore : public Record {
public:

  TYPE_PROJECTVAL_FlatStore () : Record(TAG_TYPE_PROJECTVAL_FlatStore, 2) {}

  TYPE_PROJECTVAL_FlatStore &Init (const Record &p1, const Record &p2)
  {
    SetField(pos_PROJECTVAL_FlatStore_Ast, p1);
    SetField(pos_PROJECTVAL_FlatStore_Origin, p2);
    return *this; };

  TYPE_PROJECTVAL_FlatStore (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_PROJECTVAL_FlatStore";
  }

  const Record & get_Ast () const { return this->GetRecord(pos_PROJECTVAL_FlatStore_Ast); };
  void set_Ast (const Record &p) { this->SetField(pos_PROJECTVAL_FlatStore_Ast, p); };
  const Record & get_Origin () const { return this->GetRecord(pos_PROJECTVAL_FlatStore_Origin); };
  void set_Origin (const Record &p) { this->SetField(pos_PROJECTVAL_FlatStore_Origin, p); };
};
#endif // VDMSL

class Values : public vdm_ProjectTypes {
public:
  static wstring Seq2Str (const SEQ<Char> & msg)
  {
    return msg.GetString();
  }

  static TYPE_ProjectTypes_FileName mk_FileName (const std::wstring & file)
  {
    return TYPE_ProjectTypes_FileName().Init(SEQ<Char>(file));
  }

  static TYPE_ProjectTypes_Name mk_Name (const std::wstring & name)
  {
    return TYPE_ProjectTypes_Name().Init(SEQ<Char>(name));
  }
  
  static TYPE_ProjectTypes_InfoString mk_InfoString (const std::wstring & name)
  {
    return TYPE_ProjectTypes_InfoString().Init(SEQ<Char>(name));
  }

  static TYPE_ProjectTypes_DepGraph mk_DepGraph (const Record& g)
  {
    TYPE_ProjectTypes_DepGraph dep;
    dep.SetField (pos_DepGraph_g,g);   // Token is spec
    return dep;
  }

  static TYPE_ProjectTypes_TCEnv mk_TCEnv (const Tuple& t)
  {
    TYPE_ProjectTypes_TCEnv env;
    env.SetField (pos_TCEnv_e,t);      // Token in spec
    return env;
  }

  static TYPE_ProjectTypes_ModuleName mk_ModuleName (const std::wstring & nm)
  {
    TYPE_ProjectTypes_ModuleName f;
    f.set_nm (Sequence (nm));
    return f;
  }

  static Record ExtractDepGraph (const TYPE_ProjectTypes_DepGraph & gr)
  {
    return gr.GetField (pos_DepGraph_g);
  }

  static Tuple ExtractTCEnv (const TYPE_ProjectTypes_TCEnv & e) {
    return e.GetField (pos_TCEnv_e);
  }
  
  static std::wstring ExtractFileName (const TYPE_ProjectTypes_FileName & fnm)
  {
    return Seq2Str (fnm.GetSequence(pos_FileName_nm));
  }
  
  static std::wstring ExtractName (const TYPE_ProjectTypes_Name & nm)
  {
    return Seq2Str (nm.GetSequence(pos_Name_nm));
  }
  
  static std::wstring ExtractInfoString (const TYPE_ProjectTypes_InfoString & nm)
  {
    return Seq2Str (nm.GetSequence(pos_InfoString_nm));
  }
  
  static std::wstring ExtractModuleName (const TYPE_ProjectTypes_ModuleName & mnm)
  {
    return Seq2Str (mnm.GetSequence(pos_ModuleName_nm));
  }
  
  // mk_AstVal
  // AST : AS`DLModule | AS`Module | AS`Class | CPP`Module
  // ==> ProjectTypes`AstVal
  static TYPE_ProjectTypes_AstVal mk_AstVal (const Record & AST)
  {
    TYPE_ProjectTypes_AstVal astval;
    astval.SetField (pos_AstVal_val, AST); // Token in spec
    return astval;
  }

  // mk_Module
  // AsName : AS`Name
  // AsModule : AS`DLModule | AS`Module | AS`Class | CPP`Module
  // ==> ProjectTypes`Module
  static TYPE_ProjectTypes_Module mk_Module (const TYPE_AS_Name & AsName, const Record & AsModule)
  {
    TYPE_ProjectTypes_Module mod;
    mod.Init( PTAUX::ASName2ModuleName (AsName), mk_AstVal(AsModule) );
    return mod;
  }

  static Record ExtractModuleAst (const TYPE_ProjectTypes_Module & m)
  {
    return ExtractAstVal (m.get_ast());
  }
  
#ifdef VDMSL
  // mk_FlatSpec
  // asName : AS`Name
  // asDefs : AS`Module
  static TYPE_ProjectTypes_FlatSpec mk_FlatSpec (const TYPE_AS_Name & asName,
                                                 const TYPE_AS_Module & asDefs,
			                         const Record & origin)
  {
    TYPE_PROJECTVAL_FlatStore store;
    store.SetField(pos_PROJECTVAL_FlatStore_Ast, asDefs);
    store.SetField(pos_PROJECTVAL_FlatStore_Origin, origin);
    
    TYPE_ProjectTypes_FlatSpec FlatSpec;
    //FlatSpec.Init( mk_ModuleName (asName), mk_AstVal(store) );
    FlatSpec.Init( PTAUX::ASName2ModuleName (asName), mk_AstVal(store) );
    return FlatSpec;
  }
#endif // VDMSL

  static TYPE_ProjectTypes_CGInfo mk_CGInfo (const Tuple& info)
  {
    TYPE_ProjectTypes_CGInfo CGInfo;
//    CGInfo.set_repos( info );
    CGInfo.SetField (pos_CGInfo_repos, info);  // Token in spec
    return CGInfo;
  }

  static TYPE_ProjectTypes_ToolCommand mk_ToolCommand (const Token & command)
  {
    return TYPE_ProjectTypes_ToolCommand().Init(command);
  }

  static TYPE_ProjectTypes_ErrMsg mk_ErrMsg(const TYPE_CI_FileId & fid,
                                            const TYPE_CI_Line & line,
                                            const TYPE_CI_Column & col,
                                            const type_cLL & descr_l)
  {
    return TYPE_ProjectTypes_ErrMsg().Init(fid, line, col, descr_l);
  }

  static bool is_ErrMsg(const TYPE_ProjectTypes_Message & em)
  {
    return em.Is(TAG_TYPE_ProjectTypes_ErrMsg);
  }

  static Tuple ExtractErrMsg(const TYPE_ProjectTypes_Message & msg)
  {
    switch(msg.GetTag()) {
      case TAG_TYPE_ProjectTypes_ErrMsg: {
        return mk_(msg.GetInt(pos_ErrMsg_fid),
                   msg.GetInt(pos_ErrMsg_line),
                   msg.GetInt(pos_ErrMsg_col),
                   msg.GetSequence(pos_ErrMsg_msg));
      }
      case TAG_TYPE_ProjectTypes_WarnMsg: {
        return mk_(msg.GetInt(pos_WarnMsg_fid),
                   msg.GetInt(pos_WarnMsg_line),
                   msg.GetInt(pos_WarnMsg_col),
                   msg.GetSequence(pos_WarnMsg_msg));
      }
      default: {
        return mk_(Int(-1), Int(-1), Int(-1), Sequence());
      }
    } 
  }

  static bool is_ToolCommand(const Record & toolcom)
  {
    return toolcom.Is(TAG_TYPE_ProjectTypes_ToolCommand);
  }

  static bool is_FileName(const Record & r)
  {
    return r.Is(TAG_TYPE_ProjectTypes_FileName);
  }
  
  static TYPE_ProjectTypes_WarnMsg mk_WarnMsg(const TYPE_CI_FileId & fid,
                                              const TYPE_CI_Line & line,
                                              const TYPE_CI_Column & col,
                                              const type_cLL & descr_l)
  {
    return TYPE_ProjectTypes_WarnMsg().Init(fid, line, col, descr_l);
  }

  static TYPE_ProjectTypes_BreakNameSet mk_BreakNameSet (const TYPE_AS_Name& AsName, const Int & num)
  {
    TYPE_ProjectTypes_BreakNameSet bs;
    bs.SetField(pos_BreakNameSet_bp, AsName);  // Token is spec
    bs.SetField(pos_BreakNameSet_num, num);
    return bs;
  }
  
  static TYPE_ProjectTypes_BreakPosSet mk_BreakPosSet (const std::wstring& file,
                                                       const Int & line,
                                                       const Int & col,
                                                       const Int & num)
  {
    TYPE_ProjectTypes_BreakPosSet bs;
    bs.SetField (pos_BreakPosSet_bp, ASTAUX::MkName(file)); // Token is spec
    bs.set_line (line);
    bs.set_col (col);
    bs.set_num (num);
    return bs;
  }
  
  static TYPE_ProjectTypes_BreakEnable mk_BreakEnable (const Int & num)
  {
    return TYPE_ProjectTypes_BreakEnable().Init(num);
  }

  static TYPE_ProjectTypes_BreakDisable mk_BreakDisable (const Int & num)
  {
    return TYPE_ProjectTypes_BreakDisable().Init(num);
  }

  static TYPE_ProjectTypes_BreakRemove mk_BreakRemove (const Int & num)
  {
    return TYPE_ProjectTypes_BreakRemove().Init(num);
  }

  static TYPE_ProjectTypes_BackTrace mk_BackTrace (const Sequence & bt_l)
  {
    return TYPE_ProjectTypes_BackTrace().Init(bt_l);
  }

  static TYPE_ProjectTypes_BtGotoLevel mk_BtGotoLevel (int level)
  {
    return TYPE_ProjectTypes_BtGotoLevel().Init(Int(level));
  }
  
  static int ExtractBtGotoLevel(const TYPE_ProjectTypes_BtGotoLevel & bt)
  {
    return bt.GetIntValue(pos_BtGotoLevel_level);
  }   

  static TYPE_ProjectTypes_DependInfo mk_DependInfo (
                                const TYPE_ProjectTypes_ModuleName & name,
                                const SET<TYPE_ProjectTypes_ModuleName>& supers,
                                const SET<TYPE_ProjectTypes_ModuleName>& subs,
                                const SET<TYPE_ProjectTypes_ModuleName>& uses, 
                                const SET<TYPE_ProjectTypes_ModuleName>& used,
                                const Bool& completed)
  {
    return TYPE_ProjectTypes_DependInfo().Init(name, supers, subs, uses, used, completed);
  }

  static TYPE_ProjectTypes_DependUpdate mk_DependUpdate (const SET<TYPE_ProjectTypes_ModuleName>& modules)
  {
    return TYPE_ProjectTypes_DependUpdate().Init(modules);
  }

  static TYPE_ProjectTypes_DrawInhTree mk_DrawInhTree(const Sequence & node_line_s)
  {
    return TYPE_ProjectTypes_DrawInhTree().Init(node_line_s);
  }

  static TYPE_ProjectTypes_ClearInhTree mk_ClearInhTree () {
    TYPE_ProjectTypes_ClearInhTree cit;
    return cit;
  }

  static TYPE_ProjectTypes_ClearDebugWindow mk_ClearDebugWindow() {
    TYPE_ProjectTypes_ClearDebugWindow cdw;
    return cdw;
  }
  
  static TYPE_ProjectTypes_UpdateSelections mk_UpdateSelections () {
    TYPE_ProjectTypes_UpdateSelections us;
    return us;
  }

  static TYPE_ProjectTypes_Node mk_Node(const Int& x, const Int& y, const Token& nm)
  {
    TYPE_ProjectTypes_Node node;
    node.set_x(x);
    node.set_y(y);
//    node.set_name(nm);
    node.SetField(pos_Node_name, nm); // Token is spec
    return node;
  }

  static TYPE_ProjectTypes_Line mk_Line(const Int& x1, const Int& x2, const Int& y1, const Int& y2)
  {
    type_ii2P pos1;
    type_ii2P pos2;
    pos1.SetField(1, x1);
    pos1.SetField(2, x2);
    pos2.SetField(1, y1);
    pos2.SetField(2, y2);

    TYPE_ProjectTypes_Line line;
    line.Init( pos1, pos2 );

    return line;
  }

  static bool is_WarnMsg(const TYPE_ProjectTypes_Message & wm)
  {
    return wm.Is(TAG_TYPE_ProjectTypes_WarnMsg);
  }
  
  static TYPE_ProjectTypes_InfoMsg mk_InfoMsg(const type_cLL & msg)
  {
    return TYPE_ProjectTypes_InfoMsg().Init(msg);
  }

  // ExtractAstVal
  // astval : ProjectTypes`AstVal
#ifdef VDMSL
  // -> AS`DLModule | AS`Module | TYPE_PROJECTVAL`FlatStore
#endif // VDMSL
#ifdef VDMPP
  // -> AS`Class | CPP`Module
#endif // VDMPP
  static Record ExtractAstVal (const TYPE_ProjectTypes_AstVal & astval)
  {
    return astval.GetRecord (pos_AstVal_val);
  }
   
  // ExtractAstOrFlat
  // astval : ProjectTypes`AstVal
  // -> AS`DLModule | AS`Module | AS`Class | CPP`Module
  static Record ExtractAstOrFlat (const TYPE_ProjectTypes_AstVal & astval)
  {
    Record ast (ExtractAstVal (astval));
#ifdef VDMSL
    if (ast.Is(TAG_TYPE_PROJECTVAL_FlatStore))
      return ast.GetRecord(pos_PROJECTVAL_FlatStore_Ast);
    else
      return ast;
#endif // VDMSL
#ifdef VDMPP
    return ast;
#endif // VDMPP
  }


//  static TYPE_ProjectTypes_AstVal UpdateAstOrFlat (const TYPE_ProjectTypes_AstVal & AstVal,
//                                                   const TYPE_AS_Module & AST)
//  {
//    Record ast (ExtractAstVal (AstVal));
//    if (ast.Is(TAG_TYPE_PROJECTVAL_FlatStore)) {
//      ast.SetField (pos_PROJECTVAL_FlatStore_Ast, AST);
//      return mk_AstVal (ast);
//    }
//    else
//      return mk_AstVal (AST);
//  }
  
#ifdef VDMSL
  // ExtractFlatSpec
  // flatSpec : ProjectTypes`FlatSpec
  // -> AS`Module * Origin
  static Tuple ExtractFlatSpec (const TYPE_ProjectTypes_FlatSpec & flatSpec)
  {
    const TYPE_ProjectTypes_AstVal & astVal (flatSpec.GetRecord(pos_FlatSpec_ast));
    TYPE_PROJECTVAL_FlatStore store (ExtractAstVal (astVal));
    return mk_(store.GetRecord(pos_PROJECTVAL_FlatStore_Ast),
               store.GetRecord(pos_PROJECTVAL_FlatStore_Origin));
  }
#endif // VDMSL
  
  static std::wstring ExtractToolCommand (const TYPE_ProjectTypes_ToolCommand& ToolCommand)
  {
    Token command (ToolCommand.GetField(pos_ToolCommand_command));
    return command.GetString (); // 20100616
  }

  static std::wstring ExtractMessage (const TYPE_ProjectTypes_InfoMsg & msg)
  {
    return Seq2Str (msg.GetSequence(pos_InfoMsg_msg));
  }

  static status_type::status vdm_Status2status (const Quote & Status)
  {
    status_type::status status;
    if (Status == Quote (L"NONE"))
      status = status_type::status_none;
    else if (Status == Quote (L"ERROR"))
      status = status_type::status_error;
    else if (Status == Quote (L"OK"))
      status = status_type::status_ok;
    else if (Status == Quote (L"POS"))
      status = status_type::status_pos;
    else if (Status == Quote (L"DEF"))
      status = status_type::status_def;
    else
      // This must never happen!
      status = status_type::status_none;
    return status;
  }

  static status_type ExtractStatus (const Tuple & FileAndModuleStatus)
  {
    const Generic & UnitStat (FileAndModuleStatus.GetField (1));
    const TYPE_ProjectTypes_FileStat & FileStat (FileAndModuleStatus.GetRecord (2));

    status_type status;
    status.syntax = vdm_Status2status(FileStat.GetField(pos_FileStat_syntax));
    status.pp = vdm_Status2status(FileStat.GetField(pos_FileStat_pp));

    if (UnitStat.IsNil ()) {
      status.type = status_type::status_none;
      status.cg = status_type::status_none;
      status.jcg = status_type::status_none;
    }
    else {
      TYPE_ProjectTypes_UnitStat UStat (UnitStat);
      status.type = vdm_Status2status (UStat.GetField(pos_UnitStat_type));
      status.cg = vdm_Status2status (UStat.GetField(pos_UnitStat_cg));
      status.jcg = vdm_Status2status (UStat.GetField(pos_UnitStat_javaCg));
    }
    return status;
  }

  static Tuple ExtractBreakNameSet (const TYPE_ProjectTypes_BreakNameSet & bp)
  {
    return mk_(bp.GetField (pos_BreakNameSet_bp), bp.GetInt(pos_BreakNameSet_num));
  }

  static Tuple ExtractBreakPosSet(const TYPE_ProjectTypes_BreakPosSet & bp)
  {
    return mk_(bp.GetField(pos_BreakPosSet_bp),
               bp.GetField(pos_BreakPosSet_line),
               bp.GetField(pos_BreakPosSet_col),
               bp.GetField(pos_BreakPosSet_num));
  }
  
  static int ExtractBreakEnable(const TYPE_ProjectTypes_BreakEnable& bp) 
  {
    return bp.get_bp().GetValue();
  }
  
  static int ExtractBreakDisable(const TYPE_ProjectTypes_BreakDisable& bp) 
  {
    return bp.get_bp().GetValue();
  }

  static int ExtractBreakRemove(const TYPE_ProjectTypes_BreakRemove& bp)
  {
    return bp.get_bp().GetValue();
  }
      
  static Sequence ExtractBackTrace (const TYPE_ProjectTypes_BackTrace& bt)
  {
    return bt.get_info();
  }
  
  static Set ExtractFunctionSet (const TYPE_ProjectTypes_FunctionSet& fn)
  {
    return fn.get_nms();
  }
  
  static Generic ExtractCGInfo (const Generic& CGInfo)
  {
    if (CGInfo.IsRecord ())
      return Record (CGInfo).GetField (pos_CGInfo_repos);
    else
      return Nil ();
  }

  static TYPE_ProjectTypes_SemVal mk_SemVal (const Record& v)
  {
    TYPE_ProjectTypes_SemVal r;
//    r.set_v (v);
    r.SetField (pos_SemVal_v,v);  // Token is spec
    return r;
  }

  static Generic ExtractSemVal (const Generic& s)
  {
    if (s.IsRecord ())
      return Record (s).GetField(pos_SemVal_v);
    else
      return Nil ();
  }

  static std::wstring ExtractOldProjName (const TYPE_ProjectTypes_ChangedProjName& r)
  {
    Generic nm (r.get_oldn ());
    if (nm.IsRecord ())
      return ExtractFileName (nm);
    else
      return L"";
  }

  static std::wstring ExtractNewProjName (const TYPE_ProjectTypes_ChangedProjName& r)
  {
    Generic nm (r.get_newn ());
    if (nm.IsRecord ())
      return ExtractFileName (nm);
    else
      return L"";
  }
};

std::wstring PTAUX::ExtractOldProjName (const TYPE_ProjectTypes_ChangedProjName& r)
{
  return Values::ExtractOldProjName (r);
}

std::wstring PTAUX::ExtractNewProjName (const TYPE_ProjectTypes_ChangedProjName& r)
{
  return Values::ExtractNewProjName (r);
}

TYPE_ProjectTypes_TCEnv PTAUX::mk_TCEnv (const Tuple& t)
{
  return Values::mk_TCEnv (t);
}

Tuple PTAUX::ExtractTCEnv (const TYPE_ProjectTypes_TCEnv & e)
{
  return Values::ExtractTCEnv (e);
}

TYPE_ProjectTypes_DepGraph PTAUX::mk_DepGraph (const Record& g)
{
  return Values::mk_DepGraph (g);
}

Record PTAUX::ExtractDepGraph (const TYPE_ProjectTypes_DepGraph& d)
{
  return Values::ExtractDepGraph (d);
}

TYPE_ProjectTypes_SemVal PTAUX::mk_SemVal (const Record& v)
{
  return Values::mk_SemVal(v);
}

Generic PTAUX::ExtractSemVal (const Generic& s)
{
  return Values::ExtractSemVal (s);
}

TYPE_ProjectTypes_FileName PTAUX::mk_FileName (const std::wstring& file)
{
  return Values::mk_FileName (file);
}

TYPE_ProjectTypes_Name PTAUX::mk_Name (const std::wstring& name)
{
  return Values::mk_Name (name);
}

TYPE_ProjectTypes_InfoString PTAUX::mk_InfoString (const std::wstring& name)
{
  return Values::mk_InfoString (name);
}

TYPE_ProjectTypes_ModuleName PTAUX::mk_ModuleName (const std::wstring& file)
{
  return Values::mk_ModuleName (file);
}

Record PTAUX::ExtractModuleAst (const TYPE_ProjectTypes_Module& m)
{
    return Values::ExtractModuleAst (m);
}

//TYPE_ProjectTypes_ModuleName PTAUX::mk_ModuleName (const TYPE_AS_Name& file)
//{
//  return Values::mk_ModuleName (file);
//}

TYPE_ProjectTypes_Module PTAUX::mk_Module (const TYPE_AS_Name & AsName, const Record & AsModule)
{
  return Values::mk_Module(AsName, AsModule);
}

#ifdef VDMSL
TYPE_ProjectTypes_FlatSpec PTAUX::mk_FlatSpec (const TYPE_AS_Name & asName,
                                               const Record & asDefs,
                                               const Record & origin)
{
  return Values::mk_FlatSpec(asName, asDefs, origin);
}
#endif // VDMSL

TYPE_ProjectTypes_CGInfo PTAUX::mk_CGInfo (const Tuple& info)
{
  return Values::mk_CGInfo (info);
}

TYPE_ProjectTypes_ToolCommand PTAUX::mk_ToolCommand (const std::wstring& command)
{
  return Values::mk_ToolCommand (Token(command));
}

TYPE_ProjectTypes_ToolCommand PTAUX::mk_ToolCommand (const std::string& command)
{
  return mk_ToolCommand (TBWSTR::MakeAsciiWstring(command));
}

TYPE_ProjectTypes_ErrMsg PTAUX::mk_ErrMsg(const TYPE_CI_FileId & f,
                                          const TYPE_CI_Line & l,
                                          const TYPE_CI_Column & c,
                                          const type_cLL & m)
{
  return Values::mk_ErrMsg(f, l, c, m);
}

TYPE_ProjectTypes_BreakNameSet PTAUX::mk_BreakNameSet (const TYPE_AS_Name & AsName,
                                                       const Int & num)
{
  return Values::mk_BreakNameSet (AsName, num);
}

TYPE_ProjectTypes_BreakPosSet PTAUX::mk_BreakPosSet (const std::wstring & name,
                                                     const Int & line,
                                                     const Int & col,
                                                     const Int & num)
{
  return Values::mk_BreakPosSet (name, line, col, num);
}

TYPE_ProjectTypes_BreakEnable PTAUX::mk_BreakEnable (const Int & num)
{
  return Values::mk_BreakEnable (num);
}

TYPE_ProjectTypes_BreakDisable PTAUX::mk_BreakDisable (const Int & num)
{
  return Values::mk_BreakDisable (num);
}

TYPE_ProjectTypes_BreakRemove PTAUX::mk_BreakRemove (const Int & num)
{
  return Values::mk_BreakRemove (num);
}

TYPE_ProjectTypes_BackTrace PTAUX::mk_BackTrace (const Sequence & bt_l)
{
  return Values::mk_BackTrace(bt_l);
}

TYPE_ProjectTypes_BtGotoLevel PTAUX::mk_BtGotoLevel (int level)
{
  return Values::mk_BtGotoLevel(level);
}

int PTAUX::ExtractBtGotoLevel (const TYPE_ProjectTypes_BtGotoLevel & elm)
{
  return Values::ExtractBtGotoLevel (elm);
}

TYPE_ProjectTypes_DependInfo PTAUX::mk_DependInfo (const TYPE_ProjectTypes_ModuleName & name,
                                                   const SET<TYPE_ProjectTypes_ModuleName> & supers,
                                                   const SET<TYPE_ProjectTypes_ModuleName> & subs,
                                                   const SET<TYPE_ProjectTypes_ModuleName> & uses,
                                                   const SET<TYPE_ProjectTypes_ModuleName> & used,
                                                   const Bool & completed)
{
  return Values::mk_DependInfo(name, supers, subs, uses, used, completed);
}

TYPE_ProjectTypes_DependUpdate PTAUX::mk_DependUpdate (const SET<TYPE_ProjectTypes_ModuleName> & modules)
{
  return Values::mk_DependUpdate(modules);
}

TYPE_ProjectTypes_DrawInhTree PTAUX::mk_DrawInhTree(const Sequence & node_line_s)
{
  return Values::mk_DrawInhTree(node_line_s);
}

TYPE_ProjectTypes_ClearInhTree PTAUX::mk_ClearInhTree()
{
  return Values::mk_ClearInhTree();
}

TYPE_ProjectTypes_ClearDebugWindow PTAUX::mk_ClearDebugWindow()
{
  return Values::mk_ClearDebugWindow();
}

TYPE_ProjectTypes_UpdateSelections PTAUX::mk_UpdateSelections()
{
  return Values::mk_UpdateSelections();
}

TYPE_ProjectTypes_Node PTAUX::mk_Node(const Int & x, const Int & y, const Token & nm)
{
  return Values::mk_Node(x, y, nm);
}

TYPE_ProjectTypes_Line PTAUX::mk_Line(const Int & x1, const Int & x2, const Int & y1, const Int & y2)
{
  return Values::mk_Line(x1, x2, y1, y2);
}

bool PTAUX::is_ErrMsg(const TYPE_ProjectTypes_Message & em)
{
  return Values::is_ErrMsg(em);
}

bool PTAUX::is_ToolCommand(const Record & toolcom)
{
  return Values::is_ToolCommand(toolcom);
}

bool PTAUX::is_FileName(const Record & r)
{
  return Values::is_FileName(r);
}

TYPE_ProjectTypes_WarnMsg PTAUX::mk_WarnMsg(const TYPE_CI_FileId & f,
                                            const TYPE_CI_Line & l,
                                            const TYPE_CI_Column & c,
                                            const type_cLL & m)
{
  return Values::mk_WarnMsg(f, l, c, m);
}

bool PTAUX::is_WarnMsg(const TYPE_ProjectTypes_Message & wm)
{
  return Values::is_WarnMsg(wm);
}

TYPE_ProjectTypes_InfoMsg PTAUX::mk_InfoMsg(const type_cLL & m)
{
  return Values::mk_InfoMsg(m);
}

#ifdef VDMSL
// ExtractFlatSpec
// flatSpec : ProjectTypes`FlatSpec
// -> AS`Module * Origin
Tuple PTAUX::ExtractFlatSpec (const TYPE_ProjectTypes_FlatSpec & flatSpec)
{
  return Values::ExtractFlatSpec (flatSpec);
}
#endif // VDMSL

std::wstring PTAUX::ExtractFileName (const TYPE_ProjectTypes_FileName & fnm)
{
  return Values::ExtractFileName (fnm);
}

std::wstring PTAUX::ExtractName (const TYPE_ProjectTypes_Name & nm)
{
  return Values::ExtractName (nm);
}

std::wstring PTAUX::ExtractInfoString (const TYPE_ProjectTypes_InfoString & nm)
{
  return Values::ExtractInfoString (nm);
}

std::wstring PTAUX::ExtractModuleName (const TYPE_ProjectTypes_ModuleName& fnm)
{
  return Values::ExtractModuleName (fnm);
}

// mk_AstVal
// AST : AS`DLModule | AS`Module | AS`Class | CPP`Module | PROJECTVAL`FlatStore
// ==> ProjectTypes`AstVal
TYPE_ProjectTypes_AstVal PTAUX::mk_AstVal (const Record & AST)
{
  return Values::mk_AstVal (AST);
}

// ExtractAstVal
// astval : ProjectTypes`AstVal
// ==> AS`DLModule | AS`Module | AS`Class | CPP`Module | PROJECTVAL`FlatStore
Record PTAUX::ExtractAstVal (const TYPE_ProjectTypes_AstVal & astval)
{
  return Values::ExtractAstVal (astval);
}

Record PTAUX::ExtractAstOrFlat (const TYPE_ProjectTypes_AstVal & astval)
{
  return Values::ExtractAstOrFlat (astval);
}

//TYPE_ProjectTypes_AstVal PTAUX::UpdateAstOrFlat (const TYPE_ProjectTypes_AstVal & AstVal,
//                                                 const TYPE_AS_Module & AST)
//{
//  return Values::UpdateAstOrFlat (AstVal, AST);
//}

SEQ<Record> PTAUX::ExtractAstOrFlats (const SEQ<TYPE_ProjectTypes_AstVal> & asts)
{
  SEQ<Record> val_l; // seq of (AS`DLModule | AS`Module | AS`Class | CPP`Module)
  size_t len_asts = asts.Length();
  for (size_t idx = 1; idx <= len_asts; idx++)
    val_l.ImpAppend (Values::ExtractAstOrFlat(asts[idx]));
  return val_l;
}

std::wstring PTAUX::ExtractMessage (const TYPE_ProjectTypes_InfoMsg & msg)
{
  return Values::ExtractMessage (msg);
}

status_type PTAUX::ExtractStatus (const Tuple & FileAndModuleStatus)
{
  return Values::ExtractStatus (FileAndModuleStatus);
}

Generic PTAUX::ExtractCGInfo (const Generic& CGInfo)
{
  return Values::ExtractCGInfo (CGInfo);
}

std::wstring PTAUX::ExtractToolCommand (const TYPE_ProjectTypes_ToolCommand & ToolCommand)
{
  return Values::ExtractToolCommand (ToolCommand);
}

Tuple PTAUX::ExtractBreakNameSet (const TYPE_ProjectTypes_BreakNameSet & bp)
{
  return Values::ExtractBreakNameSet(bp);
}

Tuple PTAUX::ExtractBreakPosSet(const TYPE_ProjectTypes_BreakPosSet & bp)
{
  return Values::ExtractBreakPosSet(bp);
}

int PTAUX::ExtractBreakEnable(const TYPE_ProjectTypes_BreakEnable& bp)
{
  return Values::ExtractBreakEnable(bp);
}

int PTAUX::ExtractBreakDisable(const TYPE_ProjectTypes_BreakDisable& bp)
{
  return Values::ExtractBreakDisable(bp);
}

int PTAUX::ExtractBreakRemove(const TYPE_ProjectTypes_BreakRemove& bp)
{
  return Values::ExtractBreakRemove(bp);
}

Sequence PTAUX::ExtractBackTrace (const TYPE_ProjectTypes_BackTrace& bt)
{
  return Values::ExtractBackTrace(bt);
}

Set PTAUX::ExtractFunctionSet (const TYPE_ProjectTypes_FunctionSet& fn)
{
  return Values::ExtractFunctionSet(fn);
}

// ExtractMessageAll
// file : seq1 of char
// msg : ProjectTypes`ErrMsg | ProjectTypes`WarnMsg
std::wstring PTAUX::ExtractMessageAll(const std::wstring & file, const TYPE_ProjectTypes_Message & msg)
{
  std::wstring messageString;

  if (file != std::wstring(L""))
    messageString = file + L", ";

  Tuple t (Values::ExtractErrMsg(msg));
  int fileid = t.GetIntValue(1);
  int lineid = t.GetIntValue(2);
  int columnid = t.GetIntValue(3);

  if ((lineid > 0) && (columnid > 0))
  {
    std::wstring lid (Int(lineid).ascii());    
    std::wstring cid (Int(columnid).ascii());
    if (fileid == -10 && lineid == 1) {
      messageString += L"c. " + cid ;
    } 
    else {
      messageString += L"l. " + lid + L", c. " + cid;
    }
  }
  else
    messageString += L"No precise position information available";
  
  const type_cLL & descr_l (t.GetSequence(4));
  messageString += (descr_l.IsEmpty() ? L"\n" : L":\n"); 
  size_t len_descr_l = descr_l.Length();
  for (size_t idx = 1; idx <= len_descr_l; idx++)
  {
    std::wstring ws = L"  " + descr_l[idx].GetString() + L"\n";
    messageString.append(ws) ;
  }
  return messageString;
}

// ModuleName2ASName converts ModuleNames to ASName as they
// are represented in code generated code.
TYPE_AS_Name PTAUX::ModuleName2ASName (const TYPE_ProjectTypes_ModuleName & modnm)
{
  TYPE_AS_Ids nm_l;
  nm_l.ImpAppend (modnm.get_nm());
  return ASTAUX::MkNameFromIds (nm_l, NilContextId);
}
 
TYPE_ProjectTypes_ModuleName PTAUX::ASName2ModuleName (const TYPE_AS_Name & asnm)
{
  const TYPE_AS_Ids & ids (asnm.GetSequence(pos_AS_Name_ids));
  std::wstring s (ids[1].GetString());
  return mk_ModuleName (s);
}

SET<TYPE_ProjectTypes_ModuleName> PTAUX::ASNameSet2ModuleNameSet (const SET<TYPE_AS_Name> & asnm_s)
{
  SET<TYPE_AS_Name> asnm_s_q (asnm_s);
  // Transform set of AS`Name to set of ModuleName
  SET<TYPE_ProjectTypes_ModuleName> returnset_s;
  Generic nm;
  for (bool bb = asnm_s_q.First (nm); bb; bb = asnm_s_q.Next (nm)) {
    returnset_s.Insert (ASName2ModuleName (nm));
  }
  return returnset_s;
}

SET<TYPE_AS_Name> PTAUX::ModuleNameSet2ASNameSet (const SET<TYPE_ProjectTypes_ModuleName> & modnm_s)
{
  SET<TYPE_ProjectTypes_ModuleName> modnm_s_q (modnm_s);
  // Transform set of ModuleName to set of AS`Name
  SET<TYPE_AS_Name> returnset_s;
  Generic nm;
  for (bool bb = modnm_s_q.First (nm); bb; bb = modnm_s_q.Next (nm)) {
    returnset_s.Insert (ModuleName2ASName (nm));
  }
  return returnset_s;
}


bool PTAUX::isEverythingTypeCorrect = false;

bool PTAUX::IsEverythingTypeCorrect()
{
  return PTAUX::isEverythingTypeCorrect;
}

void PTAUX::SomethingIsTypeIncorrect()
{
  PTAUX::isEverythingTypeCorrect = false;
}

void PTAUX::UpdateIsEverythingTypeCorrect()
{
  bool isTypeCorrect = true;
  SET<TYPE_ProjectTypes_ModuleName> mod_s (ToolMediator::Repos()->vdm_AllModules());
  Generic modnm;
  for (bool bb = mod_s.First(modnm); bb && isTypeCorrect; bb = mod_s.Next(modnm))
  {
    status_type st = ExtractStatus(ToolMediator::Repos()->vdm_Status(modnm));
      
    switch (st.syntax) {
      case status_type::status_error:
      case status_type::status_none:
          isTypeCorrect = false;
      break;
      default:
      break;
    }

    if(!isTypeCorrect) continue;

    switch (st.type) {
      case status_type::status_error:
      case status_type::status_none:
        isTypeCorrect = false;
      break;
      default:
      break;
    }
  }
  PTAUX::isEverythingTypeCorrect = isTypeCorrect;
}

//first sort on ERROR < WARNING then sort on file number then on line number.`
// Callled from QS (QuickSort)
// Compare
// tp1 : ProjectTypes`ErrMsg | ProjectTypes`WarnMsg
// tp2 : ProjectTypes`ErrMsg | ProjectTypes`WarnMsg
// sep : bool
// +> nat
int PTAUX::Compare (const TYPE_ProjectTypes_Message & tp1,
                    const TYPE_ProjectTypes_Message & tp2,
                    bool sep)
{
  if (sep) {
    if (PTAUX::is_ErrMsg(tp1) && PTAUX::is_WarnMsg(tp2))
      return 0;
    else if (PTAUX::is_ErrMsg(tp2) && PTAUX::is_WarnMsg(tp1))
      return 2;
  }

  // fid
  int f1 = tp1.GetIntValue (1);
  int f2 = tp2.GetIntValue (1);

  if (f1 < f2)
    return 0;
  else if (f1 == f2)
  {
    // line
    int l1 = tp1.GetIntValue (2);
    int l2 = tp2.GetIntValue (2);
    if (l1 < l2)
      return 0;
    else if (l1 == l2)
    {
      // col
      int c1 = tp1.GetIntValue (3);
      int c2 = tp2.GetIntValue (3);

      if (c1 < c2)
        return 0;
      else if (c1 == c2)
        return 1;
      else
        return 2;
    }
    else
      return 2;
  }
  else
    return 2;
}

//QS implements the QuickSort algorithm and sorts
//error messages (syntax and type errors).
// QS
// sq : seq of (ProjectTypes`ErrMsg | ProjectTypes`WarnMsg) 
// +> seq of (ProjectTypes`ErrMsg | ProjectTypes`WarnMsg) 
SEQ<TYPE_ProjectTypes_Message> PTAUX::QS (const SEQ<TYPE_ProjectTypes_Message> & sq, bool sep)
{
  if (sq.Length() <= 1)
    return sq;
  else
  {
    TYPE_ProjectTypes_Message half (sq[sq.Length() / 2]);
    SEQ<TYPE_ProjectTypes_Message> sq1, sq2, sq3;
    size_t len_sq = sq.Length();
    for (size_t i = 1; i <= len_sq; i++)
    {
      const TYPE_ProjectTypes_Message & mes (sq[i]);
      switch (Compare (sq[i], half, sep)) {
        case 0: {
          sq1.ImpAppend(mes);
          break;
        }
        case 1: {
          sq2.ImpAppend(mes);
          break;
        }
        case 2: {
          sq3.ImpAppend(mes);
          break;
        }
      }
    }
    SEQ<TYPE_ProjectTypes_Message> sorted_res (QS (sq1, sep));
    sorted_res.ImpConc (sq2);
    sorted_res.ImpConc (QS (sq3, sep));
    return sorted_res;
  }
}

