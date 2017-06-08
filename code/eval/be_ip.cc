/***
*  * WHAT
*  *    Back-end to the translation of the result of
*  *    the interpretation of the dynamic semantics
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/be_ip.cc,v $
*  * VERSION
*  *    $Revision: 1.54 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:03 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Hanne Carlsen + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include "contextinfo.h"
#include "AS.h"
#include "parse_expr.h"
#include "semvalues.h"
#include "astaux.h"
#include "val2x.h"
#include "asquotes.h"
#include "libman.h"
#include "cli_options.h"
#include "settings.h"
#include "tbutils.h"

Generic ConvASTofSem2Generic(const Record &);

Generic ConvASTRec2Generic(const Record & ast)
{
  switch(ast.GetTag()) {
    case TAG_TYPE_AS_RecordConstructorExpr: {
      const TYPE_AS_Name & tagname (ast.GetRecord(1));
      const SEQ<TYPE_AS_Expr> & fields (ast.GetSequence(2));
      wstring tag_str (ASTAUX::ASName2String(tagname));
      if ( tag_str == L"SEM`REC" ) {
  
        // Compute the record tag.
        // Example of format of tag:
        // mk_AS`RecordConstructorExpr( mk_AS`Name( [ L"AS","Name" ],
        //    nil ),
        // [ mk_AS`SeqEnumerationExpr( [ mk_AS`TextLit( L"A" ),
        //        mk_AS`TextLit( L"E" ) ],
        //      nil ),
        //   mk_AS`NilLit( ) ],
        // nil ),
        const TYPE_AS_RecordConstructorExpr & asrec (fields[1]);
        const SEQ<TYPE_AS_Expr> & l (asrec.get_fields());
        const TYPE_AS_SeqEnumerationExpr & asseqenum (l[1]);
        Sequence val (ConvASTofSem2Generic(asseqenum));
  
        const Sequence & firstname_l (val[1]);
        const Sequence & secondname_l (val[2]);
        wstring firstname, secondname;
        if (!firstname_l.GetString(firstname) ||
            !secondname_l.GetString(secondname)) {
          vdm_err << L"InternalError \n";
          return Generic();
        }
  
        TYPE_AS_Ids tag_ids;
        tag_ids.ImpAppend( ASTAUX::MkId(firstname));
        tag_ids.ImpAppend( ASTAUX::MkId(secondname));
        TYPE_AS_Name tag (ASTAUX::MkNameFromIds(tag_ids, NilContextId));
  
        // The map of not abstract fields and abstract fields
        // REC_V ::
        //    tag : AS`Name
        //    v   : map nat to SEM`VAL
        //    v_dc: map nat to SEM`VAL
        Map v (ConvASTofSem2Generic(fields[2]));
        Map v_dc (ConvASTofSem2Generic(fields[3]));
  
        // Declare the record if it is not declared
  
        if (!SemRec::IsDecl(tag)) {
          SEQ<Bool> dc_l;
          int size (v.Size() + v_dc.Size());
          for (int i = 1; i <= size; i++) {
            dc_l.ImpAppend(Bool(true));
          }
          SemRec::Decl_SEM_REC(tag, size, dc_l);
        }
  
        // Construct the sequence of semantic values.
        v.ImpOverride(v_dc);
        Sequence fields_v;
        int size (v.Size());
        for (int i = 1; i <= size; i++) {
          fields_v.ImpAppend(v[Int(i)]);
        }
  
        TYPE_DYNSEM_SEM_SemRecord rec_v (SemRec::mk_SEM_REC(tag, fields_v));
        return rec_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value);
  
      }
      else if ( tag_str == L"SEM`BOOL" ||
                tag_str == L"SEM`NUM" ||
                tag_str == L"SEM`CHAR" ||
                tag_str == L"SEM`SEQ" ||
                tag_str == L"SEM`SET" ||
                tag_str == L"SEM`MAP"
              ) {
        return ConvASTofSem2Generic(fields[1]);
      }
      else if (tag_str == L"SEM`TUPLE") {
        Sequence l (ConvASTofSem2Generic(fields[1]));
        size_t size (l.Length());
        Tuple res(size);
        for (size_t i=1; i<=size; i++) {
          res.SetField(i, l[i]);
        }
        return res;
      }
      else if (tag_str == L"SEM`TOKEN") {
        return mk_token(ConvASTofSem2Generic(fields[1]));
      }
      else if (tag_str == L"SEM`QUOTE") {
        Sequence l (ConvASTofSem2Generic(fields[1]));
        wstring str;
        if (!l.GetString(str)) {
          vdm_err << L"Internal Error in ConvASTRec2Generic. Unknown content of QuoteLit" << endl;
          return Generic();
        }
        return Quote(str);
      }
      else if ( tag_str == L"SEM`NIL" ) {
        return Nil();
      }
      else if ( tag_str == L"SEM`OBJ_Ref" ||
                tag_str == L"SEM`OBJ" ||
                tag_str == L"SEM`UNDEF" ||
                //tag_str == L"SEM`DLVAL"  ||
                tag_str == L"SEM`LOC" )
      {
        vdm_err << L"Internal Error in ConvASTRec2Generic. Cannot handle semantic value :" << tag_str << endl;
        return Generic();
      }
      else if ( tag_str == L"SEM`EXIT" )
      {
        return ConvASTofSem2Generic(fields[1]);
      }
      else if ( tag_str == L"SEM`ExplFN" ||
                tag_str == L"SEM`ExplPOLY" ||
                tag_str == L"SEM`DLFN" ) {
        return Token(L"FUNCTION_VAL");
      }
      else if ( tag_str == L"SEM`ExplOP" ||
                tag_str == L"SEM`DLOP" ) {
        return Token(L"OPERATION_VAL");
      }
      else if ( tag_str == L"SEM`RETURN" ||
                tag_str == L"SEM`CONT" ) {
        wcout << L"(no return value)";
        return Token(L"(no return value)");
      }
    }
    case TAG_TYPE_AS_BoolLit :
      return ast.GetBool(pos_AS_BoolLit_val);
    case TAG_TYPE_AS_RealLit :
      return ast.GetReal(pos_AS_RealLit_val);
    case TAG_TYPE_AS_NumLit :
      return ast.GetReal(pos_AS_NumLit_val);
    case TAG_TYPE_AS_CharLit :
      return ast.GetChar(pos_AS_CharLit_val);
    case TAG_TYPE_AS_TextLit :
      return ast.GetSequence(pos_AS_TextLit_val);

    case TAG_TYPE_AS_QuoteLit : {
      const SEQ<Char> & val (ast.GetField(pos_AS_QuoteLit_val));
      wstring str;
      if (!val.GetString(str)) {
        vdm_err << L"Internal Error in ConvASTRec2Generic. Unknown content " << endl
                << L"of QuoteLit" << endl;
        return Generic();
      }
      return Quote(str);
    }
    case TAG_TYPE_AS_SetEnumerationExpr : {
      const SEQ<TYPE_AS_Expr> & els (ast.GetSequence(pos_AS_SetEnumerationExpr_els));
      Set res;
      size_t len_els = els.Length(); 
      for (size_t index = 1; index <= len_els; index++)
        res.Insert(ConvASTofSem2Generic(els[index]));
      return res;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr : {
      const SEQ<TYPE_AS_Expr> & els (ast.GetSequence(pos_AS_SeqEnumerationExpr_els));
      Sequence res;
      size_t len_els = els.Length();
      for (size_t index = 1; index <= len_els; index++)
        res.ImpAppend(ConvASTofSem2Generic(els[index]));
      return res;
    }
    case TAG_TYPE_AS_MapEnumerationExpr : {
      const SEQ<TYPE_AS_Maplet> & els (ast.GetSequence(pos_AS_MapEnumerationExpr_els));
      Map res;
      size_t len_els = els.Length();
      for (size_t index = 1; index <= len_els; index++)
      {
        const TYPE_AS_Maplet & m (els[index]);
        res.Insert(ConvASTofSem2Generic(m.GetRecord(pos_AS_Maplet_mapdom)),
                   ConvASTofSem2Generic(m.GetRecord(pos_AS_Maplet_maprng)));
      }
      return res;
    }
    case TAG_TYPE_AS_NilLit : return Nil();
    case TAG_TYPE_AS_PrefixExpr : {
      const Int & op (ast.GetInt(pos_AS_PrefixExpr_opr));
      if ( op != NUMMINUS ) {
        vdm_err <<L"Internal error in ConvASTRec2Generic: Unknown operator in PrefixExpr" << endl;
        return Generic();
      }
      Real r (ConvASTofSem2Generic(ast.GetRecord(pos_AS_PrefixExpr_arg)));
      return Real(-r);
    }
    default: {
      vdm_err <<L"Internal error in ConvASTRec2Generic: " << endl
              <<L"Unknown AS tag" << endl;
      return Generic();
    }
  }
}

Generic ConvASTofSem2Generic(const Record & ast)
{
  if (ast.IsRecord()) {
    return ConvASTRec2Generic(ast);
  }
  else {
    vdm_err << L"Internal Error in ConvAST2CPP. Met mt_text" << endl;
    return Generic();
  }
}

TBOptions * cliOpt = NULL;
vdm_ToolKit * toolkit = NULL;

int main()
{
  cliOpt = (TBOptions*) new CLIOptions;

  // Initialise the sem records
  init_AS();
  VAL2X::init_EVAL_SEM();

  // parse the file
  Sequence exprs;
  ContextInfo ci;
  bool parse_error = ParseAnExpr(exprs, ci);

  if (parse_error) {
    vdm_err << L"Some error occurred during the parsing of the expression\n";
  }

  // Convert the expression to the corresponding meta-iv rep.
  Generic g (ConvASTofSem2Generic(exprs.Hd()));

// 2010023 -->
  Generic ast (exprs.Hd());
  if (ast.Is(TAG_TYPE_AS_RecordConstructorExpr))
  {
    const TYPE_AS_Name & tagname (Record(ast).GetRecord(pos_AS_RecordConstructorExpr_tag));
    wstring tag_str (ASTAUX::ASName2String(tagname));
    if ( tag_str == L"SEM`EXIT" ) {
      wcout << L"exit ";
    }
  }
  
  // Print the semantic value on the standard output.
  if (g != Token(L"(no return value)"))
    g.pr_ascii(wcout, ( Settings.PrintFormat() ? 0 : -1 ));

  if (cliOpt != NULL) delete cliOpt;
  return 0;
}
