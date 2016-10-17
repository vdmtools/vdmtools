/***
*  * WHAT
*  *    Implementation of the stack evaluator error handler.
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/stack_rterr.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:13 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    OS + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    implementation of rterr.vdm
***/

#ifndef _STACK_RTERR_H
#define _STACK_RTERR_H


#include "AS.h"
#include "metaiv.h"

class StackRTErr
{
public:

  // In the specification there is a function call RTERR`Error, which do
  // not take the L"what" parameter, but in the implementation there seems
  // to be invokation of the funcitons which do.
  // 16 Apr. 1999 11:35 -- Jesper K. Pedersen

  void RtError(const wchar_t *Where, 
               Generic exp_v /*SEM`VAL*/, 
               Generic exp_tp /*AS`Type*/, 
               Sequence s) { RunTimeError(Where,wstring(L"UPS"),Record());}
  // This function should do something like the Error() operation of
  // module RTERR, i.e. report the error by using the context id on
  // the top of the context stack.
  //
  // This function must throw an exception!!!
  //

  void RtError(const wchar_t *Where, wstring What,
               Generic exp_v /*SEM`VAL*/, 
               Generic exp_tp /*AS`Type*/, 
               Sequence s) {RunTimeError(Where,wstring(L"UPS"),Record());}
  // This function should do something like the Error() operation of
  // module RTERR, i.e. report the error by using the context id on
  // the top of the context stack.
  //
  // This function must throw an exception!!!V
  //

  void StackError(const wchar_t *, Generic /*SEM`VAL*/, Sequence) {RunTimeError(L"UPS",wstring("UPS"),Record());}
  // This function is used to report errors that has to do with the
  // workings of the stack evaluator. Typically errors that will arise
  // if the evaluation- or call stack is corrupted during
  // evaluation. I.e. this function should _not_ be used to report run
  // time errors that are due to the user...!
  //
  // This function must throw an exception!!!
  //
};




#endif //_STACK_RTERR_H
