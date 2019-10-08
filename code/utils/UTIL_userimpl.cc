//
// UTIL.cc
//
// Implementation of util.vdm v1.3
//
// $Id: UTIL.cc,v 1.6 2001/09/06 10:10:01 mikhail Exp $
//

#include "UTIL.h"
#include "projectval.h"
#include "tbutils.h"
#include "rt_errmsg.h"
#include "tb_exceptions.h"
#include "astaux.h"
#include "statsem.h"

Real vdm_UTIL_minset(const type_rS & p_s)
{
  return UTIL::minset(p_s);
}

type_cL vdm_UTIL_itoa(const Int& i )
{
  return UTIL::itoa(i);
}

type_cL vdm_UTIL_ReplacePercent(const type_cL & err, const type_cLL & Txts)
{
  return UTIL::ReplacePercent(err, Txts);
}

// set2seq
// s : set of @tp
// -> seq of @tp
Sequence UTIL::set2seq(const Set & s)
{
  Set s_q (s);
  Sequence res;
  Generic v;
  for (bool bb = s_q.First(v); bb; bb = s_q.Next(v)) {
    res.ImpAppend(v);
  }
  return res;
}

// minset
// p_s : set of real
// -> real
Real UTIL::minset(const SET<Real> & p_s)
{
  Real l_v (p_s.GetElem());
  if( p_s.Card() == 1 ) {
    return l_v;
  }
  else {
    Set new_p_s( p_s );
    new_p_s.RemElem( l_v );
    Real l_minrest (minset( new_p_s ));
    if( l_v > l_minrest ) {
      return l_minrest;
    }
    else {
      return l_v;
    }
  }
}

// itoa
// n : nat
// -> seq of char
SEQ<Char> UTIL::itoa(const Int & n)
{
  return SEQ<Char>(n.ascii());
}

// ReplacePercent
// err : seq of char
// Txts : seq of seq of char
// ==> seq of char
SEQ<Char> UTIL::ReplacePercent(const SEQ<Char> & err, const SEQ< SEQ<Char> > & Txts)
{
  int count = Txts.Length();
  bool percent = false;
  int max = 0;

  SEQ<Char> res;
  size_t len_err = err.Length();
  for (size_t idx = 1; idx <= len_err; idx++) {
    const Char & ch (err[idx]);
    switch (ch.GetValue()) {

      case L'%': {
        if (percent) {
          percent = false;
          res.ImpAppend(ch);
        } 
        else {
          percent = true;
        }
        break;
      }

      case L'1':
      case L'2':
      case L'3':
      case L'4':
      case L'5':
      case L'6':
      case L'7':
      case L'8':
      case L'9': {
        if (percent) {
          int num = ch.GetValue() - L'0';
          if (count < num) {

#ifndef TESTSPEC
#ifndef _MSC_VER
            vdm_err << endl;
            vdm_err << L"Number of arguments doesn't match expected number of arguments ";
            vdm_err << L"to error function in UTIL::ReplacePercent" << endl;
            vdm_err << L"message: " << err << endl;
            vdm_err << L"arguments: " << Txts << endl << flush;
//            vdm_err << L"Error index out of range in UTIL::ReplacePercent" << endl;
#endif //_MSC_VER
#endif //TESTSPEC
            throw TB_Exception(ERR_IP);
          }
          percent = false;
          if (num > max) {
            max = num;
          }
          res.ImpConc(Txts[num]);
        }
        else {
          res.ImpAppend(ch);
        }
        break;
      }
//      case L'"': {
//        res.ImpAppend(ch);
//        break;
//      }
      default: {
        res.ImpAppend(ch);
        break;
      }
    }
  }
  if (count > max) {
#ifndef TESTSPEC
#ifndef _MSC_VER
    vdm_err << endl;
    vdm_err << L"Number of arguments doesn't match expected number of arguments ";
    vdm_err << L"to error function in UTIL::ReplacePercent" << endl;
    vdm_err << L"message: " << err << endl;
    vdm_err << L"arguments: " << Txts << endl << flush;
#endif //_MSC_VER
#endif //TESTSPEC
    throw TB_Exception(ERR_IP);
  }
  return res;
}

// split[@T]
// cs : seq of @T
// ch : @T
// -> seq of seq of @T
Sequence UTIL::split(const Sequence & l, const Common & sep) {
  if ( 0 == l.Find(sep)) {
    return mk_sequence(l);
  }
  else {
    Sequence seq = l;
    Sequence res;
    while (!seq.IsEmpty()) {
      size_t index = seq.Find(sep);
      if ( 0 == index ) {
        res.ImpAppend(seq);
        seq.Clear();
      }
      else {
        res.ImpAppend(seq.SubSequence( 1, index - 1 ));
        seq = seq.SubSequence( index + 1, seq.Length() );
      }
    }
    return res;
  }
}

