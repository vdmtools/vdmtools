/***
*  * WHAT
*  *    Header for translation, initialization and library functions.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/libman.h,v $
*  * VERSION
*  *    $Revision: 1.71 $
*  * DATE
*  *    $Date: 2006/09/23 10:58:19 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __libman_h__
#define __libman_h__

#include "stackeval.h"
#include "compile.h"
#include "evalstate.h"
#include "evaldebug.h"
#ifdef VDMPP
#include "evalsystem.h"
#include "schd.h"
#endif // VDMPP

class LibraryManager
{
public:
  static LibraryManager * libman;

private:
  EvalDebugger * the_EvalDebugger;
  StackEval * the_StackMachine;
  StackCompiler * the_Compiler;
  EvalState * the_State;

#ifdef VDMPP
  SYSTEM * the_System;
  SCHD * the_Scheduler;
#endif // VDMPP

public:
  LibraryManager() {
    this->the_EvalDebugger = NULL;
    this->the_StackMachine = NULL;
    this->the_Compiler = NULL;
    this->the_State = NULL;
#ifdef VDMPP
    this->the_System = NULL;
    this->the_Scheduler = NULL;
#endif // VDMPP
  }
  ~LibraryManager ();

  static void Init();
  static void Finish();

  EvalDebugger & GetDebugger();
  StackCompiler & GetCompiler();
  StackEval & GetStackMachine();
  EvalState & GetState();
#ifdef VDMPP
  SCHD & GetScheduler();
  SYSTEM & GetSystem();
#endif // VDMPP
};

EvalDebugger & theDebugger();
StackCompiler & theCompiler();
StackEval & theStackMachine();
EvalState & theState();
#ifdef VDMPP
SCHD & theScheduler();
SYSTEM & theSystem();
#endif // VDMPP

#endif // __libman_h__
