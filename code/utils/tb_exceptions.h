/***
*  * WHAT
*  *    Define exception class for the Toolbox
*  * FILE
*  *    $Id: tb_exceptions.h,v 1.1 1999/09/21 13:06:58 erik Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __tb_exceptions_h__
#define __tb_exceptions_h__

class TB_Exception 
{
public:
  TB_Exception(int err) : ExType(err) { }

  int GetExType() const { return this->ExType; };

  static void EXC_ThrowHandler(int error, int exi);

private:
  int ExType; 
}; 

#endif // __tb_exceptions_h__
