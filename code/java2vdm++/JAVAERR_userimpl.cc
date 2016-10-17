#include "metaiv.h"
#include "projectval.h"
#include "JAVAERR.h"
#include "contextinfo.h"
#include "tokenlist.h"
#include "tools.h"

Sequence javaErrors;
int numErrs;
int numWarns;

Sequence GetJavaErrors()
{
  return javaErrors;
}

void ClearJavaErrors()
{
  javaErrors = Sequence();
  numErrs = 0;
  numWarns = 0;
}

int GetNumJavaErrs()
{
  return numErrs;
}

int GetNumJavaWarnings()
{
  return numWarns;
}


TYPE_JAVAERR_ErrMsg vdm_JAVAERR_GenErr (const TYPE_CI_ContextId & p_cid, 
                                        const TYPE_JAVAERR_SeverityType &p_SvTp, 
                                        const Int & p_num, 
                                        const type_cL & p_Txt)
{
  wstring l_errStr (L"[");
  l_errStr += p_num.ascii();
  l_errStr += (L"]");

  wstring l_Txt;
  if (p_SvTp == Quote(L"WARNING"))
    l_Txt = L"Warning" + l_errStr;
  else
    l_Txt = L"Error" + l_errStr;

  l_Txt += L" : ";
  wstring p_TxtStr;
  p_Txt.GetString(p_TxtStr);
  l_Txt += p_TxtStr;

  type_cLL descr_l;
  descr_l.ImpAppend(SEQ<Char>(l_Txt));

  Int pos_line, pos_col, fid;
  if (p_cid == NilContextId)
  {
    fid = -1;
    pos_line = 0;
    pos_col = -1;
  }
  else
  {
    Tuple gfp (GetCI().GetFilePos(p_cid));
    const TYPE_CI_FileId & l_fid (gfp.GetInt(2));
    const TYPE_CI_TokenPos & l_astpos (gfp.GetRecord(4));
    fid = l_fid;
    pos_line = l_astpos.get_abs_uline();
    pos_col = l_astpos.get_column();
  }

  TYPE_ProjectTypes_Message ss;
  if (p_SvTp == Quote(L"WARNING"))
  {
    ss = PTAUX::mk_WarnMsg(fid, pos_line, pos_col, descr_l);
    numWarns++;
  }
  else
  {
    ss = PTAUX::mk_ErrMsg(fid, pos_line, pos_col, descr_l);
    numErrs++;
  }

  javaErrors.ImpAppend(ss);
  return (Generic) ss;
}
