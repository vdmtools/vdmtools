/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/vdmstdlib.h,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2006/02/17 04:32:21 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    IDERS-
*  * PROJECT
*  *    IDERS Project EP8593
*  * AUTHOR
*  *    $Author: vdmtools $
*  * LOG
*  *    $Log: vdmstdlib.h,v $
*  *    Revision 1.4  2006/02/17 04:32:21  vdmtools
*  *    TYPE_AS_Name
*  *
*  *    Revision 1.3  2005/12/02 06:28:32  vdmtools
*  *    const
*  *
*  *    Revision 1.2  1999/02/18 14:43:31  erik
*  *    Merge of branch with tag etn_981122_speed_interp_bt.
*  *
*  *    Revision 1.1.4.1  1998/11/28 19:07:53  erik
*  *    Use semantic value typedefs svt_* and creater-functions from semvalues.h
*  *
*  *    Revision 1.1  1998/10/27 12:38:38  jeppe
*  *    Initial version of standard MATH lib. Some DL stuff enabled for VDM++,
*  *    mainly Sem2M4 and M42Sem. New function IsVdmStdLib called when
*  *    interpreting 'not yet specified' functions/operations.
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef _vdmstdlib_h_
#define _vdmstdlib_h_

#include "metaiv.h"
#include "semvalues.h"
#include "AS.h"
#include "random.h"

class VdmStdLib
{
public:
  Tuple IsVdmStdLib (const TYPE_AS_Name &, const TYPE_AS_Name &, const SEQ<TYPE_SEM_VAL> &);
  VdmStdLib();
  virtual ~VdmStdLib() {};
  void InitRandom();
  long SetSeed(long seed);
  long GetSeed();
  int GetRandom(int i);

private:
  wstring StripDoubleQuotes (const wstring & s);
  bool IsEq(const TYPE_AS_Name & s, const wstring & q);
  Tuple EvalStdLibMath (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);

  bool filedirecOK (const TYPE_SEM_VAL & d, ios_base::openmode& fd);
  Tuple EvalStdLibIO (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);
  Tuple EvalStdLibCSV (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);

  Tuple EvalStdLibVDMToolsWizard (const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);
  Map GetAstOfModuleNames(const SET< SEQ<Char> > &);
  Map GetTokenInfoOfFileNames(const SET< SEQ<Char> > &);
  Map GetContextNodeInfoOfFileNames(const SET< SEQ<Char> > &, const Generic &);

  Tuple EvalStdLibVDMByteUtil(const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);
  Tuple EvalStdLibVDMUtil(const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);
#ifdef VDMPP
  Tuple EvalStdLibThreadUtil(const TYPE_AS_Name & fnname, const SEQ<TYPE_SEM_VAL> & arg_lv);
#endif // VDMPP

  int CreateMaskByte(int from, int to);
  wchar_t NumToHexChar(int i);

  Random rnd;
  long rnd_seed;
  wstring vdmferror;
};

#endif // _vdmstdlib_h_
