#include "pog-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "POG_uMEDIATOR.h"
#include "SPEC_uAST.h"

static PogInterface thePogInterface;

PogInterface& GetPogInterface()
{
  return thePogInterface;
};

void PogInterface::setup()
{
  vdm_SPEC_uAST_setup();
  vdm_POG_uMEDIATOR_setupPOG();
}

void PogInterface::genPOForTypeDef (int index)
{
  // vdm_POG_uMEDIATOR_GenPOForTypeDef(Int(index));
}

void PogInterface::genPOForFnDef (int index)
{
  // vdm_POG_uMEDIATOR_GenPOForFnDef(Int(index));
}

void PogInterface::genPOForValueDef ()
{
  // vdm_POG_uMEDIATOR_GenPOForValueDef();
}

int PogInterface::getNumberOfPO()
{
//  return 0; //vdm_POG_uMEDIATOR_GetNumberOfPO().GetValue();  
  return vdm_POG_uMEDIATOR_GetNumberOfPO().GetValue();  
}

wstring PogInterface::getPO(int index)
{
  return wstring(); //Seq2Str(vdm_POG_uMEDIATOR_GetPO(Int(index)));
}

wstring PogInterface::getPOStatus(int index)
{
  return wstring(); //Seq2Str(vdm_POG_uMEDIATOR_GetPOStatus(Int(index)));
}

wstring PogInterface::getPOName(int index)
{
  return wstring(); //Seq2Str(vdm_POG_uMEDIATOR_GetPOName(Int(index)));
}

wstring PogInterface::getPOLocation(int index)
{
  return wstring(); // Seq2Str(vdm_POG_uMEDIATOR_GetPOLocation(Int(index)));
}

int PogInterface::getPOLine(int index)
{
  return 0; //vdm_POG_uMEDIATOR_GetPOLine(Int(index)).GetValue();
}

int PogInterface::getPOCol(int index)
{
  return 0; //vdm_POG_uMEDIATOR_GetPOCol(Int(index)).GetValue();
}
wstring PogInterface::getPODueTo(int index)
{
    /*
      cout << "PogInterface::getPODueTo" << endl;
 cout << index << endl;
 cout << Int(index) << endl;
   Int cast_index(index);
  TYPE_POG_uMEDIATOR_string seq;
  string str;
  cout << index << endl;
  cout << cast_index.GetValue() << endl;
  seq = vdm_POG_uMEDIATOR_GetPODueTo(cast_index);
  cout << "so far" << endl;
  str = Seq2Str(seq);
  cout << str << endl;
  return str;
  */
  return wstring(); //Seq2Str(vdm_POG_uMEDIATOR_GetPODueTo(Int(index)));
}

wstring PogInterface::getPOClassification(int index)
{
  return wstring(); //Seq2Str(vdm_POG_uMEDIATOR_GetPOClassification(Int(index)));
}

void PogInterface::genPO (const SEQ<TYPE_AS_Name> & modNames)
{
  if(modNames.IsEmpty()) {
    vdm_POG_uMEDIATOR_GenPO();
  } else {
    vdm_POG_uMEDIATOR_GenPOforModules (modNames);
  }
}

wstring PogInterface::getPrettyPO()
{
    return PTAUX::Seq2Str (vdm_POG_uMEDIATOR_GetPrettyPO());
}

wstring PogInterface::getSpecWithPO()
{
    return PTAUX::Seq2Str (vdm_POG_uMEDIATOR_GlueSpecWithPoLst());
}

SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> PogInterface::getPOs()
{
  return vdm_POG_uMEDIATOR_GetPOs();
}

Sequence PogInterface::getTextPOs()
{
  return toTextSeq(getPOs ());
}

