/***
*  * WHAT
*  *    A program which converts VAL's into ascii
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/pp-ast.cc,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <fstream>
#include <stdlib.h>
#include "metaiv.h"
#include "val2x.h"
#include "tbutils.h"
#include "mini-p-print.h"
#include "contextinfo.h"
#include "Errors.h"
#include "ToolMediator.h"
#include "projectval.h"
#include "tb_exceptions.h"
#include "parse_expr.h"
#include "pexpr.h"
#include "astaux.h"
#include "asquotes.h"
#include "intconvquotes.h"
#include "tb_wstring.h"

// This is a flag, which indicates if this is the static test environment or not
// 15 Oct. 1998 11:45 -- Jesper K. Pedersen
int StaticTestEnvP=0;

bool ParseAnExprFromStream(Sequence& exprs, ContextInfo& ci, istream &in);

// This is the inverse parser, which translate "an AST of an AST of a
// programL" to an "AST of a program". This is necesary, when an ast i read
// in from disk, with the expectation of having this AST as a structure.
// Unfortunately, the AST will be located within an AST.
// 3 Feb. 1999 15:52 -- Jesper K. Pedersen 
Generic TranslateAST(Generic AST) 
{
  if (AST.IsRecord()) {

    //------------ mk_ ------------------------------------
    if (((Record) AST).Is(TAG_TYPE_AS_RecordConstructorExpr)) {
      TYPE_AS_RecordConstructorExpr ASTR = (TYPE_AS_RecordConstructorExpr) AST;
      int tagNo,size;
      TYPE_AS_Name tagAsNm = ASTR.get_tag();
      wstring tagNm = ASTAUX::ASName2String(tagAsNm);
      Sequence fields = ASTR.get_fields();
      if (VDMGetDefaultRecInfoMap().GetTag(tagNm, tagNo, size)) {
        Record result(tagNo,size);
        if (tagNo == TAG_TYPE_REP_NumericTypeRep) { 
          Record type = fields[1];
          Quote TypeFields (type.GetField(1));
          result.SetField(1, INT2Q::TransformIntegersBack(TypeFields));
        } else {
          for(int i=1;i<=size;i++) {
            result.SetField(i,TranslateAST(fields[i]));
          }
        }
        return result;
      }
      else {
        return Nil();
      }
    }

    //------------ SeqEnumerationExpr -----------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_SeqEnumerationExpr)) {
      Sequence els =((TYPE_AS_SeqEnumerationExpr) AST).get_els();
      Sequence res;
      Generic elm;
      for(int more=els.First(elm); more; more=els.Next(elm)) {
        res.ImpAppend(TranslateAST(elm));
      }
      return res;
    }

    //------------ MapEnumerationExpr ----------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_MapEnumerationExpr)) {
      Map res;
      Sequence els =((TYPE_AS_MapEnumerationExpr) AST).get_els();
      Generic g;
      for(int more=els.First(g); more; more=els.Next(g)) {
        TYPE_AS_Maplet elm = g;
        TYPE_AS_Expr dom = elm.get_mapdom();
        TYPE_AS_Expr rng = elm.get_maprng();
        res.Insert(TranslateAST(dom),TranslateAST(rng));
      }
      return res;
    }

    //----------- TupleConstructorExpr----------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_TupleConstructorExpr)) {
      TYPE_AS_TupleConstructorExpr tuple = AST;
      Sequence fields = tuple.get_fields();
      Tuple res(fields.Length());
      for (int i=1; i <= fields.Length(); i++) {
        res.SetField(i, TranslateAST(fields[i]));
      }
      return res;
    }
      
    //----------- SetEnumerationExpr----------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_SetEnumerationExpr)) {
      Set res;
      Sequence els =((TYPE_AS_SeqEnumerationExpr) AST).get_els();
      Generic elm;
      for(int more=els.First(elm); more; more=els.Next(elm)) {
        res.Insert(TranslateAST(elm));
      }
      return res;
    }
      

    //------------ RealLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_RealLit)) {
      return ((TYPE_AS_RealLit) AST).get_val();
    }

    //------------ NumLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_NumLit)) {
      return ((TYPE_AS_NumLit) AST).get_val();
    }

    //------------ BoolLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_BoolLit)) {
      return ((TYPE_AS_BoolLit) AST).get_val();
    }

    //------------ CharLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_CharLit)) {
      return ((TYPE_AS_CharLit) AST).get_val();
    }

    //------------ TextLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_TextLit)) {
      return ((TYPE_AS_TextLit) AST).get_val();
    }

    //------------ QuoteLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_QuoteLit)) {
      Token txt=((Record) AST).GetField(1);
      return Quote(txt.GetValue());
    }
    
    //------------ NilLit ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_NilLit)) {
      return Nil();
    }

    //------------ PrefixExpr ---------------------------------
    else if (((Record) AST).Is(TAG_TYPE_AS_PrefixExpr)) {
      Int op = (Int) ((TYPE_AS_PrefixExpr) AST).get_opr();
      TYPE_AS_Expr expr = ((TYPE_AS_PrefixExpr) AST).get_arg();
      
      if (op == NUMMINUS) {
        Generic val = TranslateAST(expr);
        if (!val.IsReal()) {
          wcout << L"Expr is not a real!";
          exit(-1);
        }
        return -((Real) val);
      }
      else {
        wcout << L"unhandled op: " << op;
        exit(-1);
      }
    }
    else {
      wcout << L"Unknow AST:" << endl << AST;
      exit(-1);
    }
  }
  else {
    wcout << L"Not a record!" << endl << AST;
    exit(-1);
    //return AST;
  }
}

  
int main(int argc, const char *argv[]) 
{
  if (argc != 2) {
    wcout << L"Usage: " << TBWSTR::cinstr2wstring(argv[0]) << L" <inputfile>" << endl;
    exit(-1);
  }

  //wchar_t *type = argv[1];
  wstring type = TBWSTR::cinstr2wstring(argv[1]);

    
  wstring infile = TBWSTR::cinstr2wstring(argv[2]);

  ifstream istr (TBWSTR::wstring2fsstr(infile).c_str());
  Generic val;

  if (!istr) {
    vdm_log << infile << L" not found" << endl;
    exit(1);
  }

  // initialize
  init_AS();
  init_REP();

  Sequence exprs;
  ContextInfo ci;
  bool parse_error = ParseAnExprFromStream(exprs,ci,istr);
  if (parse_error) {
    wcout << L"Parse Error!" << endl;
    exit(-1);
  }
  Generic expr = ((Sequence) exprs)[1];
  Generic ast = TranslateAST(expr);

  wcout << MPP::MiniPP(ast) << endl;
}
