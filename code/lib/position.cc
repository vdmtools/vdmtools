/***
*  * WHAT
*  *    VDM-SL/VDM++ position information handler
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/position.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 1999/09/07 07:11:20 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    VDM-SL Toolbox / AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: hanne $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "position.h"
#include "astaux.h"
#include "contextinfo.h"

Int POSITION::err_cid = -1;

void POSITION::SetPosition(const Record & TroubleSome)
{
  if (ASTAUX::IsASTRec(TroubleSome))
  {
    POSITION::err_cid = ASTAUX::GetCid(TroubleSome); 
  }
  else
  {
    POSITION::err_cid = NilContextId;
  }
}    

void POSITION::SetPosition(const TYPE_CI_ContextId & cid)
{
  POSITION::err_cid = cid;
}

Tuple POSITION::GetPos()
{
  Tuple gfp (GetCI().GetFilePos(err_cid));
  if (gfp.GetBoolValue(1))
  {
    const TYPE_CI_FileId & fid (gfp.GetField(2));
    const TYPE_CI_TokenPos & stpos (gfp.GetRecord(3));
//    const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
    const TYPE_CI_TokenPos & endpos (gfp.GetRecord(5));
    return mk_(fid,
               stpos.GetInt(pos_CI_TokenPos_abs_uline),
               stpos.GetInt(pos_CI_TokenPos_column),
               endpos.GetInt(pos_CI_TokenPos_abs_uline),
               endpos.GetInt(pos_CI_TokenPos_column));
  }
  else
  {
    return mk_(Int(-10), Int(-1), Int(-1), Int(-1), Int(-1)); 
  }
}
  
TYPE_CI_FileId POSITION::GetErrFile()
{
  Tuple t (GetPos());
  return t.GetInt(1);
}
 
TYPE_CI_Line POSITION::GetErrLine()
{
  Tuple t (GetPos());
  return t.GetInt(2);
}
 
TYPE_CI_Column POSITION::GetErrCol()
{
  Tuple t (GetPos());
  return t.GetInt(3);
}

int POSITION::GetLength()
{
  Tuple t (GetPos());
  int sline = t.GetIntValue(2);
  int scol = t.GetIntValue(3);
  int eline = t.GetIntValue(4);
  int ecol = t.GetIntValue(5);
  return ((sline == eline) ? (ecol - scol) : 1);
}