SET<TYPE_TEST_ProofObligationPP> PogInterface::getTestPOs()
{
  SET<TYPE_TEST_ProofObligationPP> tpos;
  SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> pos (getPOs());
  Generic po_g;
  for(bool cc = pos.First(po_g); cc; cc = pos.Next(po_g))
  {
    TYPE_POG_uMEDIATOR_ProofObligationPP po (po_g);
//    if( po.get_po().IsEmpty() ) continue;
      
    TYPE_TEST_ProofObligationPP tpo;
    tpo.Init(po.get_po(), po.get_kind(), po.get_source(), po.get_loc());
    tpos.Insert(tpo);
  }
  return tpos;
}


//------- Entry dealing with displaying and filtering POGs

Set PogInterface::getModnms()
{
  return vdm_POG_uMEDIATOR_getModnms();
}

Set PogInterface::getMemnms()
{
  return vdm_POG_uMEDIATOR_getMemnms();
}

Set PogInterface::getLocs()
{
  return vdm_POG_uMEDIATOR_getLocs();
}

Set PogInterface::getKinds()
{
  return vdm_POG_uMEDIATOR_getKinds();
}

Set PogInterface::getChckMarks()
{
  return vdm_POG_uMEDIATOR_getChckMarks();
}

wstring PogInterface::printName(const Generic & v)
{
  Sequence s (vdm_PP_uTAR_uPO_PrintName(v));
  return s.GetString();
}

void PogInterface::setModnms(const Set & s)
{
  vdm_POG_uMEDIATOR_setModnms(s);
}

void PogInterface::setMemnms(const Set & s)
{
  vdm_POG_uMEDIATOR_setMemnms(s);
}

void PogInterface::setLocs(const Set & s)
{
  vdm_POG_uMEDIATOR_setLocs(s);
}

void PogInterface::setKinds(const Set & s)
{
  vdm_POG_uMEDIATOR_setKinds(s);
}

void PogInterface::setChckMarks(const Set & s)
{
  vdm_POG_uMEDIATOR_setChckMarks(s);
}

wstring PogInterface::printPOKind(const Generic & v)
{
  Sequence s (vdm_PP_uTAR_uPO_PrintPOKind(TYPE_POGTP_Classification(v)));
  return s.GetString();
}

wstring PogInterface::printPOLocClassification(const Generic & v)
{
  Sequence s (vdm_PP_uTAR_uPO_PrintPOLocClassification(TYPE_POGTP_LocClassification(v)));
  return s.GetString();
}

wstring PogInterface::printBool(const Generic & b)
{
  return (Bool(b) ? L"Yes" : L"No");
}

SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> PogInterface::applyFilters()
{
  return vdm_POG_uMEDIATOR_applyFilters();
}

Sequence PogInterface::applyFiltersText()
{
  return toTextSeq(vdm_POG_uMEDIATOR_applyFilters());
}

