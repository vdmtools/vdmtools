/***
 *  * WHAT
 *  *    Function for testing the static semantics checker.
 *  *    Implementation of zzz.vdm 1.6.2.1
 *  * FILE
 *  *    $Source: /home/vdmtools/cvsroot/toolbox/code/statsem/test-vdmsl.cc,v $
 *  * VERSION
 *  *    $Revision: 1.47 $
 *  * DATE
 *  *    $Date: 2005/01/21 03:16:06 $
 *  * STATUS
 *  *    $State: Exp $
 *  * REFERENCES
 *  *    
 *  * PROJECT
 *  *    Afrodite - ESPRIT programme, project no. 6500.
 *  * AUTHOR
 *  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
 *  * COPYRIGHT
 *  *    (C) Kyushu University
 ***/

#include <stdlib.h>		// prototypes for exit, srand and getopt
#include <iostream>
#include <stdio.h>              // For sprintf.
#include "vdmpars.h"
#include "pexpr.h"
#include "date.h"
#include "statsem.h"
#include "tbutils.h"

#include "projectval.h"
#include "Errors.h"
#include "BatchErr.h"
#include "ToolMediator.h"
#include "ProjectTypes.h"

#include "intconvquotes.h"
#include "tools.h"
#include "settings.h"

extern L"C" {
  extern int getopt(int, wchar_t*const*, const wchar_t*);
  extern wchar_t *optarg; 
  extern int optind;
}


static ofstream outstr(L"m4pp");

Sequence StatSemErrors;


static Token current_class;
static Map Classes;
static StatSem SS;

class AsciiInterface : public vdm_Interface
{
public:
  virtual void vdm_ChangedFile() {}
  virtual void vdm_ChangedMod() {}

  virtual void vdm_Log(const type_cL & msg) {
    vdm_log << Seq2Str (msg) << L"\n";
  }

  virtual void vdm_Error(const type_cL & msg) {
    vdm_err << Seq2Str (msg) << L"\n";
  }

  virtual void vdm_ExecuteMainLoop () {}
  virtual void vdm_CallBack (const Record & /*cmd*/) {}
};

class AsciiErrors : public vdm_Errors
{
private:
  virtual wstring GetFileName(int fileid)
  {
    vdm_ToolMediator * med = ObjGet_vdm_ToolMediator (vdm_mediator);
    Generic file = med->vdm_GetFileName (fileid);
    if (file.IsRecord () && Record(file).Is(vdm_FileName))
      return ExtractFileName(file);
    else
      return L"";
  }
  
public:
  virtual void vdm_SetFilePosition(const Record& msg)
  {
    int fileid = Int (msg.GetField (pos_ErrMsg_fid)).GetValue();
    //    int lineid = Int (msg.GetField (pos_ErrMsg_line)).GetValue();
    int columnid = Int (msg.GetField (pos_ErrMsg_col)).GetValue();

    wstring file = GetFileName (fileid);

    if (fileid != 0)
      vdm_err << L"c." << columnid << " in the command-line\n";
  }

  virtual void vdm_ShowMsg() {
    Record msg = vdm_msgs[vdm_msgPtr];
    vdm_SetFilePosition(msg);
    wstring file = GetFileName (Int (msg.GetField (pos_ErrMsg_fid))); 
    vdm_err << ExtractMessageAll(file, msg);
  }
  
  virtual void vdm_MakeStatus(const Int& NoErr, const Int& NoWarn)
  {
    wstring msg;
    if (NoErr > 0)
      msg = L"Errors: " + NoErr.ascii();

    if (NoWarn > 0)
    {
      if (NoErr > 0) msg += L" ";
      msg += L"Warnings: " + NoWarn.ascii();
    }
    
    if ((NoErr > 0) || (NoWarn > 0))
    {
      vdm_err << msg << endl << flush;
    }
  }
};

