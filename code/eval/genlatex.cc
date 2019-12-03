/***
*  * WHAT
*  *    Implementation of c++ class for latex generation from
*  *    VDM-SL abstract syntax
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/genlatex.cc,v $
*  * VERSION
*  *    $Revision: 1.134 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:04 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    M.H.G. Verhoef + C. de Bruin
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

///
///   INCLUDE SYSTEM LIBRARIES
///

///
///   INCLUDE LOCAL LIBRARIES
///
#include "tools.h"
#include "genlatex.h"         // include genlatexoutput class definition
#include "asquotes.h"
#include "rt_info.h"          // include Is_Covered
#include "astaux.h"           // include MkName
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "backslash.h"
#include "tb_wstring.h"
#include "libman.h"
#include "settings.h"
#include "tbutils.h"

//
// definition of line feed (\\) in the generated output.
//
#define LF  " \\\\\n"
#define LF0 " \\\\ "
#define LATEXSPACE "\\kw{ }"
#define LATEXSPACE2 "\\hspace{0.5em}"

///
///   PUBLIC FUNCTIONS IN THIS CLASS
///

/////////////////
// Name: GenLatexOutput
// Descr:
//
// The function assumes that the pp-parser just has been called.
// That is, that the orderly and modorder is computed.
// The Opt parameter has the following meaning
//    == 0  => no index
//    == 1  => index (-n option)
//    == 2  => index (-N option) ???
//
// AST:
// Is the set of asts that are contained in the specification
// manager, and not the asts just parsed by the pp-parser to
// to update the orderly and modorder parts.
/////////////////

GenLatexOutput::GenLatexOutput (const std::wstring & OutputFilename,  // output file name
                                const Set & AST,                      // the ast`s to pp
                                int Opt,                              // Should Latex index be generated
                                const Sequence & fns,                 // list of set of (fct|op)
                                const std::wstring & thefile,         // the file to be pretty printed
                                ContextInfo & ci)   // The contextinfo table to pretty print from.
{
  this->not_covered_area = false;
  this->linefeed = true;
  this->post_condition_scope = false;

  ////////////////////
  // Open file
  ////////////////////

  std::wstring Fname (OutputFilename);
  this->contextinfo = &ci;

  this->LatexOutput.close();
  // LatexOutput is global (yuck!) so we better clear its error state
  // otherwise open fails on win32.
  this->LatexOutput.clear();
  this->LatexOutput.open(TBWSTR::wstring2fsstr(Fname).c_str()); //, io_writeonly, a_create);

  this->INDEX = Opt;

  if (this->LatexOutput.good()) {
    // do the conversion stuff
    // we have to convert either an AST with a single definitions block
    // or a number of modules.

    Record tmp_ast (AST.GetElem());

    Record mod_name (tmp_ast.GetField(1));

    // Is it a flat spec?
    // In this case use orderly, modorder is not initialised!
#ifdef VDMSL
    if (mod_name == ASTAUX::GetDefaultModASName()) {
      //////////////////////
      // AST is list of definitions
      //////////////////////

      // init global variables
      this->Fns = fns.Hd();
      //this->current_ast = TYPE_AS_Definitions (tmp_ast.GetField(3));
      const TYPE_AS_Definitions & defs (tmp_ast.GetRecord(3));
      //this->current_module = Nil ();

      ///////////////////
      // Generate defined items in the order they are defined in
      // the flat specification
      ///////////////////

      GenDefinitions (Nil(), defs, VDMPARSER::orderly, false);
    }
    else
#endif // VDMSL
    {
      //////////////////////////
      // AST is list of modules/classes
      //////////////////////////

      size_t len_modorder = VDMPARSER::modorder.Length();
      for (size_t i = 1; i <= len_modorder; i++) {
        /////////////////
        // Get the next ast corresponding to
        // the next module in the file
        /////////////////
        Record name (((Record) (VDMPARSER::modorder.Index(i))).GetField(3));
        Record ast;
        Set AST_q (AST); 
        Generic ast_g;
        bool exists = false;
        for ( bool bb = AST_q.First(ast_g); bb && !exists; bb = AST_q.Next(ast_g) ) {
          if (name == ((Record) ast_g).GetField(1)) {
            exists = true;
            ast = ast_g;
          }
        }

        if (!exists) {
          std::wstring Msg =
#ifdef VDMSL
            L"Module Name :"
#endif // VDMSL
#ifdef VDMPP
            L"Class Name :"
#endif // VDMPP
            + ASTAUX::ASName2String(name) + L"was not found in file :" + thefile + L"\n";
          ErrMsg1( Record (), Msg);
        }
#ifdef VDMSL
        switch(ast.GetTag()) {
          case TAG_TYPE_AS_Module: {
            this->Fns = fns.Index(i);
            GenModule (ast, VDMPARSER::modorder.Index (i));
            break;
          }
          case TAG_TYPE_AS_DLModule: {
            GenDLModule (ast, VDMPARSER::modorder.Index (i));
            break;
          }
        }
#endif // VDMSL
#ifdef VDMPP
        this->Fns = Set();
        GenClass (ast, VDMPARSER::modorder.Index (i));
#endif // VDMPP
      }
    }
    LatexOutput.flush();
    LatexOutput.close();
  }
  else {
    std::wstring Msg = L"Could not open outputfile ";
    Msg += Fname;
    ErrMsg1 (Record (), Msg);
  }
}

GenLatexOutput::~GenLatexOutput ()
{
  LatexOutput.close();
}

////////////////////////////
// Name : GenLatexOutput::ErrMsg1
// Descr:
//   Error message display routine
/////////////////////

void GenLatexOutput::ErrMsg1(const Record & ast, // the item that caused the error
                             const std::wstring & Msg)  // the error message
{
  Sequence descr_l;
  descr_l.ImpAppend (SEQ<Char>(L"Warning : " + Msg));

  //////////////
  // Position information
  /////////////
  int fid = 0, line = 0, col = 0;
  Tuple gfp (GetCI().GetFilePos(ASTAUX::GetCid(ast)));
  if (gfp.GetBool(1)) {
    fid = gfp.GetInt(2);
    TYPE_CI_TokenPos astpos (gfp.GetField(4));
    line = astpos.get_abs_uline().GetValue();
    col  = astpos.get_column().GetValue();
  }
  else {
    descr_l.ImpAppend (SEQ<Char>(L"(Imprecise position information.)"));
  }

  //////////////
  // Write Message
  //////////////

//  TYPE_ProjectTypes_WarnMsg err_rc (PTAUX::mk_WarnMsg(fid, line, col, descr_l));
  TYPE_ProjectTypes_WarnMsg err_rc;
  err_rc.Init(Int(fid), Int(line), Int(col), descr_l);
  ToolMediator::Errs()->vdm_AddMsg(err_rc);
}

#ifdef VDMSL
/** Module Part **/

/////////////////////////////////
// Name: GenModule
// Descr:
//
// The deforder argument has two fields:
// 1) is the orderly sequence from the parser
// 2) is a bool indicating if any Tex follows the current module
/////////////////////////////////

void GenLatexOutput::GenModule (const TYPE_AS_Module & mod_ast,  // The module (AST)
                                const Record & deforder) // see above
{
  const TYPE_AS_Name & mod_nm (mod_ast.GetRecord(pos_AS_Module_nm));
  const TYPE_AS_Interface & intf (mod_ast.GetRecord(pos_AS_Module_intf));
  const Generic & defs (mod_ast.GetField(pos_AS_Module_defs));

  //this->current_module = mod_nm;

  //////////////
  // Insert TexBreak if module is followed by Tex
  // Needed in order to merge sl-spec with latex-doc-part
  //////////////

  if (deforder.GetBoolValue(2)) {
    LatexOutput << "\n\n##!!#!\n\n";
  }

  //////////////
  // Generate Latex for module
  //////////////

//  LatexOutput << "{\\small\\sf module} $";
  LatexOutput << "\\kModule $";
  GenName (mod_nm, this->LatexOutput);
  LatexOutput << "$";

  ///////////////
  // Generate Index for current module
  ///////////////

  if (this->INDEX) {
    GenIndex (mod_nm, mod_index);
  }
  LatexOutput << endl;

  GenInterface (intf);

  ////////////////
  // Generate definition part of module
  ////////////////

  if (! defs.IsNil ()) {
    //this->current_ast = TYPE_AS_Definitions (defs);
    GenDefinitions (mod_nm, defs, deforder.GetField (1), true);
  }

//  LatexOutput << "{\\small\\sf end} {\\it ";
//  GenName (mod_nm);
//  LatexOutput << "}";
  LatexOutput << "\\kEnd $";
  GenName (mod_nm, this->LatexOutput);
  LatexOutput << "$";

  LatexOutput << endl;
  LatexOutput << endl;
  LatexOutput.flush ();
}

void GenLatexOutput::GenDLModule (const TYPE_AS_DLModule & mod_ast, // the dl module
                                  const Record & deforder) // see GenModule
{
  const TYPE_AS_Name & mod_nm (mod_ast.GetRecord(pos_AS_DLModule_nm));
  const TYPE_AS_DLInterface & intf (mod_ast.GetRecord(pos_AS_DLModule_intf));
  const Generic & useslib (mod_ast.GetField(pos_AS_DLModule_useslib));

  //this->current_module = mod_nm;

  //////////////
  // Insert TexBreak if dlmodule is followed by Tex
  // Needed in order to merge sl-spec with latex-doc-part
  //////////////

  if (deforder.GetBoolValue(2)) {
    LatexOutput << "\n\n##!!#!\n\n";
  }

  //////////////
  // Generate Latex for dlmodule
  //////////////

//  LatexOutput << "{\\small\\sf dlmodule} $";
  LatexOutput << "\\kDLModule $";
  GenName (mod_nm, this->LatexOutput);
  LatexOutput << "$";

  ///////////////
  // Generate Index for current module
  ///////////////

  if (this->INDEX) {
    GenIndex (mod_nm, mod_index);
  }
  LatexOutput << endl;

  GenDLInterface (intf);

  if (!useslib.IsNil()) {
    LatexOutput << "\\begin{expr}";
    LatexOutput << endl;
    LatexOutput << "\\kw{uselib }";
    GenLiteral(useslib, this->LatexOutput);
    LatexOutput << "\\end{expr}";
    LatexOutput << endl;
  }
//  LatexOutput << "{\\small\\sf end} {\\it ";
//  GenName (mod_nm);
//  LatexOutput << "}";
  LatexOutput << "\\kEnd $";
  GenName (mod_nm, this->LatexOutput);
  LatexOutput << "$";

  LatexOutput << endl;
  LatexOutput << endl;
  LatexOutput.flush ();
}

void GenLatexOutput::GenInterface (const TYPE_AS_Interface & intf)
{
  const Generic & mod_exp (intf.GetField(pos_AS_Interface_exp));
  const Map &     mod_imp (intf.GetMap(pos_AS_Interface_imp));

  if (! mod_imp.IsEmpty ()) {
    LatexOutput << "\\begin{interface}{imports}";
    LatexOutput << endl;
    Set dom_mod_imp (mod_imp.Dom());
    Generic elm;
    for (bool bb = dom_mod_imp.First (elm); bb; bb = dom_mod_imp.Next (elm)) {
      GenImportSig (elm, mod_imp[elm], false);
    }
    LatexOutput << "\\end{interface}";
    LatexOutput << endl;
  }

  GenExportSig (mod_exp);
}

void GenLatexOutput::GenDLInterface (const TYPE_AS_DLInterface & intf)
{
  const Map & mod_imp (intf.GetMap(pos_AS_DLInterface_imp));
  const TYPE_AS_DLExportSig & mod_exp (intf.GetRecord(pos_AS_DLInterface_exp));

  if (! mod_imp.IsEmpty ()) {
    LatexOutput << "\\begin{interface}{imports}";
    LatexOutput << endl;
    Set dom_mod_imp (mod_imp.Dom());
    Generic elm;
    for (bool bb = dom_mod_imp.First (elm); bb; bb = dom_mod_imp.Next (elm)) {
        GenDLImportSig (elm, mod_imp[elm], false);
    }
    LatexOutput << "\\end{interface}";
    LatexOutput << endl;
  }

  GenDLExportSig (mod_exp);
}