Tuple PogInterface::toText(const TYPE_POG_uMEDIATOR_ProofObligationPP & po)
{
  Int line (-1);
  Int column (-1);
  Int length (-1);
  Sequence file (L"");;

// 20081209 -->
  Tuple gfp (GetCI().GetFilePos(po.get_cid()));
  if (gfp.GetBoolValue(1))
  {
    const TYPE_CI_FileId & fid (gfp.GetInt(2));
    const TYPE_CI_TokenPos & stpos (gfp.GetRecord(3));
//    const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
    const TYPE_CI_TokenPos & endpos (gfp.GetRecord(5));
    int sline = stpos.get_abs_uline();
    int scol = stpos.get_column();
    int eline = endpos.get_abs_uline();
    int ecol = endpos.get_column();

    file = Sequence(ToolMediator::GetFileName(fid));
    line = sline;
    column = scol;
    
    if (sline == eline)
      length = (ecol - scol);
    else
      length = 1;
  }
// <-- 20081209

  TYPE_POGTP_LocContext loc(po.get_loc());

  Sequence modnm;
  Generic gmodnm = loc.get_modName();
  if(!gmodnm.IsNil()) {
    modnm = TYPE_AS_Name(gmodnm).get_ids()[1];
  }

  Generic gmemberName = loc.get_name();
  Sequence memnm;
  if(!gmodnm.IsNil()) {
    memnm = TYPE_AS_Name(gmemberName).get_ids()[1];
  }

  Generic get_checked (po.get_checked());
  TYPE_POGTP_LocClassification loc_class (loc.GetField(pos_POGTP_LocContext_class));
  Generic po_kind (po.get_kind());

  Sequence tmpfn;
  if (!file.IsEmpty()) {
    wstring fnm (file.GetString());
    TYPE_ProjectTypes_FileName tfn (ToolMediator::GetVDMTempFileName (PTAUX::mk_FileName (fnm)));
    tmpfn = Sequence(PTAUX::ExtractFileName (tfn));
  }

  Tuple res(12);
  res.SetField(1,Sequence(printBool(get_checked)));                            // checked
  res.SetField(2,modnm);                                                       // modnm
  res.SetField(3,memnm);                                                       // memnm
  res.SetField(4,Sequence(PogInterface::printPOLocClassification(loc_class))); // loc class
  res.SetField(5,Sequence(PogInterface::printPOKind(po_kind)));                // kind
  res.SetField(6,Int(po.get_no()));                                            // no
  res.SetField(7,file);                                                        // file
  res.SetField(8,line);                                                        // line
  res.SetField(9,column);                                                      // column
  res.SetField(10,length);                                                     // length
  res.SetField(11,po.get_po());                                                // po
  res.SetField(12,tmpfn);                                                      // tmpfn

  return res;
}

Sequence PogInterface::toTextSeq(const SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> & pogseq)
{
  Sequence res;
  if (!pogseq.IsEmpty())
  {
    size_t len_pogseq = pogseq.Length();
    for (size_t idx = 1; idx <= len_pogseq; idx++)
    {
      const TYPE_POG_uMEDIATOR_ProofObligationPP & po (pogseq[idx]);
      res.ImpAppend(toText(po));
    }
  }
  return res;
}

Tuple PogInterface::getPoId(const TYPE_POG_uMEDIATOR_ProofObligationPP & po)
{
  return mk_(po.get_loc(), po.get_no());
}

Bool PogInterface::setCheckMark(const Tuple & poId)
{
  return vdm_POG_uMEDIATOR_setCheckMark(poId);
}

Quote PogInterface::toLocs(const wstring & kind)
{
  if (kind == L"type")                    return Quote(L"types");
  else if (kind == L"function")           return Quote(L"functions");
  else if (kind == L"operation")          return Quote(L"operations");
  else if (kind == L"state")              return Quote(L"state");
  else if (kind == L"instance variables") return Quote(L"instanceVariables");
  else if (kind == L"instance invariant") return Quote(L"instanceInvariant");
  else if (kind == L"value")              return Quote(L"values");
  else if (kind == L"conjecture")         return Quote(L"conjectures");
  else if (kind == L"expression")         return Quote(L"expression");
  return Quote(L"unknown");
}