int EvalSpec(wstring fname, int POS_WANTED, vdm_Errors* errs, Sequence& res_asts, ContextInfo& ci)
{  
//  Sequence tmp;
//  SyntaxErrors = tmp;

  ifstream sp(fname.c_str());
  if (!sp)
    {  vdm_log << L"Couldn't open specification file: \"" << fname << "\"\n";
       return 0;
     }

  if (orderly != NULL)
    delete orderly;
  orderly = new Sequence ();

  if (modorder != NULL)
    delete modorder;
  modorder = new Sequence ();
  
  LEXINPUT.ReadFrom (sp);
  vdm_log << L"Parsing and installing \"" << fname << "\" ...\n";
  int parse_error = pars_vdm (POS_WANTED,
                              0,
                              errs,
                              res_asts,
//                              &(ci.GetTokenList(CI_FID_TMP_FILE)), 
                              ci,
                              false,
                              TYPE_CI_FileId(CI_FID_TMP_FILE));

  if (parse_error) {
    vdm_log << L"Some error occured during the parsing of: "
         << fname << L"\n";
    return 0;
  }
  else {
    Generic g;
    for (bool bb = res_asts.First(g); bb; bb = res_asts.Next(g)) {
      Record def (g);
      Generic n (def.GetField (1));
      if (n.IsRecord ()) {
	Record nm (n);
	Token class_name (Sequence (nm.GetField (1)) [1]);
	current_class = class_name;
	if (Classes.Dom().InSet(class_name))
	  vdm_log << L"Redefining class " << class_name.ascii()
	       << L"\n";
	Classes.ImpModify(class_name, g);
      }
    }
    return 1;
  }
}


int EvalArg (wstring argname, vdm_Errors * errs, Sequence& res, ContextInfo &ci)
{
  ifstream sp(argname.c_str());
  if (!sp)
    {  vdm_log << L"Couldn't open argument file: \"" << argname << "\"\n";
       return 0;
     }

  if (orderly != NULL)
    delete orderly;
  orderly = new Sequence ();

  if (modorder != NULL)
    delete modorder;
  modorder = new Sequence ();

  LEXINPUT.ReadFrom (sp);

  errs->vdm_ClearAll();
  int parse_error = pars_vdm_expr (1,
                                   errs,
                                   res, 
                                   ci,
                                   false,
                                   TYPE_CI_FileId(CI_FID_TMP_FILE));

  errs->vdm_Done();
  errs->vdm_AllDone();
  
  if (parse_error) {
    vdm_log << L"Some error occured during parsing of argument file\n";
    return 0;
  }
  else // exprs->Hd() contains the expression
    return 1;
}

Tuple CreateRes2(wstring s, const Sequence& seq)
{
  Tuple res(2);
  res.SetField(1, Quote (s));
  res.SetField(2, seq);
  return res; 
}

Tuple CreateRes3(wstring s, const Generic& g, const Sequence& seq)
{
  Tuple res(3);
  res.SetField(1, Quote (s));
  res.SetField(2, g);
  res.SetField(3, seq);
  return res; 
}

void print_errmsg (ostream&os, const Record& err,
                   VDMFormatter vg, bool /*simple*/) 
{
  // Tag == TAG_TYPE_ProjectTypes_ErrMsg means printing an error message.
  // The format is the following:
  // 
  //   err      :: seq of wchar_t (the description of the error),
  //                           (implemented as a Token)
  //   severity :: <ERROR>
  
  os << L"mk_ERR`ErrMsg(\n";
  Sequence descr_l = err.GetField (4);
  Token descr_t(descr_l.Hd());
  vg.IncrementIndent();
  vg.print_spaces(os, vg.GetIndent());
  os << 'L"' << descr_t.GetValue() << "\",\n"; 
  vg.print_spaces(os, vg.GetIndent());
  os << Quote(L"ERROR") << " )";
}

