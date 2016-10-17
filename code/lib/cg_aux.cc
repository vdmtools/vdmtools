/***
*  * WHAT
*  *    CG library interface definitions
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/cg_aux.cc,v $
*  * VERSION
*  *    $Revision: 1.15 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <fstream>
#include "cg.h"
#include "cg_aux.h"

#include <time.h>
#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/time.h>
#endif // _MSC_VER

Set CGUTIL::Permute (const Sequence& l)
{
  return l.Permute();
}

//static Sequence QS (const Sequence& sq);
Sequence CGUTIL::Sort (const Set& ns_)
{
  Sequence res;
  if (ns_.IsEmpty ())
    return res;
  else {
    Set ns (ns_);
    Generic tmpe;
    for (bool bb = ns.First (tmpe); bb; bb = ns.Next (tmpe))
      if (! (tmpe.IsInt () || tmpe.IsReal ()))
        return res.ImpPrepend(tmpe);
      else
        res.ImpAppend (tmpe);
    return CGUTIL::QS (res);
  }
}

Sequence CGUTIL::QS (const Sequence & sq)
{
  if (sq.Length() <= 1)
    return sq;
  else {
    Real half (sq[sq.Length() / 2]);
    size_t len_sq = sq.Length();
    Sequence sq1, sq2, sq3;
    for (size_t idx = 1; idx <= len_sq; idx++)
    {
      Real r1 (sq[idx]);
      if (r1 < half)
        sq1.ImpAppend (r1);
      else if (r1 == half)
        sq2.ImpAppend (r1);
      else
        sq3.ImpAppend (r1);
    }
    Sequence sorted_res (QS (sq1));
    sorted_res.ImpConc (sq2);
    sorted_res.ImpConc (QS (sq3));
    return sorted_res;
  }
}

bool CGUTIL::is_real(const Generic & var)
{
  return (var.IsInt() || var.IsReal());
}

bool CGUTIL::is_int(const Generic & var)
{
  return (var.IsInt() || (var.IsReal() && static_cast<const Real &>(var).IsInt()));
}

bool CGUTIL::is_nat(const Generic & var)
{
  return ((var.IsInt() && (static_cast<const Int &>(var).GetValue() >= 0)) ||
          (var.IsReal() && static_cast<const Real &>(var).IsNat()));
}

bool CGUTIL::is_nat1(const Generic & var)
{
  return ((var.IsInt() && (static_cast<const Int &>(var).GetValue() > 0)) ||
          (var.IsReal() && static_cast<const Real &>(var).IsNatOne()));
}

Int CGUTIL::IAbs(const Real & v)
{
  return static_cast<const Int &>(v.Abs());
}

Real CGUTIL::RAbs(const Real & v)
{
  return v.Abs();
}

Int CGUTIL::Mod(const Int & v1, const Int & v2)
{
  return static_cast<const Int &>(static_cast<const Real &>(v1).Mod(v2));
}

Int CGUTIL::Rem(const Int & v1, const Int & v2)
{
  return static_cast<const Int &>(static_cast<const Real &>(v1).Rem(v2));
}

Set Permute(const Sequence & l)
{
  return CGUTIL::Permute(l);
}

Sequence Sort(const Set & s)
{
  return CGUTIL::Sort(s);
}

bool IsInteger(const Generic & g)
{
  return CGUTIL::is_int(g);
}

/*
Set GenAllComb(const Sequence & listv)
{
  Set allComb;
  Set res;
  allComb.Insert(Sequence());
  Generic e1;
  for (bool bb = listv.First(e1); bb; bb = listv.Next(e1))
  {
    Set s1 (e1);
    res = Set();
    Generic e2;
    for (bool cc = allComb.First(e2); cc; cc = allComb.Next(e2)) {
      Sequence l1 (e2);
      Generic e3;
      for (bool dd = s1.First(e3); dd; dd = s1.Next(e3)) {
        Sequence l2 (l1);
        l2.ImpAppend(e3);
        res.Insert(l2);
      }
    }
    allComb = res;
  }
  return allComb;
}
*/

Int CGUTIL::GetCurrentTimeInMillis()
{
#ifdef _MSC_VER
#ifdef _MSC_EXTENSIONS
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  GetSystemTimeAsFileTime(&ft);

  tmpres |= ft.dwHighDateTime;
  tmpres <<= 32;
  tmpres |= ft.dwLowDateTime;
  tmpres /= 10;  /*convert into microseconds*/
  tmpres -= DELTA_EPOCH_IN_MICROSECS;
  tmpres /= 1000;  /*convert into miliseconds*/
  Int tm (tmpres);
#else
  timeval tv;
  gettimeofday(&tv, NULL);
  Int tm (((int64_t)(tv.tv_sec)) * 1000 + ((int64_t)(tv.tv_usec))/1000);
#endif // _MSC_VER
  return tm;
}
