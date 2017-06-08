/***
*  * WHAT
*  *    Function for testing the static semantics checker.
*  *    Implementation of test.vdm r1.21.2.2
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/statsem/test.cc,v $
*  * VERSION
*  *    $Revision: 1.23 $
*  * DATE
*  *    $Date: 2005/12/07 08:36:03 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "statsem.h"

Tuple StatSem::TestFunc (const Record & seqenum, const Int & ind)
{
  Record expr (QuoteLit,2);
#if 0 
 Sequence ql1_l;
  ql1_l.ImpAppend(Char('E'));
  ql1_l.ImpAppend(Char('X'));
  ql1_l.ImpAppend(Char('P'));
  ql1_l.ImpAppend(Char('R'));
#endif
  Token ql1_l(L"EXPR");

  expr.SetField(1, ql1_l);
  Record dummy (1,0);
  expr.SetField(2, dummy);

  Record typecomp (QuoteLit,2);
#if 0
  Sequence ql_l;
  ql_l.ImpAppend(Char('T'));
  ql_l.ImpAppend(Char('Y'));
  ql_l.ImpAppend(Char('P'));
  ql_l.ImpAppend(Char('E'));
  ql_l.ImpAppend(Char('C'));
  ql_l.ImpAppend(Char('O'));
  ql_l.ImpAppend(Char('M'));
  ql_l.ImpAppend(Char('P'));
#endif
  Token ql_l(L"TYPECOMP");
  typecomp.SetField(1, ql_l);
  typecomp.SetField(2, dummy);

  Bool reswf(1);
  Sequence test(seqenum.GetField(1));
  Record func(test[1]);
  
  if (func == expr) {
    Tuple res (wf_Expr(ind, test[2], Record(AllTypeRep,0)));
#ifdef VDMPP
    Generic g (StripAccessType (res.GetField (2)));
    res.SetField (2,g);
#endif
    return res;
  }
    if (func == typecomp)
    { int i,j;
      for(i=2; i<= test.Length(); i++)
        for(j=i+1; j <= test.Length(); j++)
          { Record rc1 (TypeNameRep, 1); 
            Record rc2 (TypeNameRep, 1);
            Bool iandj;

            rc1.SetField(1, test[i]); 
            rc2.SetField(1, test[j]); 
  
            iandj = IsCompatible(ind, rc1, rc2);

            if (!iandj.GetValue())
              { reswf = 0;
                //---------------------------------
                // Error message #338
                // L"%1" is not compatible with "%2"
                //---------------------------------
                GenErr(Nil(), ERR,338,PrintName(test[i]),PrintName(test[j]), Sequence());
              }
          }
      Tuple res(2);
      Record unittprep (UnitTypeRep, 0);
#ifdef VDMPP
      unittprep = StripAccessType (unittprep);
#endif // VDMPP
      res.SetField(1, reswf);
      res.SetField(2, unittprep);
      return res;
    }
    else
      vdm_log << L"Test not defined\n";
    
    vdm_err << L"Internal error. Did you remember to use the -s option?\n";
    exit(-1);
}

// TestStmt
Bool StatSem::TestStmt (const Record & unit, const Record & see, const Int & ind)
{
  Sequence test (see.GetField(1));
  Record quotelit (test[1]);
  wstring func (Token(quotelit.GetField(1)).GetValue());
  if (func == L"STMT") {
    Record opnm (test[2]);
#ifdef VDMSL 
    if (ExpandModule (POS, unit))
#endif // VDMSL
#ifdef VDMPP
    if (ExpandClass (POS, unit))
#endif // VDMPP
 {
      Tuple infer (FindConstruct (opnm, unit.GetField(3)));
      Int kind (infer.GetField (1));
      Generic construct (infer.GetField (2));

      if (construct.IsNil()) {
        //------------------------------------------------
        // Error message #339
        // You are trying to test a non-existing construct
        //------------------------------------------------
        GenErr(Nil(), ERR,339, Sequence());
        return Bool(false);
      }
      else {
        switch(kind.GetValue ()) {
        case OP:
          SetContext (ALL);
          if (Record (construct).Is(ImplOpDef))
            return Bool(wf_ImplOperation (ind, construct));
          else if (AnyHandlers (Record (construct).GetField (4))) {
            wf_Operations(ind, Record (unit.GetField (3)).GetField (4));
            StatSemErrors = Sequence ();
            FullOpTest(true);
            return Bool(wf_ExplOperation(ind,construct));
          }
          else if (Record (construct).Is(ExplOpDef))
            return Bool(wf_ExplOperation(ind,construct));
          else
            return Bool(wf_ExtExplOperation(ind,construct));
          break;
        case FUN:
          if (Record (construct).Is(ImplFnDef))
            return Bool(wf_ImplFunction (ind, construct));
          else if (Record (construct).Is(ExplFnDef))
            return Bool(wf_ExplFunction (ind, construct));
          else
            return Bool(wf_ExtExplFunction (ind, construct));
          break;
#ifdef VDMPP
	case INST:
	  return Bool (wf_InstanceVars (ind,Record
					(unit.GetField (3)).GetField (5)));
#endif // VDMPP
        case TYPE: {
          Map tpm;
          tpm.Insert(opnm,construct);
          return Bool(wf_TypeDefs(ind,tpm));
          break;
        }
        default:
          vdm_log << L"Non-existing construct type\n";
          return Bool(false);
        }
      }
    }
    else {
      //-----------------------------
      // Error message #340
      // Module could not be expanded
      //-----------------------------
      GenErr(Nil(),ERR,340, Sequence());
      return Bool(false);
    }
  }
#ifdef VDMSL 
  else if (func == L"MODULES")
    return Bool(wf_Module (ind, unit));
#endif // VDMSL
#ifdef VDMPP
  else if (func == L"CLASSES")
    return Bool (wf_Class (ind,unit));
#endif // VDMPP
  else { 
    vdm_log << L"Non-existing construct type\n";
    return Bool(false);
  }
}

Tuple StatSem::FindConstruct (const Record & fullnm, const Generic & defg)
{
  Tuple res (2);
  if (!defg.IsNil ()) {
    TYPE_AS_Definitions defs (defg);
    Record nm(fullnm);
    Sequence ids (fullnm.GetField(1));
    if (ids.Length() > 1) {
      ids.ImpTl();
      nm.SetField(1,ids);
    }
    Map tnm (defs.get_typem ());
    Map fnm (defs.get_fnm ());
    Map opm (defs.get_opm ());
    
    Generic g;
    if (fnm.DomExists(nm,g)) {
      res.SetField (1, (Int)FUN);
      res.SetField (2, g);
    }
    else if (opm.DomExists (nm,g)) {
      res.SetField (1, (Int)OP);
      res.SetField (2, g);
    }
    else if (tnm.DomExists(nm,g)) {
      res.SetField(1,Int(TYPE));
      res.SetField(2,g);
    }
#ifdef VDMPP
    else if (nm == Record (MkName (Token (L"TestInstVar"),NilContextId))) {
      res.SetField (1, (Int)INST);
      res.SetField (2, Nil ());
    }
#endif // VDMPP
    else {
      res.SetField (1, (Int)OP);
      res.SetField (2, Nil ());
    }
  }
  else {
    res.SetField (1, (Int)OP);
    res.SetField (2, Nil ());
  }

  return res;
}

int StatSem::AnyHandlers (const Record & opbody)
{
  if (opbody.GetField (1).IsRecord ())
    return AnyHandlersAux (Record (opbody.GetField (1)));
  else
    return 0;
}

int StatSem::AnyHandlersAux (const Record & stmt) {
  int tag = stmt.GetTag ();
  switch (tag) {
  case DefStmt:
  case LetStmt:
  case WhileLoopStmt:
    return AnyHandlersAux (stmt.GetField (2));
  case LetBeSTStmt:
  case SetForLoopStmt:
    return AnyHandlersAux (stmt.GetField (3));
  case SeqForLoopStmt:
    return AnyHandlersAux (stmt.GetField (4));
  case IndexForLoopStmt:
    return AnyHandlersAux (stmt.GetField (5));
  case IfStmt: {
    Generic elseifPart = stmt.GetField(4);
    if (AnyHandlersAux (stmt.GetField (2)))
      return 1;
    else if (! elseifPart.IsNil() && (AnyHandlersAux(elseifPart)))
      return 1;
    else {
      Generic g;
      Sequence els_l (stmt.GetField (3));
      for (bool bb = els_l.First (g); bb; bb = els_l.Next (g))
	if (AnyHandlersAux (Record (g).GetField (2)))
	  return 1;
      return 0;
    }
  }
  case CasesStmt : {
    Generic s (stmt.GetField (3));
    if (!s.IsNil () && AnyHandlersAux(s))
      return 1;
    else {
      Generic g;
      Sequence alt_l (stmt.GetField (2));
      for (bool bb = alt_l.First (g); bb; bb = alt_l.Next (g))
	if (AnyHandlersAux (Record (g).GetField (2)))
	  return 1;
      return 0;
    }
  }
  case AlwaysStmt:
  case TrapStmt:
  case RecTrapStmt:
    return 1;
  case NonDetStmt:
  case BlockStmt: {
    Sequence s_l;
    if (tag == BlockStmt)
      s_l = stmt.GetField (2);
    else
      s_l = stmt.GetField (1);
    Generic g;
    for (bool bb = s_l.First (g); bb; bb = s_l.Next (g))
      if (AnyHandlersAux (g))
	return 1;
    return 0;
  }
  default:
    return 0;
  }
}