void print_warnmsg (ostream&os, const Record& err,
                    VDMFormatter vg, bool /*simple*/) 
{
  // Tag == TAG_TYPE_ProjectTypes_ErrMsg means printing an error message.
  // The format is the following:
  // 
  //   err      :: seq of wchar_t (the description of the error),
  //                           (implemented as a Token)
  //   severity :: <WARNING>
  
  os << L"mk_ERR`ErrMsg(\n";
  Sequence descr_l = err.GetField (4);
  Token descr_t(descr_l.Hd());
  vg.IncrementIndent();
  vg.print_spaces(os, vg.GetIndent());
  os << 'L"' << descr_t.GetValue() << "\",\n"; 
  vg.print_spaces(os, vg.GetIndent());
  os << Quote(L"WARNING") << " )";
}

void init_ErrMsg()
{
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_ProjectTypes_ErrMsg, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_ProjectTypes_ErrMsg, 4, L"ERR`ErrMsg");
  VDMGetDefaultRecInfoMap().SetPrintFunction(TAG_TYPE_ProjectTypes_ErrMsg, 4, print_errmsg);
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_ProjectTypes_WarnMsg, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_ProjectTypes_WarnMsg, 4, L"ERR`ErrMsg");
  VDMGetDefaultRecInfoMap().SetPrintFunction(TAG_TYPE_ProjectTypes_WarnMsg, 4, print_warnmsg);
}


void OutputRes(Tuple res)
{
  res = TransformIntegers (res);
  //  int length = res.Length();
  //  res.SetField (length, TransformErrors (res.GetField (length)));

  outstr << res;
  //  res.pr_ascii(outstr);
  vdm_log << L"Actually Result : ";
  // res.pr_ascii(vdm_log);
  vdm_log << res;
  vdm_log << L"\n";
		  
}

#ifdef VDMSL
Record ExtractModule (Record arg, Map mod_l) {

  // Notice that mod_l is a map here, whereas it is a Sequence in zzz.vdm
  Sequence test (arg.GetField(1));
  Record quotelit (test[1]);
  wstring func (Token(quotelit.GetField(1)).GetValue());

  Record nm (test[2]);
  if (func == L"STMT") {
    Sequence ids (nm.GetField(1));
    Generic dom, rng;
    if (ids.Length() == 1) {
      mod_l.First(dom,rng);
      return rng;
    }
    else {
      Token modnm = ids.Hd();
      for (bool bb=mod_l.First(dom,rng); bb; bb=mod_l.Next(dom,rng)) 
	if (Sequence(Record(rng).GetField(1)).Hd() == modnm)
	  return rng;
    }
  }
  else if (func == L"MODULES") {
    Token nm (Id2String(GetFirstId(test[2])));
    Generic rng;
    if (mod_l.DomExists(nm, rng)) {
      cout << L"mod_l(nm) = " << rng << endl;
      return rng;
    }
    vdm_log << L"ExtractModule: No module found" << endl;
    return Record();
  }
  else {
    vdm_err << L"Internal Error: Did you remember to use the -e option?\n";
    exit(-1);
  }
  // Dummy return for VC++
  return Generic(); 
}
#endif VDMSL