Set PogInterface::toKind(const wstring & kind)
{
  if (kind == L"sequence modification")     return mk_set(Quote(L"SeqModification"));
  else if (kind == L"sequence application") return mk_set(Quote(L"SeqApplication"));
  //else if (kind == L"map merge") return mk_set(Quote(L"MapMerge"));
  //else if (kind == L"distributed map merge") return mk_set(Quote(L"MapDistributedMerge"));
  else if (kind == L"map application") return mk_set(Quote(L"MapApplication"));
  else if (kind == L"map composition") return mk_set(Quote(L"MapComposition"));
  else if (kind == L"map iteration") return mk_set(Quote(L"MapIteration"));
  else if (kind == L"tuple selection") return mk_set(Quote(L"TupleSelect"));
  else if (kind == L"function application") return mk_set(Quote(L"FunctionApplication"));
  //else if (kind == L"operation application") return mk_set(Quote(L"OperationApplication"));
  else if (kind == L"function composition") return mk_set(Quote(L"FuncComp"));
  else if (kind == L"function iteration") return mk_set(Quote(L"FuncIteration"));
  //else if (kind == L"sequence comprehension") return mk_set(Quote(L"SeqComprehension"));
  //else if (kind == L"map comprehension") return mk_set(Quote(L"MapComprehension"));
  else if (kind == L"map enumeration") return mk_set(Quote(L"MapEnumeration"));
  else if (kind == L"compatible maps") return mk_set(Quote(L"MapCompatible"));
  else if (kind == L"map compatibility") return mk_set(Quote(L"MapSetOfCompatible"));
  else if (kind == L"map enumeration") return mk_set(Quote(L"MapSeqOfCompatible"));
  else if (kind == L"non-emptiness of let be such binding") return mk_set(Quote(L"LetBeStStmt"), Quote(L"LetBeStExpr"));
  else if (kind == L"exhaustive matching in cases expression") return mk_set(Quote(L"CasesExpr"));
  else if (kind == L"unique existence binding") return mk_set(Quote(L"ExistsUnique"));
  else if (kind == L"subtype") return mk_set(Quote(L"Subtype"));
  else if (kind == L"invariants") return mk_set(Quote(L"Invariants"));
  else if (kind == L"equality") return mk_set(Quote(L"Equality"));
  else if (kind == L"order") return mk_set(Quote(L"Order"));
  //else if (kind == L"flatType") return mk_set(Quote(L"FlatType"));
  else if (kind == L"non emptiness of binding") return mk_set(Quote(L"ValueBinding"));
  else if (kind == L"pattern binding") return mk_set(Quote(L"PatternBinding"));
  else if (kind == L"exhaustive function patterns") return mk_set(Quote(L"FunctionPatterns"));
  else if (kind == L"satisfiability") return mk_set(Quote(L"Satisfiability"));
  else if (kind == L"post condition") return mk_set(Quote(L"OpPostCondition"), Quote(L"PostCondition"));
  ////else if (kind == L"post condition") return mk_set(Quote(L"OpPostCondition")); 
  //else if (kind == L"OperationSat") return mk_set(Quote(L"OperationSat"));
  else if (kind == L"finiteness of set") return mk_set(Quote(L"FiniteSet"));
  else if (kind == L"finiteness of map") return mk_set(Quote(L"FiniteMap"));
  else if (kind == L"iteration of non-endofunction") return mk_set(Quote(L"NonEndoIteration"));
  else if (kind == L"non-zeroness") return mk_set(Quote(L"NonZero"));
  else if (kind == L"non-empty set") return mk_set(Quote(L"NonEmptySet"));
  else if (kind == L"non-empty sequence") return mk_set(Quote(L"NonEmptySeq"));
  //else if (kind == L"inverse map") return mk_set(Quote(L"MapInverse"));
  //else if (kind == L"division") return mk_set(Quote(L"IntDivision"));
  //else if (kind == L"remainder") return mk_set(Quote(L"Remainder"));
  //else if (kind == L"modulus") return mk_set(Quote(L"Modulus"));
  //else if (kind == L"distributed intersection") return mk_set(Quote(L"DInter"));
  //else if (kind == L"head of sequence") return mk_set(Quote(L"SeqHead"));
  //else if (kind == L"tail of sequence") return mk_set(Quote(L"SeqTail"));
  //else if (kind == L"set comprehension") return mk_set(Quote(L"SetComprehension"));
  ////else if (kind == L"non-emptiness of let be such binding") return mk_set(Quote(L"LetBeStStmt"));
  else if (kind == L"exhaustive matching in cases statement") return mk_set(Quote(L"CasesStmt"));
  else if (kind == L"terminating while loop") return mk_set(Quote(L"WhileLoopStmt"));
  else if (kind == L"state invariants") return mk_set(Quote(L"StateInvariants"));
  else if (kind == L"recursive function") return mk_set(Quote(L"RecursiveFunction"));
  return Set();
}
