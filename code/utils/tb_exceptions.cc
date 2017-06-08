/***
*  * WHAT
*  *    Define exception class for the Toolbox
*  * FILE
*  *    $Id: tb_exceptions.cc,v 1.1 1999/09/21 13:06:59 erik Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <tb_exceptions.h>

void TB_Exception::EXC_ThrowHandler(int error, int exi)
{
  throw TB_Exception(error);
}