#ifdef VDMPP
Tuple ExtractClass (Record arg, Map mod_l) {

  // Notice that mod_l is a map here, whereas it is a Sequence in zzz.vdm
  Sequence test (arg.GetField(1));
  Record quotelit (test[1]);
  wstring func (Token(quotelit.GetField(1)).GetValue());
  Record nm (test[2]);
  Sequence superclasses;
  if (test.Length() == 3) {
    Record setenumexpr (test[3]);
    Sequence nmseq (setenumexpr.GetField(1));
    Generic nm;
    for (bool i = nmseq.First(nm); i; i = nmseq.Next(nm)) {
      Generic rng;
      Token nm2 (Id2String(GetFirstId(nm)));
      if (mod_l.DomExists(nm2, rng))
	superclasses.ImpAppend(rng);
    }
  }

  if (func == L"STMT") {
    // Here the global variable 'current_class' which is the name of
    // the last class in the specification is used. This last class is
    // the actual class being checked.
    
    Tuple tpl(2);
    Sequence ids (nm.GetField(1));
    if (ids.Length() == 1) {
      Generic rng (mod_l[current_class]);
      tpl.SetField(1, rng);
    }
    else {
      Token cls = ids.Hd();
      Generic dom, rng;
      for (bool bb = mod_l.First(dom,rng); bb; bb = mod_l.Next(dom,rng)) {
	Record nm = Record(rng).GetField(1);
	Sequence ids = nm.GetField(1);
	if (ids.Hd() == cls)
	  tpl.SetField(1,rng);
      }
    }
    tpl.SetField(2, superclasses);
    return tpl;
  }
  else if (func == L"CLASSES") {
    Token nm (Id2String(GetFirstId(nm)));
    Tuple res(2);
    Generic dom, rng;
    if (mod_l.DomExists(nm, rng)) {
      res.SetField (1,rng);      
      res.SetField (2,superclasses);
      return res;
    }
    vdm_log << L"ExtractClass: No classes found" << endl;
    return Tuple();
  }
  // Dummy return for VC++
  return Generic(); 
}
#endif // VDMPP

vdm_ToolKit * toolkit = NULL;

