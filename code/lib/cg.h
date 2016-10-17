/***
*  * WHAT
*  *    Code generator auxiliary functions
*  * ID
*  *    $Id: cg.h,v 1.16 2005/05/27 00:21:34 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __cg_h__
#define __cg_h__

#include <string>
#include "metaiv.h"
#include "random.h"

class VDMSpecException
{
private:
  Generic val;

public:
  VDMSpecException() : val(Nil()) {};
  VDMSpecException(const Common & v) : val(v) {};
  const Generic & getValue() { return this->val; };
};

class CGUTIL
{
public:
  static Set Permute(const Sequence&);
  static Sequence Sort(const Set&);
  static bool is_real(const Generic&);
  static bool is_int(const Generic&);
  static bool is_nat(const Generic&);
  static bool is_nat1(const Generic&);

  static void PrintPosition();
  static void RunTime(wstring);
  static void NotSupported(wstring);
  static void PushPosInfo(int, int);
  static void PopPosInfo();
  static void PushFile(wstring);
  static void PopFile();
  static void AddRecordTag(const wstring &, const int&);
  static bool ParseVDMValue(const wstring & filename, Generic& res);
  static bool ParseVDMValueString(const wstring & str, Generic& res);

  static Int IAbs(const Real & v);
  static Real RAbs(const Real & v);
  static Int Mod(const Int & v1, const Int & v2);
  static Int Rem(const Int & v1, const Int & v2);

  static Int GetCurrentTimeInMillis();

  template <class T>
  static T Get(const Sequence & l, int64_t i)
  {
    if (l.OutOfRange(i))
      CGUTIL::RunTime(L"Run-Time Error:Illegal index");
    return static_cast<T>(l[i]);
  }

  static void SetRandomSeed(long);
  static Sequence GetRandomSequence(const Int & n);

private:
  static Sequence QS (const Sequence & sq);
  static Stack file_stack, pos_stack;
  static long rseed;
  static Random r;
};

void PrintPosition();
void RunTime(wstring);
void NotSupported(wstring);
void PushPosInfo(int, int);
void PopPosInfo();
void PushFile(wstring);
void PopFile();
void AddRecordTag(const wstring &, const int &);
bool ParseVDMValue(const wstring & filename, Generic& res);
bool ParseVDMValueString(const wstring & str, Generic& res);

// for transition
class CGAUX : public CGUTIL {
};

// OPTIONS
bool cg_OptionGenValues();
bool cg_OptionGenFctOps();
bool cg_OptionGenTpInv();

#endif // __cg_h__