void GenLatexOutput::GenImportSig (const TYPE_AS_Name & mod_id, const Generic & isig, bool ininst)
{
  if (! ininst) {
    LatexOutput << "\\begin{from}{";
    GenName (mod_id, this->LatexOutput);
    LatexOutput << "}";
    LatexOutput << endl;
  }

  if (isig.IsNil ()) {
    if (ininst) {
      LatexOutput << "\\ ";
    }
    LatexOutput << "\\kw{all}";
    LatexOutput << endl;
  }
  else {
    TYPE_AS_ImportSig imp_sig (isig);
    Map tps (imp_sig.GetMap(pos_AS_ImportSig_tps));
    Map val (imp_sig.GetMap(pos_AS_ImportSig_val));
    Map fns (imp_sig.GetMap(pos_AS_ImportSig_fns));
    Map ops (imp_sig.GetMap(pos_AS_ImportSig_ops));
    Map ren (InverseMap (imp_sig.GetMap(pos_AS_ImportSig_ren)));

    if (! tps.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{types}";
      LatexOutput << endl;

      Generic elm;
      bool bb = tps.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);

        GenRenaming (elm, ren);
        bb = tps.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! val.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{values}";
      LatexOutput << endl;

      Generic elm;
      bool bb = val.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        Generic val_imp (val[elm]);
        if (! val_imp.IsNil ()) {
          LatexOutput << ":";
          GenType (val_imp);
        }
        GenRenaming (elm, ren);
        bb = val.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! fns.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{functions}";
      LatexOutput << endl;

      Generic elm;
      bool bb = fns.First (elm);
      while (bb) {
        Generic fn_imp (fns[elm]);
        GenName (elm, this->LatexOutput);
        if (! fn_imp.IsNil ()) {
          Sequence tpparms (Tuple(fn_imp).GetField (1));
          Record fnt (Tuple(fn_imp).GetField (2));
          size_t len = tpparms.Length ();
          if (len != 0) {
            LatexOutput << "[";
            for (size_t bb = 1; bb <= len; bb++) {
              TYPE_AS_TypeVar tv (tpparms[bb]);
              GenTypeVar (tv);
              if (bb < len) {
                LatexOutput << ",";
              }
            }
            LatexOutput << "]";
          }
        
          LatexOutput << ":";
          GenType (fnt);
        }
        GenRenaming (elm, ren);
        bb = fns.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! ops.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{operations}";
      LatexOutput << endl;
      Generic elm;
      bool bb = ops.First (elm);
      while (bb) {
        Generic op_imp (ops[elm]);
        GenName (elm, this->LatexOutput);
        if (! op_imp.IsNil ()) {
          LatexOutput << ":";
          GenType (op_imp);
        }
        GenRenaming (elm, ren);
        bb = ops.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }
  }

  if (! ininst) {
    LatexOutput << "\\end{from}";
    LatexOutput << endl;
  }

  LatexOutput.flush ();
}

void GenLatexOutput::GenDLImportSig (const TYPE_AS_Name & mod_id, const Generic & isig, bool ininst)
{
  if (! ininst) {
    LatexOutput << "\\begin{from}{";
    GenName (mod_id, this->LatexOutput);
    LatexOutput << "}";
    LatexOutput << endl;
  }

  TYPE_AS_DLImportSig imp_sig (isig);
  Map tps (imp_sig.GetMap(pos_AS_DLImportSig_tps));

  if (! tps.IsEmpty ()) {
    LatexOutput << "\\begin{modulesignature}{types}";
    LatexOutput << endl;

    Generic elm;
    bool bb = tps.First (elm);
    while (bb) {
      GenName (elm, this->LatexOutput);

      bb = tps.Next (elm);
      if (bb) {
        LatexOutput << ";" << LF;
      }
    }
    LatexOutput << endl;
    LatexOutput << "\\end{modulesignature}";
    LatexOutput << endl;
  }

  if (! ininst) {
    LatexOutput << "\\end{from}";
    LatexOutput << endl;
  }

  LatexOutput.flush ();
}

void GenLatexOutput::GenDLExportSig (const Generic & esig)
{
  LatexOutput << "\\begin{interface}{exports}";
  LatexOutput << endl;

  if (esig.IsNil ()) {
    LatexOutput << "\\kw{all}";
    LatexOutput << endl;
  }
  else {
    TYPE_AS_DLExportSig exp_sig (esig);
    Map val (exp_sig.GetMap(pos_AS_DLExportSig_val));
    Map fns (exp_sig.GetMap(pos_AS_DLExportSig_fns));
    Map ops (exp_sig.GetMap(pos_AS_DLExportSig_ops));

    if (! val.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{values}";
      LatexOutput << endl;

      Generic elm;
      bool bb = val.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        LatexOutput << ":";
        GenType (val[elm]);
        bb = val.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! fns.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{functions}";
      LatexOutput << endl;

      Generic elm;
      bool bb = fns.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        Record fnt (fns[elm]);
        LatexOutput << ":";
        GenType (fnt);
        bb = fns.Next (elm);
        if (bb)
          LatexOutput << ";" << LF;
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! ops.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{operations}";
      LatexOutput << endl;

      Generic elm;
      bool bb = ops.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        LatexOutput << ":";
        GenType (ops[elm]);
        bb = ops.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

  }
  LatexOutput << "\\end{interface}";
  LatexOutput << endl;
  LatexOutput.flush ();
}

void GenLatexOutput::GenExportSig (const Generic & esig)
{
  LatexOutput << "\\begin{interface}{exports}";
  LatexOutput << endl;

  if (esig.IsNil ()) {
    LatexOutput << "\\kw{all}";
    LatexOutput << endl;
  }
  else {
    TYPE_AS_ExportSig exp_sig (esig);
    Map tps (exp_sig.GetMap(pos_AS_ExportSig_tps));
    Map val (exp_sig.GetMap(pos_AS_ExportSig_val));
    Map fns (exp_sig.GetMap(pos_AS_ExportSig_fns));
    Map ops (exp_sig.GetMap(pos_AS_ExportSig_ops));

    if (! tps.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{types}";
      LatexOutput << endl;

      Generic elm;
      bool bb = tps.First (elm);
      while (bb) {
        if (! tps[elm].IsNil ()) {
          LatexOutput << "\\kw{struct}\\ ";
        }
        GenName (elm, this->LatexOutput);
        bb = tps.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! val.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{values}";
      LatexOutput << endl;

      Generic elm;
      bool bb = val.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        LatexOutput << ":";
        GenType (val[elm]);
        bb = val.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! fns.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{functions}";
      LatexOutput << endl;

      Generic elm;
      bool bb = fns.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        Tuple fnselm (fns[elm]);
        Sequence tpparms (fnselm.GetField (1));
        Record fnt (fnselm.GetField (2));
        size_t len = tpparms.Length ();
        if (len != 0) {
          LatexOutput << "[";
          for (size_t bb = 1; bb <= len; bb++) {
            TYPE_AS_TypeVar tv (tpparms[bb]);
            GenTypeVar (tv);
            if (bb < len) {
              LatexOutput << ",";
            }
          }
          LatexOutput << "]";
        }
        LatexOutput << ":";
        GenType (fnt);
        bb = fns.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }

    if (! ops.IsEmpty ()) {
      LatexOutput << "\\begin{modulesignature}{operations}";
      LatexOutput << endl;

      Generic elm;
      bool bb = ops.First (elm);
      while (bb) {
        GenName (elm, this->LatexOutput);
        LatexOutput << ":";
        GenType (ops[elm]);
        bb = ops.Next (elm);
        if (bb) {
          LatexOutput << ";" << LF;
        }
      }
      LatexOutput << endl;
      LatexOutput << "\\end{modulesignature}";
      LatexOutput << endl;
    }
  }

  LatexOutput << "\\end{interface}";
  LatexOutput << endl;
  LatexOutput.flush ();
}
#endif // VDMSL

/////////////////////////////////
// Name: GenClass
// Descr:
//
// The deforder argument has two fields:
// 1) is the orderly sequence from the parser
// 2) is a bool indicating if any Tex follows the current module
/////////////////////////////////

#ifdef VDMPP
// GenClass
void GenLatexOutput::GenClass (const TYPE_AS_Class & ast,      // The clas (AST)
                               const Record & deforder)        // see above
{
  const TYPE_AS_Name & nm (ast.GetRecord(pos_AS_Class_nm));
  const SEQ<TYPE_AS_Name> & supercls (ast.GetSequence(pos_AS_Class_supercls));
  const Generic & defs (ast.GetField(pos_AS_Class_defs));

  //this->current_module = nm;

  //////////////
  // Insert TexBreak if class is followed by Tex
  // Needed in order to merge sl-spec with latex-doc-part
  //////////////

  if (deforder.GetBoolValue(2)) {
    LatexOutput << "\n\n##!!#!\n\n";
  }

  //////////////
  // Generate Latex for class
  //////////////

  if(ast.GetBoolValue(pos_AS_Class_sys)) {
//    LatexOutput << "{\\small\\sf system} $";
    LatexOutput << "\\kSystem $";
  }
  else {
//    LatexOutput << "{\\small\\sf class} $";
    LatexOutput << "\\kClass $";
  }
  GenName (nm, this->LatexOutput);
  LatexOutput << "$";

  size_t len_supercls = supercls.Length ();
  if (len_supercls != 0) {
//    LatexOutput << " {\\small\\sf is subclass of}  $";
    LatexOutput << " \\kISO $";
    for (size_t bb = 1; bb <= len_supercls; bb++) {
      TYPE_AS_Name nm (supercls[bb]);
      GenName (nm, this->LatexOutput);
      if (bb < len_supercls) {
        LatexOutput << ",";
      }
    }
    LatexOutput << "$";
  }

  ///////////////
  // Generate Index for current class
  ///////////////

  if (this->INDEX) {
    GenIndex (nm, class_index);
  }
  LatexOutput << endl;
  LatexOutput << "\\par";
  LatexOutput << endl;

  ////////////////
  // Generate definition part of module
  ////////////////

  if (! defs.IsNil ()) {
    //this->current_ast = TYPE_AS_Definitions (defs);
    GenDefinitions (nm, defs, deforder.GetField (1), false);
  }

//  LatexOutput << "{\\small\\sf end} {\\it ";
//  GenName (nm);
//  LatexOutput << "}";
  LatexOutput << "\\kEnd $";
  GenName (nm, this->LatexOutput);
  LatexOutput << "$";

  LatexOutput << endl;
  LatexOutput << endl;
  LatexOutput.flush ();
}
#endif // VDMPP

/** Definitions part ********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

//////////////////////////
// Name: GenDefinitions
// Descr:
//   Generate definitions in module
//////////////////////////

// GenDefinitions
void GenLatexOutput::GenDefinitions (const Generic & modnm,
                                     const TYPE_AS_Definitions & defs,
                                     const Sequence & order, // the orderly sequence for the module
                                     bool inmod)             // inmod == true => inside a module
                                                             // inmod == false => outside module
{
  if (inmod) {
    LatexOutput << "\\kDefinitions";
    LatexOutput << endl;
  }

  const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tps (defs.GetMap(pos_AS_Definitions_typem));
  const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fns (defs.GetMap(pos_AS_Definitions_fnm));
  const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & ops (defs.GetMap(pos_AS_Definitions_opm));
  SEQ<TYPE_AS_ValueDef> val (defs.GetSequence(pos_AS_Definitions_valuem));
#ifdef VDMSL
  const Generic & sd (defs.GetField(pos_AS_Definitions_State));
#endif // VDMSL
#ifdef VDMPP
  SEQ<TYPE_AS_InstanceVarDef> instvars (defs.GetSequence(pos_AS_Definitions_instvars));
  SEQ<TYPE_AS_SyncDef> syncs (defs.GetSequence(pos_AS_Definitions_syncs));
  const Generic & thread (defs.GetField(pos_AS_Definitions_threaddef));
#endif //VDMPP
  const Map & traces (defs.GetMap(pos_AS_Definitions_tracem));

  int prev_type = 0;
  int next_type = 0;

  //////////////////
  // Use order to reestabish the order in which the
  // the definitions should be printed
  //////////////////

  size_t len_order = order.Length();
  for (size_t i = 1; i <= len_order; i++) {
    ///////////////////
    // Get the type of the item following the current
    // used to determine when type/value/.. should be inserted
    //////////////////

    next_type = TAG_TYPE_PRETTY_PRINTER_Texbreak;
    int64_t n = i+1;
    while (next_type == TAG_TYPE_PRETTY_PRINTER_Texbreak) {
      if (n > order.Length ()) {
        next_type = 0;
      }
      else {
        next_type = ((Tuple) order[n++]).GetIntValue(1);
      }
    }

    /////////////////
    // Generate Latex for definition
    /////////////////

    Tuple what (order[i]);
    int what_type = what.GetIntValue(1);
    Generic construct (what.GetField(2));

    switch(what_type) {
      case TAG_TYPE_AS_TypeDef: {
        StartDef (TAG_TYPE_AS_TypeDef, &prev_type);
        TYPE_AS_Name type_id (construct);
        TYPE_AS_TypeDef type_def (tps[type_id]);
        GenTypeDef (modnm, type_def, next_type == TAG_TYPE_AS_TypeDef);
        break;
      }
#ifdef VDMSL
      case TAG_TYPE_AS_StateDef: {
        StartDef (TAG_TYPE_AS_StateDef, &prev_type);
        GenStateDef (modnm, sd);
        break;
      }
#endif // VDMSL
      case TAG_TYPE_AS_ValueDef: {
        // value definitions are put into the val sequence unordered,
        // therefore, we can just take the head of this sequence, and
        // remove it after being processed.
        StartDef (TAG_TYPE_AS_ValueDef, &prev_type);
        TYPE_AS_ValueDef val_def (val.Hd ());
        GenValueDef (modnm, val_def, next_type == TAG_TYPE_AS_ValueDef);
        val.ImpTl ();
        break;
      }
      case TAG_TYPE_PRETTY_PRINTER_FnDef: {
        StartDef (TAG_TYPE_PRETTY_PRINTER_FnDef, &prev_type);
        TYPE_AS_Name fn_id (construct);
#ifdef VDMSL
        TYPE_AS_FnDef fn_def (fns[fn_id]);
#endif // VDMSL
#ifdef VDMPP
        if (!fns.DomExists(fn_id) && MANGLE::IsMangled(fn_id)) {
          fn_id = MANGLE::GetUnmangledName(fn_id);
        }
        TYPE_AS_FnDef fn_def;
        if (fns.DomExists(fn_id)) {
          fn_def = fns[fn_id];
        }
        else {
          wcout << fn_id << L"is't exists" << endl;
        }
#endif // VDMPP
        GenFunctionDef (modnm, fn_def, next_type == TAG_TYPE_PRETTY_PRINTER_FnDef);
        break;
      }
      case TAG_TYPE_PRETTY_PRINTER_OpDef: {
        StartDef (TAG_TYPE_PRETTY_PRINTER_OpDef, &prev_type);
        TYPE_AS_Name op_id (construct);
#ifdef VDMSL
        TYPE_AS_OpDef op_def (ops[op_id]);
#endif // VDMSL
#ifdef VDMPP
        if (!ops.DomExists(op_id) && MANGLE::IsMangled(op_id)) {
          op_id = MANGLE::GetUnmangledName(op_id);
        }
        TYPE_AS_OpDef op_def;
        if (ops.DomExists(op_id)) {
          op_def = ops[op_id];
        }
        else {
          wcout << op_id << L"is't exists" << endl;
        }
#endif // VDMPP
        GenOperationDef (modnm, op_def, next_type == TAG_TYPE_PRETTY_PRINTER_OpDef);
        break;
      }
#ifdef VDMPP
      // instance variables, synchronisation,
      // and threads are all put into unorderes sequences as
      // value definitions. See comment above.
      case TAG_TYPE_PRETTY_PRINTER_InstVarDef: {
        StartDef (TAG_TYPE_PRETTY_PRINTER_InstVarDef, &prev_type);
        TYPE_AS_InstanceVarDef def (instvars.Hd ());
        GenInstanceVarDef (modnm, def, next_type != TAG_TYPE_PRETTY_PRINTER_InstVarDef);
        instvars.ImpTl ();
        break;
      }
      // synchronisation
      case TAG_TYPE_PRETTY_PRINTER_SyncDef: {
        StartDef (TAG_TYPE_PRETTY_PRINTER_SyncDef, &prev_type);
        TYPE_AS_SyncDef def (syncs.Hd ());
        GenSyncDef (modnm, def, next_type == TAG_TYPE_PRETTY_PRINTER_SyncDef);
        syncs.ImpTl();
        break;
      }
      // thread
      case TAG_TYPE_PRETTY_PRINTER_ThreadDef: {
        StartDef (TAG_TYPE_PRETTY_PRINTER_ThreadDef, &prev_type);
        GenThreadDef (modnm, thread, next_type == TAG_TYPE_PRETTY_PRINTER_ThreadDef); //FIXME - 2nd field not needed
        break;
      }
#endif // VDMPP
      case TAG_TYPE_PRETTY_PRINTER_Traces: {
        StartDef (TAG_TYPE_PRETTY_PRINTER_Traces, &prev_type);
        SEQ<TYPE_AS_Name> name_l (construct);
        SEQ<TYPE_AS_TraceDefTerm> tdt_l (traces[construct]);
        GenTraces(modnm, name_l, tdt_l, next_type == TAG_TYPE_PRETTY_PRINTER_Traces);
        break;
      }
      case TAG_TYPE_PRETTY_PRINTER_Texbreak: {
        LatexOutput << "\n\n##!!#!\n\n";
        break;
      }
      default: {
        LatexOutput << "";
        break;
      }
    }
  }
  LatexOutput.flush ();
}


/** Type Definitions part ***************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenTypeDef (const Generic & modnm, const TYPE_AS_TypeDef & tpdef, bool semicolon)
{
  const TYPE_AS_Name & nm    (tpdef.GetRecord(pos_AS_TypeDef_nm));
  const TYPE_AS_Type & shape (tpdef.GetRecord(pos_AS_TypeDef_shape));
  const Generic & inv        (tpdef.GetField(pos_AS_TypeDef_Inv));
  const Int & access         (tpdef.GetInt(pos_AS_TypeDef_access));

  if (! shape.Is (TAG_TYPE_AS_CompositeType)) {
    LatexOutput << "\\type{";
    GenAccess(access,tpdef,false,false);
    LatexOutput << LATEXSPACE2;
    GenName (nm, this->LatexOutput);

    if (this->INDEX) {
      GenIndex (nm, typedef_index);
    }
    LatexOutput << "}{";
    GenType (shape);
    if (inv.IsNil() && semicolon) {
      LatexOutput << ";";
    }
    LatexOutput << "}";
    if (!inv.IsNil()) {
      LatexOutput << endl;
      GenInv (modnm, inv, semicolon);
    }
  }
  else {
    // TODO: map nat to compose ....
    if (!(nm == shape.GetRecord(pos_AS_CompositeType_name))) {
      LatexOutput << "\\type{";
      GenAccess(access, tpdef, false, false);
      LatexOutput << LATEXSPACE2;
      GenName (nm, this->LatexOutput);

      if (this->INDEX) {
        GenIndex (nm, typedef_index);
      }
      LatexOutput << "}{";
      GenCompositeType (shape, semicolon && inv.IsNil());
      LatexOutput << "}";
    }
    else {
      GenAccess(access,tpdef,false,false);
      //      LatexOutput << "\\kw{ }";
      GenRecordType (shape, semicolon && inv.IsNil());
    }
    if (!inv.IsNil()) {
      LatexOutput << endl;
      GenInv(modnm, inv, semicolon);
    }
  }

  LatexOutput << endl;
  LatexOutput.flush ();
}

void GenLatexOutput::GenType (const TYPE_AS_Type & shape)
{
  switch (shape.GetTag ()) {
    case TAG_TYPE_AS_BooleanType: {
      GenBooleanType (shape);
      break;
    }
    case TAG_TYPE_AS_NumericType: {
      GenNumericType (shape);
      break;
    }
    case TAG_TYPE_AS_TokenType: {
      GenTokenType (shape);
      break;
    }
    case TAG_TYPE_AS_CharType: {
      GenCharType (shape);
      break;
    }
    case TAG_TYPE_AS_VoidType: {
      GenVoidType (shape);
      break;
    }
    case TAG_TYPE_AS_QuoteType: {
      GenQuoteLit (shape.GetRecord(pos_AS_QuoteType_lit), this->LatexOutput);
      break;
    }
    case TAG_TYPE_AS_CompositeType: {
      GenCompositeType (shape, false);
      break;
    }
    case TAG_TYPE_AS_UnionType: {
      GenUnionType (shape);
      break;
    }
    case TAG_TYPE_AS_ProductType: {
      GenProductType (shape);
      break;
    }
    case TAG_TYPE_AS_OptionalType: {
      GenOptionalType (shape);
      break;
    }
    case TAG_TYPE_AS_Set0Type: {
      GenSet0Type (shape);
      break;
    }
    case TAG_TYPE_AS_Set1Type: {
      GenSet1Type (shape);
      break;
    }
    case TAG_TYPE_AS_Seq0Type: {
      GenSeq0Type (shape);
      break;
    }
    case TAG_TYPE_AS_Seq1Type: {
      GenSeq1Type (shape);
      break;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      GenGeneralMap0Type (shape);
      break;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      GenGeneralMap1Type (shape);
      break;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      GenInjectiveMap0Type (shape);
      break;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      GenInjectiveMap1Type (shape);
      break;
    }
    case TAG_TYPE_AS_TypeName: {
      GenTypeName (shape);
      break;
    }
    case TAG_TYPE_AS_PartialFnType: {
      GenPartialFnType (shape);
      break;
    }
    case TAG_TYPE_AS_TotalFnType: {
      GenTotalFnType (shape);
      break;
    }
    case TAG_TYPE_AS_OpType: {
      GenOpType (shape);
      break;
    }
    case TAG_TYPE_AS_TypeVar: {
      GenTypeVar (shape);
      break;
    }
    case TAG_TYPE_AS_BracketedType: {
      GenBracketedType (shape);
      break;
    }
    default: {
      GenLiteral (shape, this->LatexOutput);
      break;
    }
  }
}

void GenLatexOutput::GenInv (const Generic & modnm, const TYPE_AS_Invariant & InvTpl, bool semicolon)
{
  const TYPE_AS_Pattern & pat (InvTpl.GetRecord(pos_AS_Invariant_pat));
  const TYPE_AS_Expr & exp (InvTpl.GetRecord(pos_AS_Invariant_expr));

  LatexOutput << "\\begin{invfn}{";
  GenPattern (modnm, pat);
  LatexOutput << "}";

  int patline = PatLine(pat);
  int expline = GetLine(exp);
  if (patline < expline ) {
    LatexOutput << LF;
  }
  GenExpr (modnm, exp);
  if (semicolon) {
    LatexOutput << ";";
  }
  LatexOutput << endl;
  LatexOutput << "\\end{invfn}";
}

void GenLatexOutput::GenEqual(const Generic & modnm, const TYPE_AS_Equal& ord, bool semicolon)
{
  const TYPE_AS_Pattern & lhs (ord.GetRecord(pos_AS_Equal_lhs));
  const TYPE_AS_Pattern & rhs (ord.GetRecord(pos_AS_Equal_rhs));
  const TYPE_AS_Expr & expr   (ord.GetRecord(pos_AS_Equal_expr));
}

void GenLatexOutput::GenOrder(const Generic & modnm, const TYPE_AS_Order& ord, bool semicolon)
{
  const TYPE_AS_Pattern & lhs (ord.GetRecord(pos_AS_Order_lhs));
  const TYPE_AS_Pattern & rhs (ord.GetRecord(pos_AS_Order_rhs));
  const TYPE_AS_Expr & expr   (ord.GetRecord(pos_AS_Order_expr));
}

void GenLatexOutput::GenFnType (const TYPE_AS_FnType & shape)
{
  switch(shape.GetTag()) {
    case TAG_TYPE_AS_TotalFnType:   { GenTotalFnType (shape); break; }
    case TAG_TYPE_AS_PartialFnType: { GenPartialFnType (shape); break; }
    default: { break; }
  }
}

void GenLatexOutput::GenPartialFnType (const TYPE_AS_PartialFnType & shape)
{
  const SEQ<TYPE_AS_Type> & fndom (shape.GetSequence(pos_AS_PartialFnType_fndom));
  const TYPE_AS_Type & fnrng (shape.GetRecord(pos_AS_PartialFnType_fnrng));

  if (fndom.IsEmpty ()) {
    LatexOutput << "()";
    LatexOutput << " \\Pto ";
  }
  else {
    if (fndom.Length() > 1) {
      TYPE_AS_ProductType rc;
      rc.set_tps(fndom);
      rc.set_cid(shape.GetInt(pos_AS_PartialFnType_cid));
      GenType(rc);
    }
    else {
      GenType(fndom.Hd ());
    }
    LatexOutput << " \\Pto ";

    if (GetLine (fnrng) > GetLine (fndom. Hd ())) {
      LatexOutput << LF;
    }
  }
  GenType (fnrng);
}

void GenLatexOutput::GenTotalFnType (const TYPE_AS_TotalFnType & shape)
{
  const SEQ<TYPE_AS_Type> & fndom (shape.GetSequence(pos_AS_TotalFnType_fndom));
  const TYPE_AS_Type & fnrng (shape.GetRecord(pos_AS_TotalFnType_fnrng));

  if (fndom.IsEmpty ()) {
    LatexOutput << "()";
  }
  else {
    if (fndom.Length () > 1) {
      TYPE_AS_ProductType rc;
      rc.set_tps(fndom);
      rc.set_cid(shape.GetInt(pos_AS_TotalFnType_cid));
      GenType(rc);
    }
    else {
      GenType(fndom.Hd ());
    }
    if (GetLine (shape) > GetLine (fndom.Hd ())) {
      LatexOutput << LF;
    }
  }
  LatexOutput << " \\To ";

  if (GetLine (shape) < GetLine (fnrng)) {
    LatexOutput << LF;
  }
  GenType (fnrng);
}

void GenLatexOutput::GenOpType(const TYPE_AS_OpType & shape)
{
  const SEQ<TYPE_AS_Type> & lhs (shape.GetSequence(pos_AS_OpType_opdom));
  const TYPE_AS_Type & rhs (shape.GetRecord(pos_AS_OpType_oprng));

  if (lhs.IsEmpty ()) {
    LatexOutput << "()";
    LatexOutput << " \\Oto ";
  }
  else {
    if (lhs.Length () > 1) {
      TYPE_AS_ProductType rc;
      rc.set_tps(lhs);
      rc.set_cid(shape.GetInt(pos_AS_OpType_cid));
      GenType(rc);
    }
    else {
      GenType(lhs.Hd ());
    }
    LatexOutput << " \\Oto ";
    if (!rhs.IsNil() && GetLine (rhs) > GetLine (lhs.Hd())) {
      LatexOutput << LF;
    }
  }
  GenType (rhs);
}

void GenLatexOutput::GenBooleanType (const TYPE_AS_BooleanType & BTp)
{
  LatexOutput << "\\Bool ";
}

void GenLatexOutput::GenNumericType (const TYPE_AS_NumericType & BTp)
{
  const Int & bas_tp (BTp.GetInt(pos_AS_NumericType_qtp));

  switch (bas_tp.GetValue ()) {
    case NAT:     { LatexOutput << "\\Nat "; break; }
    case NATONE:  { LatexOutput << "\\Natone "; break; }
    case INTEGER: { LatexOutput << "\\Int "; break; }
    case RAT:     { LatexOutput << "\\Rat "; break; }
    case REAL:    { LatexOutput << "\\Real "; break; }
    default: { ErrMsg1 (BTp, L"Unknown basic type"); LatexOutput << ""; break; }
  }
}

void GenLatexOutput::GenTokenType (const TYPE_AS_TokenType & BTp)
{
  LatexOutput << "\\Token ";
}

void GenLatexOutput::GenCharType (const TYPE_AS_CharType & BTp)
{
  LatexOutput << "\\Char ";
}

void GenLatexOutput::GenVoidType (const TYPE_AS_VoidType & tp)
{
  LatexOutput << "()";
}

void GenLatexOutput::GenQuoteLit (const TYPE_AS_QuoteLit & QLt, std::ofstream & ofs)
{
  std::wstring qwnm (QLt.GetSequence(pos_AS_QuoteLit_val).GetString());

  ofs << "\\const{";
  std::wstring wstr;
  for (int i = 0; i < (int)qwnm.length(); i++) {
    if (qwnm[i] == L'_') {
      if(wstr.length() != 0) {
        ofs << TBWSTR::wstring2mbstr(wstr);
      }
      ofs << "\\hbox{\\textunderscore}";
      wstr = L"";
    }
    else {
     wstr += qwnm[i];
    }
  }
  if(wstr.length() != 0) {
    ofs << TBWSTR::wstring2mbstr(wstr);
  }
  ofs << "}";
}

void GenLatexOutput::GenUnionType (const TYPE_AS_UnionType & UTp)
{
  SEQ<TYPE_AS_Type> tp_s (UTp.GetSequence(pos_AS_UnionType_tps));

  Generic elm_t;
  bool bb = tp_s.First(elm_t);

  while (bb) {
    TYPE_AS_Type rc (elm_t);
    GenType (rc);
    bb = tp_s.Next(elm_t);
    if (bb) {
      this->LatexOutput << " \\TypeOr ";
      if (GetLine (rc) < GetLine (elm_t)) {
        this->LatexOutput << LF;
      }
    }
  }
}

void GenLatexOutput::GenProductType (const TYPE_AS_ProductType & TTp)
{
  const SEQ<TYPE_AS_Type> & tps (TTp.GetSequence(pos_AS_ProductType_tps));

  size_t len_tps = tps.Length ();
  for (size_t i = 1 ; i <= len_tps ; i++) {
    GenType (tps[i]);
    if (i < len_tps) {
      this->LatexOutput << " \\Mult ";
    }
  }
}

void GenLatexOutput::GenOptionalType (const TYPE_AS_OptionalType & OTp)
{
  const TYPE_AS_Type & otp (OTp.GetRecord(pos_AS_OptionalType_tp));

  this->LatexOutput << "\\Opt{";
  GenType (otp);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenSet0Type (const TYPE_AS_Set0Type & STp)
{
  const TYPE_AS_Type & tp (STp.GetRecord(pos_AS_Set0Type_elemtp));

  this->LatexOutput << "\\setof{";
  GenType(tp);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenSet1Type (const TYPE_AS_Set1Type & STp)
{
  const TYPE_AS_Type & tp (STp.GetRecord(pos_AS_Set1Type_elemtp));

  this->LatexOutput << "\\setof{";
  GenType(tp);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenSeq0Type (const TYPE_AS_Seq0Type & STp)
{
  const TYPE_AS_Type & tp (STp.GetRecord(pos_AS_Seq0Type_elemtp));

  this->LatexOutput << "\\seqof*{";
  GenType(tp);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenSeq1Type (const TYPE_AS_Seq1Type & STp)
{
  const TYPE_AS_Type & tp (STp.GetRecord(pos_AS_Seq1Type_elemtp));

  this->LatexOutput << "\\seqof+{";
  GenType(tp);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenGeneralMap0Type (const TYPE_AS_GeneralMap0Type & MTp)
{
  const TYPE_AS_Type & dom (MTp.GetRecord(pos_AS_GeneralMap0Type_mapdom));
  const TYPE_AS_Type & rng (MTp.GetRecord(pos_AS_GeneralMap0Type_maprng));

  this->LatexOutput << "\\mapof{";
  GenType(dom);
  this->LatexOutput << "\\Gmap ";
  GenType(rng);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenGeneralMap1Type (const TYPE_AS_GeneralMap1Type & MTp)
{
  const TYPE_AS_Type & dom (MTp.GetRecord(pos_AS_GeneralMap1Type_mapdom));
  const TYPE_AS_Type & rng (MTp.GetRecord(pos_AS_GeneralMap1Type_maprng));

  this->LatexOutput << "\\mapof{";
  GenType(dom);
  this->LatexOutput << "\\Gmap ";
  GenType(rng);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenInjectiveMap0Type (const TYPE_AS_InjectiveMap0Type & MTp)
{
  const TYPE_AS_Type & dom (MTp.GetRecord(pos_AS_InjectiveMap0Type_mapdom));
  const TYPE_AS_Type & rng (MTp.GetRecord(pos_AS_InjectiveMap0Type_maprng));

  this->LatexOutput << "\\inmapof{";
  GenType(dom);
  this->LatexOutput << "\\Bmap ";
  GenType(rng);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenInjectiveMap1Type (const TYPE_AS_InjectiveMap1Type & MTp)
{
  const TYPE_AS_Type & dom (MTp.GetRecord(pos_AS_InjectiveMap1Type_mapdom));
  const TYPE_AS_Type & rng (MTp.GetRecord(pos_AS_InjectiveMap1Type_maprng));

  this->LatexOutput << "\\inmapof{";
  GenType(dom);
  this->LatexOutput << "\\Bmap ";
  GenType(rng);
  this->LatexOutput << "}";
}

void GenLatexOutput::GenTypeName (const TYPE_AS_TypeName & Tname)
{
  const TYPE_AS_Name & nm (Tname.GetRecord(pos_AS_TypeName_name));

  GenName (nm, this->LatexOutput);
  if (this->INDEX) {
    GenIndex (nm, typeocc_index);
  }
}

// Record type is a composite type with the double colon notation
void GenLatexOutput::GenRecordType (const TYPE_AS_CompositeType & RTp, bool semicolon)
{
  const TYPE_AS_Name & name (RTp.GetRecord(pos_AS_CompositeType_name));
  const SEQ<TYPE_AS_Field> & flds (RTp.GetSequence(pos_AS_CompositeType_fields));

  this->LatexOutput << "\\begin{record}{";
  //  LatexOutput << "\\kw{ }";

  GenName (name, this->LatexOutput);
  if (this->INDEX) {
    GenIndex (name, typedef_index);
  }
  this->LatexOutput << "}"; //LatexOutput << "}\n";

  if (!flds.IsEmpty()) {
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++) {
      const TYPE_AS_Field & field (flds[idx]);
      const Generic & sel_r   (field.GetField(pos_AS_Field_sel));
      const TYPE_AS_Type & tp (field.GetRecord(pos_AS_Field_type));
      const Bool & dc         (field.GetBool(pos_AS_Field_dc));

      if (!sel_r.IsNil()) {
        GenName (sel_r, this->LatexOutput);
      }
      // to make sure line breaks are properly handled, we enclose the
      // generated type with curly braces.
      if (!dc) {
        this->LatexOutput << " :{";
      }
      else {
        this->LatexOutput << " :- {";
      }
      GenType (tp);

      if (idx < len_flds) {
        this->LatexOutput << "}" << LF;
      }
      else {
        if (semicolon) {
          this->LatexOutput << "; }" << LF;
        }
        else {
          this->LatexOutput << "}" << LF;
        }
      }
    }
  }
  else {
    if (semicolon) {
      this->LatexOutput << ": ;" << LF;
    }
    else {
      this->LatexOutput << ":" << LF;
    }
  }

  LatexOutput << "\\end{record}";
}

void GenLatexOutput::GenCompositeType (const TYPE_AS_CompositeType & CompTp, bool semicolon)
{
  const TYPE_AS_Name & name (CompTp.GetRecord(pos_AS_CompositeType_name));
  const SEQ<TYPE_AS_Field> & flds (CompTp.GetSequence(pos_AS_CompositeType_fields));

  this->LatexOutput << "\\begin{composite}{";
  GenName(name, this->LatexOutput);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  if (!flds.IsEmpty()) {
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++) {
      const TYPE_AS_Field & field (flds[idx]);
      const Generic & sel_r   (field.GetField(pos_AS_Field_sel));
      const TYPE_AS_Type & tp (field.GetRecord(pos_AS_Field_type));
      const Bool & dc         (field.GetBool(pos_AS_Field_dc));

      if (!sel_r.IsNil()) {
        GenName (sel_r, this->LatexOutput);
      }
      // to make sure line breaks are properly handled, we enclose the
      // generated type with curly braces.
      if (!dc) {
        this->LatexOutput << " :{";
      }
      else {
        this->LatexOutput << " :-{";
      }
      GenType (tp);
      this->LatexOutput << "}" << LF;
    }
  }
  else {
    this->LatexOutput << ": ;" << LF;
  }

  LatexOutput << "\\end{composite}";

  if (semicolon) {
    this->LatexOutput << ";";
  }
}

void GenLatexOutput::GenTypeVar (const TYPE_AS_TypeVar & TypeV)
{
  const TYPE_AS_Name & nm (TypeV.GetRecord(pos_AS_TypeVar_name));

  this->LatexOutput << "@";
  GenName (nm, this->LatexOutput);
}

void GenLatexOutput::GenBracketedType (const TYPE_AS_BracketedType & BrackT)
{
  const TYPE_AS_Type & tp (BrackT.GetRecord(pos_AS_BracketedType_tp));

  this->LatexOutput << "\\Lp ";
  GenType (tp);
  this->LatexOutput << " \\Rp ";
}

/** State Definitions part **************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

#ifdef VDMSL
void GenLatexOutput::GenStateDef (const Generic & modnm, const TYPE_AS_StateDef & StateDef1)
{
  const TYPE_AS_CompositeType & tp (StateDef1.GetRecord(pos_AS_StateDef_tp));
  const Generic & inv              (StateDef1.GetField(pos_AS_StateDef_Inv));
  const Generic & initg            (StateDef1.GetField(pos_AS_StateDef_Init));

  const TYPE_AS_Name & name       (tp.GetRecord(pos_AS_CompositeType_name));
  const SEQ<TYPE_AS_Field> & flds (tp.GetSequence(pos_AS_CompositeType_fields));

  LatexOutput << "\\begin{statedef}{";
  GenName (name, this->LatexOutput);

  if (this->INDEX) {
    GenIndex (name, state_index);
  }
  LatexOutput << "}";
  LatexOutput << endl;

  size_t len_flds = flds.Length();
  for (size_t idx = 1; idx <= len_flds; idx++) {
    const TYPE_AS_Field & field (flds[idx]);
    const Generic & sel_r   (field.GetField(pos_AS_Field_sel));
    const TYPE_AS_Type & tp (field.GetRecord(pos_AS_Field_type));

    if (!sel_r.IsNil()) {
      GenName (sel_r, this->LatexOutput);
    }
    this->LatexOutput << " :{";
    GenType(tp);
    this->LatexOutput << "}" << LF;
  }

  if (!inv.IsNil()) {
    GenInvariant (modnm, inv);
    this->LatexOutput << endl;
  }

  if (!initg.IsNil()) {
    TYPE_AS_StateInit init (initg);
    GenInit (modnm, init.GetRecord(pos_AS_StateInit_pat), init.GetRecord(pos_AS_StateInit_expr));
    this->LatexOutput << endl;
  }

  this->LatexOutput << "\\end{statedef}";
  this->LatexOutput << endl;
  this->LatexOutput.flush ();
}
#endif // VDMSL

void GenLatexOutput::GenInvariant (const Generic & modnm, const TYPE_AS_Invariant & Inv)
{
  const TYPE_AS_Pattern & pat  (Inv.GetRecord(pos_AS_Invariant_pat));
  const TYPE_AS_Expr & expr    (Inv.GetRecord(pos_AS_Invariant_expr));

  this->LatexOutput << "\\begin{invfn}{";
  GenPattern(modnm, pat);
  this->LatexOutput << "}";

  int patline = PatLine(pat);
  int expline = GetLine(expr);
  if (patline < expline) {
    this->LatexOutput << LF;
  }
  GenExpr(modnm, expr);
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{invfn}";
}

void GenLatexOutput::GenStatic(const Bool& statiC, const Record& from, bool prelinebreak, bool postlinebreak)
{
#ifdef VDMPP
  if(statiC.GetValue()) {

    if (prelinebreak) {
      this->LatexOutput << endl;
    }
    this->LatexOutput << "\\kw{static}";

    if (postlinebreak) {
      this->LatexOutput << endl;
    }
  }
#endif
}

void GenLatexOutput::GenAsync(const Bool& sync, const Record & from, bool prelinebreak, bool postlinebreak)
{
#ifdef VDMPP
  if(!sync) {

    if (prelinebreak) {
      this->LatexOutput << endl;
    }
    this->LatexOutput << "\\kw{async}";

    if (postlinebreak) {
      this->LatexOutput << endl;
    }
  }
#endif
}

void GenLatexOutput::GenAccess(const Int& access,
                               const Record& from,
                               bool prelinebreak,
                               bool postlinebreak)
{

  int acc = access.GetValue();
#ifdef VDMPP
  if (prelinebreak && (acc == PRIVATE_AS ||
                       acc == PROTECTED_AS ||
                       acc == PUBLIC_AS) ) {
    this->LatexOutput << endl;
  }
#endif //VDMPP

  switch (acc) {
    case DEFAULT_AS : { break; }
#ifdef VDMPP
    case PRIVATE_AS :   { this->LatexOutput << "\\kw{private}"; break; }
    case PROTECTED_AS : { this->LatexOutput << "\\kw{protected}"; break; }
    case PUBLIC_AS :    { this->LatexOutput << "\\kw{public}"; break; }
#endif //VDMPP
    case NOT_INITIALISED_AS : { break; }
    default: { ErrMsg1(from,L"Unknown Access value"); break; }
  }

#ifdef VDMPP
  if (postlinebreak && (acc == PRIVATE_AS ||
                        acc == PROTECTED_AS ||
                        acc == PUBLIC_AS) ) {
    this->LatexOutput << endl;
  }
#endif //VDMPP
}

#ifdef VDMSL
void GenLatexOutput::GenInit (const Generic & modnm, const TYPE_AS_Pattern & pat, const TYPE_AS_Expr & Init)
{
  this->LatexOutput << "\\begin{initfn}{";
  GenPattern (modnm, pat);
  this->LatexOutput << "}";
  this->LatexOutput << endl;
  GenExpr(modnm, Init);
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{initfn}";
}
#endif // VDMSL

/** VDM++ Defs *************************************************************/
/****************************************************************************/

#ifdef VDMPP
void
GenLatexOutput::GenInstanceVarDef (const Generic & modnm, const TYPE_AS_InstanceVarDef& instvar, bool last_instvar)
{
  // InstanceVarDef = AssignDef | InstanceInv;

  switch(instvar.GetTag()) {
    case TAG_TYPE_AS_InstAssignDef: {
      this->LatexOutput << "\\begin{insvar}";
      GenInstAssignDef (modnm, instvar);
      this->LatexOutput << endl;
      this->LatexOutput << "\\end{insvar}";
      this->LatexOutput << endl;
      break;
    }
    case TAG_TYPE_AS_InstanceInv: {
      GenInstanceInv (modnm, instvar);
      break;
    }
    default: {
      break;
    }
  }
  if (last_instvar) {
    this->LatexOutput << "\\par";
    this->LatexOutput << endl;
  }
}

void GenLatexOutput::GenInstAssignDef(const Generic & modnm, const TYPE_AS_InstAssignDef & iad)
{
  const TYPE_AS_AssignDef & asdef (iad.GetRecord(pos_AS_InstAssignDef_ad));
  const TYPE_AS_Access & access   (iad.GetField(pos_AS_InstAssignDef_access));
  const Bool & statiC             (iad.GetBool(pos_AS_InstAssignDef_stat));

  int acc = Int(access).GetValue();
  if ((acc != DEFAULT_AS) || statiC.GetValue()) {
     GenAccess(access, iad, false, false);
     if ((acc != DEFAULT_AS) && statiC.GetValue()) {
       this->LatexOutput << LATEXSPACE2;
     }
     GenStatic(statiC, iad, false, false);
  }
  GenAssignDef(modnm, asdef);
}

void GenLatexOutput::GenInstanceInv (const Generic & modnm, const TYPE_AS_InstanceInv & instinv)
{
  const TYPE_AS_Expr & expr     (instinv.GetRecord(pos_AS_InstanceInv_expr));
  const TYPE_AS_Access & access (instinv.GetField(pos_AS_InstanceInv_access));

  GenAccess(access, instinv, false, true);
  this->LatexOutput << "\\begin{instinvfn}";
  this->LatexOutput << endl;
  GenExpr(modnm, expr);
  this->LatexOutput << "\\end{instinvfn}";
  this->LatexOutput << endl;
}

void GenLatexOutput::GenSyncDef (const Generic & modnm, const TYPE_AS_SyncDef & sync, bool semicolon)
{
  // SyncDef = Permission | Trace;

  switch(sync.GetTag()) {
    case TAG_TYPE_AS_Permission: { GenPermission(modnm, sync, semicolon); break; }
    case TAG_TYPE_AS_Mutex:      { GenMutex(modnm, sync, semicolon); break; }
    default: { break; }
  }
}

void GenLatexOutput::GenPermission(const Generic & modnm, const TYPE_AS_Permission & perm, bool semicolon)
{
  const TYPE_AS_Name & spec  (perm.GetRecord(pos_AS_Permission_spec));
  const TYPE_AS_Expr & guard (perm.GetRecord(pos_AS_Permission_guard));

  if (this->INDEX) {
    GenIndex(spec, methodocc_index);
  }
  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\per{";
  GenName(spec, this->LatexOutput);
  this->LatexOutput << "}{";
  GenExpr(modnm, guard);

  if (semicolon) {
    this->LatexOutput << ";";
  }
  this->LatexOutput << "}";
  this->LatexOutput << endl;
}

void GenLatexOutput::GenMutex(const Generic & modnm, const TYPE_AS_Mutex & mutex, bool semicolon)
{
  const Generic & ops (mutex.GetField(pos_AS_Mutex_ops));
  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\mutex{";

  if (!ops.IsNil()) {
    GenNameList(ops, this->LatexOutput);
  }
  else {
    this->LatexOutput << "\\kw{all}";
  }
  this->LatexOutput << "}";

  if (semicolon) {
    this->LatexOutput << ";";
  }
  this->LatexOutput << endl;
}

void GenLatexOutput::GenNameList(const TYPE_AS_NameList & nm_l, std::ofstream & ofs)
{
  if (!nm_l.IsEmpty()) {
    size_t len_nm_l = nm_l.Length();
    for (size_t idx = 1; idx <= len_nm_l; idx++) {
      GenName(nm_l[idx], ofs);
      if (idx < len_nm_l) {
        ofs << ",";
      }
    }
  }
}

void GenLatexOutput::GenThreadDef (const Generic & modnm, const TYPE_AS_ThreadDef & thread, bool semicolon)
{
  // ThreadDef = PerObl | SpoObl | Stmt;

  switch(thread.GetTag()) {
    case TAG_TYPE_AS_PerObl: {
      GenPerObl(modnm, thread);
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_AS_SpoObl: {
      GenSpoObl(modnm, thread);
      LatexOutput << endl;
      break;
    }
    default: {
      LatexOutput << "\\begin{thread}";
      LatexOutput << endl;
      GenStmt(modnm, thread);
      LatexOutput << endl;
      LatexOutput << "\\end{thread}";
      LatexOutput << endl;
      break;
    }
  }
}

void GenLatexOutput::GenPerObl (const Generic & modnm, const TYPE_AS_PerObl &po)
{
  if (this->INDEX) {
    GenIndex(po.get_mtd(), methodocc_index);
  }
  LatexOutput << LineFeed();
  LatexOutput << "\\periodic{";

  GenExpr(modnm, po.get_period()); LatexOutput << ",";
  GenExpr(modnm, po.get_jitter()); LatexOutput << ",";
  GenExpr(modnm, po.get_delay());  LatexOutput << ",";
  GenExpr(modnm, po.get_offset());

  LatexOutput << "}{";
  GenName(po.get_mtd(), this->LatexOutput);
  LatexOutput << "}";
}

void GenLatexOutput::GenSpoObl (const Generic & modnm, const TYPE_AS_SpoObl &po)
{
  if (this->INDEX) {
    GenIndex(po.get_mtd(), methodocc_index);
  }
  LatexOutput << LineFeed();
  LatexOutput << "\\sporadic{";

  GenExpr(modnm, po.get_delay());  LatexOutput << ",";
  GenExpr(modnm, po.get_bound());  LatexOutput << ",";
  GenExpr(modnm, po.get_offset());

  LatexOutput << "}{";
  GenName(po.get_mtd(), this->LatexOutput);
  LatexOutput << "}";
}
#endif // VDMPP

// GenTraces
void GenLatexOutput::GenTraces(const Generic & modnm, 
                               const SEQ<TYPE_AS_Name> & name_l,
                               const SEQ<TYPE_AS_TraceDefTerm> & tdt_l,
                               bool semicolon)
{
  LatexOutput << "\\begin{traces}";
  LatexOutput << endl;
  size_t len_name_l = name_l.Length();
  for (size_t i = 1; i <= len_name_l; i++) {
    if (i > 1) {
      LatexOutput << "/";
    }
    GenName(name_l[i], this->LatexOutput);
  }

  LatexOutput << ": ";
  LatexOutput << LF;
  GenTraceDefList(modnm, tdt_l);
  
  LatexOutput << LF;
  LatexOutput << "\\end{traces}";

  if (semicolon) {
    LatexOutput << ";";
  }
  LatexOutput << endl;
}

void GenLatexOutput::GenTraceDefList(const Generic & modnm, const SEQ<TYPE_AS_TraceDefTerm> & tdt_l)
{
  size_t len_tdt_l = tdt_l.Length();
  for (size_t j = 1; j <= len_tdt_l; j++) {
    if (j > 1) {
      LatexOutput << ";" << LF;
    }
    GenTraceDefTerm(modnm, tdt_l[j]);
  }
}

void GenLatexOutput::GenTraceDefTerm(const Generic & modnm, const TYPE_AS_TraceDefTerm & tdt)
{
  switch(tdt.GetTag()) {
    case TAG_TYPE_AS_TraceApplyExpr:       { GenTraceApplyExpr(modnm, tdt); break; }
    case TAG_TYPE_AS_TraceBracketedExpr:   { GenTraceBracketedExpr(modnm, tdt); break; }
    case TAG_TYPE_AS_TracePermuteExpr:     { GenTracePermuteExpr(modnm, tdt); break; }
    case TAG_TYPE_AS_QualifiedTrace:       { GenQualifiedTrace(modnm, tdt); break; }
    case TAG_TYPE_AS_RepeatTrace:          { GenRepeatTrace(modnm, tdt); break; }
    case TAG_TYPE_AS_QualifiedRepeatTrace: { GenQualifiedRepeatTrace(modnm, tdt); break; }
    case TAG_TYPE_AS_TraceDefAltn:         { GenTraceDefAltn(modnm, tdt); break; }
  }
}

void GenLatexOutput::GenTraceApplyExpr(const Generic & modnm, const TYPE_AS_TraceApplyExpr & te)
{
#ifdef VDMSL
  const TYPE_AS_Name & op        (te.GetRecord(pos_AS_TraceApplyExpr_op));
  const SEQ<TYPE_AS_Expr> & args (te.GetSequence(pos_AS_TraceApplyExpr_args));
  const TYPE_CI_ContextId & cid  (te.GetInt(pos_AS_TraceApplyExpr_cid));

  TYPE_AS_ApplyExpr ae;
  ae.Init(op, args, cid);
  GenApplyExpr (modnm, ae);
#endif // VDMSL
#ifdef VDMPP
  const Generic & obj            (te.GetRecord(pos_AS_TraceApplyExpr_obj));
  const TYPE_AS_Name & op        (te.GetRecord(pos_AS_TraceApplyExpr_op));
  const SEQ<TYPE_AS_Expr> & args (te.GetSequence(pos_AS_TraceApplyExpr_args));
  const TYPE_CI_ContextId & cid  (te.GetInt(pos_AS_TraceApplyExpr_cid));

  if (obj.IsNil()) {
    TYPE_AS_ApplyExpr ae;
    ae.Init(op, args, cid);
    GenApplyExpr (modnm, ae);
  }
  else {
    TYPE_AS_FieldSelectExpr fse;
    fse.Init(obj, op, cid);
    TYPE_AS_ApplyExpr ae;
    ae.Init(fse, args, cid);
    GenApplyExpr (modnm, ae);
  }
#endif // VDMPP
}

void GenLatexOutput::GenTraceBracketedExpr(const Generic & modnm, const TYPE_AS_TraceBracketedExpr & tbe)
{
  const SEQ<TYPE_AS_TraceDefTerm> & list (tbe.GetSequence(pos_AS_TraceBracketedExpr_list));
  
  this->LatexOutput << "\\pex{";

  this->linefeed = false;
  GenTraceDefList(modnm, list);

  this->LatexOutput << "}";
}

void GenLatexOutput::GenTracePermuteExpr(const Generic & modnm, const TYPE_AS_TracePermuteExpr & tbe)
{
  const SEQ<TYPE_AS_TraceDefTerm> & list (tbe.GetSequence(pos_AS_TracePermuteExpr_list));
  
  this->LatexOutput << "\\begin{nondetstmt}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenTraceDefList(modnm, list);

  this->LatexOutput << "\\end{nondetstmt}";
}

void GenLatexOutput::GenQualifiedTrace(const Generic & modnm, const TYPE_AS_QualifiedTrace & qt)
{
  const SEQ<TYPE_AS_TraceBind> & binds (qt.GetSequence(pos_AS_QualifiedTrace_binds));
  const TYPE_AS_SimpleTrace & tdef     (qt.GetRecord(pos_AS_QualifiedTrace_tdef));

  size_t len_binds = binds.Length();
  for (size_t i = 1; i <= len_binds; i++) {
    if (i > 1) {
      LatexOutput << LF;
    }
    GenTraceBind(modnm, binds[i]);
  }
  LatexOutput << LF;
  GenTraceDefTerm(modnm, tdef);
}

void GenLatexOutput::GenRepeatTrace(const Generic & modnm, const TYPE_AS_RepeatTrace & rt)
{
  const TYPE_AS_SimpleTrace & tdef       (rt.GetRecord(pos_AS_RepeatTrace_tdef));
  const TYPE_AS_TraceRepeatPattern & pat (rt.GetField(pos_AS_RepeatTrace_pat));

  GenTraceDefTerm(modnm, tdef);
  LatexOutput << LF;
  GenTraceRepeatPattern(modnm, pat);
}

void GenLatexOutput::GenQualifiedRepeatTrace(const Generic & modnm, const TYPE_AS_QualifiedRepeatTrace & qrt)
{
  const SEQ<TYPE_AS_TraceBind> & binds   (qrt.GetSequence(pos_AS_QualifiedRepeatTrace_binds));
  const TYPE_AS_SimpleTrace & tdef       (qrt.GetRecord(pos_AS_QualifiedRepeatTrace_tdef));
  const TYPE_AS_TraceRepeatPattern & pat (qrt.GetField(pos_AS_QualifiedRepeatTrace_pat));

  size_t len_binds = binds.Length();
  for (size_t i = 1; i <= len_binds; i++) {
    if (i > 1) {
      LatexOutput << LF;
    }
    GenTraceBind(modnm, binds[i]);
  }
  LatexOutput << LF;
  GenTraceDefTerm(modnm, tdef);
  LatexOutput << LF;
  GenTraceRepeatPattern(modnm, pat);
}

void GenLatexOutput::GenTraceDefAltn(const Generic & modnm, const TYPE_AS_TraceDefAltn & tda)
{
  const TYPE_AS_TraceDefTerm & term (tda.GetRecord(pos_AS_TraceDefAltn_term));
  const TYPE_AS_TraceDefTerm & tdef (tda.GetRecord(pos_AS_TraceDefAltn_tdef));

  GenTraceDefTerm(modnm, term);
  GenTraceDefTerm(modnm, tdef);
}

void GenLatexOutput::GenTraceRepeatPattern(const Generic & modnm, const TYPE_AS_TraceRepeatPattern & trp)
{
  if (trp.IsRecord()) {
    switch(Record(trp).GetTag()) {
      case TAG_TYPE_AS_RepeatFixed: {
        TYPE_AS_RepeatFixed rf (trp);
        LatexOutput << "\\hbox{\\textbraceleft}";
        GenExpr(modnm, rf.GetRecord(pos_AS_RepeatFixed_num));
        LatexOutput << "\\hbox{\\textbraceright}";
        break;
      }
      case TAG_TYPE_AS_RepeatInterval: {
        TYPE_AS_RepeatInterval ri (trp);
        LatexOutput << "\\hbox{\\textbraceleft}";
        GenExpr(modnm, ri.GetRecord(pos_AS_RepeatInterval_low));
        LatexOutput << ",";
        GenExpr(modnm, ri.GetRecord(pos_AS_RepeatInterval_high));
        LatexOutput << "\\hbox{\\textbraceright}";
        break;
      }
    }
  }
  else if(trp.IsInt())
  {
    switch(Int(trp).GetValue()) {
      case ANY:        { LatexOutput << "*"; break; }
      case ATLEASTONE: { LatexOutput << "+"; break; }
      case POSSIBLY:   { LatexOutput << "?"; break; }
    }
  }
}

void GenLatexOutput::GenTraceBind(const Generic & modnm, const TYPE_AS_TraceBind & tb)
{
  switch(tb.GetTag()) {
    case TAG_TYPE_AS_LocalTraceBind: { GenLocalTraceBind(modnm, tb); break; }
    case TAG_TYPE_AS_LetTraceBind:   { GenLetTraceBind(modnm, tb); break; }
    case TAG_TYPE_AS_LetBeTraceBind: { GenLetBeTraceBind(modnm, tb); break; }
  }
}

void GenLatexOutput::GenLocalTraceBind(const Generic & modnm, const TYPE_AS_LocalTraceBind & ltb)
{
  const SEQ<TYPE_AS_LocalDef> & ldef_l (ltb.GetSequence(pos_AS_LocalTraceBind_localdefs));

  this->LatexOutput << "\\Letbe";
  this->LatexOutput << LATEXSPACE2;

  size_t len_ldef_l = ldef_l.Length();
  for (size_t i = 1; i <= len_ldef_l; i++) {
    const TYPE_AS_LocalDef & def (ldef_l[i]);
    switch(def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        TYPE_AS_ValueDef vd (def);
        TYPE_AS_Pattern pat (vd.get_pat());
        TYPE_AS_Expr val (vd.get_val());
        Generic tp (vd.get_tp());
        LatexOutput << "\\patdef{";
        GenPattern (modnm, pat);
        if (! tp.IsNil ()) {
          LatexOutput << " : ";
          GenType (tp);
        }
        LatexOutput << "}";

        int l1 = PatLine (pat);
        int l2 = GetLine (val);

        if (l2 > l1) {
          LatexOutput << LF;
        }
        LatexOutput << "{";
        GenExpr (modnm, val);
        LatexOutput << "}";
        LatexOutput << endl;
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef:
      case TAG_TYPE_AS_ImplFnDef: {
        GenFunctionDef (modnm, def, false);
        break;
      }
      default:
        break;
    }
  }
  this->linefeed = false;
  this->LatexOutput << " \\Lin" << LF;
}

void GenLatexOutput::GenLetTraceBind(const Generic & modnm, const TYPE_AS_LetTraceBind & ltb)
{
  const SEQ<TYPE_AS_MultBind> & bind_l (ltb.GetSequence(pos_AS_LetTraceBind_bind));

  // TODO:
  this->LatexOutput << "\\begin{letexpr}";
  this->LatexOutput << endl;

  int l1 = BindLine (bind_l[1]);

  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++) {
    const TYPE_AS_MultBind & bind (bind_l[idx]);
    int l2 = BindLine (bind);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l2 > l1) {
      LatexOutput << LF;
    }
    GenMultBind (modnm, bind);
    l1 = l2;
  }

  this->linefeed = false;
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{letexpr}";
}

void GenLatexOutput::GenLetBeTraceBind(const Generic & modnm, const TYPE_AS_LetBeTraceBind & lbtb)
{
  const SEQ<TYPE_AS_MultBind> & bind_l (lbtb.GetSequence(pos_AS_LetBeTraceBind_bind));
  const TYPE_AS_Expr & stexpr (lbtb.GetRecord(pos_AS_LetBeTraceBind_stexpr));

  this->LatexOutput << "\\Letbe";
  this->LatexOutput << LATEXSPACE2;

  int l1 = BindLine (bind_l[1]);

  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++) {
    const TYPE_AS_MultBind & bind (bind_l[idx]);
    int l2 = BindLine (bind);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l2 > l1) {
      LatexOutput << LF;
    }
    GenMultBind (modnm, bind);
    l1 = l2;
  }

  this->LatexOutput << " \\Best";

  int l3 = GetLine (stexpr);

  if (l3 > l1) {
    this->LatexOutput << LF;
  }
  this->linefeed = false;
  this->LatexOutput << LATEXSPACE2;
  GenExpr(modnm, stexpr);

  this->linefeed = false;
  this->LatexOutput << " \\Lin" << LF;
}

/** Values part *************************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

////////////////
// Name: GenLatexOutput::GenValueDef
// Descr:
//   Generate a value definition
////////////////

void GenLatexOutput::GenValueDef (const Generic & modnm, const TYPE_AS_ValueDef & ValRec, bool semicolon)
{
  const TYPE_AS_Pattern & pat (ValRec.GetRecord(pos_AS_ValueDef_pat));
  const TYPE_AS_Expr & expr   (ValRec.GetRecord(pos_AS_ValueDef_val));
  const Generic & tp          (ValRec.GetField(pos_AS_ValueDef_tp));
  const Int & access          (ValRec.GetInt(pos_AS_ValueDef_access));
  const Bool & statiC         (ValRec.GetBool(pos_AS_ValueDef_stat));

  if(access.GetValue()!=DEFAULT_AS || statiC.GetValue()) {
    LatexOutput << "\\kw{";
    GenAccess(access, ValRec, false, false);
    if(access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
      LatexOutput << LATEXSPACE2;
      GenStatic(statiC, ValRec, false, false);
    }
    LatexOutput << "}";
  }

  //  LatexOutput << "\\kw{ }"; // It is a hack to insert space

  LatexOutput << "\\val{";
  GenPattern(modnm, pat);
  LatexOutput << "}";

  if (! tp.IsNil ()) {
    LatexOutput << "[";
    GenType (tp);
    LatexOutput << "]";
  }

  LatexOutput << "{";
  GenExpr(modnm, expr);
  if (semicolon) {
    LatexOutput << ";";
  }
  LatexOutput << "}";
  LatexOutput << endl;
  LatexOutput.flush ();
}

/** Function Definitions part ***********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenFunctionDef (const Generic & mod, const TYPE_AS_FnDef & fn_def, bool semicolon)
{
  switch(fn_def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { GenExplFnDef (mod, fn_def, semicolon); break; }
    case TAG_TYPE_AS_ExtExplFnDef: { GenExtExplFnDef(mod, fn_def, semicolon); break; }
    case TAG_TYPE_AS_ImplFnDef:    { GenImplFnDef (mod, fn_def, semicolon); break; }
    default: break;
  }
  this->LatexOutput.flush ();
}

// GenExplFnDef
void GenLatexOutput::GenExplFnDef (const Generic & modnm, const TYPE_AS_ExplFnDef & fn_def, bool semicolon)
{
#ifdef PROTO
  if (Settings.GetPrettyPrintRTI()) {
    //CalculateValCov (this->current_module, fn_def, LatexOutput);
    CalculateValCov (modnm, fn_def, LatexOutput);
  }
#endif //PROTO

  const TYPE_AS_Name & id               (fn_def.GetRecord(pos_AS_ExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpparms  (fn_def.GetSequence(pos_AS_ExplFnDef_tpparms));
  const SEQ<TYPE_AS_Parameters> & parms (fn_def.GetSequence(pos_AS_ExplFnDef_parms));
  const TYPE_AS_FnType & tp             (fn_def.GetRecord(pos_AS_ExplFnDef_tp));
  const TYPE_AS_FnBody & body           (fn_def.GetRecord(pos_AS_ExplFnDef_body));
  const Generic & pre                   (fn_def.GetField(pos_AS_ExplFnDef_fnpre));
  const Generic & post                  (fn_def.GetField(pos_AS_ExplFnDef_fnpost));
  const Int & access                    (fn_def.GetInt(pos_AS_ExplFnDef_access));
  const Bool & statiC                   (fn_def.GetBool(pos_AS_ExplFnDef_stat));
  const Generic & measu                 (fn_def.GetField(pos_AS_ExplFnDef_measu));

  if(access.GetValue() != DEFAULT_AS || statiC.GetValue()) {
      LatexOutput << "\\kw{";
      GenAccess(access, fn_def, false, false);
      if(access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
          LatexOutput << LATEXSPACE2;
          GenStatic(statiC, fn_def, false, false);
      }
      LatexOutput << "}";
  }

  LatexOutput << "\\begin{fn}[e]{";
  GenName (id, this->LatexOutput);
  LatexOutput << "}";

  if (this->INDEX) {
    GenIndex (id, fct_index);
  }
  LatexOutput << "%";
  LatexOutput << endl;
  LatexOutput << "\\signature";

  if (!tpparms.IsEmpty()) {
    LatexOutput << "[";
    size_t len_tpparms = tpparms.Length ();
    for (size_t i = 1; i <= len_tpparms; i++) {
      TYPE_AS_TypeVar tv (tpparms[i]);
      GenTypeVar (tv);
      if (i < len_tpparms) {
        LatexOutput << ",";
      }
    }
    LatexOutput << "]";
  }

  this->LatexOutput << "{";
  GenFnType (tp);
  this->LatexOutput << "}";
  this->LatexOutput << endl;
  this->LatexOutput << "\\parms*{";

  if (!parms.IsEmpty()) {
    int l1 = 0;
    int l2 = -1;

    size_t len_parms = parms.Length ();
    for (size_t bb = 1; bb <= len_parms; bb++) {
      SEQ<TYPE_AS_Pattern> prm (parms[bb]);

      if (!prm.IsEmpty()) {
        l1 = PatLine (prm[1]);
      }
      if ((l1 > l2) && (l2 != -1)) {
        this->LatexOutput << LF;
      }
      this->LatexOutput << "\\Lp ";

      size_t len_prm = prm.Length ();
      for (size_t cc = 1; cc <= len_prm; cc++) {
        l2 = PatLine (prm[cc]);

        if (l2 > l1) {
          this->LatexOutput << LF;
        }
        GenPattern (modnm, prm[cc]);
        if (cc < len_prm) {
          this->LatexOutput << ",";
        }
        l1 = l2;
      }
      this->LatexOutput << "\\Rp ";
    }
  }

  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->LatexOutput << "\\annlab[o]{";
  //if (!this->current_module.IsNil ())
  if (!modnm.IsNil ()) {
    //GenLabelName (this->current_module);
    GenLabelName (modnm, this->LatexOutput);
    this->LatexOutput << "`";
  }
  GenLabelName (id, this->LatexOutput);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenFnBody (modnm, body);

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  if (!post.IsNil()) {
    GenPost (modnm, post);
  }
  if (!measu.IsNil()) {
    this->LatexOutput << endl;
    this->LatexOutput << "\\begin{measu}";
    this->LatexOutput << endl;
    GenName (measu, this->LatexOutput);
    this->LatexOutput << endl;
    this->LatexOutput << "\\end{measu}";
  }

  if (semicolon)
    this->LatexOutput << ";";

  this->LatexOutput << endl;
  this->LatexOutput << "\\end{fn}";
  this->LatexOutput << endl;
}

void GenLatexOutput::GenExtExplFnDef (const Generic & modnm, const TYPE_AS_ExtExplFnDef & fn_def, bool semicolon)
{
  const TYPE_AS_Name & id                   (fn_def.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpparms      (fn_def.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps   (fn_def.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & residtplist (fn_def.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const TYPE_AS_FnBody & body               (fn_def.GetRecord(pos_AS_ExtExplFnDef_body));
  const Generic & pre                       (fn_def.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & post                      (fn_def.GetField(pos_AS_ExtExplFnDef_fnpost));
  const Int & access                        (fn_def.GetInt(pos_AS_ExtExplFnDef_access));
  const Bool & statiC                       (fn_def.GetBool(pos_AS_ExtExplFnDef_stat));

  if (access.GetValue() != DEFAULT_AS || statiC.GetValue()) {
    this->LatexOutput << "\\kw{";
    GenAccess(access, fn_def, false, false);
    if (access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
      this->LatexOutput << LATEXSPACE2;
      GenStatic(statiC, fn_def, false, false);
    }
    this->LatexOutput << "}";
  }

  this->LatexOutput << "\\begin{fn}[e]{";
  GenName (id, this->LatexOutput);

      // we have to put the polymorphic type vars in the function name,
      // because the macros do not offer another solution
  if (!tpparms.IsEmpty()) {
    this->LatexOutput << "[";
    size_t len_tpparms = tpparms.Length ();
    for (size_t bb = 1; bb <= len_tpparms; bb++) {
      TYPE_AS_TypeVar tv (tpparms[bb]);
      GenTypeVar (tv);
      if (bb < len_tpparms) {
        this->LatexOutput << ",";
      }
    }
    this->LatexOutput << "]";
  }

  this->LatexOutput << "}";

  if (this->INDEX) {
    GenIndex (id, fct_index);
  }
  this->LatexOutput << "%";
  this->LatexOutput << endl;
  this->LatexOutput << "\\parms{";

  if (!partps.IsEmpty()) {
    size_t len_partps = partps.Length ();
    for (size_t bb = 1; bb <= len_partps; bb++) {
      TYPE_AS_PatTypePair ptp (partps[bb]);
      SEQ<TYPE_AS_Pattern> patlist (ptp.get_pats ());
      TYPE_AS_Type tp (ptp.get_tp ());
      size_t len_patlist = patlist.Length ();
      for (size_t cc = 1; cc <= len_patlist; cc++) {
        GenPattern (modnm, patlist[cc]);
        if (cc < len_patlist) {
          this->LatexOutput << ",";
        }
      }
      this->LatexOutput << " : ";
      GenType (tp);

      if (bb < len_partps) {
        this->LatexOutput << ",";
      }
    }
  }

  this->LatexOutput << "}";
  this->LatexOutput << "[";

  size_t len_residtplist = residtplist.Length();
  for (size_t i = 1; i<= len_residtplist; i++) {
    const TYPE_AS_NameType & residtp (residtplist[i]);

    GenName (residtp.get_nm (), this->LatexOutput);
    this->LatexOutput << " : ";
    GenType (residtp.get_tp ());
    if (i < len_residtplist) {
      this->LatexOutput << ",";
    }
  }
  this->LatexOutput << "]";
  this->LatexOutput << endl;

  this->LatexOutput << "\\annlab[o]{";
  //if (!this->current_module.IsNil ())
  if (!modnm.IsNil ()) {
    //GenLabelName (this->current_module);
    GenLabelName (modnm, this->LatexOutput);
    this->LatexOutput << "`";
  }
  GenLabelName (id, this->LatexOutput);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  GenFnBody (modnm, body);

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  if (!post.IsNil()) {
    GenPost (modnm, post);
  }
  if (semicolon) {
    this->LatexOutput << ";";
  }
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{fn}";
  this->LatexOutput << endl;
}

void GenLatexOutput::GenImplFnDef (const Generic & modnm, const TYPE_AS_ImplFnDef & fn_def, bool semicolon)
{
  const TYPE_AS_Name & id                   (fn_def.GetRecord(pos_AS_ImplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpparms      (fn_def.GetSequence(pos_AS_ImplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps   (fn_def.GetSequence(pos_AS_ImplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & residtplist (fn_def.GetSequence(pos_AS_ImplFnDef_resnmtps));
  const Generic & pre                       (fn_def.GetField(pos_AS_ImplFnDef_fnpre));
  const Generic & post                      (fn_def.GetField(pos_AS_ImplFnDef_fnpost));
  const Int & access                        (fn_def.GetInt(pos_AS_ImplFnDef_access));
  const Bool & statiC                       (fn_def.GetBool(pos_AS_ImplFnDef_stat));

  if (access.GetValue() != DEFAULT_AS || statiC.GetValue()) {
    this->LatexOutput << "\\kw{";
    GenAccess(access, fn_def, false, false);
    if (access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
      this->LatexOutput << LATEXSPACE2;
      GenStatic(statiC, fn_def, false, false);
    }
    this->LatexOutput << "}";
  }

  this->LatexOutput << "\\begin{fn}[i]{";
  GenName (id, this->LatexOutput);

  // we have to put the polymorphic type vars in the function name,
  // because the macros do not offer another solution
  if (!tpparms.IsEmpty()) {
    this->LatexOutput << "[";
    size_t len_tpparms = tpparms.Length ();
    for (size_t bb = 1; bb <= len_tpparms; bb++) {
      TYPE_AS_TypeVar tv (tpparms[bb]);
      GenTypeVar (tv);
      if (bb < len_tpparms) {
        this->LatexOutput << ",";
      }
    }
    this->LatexOutput << "]";
  }
  this->LatexOutput << "}";

  if (this->INDEX) {
    GenIndex (id, fct_index);
  }
  this->LatexOutput << "%";
  this->LatexOutput << endl;

  this->LatexOutput << "\\parms{";
  if (!partps.IsEmpty()) {
    size_t len_partps = partps.Length ();
    for (size_t bb = 1; bb <= len_partps; bb++) {
      TYPE_AS_PatTypePair ptp (partps[bb]);
      SEQ<TYPE_AS_Pattern> patlist (ptp.get_pats ());
      TYPE_AS_Type tp (ptp.get_tp ());
      size_t len_patlist = patlist.Length ();
      for (size_t cc = 1; cc <= len_patlist; cc++) {
        GenPattern (modnm, patlist[cc]);
        if (cc < len_patlist) {
          this->LatexOutput << ",";
        }
      }
      this->LatexOutput << " : ";
      GenType (tp);

      if (bb < len_partps) {
        this->LatexOutput << ",";
      }
    }
  }
  this->LatexOutput << "}";

  this->LatexOutput << "[";
  size_t len_residtplist = residtplist.Length();
  for (size_t i = 1; i <= len_residtplist; i++) {
    TYPE_AS_NameType residtp (residtplist[i]);

    GenName (residtp.get_nm (), this->LatexOutput);
    this->LatexOutput << " : ";
    GenType (residtp.get_tp ());
    if (i < len_residtplist) {
      this->LatexOutput << ",";
    }
  }
  this->LatexOutput << "]";
  this->LatexOutput << endl;

  this->LatexOutput << "\\annlab[o]{";
  //if (!this->current_module.IsNil ())
  if (!modnm.IsNil ()) {
    //GenLabelName (this->current_module);
    GenLabelName (modnm, this->LatexOutput);
    this->LatexOutput << "`";
  }
  GenLabelName (id, this->LatexOutput);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  GenPost (modnm, post);

  if (semicolon) {
    this->LatexOutput << ";";
  }
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{fn}";
  this->LatexOutput << endl;
}

void GenLatexOutput::GenPre (const Generic & modnm, const TYPE_AS_Expr & pre)
{
  LatexOutput << endl;
  LatexOutput << "\\begin{precond}";
  LatexOutput << endl;
  GenExpr (modnm, pre);
  LatexOutput << endl;
  LatexOutput << "\\end{precond}";
}

void GenLatexOutput::GenPost (const Generic & modnm, const TYPE_AS_Expr & post)
{
  LatexOutput << endl;
  LatexOutput << "\\begin{postcond}";
  LatexOutput << endl;
  this->post_condition_scope = true;
  GenExpr (modnm, post);
  this->post_condition_scope = false;
  LatexOutput << endl;
  LatexOutput << "\\end{postcond}";
}

void GenLatexOutput::GenFnBody (const Generic & modnm, const TYPE_AS_FnBody & fnbody)
{
  const Generic & body (fnbody.GetField(pos_AS_FnBody_body));
  if (body.IsRecord()) {
    GenExpr(modnm, body);
  }
#ifdef VDMPP
  else if (Int(body).GetValue() == SUBRESP) {
    LatexOutput << "\\issubclassresp";
  }
#endif // VDMPP
  else {
    LatexOutput << "\\isnotyetspec";
  }
}

void GenLatexOutput::GenOpBody (const Generic & modnm, const TYPE_AS_OpBody & opbody)
{
  const Generic & body(opbody.GetField(pos_AS_OpBody_body));
  if (body.IsRecord()) {
    GenStmt(modnm, body);
  }
#ifdef VDMPP
  else if (Int(body).GetValue() == SUBRESP) {
    LatexOutput << "\\issubclassresp";
  }
#endif // VDMPP
  else {
    LatexOutput << "\\isnotyetspec";
  }
}

/** Operation Definitions part **********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenOperationDef (const Generic & modnm, const TYPE_AS_OpDef & opdef, bool semicolon)
{
  switch(opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { GenExplOpDef (modnm, opdef, semicolon); break; }
    case TAG_TYPE_AS_ExtExplOpDef: { GenExtExplOpDef(modnm, opdef, semicolon); break; }
    case TAG_TYPE_AS_ImplOpDef:    { GenImplOpDef (modnm, opdef, semicolon); break; }
    default: break;
  }
  this->LatexOutput.flush ();
}

void GenLatexOutput::GenExplOpDef (const Generic & modnm, const TYPE_AS_ExplOpDef & op_def, bool semicolon)
{
#ifdef PROTO
  if (Settings.GetPrettyPrintRTI())
    //CalculateValCov (this->current_module, op_def, LatexOutput);
    CalculateValCov (modnm, op_def, LatexOutput);
#endif //PROTO

  const TYPE_AS_Name & id            (op_def.GetRecord(pos_AS_ExplOpDef_nm));
  const TYPE_AS_OpType & tp          (op_def.GetRecord(pos_AS_ExplOpDef_tp));
  const TYPE_AS_OpBody & body        (op_def.GetRecord(pos_AS_ExplOpDef_body));
  const SEQ<TYPE_AS_Pattern> & parms (op_def.GetSequence(pos_AS_ExplOpDef_parms));
  const Generic & pre                (op_def.GetField(pos_AS_ExplOpDef_oppre));
  const Generic & post               (op_def.GetField(pos_AS_ExplOpDef_oppost));
  const Int & access                 (op_def.GetInt(pos_AS_ExplOpDef_access));
  const Bool & statiC                (op_def.GetBool(pos_AS_ExplOpDef_stat));
  const Bool & sync                  (op_def.GetBool(pos_AS_ExplOpDef_opsync));

  if (access.GetValue() != DEFAULT_AS || statiC.GetValue() || !sync) {
    this->LatexOutput << "\\kw{";
    if (!sync) {
      GenAsync(sync, op_def, false, false);
      if (access.GetValue() != DEFAULT_AS || statiC.GetValue()) {
        this->LatexOutput << LATEXSPACE2;
      }
    }
    GenAccess(access, op_def, false, false);
    if (access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
      this->LatexOutput << LATEXSPACE2;
      GenStatic(statiC, op_def, false, false);
    }
    this->LatexOutput << "}";
  }

  this->LatexOutput << "\\begin{op}[e]{";
  GenName (id, this->LatexOutput);
  this->LatexOutput << "}";

  if (this->INDEX) {
    GenIndex (id, fct_index);
  }
  this->LatexOutput << "%";
  this->LatexOutput << endl;
  this->LatexOutput << "\\signature{";
  GenOpType (tp);
  this->LatexOutput << "}";
  this->LatexOutput << endl;
  this->LatexOutput << "\\parms{";

  if (!parms.IsEmpty()) {
    size_t len = parms.Length ();
    for (size_t bb = 1; bb <= len; bb++) {
      TYPE_AS_Pattern pat (parms[bb]);
      GenPattern (modnm, pat);
      if (bb < len)
        LatexOutput << ",";
    }
  }
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->LatexOutput << "\\annlab[o]{";
  if (!modnm.IsNil ()) {
    GenLabelName (modnm, this->LatexOutput);
    this->LatexOutput << "`";
  }
  GenLabelName (id, this->LatexOutput);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenOpBody(modnm, body);

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  if (!post.IsNil()) {
    GenPost(modnm, post);
  }

  if (semicolon) {
    this->LatexOutput << ";";
  }
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{op}";
  this->LatexOutput << endl;
}


void GenLatexOutput::GenExtExplOpDef (const Generic & modnm, const TYPE_AS_ExtExplOpDef & op_def, bool semicolon)
{
  const TYPE_AS_Name & id                   (op_def.GetRecord(pos_AS_ExtExplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps   (op_def.GetSequence(pos_AS_ExtExplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & residtplist (op_def.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  const TYPE_AS_OpBody & body               (op_def.GetRecord(pos_AS_ExtExplOpDef_body));
  const SEQ<TYPE_AS_ExtInf> & opext         (op_def.GetSequence(pos_AS_ExtExplOpDef_opext));
  const SEQ<TYPE_AS_Error> & operr          (op_def.GetSequence(pos_AS_ExtExplOpDef_excps));
  const Generic & pre                       (op_def.GetField(pos_AS_ExtExplOpDef_oppre));
  const Generic & post                      (op_def.GetField(pos_AS_ExtExplOpDef_oppost));
  const Int & access                        (op_def.GetInt(pos_AS_ExtExplOpDef_access));
  const Bool & statiC                       (op_def.GetBool(pos_AS_ExtExplOpDef_stat));
  const Bool & sync                         (op_def.GetBool(pos_AS_ExtExplOpDef_opsync));

  if (access.GetValue() != DEFAULT_AS || statiC.GetValue() || !sync) {
    LatexOutput << "\\kw{";
    if (!sync) {
      GenAsync(sync, op_def, false, false);
      if (access.GetValue() != DEFAULT_AS || statiC.GetValue())
        LatexOutput << LATEXSPACE2;
    }
    GenAccess(access, op_def, false, false);
    if (access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
      LatexOutput << LATEXSPACE2;
      GenStatic(statiC, op_def, false, false);
    }
    LatexOutput << "}";
  }

  LatexOutput << "\\begin{op}[e]{";
  GenName (id, this->LatexOutput);
  LatexOutput << "}";

  if (this->INDEX) {
    GenIndex (id, fct_index);
  }
  LatexOutput << "%";
  LatexOutput << endl;
  LatexOutput << "\\parms{";

  if (!partps.IsEmpty()) {
    size_t len_partps = partps.Length ();
    for (size_t bb = 1; bb <= len_partps; bb++) {
      TYPE_AS_PatTypePair pattp (partps[bb]);
      SEQ<TYPE_AS_Pattern> patlist (pattp.get_pats ());
      TYPE_AS_Type tp (pattp.get_tp ());
      size_t len_patlist = patlist.Length ();
      for (size_t cc = 1; cc <= len_patlist; cc++) {
        GenPattern (modnm, patlist[cc]);
        if (cc < len_patlist) {
          LatexOutput << ",";
        }
      }
      LatexOutput << " : ";
      GenType (tp);

      if (bb < len_partps) {
        LatexOutput << ",";
      }
    }
  }
  LatexOutput << "}";

  if (residtplist.Length() > 0) {
    LatexOutput << "[";
    size_t len_residtplist = residtplist.Length();
    for (size_t i = 1; i <= len_residtplist; i++) {
      TYPE_AS_NameType residtp (residtplist[i]);
      GenName (residtp.get_nm (), this->LatexOutput);
      LatexOutput << " : ";
      GenType (residtp.get_tp ());
      if (i < (size_t)(residtplist.Length())) {
        LatexOutput << ",";
      }
    }
    LatexOutput << "]";
  }

  LatexOutput << "\\annlab[o]{";
  if (!modnm.IsNil ()) {
    GenLabelName (modnm, this->LatexOutput);
    LatexOutput << "`";
  }
  GenLabelName (id, this->LatexOutput);
  LatexOutput << "}";

  this->linefeed = false;
  GenOpBody(modnm, body);

  GenExternals (opext);

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  if (!post.IsNil()) {
    GenPost (modnm, post);
  }
  GenExceptions (modnm, operr);

  if (semicolon) {
    LatexOutput << ";";
  }
  LatexOutput << endl;
  LatexOutput << "\\end{op}";
  LatexOutput << endl;
}

void GenLatexOutput::GenImplOpDef (const Generic & modnm, const TYPE_AS_ImplOpDef & op_def, bool semicolon)
{
  const TYPE_AS_Name & id                   (op_def.GetRecord(pos_AS_ImplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps   (op_def.GetSequence(pos_AS_ImplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & residtplist (op_def.GetSequence(pos_AS_ImplOpDef_resnmtps));
  const SEQ<TYPE_AS_ExtInf> & opext         (op_def.GetSequence(pos_AS_ImplOpDef_opext));
  const SEQ<TYPE_AS_Error> & operr          (op_def.GetSequence(pos_AS_ImplOpDef_excps));
  const Generic & pre                       (op_def.GetField(pos_AS_ImplOpDef_oppre));
  const TYPE_AS_Expr & post                 (op_def.GetRecord(pos_AS_ImplOpDef_oppost));
  const Int & access                        (op_def.GetInt(pos_AS_ImplOpDef_access));
  const Bool & statiC                       (op_def.GetBool(pos_AS_ImplOpDef_stat));
  const Bool & sync                         (op_def.GetBool(pos_AS_ImplOpDef_opsync));

  if (access.GetValue() != DEFAULT_AS || statiC.GetValue() || !sync) {
    LatexOutput << "\\kw{";
    if (!sync) {
      GenAsync(sync, op_def, false, false);
      if (access.GetValue() != DEFAULT_AS || statiC.GetValue()) {
        LatexOutput << LATEXSPACE2;
      }
    }
    GenAccess(access, op_def, false, false);
    if (access.GetValue() != DEFAULT_AS && statiC.GetValue()) {
      LatexOutput << LATEXSPACE2;
      GenStatic(statiC, op_def, false, false);
    }
    LatexOutput << "}";
  }

  LatexOutput << "\\begin{op}[i]{";
  GenName (id, this->LatexOutput);
  LatexOutput << "}";

  if (this->INDEX) {
    GenIndex (id, fct_index);
  }
  LatexOutput << "%";
  LatexOutput << endl;
  LatexOutput << "\\parms{";

  if (!partps.IsEmpty()) {
    size_t len_partps = partps.Length ();
    for (size_t bb = 1; bb <= len_partps; bb++) {
      TYPE_AS_PatTypePair pattp (partps[bb]);
      SEQ<TYPE_AS_Pattern> patlist (pattp.get_pats ());
      TYPE_AS_Type tp (pattp.get_tp ());
      size_t len_patlist = patlist.Length ();
      for (size_t cc = 1; cc <= len_patlist; cc++) {
        GenPattern (modnm, patlist[cc]);
        if (cc < len_patlist) {
          LatexOutput << ",";
        }
      }
      LatexOutput << " : ";
      GenType (tp);

      if (bb < len_partps) {
        LatexOutput << ",";
      }
    }
  }
  LatexOutput << "}";

  if (!residtplist.IsEmpty()) {
    LatexOutput << "[";
    size_t len_residtplist = residtplist.Length();
    for (size_t i = 1; i <= len_residtplist; i++) {
      TYPE_AS_NameType residtp (residtplist[i]);
      GenName (residtp.get_nm (), this->LatexOutput);
      LatexOutput << " : ";
      GenType (residtp.get_tp ());
      if (i < (size_t)(residtplist.Length())) {
        LatexOutput << ",";
      }
    }
    LatexOutput << "]";
  }

  LatexOutput << "\\annlab[o]{";
  if (!modnm.IsNil ()) {
    GenLabelName (modnm, this->LatexOutput);
    LatexOutput << "`";
  }
  GenLabelName (id, this->LatexOutput);
  LatexOutput << "}";

  GenExternals (opext);

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  GenPost (modnm, post);

  GenExceptions (modnm, operr);

  if (semicolon) {
    LatexOutput << ";";
  }
  LatexOutput << endl;
  LatexOutput << "\\end{op}";
  LatexOutput << endl;
}

// GenExternals
void GenLatexOutput::GenExternals (const SEQ<TYPE_AS_ExtInf> & Einf)
{
  if (!Einf.IsEmpty()) {
    LatexOutput << endl;
    LatexOutput << "\\begin{externals}";
    LatexOutput << endl;

    size_t len_einf = Einf.Length();
    for (size_t i = 1; i <= len_einf; i++) {
      const TYPE_AS_ExtInf & ext (Einf[i]);
      const TYPE_AS_Mode & mode (ext.GetInt(pos_AS_ExtInf_mode));
      const SEQ<TYPE_AS_Name> & rest (ext.GetSequence(pos_AS_ExtInf_vars));
      const Generic & tp (ext.GetField(pos_AS_ExtInf_tp));
      if (Int(mode).GetValue() == READ) {
        LatexOutput << "\\Rd ";
      }
      else {
        LatexOutput << "\\Wr ";
      }
      size_t len_rest = rest.Length ();
      for (size_t cc = 1; cc <= len_rest ; cc++) {
        GenName (rest[cc], this->LatexOutput);
        if (cc < len_rest) {
          LatexOutput << ",";
        }
        else {
          LatexOutput << " :";
        }
      }

      if (tp.IsNil ()) {
        LatexOutput << LF;
      }
      else {
        LatexOutput << " {";
        GenType (tp);
        LatexOutput << "} " << LF;
      }
    }

    LatexOutput << "\\end{externals}";
  }
}

// GenExceptions
// opexc :
void GenLatexOutput::GenExceptions (const Generic & modnm, const SEQ<TYPE_AS_Error> & opexc)
{
  if (!opexc.IsEmpty ()) {
    this->LatexOutput << endl;
    this->LatexOutput << "\\begin{exceptions}";
    this->LatexOutput << endl;

    size_t len_opexc = opexc.Length();
    for (size_t i = 1; i <= len_opexc; i++) {
      const TYPE_AS_Error & err (opexc[i]);
      const TYPE_AS_Name & id     (err.GetRecord(pos_AS_Error_nm));
      const TYPE_AS_Expr & cond   (err.GetRecord(pos_AS_Error_cond));
      const TYPE_AS_Expr & action (err.GetRecord(pos_AS_Error_action));

      this->LatexOutput << "\\err{";
      GenName (id, this->LatexOutput);
      this->LatexOutput << "}{";
      GenExpr (modnm, cond);
      this->LatexOutput << " \\To ";
      GenExpr (modnm, action);
      this->LatexOutput << "}";
      this->LatexOutput << endl;
    }
    this->LatexOutput << "\\end{exceptions}";
  }
}

/** Statement part **********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenStmt(const Generic & modnm, const TYPE_AS_Stmt & stmt)
{
  // Indicate if  statement is covered

  bool not_covered = false;
  if (Settings.GetPrettyPrintRTI() &&
      !this->not_covered_area &&
      !RTINFO::Is_Covered (stmt, *this->contextinfo)) {
    not_covered = true;
    this->not_covered_area = true;
    this->LatexOutput << "\\color{not-covered}";
  }

  // gen statement
  switch (stmt.GetTag ()) {
    case TAG_TYPE_AS_DefStmt:           { GenDefStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_LetStmt:           { GenLetStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_LetBeSTStmt:       { GenLetBeSTStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_AssignStmt:        { GenAssignStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_AtomicAssignStmt:  { GenAtomicAssignStmt(modnm, stmt); break; }
    case TAG_TYPE_AS_SeqForLoopStmt:    { GenSeqForLoopStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_SetForLoopStmt:    { GenSetForLoopStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_IndexForLoopStmt:  { GenIndexForLoopStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_WhileLoopStmt:     { GenWhileLoopStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_CallStmt:          { GenCallStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_ErrorStmt:         { GenErrorStmt (); break; }
    case TAG_TYPE_AS_AlwaysStmt:        { GenAlwaysStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_ExitStmt:          { GenExitStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_TrapStmt:          { GenTrapStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_RecTrapStmt:       { GenRecTrapStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_NonDetStmt:        { GenNonDetStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_ReturnStmt:        { GenReturnStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_IfStmt:            { GenIfStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_CasesStmt:         { GenCasesStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_BlockStmt:         { GenBlockStmt (modnm, stmt); break; }
    case TAG_TYPE_AS_IdentStmt:         { GenIdentStmt (); break; }
    case TAG_TYPE_AS_SpecificationStmt: { GenSpecificationStmt(modnm, stmt); break; }
#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt:         { GenStartStmt(modnm, stmt); break; }
    case TAG_TYPE_AS_StartListStmt:     { GenStartListStmt(modnm, stmt); break; }
    case TAG_TYPE_AS_StopStmt:          { GenStopStmt(modnm, stmt); break; }
    case TAG_TYPE_AS_StopListStmt:      { GenStopListStmt(modnm, stmt); break; }
#ifdef VICE
    case TAG_TYPE_AS_DurationStmt:      { GenDurationStmt(modnm, stmt); break; }
    case TAG_TYPE_AS_CycleStmt:         { GenCycleStmt(modnm, stmt); break; }
#endif // VICE
#endif // VDMPP
  
    default: { ErrMsg1 (stmt,L"Unknown statement encountered"); }
  }
  // post Covered
  if (Settings.GetPrettyPrintRTI() && not_covered) {
    this->not_covered_area = false;
    this->LatexOutput << "\\color{covered}";
  }
}

// GenDefStmt
// defs : AS`DefStmt
void GenLatexOutput::GenDefStmt(const Generic & modnm, const TYPE_AS_DefStmt & defs)
{
  const type_dd2PL & value_l   (defs.GetTuple(pos_AS_DefStmt_value));
  const TYPE_AS_Stmt & in_stmt (defs.GetRecord(pos_AS_DefStmt_In));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{defstmt}";
  this->LatexOutput << endl;

  size_t len_value_l = value_l.Length();
  for (size_t i = 1; i <= len_value_l; i++) {
    const Tuple & t (value_l[i]);
    const TYPE_AS_PatternBind & bind (t.GetRecord (1));
    const TYPE_AS_Expr & expr        (t.GetRecord (2));

    this->LatexOutput << "\\eqdef{";
    GenPatternBind (modnm, bind);
    this->LatexOutput << "}";

    int l1 = PatBindLine (bind);
    int l2 = GetLine (expr);

    if (l2 > l1) {
      this->LatexOutput << LF;
    }
    this->LatexOutput << "{";
    GenExpr (modnm, expr);
    this->LatexOutput << "}";
    this->LatexOutput << endl;
  }

  this->linefeed = false;
  this->LatexOutput << "\\end{defstmt}" << LF;
  GenStmt (modnm, in_stmt);
}

// GenLetStmt
// ls : AS`LetStmt
void GenLatexOutput::GenLetStmt(const Generic & modnm, const TYPE_AS_LetStmt & ls)
{
  const SEQ<TYPE_AS_LocalDef> & localdef (ls.GetSequence(pos_AS_LetStmt_localdef));
  const TYPE_AS_Stmt & in_stmt           (ls.GetRecord(pos_AS_LetStmt_In));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{letstmt}";
  this->LatexOutput << endl;

  size_t len_localdef = localdef.Length();
  for (size_t i = 1; i <= len_localdef; i++) {
    const TYPE_AS_LocalDef & def (localdef[i]);
    switch(def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        TYPE_AS_ValueDef vd (def);
        TYPE_AS_Pattern pat (vd.get_pat());
        TYPE_AS_Expr val (vd.get_val());
        Generic tp (vd.get_tp());

        this->LatexOutput << "\\patdef{";
        GenPattern (modnm, pat);

        if (! tp.IsNil ()) {
          this->LatexOutput << " : ";
          GenType (tp);
        }
        this->LatexOutput << "}";

        int l1 = PatLine (pat);
        int l2 = GetLine (val);

        if (l2 > l1) {
          this->LatexOutput << LF;
        }
        this->LatexOutput << "{";
        GenExpr (modnm, val);
        this->LatexOutput << "}";
        this->LatexOutput << endl;
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef:
      case TAG_TYPE_AS_ImplFnDef: {
        GenFunctionDef (modnm, def, false);
        break;
      }
      default:
        break;
    }
  }

  this->linefeed = false;
  this->LatexOutput << "\\end{letstmt}" << LF;
  GenStmt (modnm, in_stmt);
}

// GenLetBeSTStmt
// lbsts : AS`LetBeSTStmt
void GenLatexOutput::GenLetBeSTStmt(const Generic & modnm, const TYPE_AS_LetBeSTStmt & lbsts)
{
  const SEQ<TYPE_AS_MultBind> & bind_l (lbsts.GetSequence(pos_AS_LetBeSTStmt_bind));
  const Generic & st_expr      (lbsts.GetField(pos_AS_LetBeSTStmt_St));
  const TYPE_AS_Stmt & in_stmt (lbsts.GetRecord(pos_AS_LetBeSTStmt_In));

  this->LatexOutput << "\\Letbe";

  if (st_expr.IsNil() ||
      (st_expr.Is (TAG_TYPE_AS_BoolLit) && Record(st_expr).GetBoolValue(pos_AS_BoolLit_val))) {
    this->LatexOutput << "* ";

    int l1 = BindLine (bind_l[1]);

    size_t len_bind_l = bind_l.Length();
    for (size_t idx = 1; idx <= len_bind_l; idx++) {
      const TYPE_AS_MultBind & bind (bind_l[idx]);
      int l2 = BindLine (bind);
      if (idx > 1) {
        LatexOutput << ",";
      }
      if (l2 > l1) {
        LatexOutput << LF;
      }
      GenMultBind (modnm, bind);
      l1 = l2;
    }
  }
  else {
    this->LatexOutput << LATEXSPACE2;

    int l1 = BindLine (bind_l[1]);

    size_t len_bind_l = bind_l.Length();
    for (size_t idx = 1; idx <= len_bind_l; idx++) {
      const TYPE_AS_MultBind & bind (bind_l[idx]);
      int l2 = BindLine (bind);
      if (idx > 1) {
        LatexOutput << ",";
      }
      if (l2 > l1) {
        LatexOutput << LF;
      }
      GenMultBind (modnm, bind);
      l1 = l2;
    }

    this->LatexOutput << " \\Best";

    int l3 = GetLine (st_expr);

    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    this->linefeed = false;
    this->LatexOutput << LATEXSPACE2;
    GenExpr (modnm, st_expr);
  }

  this->linefeed = false;
  this->LatexOutput << " \\Lin" << LF;
  GenStmt (modnm, in_stmt);
}

// GenAssignStmt
// as : AS`AssignStmt
void GenLatexOutput::GenAssignStmt(const Generic & modnm, const TYPE_AS_AssignStmt & as)
{
  const TYPE_AS_StateDesignator & sd (as.GetRecord(pos_AS_AssignStmt_lhs));
  const TYPE_AS_Expr & expr          (as.GetRecord(pos_AS_AssignStmt_rhs));

  this->LatexOutput << "\\ass{";
  GenStateDesignator (modnm, sd);
  this->LatexOutput << "}";

  this->LatexOutput << "{";
  GenExpr (modnm, expr);
  this->LatexOutput << "}";
}

// GenAtomicAssignStmt
// aas : AS`AtomicAssignStmt
void GenLatexOutput::GenAtomicAssignStmt(const Generic & modnm, const TYPE_AS_AtomicAssignStmt & aas)
{
  const SEQ<TYPE_AS_AssignStmt> & atm_l (aas.GetSequence(pos_AS_AtomicAssignStmt_atm));

  this->LatexOutput << "\\kw{atomic}";
  this->LatexOutput << endl;
  this->LatexOutput << "\\begin{blockstmt}";

  this->linefeed = false;
  size_t len_atm_l = atm_l.Length ();
  for (size_t i = 1; i <= len_atm_l; i++) {
    GenStmt (modnm, atm_l[i]);
    if (i < len_atm_l) {
      this->LatexOutput << ";" << LF;
    }
    else {
      this->LatexOutput << endl;
    }
  }

  this->LatexOutput << "\\end{blockstmt}";
}

// GenStateDesignator
// sd : AS`StateDesignator
void GenLatexOutput::GenStateDesignator(const Generic & modnm, const TYPE_AS_StateDesignator & sd)
{
  switch(sd.GetTag()) {
    case TAG_TYPE_AS_Name:        { GenName (sd, this->LatexOutput); break; }
    case TAG_TYPE_AS_MapOrSeqRef: { GenMapOrSeqRef (modnm, sd); break; }
    case TAG_TYPE_AS_FieldRef:    { GenFieldRef (modnm, sd); break; }
    case TAG_TYPE_AS_NarrowExpr:  { GenNarrowExpr (modnm, sd); }
    default: { break; }
  }
}

// GenMapOrSeqRef
// mosr : AS`MapOrSeqRef
void GenLatexOutput::GenMapOrSeqRef(const Generic & modnm, const TYPE_AS_MapOrSeqRef & mosr)
{
  const TYPE_AS_StateDesignator & sd (mosr.GetRecord(pos_AS_MapOrSeqRef_var));
  const TYPE_AS_Expr & dom           (mosr.GetRecord(pos_AS_MapOrSeqRef_arg));

  GenStateDesignator (modnm, sd);
  this->LatexOutput << "\\pex{";
  GenExpr (modnm, dom);
  this->LatexOutput << "}";
}

// GenFieldRef
// fr : AS`FieldRef
void GenLatexOutput::GenFieldRef (const Generic & modnm, const TYPE_AS_FieldRef & fr)
{
  const TYPE_AS_StateDesignator & sd (fr.GetRecord(pos_AS_FieldRef_var));
  const TYPE_AS_Name & sel           (fr.GetRecord(pos_AS_FieldRef_sel));

  GenStateDesignator (modnm, sd);
  this->LatexOutput << ".";
  GenName (sel, this->LatexOutput);
}

// GenSeqForLoopStmt
// sfls : AS`SeqForLoopStmt
void GenLatexOutput::GenSeqForLoopStmt (const Generic & modnm, const TYPE_AS_SeqForLoopStmt & sfls)
{
  const TYPE_AS_PatternBind & bind (sfls.GetRecord(pos_AS_SeqForLoopStmt_cv));
//  const Generic & dirn             (sfls.GetField(pos_AS_SeqForLoopStmt_dirn));
  const TYPE_AS_Expr & fseq        (sfls.GetRecord(pos_AS_SeqForLoopStmt_fseq));
  const TYPE_AS_Stmt & body        (sfls.GetRecord(pos_AS_SeqForLoopStmt_body));

  int l1 = PatBindLine (bind);  // was BindLine -> return -1 as default case.
  int l2 = GetLine (fseq);

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{seqfor}{";

  GenPatternBind (modnm, bind);
  this->LatexOutput << "}";

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
//  if (dirn.IsQuote ())
//    this->LatexOutput << "[rev]";

  this->LatexOutput << "{";
  GenExpr (modnm, fseq);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenStmt (modnm, body);
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{seqfor}";
}

// GenSetForLoopStmt
// sfls : AS`SetForLoopStmt
void GenLatexOutput::GenSetForLoopStmt (const Generic & modnm, const TYPE_AS_SetForLoopStmt & sfls)
{
  const TYPE_AS_Pattern & pat (sfls.GetRecord(pos_AS_SetForLoopStmt_cv));
  const TYPE_AS_Expr & set    (sfls.GetRecord(pos_AS_SetForLoopStmt_fset));
  const TYPE_AS_Stmt & body   (sfls.GetRecord(pos_AS_SetForLoopStmt_body));

  int l1 = PatLine (pat);
  int l2 = GetLine (set);

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{setfor}{";

  GenPattern (modnm, pat);
  this->LatexOutput << "}";

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  this->LatexOutput << "{";
  GenExpr (modnm, set);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenStmt (modnm, body);
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{setfor}";
}

// GenIndexForLoopStmt
// ifls : AS`IndexForLoopStmt
void GenLatexOutput::GenIndexForLoopStmt (const Generic & modnm, const TYPE_AS_IndexForLoopStmt & ifls)
{
  const TYPE_AS_Name & id   (ifls.GetRecord(pos_AS_IndexForLoopStmt_cv));
  const TYPE_AS_Expr & lb_e (ifls.GetRecord(pos_AS_IndexForLoopStmt_lb));
  const TYPE_AS_Expr & ub_e (ifls.GetRecord(pos_AS_IndexForLoopStmt_ub));
  const Generic & by_e      (ifls.GetField(pos_AS_IndexForLoopStmt_By));
  const TYPE_AS_Stmt & body (ifls.GetRecord(pos_AS_IndexForLoopStmt_body));

  int l1 = GetLine (ifls);
  int l2 = GetLine (lb_e);
  int l3 = GetLine (ub_e);
  int l4 = GetLine (by_e);

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{indfor}{";

  GenName (id, this->LatexOutput);
  this->LatexOutput << "}";

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  this->LatexOutput << "{";
  GenExpr (modnm, lb_e);
  this->LatexOutput << "}";

  if (l3 > l2) {
    this->LatexOutput << LF;
  }
  this->LatexOutput << "{";
  GenExpr (modnm, ub_e);
  this->LatexOutput << "}";

  if (by_e.IsNil ()) {
    this->LatexOutput << endl;
  }
  else {
    TYPE_AS_Expr incr_by (by_e);

//    if (!( incr_by.Is(TAG_TYPE_AS_RealLit) &&
//        ((Real) incr_by.GetField(1)).GetValue() == 1.0 )) {
      if (l4 > l3) {
        this->LatexOutput << LF;
      }
      this->LatexOutput << "[";
      GenExpr (modnm, by_e);
      this->LatexOutput << "]";
      this->LatexOutput << endl;
//    }
  }

  this->linefeed = false;
  GenStmt (modnm, body);
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{indfor}";
}

// GenWhileLoopStmt
// wls : AS`WhileLoopStmt
void GenLatexOutput::GenWhileLoopStmt (const Generic & modnm, const TYPE_AS_WhileLoopStmt & wls)
{
  const TYPE_AS_Expr & test (wls.GetRecord(pos_AS_WhileLoopStmt_test));
  const TYPE_AS_Stmt & body (wls.GetRecord(pos_AS_WhileLoopStmt_body));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{while}{";

  GenExpr (modnm, test);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenStmt (modnm, body);
  this->LatexOutput << endl;
  this->LatexOutput << "\\end{while}";
}

// GenCallStmt
// cs : AS`CallStmt
void GenLatexOutput::GenCallStmt (const Generic & modnm, const TYPE_AS_CallStmt & cs)
{
#ifdef VDMPP
  const Generic & obj            (cs.GetField(pos_AS_CallStmt_obj));
#endif // VDMPP
  const TYPE_AS_Name & oprt      (cs.GetRecord(pos_AS_CallStmt_oprt));
  const SEQ<TYPE_AS_Expr> & args (cs.GetSequence(pos_AS_CallStmt_args));

#ifdef VDMPP
  if (! obj.IsNil ()) {
    this->linefeed = false;
    bool isName = obj.IsRecord() && Record(obj).Is(TAG_TYPE_AS_Name);
    int len = isName ? Record(obj).GetSequence(pos_AS_Name_ids).Length() : 0;
    if(isName && len == 1) {
      this->LatexOutput << "$";
    }
    GenExpr(modnm, obj);
    if(isName && len == 1) {
      this->LatexOutput << "$";
    }
    this->LatexOutput << ".";
  }
#endif // VDMPP

  this->LatexOutput << "\\call{";

  GenName (oprt, this->LatexOutput);

  if (this->INDEX) {
    GenIndex (oprt, fctocc_index);
  }
  this->LatexOutput << "}";

  int l1 = GetLine (oprt);

  if (!args.IsEmpty()) {
    int l2 = GetLine (args[1]);
    if (l2 > l1) {
      this->LatexOutput << LF;
    }
  }

  this->LatexOutput << "{";
  bool first(true);
  size_t len_args = args.Length ();
  for (size_t bb = 1; bb <= len_args; bb++) {
    int l3 = GetLine (args[bb]);

    if (l3 > l1 && !first) {
      this->LatexOutput << LF;
    }
    GenExpr (modnm, args[bb]);
    if (bb < len_args) {
      this->LatexOutput << ",";
    }
    l1 = l3;
    first = false;
  }

  this->LatexOutput << "}";
}

// GenReturnStmt
// rs : AS`ReturnStmt
void GenLatexOutput::GenReturnStmt(const Generic & modnm, const TYPE_AS_ReturnStmt & rs)
{
  const Generic & ret_e (rs.GetField(pos_AS_ReturnStmt_val));

  this->LatexOutput << "\\return{";

  if (! ret_e.IsNil ()) {
    GenExpr (modnm, ret_e);
  }
  else {
    this->LatexOutput <<"\\mbox{}";
  }

  this->LatexOutput << "}";
}

// GenIfStmt
// is : AS`IfStmt
void GenLatexOutput::GenIfStmt(const Generic & modnm, const TYPE_AS_IfStmt & is)
{
  const TYPE_AS_Expr & test_e            (is.GetRecord(pos_AS_IfStmt_test));
  const TYPE_AS_Stmt & cons              (is.GetRecord(pos_AS_IfStmt_cons));
  const SEQ<TYPE_AS_ElseifStmt> & elif_l (is.GetSequence(pos_AS_IfStmt_elsif));
  const Generic & altn                   (is.GetField(pos_AS_IfStmt_altn));

  this->LatexOutput << "\\If ";

  this->linefeed = false;
  GenExpr (modnm, test_e);
  this->LatexOutput << endl;
  this->LatexOutput << "\\Then" << LF;
  GenStmt (modnm, cons);

  size_t len_elif_l = elif_l.Length();
  for (size_t i = 1; i <= len_elif_l; i++) {
    const TYPE_AS_ElseifStmt & elif (elif_l[i]);
    const TYPE_AS_Expr & elif_test (elif.GetRecord(pos_AS_ElseifStmt_test));
    const TYPE_AS_Stmt & elif_cons (elif.GetRecord(pos_AS_ElseifStmt_cons));

    this->LatexOutput << endl;
    this->LatexOutput << "\\Elseif ";
    GenExpr (modnm, elif_test);
    this->LatexOutput << endl;
    this->LatexOutput << "\\Then" << LF;
    GenStmt (modnm, elif_cons);
  }

  if (! altn.IsNil()) {
    this->LatexOutput << endl;
    this->LatexOutput << "\\Else" << LF;
    GenStmt (modnm, altn);
  }
  this->LatexOutput << endl;
  this->LatexOutput << "\\Fi";
}

// GenCasesStmt
// cs : AS`CasesStmt
void GenLatexOutput::GenCasesStmt(const Generic & modnm, const TYPE_AS_CasesStmt & cs)
{
  const TYPE_AS_Expr & test_e              (cs.GetRecord(pos_AS_CasesStmt_sel));
  const SEQ<TYPE_AS_CasesStmtAltn> & alt_l (cs.GetSequence(pos_AS_CasesStmt_altns));
  const Generic & others                   (cs.GetField(pos_AS_CasesStmt_Others));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{Cases}{";

  GenExpr (modnm, test_e);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  size_t len_alt_l = alt_l.Length();
  for (size_t i = 1; i <= len_alt_l; i++) {
    const TYPE_AS_CasesStmtAltn & alt (alt_l[i]);
    const SEQ<TYPE_AS_Pattern> & pat_l (alt.GetSequence(pos_AS_CasesStmtAltn_match));
    const TYPE_AS_Stmt & stmt          (alt.GetRecord(pos_AS_CasesStmtAltn_body));

    this->LatexOutput << "\\alt{";

    int l1 = PatLine (pat_l[1]);

    size_t len_pat_l = pat_l.Length ();
    for (size_t cc = 1; cc <= len_pat_l; cc++) {
      int l2 = PatLine (pat_l[cc]);

      if (l2 > l1) {
        this->LatexOutput << LF;
      }
      bool not_covered = false;
      if (Settings.GetPrettyPrintRTI() &&
          !this->not_covered_area &&
          !RTINFO::Is_Covered (pat_l[cc], *this->contextinfo)) {
        not_covered = true;
        this->not_covered_area = true;
        this->LatexOutput << "\\color{not-covered}";
      }
      GenPattern (modnm, pat_l[cc]);
      // post Covered
      if (Settings.GetPrettyPrintRTI() && not_covered) {
        this->not_covered_area = false;
        this->LatexOutput << "\\color{covered}";
      }

      if (cc < len_pat_l) {
        this->LatexOutput << ",";
      }
      l1 = l2;
    }
    this->linefeed = false;
    this->LatexOutput << "}";

    int l3 = GetLine (stmt);

    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    this->LatexOutput << "{";
    GenStmt (modnm, stmt);
    this->LatexOutput << "}";
    this->LatexOutput << endl;
  }

  if (! others.IsNil ()) {
    this->linefeed = false;
    this->LatexOutput << "\\others{";
    GenStmt (modnm, others);
    this->LatexOutput << "}";
    this->LatexOutput << endl;
  }

  this->LatexOutput << "\\end{Cases}";
}

// GenErrorStmt
void GenLatexOutput::GenErrorStmt (void)
{
  this->LatexOutput << "\\Error ";
}

// GenAlwaysStmt
// as : AS`AlwaysStmt
void GenLatexOutput::GenAlwaysStmt (const Generic & modnm, const TYPE_AS_AlwaysStmt & as)
{
  const TYPE_AS_Stmt & stmt  (as.GetRecord(pos_AS_AlwaysStmt_Post));
  const TYPE_AS_Stmt & block (as.GetRecord(pos_AS_AlwaysStmt_body));

  this->LatexOutput << "\\always{";

  GenStmt (modnm, stmt);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenStmt(modnm, block);
}

// GenExitStmt
// es : AS`ExitStmt
void GenLatexOutput::GenExitStmt(const Generic & modnm, const TYPE_AS_ExitStmt & es)
{
  const Generic & exit_e (es.GetField(pos_AS_ExitStmt_expr));

  this->LatexOutput << "\\exit{";

  if (! exit_e.IsNil ()) {
    GenExpr (modnm, exit_e);
  }
  this->LatexOutput << "}";
}

// GenTrapStmt
// ts : AS`TrapStmt
void GenLatexOutput::GenTrapStmt (const Generic & modnm, const TYPE_AS_TrapStmt & ts)
{
  const TYPE_AS_PatternBind & bind (ts.GetRecord(pos_AS_TrapStmt_pat));
  const TYPE_AS_Stmt & stmt        (ts.GetRecord(pos_AS_TrapStmt_Post));
  const TYPE_AS_Stmt & block       (ts.GetRecord(pos_AS_TrapStmt_body));

  this->LatexOutput << "\\trap{";

  GenPatternBind (modnm, bind);

  this->LatexOutput << "}";

  int l1 = PatBindLine (bind);
  int l2 = GetLine (stmt);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  this->LatexOutput << "{\\ ";
  GenStmt (modnm, stmt);
  this->LatexOutput << "}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenStmt (modnm, block);
}

// GenRecTrapStmt
// rts : AS_RecTrapStmt
void GenLatexOutput:: GenRecTrapStmt (const Generic & modnm,const TYPE_AS_RecTrapStmt & rts)
{
  const SEQ<TYPE_AS_Trap> & trap_l (rts.GetSequence(pos_AS_RecTrapStmt_traps));
  const TYPE_AS_Stmt & block       (rts.GetRecord(pos_AS_RecTrapStmt_body));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{rectrap}";
  this->LatexOutput << endl;

  size_t len_trap_l = trap_l.Length();
  for (size_t i = 1; i <= len_trap_l; i++) {
    const TYPE_AS_Trap & trap (trap_l[i]);
    const TYPE_AS_PatternBind & bind (trap.GetRecord(pos_AS_Trap_match));
    const TYPE_AS_Stmt & stmt        (trap.GetRecord(pos_AS_Trap_trappost));

    this->linefeed = false;
    this->LatexOutput << "\\trap{";
    GenPatternBind (modnm, bind);
    this->LatexOutput << "}";

    int l1 = PatBindLine (bind);
    int l2 = GetLine (stmt);

    if (l2 > l1) {
      this->LatexOutput << LF;
    }
    this->LatexOutput << "{";
    GenStmt (modnm, stmt);
    this->LatexOutput << "}";
    this->LatexOutput << endl;
  }

  this->LatexOutput << "\\end{rectrap}";
  this->LatexOutput << endl;

  this->linefeed = false;
  GenStmt (modnm, block);
}

// GenAssignDef
// ad : AS`AssignDef
void GenLatexOutput::GenAssignDef (const Generic & modnm, const TYPE_AS_AssignDef & ad)
{
  const TYPE_AS_Name & var (ad.GetRecord(pos_AS_AssignDef_var));
  const TYPE_AS_Type & tp  (ad.GetRecord(pos_AS_AssignDef_tp));
  const Generic & init     (ad.GetField(pos_AS_AssignDef_dclinit));

  this->LatexOutput << endl;
  this->LatexOutput << "\\assdef{";
  GenName (var, this->LatexOutput);
  this->LatexOutput << "}{";
  GenType (tp);
  this->LatexOutput << "}";

  if (! init.IsNil ()) {
    this->linefeed = false;
    this->LatexOutput << "[{";
    GenExpr(modnm, init);
    this->LatexOutput << "}]";
  }
}

// GenBlockStmt
// bs : AS`BlockStmt
void GenLatexOutput::GenBlockStmt (const Generic & modnm, const TYPE_AS_BlockStmt & bs)
{
  const SEQ<TYPE_AS_AssignDef> & assdef_l (bs.GetSequence(pos_AS_BlockStmt_dcls));
  const SEQ<TYPE_AS_Stmt> & stmt_l        (bs.GetSequence(pos_AS_BlockStmt_stmts));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{blockstmt}";
  this->LatexOutput << endl;

  if (!assdef_l.IsEmpty()) {
    this->LatexOutput << "\\begin{dclstmt}";

    size_t len_assdef_l = assdef_l.Length();
    for (size_t i = 1; i <= len_assdef_l; i++) {
      GenAssignDef(modnm, assdef_l[i]);
    }

    this->LatexOutput << endl;
    this->LatexOutput << "\\end{dclstmt}";
    this->LatexOutput << endl;
  }

  this->linefeed = false;

  size_t len_stmt_l = stmt_l.Length ();
  for (size_t cc = 1; cc <= len_stmt_l; cc++) {
    GenStmt (modnm, stmt_l[cc]);
    if (cc < len_stmt_l) {
      this->LatexOutput << " ;" << LF;
    }
    else {
      this->LatexOutput << endl;
    }
  }

  this->LatexOutput << "\\end{blockstmt}";
}

// GenNonDetStmt
// nds : AS`NonDetStmt
void GenLatexOutput::GenNonDetStmt (const Generic & modnm, const TYPE_AS_NonDetStmt & nds)
{
  const SEQ<TYPE_AS_Stmt> & stmt_l (nds.GetSequence(pos_AS_NonDetStmt_stmts));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{nondetstmt}";
  this->LatexOutput << endl;

  this->linefeed = false;
  size_t len_stmt_l = stmt_l.Length ();
  for (size_t i = 1; i <= len_stmt_l; i++) {

    GenStmt (modnm, stmt_l[i]);
    if (i < len_stmt_l) {
      this->LatexOutput << "," << LF;
    }
    else {
      this->LatexOutput << endl;
    }
  }

  this->LatexOutput << "\\end{nondetstmt}";
}

// GenIdentStmt
// ids : AS`IdentStmt
void GenLatexOutput::GenIdentStmt (void)
{
  this->LatexOutput << "\\Skip ";
}

// GenSpecificationStmt
// ss : AS`SpecificationStmt
void GenLatexOutput::GenSpecificationStmt(const Generic & modnm, const TYPE_AS_SpecificationStmt& ss)
{
  const SEQ<TYPE_AS_ExtInf> & opext (ss.GetSequence(pos_AS_SpecificationStmt_opext));
  const Generic & pre               (ss.GetField(pos_AS_SpecificationStmt_oppre));
  const TYPE_AS_Expr & post         (ss.GetRecord(pos_AS_SpecificationStmt_oppost));
  const SEQ<TYPE_AS_Error> & excps  (ss.GetSequence(pos_AS_SpecificationStmt_excps));

  this->LatexOutput << "[";
  GenExternals (opext);

  if (!pre.IsNil()) {
    GenPre (modnm, pre);
  }
  GenPost (modnm, post);

  GenExceptions (modnm, excps);

  this->LatexOutput << "]";
}

#ifdef VDMPP
// GenStartStmt
// ss : AS`StartStmt
void GenLatexOutput::GenStartStmt(const Generic & modnm, const TYPE_AS_StartStmt & ss)
{
  const TYPE_AS_Expr & expr (ss.GetRecord(pos_AS_StartStmt_expr));

  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\start{";
  GenExpr(modnm, expr);
  this->LatexOutput << "}";
}

// GenStartListStmt
// sls : AS`StartListStmt
void GenLatexOutput::GenStartListStmt(const Generic & modnm, const TYPE_AS_StartListStmt & sls)
{
  const TYPE_AS_Expr & expr (sls.GetRecord(pos_AS_StartListStmt_expr));

  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\startlist{";
  GenExpr(modnm, expr);
  this->LatexOutput << "}";
}

// GenStopStmt
// ss : AS`StopStmt
void GenLatexOutput::GenStopStmt(const Generic & modnm, const TYPE_AS_StopStmt & ss)
{
  const TYPE_AS_Expr & expr (ss.GetRecord(pos_AS_StopStmt_expr));

  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\stop{";
  GenExpr(modnm, expr);
  this->LatexOutput << "}";
}

// GenStopListStmt
// sls : AS`StopListStmt
void GenLatexOutput::GenStopListStmt(const Generic & modnm, const TYPE_AS_StopListStmt & sls)
{
  const TYPE_AS_Expr & expr (sls.GetRecord(pos_AS_StopListStmt_expr));

  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\stoplist{";
  GenExpr(modnm, expr);
  this->LatexOutput << "}";
}

#ifdef VICE
// GenDurationStmt
// ds : AS`DurationStmt
void GenLatexOutput::GenDurationStmt(const Generic & modnm, const TYPE_AS_DurationStmt & ds)
{
  const TYPE_AS_Expr & num  (ds.GetRecord(pos_AS_DurationStmt_num));
  const TYPE_AS_Stmt & stmt (ds.GetRecord(pos_AS_DurationStmt_stmt)); 

  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\duration{";
  GenExpr(modnm, num);
  this->LatexOutput << "}{";
  GenStmt(modnm, stmt);
  this->LatexOutput << "}";
}

// GenCycleStmt
// cs : AS`CycleStmt
void GenLatexOutput::GenCycleStmt(const Generic & modnm, const TYPE_AS_CycleStmt & cs)
{
  const TYPE_AS_Expr & num  (cs.GetRecord(pos_AS_CycleStmt_num));
  const TYPE_AS_Stmt & stmt (cs.GetRecord(pos_AS_CycleStmt_stmt));

  this->LatexOutput << LineFeed();
  this->LatexOutput << "\\cycles{";
  GenExpr(modnm, num);
  this->LatexOutput << "}{";
  GenStmt(modnm, stmt);
  this->LatexOutput << "}";
}

#endif // VICE
#endif // VDMPP

/** Expressions part ********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenExpr (const Generic & modnm, const TYPE_AS_Expr & Expr)
{
  // handle covering
  bool not_covered = false;
  if (Settings.GetPrettyPrintRTI() &&
      !this->not_covered_area &&
      !RTINFO::Is_Covered (Expr, *this->contextinfo)) {
    not_covered = true;
    this->not_covered_area = true;
    this->LatexOutput << "\\color{not-covered}";
  }

  switch (Expr.GetTag ()) {
    case TAG_TYPE_AS_PrefixExpr:               { GenPrefixExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_BinaryExpr:               { GenBinaryExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_DefExpr:                  { GenDefExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_LetExpr:                  { GenLetExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_LetBeSTExpr:              { GenLetBeSTExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_IfExpr:                   { GenIfExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_CasesExpr:                { GenCasesExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_AllOrExistsExpr:          { GenAllOrExistsExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_ExistsUniqueExpr:         { GenExistsUniqueExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_IotaExpr:                 { GenIotaExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SetEnumerationExpr:       { GenSetEnumerationExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SetComprehensionExpr:     { GenSetComprehensionExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SetRangeExpr:             { GenSetRangeExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SeqEnumerationExpr:       { GenSeqEnumerationExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SeqComprehensionExpr:     { GenSeqComprehensionExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SubSequenceExpr:          { GenSubSequenceExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: { GenSeqModifyMapOverrideExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_MapEnumerationExpr:       { GenMapEnumerationExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_MapComprehensionExpr:     { GenMapComprehensionExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_TupleConstructorExpr:     { GenTupleConstructorExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_RecordConstructorExpr:    { GenRecordConstructorExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_RecordModifierExpr:       { GenRecordModifierExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_ApplyExpr:                { GenApplyExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_FieldSelectExpr:          { GenFieldSelectExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_TokenConstructorExpr:     { GenTokenConstructorExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_LambdaExpr:               { GenLambdaExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_FctTypeInstExpr:          { GenFctTypeInstExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_IsExpr:                   { GenIsExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_QuoteLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NumLit:
    case TAG_TYPE_AS_NilLit:                   { GenLiteral (Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_Name:                     { GenName(Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_OldName:                  { GenOldName (Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_UndefinedExpr:            { GenUndefinedExpr (); break; }
    case TAG_TYPE_AS_TupleSelectExpr:          { GenTupleSelectExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_TypeJudgementExpr:        { GenTypeJudgementExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_NarrowExpr:               { GenNarrowExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_PreConditionApplyExpr:    { GenPreConditionApplyExpr(modnm, Expr); break; }
    case TAG_TYPE_AS_BracketedExpr:            { GenBracketedExpr (modnm, Expr); break; }
    case TAG_TYPE_AS_Macro:                    { GenMacro (modnm, Expr); break; }
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr:           { GenNewExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_SelfExpr:          { GenSelfExpr(this->LatexOutput); break; }
    case TAG_TYPE_AS_IsOfClassExpr:     { GenIsOfClassExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_IsOfBaseClassExpr: { GenIsOfBaseClassExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_SameBaseClassExpr: { GenSameBaseClassExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_SameClassExpr:     { GenSameClassExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_ActExpr:           { GenActExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_FinExpr:           { GenFinExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_ActiveExpr:        { GenActiveExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_WaitingExpr:       { GenWaitingExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_ReqExpr:           { GenReqExpr(modnm, Expr, this->LatexOutput); break; }
    case TAG_TYPE_AS_ThreadIdExpr:      { GenThreadIdExpr(this->LatexOutput); break; }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr:       { GenCurTimeExpr(this->LatexOutput); break; }
#endif // VICE
#endif // VDMPP
    default: { ErrMsg1 (Expr, L"Unknown expression encountered"); }
  }

  if (Settings.GetPrettyPrintRTI() && not_covered) {
    this->not_covered_area = false;
    this->LatexOutput << "\\color{covered}";
  }
}

/** Complex Expressions part ************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

// GenDefExpr
// de : AS`DefExpr
void GenLatexOutput::GenDefExpr(const Generic & modnm, const TYPE_AS_DefExpr & de)
{
  const type_dd2PL & value_l   (de.GetTuple(pos_AS_DefExpr_Def));
  const TYPE_AS_Expr & in_expr (de.GetRecord(pos_AS_DefExpr_In));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{defexpr}";
  this->LatexOutput << endl;

  size_t len_value_l = value_l.Length();
  for (size_t i = 1; i < len_value_l; i++) {
    const Tuple & t (value_l[i]);
    const TYPE_AS_PatternBind & bind (t.GetRecord(1));
    const TYPE_AS_Expr & expr        (t.GetRecord(2));

    this->linefeed = false;
    this->LatexOutput << "\\eqdef{";
    GenPatternBind (modnm, bind);
    this->LatexOutput << "}";

    int l1 = PatBindLine (bind);
    int l2 = GetLine (expr);

    if (l2 > l1) {
      this->LatexOutput << LF;
    }
    this->LatexOutput << "{";
    GenExpr (modnm, expr);
    this->LatexOutput << "}";
    this->LatexOutput << endl;
  }

  this->LatexOutput << "\\end{defexpr}" << LF;
  this->linefeed = false;
  GenExpr (modnm, in_expr);
}

// GenLetExpr
// le : AS`LetExpr
void GenLatexOutput::GenLetExpr(const Generic & modnm, const TYPE_AS_LetExpr & le)
{
  const SEQ<TYPE_AS_LocalDef> & localdef (le.GetSequence(pos_AS_LetExpr_localdef));
  const TYPE_AS_Expr & in_expr           (le.GetRecord(pos_AS_LetExpr_body));

  this->LatexOutput << LineFeed ();
  this->LatexOutput << "\\begin{letexpr}";
  this->LatexOutput << endl;

  size_t len_localdef = localdef.Length();
  for (size_t i = 1; i <= len_localdef; i++) {
    const TYPE_AS_LocalDef & def (localdef[i]);
    this->linefeed = false;
    switch(def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        const TYPE_AS_Pattern & pat (def.GetRecord(pos_AS_ValueDef_pat));
        const Generic & tp          (def.GetField(pos_AS_ValueDef_tp));
        const TYPE_AS_Expr & val    (def.GetRecord(pos_AS_ValueDef_val));

        this->LatexOutput << "\\patdef{";
        GenPattern (modnm, pat);
        if (! tp.IsNil ()) {
          this->LatexOutput << " : ";
          GenType (tp);
        }
        this->LatexOutput << "}";

        int l1 = PatLine (pat);
        int l2 = GetLine (val);

        if (l2 > l1) {
          this->LatexOutput << LF;
        }
        this->LatexOutput << "{";
        GenExpr (modnm, val);
        this->LatexOutput << "}";
        this->LatexOutput << endl;
        break;
      }
      case TAG_TYPE_AS_ExplFnDef:
      case TAG_TYPE_AS_ExtExplFnDef:
      case TAG_TYPE_AS_ImplFnDef: {
        GenFunctionDef (modnm, def, false);
        break;
      }
      default:
        break;
    }
  }

  this->LatexOutput << "\\end{letexpr}" << LF;
  this->linefeed = false;
  GenExpr (modnm, in_expr);
}

// GenLetBeSTExpr
// lbste : AS`LetBeSTExpr
void GenLatexOutput::GenLetBeSTExpr(const Generic & modnm, const TYPE_AS_LetBeSTExpr & lbste)
{
   const SEQ<TYPE_AS_MultBind> & bind_l (lbste.GetSequence(pos_AS_LetBeSTExpr_lhs));
   const Generic & st_expr      (lbste.GetField(pos_AS_LetBeSTExpr_St));
   const TYPE_AS_Expr & in_expr (lbste.GetRecord(pos_AS_LetBeSTExpr_In));

  this->LatexOutput << "\\Letbe";

  if (st_expr.IsNil() ||
      (st_expr.Is (TAG_TYPE_AS_BoolLit) && Record(st_expr).GetBoolValue(pos_AS_BoolLit_val))) {
    this->LatexOutput << "* ";

    int l1 = BindLine (bind_l[1]);

    size_t len_bind_l = bind_l.Length();
    for (size_t idx = 1; idx <= len_bind_l; idx++) {
      const TYPE_AS_MultBind & bind (bind_l[idx]);
      int l2 = BindLine (bind);
      if (idx > 1) {
        LatexOutput << ",";
      }
      if (l2 > l1) {
        LatexOutput << LF;
      }
      GenMultBind (modnm, bind);
      l1 = l2;
    }
  }
  else {
    this->LatexOutput << LATEXSPACE2;

    int l1 = BindLine (bind_l[1]);

    size_t len_bind_l = bind_l.Length();
    for (size_t idx = 1; idx <= len_bind_l; idx++) {
      const TYPE_AS_MultBind & bind (bind_l[idx]);
      int l2 = BindLine (bind);
      if (idx > 1) {
        LatexOutput << ",";
      }
      if (l2 > l1) {
        LatexOutput << LF;
      }
      GenMultBind (modnm, bind);
      l1 = l2;
    }

    this->LatexOutput << " \\Best";

    int l3 = GetLine (st_expr);

    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    this->LatexOutput << LATEXSPACE2;
    this->linefeed = false;
    GenExpr (modnm, st_expr);
  }

  this->LatexOutput << " \\Lin" << LF;
  this->linefeed = false;
  GenExpr (modnm, in_expr);
}

// GenIfExpr
// ie : AS`IfExpr
void GenLatexOutput::GenIfExpr(const Generic & modnm, const TYPE_AS_IfExpr & ie)
{
  const TYPE_AS_Expr & test_e            (ie.GetRecord(pos_AS_IfExpr_test));
  const TYPE_AS_Expr & cons              (ie.GetRecord(pos_AS_IfExpr_cons));
  const SEQ<TYPE_AS_ElseifExpr> & elif_l (ie.GetSequence(pos_AS_IfExpr_elsif));
  const TYPE_AS_Expr & altn              (ie.GetRecord(pos_AS_IfExpr_altn));

  this->LatexOutput << "\\If ";

  this->linefeed = false;
  GenExpr (modnm, test_e);
  this->LatexOutput << endl;
  this->LatexOutput << "\\Then" << LF;
  GenExpr (modnm, cons);

  if (!elif_l.IsEmpty()) {
    size_t len_elif_l = elif_l.Length();
    for (size_t i = 1; i <= len_elif_l; i++) {
      const TYPE_AS_ElseifExpr & elif (elif_l[i]);
      const TYPE_AS_Expr & elif_test (elif.GetRecord(pos_AS_ElseifExpr_test));
      const TYPE_AS_Expr & elif_cons (elif.GetRecord(pos_AS_ElseifExpr_cons));

      this->LatexOutput << endl;
      this->LatexOutput << "\\Elseif ";
      this->linefeed = false;
      GenExpr (modnm, elif_test);
      this->LatexOutput << endl;
      this->LatexOutput << "\\Then" << LF;
      this->linefeed = false;
      GenExpr (modnm, elif_cons);
    }
  }
  this->LatexOutput << endl;
  this->LatexOutput << "\\Else" << LF;
  this->linefeed = false;
  GenExpr (modnm, altn);
  this->LatexOutput << endl;
  this->LatexOutput << "\\Fi";
}

void GenLatexOutput::GenCasesExpr(const Generic & modnm, const TYPE_AS_CasesExpr & CasesE)
{
  TYPE_AS_Expr test_e (CasesE.get_sel());
  SEQ<TYPE_AS_CaseAltn> alt_l (CasesE.get_altns());
  Generic others (CasesE.get_Others());

  LatexOutput << LineFeed ();
  LatexOutput << "\\begin{Cases}{";

  GenExpr (modnm, test_e);
  LatexOutput << "}";
  LatexOutput << endl;

  Generic galt;
  for (bool bb = alt_l.First(galt); bb; bb = alt_l.Next(galt)) {
    TYPE_AS_CaseAltn alt (galt);
    SEQ<TYPE_AS_Pattern> pat_l (alt.get_match());
    TYPE_AS_Expr expr  (alt.get_body());

    LatexOutput << "\\alt{";

    int l1 = PatLine (pat_l[1]);

    size_t len = pat_l.Length ();
    for (size_t cc = 1; cc <= len; cc++) {
      int l2 = PatLine (pat_l[cc]);

      if (l2 > l1) {
        LatexOutput << LF;
      }
      bool not_covered = false;
      if (Settings.GetPrettyPrintRTI() &&
          !this->not_covered_area &&
          !RTINFO::Is_Covered (pat_l[cc], *this->contextinfo)) {
        not_covered = true;
        this->not_covered_area = true;
        this->LatexOutput << "\\color{not-covered}";
      }
      GenPattern (modnm, pat_l[cc]);
      // post Covered
      if (Settings.GetPrettyPrintRTI() && not_covered) {
        this->not_covered_area = false;
        this->LatexOutput << "\\color{covered}";
      }
      if (cc < len) {
        LatexOutput << ",";
      }
      l1 = l2;
    }

    this->linefeed = false;
    LatexOutput << "}";

    int l3 = GetLine (expr);

    if (l3 > l1) {
      LatexOutput << LF;
    }
    LatexOutput << "{";
    GenExpr (modnm, expr);
    LatexOutput << "}";
    LatexOutput << endl;
  }

  if (! others.IsNil ()) {
    this->linefeed = false;
    LatexOutput << "\\others{";
    GenExpr (modnm, others);
    LatexOutput << "}";
    LatexOutput << endl;
  }

  LatexOutput << "\\end{Cases}";
}

/** Quantified Expressions part *********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenAllOrExistsExpr(const Generic & modnm, const TYPE_AS_AllOrExistsExpr & AllOrExists)
{
  const TYPE_AS_AllOrExistsQuantifier & quant (AllOrExists.GetRecord(pos_AS_AllOrExistsExpr_quant));
  const SEQ<TYPE_AS_MultBind> & bind_l (AllOrExists.GetSequence(pos_AS_AllOrExistsExpr_bind));
  const TYPE_AS_Expr & pred_e (AllOrExists.GetRecord(pos_AS_AllOrExistsExpr_pred));

  if (Int(quant).GetValue() == ALL) {
    LatexOutput << "\\all{";
  }
  else {
    LatexOutput << "\\exists{";
  }
  int l1 = BindLine (bind_l[1]);

  size_t len_bind_l = bind_l.Length();
  for (size_t idx = 1; idx <= len_bind_l; idx++) {
    const TYPE_AS_MultBind & bind (bind_l[idx]); 
    int l2 = BindLine (bind);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l2 > l1) {
      LatexOutput << LF;
    }
    GenMultBind (modnm, bind);
    l1 = l2;
  }

  LatexOutput << "}";

  int l3 = GetLine (pred_e);

  if (l3 > l1) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  LatexOutput << "{";
  GenExpr (modnm, pred_e);
  LatexOutput << "}";
}

void GenLatexOutput::GenExistsUniqueExpr(const Generic & modnm, const TYPE_AS_ExistsUniqueExpr & ExistsUnique)
{
  const TYPE_AS_Bind & bind (ExistsUnique.GetRecord(pos_AS_ExistsUniqueExpr_bind));
  const TYPE_AS_Expr & pred_e (ExistsUnique.GetRecord(pos_AS_ExistsUniqueExpr_pred));

  LatexOutput << "\\unique{";
  GenBind(modnm, bind);
  LatexOutput << "}";

  int l1 = BindLine (bind);
  int l2 = GetLine (pred_e);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  LatexOutput << "{";
  GenExpr(modnm, pred_e);
  LatexOutput << "}";
}

void GenLatexOutput::GenIotaExpr (const Generic & modnm, const TYPE_AS_IotaExpr & IotaE)
{
  const TYPE_AS_Bind & bind (IotaE.GetRecord(pos_AS_IotaExpr_bind));
  const TYPE_AS_Expr & pred_e (IotaE.GetRecord(pos_AS_IotaExpr_pred));

  LatexOutput << "\\iota{";
  GenBind (modnm, bind);
  LatexOutput << "}";

  int l1 = BindLine (bind);
  int l2 = GetLine (pred_e);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  LatexOutput << "{";
  GenExpr (modnm, pred_e);
  LatexOutput << "}";
}

/** Set Expressions part ****************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenSetEnumerationExpr(const Generic & modnm, const TYPE_AS_SetEnumerationExpr & SetE)
{
  const SEQ<TYPE_AS_Expr> & els_se (SetE.GetSequence(pos_AS_SetEnumerationExpr_els));

  LatexOutput << "\\set{";

  int l1 = GetLine (SetE);

  size_t len_els_se = els_se.Length();
  for (size_t idx = 1; idx <= len_els_se; idx++) {
    const TYPE_AS_Expr & elm_e (els_se[idx]);
    int l2 = GetLine (elm_e);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l2 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    GenExpr (modnm, elm_e);
    l1 = l2;
  }

  LatexOutput << "}";
}

void GenLatexOutput::GenSetComprehensionExpr(const Generic & modnm, const TYPE_AS_SetComprehensionExpr & SetC)
{
  const TYPE_AS_Expr & elem_e (SetC.GetRecord(pos_AS_SetComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind_s (SetC.GetSequence(pos_AS_SetComprehensionExpr_bind));
  const Generic & pred_e (SetC.GetField(pos_AS_SetComprehensionExpr_pred));

  LatexOutput << "\\setcomp{";

  this->linefeed = false;
  GenExpr(modnm, elem_e);
  LatexOutput << "}";

  int l1 = GetLine (elem_e);
  int l2 = BindLine (bind_s[1]);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  LatexOutput << "{";

  l1 = l2;

/*
  Generic env;
  bool bb = bind_s.First (env);
  while(bb) {
    int l3 = BindLine (env);

    if (l3 > l1) {
      LatexOutput << LF;
    }
    GenMultBind(modnm, env);
    bb = bind_s.Next (env);
    if (bb) {
      LatexOutput << ",";
    }
    l1 = l3;
  }
*/
  size_t len_bind_s = bind_s.Length();
  for (size_t idx = 1; idx <= len_bind_s; idx++) {
    const TYPE_AS_MultBind & mb (bind_s[idx]);
    int l3 = BindLine (mb);
    if (l3 > l1) {
      LatexOutput << LF;
    }
    GenMultBind(modnm, mb);
    if (idx < len_bind_s) {
      LatexOutput << ",";
    }
    l1 = l3;
  }

  LatexOutput << "}";

  if (! (pred_e.IsNil() ||
         (pred_e.Is(TAG_TYPE_AS_BoolLit) && ((Record) pred_e).GetBoolValue (1)))) {

    int l4 = GetLine (pred_e);
    if (l4 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    LatexOutput << "[";
    GenExpr(modnm, pred_e);
    LatexOutput << "]";
  }
}

void GenLatexOutput::GenSetRangeExpr(const Generic & modnm, const TYPE_AS_SetRangeExpr & SetR)
{
  const TYPE_AS_Expr & lb_e (SetR.GetRecord(pos_AS_SetRangeExpr_lb));
  const TYPE_AS_Expr & ub_e (SetR.GetRecord(pos_AS_SetRangeExpr_ub));

  this->linefeed = false;

  int l1 = GetLine (lb_e);
  int l2 = GetLine (ub_e);

  LatexOutput << "\\setrange{";

  GenExpr(modnm, lb_e);
  LatexOutput << "}";

  if (l2 > l1) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  LatexOutput << "{";
  GenExpr(modnm, ub_e);
  LatexOutput << "}";
}


/** Sequence Expressions part ***********************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenSeqEnumerationExpr (const Generic & modnm, const TYPE_AS_SeqEnumerationExpr & SeqE)
{
  const SEQ<TYPE_AS_Expr> & els_le (SeqE.GetSequence(pos_AS_SeqEnumerationExpr_els));

  LatexOutput << "\\seq{";

  int l1 = GetLine (SeqE);

  size_t len_els_le = els_le.Length();
  for (size_t idx = 1; idx <= len_els_le; idx++) {
    const TYPE_AS_Expr & elm_e (els_le[idx]);
    int l2 = GetLine (elm_e);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l2 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    GenExpr(modnm, elm_e);
    l1 = l2;
  }

  LatexOutput << "}";
}

void GenLatexOutput::GenSeqComprehensionExpr (const Generic & modnm, const TYPE_AS_SeqComprehensionExpr & SeqC)
{
  const TYPE_AS_Expr & elem_e (SeqC.GetRecord(pos_AS_SeqComprehensionExpr_elem));
  const Record & bind (SeqC.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  const Generic & pred_e (SeqC.GetField(pos_AS_SeqComprehensionExpr_pred));

  LatexOutput << "\\seqcomp{";

  this->linefeed = false;
  GenExpr(modnm, elem_e);
  LatexOutput << "}";

  int l1 = GetLine (elem_e);
  //int l2 = BindLine (bind);
  int l2 = (bind.Is(TAG_TYPE_AS_SetBind) ? PatLine (bind.GetRecord(pos_AS_SetBind_pat))
                                         : PatLine (bind.GetRecord(pos_AS_SeqBind_pat)));

  if (l2 > l1) {
    LatexOutput << LF;
  }
  l1 = l2;
  this->linefeed = false;
  LatexOutput << "{";
  
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      break;
      GenSetBind(modnm, bind);
    }
    case TAG_TYPE_AS_SeqBind: {
      break;
      GenSeqBind(modnm, bind);
    }
  }
  LatexOutput << "}";

  if (! ( pred_e.IsNil() ||
          (pred_e.Is(TAG_TYPE_AS_BoolLit) && Record(pred_e).GetBoolValue(pos_AS_BoolLit_val)))) {
    int l3 = GetLine (pred_e);

    if (l3 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    LatexOutput << "[";
    GenExpr(modnm, pred_e);
    LatexOutput << "]";
  }
}

void GenLatexOutput::GenSubSequenceExpr (const Generic & modnm, const TYPE_AS_SubSequenceExpr & SubSeq)
{
  const TYPE_AS_Expr & seq_e  (SubSeq.GetRecord(pos_AS_SubSequenceExpr_sequence));
  const TYPE_AS_Expr & from_e (SubSeq.GetRecord(pos_AS_SubSequenceExpr_frompos));
  const TYPE_AS_Expr & to_e   (SubSeq.GetRecord(pos_AS_SubSequenceExpr_topos));

  this->linefeed = false;
  LatexOutput << "\\subseq{";
  GenExpr(modnm, seq_e);
  LatexOutput << "}";

  int l1 = GetLine (seq_e);
  int l2 = GetLine (from_e);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  LatexOutput << "{";
  GenExpr(modnm, from_e);
  LatexOutput << "}";

  int l3 = GetLine (to_e);

  if (l3 > l2) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  LatexOutput << "{";
  GenExpr(modnm, to_e);
  LatexOutput << "}";
}

void GenLatexOutput::GenSeqModifyMapOverrideExpr(const Generic & modnm, const TYPE_AS_SeqModifyMapOverrideExpr & SeqM)
{
  const TYPE_AS_Expr & seqmap_e (SeqM.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap));
  const TYPE_AS_Expr & map_e    (SeqM.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp));

  this->linefeed = false;
  GenExpr (modnm, seqmap_e);
  LatexOutput << " \\Override ";

  int l1 = GetLine (seqmap_e);
  int l2 = GetLine (map_e);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  GenExpr (modnm, map_e);
}

/** Map Expressions part ****************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenMaplet (const Generic & modnm, const TYPE_AS_Expr & dom_e, const TYPE_AS_Expr & rng_e)
{
  GenExpr (modnm, dom_e);
  LatexOutput << " \\Mapsto ";

  int l1 = GetLine (dom_e);
  int l2 = GetLine (rng_e);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  GenExpr (modnm, rng_e);
}

void GenLatexOutput::GenMapEnumerationExpr (const Generic & modnm, const TYPE_AS_MapEnumerationExpr & MapE)
{
  const SEQ<TYPE_AS_Maplet> & els_s (MapE.GetSequence(pos_AS_MapEnumerationExpr_els));

  if (els_s.IsEmpty()) {
    LatexOutput << "\\Emptymap ";

  }
  else {
    LatexOutput << "\\map{";

    int l1 = GetLine (MapE);

    size_t len_els_s = els_s.Length();
    for (size_t idx = 1; idx <= len_els_s; idx++) {
      const TYPE_AS_Maplet & maplet (els_s[idx]);
      const TYPE_AS_Expr & dom_e (maplet.GetRecord(pos_AS_Maplet_mapdom));
      const TYPE_AS_Expr & rng_e (maplet.GetRecord(pos_AS_Maplet_maprng));

      int l2 = GetLine (dom_e);
      if (idx > 1) {
        LatexOutput << ",";
      }
      if (l2 > l1) {
        LatexOutput << LF;
      }
      this->linefeed = false;
      GenMaplet (modnm, dom_e, rng_e);
      l1 = GetLine (rng_e);
    }

    LatexOutput << "}";
  }
}

void GenLatexOutput::GenMapComprehensionExpr(const Generic & modnm, const TYPE_AS_MapComprehensionExpr & MapC)
{
  const TYPE_AS_Maplet & elem (MapC.GetRecord(pos_AS_MapComprehensionExpr_elem));
  const SEQ<TYPE_AS_MultBind> & bind_s (MapC.GetSequence(pos_AS_MapComprehensionExpr_bind));
  const Generic & pred_e (MapC.GetField(pos_AS_MapComprehensionExpr_pred));

  const TYPE_AS_Expr & dom_e (elem.GetRecord(pos_AS_Maplet_mapdom));
  const TYPE_AS_Expr & rng_e (elem.GetRecord(pos_AS_Maplet_maprng));

  LatexOutput << "\\mapcomp{";

  this->linefeed = false;
  GenMaplet (modnm, dom_e, rng_e);
  LatexOutput << "}";

  int l1 = GetLine (rng_e);
  int l2 = BindLine (bind_s[1]);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  LatexOutput << "{";

  l1 = l2;

  size_t len_bind_s = bind_s.Length();
  for (size_t idx = 1; idx <= len_bind_s; idx++) {
    const TYPE_AS_MultBind & env (bind_s[idx]);
    int l3 = BindLine (env);

    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l3 > l1) {
      LatexOutput << LF;
    }
    GenMultBind (modnm, env);
    l1 = l3;
  }

  LatexOutput << "}";

  if (! (pred_e.IsNil() ||
         (pred_e.Is(TAG_TYPE_AS_BoolLit) && Record(pred_e).GetBoolValue(pos_AS_BoolLit_val)))) {
    int l4 = GetLine (pred_e);

    if (l4 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    LatexOutput << "[";
    GenExpr (modnm, pred_e);
    LatexOutput << "]";
  }
}

/** Tuple Expressions part **************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenTupleConstructorExpr(const Generic & modnm, const TYPE_AS_TupleConstructorExpr & TupleC)
{
  const SEQ<TYPE_AS_Expr> & fields_le (TupleC.GetSequence(pos_AS_TupleConstructorExpr_fields));

//  this->LatexOutput << "\\reccons{\\kw{mk-}}";
  this->LatexOutput << "\\reccons{\\kw{mk\\textunderscore}}";

  int l1 = GetLine (TupleC);

  if (!fields_le.IsEmpty ()) {
    int l2 = GetLine (fields_le[1]);
    if (l2 > l1) {
      this->LatexOutput << LF;
    }
  }

  this->LatexOutput << "{";

  size_t len_fields_le = fields_le.Length();
  for (size_t idx = 1; idx <= len_fields_le; idx++) {
    const TYPE_AS_Expr & field_e (fields_le[idx]);
    int l3 = GetLine (field_e);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l3 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    GenExpr(modnm, field_e);
    l1 = l3;
  }

  LatexOutput << "}";
}

void GenLatexOutput::GenTokenConstructorExpr (const Generic & modnm, const TYPE_AS_TokenConstructorExpr & TokE)
{
  const TYPE_AS_Expr & field (TokE.GetSequence(pos_AS_TokenConstructorExpr_field));

  this->LatexOutput << "\\reccons{\\kw{mk\\textunderscore} \\Token}";

  int l1 = GetLine (TokE);
  int l2 = GetLine (field);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  this->LatexOutput << "{";
  this->linefeed = false;
  GenExpr (modnm, field);
  this->LatexOutput << "}";
}


/** Record Expressions part *************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/


void GenLatexOutput::GenRecordConstructorExpr (const Generic & modnm, const TYPE_AS_RecordConstructorExpr & RecordC)
{
  const TYPE_AS_Name & tag (RecordC.GetRecord(pos_AS_RecordConstructorExpr_tag));
  const SEQ<TYPE_AS_Expr> & fields_le (RecordC.GetSequence(pos_AS_RecordConstructorExpr_fields));

  this->LatexOutput << "\\reccons{\\kw{mk\\textunderscore} ";

  GenName (tag, this->LatexOutput);

  if (this->INDEX) {
    GenIndex (tag, typeocc_index);
  }
  this->LatexOutput << "}";

  int l1 = GetLine (RecordC);

  if (!fields_le.IsEmpty()) {
    int l2 = GetLine (fields_le[1]);
    if (l2 > l1) {
      this->LatexOutput << LF;
    }
  }

  this->LatexOutput << "{";

  size_t len = fields_le.Length ();
  for(size_t idx = 1; idx <= len; idx++) {
    const TYPE_AS_Expr & field (fields_le[idx]);
    int l3 = GetLine (field);

    if (idx > 1) {
      this->LatexOutput << ",";
    }
    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    this->linefeed = false;
    GenExpr(modnm, field);
    l1 = l3;
  }
  this->LatexOutput << "}";
}

void GenLatexOutput::GenRecordModifierExpr (const Generic & modnm, const TYPE_AS_RecordModifierExpr & RecordM)
{
  const TYPE_AS_Expr & rec_e(RecordM.GetRecord(pos_AS_RecordModifierExpr_rec));
  const SEQ<TYPE_AS_RecordModification> & modifiers (RecordM.GetSequence(pos_AS_RecordModifierExpr_modifiers));

  this->LatexOutput << "\\recmod{";

  this->linefeed = false;
  GenExpr(modnm, rec_e);
  this->LatexOutput << "}";

  int l1 = GetLine (RecordM);

  if (!modifiers.IsEmpty ()) {
    int l2 = GetLine (modifiers[1].get_newexpr());
    if (l2 > l1) {
      this->LatexOutput << LF;
    }
  }

  this->LatexOutput << "{";

  size_t len_modifiers = modifiers.Length();
  for (size_t idx = 1; idx <= len_modifiers; idx++) {
    const TYPE_AS_RecordModification & rm (modifiers[idx]);
    const TYPE_AS_Name & nm (rm.GetRecord(pos_AS_RecordModification_field));
    const TYPE_AS_Expr & ex (rm.GetRecord(pos_AS_RecordModification_newexpr));
    int l3 = GetLine (ex);

    if (idx > 1) {
      this->LatexOutput << ",";
    }
    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    this->linefeed = false;
    GenName (nm, this->LatexOutput);
    this->LatexOutput << " \\Mapsto ";
    GenExpr (modnm, ex);
    l1 = l3;
  }
  this->LatexOutput << "}";
}


/** Apply Expressions part **************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenApplyExpr (const Generic & modnm, const TYPE_AS_ApplyExpr & App)
{
  const TYPE_AS_Expr & fct_e (App.GetRecord(pos_AS_ApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg_le (App.GetSequence(pos_AS_ApplyExpr_arg));

  this->linefeed = false;
  LatexOutput << "\\fnapply{";

  GenExpr(modnm, fct_e);

  if (this->INDEX && fct_e.Is (TAG_TYPE_AS_Name)) {
    GenIndex (fct_e, fctocc_index);
  }
  LatexOutput << "}";

  int l1 = GetLine (fct_e);
  if (!arg_le.IsEmpty ()) {
    int l2 = GetLine (arg_le[1]);

    if (l2 > l1) {
      LatexOutput << LF;
    }
  }

  LatexOutput << "{";

  size_t len_arg_le = arg_le.Length();
  for (size_t idx = 1; idx <= len_arg_le; idx++) {
    const TYPE_AS_Expr & arg (arg_le[idx]);
    int l3 = GetLine (arg);
    if (l3 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    GenExpr (modnm, arg);
    if (idx < len_arg_le) {
      LatexOutput << ",";
    }
    l1 = l3;
  } 
  LatexOutput << "}";
}

void GenLatexOutput::GenFieldSelectExpr (const Generic & modnm, const TYPE_AS_FieldSelectExpr & fse)
{
  const TYPE_AS_Expr & record_e (fse.GetRecord(pos_AS_FieldSelectExpr_rec));
  const TYPE_AS_Expr & field (fse.GetRecord(pos_AS_FieldSelectExpr_nm));

  this->linefeed = false;
  GenExpr (modnm, record_e);
  LatexOutput << ".";

  switch(field.GetTag()) {
    case TAG_TYPE_AS_FctTypeInstExpr: {
      GenFctTypeInstExpr(modnm, field);
      break;
    }
    case TAG_TYPE_AS_Name: {
      GenName(field, this->LatexOutput);
      break;
    }
    default:
      break;
  }
}

void GenLatexOutput::GenLambdaExpr (const Generic & modnm, const TYPE_AS_LambdaExpr & LamE)
{
  const SEQ<TYPE_AS_TypeBind> & parm (LamE.GetSequence(pos_AS_LambdaExpr_parm));
  const TYPE_AS_Expr & body (LamE.GetRecord(pos_AS_LambdaExpr_body));

  LatexOutput << "\\lambda{";

  int l1 = BindLine (parm[1]);

  size_t len_parm = parm.Length();
  for (size_t idx = 1; idx <= len_parm; idx++) {
    const TYPE_AS_TypeBind & tb (parm[idx]);
    int l2 = BindLine (tb);

    if (l2 > l1) {
      LatexOutput << LF;
    }
    GenTypeBind (modnm, tb);

    if (idx < len_parm) {
      LatexOutput << ",";
    }
    l1 = l2;
  }

  this->linefeed = false;
  LatexOutput << "}";

  int l3 = GetLine (body);

  if (l3 > l1) {
    LatexOutput << LF;
  }
  LatexOutput << "{";
  GenExpr (modnm, body);
  LatexOutput << "}";
}

/** Simple Expressions part *************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenIsExpr(const Generic & modnm, const TYPE_AS_IsExpr & IsE)
{
  const Record & type  (IsE.GetRecord(pos_AS_IsExpr_type)); // BasicType | Name
  const TYPE_AS_Expr & arg_e (IsE.GetRecord(pos_AS_IsExpr_arg));

//  LatexOutput << "\\reccons{\\kw{is-} ";
  this->LatexOutput << "\\reccons{\\kw{is\\textunderscore} ";

  switch(type.GetTag()) {
    case TAG_TYPE_AS_Name: {
      GenName (type, this->LatexOutput);
      if (this->INDEX) {
        GenIndex (type, typeocc_index);
      }
      break;
    }
    case TAG_TYPE_AS_BooleanType: {
      GenBooleanType (type);
      break;
    }
    case TAG_TYPE_AS_NumericType: {
      GenNumericType (type);
      break;
    }
    case TAG_TYPE_AS_TokenType: {
      GenTokenType (type);
      break;
    }
    case TAG_TYPE_AS_CharType: {
      GenCharType (type);
      break;
    }
    default:
      break;
  }

  this->LatexOutput << "}";

  int l1 = GetLine (IsE);
  int l2 = GetLine (arg_e);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  this->LatexOutput << "{";

  this->linefeed = false;
  GenExpr(modnm, arg_e);

  this->LatexOutput << "}";
}


/** Literal part ************************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenLiteral(const TYPE_AS_Literal & Lit, std::ofstream & ofs)
{
  switch(Lit.GetTag()) {
    case TAG_TYPE_AS_BoolLit: {
      if (Lit.GetBoolValue(pos_AS_BoolLit_val)) {
        ofs << "\\True ";
      }
      else {
        ofs << "\\False ";
      }
      break;
    }
    case TAG_TYPE_AS_RealLit: {
      const Real & val (Lit.GetReal(pos_AS_RealLit_val));
      double n = val.GetValue ();
      int ni = (int) n;
      if (n == ni) {
        ofs << (int) ni;
      }
      else {
        char buf[30];
        char *  comma = NULL;
        sprintf(buf, "%f", n);  // was form("%lf"...)

        // remove trailing 0`s

        comma = strrchr(buf,'.');
        if ( comma ) {
          char * p = buf + strlen(buf)-1;
          while ( (*p) == '0' ) {
            p--;
          }
          if ( p == comma ) {
            p--; // remove comma as well
          }
          *(p+1) = 0;
        }
        ofs << buf;
      }
      break;
    }
    case TAG_TYPE_AS_NumLit: {
      const Real & val (Lit.GetReal(pos_AS_NumLit_val));
      double n = val.GetValue ();
      int ni = (int) n;
      ofs << (int) ni;
      break;
    }
    case TAG_TYPE_AS_CharLit: {

      ofs << "\\Quote ";
      wchar_t ch = Lit.GetChar(pos_AS_CharLit_val).GetValue();
      std::wstring s;
      s += ch;
      std::wstring res;
      Backslashed::convert_internal_to_latex_backslash(s, res, false);
      ofs << TBWSTR::wstring2mbstr(res);
      ofs << " \\Quote ";
      break;
    }
    case TAG_TYPE_AS_TextLit: {
      Sequence txt (Lit.GetSequence(pos_AS_TextLit_val));
      ofs << "\\Dquote ";
      std::wstring s;
      txt.GetString(s);
      std::wstring res;
      Backslashed::convert_internal_to_latex_backslash(s, res, true);
      ofs << TBWSTR::wstring2mbstr(res);
      ofs << " \\Dquote ";
      break;
    }
    case TAG_TYPE_AS_QuoteLit: {
      GenQuoteLit (Lit, ofs);
      break;
    }
    case TAG_TYPE_AS_NilLit: {
      ofs << "\\Nil ";
      break;
    }
    default:
      ErrMsg1 (Lit,L"Unknown literal pattern");
  }
}

void GenLatexOutput::GenUndefinedExpr (void)
{
  LatexOutput << "\\Undefined ";
}

void GenLatexOutput::GenTupleSelectExpr(const Generic & modnm, const TYPE_AS_TupleSelectExpr & tse)
{
  const TYPE_AS_Expr & tuple (tse.GetRecord(pos_AS_TupleSelectExpr_tuple));
  const TYPE_AS_RealLit & no (tse.GetRecord(pos_AS_TupleSelectExpr_no));

  GenExpr(modnm, tuple);
  this->LatexOutput << ".\\#" ;
  this->LatexOutput << no.GetRealValue(pos_AS_RealLit_val);
}

void GenLatexOutput::GenTypeJudgementExpr(const Generic & modnm, const TYPE_AS_TypeJudgementExpr & tje)
{
  const TYPE_AS_Expr & expr (tje.GetRecord(pos_AS_TypeJudgementExpr_expr));
  const TYPE_AS_Type & tp (tje.GetRecord(pos_AS_TypeJudgementExpr_type));

//  LatexOutput << "\\kw{is\\_}";
  this->LatexOutput << "\\kw{is\\textunderscore}";
  this->LatexOutput << "(";
  GenExpr(modnm, expr);

  this->LatexOutput << ",";
  GenType(tp);
  this->LatexOutput << ")";
}

void GenLatexOutput::GenNarrowExpr(const Generic & modnm, const TYPE_AS_NarrowExpr & tje)
{
  const TYPE_AS_Expr & expr (tje.GetRecord(pos_AS_NarrowExpr_expr));
  const TYPE_AS_Type & tp (tje.GetRecord(pos_AS_NarrowExpr_type));

//  LatexOutput << "\\kw{is\\_}";
  this->LatexOutput << "\\kw{narrow\\textunderscore}";
  this->LatexOutput << "(";
  GenExpr(modnm, expr);

  this->LatexOutput << ",";
  GenType(tp);
  this->LatexOutput << ")";
}

void GenLatexOutput::GenPreConditionApplyExpr(const Generic & modnm, const TYPE_AS_PreConditionApplyExpr & pca)
{
  const TYPE_AS_Expr & fct (pca.GetRecord(pos_AS_PreConditionApplyExpr_fct));
  const SEQ<TYPE_AS_Expr> & arg (pca.GetSequence(pos_AS_PreConditionApplyExpr_arg));

//  LatexOutput << "\\reccons{\\kw{pre\\_}}";
  this->LatexOutput << "\\reccons{\\kw{pre\\textunderscore}}";
  this->LatexOutput << "{";
  GenExpr(modnm, fct);
  this->LatexOutput << ",";

  size_t length = arg.Length();
  for (size_t i = 1; i <= length; i++) {
    GenExpr (modnm, arg[i]);
    if (i < length) {
      this->LatexOutput << ",";
    }
  }
  this->LatexOutput << "}";
}

// GenBracketedExpr
// be : AS`BracketedExpr
void GenLatexOutput::GenBracketedExpr (const Generic & modnm, const TYPE_AS_BracketedExpr & be)
{
  const TYPE_AS_Expr & expr (be.GetRecord(pos_AS_BracketedExpr_expr));

  this->LatexOutput << "\\pex{";

  this->linefeed = false;
  GenExpr (modnm, expr);

  this->LatexOutput << "}";
}

// GenMacro
// be : AS`Macro
void GenLatexOutput::GenMacro (const Generic & modnm, const TYPE_AS_Macro & me)
{
  const Int & tp (me.GetInt(pos_AS_Macro_tp));
  switch (tp.GetValue()) {
    case MACRO_FILE: {
      this->LatexOutput << "\\kw{\\textunderscore\\textunderscore FILE \\textunderscore\\textunderscore}";
      break;
    }
    case MACRO_LINE: {
      this->LatexOutput << "\\kw{\\textunderscore\\textunderscore LINE \\textunderscore\\textunderscore}";
      break;
    }
    case MACRO_COLUMN: {
      this->LatexOutput << "\\kw{\\textunderscore\\textunderscore COLUMN \\textunderscore\\textunderscore}";
      break;
    }
    case MACRO_CLMOD: {
      this->LatexOutput << "\\kw{\\textunderscore\\textunderscore CLMOD \\textunderscore\\textunderscore}";
      break;
    }
    case MACRO_FNOP: {
      this->LatexOutput << "\\kw{\\textunderscore\\textunderscore FNOP \\textunderscore\\textunderscore}";
      break;
    }
  }
}

#ifdef VDMPP
// GenNewExpr
// ne : AS`NewExpr
void GenLatexOutput::GenNewExpr(const Generic & modnm, const TYPE_AS_NewExpr& ne, std::ofstream & ofs)
{
  const TYPE_AS_Name & cls       (ne.GetRecord(pos_AS_NewExpr_cls));
  const SEQ<TYPE_AS_Expr> & args (ne.GetSequence(pos_AS_NewExpr_args));

  ofs << "\\fnapply{";
  ofs << "\\new{";

  this->linefeed = false;
  GenExpr(modnm, cls);

  ofs << "}";
  ofs << "}";

  ofs << "{";

  if (!args.IsEmpty ()) {
    int l1 = GetLine (cls);
    int l2 = GetLine (args[1]);
    if (l2 > l1) {
      ofs << LF;
    }
    size_t len_args = args.Length();
    for (size_t i = 1; i <= len_args; i++) {
      const TYPE_AS_Expr & arg (args[i]);
      int l3 = GetLine (arg);
      if (l3 > l1) {
        ofs << LF;
      }
      this->linefeed = false;
      GenExpr (modnm, arg);

      if (i < len_args) {
        ofs << ",";
      }
      l1 = l3;
    }
  }

  ofs << "}";
};

void GenLatexOutput::GenSelfExpr(std::ofstream & ofs)
{
  ofs << "\\kSelf ";
}

void GenLatexOutput::GenIsOfClassExpr(const Generic & modnm, const TYPE_AS_IsOfClassExpr& IsE, std::ofstream & ofs)
{
  ofs << "\\isofclass{";
  GenExpr(modnm, IsE.GetRecord(pos_AS_IsOfClassExpr_cls));
  ofs << "}{";
  GenExpr(modnm, IsE.GetRecord(pos_AS_IsOfClassExpr_arg));
  ofs << "}";
}

void GenLatexOutput::GenIsOfBaseClassExpr(const Generic & modnm, const TYPE_AS_IsOfBaseClassExpr& IsE,
                                          std::ofstream & ofs)
{
  ofs << "\\isofbaseclass{";
  GenExpr(modnm, IsE.GetRecord(pos_AS_IsOfBaseClassExpr_cls));
  ofs << "}{";
  GenExpr(modnm, IsE.GetRecord(pos_AS_IsOfBaseClassExpr_arg));
  ofs << "}";
}

void GenLatexOutput::GenSameClassExpr(const Generic & modnm, const TYPE_AS_SameClassExpr& SameE, std::ofstream & ofs)
{
  ofs << "\\sameclass{";
  GenExpr(modnm, SameE.GetRecord(pos_AS_SameClassExpr_expr1));
  ofs << "}{";
  GenExpr(modnm, SameE.GetRecord(pos_AS_SameClassExpr_expr2));
  ofs << "}";
}

void GenLatexOutput::GenSameBaseClassExpr(const Generic & modnm, const TYPE_AS_SameBaseClassExpr& SameE,
                                          std::ofstream & ofs)
{
  ofs << "\\samebaseclass{";
  GenExpr(modnm, SameE.GetRecord(pos_AS_SameBaseClassExpr_expr1));
  ofs << "}{";
  GenExpr(modnm, SameE.GetRecord(pos_AS_SameBaseClassExpr_expr2));
  ofs << "}";
}

void GenLatexOutput::GenActExpr(const Generic & modnm, const TYPE_AS_ActExpr& ActE, std::ofstream & ofs)
{
  ofs << "\\act{";
  GenNameList(ActE.GetSequence(pos_AS_ActExpr_mthd), ofs);
  ofs << "}";
}

void GenLatexOutput::GenFinExpr(const Generic & modnm, const TYPE_AS_FinExpr& FinE, std::ofstream & ofs)
{
  ofs << "\\fin{";
  GenNameList(FinE.GetSequence(pos_AS_FinExpr_mthd), ofs);
  ofs << "}";
}

void GenLatexOutput::GenActiveExpr(const Generic & modnm, const TYPE_AS_ActiveExpr& ActiveE, std::ofstream & ofs)
{
//  LatexOutput << "\\active{";
  ofs << "\\actminusfin{";
  GenNameList(ActiveE.GetSequence(pos_AS_ActiveExpr_mthd), ofs);
  ofs << "}";
}

void GenLatexOutput::GenWaitingExpr(const Generic & modnm, const TYPE_AS_WaitingExpr& WaitE, std::ofstream & ofs)
{
  ofs << "\\reqminusact{";
  GenNameList(WaitE.GetSequence(pos_AS_WaitingExpr_mthd), ofs);
  ofs << "}";
}

void GenLatexOutput::GenReqExpr(const Generic & modnm, const TYPE_AS_ReqExpr& ReqE, std::ofstream & ofs)
{
  ofs << "\\req{";
  GenNameList(ReqE.GetSequence(pos_AS_ReqExpr_mthd), ofs);
  ofs << "}";
}

void GenLatexOutput::GenThreadIdExpr(std::ofstream & ofs)
{
  ofs << "\\kw{threadid}";
}

#ifdef VICE
void GenLatexOutput::GenCurTimeExpr(std::ofstream & ofs)
{
  ofs << "\\kw{time}";
}
#endif // VICE
#endif // VDMPP

void GenLatexOutput::GenFctTypeInstExpr (const Generic & modnm, const TYPE_AS_FctTypeInstExpr & FctE)
{
  const TYPE_AS_Name & polyfct (FctE.GetRecord(pos_AS_FctTypeInstExpr_polyfct));
  const SEQ<TYPE_AS_Type> & inst (FctE.GetSequence(pos_AS_FctTypeInstExpr_inst));

  GenName (polyfct, this->LatexOutput);
  this->LatexOutput << "[";

  size_t len_inst = inst.Length ();
  for (size_t index = 1; index <= len_inst; index++) {
    if (index > 1) {
      this->LatexOutput << ",";
    }
    GenType (inst[index]);
  }

  this->LatexOutput << "]";
}


/** Names part **************************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenLabelName (const TYPE_AS_Name & Name1, std::ofstream & ofs)
{
  const TYPE_AS_Ids & name_l (Name1.GetSequence(pos_AS_Name_ids));

  size_t len_name_l = name_l.Length ();
  for (size_t index = 1; index <= len_name_l; index++) {
    const TYPE_AS_Id & id (name_l[index]);
    if (index > 1) {
      ofs << "`";
    }
    GenRealLabelId (id, ofs);
  }
}

void GenLatexOutput::GenName (const TYPE_AS_Name & Name1, std::ofstream & ofs)
{
  if (this->post_condition_scope) {
    if (Name1 == ASTAUX::MkName(L"RESULT")) {
      ofs << "\\RESULT";
      return;
    }
  } 
#ifdef VDMSL
  const TYPE_AS_Ids & name_l (Name1.GetSequence(pos_AS_Name_ids));
#endif // VDMSL
#ifdef VDMPP
  TYPE_AS_Name unmangledName;
  if (MANGLE::IsMangled(Name1)) {
    unmangledName = MANGLE::GetUnmangledName(Name1);
  }
  else {
    unmangledName = Name1;
  }

  const TYPE_AS_Ids & name_l (unmangledName.GetSequence(pos_AS_Name_ids));
#endif // VDMPP

  size_t len_name_l = name_l.Length ();
  for (size_t index = 1; index <= len_name_l; index++) {
    const TYPE_AS_Id & id (name_l[index]);
    GenRealId (id, ofs);
    if (index < len_name_l) {
      ofs << "`";
    }
  }
}

void GenLatexOutput::GenOldName (const TYPE_AS_OldName & OldN, std::ofstream & ofs)
{
  TYPE_AS_Name nm;
  nm.SetField(pos_AS_OldName_ids, OldN.GetSequence(pos_AS_OldName_ids));
  nm.SetField(pos_AS_OldName_cid, OldN.GetInt(pos_AS_OldName_cid));
  ofs << "\\old{";
  GenName (nm, ofs);
  ofs << "}";
}

std::wstring GenLatexOutput::InsertGreekLetters(const std::wstring & str,
                                                const std::wstring & prefix,
                                                const std::wstring & suffix) const
{
  std::wstring res(L"");
  int Max = str.length();
  for (int i = 0; i < Max; i++) {
    if (str[i] == L'#') {
      i++;
      if (i == Max) {
        vdm_err << L"internal error (InsertGreekLetters): unexpected single symbol: '#' "
                << L"Please report" << endl
                << L"Ignoring it and continuing. ";
        break;                  // Final # is removed
      }
      std::wstring greek;
      switch (str[i]) {
        case L'a': greek = L"alpha"; break;
        case L'b': greek = L"beta"; break;
        case L'g': greek = L"gamma"; break;
        case L'd': greek = L"delta"; break;
        case L'e': greek = L"epsilon"; break;
        case L'z': greek = L"zeta"; break;
        case L'h': greek = L"eta"; break;
        case L'q': greek = L"theta"; break;
        case L'i': greek = L"iota"; break;
        case L'k': greek = L"kappa"; break;
        case L'l': greek = L"Lambdaop"; break;
        case L'm': greek = L"mu"; break;
        case L'n': greek = L"nu"; break;
        case L'x': greek = L"xi"; break;
        case L'p': greek = L"pi"; break;
        case L'r': greek = L"rho"; break;
        case L's': greek = L"sigma"; break;
        case L't': greek = L"tau"; break;
        case L'u': greek = L"upsilon"; break;
        case L'f': greek = L"phi"; break;
        case L'c': greek = L"chi"; break;
        case L'y': greek = L"psi"; break;
        case L'w': greek = L"omega"; break;
        case L'G': greek = L"Gamma"; break;
        case L'D': greek = L"Delta"; break;
        case L'Q': greek = L"Theta"; break;
        case L'L': greek = L"Lambda"; break;
        case L'X': greek = L"Xi"; break;
        case L'P': greek = L"Pi"; break;
        case L'S': greek = L"Sigma"; break;
        case L'U': greek = L"Upsilon"; break;
        case L'F': greek = L"Phi"; break;
        case L'Y': greek = L"Psi"; break;
        case L'W': greek = L"Omega"; break;
        default : {
          // Unknown #x pairs are ignored.
          vdm_err << L"internal error (InsertGreekLetters): unexpected symbol pair: #"
                  << str[i] << L" (" << (int)str[i] << L"). " << endl
                  << L"Please report" << endl
                  << L"Ignoring it and continuing. ";
          // go to next `for' iteration
          continue;
        }
      }
      res += prefix;
      res += greek;
      res += suffix;
    } else {
      res += str[i];
    }
  }
  return res;
}

std::wstring GenLatexOutput::InsertGreekLettersInMathSection (const std::wstring & str) const
{
  return InsertGreekLetters(str, L"{\\", L"}");
}


std::wstring GenLatexOutput::InsertGreekLettersInLabelSection (const std::wstring & str) const
{
  return InsertGreekLetters(str, L"{", L"}");
}

std::wstring GenLatexOutput::InsertGreekLettersInIndex (const std::wstring & str) const
{
  return InsertGreekLetters(str, L"{$\\", L" $}");
}

void GenLatexOutput::GenRealLabelId(const TYPE_AS_Id & Id, std::ofstream & ofs)
{
  ofs << TBWSTR::wstring2mbstr(ConvertStr(InsertGreekLettersInLabelSection (Id.GetString())));
}

void GenLatexOutput::GenRealId(const TYPE_AS_Id & Id, std::ofstream & ofs)
{
  ofs << TBWSTR::wstring2mbstr(ConvertStr(InsertGreekLettersInMathSection (Id.GetString())));
}

// ConvertStr
std::wstring GenLatexOutput::ConvertStr (const std::wstring & TheStr)
{
  std::wstring NewStr (TheStr);

  std::wstring RX_pre  (L"pre_");
  std::wstring RX_post (L"post_");
  std::wstring RX_inv  (L"inv_");

  if (NewStr.find(RX_pre) == 0) {
    NewStr.replace(0, RX_pre.length(), L"\\kw{pre\\textunderscore}");
  }
  else if (NewStr.find(RX_post) == 0) {
    NewStr.replace(0, RX_post.length(), L"\\kw{post\\textunderscore}");
  }
  else if (NewStr.find(RX_inv) == 0) {
    NewStr.replace(0, RX_inv.length(), L"\\kw{inv\\textunderscore}");
  }
  // replace all other underscores with '\\textunderscore'
  std::wstring res;
  for (int i = 0, Max = NewStr.length(); i < Max; i++) {
    if (NewStr[i] == L'_') {
      res += L"\\hbox{\\textunderscore}";
    }
    else {
      res += NewStr[i];
    }
  }
  return res;
}

// ConvertIndexStr
std::wstring GenLatexOutput::ConvertIndexStr (const std::wstring & id) const
{
  std::wstring res;
  for (int i = 0, Max = id.length(); i < Max; i++) {
    if (id[i] == L'_') {
      res += L"\\hbox{\\textunderscore}";
    }
    else {
      res += id[i];
    }
  }
  return res;
}

/** Unary and Binary Expressions part ***************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

void GenLatexOutput::GenPrefixExpr(const Generic & modnm, const TYPE_AS_PrefixExpr & UnaryE)
{
  const TYPE_AS_UnaryOp & opr (UnaryE.GetField(pos_AS_PrefixExpr_opr));
  const TYPE_AS_Expr & arg_e (UnaryE.GetRecord(pos_AS_PrefixExpr_arg));

  switch (Int(opr).GetValue ()) {
    case NUMPLUS :           { LatexOutput << " \\Uplus "; break; }
    case NUMMINUS :          { LatexOutput << " \\Uminus "; break; }
    case FLOOR :             { LatexOutput << " \\Floor "; break; }
    case NUMABS :            { LatexOutput << " \\Abs "; break; }
    case NOT :               { LatexOutput << " \\Not "; break; }
    case SETCARD :           { LatexOutput << " \\Card "; break; }
    case SETDISTRUNION :     { LatexOutput << " \\Dunion "; break; }
    case SETDISTRINTERSECT : { LatexOutput << " \\Dinter "; break; }
    case SETPOWER :          { LatexOutput << " \\Power "; break; }
    case SEQHEAD :           { LatexOutput << " \\Hd "; break; }
    case SEQTAIL :           { LatexOutput << " \\Tl "; break; }
    case SEQLEN :            { LatexOutput << " \\Len "; break; }
    case SEQELEMS :          { LatexOutput << " \\Elems "; break; }
    case SEQINDICES :        { LatexOutput << " \\Inds "; break; }
    case SEQDISTRCONC :      { LatexOutput << " \\Conc "; break; }
    case SEQREVERSE :        { LatexOutput << " \\Reverse "; break; }
    case MAPDOM :            { LatexOutput << " \\Dom "; break; }
    case MAPRNG :            { LatexOutput << " \\Rng "; break; }
    case MAPDISTRMERGE : { LatexOutput << " \\Merge "; break; }
    case MAPINVERSE: {
      this->LatexOutput << "\\mapinv{";
      this->linefeed = false;
      GenExpr (modnm, arg_e);
      this->LatexOutput << "}";
      return;
      break;
    }
    default :
      ErrMsg1 (UnaryE, L"Unknown unary command: " + Int(opr).ascii());
    }
  this->linefeed = false;
  GenExpr (modnm, arg_e);
}

void GenLatexOutput::GenBinaryExpr(const Generic & modnm, const TYPE_AS_BinaryExpr & BinaryE)
{
  const TYPE_AS_Expr & left_e  (BinaryE.GetRecord(pos_AS_BinaryExpr_left));
  const Int & opr (BinaryE.GetInt(pos_AS_BinaryExpr_opr));
  const TYPE_AS_Expr & right_e (BinaryE.GetRecord(pos_AS_BinaryExpr_right));

  this->linefeed = false;
  GenExpr (modnm, left_e);

  switch (opr.GetValue ()) {
    case NUMPLUS :      { LatexOutput << " \\Plus "; break; }
    case NUMMINUS :     { LatexOutput << " \\Minus "; break; }
    case NUMMULT :      { LatexOutput << " \\Mult "; break; }
    case NUMDIV :       { LatexOutput << " \\NumDiv "; break; }
    case NUMREM :       { LatexOutput << " \\Rem "; break; }
    case NUMMOD :       { LatexOutput << " \\Mod "; break; }
    case INTDIV :       { LatexOutput << " \\Div "; break; }
    case NUMLT :        { LatexOutput << " \\NumLt "; break; }
    case NUMLE :        { LatexOutput << " \\Le "; break; }
    case NUMGT :        { LatexOutput << " \\NumGt "; break; }
    case NUMGE :        { LatexOutput << " \\Ge "; break; }
    case AND :          { LatexOutput << " \\And "; break; }
    case OR :           { LatexOutput << " \\Or "; break; }
    case IMPLY :        { LatexOutput << " \\Implies "; break; }
    case EQUIV :        { LatexOutput << " \\Equiv "; break; }
    case EQ :           { LatexOutput << " \\Equals "; break; }
    case NE :           { LatexOutput << " \\Neq "; break; }
    case INSET :        { LatexOutput << " \\In "; break; }
    case NOTINSET :     { LatexOutput << " \\Notin "; break; }
    case SETUNION :     { LatexOutput << " \\Union "; break; }
    case SETINTERSECT : { LatexOutput << " \\Inter "; break; }
    case SETMINUS :     { LatexOutput << " \\Setdiff "; break; }
    case SUBSET :       { LatexOutput << " \\Subset "; break; }
    case PROPERSUBSET : { LatexOutput << " \\Psubset "; break; }
    case SEQCONC :      { LatexOutput << " \\Sconc "; break; }
    case MAPMERGE :     { LatexOutput << " \\Mapmerge "; break; }
    case MAPDOMRESTTO : { LatexOutput << " \\Dto "; break; }
    case MAPDOMRESTBY : { LatexOutput << " \\Dby "; break; }
    case MAPRNGRESTTO : { LatexOutput << " \\Rto "; break; }
    case MAPRNGRESTBY : { LatexOutput << " \\Rby "; break; }
    case COMPOSE :      { LatexOutput << " \\Compose "; break; }
    case NUMEXP :       { LatexOutput << " \\Iterate "; break; }
    default :           { ErrMsg1 (BinaryE, L"Unknown binary expression"); }
  }

  int l1 = GetLine (left_e);
  int l2 = GetLine (right_e);

  if (l2 > l1) {
    LatexOutput << LF;
  }
  this->linefeed = false;
  GenExpr (modnm, right_e);
}

/** Pattern part ************************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

///////////////////////
// Gen line number for Pattern
////////////////////////

int GenLatexOutput::PatLine (const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag ()) {
    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_MapEnumPattern:
    case TAG_TYPE_AS_TuplePattern:    { return GetLine(pat); break; }
    case TAG_TYPE_AS_MatchVal:        { return GetLine (pat.GetRecord(pos_AS_MatchVal_val)); break; }
    case TAG_TYPE_AS_SetUnionPattern: { return PatLine (pat.GetRecord (pos_AS_SetUnionPattern_lp)); break; }
    case TAG_TYPE_AS_SeqConcPattern:  { return PatLine (pat.GetRecord (pos_AS_SeqConcPattern_lp)); break; }
    case TAG_TYPE_AS_MapMergePattern: { return PatLine (pat.GetRecord (pos_AS_MapMergePattern_lp)); break; }
    case TAG_TYPE_AS_RecordPattern:   { return GetLine(pat); break; }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:    { return GetLine(pat); break; }
#endif // VDMPP
    default: { return -1; }
  }
}

void GenLatexOutput::GenPattern (const Generic & modnm, const TYPE_AS_Pattern & pat)
{
  switch (pat.GetTag ()) {
    case TAG_TYPE_AS_PatternName:     { GenPatternName (pat); break; }
    case TAG_TYPE_AS_MatchVal:        { GenMatchVal (modnm, pat); break; }
    case TAG_TYPE_AS_SetEnumPattern:  { GenSetEnumPattern (modnm, pat); break; }
    case TAG_TYPE_AS_SetUnionPattern: { GenSetUnionPattern (modnm, pat); break; }
    case TAG_TYPE_AS_SeqEnumPattern:  { GenSeqEnumPattern (modnm, pat); break; }
    case TAG_TYPE_AS_SeqConcPattern:  { GenSeqConcPattern (modnm, pat); break; }
    case TAG_TYPE_AS_MapEnumPattern:  { GenMapEnumPattern (modnm, pat); break; }
    case TAG_TYPE_AS_MapMergePattern: { GenMapMergePattern (modnm, pat); break; }
    case TAG_TYPE_AS_RecordPattern:   { GenRecordPattern (modnm, pat); break; }
    case TAG_TYPE_AS_TuplePattern:    { GenTuplePattern (modnm, pat); break; }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:   { GenObjectPattern (modnm, pat); break; }
#endif // VDMPP
    default: { ErrMsg1 (pat,L"Unknown pattern type"); }
  }
}

int GenLatexOutput::PatBindLine (const TYPE_AS_PatternBind & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_SetBind:
    case TAG_TYPE_AS_TypeBind: { return (BindLine (pat)); }
    default:                   { return (PatLine (pat)); }
  }
}

void GenLatexOutput::GenPatternBind (const Generic & modnm, const TYPE_AS_PatternBind & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_SetBind:  { GenSetBind (modnm, pat); break; }
    case TAG_TYPE_AS_TypeBind: { GenTypeBind (modnm, pat); break; }
    default:                   { GenPattern (modnm, pat); break; }
  }
}

void GenLatexOutput::GenPatternName (const TYPE_AS_PatternName & PatId)
{
  const Generic & id (PatId.GetField(pos_AS_PatternName_nm));

  if (!id.IsNil()) {
    GenName (id, this->LatexOutput);
  }
  else {
    this->LatexOutput << "\\DoNotCare";
  }
}

void GenLatexOutput::GenMatchVal (const Generic & modnm, const TYPE_AS_MatchVal & MatPat)
{
  const TYPE_AS_Expr & pat (MatPat.GetRecord(pos_AS_MatchVal_val));

  switch(pat.GetTag()) {
    case TAG_TYPE_AS_CharLit:
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_QuoteLit:
    case TAG_TYPE_AS_TextLit:
    case TAG_TYPE_AS_RealLit:
    case TAG_TYPE_AS_NilLit: {
      GenExpr (modnm, pat);
      break;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      this->linefeed = false;
      GenExpr (modnm, pat);
      break;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      this->linefeed = false;
      GenExpr (modnm, pat);
      break;
    }
    default: {
      this->LatexOutput << "\\pex{";
      this->linefeed = false;
      GenExpr(modnm, pat);
      this->LatexOutput << "}";
      break;
    }
  }
}

void GenLatexOutput::GenSetEnumPattern (const Generic & modnm, const TYPE_AS_SetEnumPattern & EnumPat)
{
  const SEQ<TYPE_AS_Pattern> & enum_pat (EnumPat.GetSequence(pos_AS_SetEnumPattern_Elems));

  if (enum_pat.IsEmpty()) {
    this->LatexOutput << "\\Emptyset ";
  }
  else {
    this->LatexOutput << "\\set{";

    int l1 = PatLine (EnumPat);

    size_t len_enum_pat = enum_pat.Length();
    for (size_t idx = 1; idx <= len_enum_pat; idx++) {
      const TYPE_AS_Pattern & enum_p (enum_pat[idx]);
      int l2 = PatLine (enum_p);
      if (l2 > l1) {
        this->LatexOutput << LF;
      }
      GenPattern(modnm, enum_p);

      if (idx < len_enum_pat) {
        this->LatexOutput << ",";
      }
      l1 = l2;
    }

    this->LatexOutput << "}";
  }
}

void GenLatexOutput::GenSetUnionPattern (const Generic & modnm, const TYPE_AS_SetUnionPattern & UnPat)
{
  const TYPE_AS_Pattern & lp (UnPat.GetRecord(pos_AS_SetUnionPattern_lp));
  const TYPE_AS_Pattern & rp (UnPat.GetRecord(pos_AS_SetUnionPattern_rp));

  GenPattern (modnm, lp);
  this->LatexOutput << " \\Union ";

  int l1 = PatLine (lp);
  int l2 = PatLine (rp);
  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  GenPattern(modnm, rp);
}

void GenLatexOutput::GenSeqEnumPattern (const Generic & modnm, const TYPE_AS_SeqEnumPattern & EnumPat)
{
  const SEQ<TYPE_AS_Pattern> & els (EnumPat.GetSequence(pos_AS_SeqEnumPattern_els));

  if (els.IsEmpty()) {
    this->LatexOutput << "\\Emptyseq ";
  }
  else {
    this->LatexOutput << "\\seq{";

    int l1 = PatLine (EnumPat);

    size_t len_els = els.Length ();
    for (size_t bb = 1 ; bb <= len_els ; bb++) {
      int l2 = PatLine (els[bb]);
      if (l2 > l1) {
        this->LatexOutput << LF;
      }
      GenPattern(modnm, els[bb]);
      if (bb < len_els) {
        this->LatexOutput << ",";
      }
      l1 = l2;
    }
    this->LatexOutput << "}";
  }
}

void GenLatexOutput::GenSeqConcPattern (const Generic & modnm, const TYPE_AS_SeqConcPattern & ConcPat)
{
  const TYPE_AS_Pattern & lp (ConcPat.GetRecord(pos_AS_SeqConcPattern_lp));
  const TYPE_AS_Pattern & rp (ConcPat.GetRecord(pos_AS_SeqConcPattern_rp));

  GenPattern (modnm, lp);
  this->LatexOutput << " \\Sconc ";

  int l1 = PatLine (lp);
  int l2 = PatLine (rp);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  GenPattern(modnm, rp);
}

void GenLatexOutput::GenMapEnumPattern (const Generic & modnm, const TYPE_AS_MapEnumPattern & EnumPat)
{
  const SEQ<TYPE_AS_MapletPattern> & mls (EnumPat.GetSequence(pos_AS_MapEnumPattern_mls));

  if (mls.IsEmpty()) {
    LatexOutput << "\\Emptymap ";

  }
  else {
    LatexOutput << "\\map{";

    int l1 = GetLine (EnumPat);

    size_t len_mls = mls.Length();
    for (size_t idx = 1; idx <= len_mls; idx++) {
      const TYPE_AS_MapletPattern & maplet (mls[idx]);
      const TYPE_AS_Pattern & dom_p (maplet.GetRecord(pos_AS_MapletPattern_dp));
      const TYPE_AS_Pattern & rng_p (maplet.GetRecord(pos_AS_MapletPattern_rp));

      int l2 = GetLine (dom_p);
      int l3 = GetLine (rng_p);
      if (idx > 1) {
        LatexOutput << ",";
      }
      if (l2 > l1) {
        LatexOutput << LF;
      }
      this->linefeed = false;
      GenPattern (modnm, dom_p);
      LatexOutput << " \\Mapsto ";

      if (l3 > l2) {
        LatexOutput << LF;
      }
      GenPattern (modnm, rng_p);
      l1 = l3;
    }

    LatexOutput << "}";
  }
}

void GenLatexOutput::GenMapMergePattern (const Generic & modnm, const TYPE_AS_MapMergePattern & MergePat)
{
  const TYPE_AS_Pattern & lp (MergePat.GetRecord(pos_AS_MapMergePattern_lp));
  const TYPE_AS_Pattern & rp (MergePat.GetRecord(pos_AS_MapMergePattern_rp));

  GenPattern (modnm, lp);
  this->LatexOutput << " \\Mapmerge ";

  int l1 = PatLine (lp);
  int l2 = PatLine (rp);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  GenPattern(modnm, rp);
}

void GenLatexOutput::GenRecordPattern (const Generic & modnm, const TYPE_AS_RecordPattern & RPat)
{
  const TYPE_AS_Name & tag (RPat.GetRecord(pos_AS_RecordPattern_nm));
  const SEQ<TYPE_AS_Pattern> & fields (RPat.GetSequence(pos_AS_RecordPattern_fields));

//  this->LatexOutput << "\\reccons{\\kw{mk-} ";
  this->LatexOutput << "\\reccons{\\kw{mk\\textunderscore} ";

  GenName (tag, this->LatexOutput);

  if (this->INDEX) {
    GenIndex (tag, typeocc_index);
  }
  this->LatexOutput << "}";

  int l1 = PatLine (RPat);

  if (!fields.IsEmpty()) {
    int l2 = PatLine (fields[1]);
    if (l2 > l1) {
      this->LatexOutput << LF;
    }
  }

  this->LatexOutput << "{";

  size_t len = fields.Length();
  for (size_t bb = 1; bb <= len ; bb++) {
    int l3 = PatLine (fields[bb]);

    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    GenPattern (modnm, fields[bb]);
    if (bb < len) {
      this->LatexOutput << ",";
    }
    l1 = l3;
  }
  this->LatexOutput << "}";
}

void GenLatexOutput::GenTuplePattern (const Generic & modnm, const TYPE_AS_TuplePattern & TupPat)
{
  const SEQ<TYPE_AS_Pattern> & pats (TupPat.GetSequence(pos_AS_TuplePattern_fields));

//  this->LatexOutput << "\\reccons{\\kw{mk-}}";
  this->LatexOutput << "\\reccons{\\kw{mk\\textunderscore}}";

  int l1 = PatLine (TupPat);

  if (!pats.IsEmpty()) {
    int l2 = PatLine (pats[1]);
    if (l2 > l1) {
      this->LatexOutput << LF;
    }
  }

  this->LatexOutput << "{";

  size_t len = pats.Length ();
  for (size_t bb = 1; bb <= len ; bb ++) {
    int l3 = PatLine (pats[bb]);

    if (l3 > l1) {
      this->LatexOutput << LF;
    }
    GenPattern (modnm, pats[bb]);
    if (bb < len) {
      this->LatexOutput << ",";
    }
    l1 = l3;
  }
  this->LatexOutput << "}";
}

#ifdef VDMPP
void GenLatexOutput::GenObjectPattern (const Generic & modnm, const TYPE_AS_ObjectPattern & ObjPat)
{
  const TYPE_AS_Name & cls (ObjPat.GetRecord(pos_AS_ObjectPattern_cls));
  const SEQ<TYPE_AS_FieldPattern> & fields (ObjPat.GetSequence(pos_AS_ObjectPattern_fields));
  // TODO
  this->LatexOutput << "\\reccons{\\kw{obj\\textunderscore} ";
  GenName (cls, this->LatexOutput);

  if (this->INDEX) {
    GenIndex (cls, typeocc_index);
  }
  this->LatexOutput << "}";

  LatexOutput << "(";

  int l1 = GetLine (ObjPat);

  size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++) {
    const TYPE_AS_FieldPattern & field (fields[idx]);
    const TYPE_AS_Name & nm (field.GetRecord(pos_AS_FieldPattern_nm));
    const TYPE_AS_Pattern & pat (field.GetRecord(pos_AS_FieldPattern_pat));

    int l2 = GetLine (nm);
    int l3 = GetLine (pat);
    if (idx > 1) {
      LatexOutput << ",";
    }
    if (l2 > l1) {
      LatexOutput << LF;
    }
    this->linefeed = false;
    GenName (nm, this->LatexOutput);
    LatexOutput << " \\Mapsto ";

    if (l3 > l2) {
      LatexOutput << LF;
    }
    GenPattern (modnm, pat);
    l1 = l3;
  }

  LatexOutput << ")";
}
#endif // VDMPP

////////////////////////
// Calculate line number for Binding in source text
////////////////////////

int GenLatexOutput::BindLine (const TYPE_AS_Bind & bind)
{
  switch (bind.GetTag ()) {
    case TAG_TYPE_AS_TypeBind:     { return PatLine (bind.GetRecord(pos_AS_TypeBind_pat)); }
    case TAG_TYPE_AS_SetBind:      { return PatLine (bind.GetRecord(pos_AS_SetBind_pat)); }
    case TAG_TYPE_AS_SeqBind:      { return PatLine (bind.GetRecord(pos_AS_SeqBind_pat)); }
    case TAG_TYPE_AS_MultSetBind:
    case TAG_TYPE_AS_MultSeqBind:
    case TAG_TYPE_AS_MultTypeBind: { return GetLine(bind); }
    default: {
      return -1;
    }
  }
}

void GenLatexOutput::GenBind (const Generic & modnm, const TYPE_AS_Bind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind:  { GenSetBind (modnm, bind); break; }
    case TAG_TYPE_AS_SeqBind:  { GenSeqBind (modnm, bind); break; }
    case TAG_TYPE_AS_TypeBind: { GenTypeBind (modnm, bind); break; }
    default: { break; }
  }
}

void GenLatexOutput::GenSetBind (const Generic & modnm, const TYPE_AS_SetBind & SetB)
{
  const TYPE_AS_Pattern & pat (SetB.GetRecord(pos_AS_SetBind_pat));
  const TYPE_AS_Expr & expr (SetB.GetRecord(pos_AS_SetBind_Set));

  GenPattern (modnm, pat);
  this->LatexOutput << " \\In ";

  int l1 = PatLine (pat);
  int l2 = GetLine (expr);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  GenExpr (modnm, expr);
}

void GenLatexOutput::GenTypeBind (const Generic & modnm, const TYPE_AS_TypeBind & TypeB)
{
  const TYPE_AS_Pattern & pat (TypeB.GetRecord(pos_AS_TypeBind_pat));
  const TYPE_AS_Type & tp (TypeB.GetRecord(pos_AS_TypeBind_tp));

  GenPattern (modnm, pat);
  this->LatexOutput << " : ";
  GenType (tp);
}

void GenLatexOutput::GenSeqBind (const Generic & modnm, const TYPE_AS_SeqBind & SeqB)
{
  const TYPE_AS_Pattern & pat (SeqB.GetRecord(pos_AS_SetBind_pat));
  const TYPE_AS_Expr & expr (SeqB.GetRecord(pos_AS_SeqBind_Seq));

  GenPattern (modnm, pat);
  this->LatexOutput << " \\In ";

  int l1 = PatLine (pat);
  int l2 = GetLine (expr);

  if (l2 > l1) {
    this->LatexOutput << LF;
  }
  GenExpr (modnm, expr);
}

void GenLatexOutput::GenMultBind (const Generic & modnm, const TYPE_AS_MultBind & MBind)
{
  switch(MBind.GetTag()) {
    case TAG_TYPE_AS_MultSetBind:  { GenMultSetBind (modnm, MBind); break; }
    case TAG_TYPE_AS_MultSeqBind:  { GenMultSeqBind (modnm, MBind); break; }
    case TAG_TYPE_AS_MultTypeBind: { GenMultTypeBind (modnm, MBind); break; }
    default: { break; }
  }
}

void GenLatexOutput::GenMultSetBind (const Generic & modnm, const TYPE_AS_MultSetBind & MSBind)
{
  const SEQ<TYPE_AS_Pattern> & pat_l (MSBind.GetSequence(pos_AS_MultSetBind_pat));
  const TYPE_AS_Expr & expr (MSBind.GetRecord(pos_AS_MultSetBind_Set));

  int l1 = BindLine (MSBind);

  size_t len = pat_l.Length ();
  for (size_t bb = 1; bb <= len; bb++) {
    int l2 = PatLine (pat_l[bb]);

    if (l2 > l1) {
      this->LatexOutput << LF;
    }
    GenPattern (modnm, pat_l[bb]);
    if (bb < (size_t)(pat_l.Length())) {
      this->LatexOutput << ",";
    }
    l1 = l2;
  }

  this->LatexOutput << " \\In ";

  int l3 = GetLine (expr);

  if (l3 > l1) {
    this->LatexOutput << LF;
  }
  GenExpr (modnm, expr);
}

void GenLatexOutput::GenMultSeqBind (const Generic & modnm, const TYPE_AS_MultSeqBind & MSBind)
{
  const SEQ<TYPE_AS_Pattern> & pat_l (MSBind.GetSequence(pos_AS_MultSeqBind_pat));
  const TYPE_AS_Expr & expr (MSBind.GetRecord(pos_AS_MultSeqBind_Seq));

  int l1 = BindLine (MSBind);

  size_t len = pat_l.Length ();
  for (size_t bb = 1; bb <= len; bb++) {
    int l2 = PatLine (pat_l[bb]);

    if (l2 > l1) {
      this->LatexOutput << LF;
    }
    GenPattern (modnm, pat_l[bb]);
    if (bb < (size_t)(pat_l.Length())) {
      this->LatexOutput << ",";
    }
    l1 = l2;
  }

  this->LatexOutput << " \\In ";

  int l3 = GetLine (expr);

  if (l3 > l1) {
    this->LatexOutput << LF;
  }
  GenExpr (modnm, expr);
}
void GenLatexOutput::GenMultTypeBind (const Generic & modnm, const TYPE_AS_MultTypeBind & MTBind)
{
  const SEQ<TYPE_AS_Pattern> & pat_l (MTBind.GetSequence(pos_AS_MultTypeBind_pat));
  const TYPE_AS_Type & tp (MTBind.GetRecord(pos_AS_MultTypeBind_tp));

  int l1 = BindLine (MTBind);

  size_t len = pat_l.Length ();
  for (size_t bb = 1; bb <= len; bb++) {
    int l2 = PatLine (pat_l[bb]);

    if (l2 > l1) {
      this->LatexOutput << LF;
    }
    GenPattern (modnm, pat_l[bb]);
    if (bb < (size_t)(pat_l.Length())) {
      this->LatexOutput << ",";
    }
    l1 = l2;
  }

  this->LatexOutput << " : ";
  GenType (tp);
}

/** Auxilary part ***********************************************************/
/** Reference : Doc. iptes-ifad-12-v1.0  ************************************/
/****************************************************************************/

std::wstring GenLatexOutput::Name2Str(const TYPE_AS_Name & Name1)
{
  const TYPE_AS_Ids & name_l (Name1.GetSequence(pos_AS_Name_ids));
  std::wstring res;

  size_t len = name_l.Length ();
  for (size_t bb = 1; bb <= len; bb++) {
    TYPE_AS_Id id (name_l[bb]);
    std::wstring ws;
    id.GetString(ws);
    res += ConvertIndexStr (ws);
    if (bb < len) {
      res += L"`";
    }
  }
  return res;
}

std::string GenLatexOutput::LineFeed (void)
{
  std::string lf;
  if (this->linefeed) {
    lf = "\n";
  }
  this->linefeed = false;
  return (lf);
}

// GetLine
int GenLatexOutput::GetLine (const Generic & rc)
{
  if (!rc.IsNil()) {
    Tuple gfp (GetCI().GetFilePos(ASTAUX::GetCid(rc)));
    if (gfp.GetBoolValue(1)) {
      const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
      return astpos.get_abs_uline().GetValue();
    }
  }
  return 0;
}

//////////////////
// Name: StartDef
// Descr:
//
/////////////////

void GenLatexOutput::StartDef (int new_type, int *old_type)
{
  if (new_type == *old_type) {
    return;
  }
  switch (new_type) {
    case TAG_TYPE_AS_TypeDef: {
      LatexOutput << "\\kTypes";
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_AS_ValueDef: {
      LatexOutput << "\\kValues";
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_PRETTY_PRINTER_FnDef: {
      LatexOutput << "\\kFunctions";
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_PRETTY_PRINTER_OpDef: {
      LatexOutput << "\\kOperations";
      LatexOutput << endl;
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_PRETTY_PRINTER_InstVarDef: {
      LatexOutput << "\\kInstanceVarDef";
      LatexOutput << endl;
      LatexOutput << "\\parlinebr";
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_PRETTY_PRINTER_ThreadDef: {
      LatexOutput << "\\kThreadDef";
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_PRETTY_PRINTER_SyncDef: {
      LatexOutput << "\\kSync";
      LatexOutput << endl;
      break;
    }
    case TAG_TYPE_PRETTY_PRINTER_Traces: {
      LatexOutput << "\\kTraces";
      LatexOutput << endl;
      break;
    }
#endif //VDMPP
  }

  *old_type = new_type;
}

Map GenLatexOutput::InverseMap (const Map & m)
{
  Map res_m;
  Set dom_m (m.Dom());
  Generic elm;
  for (bool bb = dom_m.First (elm); bb; bb = dom_m.Next (elm)) {
    res_m.Insert (m[elm], elm);
  }
  return (res_m);
}

void GenLatexOutput::GenRenaming (const Record & name, const Map & ren_m)
{
  if (ren_m.DomExists (name)) {
    LatexOutput << "\\ \\kw{renamed}\\ ";
    GenName (ren_m[name], this->LatexOutput);
  }
}

/////////////////////////
// Name: GenLatexOutput::GenIndex
// Descr:
// Generate latex index for VMD item
/////////////////////////
// the type of an index determines the style it will be printed in.
//  itype = fct_index     : function definition
//        = typedef_index : type definition
//        = typeocc_index : type name occurence
//        = mod_index     : module name definition
//        = state_index   : state definition
//        = class_index   : class name definition

void GenLatexOutput::GenIndex (const TYPE_AS_Name & name,// name of item
                               int itype)  // item type
{
  switch (itype) {
    case fct_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|FuncDef}";
      break;
    }

    case method_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|MethodDef}";
      break;
    }

    case methodocc_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|MethodOcc}";
      break;
    }

    case typedef_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|TypeDef}";
      break;
    }

    case fctocc_index: {
      // the generation of an index for function/operation occurence is
      // complicated by the fact that things cannot be looked up across
      // modules. we need to make the following assumptions:
      //   -  index only for name if defined in current module
      //   -  index for name if defined in other module
      //   -  index for name if pre/post/inv function

      if (this->INDEX == 2) {
        TYPE_AS_Ids id_l (name.get_ids ());
        if (IsPrePostInv (name) || this->Fns.InSet (name) || id_l.Length() == 2 ) {
          TYPE_AS_Name r (name);
          if (id_l.Length() == 2) {
            id_l.ImpTl();
            r.set_ids(id_l);
          }
          LatexOutput << "\\index{";
          LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (r)));
          LatexOutput << "|FuncOcc}";
        }
      }
      break;
    }

    case typeocc_index: {

      if (this->INDEX == 2) {
        TYPE_AS_Ids n (name.get_ids());
        TYPE_AS_Name r (name);
        if (n.Length() == 2) {
            n.ImpTl();
            r.set_ids(n);
        }
        LatexOutput << "\\index{";
        LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (r)));
        LatexOutput << "|TypeOcc}";
      }
      break;
    }

    case mod_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|ModDef}";
      break;
    }

    case state_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|StateDef}";
      break;
    }

    case class_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|ClassDef}";
      break;
    }

    case classocc_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|ClassOcc}";
      break;
    }

    case instvar_index: {
      LatexOutput << "\\index{";
      LatexOutput << TBWSTR::wstring2mbstr(InsertGreekLettersInIndex (Name2Str (name)));
      LatexOutput << "|InstVarDef}";
      break;
    }
  }
}

bool GenLatexOutput::IsPrePostInv (const TYPE_AS_Name & name)
{
  TYPE_AS_Ids id_l (name.get_ids());
  TYPE_AS_Id idstr (id_l[id_l.Length ()]);
  std::wstring id;
  idstr.GetString(id);

  std::wstring RX_pre  (L"pre_");
  std::wstring RX_post (L"post_");
  std::wstring RX_inv  (L"inv_");

  return (id.find(RX_pre) == 0) || (id.find(RX_post) == 0) || (id.find(RX_inv) == 0);
}