int main(int argc, wchar_t *argv[])
{
  vdm_log << L"tsi\n";
  if (argc == 1)
    {  vdm_log << L"Usage: tsi -(pdaes) argfile specfile\n";
    return (2);
    }
  
  int c;
  int pflg = 0, dflg = 0, aflg = 0, errflg = 0,
    eflg = 0, sflg = 0; //, fflg = 0;
  
  Settings.ErrorLevelPRF();
  
  InitModuleTypes();
  toolkit = new vdm_ToolKit ();
  ObjectRef TK (toolkit);
  ObjectRef inf (new AsciiInterface ());
  
  AsciiErrors * errs = new AsciiErrors();
  ObjectRef ERRS (errs);

  AsciiErrors * exprerrs = new AsciiErrors();
  ObjectRef ExprERRS (exprerrs);

  //  The Errors class must be initiated with a state.
  //  This is PromptErr in case of Ascii interface,
  //  and BatchErr in case of GUI or batch version.
    
  ObjectRef batch_state (new vdm_BatchErr());
  errs->vdm_InitState((Generic) batch_state);
  
  exprerrs->vdm_InitState( (Generic) batch_state);
  toolkit->vdm_Init ((Generic) inf,(Generic) ERRS, (Generic) ExprERRS);

  // Initialisation of Record Info Map
  init_ErrMsg();

  while ((c = getopt(argc, argv, L"pdaesf")) != -1)
    {  switch (c)
      {  
      case 'p' :
	if (dflg || aflg)
	  errflg++;
	else 
	  pflg++;
	break;
      case 'd' :
	if (pflg || aflg)
	  errflg++;
	else 
	  dflg++;
	break;
      case 'a' :
	if (pflg || dflg)
	  errflg++;
	else 
	  aflg++;
	break;
      case 'e' :
	if (sflg)
	  errflg++;
	else 
	  eflg++;
	break;
      case 's' :
	if (eflg)
	  errflg++;
	else 
	  sflg++;
	break;
      }
    }
  
  Sequence asts; 
  if (!EvalSpec(argv[4], 1, errs, asts, GetCI()))
    exit(2);
  
#ifdef VDMPP
  SS.UpdateParseEnvWithAST(asts);
#endif //VDMPP
  Sequence sqtmp;
  
  SS.StatSemErrors = Sequence (); // This line is not in zzz.vdm
  
  Sequence exprs; 
  if (!EvalArg(argv[3],errs, exprs, GetCI()))
    exit(2);
  
  if (eflg) {
#ifdef VDMSL   
    if (SS.ExpandModule((Int)POS, Classes[current_class]))
#endif //VDMSL
#ifdef VDMPP
    if (SS.ExpandClass(Int (POS), Classes[current_class]))
#endif //VDMPP
  {
	Tuple res (3);
	Tuple tp_res (2);
	
	Tuple tp (SS.TestFunc(exprs.Hd(), (Int)POS));
	Bool ok_p (tp.GetField (1));
  //  7 Oct. 1998 10:31 -- Jesper K. Pedersen
  //	if (! SS.StatSemErrors.IsEmpty())
  StatSemErrors.ImpAppend (SS.StatSemErrors);
	tp_res.SetField (1, (tp.GetField (2)));
	
	
	SS.StatSemErrors = Sequence ();
	
	tp = (SS.TestFunc(exprs.Hd(), (Int)DEF));
  // 7 Oct. 1998 10:32 -- Jesper K. Pedersen
	// if (! SS.StatSemErrors.IsEmpty())
  StatSemErrors.ImpAppend (SS.StatSemErrors);
	Bool ok_d (tp.GetField (1));
	tp_res.SetField (2, (tp.GetField (2)));
	
	if (ok_p.GetValue())
	  if (ok_d.GetValue()) {
      OutputRes(CreateRes3(L"OK", tp_res, StatSemErrors)); 
    }
	  else {
      OutputRes(CreateRes3(L"DEFTYPEERROR", tp_res, StatSemErrors)); 
    }
	else 
    if (ok_d.GetValue()) {
      OutputRes(CreateRes3(L"POSTYPEERROR", tp_res, StatSemErrors)); 
    }
    else {
	    OutputRes(CreateRes3(L"POSDEFTYPEERROR", tp_res, StatSemErrors)); 
	  }
  }
  else 
    OutputRes(CreateRes3(L"ERROR", Nil(), StatSemErrors));
  }
  else if (sflg) {
#ifdef VDMSL
    Generic gg, rng;
    for (bool bbb = Classes.First(gg,rng); bbb; bbb = Classes.Next(gg,rng))
      SS.UpdateModuleEnv (rng);

    Record ast (ExtractModule(exprs.Hd(), Classes));
#endif // VDMSL
#ifdef VDMPP
    Tuple tmp (ExtractClass(exprs.Hd(), Classes));
    Record ast = tmp.GetField(1);
    Sequence superclasses = tmp.GetField(2);
    Generic sc;
    for (bool bb = superclasses.First(sc); bb; bb = superclasses.Next(sc)) 
      SS.wf_Class(Int(POS),Record(sc));
    SS.StatSemErrors = Sequence ();
    
#endif // VDMPP
    
    Tuple res (2);
    Bool ok_p (SS.TestStmt(ast, exprs.Hd(), (Int)POS));
    StatSemErrors.ImpAppend (SS.StatSemErrors);
    SS.StatSemErrors = Sequence ();
    SS.InitEnv();
    Bool ok_d (SS.TestStmt(ast, exprs.Hd(), (Int)DEF));
    StatSemErrors.ImpAppend (SS.StatSemErrors);
    
    if (ok_p.GetValue())
      if (ok_d.GetValue())	{
        OutputRes(CreateRes2(L"OK", StatSemErrors));                 
      }
      else {
        OutputRes(CreateRes2(L"DEFTYPEERROR", StatSemErrors));                 
      }
    else if (ok_d.GetValue()) {
      OutputRes(CreateRes2(L"POSTYPEERROR", StatSemErrors));                 
    }
    else {
      OutputRes(CreateRes2(L"POSDEFTYPEERROR", StatSemErrors));
    }
  }
  else {
    vdm_log << L"Define e or p flag\n";
    OutputRes(CreateRes3(L"ERROR", Nil(), StatSemErrors)); 
  }
  return 0; 
}
