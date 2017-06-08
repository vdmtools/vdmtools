/***
*  * WHAT
*  *    A function which converts an AST in which identifiers are represented
*  *    as tokens into an AST in which identifiers are represented as a 
*  *    sequence of wchar_t.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/transident.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2001/06/12 15:05:01 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Informa
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#include "transident.h"
#include "cgtag.h"
#include "AS.h"




////////////////////////////////
// Tranforms L"Token" to "seq of wchar_t" as it is being used in the
// generated code.
////////////////////////////////

Generic TransformIdentifiers(Generic g)
{
  metaivType mt;
  mt = g.gettype();

  switch(mt) {
  case mt_record: {
    Record rc(g);
    int tag = rc.GetTag();
    switch (tag) {

      /////////////////////
      // CPP TAGS
      /////////////////////

    case SquareIncl_CPP:
    case QuoteIncl_CPP:
    case StringLit_CPP:
    case Identifier_CPP: {
      Token id(rc.GetField(1));
      Record res(tag,1);
      res.SetField(1,Sequence(id.GetValue()));
      return res;
    }
    case IdMacroDef_CPP: {
      Token id(rc.GetField(1));
      Token ts(rc.GetField(2));
      Record res(tag,2);
      res.SetField(1,Sequence(id.GetValue()));
      res.SetField(2,Sequence(ts.GetValue()));
      return res;
    }

    case FctMacroDef_CPP : {
      Token id(rc.GetField(1));
      Token ts(rc.GetField(3));
      Record res(tag,3);
      res.SetField(1,Sequence(id.GetValue()));
      res.SetField(2,TransformIdentifiers(rc.GetField(2)));
      res.SetField(3,Sequence(ts.GetValue()));
      return res;
    }

    case File_CPP : {
      Token name(rc.GetField(1));
      Record res(tag,3);
      res.SetField(1,Sequence(name.GetValue()));
      res.SetField(2,TransformIdentifiers(rc.GetField(2)));
      res.SetField(3,TransformIdentifiers(rc.GetField(3)));
      return res;
    }

      /////////////////////
      // AS TAGS
      /////////////////////

    case TAG_TYPE_AS_OldName:
    case TAG_TYPE_AS_Name: {
      Sequence ids (rc.GetField(1));
      Sequence newids;
      Record res(tag,2);
      res.SetField(1,ids);
      res.SetField(2,rc.GetField(2));
      return res;
      break;
    }

    case QuoteLit:
      {
      Record res(tag,2);
      res.SetField(1,Sequence(rc.GetField(1)));
      res.SetField(2,rc.GetField(2));
      return res;
      }

    default: {
      int len = rc.Length();
      for (int i=1; i<=len; i++) {
	Generic field = TransformIdentifiers(rc.GetField(i));
	rc.SetField(i,field);
      }}
    }
    return rc;
    break;
  }
  case mt_tuple: {
    Tuple t = g;
    int len = t.Length();
    Generic field;
    for (int i=1; i<=len; i++) {
      field = t.GetField(i);
      field = TransformIdentifiers(field);
      t.SetField(i,field);
    }
    return t;
    break;
  }
  case mt_set: {
    Set s = g;
    Set res;
    Generic elem;
    for (int cont=s.First(elem); cont; cont=s.Next(elem)) {
      elem = TransformIdentifiers(elem);
      res.Insert(elem);
    }
    return res;
    break;
  }
  case mt_map: {
    Map m = g;
    Map res;
    Generic key;
    Generic dom;
    for (int cont=m.First(key,dom); cont; cont=m.Next(key,dom)) {
      key = TransformIdentifiers(key);
      dom = TransformIdentifiers(dom);
      res.Insert(key,dom);
    }
    return res;
    break;
  }
  case mt_sequence: {
    Sequence s = g;
    int len = s.Length();
    Generic elem;
    for (int i=1; i<=len; i++) {
      elem = s[i];
      elem = TransformIdentifiers(elem);
      s.ImpModify(i,elem);
    }
    return s;
    break;
  }
  default:
    break;
  }
  return g;
}


///////////////////////////////////////////////////////////
/// T H E  O T H E R   W A Y
///////////////////////////////////////////////////////////

Generic TransformIdentifiersBack(Generic g)
{
  metaivType mt;
  mt = g.gettype();

  switch(mt) {
  case mt_record: {
    Record rc(g);
    int tag = rc.GetTag();
    switch (tag) {

      /////////////////////
      // CPP TAGS
      /////////////////////

    case SquareIncl_CPP:
    case QuoteIncl_CPP:
    case StringLit_CPP:
    case Identifier_CPP: {
      Record res(tag,1);
      wstring s;
      Sequence(rc.GetField(1)).GetString(s); 
      return res.SetField(1,Token(s));
    }

    case IdMacroDef_CPP: {
      Sequence id(rc.GetField(1));
      Sequence ts(rc.GetField(2));
      wstring s1,s2;
      id.GetString(s1);
      ts.GetString(s2);
      Record res(tag,2);
      res.SetField(1,Token(s1));
      res.SetField(2,Token(s2));
      return res;
    }

    case FctMacroDef_CPP : {
      Sequence id(rc.GetField(1));
      Sequence ts(rc.GetField(3));
      wstring s1,s2;
      id.GetString(s1);
      ts.GetString(s2);
      Record res(tag,3);
      res.SetField(1,Token(s1));
      res.SetField(2,TransformIdentifiersBack(rc.GetField(2)));
      res.SetField(3,Token(s2));
      return res;
    }
    case File_CPP : {
      Sequence name(rc.GetField(1));
      wstring s1;
      name.GetString(s1);
      Record res(tag,3);
      res.SetField(1,Token(s1));
      res.SetField(2,TransformIdentifiersBack(rc.GetField(2)));
      res.SetField(3,TransformIdentifiersBack(rc.GetField(3)));
      return res;
    }

      /////////////////////
      // AS TAGS
      /////////////////////

    case TAG_TYPE_AS_OldName:
    case TAG_TYPE_AS_Name: {
      Sequence ids (rc.GetField(1));
      Sequence newids;
      Generic g;
      for (int i=ids.First(g); i; i=ids.Next(g)) {
	Sequence id(g);
        wstring s;
        id.GetString(s);
	newids.ImpAppend(Token(s));
      }
      Record res(rc); // do not throw PosInfo away !
      res.SetField(1,newids);
      res.SetField(2,rc.GetField(2));
      return res;
      break;
    }

    case QuoteLit: {
      Sequence id(rc.GetField(1));
      wstring s;
      id.GetString(s);
      Record res(tag,2);
      res.SetField(1,Token(s));
      res.SetField(2,rc.GetField(2));
      return res;
    }

    default: {
      int len = rc.Length();
      for (int i=1; i<=len; i++) {
	Generic field = TransformIdentifiersBack(rc.GetField(i));
	rc.SetField(i,field);
      }}
    }
    return rc;
    break;
  }
  case mt_tuple: {
    Tuple t = g;
    int len = t.Length();
    Generic field;
    for (int i=1; i<=len; i++) {
      field = t.GetField(i);
      field = TransformIdentifiersBack(field);
      t.SetField(i,field);
    }
    return t;
    break;
  }
  case mt_set: {
    Set s = g;
    Set res;
    Generic elem;
    for (int cont=s.First(elem); cont; cont=s.Next(elem)) {
      elem = TransformIdentifiersBack(elem);
      res.Insert(elem);
    }
    return res;
    break;
  }
  case mt_map: {
    Map m = g;
    Map res;
    Generic key;
    Generic dom;
    for (int cont=m.First(key,dom); cont; cont=m.Next(key,dom)) {
      key = TransformIdentifiersBack(key);
      dom = TransformIdentifiersBack(dom);
      res.Insert(key,dom);
    }
    return res;
    break;
  }
  case mt_sequence: {
    Sequence s = g;
    int len = s.Length();
    Generic elem;
    for (int i=1; i<=len; i++) {
      elem = s[i];
      elem = TransformIdentifiersBack(elem);
      s.ImpModify(i,elem);
    }
    return s;
    break;
  }
  default:
    break;
  }
  return g;
}
